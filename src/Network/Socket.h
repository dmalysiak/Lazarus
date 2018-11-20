/*
 * Socket.h
 *
 *  Created on: 28.02.2013
 *      Author: Darius Malysiak
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include "../BaseObject.h"
#include "../DataStructures/FastKTuple.h"

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#include <string>

namespace Lazarus
{

class Socket: public Lazarus::BaseObject {
public:

	/**
	 * This constructor takes ownership over the socket, thus it may be closed at any time by it.
	 */
	Socket(int socket_fd);
	virtual ~Socket();

	/**
	 * This method takes ownership over the socket, thus it may be closed at any time by it.
	 */
	int get_m_socket_fd() const;

	virtual void printSocketInformation() = 0;
	virtual void closeSocket() = 0;

	void enableNonblocking();
	void disableNonblocking();

	/**
	 * Returns a tuple with two numbers: size of the socket receive / write buffer.
	 * */
	Lazarus::FastKTuple<unsigned int> getCurrentSocketBufferSizes();

	/**
	 * Returns a tuple with two numbers: default/max size of the OS socket receive buffer
	 * */
	static Lazarus::FastKTuple<unsigned int> getOSReadBufferSize();

	/**
	 * Returns a tuple with two numbers: default/max size of the OS socket send buffer
	 * */
	static Lazarus::FastKTuple<unsigned int> getOSWriteBufferSize();

	/**
	 * Returns the operating systems receive buffer size for the given socket.
	 * */
	static int getSocketReceiveBufferSize(const Socket& socket);

	/**
	 * Sets the operating systems receive buffer size for the given socket, keep in mind that the OS will double the specified size!
	 * Will return 0 in case of success, otherwise an error code of setsockopt(.).
	 * */
	static int setSocketReceiveBufferSize(const Socket& socket, int size);


	/**
	 * Returns the operating systems send buffer size for the given socket.
	 * */
	static int getSocketSendBufferSize(const Socket& socket);

	/**
	 * Sets the operating systems send buffer size for the given socket, keep in mind that the OS will double the specified size!
	 * Will return 0 in case of success, otherwise an error code of setsockopt(.).
	 * */
	static int setSocketSendBufferSize(const Socket& socket, int size);

	enum CONNECTION_PROTOCOL{CONNECTION_PROTOCOL_UNKNOWN,CONNECTION_PROTOCOL_TCP,CONNECTION_PROTOCOL_UDP,CONNECTION_PROTOCOL_RAW,CONNECTION_PROTOCOL_PACKET};

	enum ADDRESS_TYPE{ADDRESS_TYPE_IPV4,ADDRESS_TYPE_IPV6};

	std::string m_local_address;
	std::string m_remote_address;

protected:
	int m_socket_fd;
};

}

#endif /* SOCKET_H_ */
