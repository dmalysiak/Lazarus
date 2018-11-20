/*
 * Connection.cpp
 *
 *  Created on: 27.02.2013
 *      Author: Darius Malysiak
 */

#include "ConnectionPoll.h"

#include <string.h>
#include <stdlib.h>

namespace Lazarus
{

ConnectionPoll::ConnectionPoll(unsigned int connection_id, unsigned int receive_buffer_size,
		unsigned int send_buffer_size)
{
	m_connection_id = connection_id;
	mp_socket = NULL;

	mp_send_buffer = new Lazarus::Buffer(send_buffer_size);
	mp_receive_buffer = new Lazarus::Buffer(receive_buffer_size);

	m_external_buffer = false;

	//*********** new internal poll system
	memset(&m_poll_in_fds, 0 , sizeof(struct pollfd));
	memset(&m_poll_out_fds, 0 , sizeof(struct pollfd));

	m_socket_active = false;

	m_poll_out_query_required = false;
}

ConnectionPoll::~ConnectionPoll()
{
	//free the buffers
	if(m_external_buffer == false)
	{
		DELETE_NULL_CHECKING(mp_send_buffer);
		DELETE_NULL_CHECKING(mp_receive_buffer);
	}

	//close the socket
	if(mp_socket != NULL)
	{
		mp_socket->closeSocket();
		delete mp_socket;
		mp_socket = NULL;
	}
}

void ConnectionPoll::setExternalBuffer(Lazarus::Buffer* send, Lazarus::Buffer* recv)
{
	if(m_external_buffer == false)
	{
		DELETE_NULL_CHECKING(mp_send_buffer);
		DELETE_NULL_CHECKING(mp_receive_buffer);
	}

	mp_send_buffer = send;
	mp_receive_buffer = recv;

	m_external_buffer = true;
}

bool ConnectionPoll::get_m_socket_active()
{
	return m_socket_active;
}

void ConnectionPoll::printConnectionInformation()
{
	printf("not implemented\n");
	return;
}


Socket* ConnectionPoll::get_mp_socket()
{
	return mp_socket;
}

void ConnectionPoll::set_mp_socket(Socket* socket)
{
	//close any existing socket
	if(mp_socket != NULL)
	{
		mp_socket->closeSocket();
		delete mp_socket;
	}

	mp_socket = socket;

	//set the socket to non blocking
	mp_socket->enableNonblocking();

	//register the socket with the poll system
	m_poll_out_fds.fd = this->mp_socket->get_m_socket_fd();
	m_poll_out_fds.events = POLLOUT;

	//register the socket with the poll system
	m_poll_in_fds.fd = this->mp_socket->get_m_socket_fd();
	m_poll_in_fds.events = POLLIN;

	m_socket_active = true;

	//check whether the receive/send buffers are large enough
	if(mp_socket->get_m_socket_fd() != -1)
	{
		Lazarus::FastKTuple<unsigned int> size = mp_socket->getCurrentSocketBufferSizes();

		if(size[0] > mp_receive_buffer->get_m_length())
		{
			printf("WARNING: the sockets OS receive buffer is larger than the objects receive buffer\n");
			if(m_external_buffer == false)//in case of internal buffers: increase the size
			{
				delete mp_receive_buffer;
				mp_receive_buffer = new Lazarus::Buffer(size[0]);
			}
		}

		if(size[1] > mp_send_buffer->get_m_length())
		{
			printf("WARNING: the sockets OS send buffer is larger than the objects send buffer\n");
			if(m_external_buffer == false)//in case of internal buffers: increase the size
			{
				delete mp_send_buffer;
				mp_send_buffer = new Lazarus::Buffer(size[1]);
			}
		}
	}
	else
	{
		printf("ERROR: something went wrong setting the socket\n");
	}

}



}
