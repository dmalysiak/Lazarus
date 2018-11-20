/*
 * Connection.cpp
 *
 *  Created on: 27.02.2013
 *      Author: Darius Malysiak
 */

#include "Connection.h"

#include <string.h>
#include <stdlib.h>

namespace Lazarus
{

Connection::Connection(unsigned int connection_id,unsigned int receive_buffer_size,
		unsigned int send_buffer_size, int epoll_in_events,
		int epoll_out_events)
{
	m_connection_id = connection_id;
	mp_socket = NULL;

	mp_send_buffer = new Lazarus::Buffer(send_buffer_size);
	mp_receive_buffer = new Lazarus::Buffer(receive_buffer_size);
	m_external_out_epoll = false;
	m_external_in_epoll = false;
    m_epoll_out_query_required = false;

    m_max_epoll_in_events = epoll_in_events;
	m_max_epoll_out_events = epoll_out_events;

	m_external_buffer = false;

	//*********** new internal epoll system
	mp_active_in_events  = (struct epoll_event*) calloc(epoll_in_events, sizeof(struct epoll_event));
	m_epoll_in_fd = epoll_create1(0);
	if(m_epoll_in_fd == -1)
	{
	  printf("ERROR: could not instantiate in-epoll\n");
	  exit(1);
	}

	//create an epoll out-instance
	mp_active_out_events  = (struct epoll_event*) calloc(epoll_out_events, sizeof(struct epoll_event));
	m_epoll_out_fd = epoll_create1(0);
	if(m_epoll_out_fd == -1)
	{
	  printf("ERROR: could not instantiate out-epoll\n");
	  exit(1);
	}


	m_socket_active = false;
}

Connection::Connection(unsigned int connection_id,
			int epoll_in_fd,
			struct epoll_event* active_in_events,
			unsigned int receive_buffer_size,
			unsigned int send_buffer_size, int epoll_in_events,
			int epoll_out_events)
{
	m_connection_id = connection_id;
	mp_socket = NULL;

	mp_send_buffer = new Lazarus::Buffer(send_buffer_size);
	mp_receive_buffer = new Lazarus::Buffer(receive_buffer_size);
    m_epoll_out_query_required = true;

    //*********** new internal epoll system
	//create an epoll out-instance
	mp_active_out_events  = (struct epoll_event*) calloc(epoll_out_events, sizeof(struct epoll_event));
	m_epoll_out_fd = epoll_create1(0);
	if(m_epoll_out_fd == -1)
	{
	  printf("ERROR: could not instantiate out-epoll\n");
	  exit(1);
	}

	//*********** external epol system
	m_external_out_epoll = false;
	m_external_in_epoll = true;
	m_epoll_in_fd = epoll_in_fd;
	mp_active_in_events = active_in_events;

	m_max_epoll_in_events = epoll_in_events;
	m_max_epoll_out_events = epoll_out_events;

	m_socket_active = false;
	m_external_buffer = false;
}

Connection::Connection(unsigned int connection_id,
			int epoll_in_fd, int epoll_out_fd,
			struct epoll_event* active_in_events,
			struct epoll_event* active_out_events,
			unsigned int receive_buffer_size,
			unsigned int send_buffer_size, int epoll_in_events,
			int epoll_out_events)
{
	m_connection_id = connection_id;
	mp_socket = NULL;

	mp_send_buffer = new Lazarus::Buffer(send_buffer_size);
	mp_receive_buffer = new Lazarus::Buffer(receive_buffer_size);
    m_epoll_out_query_required = true;

	//*********** external epoll system
    m_external_out_epoll = true;
	m_external_in_epoll = true;
	m_epoll_in_fd = epoll_in_fd;
	m_epoll_out_fd = epoll_out_fd;
	mp_active_in_events = active_in_events;
	mp_active_out_events = active_out_events;

	m_max_epoll_in_events = epoll_in_events;
	m_max_epoll_out_events = epoll_out_events;

	m_socket_active = false;
	m_external_buffer = false;
}

Connection::~Connection()
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
	}

	//shutdown the >internal< epoll system
	if(m_external_out_epoll == false)
	{
		close(m_epoll_out_fd);

		free(mp_active_out_events);
	}

	if(m_external_in_epoll == false)
	{
		close(m_epoll_in_fd);

		free(mp_active_in_events);
	}
}

void Connection::setExternalBuffer(Lazarus::Buffer* send, Lazarus::Buffer* recv)
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

bool Connection::get_m_socket_active()
{
	return m_socket_active;
}

void Connection::printConnectionInformation()
{
	printf("not implemented\n");
	return;
}


Socket* Connection::get_mp_socket()
{
	return mp_socket;
}

void Connection::set_mp_socket(Socket* socket)
{
	//close any existing socket
	if(mp_socket != NULL)
	{
		//attempt to remove the socket from the internal epoll system
		if(m_external_out_epoll == false)
		{
			//register the socket with the epoll system
			struct epoll_event epoll_event;

			epoll_event.events = EPOLLOUT | EPOLLET | EPOLLRDHUP | EPOLLERR; //edge triggered with data-ready-to-write trigger
			epoll_event.data.fd = mp_socket->get_m_socket_fd();

			int error = epoll_ctl(m_epoll_out_fd, EPOLL_CTL_DEL, mp_socket->get_m_socket_fd(), &epoll_event);
			if (error == -1)
			{
				printf("ERROR: could not deregister existing socket within out-epoll, maybe it was never registered?\n");
				exit(1);
			}
		}

		if(m_external_in_epoll == false)
		{
			//register the socket with the epoll system
			struct epoll_event epoll_event;
			epoll_event.events = EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLERR; //edge triggered with data-ready-to-read trigger
			epoll_event.data.fd = mp_socket->get_m_socket_fd();

			int error = epoll_ctl(m_epoll_in_fd, EPOLL_CTL_DEL, mp_socket->get_m_socket_fd(), &epoll_event);
			if (error == -1)
			{
				printf("ERROR: could not deregister existing socket within out-epoll, maybe it was never registered?\n");
				exit(1);
			}

		}

		mp_socket->closeSocket();
		delete mp_socket;
	}

	mp_socket = socket;

	//set the socket to non blocking
	mp_socket->enableNonblocking();

	//only register the socket fd in the internal epoll system
	if(m_external_out_epoll == false)
	{
		//register the socket with the epoll system
		struct epoll_event epoll_event;

		epoll_event.events = EPOLLOUT | EPOLLET | EPOLLRDHUP | EPOLLERR; //edge triggered with data-ready-to-write trigger
		epoll_event.data.fd = mp_socket->get_m_socket_fd();

		int error = epoll_ctl(m_epoll_out_fd, EPOLL_CTL_ADD, mp_socket->get_m_socket_fd(), &epoll_event);
		if (error == -1)
		{
			printf("ERROR: could not register listener within out-epoll\n");
			exit(1);
		}
	}

	if(m_external_in_epoll == false)
	{
		//register the socket with the epoll system
		struct epoll_event epoll_event;
		epoll_event.events = EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLERR; //edge triggered with data-ready-to-read trigger
		epoll_event.data.fd = mp_socket->get_m_socket_fd();

		int error = epoll_ctl(m_epoll_in_fd, EPOLL_CTL_ADD, mp_socket->get_m_socket_fd(), &epoll_event);
		if (error == -1)
		{
			printf("ERROR: could not register listener within in-epoll\n");
			exit(1);
		}

	}

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
