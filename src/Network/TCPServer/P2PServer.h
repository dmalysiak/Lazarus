/*
 * P2PServer.h
 *
 *  Created on: 20.03.2013
 *      Author: Darius Malysiak
 */

#ifndef P2PSERVER_H_
#define P2PSERVER_H_

#include "../TCPConnectionPoll.h"
#include "../TCPSocket.h"
#include "../Protocols/Frame.h"

namespace Lazarus {

/**
 * This class allows direct TCP connection between two parties.
 * */
class P2PServer: public Lazarus::BaseObject {
public:
	/**
	 * If both default values are replaced by different data,
	 * the underlying socket will be bound to that address information.
	 * */
	P2PServer(const std::string& local_address, unsigned int local_port, const std::string& local_device="");
	virtual ~P2PServer();

	/**
	 * This blocking method will wait for an incoming client connection, returns -1 in case of timeouts, 0 in case of success and
	 * -2 in case of serious errors!
	 * A timeout value of -1 will wait infinitely.
	 * */
	int waitForClient(int timeout = -1);

	TCPConnectionPoll* get_mp_client_connection();

protected:
	TCPSocket* mp_socket;
	TCPConnectionPoll* mp_client_connection;

	std::string m_local_ip;
	unsigned int m_local_port;
	struct pollfd m_poll_in_fds;

	Frame* mp_frame;
	bool m_ready;

};

} /* namespace Lazarus */
#endif /* P2PSERVER_H_ */

