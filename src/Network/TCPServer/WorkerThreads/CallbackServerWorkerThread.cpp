/*
 * CallbackServerWorkerThread.cpp
 *
 *  Created on: 12.06.2013
 *      Author: darius
 */

#include "CallbackServerWorkerThread.h"
#include "../../Protocols/ClusterLib/CallbackServerFrameHandler.h"
#include "../../Protocols/ClusterLib/ClusterLibFrameAssembler.h"

namespace Lazarus
{

CallbackServerWorkerThread::CallbackServerWorkerThread(unsigned int max_connections,CallbackServerComFacility* com_facility,
		GenericCallback* callback)
	: TCPServerWorkerThread(max_connections) {

	mp_callback_facility = com_facility;

	mp_callback = callback;

}

CallbackServerWorkerThread::~CallbackServerWorkerThread() {
	// TODO Auto-generated destructor stub
}

void CallbackServerWorkerThread::addConnection(TCPSocket* tcpsocket)
{
	if(m_connection_count > m_max_connections)
	{
		printf("ERROR: maximal amount of connections already present in worker\n");
		tcpsocket->closeConnection();
		DELETE_NULL_CHECKING(tcpsocket);
	}

	m_socket_container.externalWRLock();

	//abort if locked
	/*required only if another thread calls TCPServerWorkerThread::killAllConnections(),
	 * in order to prevent new connections from being accepted*/
	if(m_socket_container.m_lock_list == true)
	{
		tcpsocket->closeConnection();
		DELETE_NULL_CHECKING(tcpsocket);
		m_socket_container.externalWRUnlock();
		return;
	}

	//register socket in local epoll system
	//we don't need a special mutex for the following operations, as epoll is thread safe
	struct epoll_event epoll_event;
	epoll_event.events = EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLERR; //edge triggered with data-ready-to-read trigger
	epoll_event.data.fd = tcpsocket->get_m_socket_fd();

	int error = epoll_ctl(m_epoll_in_fd, EPOLL_CTL_ADD,  tcpsocket->get_m_socket_fd(), &epoll_event);
	if (error == -1)
	{
		printf("ERROR: could not register poker within in-epoll of worker thread %d\n",m_local_thread_id);
		exit(1);
	}

	/*epoll_event.events = EPOLLOUT | EPOLLET | EPOLLRDHUP | EPOLLERR; //edge triggered with data-ready-to-read trigger
	epoll_event.data.fd = connection->get_mp_socket()->get_m_socket_fd();

	error = epoll_ctl(m_epoll_out_fd, EPOLL_CTL_ADD,  connection->get_mp_socket()->get_m_socket_fd(), &epoll_event);
	if (error == -1)
	{
		printf("ERROR: could not register poker within out-epoll of worker thread %d\n",m_local_thread_id);
		exit(1);
	}*/

	//create a tcp connection
	//TCPConnection* connection = new TCPConnection(0,m_epoll_in_fd,m_epoll_out_fd,mp_active_in_events,mp_active_out_events);
	TCPConnection* connection = new TCPConnection(0,m_epoll_in_fd,mp_active_in_events, DEFAULT_RECEIVE_BUFFER_SIZE,
				DEFAULT_SEND_BUFFER_SIZE, CallbackServerWorkerThread::MAX_EPOLL_EVENTS,2);

	//assign the socket the new connection
	connection->set_mp_socket(tcpsocket);

	//assign the shared buffer
	connection->setExternalBuffer(mp_send_buffer, mp_receive_buffer);

	//register the connection
	//mp_c_manager->registerConnection(connection);
	m_connection_count++;

	//create the frame assembler
	CallbackServerFrameHandler* frame_handler = new CallbackServerFrameHandler(connection,mp_callback_facility,mp_callback,this);
	ClusterLibFrameAssembler* assembler = new ClusterLibFrameAssembler(connection,frame_handler);


	//register the buffers
	Lazarus::FastVector4<Lazarus::Buffer*,Lazarus::Buffer*,FrameAssembler*,TCPConnection*>* container = new Lazarus::FastVector4<Lazarus::Buffer*,Lazarus::Buffer*,FrameAssembler*,TCPConnection*>();

	container->m_data1=connection->get_mp_receive_buffer();
	container->m_data2=connection->get_mp_send_buffer();
	container->m_data3=assembler;
	container->m_data4=connection;

	//insert the container into the hash list
	//m_socket_container[connection->get_mp_socket()->get_m_socket_fd()] = container;
	//m_socket_container.insertElement(connection->get_mp_socket()->get_m_socket_fd(), container );

	(*(m_socket_container.getMap()))[connection->get_mp_socket()->get_m_socket_fd()] = container;

	m_socket_container.externalWRUnlock();

	//send a greeting ping
	/*unsigned int type_size = sizeof(CallbackServer::CALLBACK_SERVER_REQUEST_TYPE);
	ClusterLibFrame frame(connection);
	CallbackServer::CALLBACK_SERVER_REQUEST_TYPE type =
			CallbackServer::CALLBACK_SERVER_REQUEST_TYPE_PING;
	Lazarus::Buffer buffer(type_size);
	memcpy(buffer.get_mp_data(),&type,type_size);
	frame.sendFrame(&buffer);*/

}

}
