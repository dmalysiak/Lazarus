/*
 * ClusterLibServer.cpp
 *
 *  Created on: 20.03.2013
 *      Author: Darius Malysiak
 */

#include "ClusterLibServer.h"

#include "../TCPServer.h"
#include "../../Threading/FunctionTask.h"
#include "../../Threading/TaskHelper.h"
#include "../../Toolbox.h"

#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/eventfd.h>
#include <math.h>
#include <errno.h>

namespace Lazarus
{

ClusterLibServer::ClusterLibServer(std::string local_address, unsigned int local_port, unsigned int max_connections,
		unsigned int worker_threads) : TCPServer(local_address,local_port,max_connections,worker_threads){

	m_is_active = false;

}



ClusterLibServer::~ClusterLibServer()
{
}



long int ClusterLibServer::findNextBestWorkerThread()
{
	unsigned int thread_index = 0;
	unsigned int current_candidate = m_max_connections;
	bool found = false;

	for(unsigned int i=0;i<m_worker_thread_count;i++)
	{
		//if a worker thread handles fewer than current_candidate connections
		if(mp_worker_threads[i]->get_m_connection_count() < current_candidate)
		{
			current_candidate = mp_worker_threads[i]->get_m_connection_count();
			thread_index = i;
			found = true;
		}
	}

	if(found == true)
	{
		printf("found a worker thread (%d) which can handle the connection\n",thread_index);
		return thread_index;
	}
	else
	{
		printf("found no worker thread which can handle the incoming connection\n");
	}

	return -1;
}

bool ClusterLibServer::isActive()
{
	if(m_is_active == true)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ClusterLibServer::listener_method()
{
	int socket_fd;
	struct sockaddr_in connection_data;
	unsigned int size = sizeof(sockaddr_in);

	m_is_active = true;

	//listen for new connections
	if(listen(mp_socket->get_m_socket_fd(),ClusterLibServer::LISTENER_QUEUE_SIZE)==-1)
	{
		printf("ERROR: could not setup a listener socket\n");
		exit(1);
	}

	while(m_shutdown_flag == false)
	{
		//printf("waiting for connections 000\n");

		//wait until connections arrive (event based)
		int waiting_event_count = epoll_wait(m_epoll_in_fd, mp_active_in_events, ClusterLibServer::MAX_EPOLL_EVENTS, -1);

		//process each waiting event
		for(int i=0;i<waiting_event_count;i++)
		{
			//in case of the listener socket
			if(mp_active_in_events[i].data.fd == this->mp_socket->get_m_socket_fd())
			{
				//check if socket has been closed
				if ( (mp_active_in_events[i].events & (EPOLLRDHUP | EPOLLHUP)) != 0)
				{
					printf("ERROR: listener socket is closed - shutting down server\n");
					m_shutdown_flag = true;
					m_thread_self_terminated = true;
					m_is_active = false;
					return;
				}

				/*THIS IS IMPORTANT: if size wasn't initialized to sizeof(connection_data), it might happen
				 * that the socket gets the IP 255.127.0.0*/
				size = sizeof(connection_data);
				socket_fd = accept(mp_socket->get_m_socket_fd(),(struct sockaddr*) &connection_data,&size);

				if(socket_fd == -1)
				{
					printf("stopping listening: error on accept or socket closed\n");
					continue;
				}

				//printf("connection accepted with FD %llu\n",socket_fd);

				//create a (non-autonomous) connection from socket data
				//TCPConnection* connection = new TCPConnection(0,false);
				TCPSocket* tcpsocket = new TCPSocket(socket_fd);

				//fill the socket information into the socket object
				char* c_strbuffer = new char[ 500 ];
				inet_ntop(AF_INET,&connection_data.sin_addr,c_strbuffer,500);

				tcpsocket->m_address_type = Socket::ADDRESS_TYPE_IPV4;
				tcpsocket->m_local_address = m_local_address;
				tcpsocket->m_local_port = m_local_port;
				//tcpsocket->m_socket_fd = socket_fd;
				tcpsocket->m_remote_address = std::string(c_strbuffer);
				tcpsocket->m_remote_port = ntohs(connection_data.sin_port);

				//free the cstring buffer
				delete[] c_strbuffer;

				//**********  hand the connection over to a worker thread ***********
				long int t_index = findNextBestWorkerThread();
				if(t_index != -1)
				{
					mp_worker_threads[t_index]->addConnection(tcpsocket);
				}
				else
				{
					tcpsocket->closeSocket();
					delete tcpsocket;
				}

			}
			//in case of epoll_listener_read_poker
			if(mp_active_in_events[i].data.fd == m_epoll_read_poker)
			{
				//printf("Listener poked\n");
				//close the main socket
				mp_socket->closeSocket();

				m_shutdown_flag = true;
				m_thread_self_terminated = true;
				m_is_active = false;
				return;
			}
		}
		//in case of a return value less than 0 from epoll_wait (happens due to signal handlers which interrupt epoll_wait, thus errno equals EINTR)
		//pthread_mutex_unlock(&m_mutex);
	}
}

}
