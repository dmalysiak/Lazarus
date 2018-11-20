/*
 * P2PServer.cpp
 *
 *  Created on: 20.03.2013
 *      Author: Darius Malysiak
 */

#include "P2PServer.h"
#include "../../Toolbox.h"

namespace Lazarus {

P2PServer::P2PServer(const std::string& local_address, unsigned int local_port, const std::string& local_device)
{
	mp_client_connection = NULL;

	int res = 0;

	if(local_address.size() != 0 && local_port != 0)
	{
		mp_socket = new TCPSocket(local_address, local_port);

		if(local_device.size() != 0)
		{
			res = mp_socket->bindSocketToDevice(local_device);
		}
		else
		{
			res = mp_socket->bindSocket();
		}

	}
	else
	{
		mp_socket = new TCPSocket();
	}

	if(res == 0)
		m_ready = true;
	else
		m_ready = false;

	//register within internal poll system
	m_poll_in_fds.fd = this->mp_socket->get_m_socket_fd();
	m_poll_in_fds.events = POLLIN;

	m_local_port = local_port;
	m_local_ip = local_address;

	mp_frame = NULL;
}

P2PServer::~P2PServer()
{
	delete mp_socket;//delete the >listener< socket
	DELETE_NULL_CHECKING(mp_client_connection);
	DELETE_NULL_CHECKING(mp_frame);
}

TCPConnectionPoll* P2PServer::get_mp_client_connection()
{
	return mp_client_connection;
}

int P2PServer::waitForClient(int timeout)
{
	if(m_ready == false)
	{
		INFO("socket not bound")
		return -1;
	}

	//delete any existing connection
	if(mp_client_connection != NULL)
	{
		delete mp_client_connection;
		mp_client_connection = NULL;
	}

	DELETE_NULL_CHECKING(mp_frame);

	int socket_fd;
	struct sockaddr_in connection_data;
	unsigned int size = sizeof(sockaddr_in);

	//listen for new connections
	if(listen(mp_socket->get_m_socket_fd(), 1) == -1)
	{
		printf("ERROR: could not setup a listener socket\n");
		Lazarus::Toolbox::decodeErrno(errno);
		exit(1);
	}

	//wait for a connection
	printf("waiting for connections\n");

	//wait until a connection arrives (event based)
	int waiting_event_count = poll(&m_poll_in_fds, 1,timeout);

	//timeout
	if(waiting_event_count == 0)
	{
		printf("ERROR: timeout\n");
		return -1;
	}

	//error
	if(m_poll_in_fds.revents != POLLIN)
	{
		printf("ERROR: expected a POLLIN\n");
		Lazarus::Toolbox::decodeErrno(errno);
		return -2;
	}

	//error
	if(waiting_event_count == -1)
	{
		printf("ERROR: fatal socket problem\n");
		Lazarus::Toolbox::decodeErrno(errno);
		return -2;
	}

	//check if socket has been closed
	if( m_poll_in_fds.revents == POLLRDHUP || m_poll_in_fds.revents == POLLHUP)
	{
		printf("ERROR: listener socket is closed\n");
		Lazarus::Toolbox::decodeErrno(errno);
		return -2;
	}

	socket_fd = accept(mp_socket->get_m_socket_fd(), (struct sockaddr*) &connection_data, &size);

	if(socket_fd == -1)
	{
		printf("stopping listening: error on accept or socket closed\n");
		Lazarus::Toolbox::decodeErrno(errno);
		return -2;
	}

	printf("connection accepted\n");
	//create a (non-autonomous) connection from socket data
	//TCPConnection* connection = new TCPConnection(0,false);
	TCPSocket* tcpsocket = new TCPSocket(socket_fd);

	//fill the socket information into the socket object
	char* c_strbuffer = new char[500];
	inet_ntop(AF_INET, &connection_data.sin_addr, c_strbuffer, 500);

	tcpsocket->m_address_type = Socket::ADDRESS_TYPE_IPV4;
	tcpsocket->m_local_address = m_local_ip;
	tcpsocket->m_local_port = m_local_port;
	//tcpsocket->m_socket_fd = socket_fd;
	tcpsocket->m_remote_address = std::string(c_strbuffer);
	tcpsocket->m_remote_port = ntohs(connection_data.sin_port);

	//free the cstring buffer
	delete[] c_strbuffer;

	mp_client_connection = new TCPConnectionPoll(0);
	mp_client_connection->set_mp_socket(tcpsocket);

	return 0;

}

}

