/*
 * TCPServerWorkerThread.cpp
 *
 *  Created on: 06.03.2013
 *      Author: Darius Malysiak
 */

#include "TCPServerWorkerThread.h"
#include "Protocols/ClusterLib/ClusterLibFrameAssembler.h"
#include "Protocols/ClusterLib/ClusterLibAssembledFrameHandler.h"

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/eventfd.h>

namespace Lazarus
{

TCPServerWorkerThread::TCPServerWorkerThread(unsigned int max_connections) {

	mp_cn_node_facility = NULL;
	mp_disconnect_callback = NULL;
	mp_connect_callback = NULL;

	m_max_connections = max_connections;
	m_connection_count = 0;

	//event queues
	mp_active_in_events = (struct epoll_event*) calloc(TCPServerWorkerThread::MAX_EPOLL_EVENTS, sizeof(struct epoll_event));
	mp_active_out_events = (struct epoll_event*) calloc(TCPServerWorkerThread::MAX_EPOLL_EVENTS, sizeof(struct epoll_event));

	//epoll system
	m_epoll_in_fd = epoll_create1(0);
	m_epoll_out_fd = epoll_create1(0);

	//create the send/receive buffers which will be shared among all connections of the worker thread
	//set the buffer size to the os maximum
	Lazarus::FastKTuple<unsigned int> readBufferSize = Socket::getOSReadBufferSize();
	Lazarus::FastKTuple<unsigned int> writeBufferSize = Socket::getOSWriteBufferSize();

	/*mp_send_buffer = new Lazarus::Buffer(TCPServerWorkerThread::WT_SEND_BUFFER_SIZE);
	mp_receive_buffer = new Lazarus::Buffer(TCPServerWorkerThread::WT_RECV_BUFFER_SIZE);*/
	mp_send_buffer = new Lazarus::Buffer(writeBufferSize[1]);
	mp_receive_buffer = new Lazarus::Buffer(readBufferSize[1]);

	//create the poker
	m_epoll_read_poker = eventfd(0,EFD_NONBLOCK); //nonblocking poking fd
	if(m_epoll_read_poker == -1)
	{
	  printf("ERROR: could not instantiate TCPServerWorkerThread in-poker\n");
	  exit(1);
	}

	//register the poker within epoll
	struct epoll_event epoll_event;
	epoll_event.events = EPOLLIN | EPOLLET; //edge triggered with data-ready-to-read trigger
	epoll_event.data.fd = m_epoll_read_poker;

	int error = epoll_ctl(m_epoll_in_fd, EPOLL_CTL_ADD,  m_epoll_read_poker, &epoll_event);
	if (error == -1)
	{
		printf("ERROR: could not register poker within in-epoll of worker thread %d\n",m_local_thread_id);
		exit(1);
	}

	m_run_count = 0;
}

TCPServerWorkerThread::~TCPServerWorkerThread()
{
	//graceful shutdown of the thread, due to the poking in preflag this should also kill all connections
	shutdown();

	delete mp_send_buffer;
	delete mp_receive_buffer;

	//shutdown the epoll system
	free(mp_active_in_events);
	free(mp_active_out_events);

	//close the fds
	close(m_epoll_read_poker);
	close(m_epoll_in_fd);
	close(m_epoll_out_fd);

}

void TCPServerWorkerThread::setConnectCallback(ConnectCallback* connect_callback)
{
	mp_connect_callback = connect_callback;
}

void TCPServerWorkerThread::setDisconnectCallback(DisconnectCallback* disconnect_callback)
{
	mp_disconnect_callback = disconnect_callback;
}

void TCPServerWorkerThread::preFlag()
{
	//poke the listener thread
	int bytes = eventfd_write(m_epoll_read_poker,1);

	if(bytes==-1)
	{
		printf("ERROR: could not poke the TCPServerWorkerThread thread %d\n",bytes);
	}
}

void TCPServerWorkerThread::run()
{
	//just exit if a restart is attempted
	if(m_run_count != 0)
	{
		printf("ERROR: can't restart a worker thread\n");
		m_shutdown_flag = true;
		m_thread_self_terminated = true;
		return;
	}

	int bytes=0;
	Lazarus::FastVector4<Lazarus::Buffer*,Lazarus::Buffer*,FrameAssembler*,TCPConnection*>* container = NULL;
	unsigned char* recv_buffer = NULL;
	unsigned int recv_buffer_length = 0;
	FrameAssembler* frame_assembler = NULL;
	TCPConnection* connection = NULL;
	int assembler_result = 0;


	while(m_shutdown_flag == false)
	{
		//printf("worker thread %d reached start of while loop (tid %ld, flag %d)\n",m_local_thread_id,m_system_thread_id,m_shutdown_flag);

		//********************** reading sockets ***************************************

		//wait until at least one socket has received some data
		int waiting_event_count = epoll_wait(m_epoll_in_fd, mp_active_in_events, TCPServerWorkerThread::MAX_EPOLL_EVENTS, -1);

		//process each waiting event
		for(int i=0;i<waiting_event_count;i++)
		{
			//printf("WT event for FD %llu \n",mp_active_in_events[i].data.fd);

			//in case of regular socket
			if(mp_active_in_events[i].data.fd != m_epoll_read_poker)
			{

				//get the buffer/buffer-length and assembler to the current socket fd
				/**
				 * Note that this won't prevent truly parallel worker threads, the read ops can and will be executed
				 * in parallel as the data container uses r/w locks, in this case only a read operation will be executed!
				 */
				try
				{	//printf("WT handles now sd %d\n",mp_active_in_events[i].data.fd);
					container = m_socket_container.getElement( mp_active_in_events[i].data.fd );
				}
				catch(const std::out_of_range &oe)//this should never happen!
				{
					printf("CATASTROPHIC ERROR IN TCP SERVER WORKER THREAD: COULD NOT GET CONNECTION CONTAINER -> SHUTTING DOWN\n");
					m_shutdown_flag = true;
					m_thread_self_terminated = true;
					break;//disregard any pending events as we are shutting down
				}
				recv_buffer = container->m_data1->get_mp_data();

				recv_buffer_length = container->m_data1->get_m_length();

				frame_assembler = container->m_data3;

				connection = container->m_data4;

				//check if socket has been closed
				if ( (mp_active_in_events[i].events & (EPOLLRDHUP | EPOLLHUP)) != 0)
				{
					printf("ERROR: socket is closed\n");

					//(if it exists) remove it from the com facility
					if( frame_assembler->getFrameHandler()->getServingNodeID() != 0 )
					{
						mp_cn_node_facility->removeNode(frame_assembler->getFrameHandler()->getServingNodeID());

						//indicate disconnect
						if(mp_disconnect_callback != NULL)
						{
							mp_disconnect_callback->call(frame_assembler->getFrameHandler()->getServingNodeID());
						}
					}

					//delete the frame assembler
					delete frame_assembler;

					//close the connection
					delete connection;

					//remove the connection from the internal socket container
					m_socket_container.eraseElement(mp_active_in_events[i].data.fd);
					m_connection_count--;

					continue;
				}

				//read all available data from socket
				while(1)
				{
					//just receive a bunch of data
					//bytes = recv(mp_active_in_events[i].data.fd,recv_buffer+received_bytes,recv_buffer_length,0);
					bytes = recv(mp_active_in_events[i].data.fd,recv_buffer,recv_buffer_length,0);

					//reading finished (error?)
					if(bytes == -1)
					{
						if(errno == EAGAIN)
						{
							//printf("read all of it\n");
							break;
						}
						else
						{
							printf("ERROR: could not receive data\n");
							break;
						}
					}

					//received_bytes += bytes;

					//call the assembler for the new data, which lies in the connection buffer
					//assembler_result = frame_assembler->assemble(received_bytes);
					assembler_result = frame_assembler->assemble(bytes);
				}


				//if an error occurred, close the connection
				if(assembler_result == -1)
				{
					printf("ERROR: something went wrong while handling the connection\n");

					//(if it exists) remove it from the com facility
					if( frame_assembler->getFrameHandler()->getServingNodeID() != 0 )
					{
						mp_cn_node_facility->removeNode(frame_assembler->getFrameHandler()->getServingNodeID());

						//indicate disconnect
						if(mp_disconnect_callback != NULL)
						{
							mp_disconnect_callback->call(frame_assembler->getFrameHandler()->getServingNodeID());
						}
					}

					//delete the frame assembler
					delete frame_assembler;

					//close the connection
					delete connection;

					//remove the connection from the internal socket container
					m_socket_container.eraseElement(mp_active_in_events[i].data.fd);
					m_connection_count--;
				}

			}

			//in case of epoll_read_poker: initiate termination
			if(mp_active_in_events->data.fd == m_epoll_read_poker)
			{
				//kill all connections
				killAllConnections();

				m_shutdown_flag = true;
				m_thread_self_terminated = true;
				break;//disregard any pending events as we are shutting down
			}
		}

		//printf("worker thread %d reached end of while loop (tid %ld)\n",m_local_thread_id,m_system_thread_id);

	}

	m_run_count=1;

}

void TCPServerWorkerThread::killAllConnections()
{
	FrameAssembler* frame_assembler = NULL;
	TCPConnection* connection = NULL;
	Lazarus::FastVector4<Lazarus::Buffer*,Lazarus::Buffer*,FrameAssembler*,TCPConnection*>* container = NULL;

	//terminate all connections
	m_socket_container.externalWRLock();

	std::unordered_map< int,Lazarus::FastVector4<Lazarus::Buffer*,Lazarus::Buffer*,Lazarus::FrameAssembler*,Lazarus::TCPConnection*>* >* map =
			m_socket_container.getMap();

	//iterate over all elements
	for( auto it = map->begin(); it != map->end(); ++it )
	{
		container = it->second;
		frame_assembler = container->m_data3;
		connection = container->m_data4;

		//(if it exists) remove it from the com facility
		if( frame_assembler->getFrameHandler()->getServingNodeID() != 0 )
		{
			mp_cn_node_facility->removeNode(frame_assembler->getFrameHandler()->getServingNodeID());
		}

		//delete the frame assembler
		delete frame_assembler;
		//close the connection
		delete connection;
	}

	//remove all entries from the map
	m_socket_container.clearMap();

	//no more connections can be taken in
	m_socket_container.m_lock_list = true;

	m_socket_container.externalWRUnlock();
}


unsigned int TCPServerWorkerThread::get_m_connection_count()
{
	return m_connection_count;
}

void TCPServerWorkerThread::setComFacility(CommunicationFacility* com_facility)
{
	mp_cn_node_facility = com_facility;
}

}
