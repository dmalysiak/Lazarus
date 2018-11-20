/*
 * Socket.h
 *
 *  Created on: 28.02.2013
 *      Author: Darius Malysiak
 */

#ifndef TCPSOCKET_H_
#define TCPSOCKET_H_

#include "Socket.h"

#include <string>

namespace Lazarus
{

class TCPSocket: public Socket {
public:

	/**
	 * This constructor should be used if no specific local address (i.e. IP and port) is desired.
	 * Binding will be impossible afterwards! It will automatically create a system TCP socket descriptor.
	 * */
	TCPSocket(enum ADDRESS_TYPE address_type = ADDRESS_TYPE_IPV4);

	/**
	 * This constructor should be used if no specific local address (i.e. IP and port) is desired.
	 * Binding will be impossible afterwards!
	 * */
	TCPSocket(int socket_fd, enum ADDRESS_TYPE address_type = ADDRESS_TYPE_IPV4);

	/**
	 * This constructor should be used if a specific local address (i.e. IP and port) is desired. Yet an
	 * explicit call to bind() or bindToDevice() will be required! It will automatically create a system TCP socket descriptor.
	 * One can use 0 as wildcard for the port and '*' for the ip.
	 * */
	TCPSocket(std::string local_address, unsigned int local_port,enum ADDRESS_TYPE address_type = ADDRESS_TYPE_IPV4);

	/**
	 * This constructor should be used if a specific local address (i.e. IP and port) is desired. Yet an
	 * explicit call to bind() or bindToDevice() will be required!
	 * It will utilize the socket descriptor (i.e. take ownership).
	 * One can use 0 as wildcard for the port and '*' for the ip.
	 * */
	TCPSocket(int socket_fd, std::string local_address, unsigned int local_port,enum ADDRESS_TYPE address_type = ADDRESS_TYPE_IPV4);
	virtual ~TCPSocket();

	/*
	 * Bind the socket to the present local address information.
	 * Binding will only associate the socket with a local (IP/PORT) address tuple, i.e.
	 * any connection through this socket will use this local address.
	 * Returns -1 for errors, -2 if already bound, 0 for success.
	 */
	virtual int bindSocket();

	/*
	 * Bind the socket to the present local address information AND to the given device.
	 * Binding will only associate the socket with a local (IP/PORT) address tuple, i.e.
	 * any connection through this socket will use this local address THROUGH THE GIVEN DEVICE.
	 * Returns -1 for errors, -2 if already bound, 0 for success.
	 * This method requires root-privileges!
	 */
	virtual int bindSocketToDevice(const std::string& device);

	/**
	 * Activates or deactivates the TCPNoDelay socket option. Activate it if a lot of high frequent but small
	 * writes / reads is expected.
	 * SH TCP sockets activate this option by default with 'true'.
	 * Returns -1 for errors, 0 for success.
	 * */
	int setNoDelay(bool b);

	/**
	 * Activates or deactivates the Cork socket option. Activate it if a lot of high frequent but small
	 * writes / reads is expected.
	 * SH TCP sockets activate this option by default with 'false'.
	 * Returns -1 for errors, 0 for success.
	 * */
	int setCork(bool b);

	/*
	 * Opens a connection to the given address.
	 */
	int connectIPv4(const std::string& target_address, unsigned int target_port);

	int closeConnection();

	virtual void printSocketInformation();
	virtual void closeSocket();

	/**
	 * Returns a tuple with three numbers: min/avg/max size of the OS socket receive buffer
	 * */
	static Lazarus::FastKTuple<unsigned int> getOSTCPReadBufferSize();

	/**
	 * Returns a tuple with three numbers: min/avg/max size of the OS socket send buffer
	 * */
	static Lazarus::FastKTuple<unsigned int> getOSTCPWriteBufferSize();

	unsigned int m_remote_port;
	unsigned int m_local_port;
	enum ADDRESS_TYPE m_address_type;

private:
	bool m_is_bound;
	bool m_is_connected;
	bool m_bind_possible;

};

}

#endif /* TCPSOCKET_H_ */
