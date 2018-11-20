/*
 * TCPConnection.cpp
 *
 *  Created on: 27.02.2013
 *      Author: Darius Malysiak
 */

#include "TCPConnectionPoll.h"
#include "TCPSocket.h"

#include <stdio.h>
#include <errno.h>
#include <iostream>
#include <fcntl.h>
#include <signal.h>

namespace Lazarus
{

TCPConnectionPoll::TCPConnectionPoll(unsigned int connection_id, unsigned int receive_buffer_size,
		unsigned int send_buffer_size):
				ConnectionPoll(connection_id,receive_buffer_size,send_buffer_size)
{
	//ignore the broken pipe signal in order to prevent the program from terminating when a receive or send fails due to closed
	//sender or receiver sockets respectively
	signal(SIGPIPE, SIG_IGN);
	m_haste = true;
}


TCPConnectionPoll::~TCPConnectionPoll()
{

}


int TCPConnectionPoll::connect(unsigned int remote_port, const std::string remote_ip, const std::string& local_ip,
			unsigned int local_port, const std::string& local_device)
{
	int res = 0;
	TCPSocket* socket = NULL;

	disconnect();

	//determine binding address
	socket = new TCPSocket(local_ip, local_port);


	//determine binding device
	if(local_device.size() != 0)
	{
		res = socket->bindSocketToDevice(local_device);
		if(res < 0)
		{
			printf("ERROR: could not fully bind TCP socket\n");
			delete socket;
			return -1;
		}
	}
	else
	{
		res = socket->bindSocket();
		if(res < 0)
		{
			printf("ERROR: could not bind TCP socket\n");
			delete socket;
			return -1;
		}
	}

	res = socket->connectIPv4(remote_ip, remote_port);

	if(res < 0)
	{
		printf("ERROR: could not connect TCP socket\n");
		return -1;
	}

	//assign the socket internally
	this->set_mp_socket(socket);

	m_is_active = true;

	return 0;
}

/**
 * This method will attempt to send the first 'data_length' bytes within the current buffer.
 * Returns -2 in case of timeouts, -1 in case of socket errors, otherwise 0.
 * */
int TCPConnectionPoll::send_data(unsigned int data_length,int timeout)
{
	//stop in case of inactive socket
	if(m_socket_active == false)
	{
		printf("ERROR: socket is closed\n");
		return -1;
	}

	//send data to the connected client
	unsigned int remaining_bytes = data_length;
	unsigned int offset = 0;
	int waiting_event_count = 0;
	unsigned char* buffer = mp_send_buffer->get_mp_data();
	int bytes = 0;
	//printf("going to send %u bytes\n",data_length);
	while(remaining_bytes > 0)
	{
		//printf("trying to send %d bytes\n",remaining_bytes);

		//wait until data can be send (event based)
		if(m_poll_out_query_required == true)
		{
			waiting_event_count = poll(&m_poll_out_fds, 1, timeout);

			//check if a timeout has occurred
			if(waiting_event_count==0)
			{
				//printf("SEND DATA timeout on epoll_wait\n");
				return -2;
			}

			//check if socket has been closed
			if((m_poll_out_fds.revents & POLLRDHUP) != 0 || (m_poll_out_fds.revents & POLLHUP) != 0 )
			{
				printf("socket is closed\n");
				m_socket_active = false;
				m_is_active = false;
				return -1;
			}

			if( (m_poll_out_fds.revents & POLLOUT) == POLLOUT)
			{
				bytes = send(mp_socket->get_m_socket_fd(),buffer+offset,remaining_bytes,0);

				if(m_haste == true)
				{
					//induce a flush of all queued data
					((TCPSocket*)mp_socket)->setCork(false);
					((TCPSocket*)mp_socket)->setNoDelay(true);
				}
			}
			else
			{
				printf("ERROR: expected POLLOUT\n");
				return -1;
			}

			//printf("send %d bytes with epoll\n",bytes);
			//printf("-------------\n");

			//sending finished (error?)
			if(bytes == -1)
			{printf("ERROR: TCPCONNECTION could not send\n");
				if(errno == EAGAIN)
				{
					printf("------------------------ socket is full --------------- poll required\n");
					m_poll_out_query_required = true;
					continue;
				}
				else
				{
					printf("ERROR: could not send data - FATAL ERROR\n");
					m_is_active = false;
					return -1;
				}
			}

			remaining_bytes = remaining_bytes - bytes;
			offset += bytes;

			m_poll_out_query_required = false;

		}
		//if no waiting is required, i.e. socket is still available for writing
		else
		{
			//check if socket has been closed
			if (fcntl(this->mp_socket->get_m_socket_fd(), F_GETFD) == -1)
			{
				printf("socket is closed\n");
				m_socket_active = false;
				m_is_active = false;
				return -1;
			}


			bytes = send(mp_socket->get_m_socket_fd(),buffer+offset,remaining_bytes,0);

			if(m_haste == true)
			{
				//induce a flush of all queued data
				((TCPSocket*)mp_socket)->setCork(false);
				((TCPSocket*)mp_socket)->setNoDelay(true);
			}

			//printf("send %d bytes without epoll\n",bytes);
			//printf("-------------\n");

			//sending finished (error?)
			if(bytes == -1)
			{
				//printf("ERROR: TCPCONNECTION could not send\n");
				if(errno == EAGAIN || errno ==EAGAIN || errno ==EWOULDBLOCK)
				{
					//printf("socket is full\n");
					m_poll_out_query_required = true;
					continue;
				}
				else
				{
					printf("ERROR: could not send data - FATAL ERROR\n");
					m_is_active = false;
					return -1;
				}
			}

			remaining_bytes = remaining_bytes - bytes;
			offset += bytes;
		}
	}

	return 0;


}

/**
 * Keep in mind that this method can only receive up to connection_buffer_size bytes.
 * It will iterate over the socket until 'expected_data' bytes have been read.
 * Returns -2 in case of timeouts, -1 in case of socket errors, otherwise 0.
 * */
int TCPConnectionPoll::wait_for_data(unsigned int expected_data,int timeout)
{
	//stop in case of inactive socket
	if(m_socket_active == false)
	{
		printf("ERROR: socket is closed\n");
		return -1;
	}

	//stop in case of too much data
	if(expected_data > mp_receive_buffer->get_m_length())
	{
		printf("ERROR: receive buffer too small for desired data amount\n");
		return -1;
	}

	//wait until expected amount has been received
	unsigned int received_bytes = 0;
	int bytes = 0;
	unsigned char* buffer = mp_receive_buffer->get_mp_data();

	while(received_bytes < expected_data)
	{
		//wait until data arrives (event based)
		int waiting_event_count = poll(&m_poll_in_fds, 1, timeout);

		//check if a timeout has occurred
		//if(errno == EINTR)
		if(waiting_event_count == 0)
		{
			printf("WAIT FOR DATA timeout on poll\n");
			return -2;
		}

		//check if socket has been closed
		if((m_poll_in_fds.revents & POLLRDHUP) != 0 || (m_poll_in_fds.revents & POLLHUP) != 0 )
		{
			printf("ERROR: socket is closed\n");
			m_socket_active = false;
			m_is_active = false;
			return -1;
		}

		if( (m_poll_in_fds.revents & POLLIN) != POLLIN)
		{
			printf("ERROR: expected POLLIN\n");
			return -1;
		}

		while(1)
		{
			bytes = recv(mp_socket->get_m_socket_fd(),buffer+received_bytes,expected_data-received_bytes,0);

			//reading finished (error?)
			if(bytes == -1)
			{
				if(errno == EWOULDBLOCK)
				{
					//read all of it
					break;
				}
				else
				{
					printf("ERROR: could not receive data\n");
					m_is_active = false;
					return -1;
				}
			}

			received_bytes = received_bytes + bytes;
		}
	}

	return 0;

}

/**
 * Keep in mind that this method can only receive up to connection_buffer_size bytes.
 * It will iterate over the socket until it is empty, every iteration will overwrite
 * the data within the connection_buffer from the previous iteration.
 * Returns -2 in case of timeouts, -1 in case of socket errors, otherwise the number of received bytes.
 * */
int TCPConnectionPoll::receive(int timeout)
{
	//stop in case of inactive socket
	if(m_socket_active == false)
	{
		//printf("ERROR: socket is closed\n");
		return -1;
	}

	int bytes = 0;
	unsigned char* buffer = mp_receive_buffer->get_mp_data();

	//printf("waiting for data\n");

	//wait until data arrives (event based)
	int waiting_event_count = poll(&m_poll_in_fds, 1, timeout);

	//printf("receive revents %d for %d events\n",m_poll_in_fds.revents,waiting_event_count);

	//check if socket has been closed
	if((m_poll_in_fds.revents & POLLRDHUP) == POLLRDHUP || (m_poll_in_fds.revents & POLLHUP) == POLLHUP )
	{
		//printf("ERROR: socket is closed\n");
		m_socket_active = false;
		m_is_active = false;
		return -1;
	}

	if( (m_poll_in_fds.revents & POLLIN) != POLLIN)
	{
		printf("ERROR: expected POLLIN\n");
		return -1;
	}

	//check if a timeout has occurred
	//if(errno == EINTR)
	if(waiting_event_count == 0)
	{
		//printf("RECEIVE timeout on epoll_wait\n");
		return -2;

	}

	//printf("receiving bytes\n");

	//just receive a bunch of data
	bytes = recv(mp_socket->get_m_socket_fd(),buffer,mp_receive_buffer->get_m_length(),0);
	//printf("received %d bytes\n",bytes);
	//reading finished (error?)
	if(bytes == -1)
	{
		//no data available
		if(errno == EWOULDBLOCK)
		{
			return bytes;
		}
		else
		{
			//printf("ERROR: could not receive data\n");
			m_is_active = false;
			return -1;
		}
	}

	//if socket was shut down
	if(bytes == 0)
	{
		return -1;
	}

	return bytes;

}


void TCPConnectionPoll::printConnectionInformation()
{
	printf("Printing connection information\n");
	printf("Socket information:\n");
	mp_socket->printSocketInformation();
}

void TCPConnectionPoll::setHaste(bool b)
{
	m_haste = b;
}

}
