/*
 * UDPSocket.h
 *
 *  Created on: Aug 29, 2013
 *      Author: clustro
 */

#ifndef UDPSOCKET_H_
#define UDPSOCKET_H_

#include "Socket.h"
#include "../Buffer.h"

#include <string>
#include <sys/poll.h>
#include <string.h>

namespace Lazarus {

class UDPSocket: public Lazarus::Socket {
public:
	UDPSocket(const std::string& local_ip, unsigned int local_port, const std::string& nic_name, enum Socket::ADDRESS_TYPE address_type = ADDRESS_TYPE_IPV4);
	virtual ~UDPSocket();

	Lazarus::Buffer* getRecvBuffer();
	Lazarus::Buffer* getSendBuffer();

	/**
	* This method returns a pointer to the internal remote address struct.
	* Obtaining such a reference is useful when receiving data, as the sender information is saved
	* in the remote address struct.
	*/
	struct sockaddr_storage* get_m_remote_address_struct();

	virtual void printSocketInformation();
	virtual void closeSocket();

	/**
	 * Bind the socket to the present local address information.
	 * Binding will only associate the socket with a local (IP/PORT) address tuple, i.e.
	 * any 'UDP connection' through this socket will use this local address.
	 * Returns -1 for errors, -2 if already bound, 0 for success.
	 */
	virtual int bindSocket();

	/**
	 * Bind the socket to the present local address information AND to the given device.
	 * Binding will only associate the socket with a local (IP/PORT) address tuple, i.e.
	 * any connection through this socket will use this local address THROUGH THE GIVEN DEVICE.
	 * Returns -1 for errors, -2 if already bound, 0 for success.
	 * This method requires root-privileges!
	 */
	virtual int bindSocketToDevice(const std::string& device);

	/**
	 * Will receive a data fragment from a single host, the source address and port are temporarily saved in the sockets
	 * 'm_remote_address' and 'm_remote_port' attributes
	 */
	virtual int receiveFromWho(unsigned int timeout_ms=0);
	virtual int receive(unsigned int timeout_ms=0);

	virtual int send(unsigned int target_port, unsigned int data_length = 0, const std::string& target_ip = "");

	/*
	 * once the this method was executed, the socket will keep the permission to send broadcasts.
	 * The target address must be specified as one can choose to send a all net broadcast (255.255.255.255)
	 * or a subnet broadcast (e.g. 192.168.0.255). If no address is specified, 'm_remote_address' will be used.
	 */
	virtual int sendBroadcast(unsigned int target_port, unsigned int data_length = 0, const std::string& target_ip = "");

	/**
	 * Returns the minimum size of the OS UDP socket receive buffer
	 * */
	static unsigned int getOSUDPMinReadBufferSize();

	/**
	 * Returns the minimum size of the OS UDP socket send buffer
	 * */
	static unsigned int getOSUDPMinWriteBufferSize();

	static const unsigned int receive_buffer_size = 65000;
	static const unsigned int send_buffer_size = 65000;

	unsigned int m_remote_port;
	unsigned int m_local_port;
	enum ADDRESS_TYPE m_address_type;
	std::string m_nic_name;

protected:
	struct sockaddr_storage m_local_address_struct;
	struct sockaddr_storage m_remote_address_struct;

	Lazarus::Buffer* mp_send_buffer;
	Lazarus::Buffer* mp_receive_buffer;

	unsigned int m_size_sockaddr;

	bool m_is_bound;
	bool m_is_bc_socket;

	//fd_set m_readfds;
	struct pollfd m_poll_fds;

	struct timeval m_timeout;
};

} /* namespace Lazarus */
#endif /* UDPSOCKET_H_ */
