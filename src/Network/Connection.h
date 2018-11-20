/*
 * Connection.h
 *
 *  Created on: 27.02.2013
 *      Author: Darius Malysiak
 */

#ifndef CONNECTION_H_
#define CONNECTION_H_

#include "GenericConnection.h"
#include "Socket.h"
#include "../Buffer.h"

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/epoll.h>


namespace Lazarus
{

class Connection: public GenericConnection {
public:

	static const int MAX_EPOLL_EVENTS = 64;

	/**
	 * This constructor will create an instance with a dedicated internal epoll system.
	 * */
	Connection(unsigned int connection_id, unsigned int receive_buffer_size = DEFAULT_RECEIVE_BUFFER_SIZE,
			unsigned int send_buffer_size = DEFAULT_SEND_BUFFER_SIZE, int epoll_in_events=MAX_EPOLL_EVENTS,
			int epoll_out_events=2);

	/**
	 * This constructor will create an instance with an already existing epoll system.
	 * epoll_in_events and epoll_out_events must be the sizes of active_in_events and active_out_events,
	 * respectively.
	 * */
	Connection(unsigned int connection_id,
				int epoll_in_fd, int epoll_out_fd,
				struct epoll_event* active_in_events,
				struct epoll_event* active_out_events,
				unsigned int receive_buffer_size = DEFAULT_RECEIVE_BUFFER_SIZE,
				unsigned int send_buffer_size = DEFAULT_SEND_BUFFER_SIZE, int epoll_in_events=MAX_EPOLL_EVENTS,
				int epoll_out_events=2);

	/**
	 * This constructor will create an instance with an already existing epoll-in system.
	 * It will create an internal epoll-out system of size epoll_out_events.
	 * epoll_in_events must be the size of active_in_events .
	 * */
	Connection(unsigned int connection_id,
				int epoll_in_fd,
				struct epoll_event* active_in_events,
				unsigned int receive_buffer_size = DEFAULT_RECEIVE_BUFFER_SIZE,
				unsigned int send_buffer_size = DEFAULT_SEND_BUFFER_SIZE, int epoll_in_events=MAX_EPOLL_EVENTS,
				int epoll_out_events=2);

	virtual ~Connection();

	/**
	 * This method permanently replaces the internal buffer with an external one.
	 * Any previously allocated internal buffer will be deleted, previous external buffers will be untouched,
	 * the internal references will only be updated to the new external buffers.
	 * Upon object destruction this buffer will not be deleted, i.e. the connection does not take ownership
	 * of the external buffer. Also keep in mind that this is irreversible, once an external buffer has been
	 * assigned this connection will never again use an internal one.
	 */
	void setExternalBuffer(Lazarus::Buffer* send, Lazarus::Buffer* recv);

	Socket* get_mp_socket();


	/**
	 * Takes ownership over the given socket, furthermore any existing socket will be closed!
	 * In case of an internal epoll system the existing socket will also be unregistered!
	 * This method is useful if the socket should have a specific configuration e.g. being bound to a certain address
	 * or even interface. The socket will always be set to NONBLOCKING!
	 * This method will always perform a check whether the send and receive buffers are sized adequately for the
	 * current OS values of the given socket. If one or both buffers should be too small a warning will be
	 * issued, in case of internal buffers the too small buffer will be increased in size up to the OS value.
	 * */
	void set_mp_socket(Socket* socket);

	virtual void printConnectionInformation();

	bool get_m_socket_active();

protected:
	bool m_external_buffer;
    bool m_epoll_out_query_required;

	bool m_external_out_epoll;
	bool m_external_in_epoll;
	int m_epoll_in_fd;
	int m_epoll_out_fd;
	struct epoll_event* mp_active_in_events;
	struct epoll_event* mp_active_out_events;

	int m_max_epoll_in_events;
	int m_max_epoll_out_events;

    bool m_socket_active;
};

}

#endif /* CONNECTION_H_ */
