/*
 * TCPConnection.h
 *
 *  Created on: 27.02.2013
 *      Author: Darius Malysiak
 */

#ifndef TCPCONNECTIONPOLL_H_
#define TCPCONNECTIONPOLL_H_

#include "ConnectionPoll.h"

namespace Lazarus
{

class TCPConnectionPoll: public ConnectionPoll {
public:

	/**
	 * This constructor will create an instance with a dedicated internal epoll system.
	 * */
	TCPConnectionPoll(unsigned int connection_id, unsigned int receive_buffer_size = DEFAULT_RECEIVE_BUFFER_SIZE,
			unsigned int send_buffer_size = DEFAULT_SEND_BUFFER_SIZE);


	virtual ~TCPConnectionPoll();

	/**
	 * This method will attempt to send the first 'data_length' bytes within the current buffer.
	 * Returns -2 in case of timeouts, -1 in case of socket errors, otherwise 0.
	 * The timeout refers to a single internal poll-call, i.e. if multiple calls are required,
	 * each of them will have the same initial timeout value.
	 * */
	virtual int send_data(unsigned int data_length,int timeout=-1);

	/**
	 * Keep in mind that this method can only receive up to connection_buffer_size bytes.
	 * It will iterate over the socket until 'expected_data' bytes have been read.
	 * Returns -2 in case of timeouts, -1 in case of socket errors, otherwise 0.
	 * */
	virtual int wait_for_data(unsigned int expected_data,int timeout=-1);

	/**
	 * Keep in mind that this method can only receive up to connection_buffer_size bytes.
	 * It will iterate over the socket until it is empty, every iteration will overwrite
	 * the data within the connection_buffer from the previous iteration.
	 * Be careful: This method WON'T consume events for other
	 * "registered" sockets (as there is no registration, only the connections socket descriptor will be used!).
	 * This is different than in the case of an epoll system!
	 * Returns -2 in case of timeouts, -1 in case of socket errors, otherwise the number of received bytes.
	 * */
	virtual int receive(int timeout = -1);

	virtual void printConnectionInformation();

	/**
	 * Connects to the given remote information. If a value for local_ip and local_port is given, the underlying
	 * socket will be bound to that ip and port. Should also a value for local_device be given, then the socket will
	 * not only be bound to a specific ip and port but also to the given device.
	 * In order to maintain a consistent state, this method will fail with -1 if an external epoll system is used.
	 * '*' and 0 are used as wildcard for IP and port, respectively.
	 * Returns 0 in case of success, -1 otherwise.
	 * An existing connection will be closed by this method.
	 * */
	virtual int connect(unsigned int remote_port, const std::string remote_ip, const std::string& local_ip="*",
			unsigned int local_port=0, const std::string& local_device="");

	/**
	 * Setting haste true will set the underlying sockets nodelay option on every write call.
	 * Careful, this essentially disables Nagles algorithm.
	 * SH TCP connections are hasted by default.
	 * */
	void setHaste(bool b);

private:

	bool m_haste;

};

}

#endif /* TCPCONNECTION_H_ */
