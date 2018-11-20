/*
 * TCPServer.cpp
 *
 *  Created on: 28.02.2013
 *      Author: Darius Malysiak
 */

#include "TCPServer.h"
#include "ConnectionManager.h"
#include "../Toolbox.h"

#include <sys/eventfd.h>
#include <math.h>

namespace Lazarus
{

TCPServer::TCPServer(std::string local_address, unsigned int local_port, unsigned int max_connections,
		unsigned int worker_threads) {

	printf("going to start tcp server with %u worker threads and a maximum of %u connections\n",worker_threads,max_connections);
	//sanity check
	if(worker_threads > max_connections)
	{
		printf("ERROR: more worker threads %u requested than maximal amount of connections %u\n",worker_threads,max_connections);
		exit(1);
	}
	if( fmod(max_connections,worker_threads) > 0.0 )
	{
		printf("ERROR: maximal amount of connections is not divisible by amount of worker threads \n");
		exit(1);
	}

	mp_disconnect_callback = NULL;
	mp_connect_callback = NULL;

	//************************** local vars

	m_run_count = 0;
	m_local_port = local_port;
	m_local_address = local_address;
	m_max_connections = max_connections;
	m_current_connections = 0;
	mp_cn_node_facility = NULL;

	//open the socket
	mp_socket = ConnectionManager::openSocketTCPIPv4(local_address,local_port);

	//set the socket to non blocking
	mp_socket->enableNonblocking();

	//DEBUG
	mp_socket->printSocketInformation();

	//worker threads
	mp_worker_threads = new TCPServerWorkerThread*[worker_threads];
	m_worker_thread_count = worker_threads;

	//************************** epoll system
	//listener
	m_epoll_in_fd = epoll_create1(0);
	mp_active_in_events  = (struct epoll_event*) calloc(TCPServer::MAX_EPOLL_EVENTS, sizeof(struct epoll_event));


	//create a read poker
	m_epoll_read_poker = eventfd(0,EFD_NONBLOCK); //nonblocking poking fd
	if(m_epoll_read_poker == -1)
	{
	  printf("ERROR: could not instantiate ClusterLibServer in-poker.\n");
	  exit(1);
	}

	//register the listener socket with epoll
	struct epoll_event epoll_event;
	epoll_event.events = EPOLLIN | EPOLLET; //edge triggered with data-ready-to-read trigger
	epoll_event.data.fd = mp_socket->get_m_socket_fd();

	int error = epoll_ctl(m_epoll_in_fd, EPOLL_CTL_ADD, mp_socket->get_m_socket_fd(), &epoll_event);
	if (error == -1)
	{
		printf("ERROR: could not register listener within in-epoll 0\n");
		exit(1);
	}

	//register the poker for the listener thread
	epoll_event.events = EPOLLIN | EPOLLET; //edge triggered with data-ready-to-read trigger
	epoll_event.data.fd = m_epoll_read_poker;

	error = epoll_ctl(m_epoll_in_fd, EPOLL_CTL_ADD,  m_epoll_read_poker, &epoll_event);
	if (error == -1)
	{
		printf("ERROR: could not register listener ClusterLibServer poker within in-epoll 0\n");
		exit(1);
	}

}

TCPServer::~TCPServer()
{
	//graceful shutdown of the eventually running thread
	shutdown();

	//kill all workers and thus all connections (as each worker kills his connections on destruction)
	for(unsigned int i=0;i<m_worker_thread_count;++i)
	{
		//shutdown the worker thread gracefully
		mp_worker_threads[i]->shutdown();

		delete mp_worker_threads[i];
	}

	delete[] mp_worker_threads;

	//since no worker thread exists anymore which could use the com facility (if it even exists), we can safely delete it
	DELETE_NULL_CHECKING(mp_cn_node_facility);

	//shutdown the epoll system
	free(mp_active_in_events);

	//close all pokers
	close(m_epoll_in_fd);
	close(m_epoll_read_poker);

	DELETE_NULL_CHECKING(mp_socket);
}

void TCPServer::setConnectCallback(ConnectCallback* connect_callback)
{
	mp_connect_callback = connect_callback;
}

void TCPServer::setDisconnectCallback(DisconnectCallback* disconnect_callback)
{
	mp_disconnect_callback = disconnect_callback;
}

void TCPServer::run()
{
	//in case of restart: abort
	if(m_run_count != 0)
	{
		printf("ERROR: can not restart TCP server\n");
	}

	listener_method();

	m_run_count = 1;
}

void TCPServer::startServer()
{
	start();
}


void TCPServer::stopServer()
{
	//graceful shutdown of the eventually running thread
	shutdown();
}

void TCPServer::preFlag()
{
	//poke the listener thread
	int bytes = eventfd_write(m_epoll_read_poker,1);

	if(bytes==-1)
	{
		printf("ERROR: could not poke the TCPServer thread %d\n",bytes);
	}
}

/**
* Returns the servers current IP address.
* */
std::string TCPServer::getAddress()
{
	return this->m_local_address;
}

/**
* Returns the servers current port.
* */
unsigned int TCPServer::getPort()
{
	return this->m_local_port;
}

}
