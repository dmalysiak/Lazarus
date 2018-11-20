/*
 * GenericConnection.h
 *
 *  Created on: 02.03.2015
 *      Author: Darius Malysiak
 */

#ifndef GENERICCONNECTION_H_
#define GENERICCONNECTION_H_

#include "../BaseObject.h"
#include "Socket.h"
#include "../Buffer.h"

namespace Lazarus {


//to keep the library simple we set these values to at least the default size of the systems tcp buffer
//cat /proc/sys/net/ipv4/tcp_rmem (min default max)
//if the value has been chosen to be smaller than the systems TCP buffer, it must be ensured that the receive method can handle a full
//system buffer of data (as the local buffer will be smaller than it).
#define DEFAULT_RECEIVE_BUFFER_SIZE 12800000
#define DEFAULT_SEND_BUFFER_SIZE 12800000

class GenericConnection: public Lazarus::BaseObject {
public:
	GenericConnection();
	virtual ~GenericConnection();

	virtual int disconnect();

	virtual int receive(int timeout = -1) = 0;
	virtual int send_data(unsigned int data_length,int timeout=-1) = 0;
	virtual int wait_for_data(unsigned int expected_data,int timeout=-1) = 0;

	unsigned int get_m_connection_id();

	Lazarus::Buffer* get_mp_receive_buffer();
	Lazarus::Buffer* get_mp_send_buffer();

	/**
	 * Returns true if the connection is still active, false if not.
	 * */
	bool isActive();

protected:
	Lazarus::Buffer* mp_send_buffer;
	Lazarus::Buffer* mp_receive_buffer;
	Socket* mp_socket;
	unsigned int m_connection_id;
	bool m_is_active;
};

} /* namespace Lazarus */

#endif /* GENERICCONNECTION_H_ */
