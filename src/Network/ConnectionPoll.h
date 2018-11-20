/*
 * Connection.h
 *
 *  Created on: 27.02.2013
 *      Author: Darius Malysiak
 */

#ifndef CONNECTIONPOLL_H_
#define CONNECTIONPOLL_H_

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
#include <sys/poll.h>

namespace Lazarus
{

class ConnectionPoll: public GenericConnection {
public:

	/**
	 * This constructor will create an instance with a dedicated internal poll system.
	 * */
	ConnectionPoll(unsigned int connection_id, unsigned int receive_buffer_size = DEFAULT_RECEIVE_BUFFER_SIZE,
			unsigned int send_buffer_size = DEFAULT_SEND_BUFFER_SIZE);

	virtual ~ConnectionPoll();

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

	//internal descriptors
	struct pollfd m_poll_in_fds;
	struct pollfd m_poll_out_fds;

    bool m_socket_active;
    bool m_poll_out_query_required;
};

}

#endif /* CONNECTION_H_ */
