/*
 * TCPConnection.cpp
 *
 *  Created on: 27.02.2013
 *      Author: Darius Malysiak
 */

#include "TCPConnection.h"
#include "TCPSocket.h"

#include <stdio.h>
#include <errno.h>
#include <iostream>
#include <fcntl.h>
#include <signal.h>

namespace Lazarus
{

TCPConnection::TCPConnection(unsigned int connection_id, unsigned int receive_buffer_size,
		unsigned int send_buffer_size, int epoll_in_events,
		int epoll_out_events): Connection(connection_id,receive_buffer_size,send_buffer_size,epoll_in_events,epoll_out_events)
{
	//ignore the broken pipe signal in order to prevent the program from terminating when a receive or send fails due to closed
	//sender or receiver sockets respectively
	signal(SIGPIPE, SIG_IGN);

	m_haste = true;
}

TCPConnection::TCPConnection(unsigned int connection_id,
		int epoll_in_fd, int epoll_out_fd,
		struct epoll_event* active_in_events,
		struct epoll_event* active_out_events,
		unsigned int receive_buffer_size,
		unsigned int send_buffer_size, int epoll_in_events,
		int epoll_out_events)
				:Connection(connection_id,
						epoll_in_fd,
						epoll_out_fd,
						active_in_events,
						active_out_events,
						receive_buffer_size,
						send_buffer_size,epoll_in_events,epoll_out_events)
{

	//ignore the broken pipe signal in order to prevent the program from terminating when a receive or send fails due to closed
	//sender or receiver sockets respectively
	signal(SIGPIPE, SIG_IGN);

	m_haste = true;
}

TCPConnection::TCPConnection(unsigned int connection_id,
		int epoll_in_fd,
		struct epoll_event* active_in_events,
		unsigned int receive_buffer_size,
		unsigned int send_buffer_size, int epoll_in_events,
		int epoll_out_events)
				:Connection(connection_id,
						epoll_in_fd,
						active_in_events,
						receive_buffer_size,
						send_buffer_size,epoll_in_events,epoll_out_events)
{

	//ignore the broken pipe signal in order to prevent the program from terminating when a receive or send fails due to closed
	//sender or receiver sockets respectively
	signal(SIGPIPE, SIG_IGN);

	m_haste = true;
}


TCPConnection::~TCPConnection()
{

}

int TCPConnection::connect(unsigned int remote_port, const std::string remote_ip, const std::string& local_ip,
			unsigned int local_port, const std::string& local_device)
{
	if(m_external_out_epoll == true || m_external_in_epoll == true)
	{
		printf("ERROR: can not internally connect with an external epoll system\n");
		return -1;
	}

	disconnect();

	int res = 0;
	TCPSocket* socket = NULL;

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
int TCPConnection::send_data(unsigned int data_length,int timeout)
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
		if(m_epoll_out_query_required == true)
		{
			waiting_event_count = epoll_wait(m_epoll_out_fd, mp_active_out_events, m_max_epoll_out_events, timeout);

			//printf("%d events in epoll set\n",waiting_event_count);
			//printf("TIMEOUT %d\n",timeout);

			//in case of the connection socket
			if(mp_active_out_events[0].data.fd == this->mp_socket->get_m_socket_fd())
			{
				//check if a timeout has occurred
				if(waiting_event_count==0)
				{
						return -2;
				}

				//check if socket has been closed
				if ( (mp_active_out_events[0].events & EPOLLRDHUP) != 0 ||  (mp_active_out_events[0].events & EPOLLHUP) != 0)
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


				//printf("send %d bytes with epoll\n",bytes);
				//printf("-------------\n");

				//sending finished (error?)
				if(bytes == -1)
				{printf("ERROR: TCPCONNECTION could not send\n");
					if(errno == EAGAIN)
					{
						printf("------------------------ socket is full --------------- epoll_wait required\n");
						m_epoll_out_query_required = true;
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

				m_epoll_out_query_required = false;
			}
			else
			{
				printf("ERROR: received out-event for different descriptor\n");
			}


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
					m_epoll_out_query_required = true;
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
int TCPConnection::wait_for_data(unsigned int expected_data,int timeout)
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
		int waiting_event_count = epoll_wait(m_epoll_in_fd, mp_active_in_events, m_max_epoll_in_events, timeout);

		//process the waiting events (this loop is required to catch the possibility of overlapping events in case of a shared epoll system)
		for(int i=0;i<waiting_event_count;i++)
		{
			//in case of the listener socket
			if(mp_active_in_events[i].data.fd == this->mp_socket->get_m_socket_fd())
			{
				//check if a timeout has occurred
				if(waiting_event_count == 0)
				{
						printf("WAIT FOR DATA timeout on epoll_wait\n");
						return -2;
				}

				//check if socket has been closed
				if( (mp_active_in_events[0].events & EPOLLRDHUP) != 0 ||  (mp_active_in_events[0].events & EPOLLHUP) != 0)
				{
					printf("ERROR: socket is closed\n");
					m_socket_active = false;
					m_is_active = false;
					return -1;
				}

				while(1)
				{
					bytes = recv(mp_socket->get_m_socket_fd(),buffer+received_bytes,expected_data-received_bytes,0);

					//reading finished (error?)
					if(bytes == -1)
					{
						if(errno == EAGAIN)
						{
							//read all available data on the socket
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
			else
			{
				printf("ERROR: received data from a different FD. Did you try to call receive "
						"from within a frame handler which in turn was invoked by a worker thread???\n");
			}
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
int TCPConnection::receive(int timeout)
{
	//stop in case of inactive socket
	if(m_socket_active == false)
	{
		//printf("ERROR1: socket is closed\n");
		return -1;
	}

	int received_bytes = 0;
	int bytes = 0;
	unsigned char* buffer = mp_receive_buffer->get_mp_data();

	//printf("waiting for data\n");

	//wait until data arrives (event based)
	int waiting_event_count = epoll_wait(m_epoll_in_fd, mp_active_in_events, m_max_epoll_in_events, timeout);

	//check if a timeout has occurred
	if(waiting_event_count == 0)
	{
			return -2;

	}

	//check if socket has been closed
	if( (mp_active_in_events[0].events & EPOLLRDHUP) != 0 || (mp_active_in_events[0].events & EPOLLHUP) != 0)
	{
		//printf("ERROR2: socket is closed\n");
		m_socket_active = false;
		return -1;
	}

	//printf("data arrived\n");

	//process the waiting events (this loop is required to catch the possibility of overlapping events in case of a shared epoll system)
	//this might not be required (??? combined epoll events??) but certainly allows the detection of stolen descriptor events (see else-block)!!
	for(int i=0;i<waiting_event_count;i++)
	{
		unsigned int bytes_to_read_max = mp_receive_buffer->get_m_length();
		//in case of the listener socket
		if(mp_active_in_events[i].data.fd == this->mp_socket->get_m_socket_fd())
		{
			//consume everything
			while(1)
			{
				//printf("receiving bytes\n");

				//just receive a bunch of data
				bytes = recv(mp_socket->get_m_socket_fd(),buffer+received_bytes,bytes_to_read_max,0);
				//printf("received %d bytes\n",bytes);
				//reading finished (error?)
				if(bytes == -1)
				{
					//everything was consumed
					if(errno == EAGAIN)
					{
						//printf("read all of it\n");
						//return received_bytes;
						break;//while(1)
					}
					else
					{
						//printf("ERROR: could not receive data\n");
						m_is_active = false;
						return -1;
					}
				}

				//the connection was closed
				if(bytes==0)
				{
					return -1;
				}

				received_bytes += bytes;

				//this ensures the loop
				received_bytes = received_bytes % bytes_to_read_max;
				//received_bytes = bytes;
			}
		}
		else
		{
			printf("ERROR: received data from a different FD. Did you try to call receive "
					"from within a frame handler which in turn was invoked by a worker thread???\n");
		}
	}

	return received_bytes;

}


void TCPConnection::printConnectionInformation()
{
	printf("Printing connection information\n");
	printf("Socket information:\n");
	mp_socket->printSocketInformation();
}

void TCPConnection::setHaste(bool b)
{
	m_haste = b;
}

}
