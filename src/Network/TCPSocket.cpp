/*
 * Socket.cpp
 *
 *  Created on: 28.02.2013
 *      Author: Darius Malysiak
 */

#include "TCPSocket.h"
#include "../Toolbox.h"

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

#include <netinet/tcp.h>

namespace Lazarus
{

TCPSocket::TCPSocket(enum ADDRESS_TYPE address_type) : Socket(-1) {

	m_address_type = address_type;
	m_local_port = 0;
	m_remote_port = 0;
	m_is_bound = false;
	m_is_connected = false;

	// obtain an ipv4 socket which uses TCP and stream communication
	int socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socket_fd == -1)
	{
		printf("ERROR: socket could not be obtained\n");
		m_socket_fd = 0;
	}
	else
	{
		//fill the socket information into the socket object
		m_socket_fd = socket_fd;
	}

	m_bind_possible = false;

	setCork(false);
	setNoDelay(true);
}

TCPSocket::TCPSocket(int socket_fd, enum ADDRESS_TYPE address_type) : Socket(socket_fd) {

	m_address_type = address_type;
	m_local_port = 0;
	m_remote_port = 0;
	m_is_bound = false;
	m_is_connected = false;

	m_bind_possible = false;

	setCork(false);
	setNoDelay(true);
}

TCPSocket::TCPSocket(std::string local_address, unsigned int local_port,enum ADDRESS_TYPE address_type) : Socket(0)
{
	m_address_type = address_type;
	m_local_port = local_port;
	m_remote_port = 0;
	m_local_address = local_address;
	m_is_bound = false;
	m_is_connected = false;

	// obtain an ipv4 socket which uses TCP and stream communication
	int socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socket_fd == -1)
	{
		printf("ERROR: socket could not be obtained\n");
		m_socket_fd = 0;
	}
	else
	{
		//fill the socket information into the socket object
		m_socket_fd = socket_fd;
	}

	m_bind_possible = true;

	setCork(false);
	setNoDelay(true);
}

TCPSocket::TCPSocket(int socket_fd, std::string local_address, unsigned int local_port,enum ADDRESS_TYPE address_type)
	: Socket(socket_fd)
{
	m_address_type = address_type;
	m_local_port = local_port;
	m_remote_port = 0;
	m_local_address = local_address;
	m_is_bound = false;
	m_is_connected = false;

	m_bind_possible = true;

	setCork(false);
	setNoDelay(true);
}

TCPSocket::~TCPSocket() {
	shutdown(m_socket_fd,SHUT_WR);
}

Lazarus::FastKTuple<unsigned int> TCPSocket::getOSTCPReadBufferSize()
{
	Lazarus::FastKTuple<unsigned int> sizes(3);

	std::string size_values = Lazarus::Toolbox::readFileToString("/proc/sys/net/ipv4/tcp_rmem");

	Lazarus::NCSList<std::string>* tokens = Lazarus::Toolbox::splitString(size_values,' ');
	auto next = tokens->getFirst();

	sizes[0] = Lazarus::Toolbox::stringToUInt(next->getm_data_ref());
	sizes[1] = Lazarus::Toolbox::stringToUInt(next->getmp_neighbor()->getm_data_ref());
	sizes[2] = Lazarus::Toolbox::stringToUInt(next->getmp_neighbor()->getmp_neighbor()->getm_data_ref());

	return sizes;
}


Lazarus::FastKTuple<unsigned int> TCPSocket::getOSTCPWriteBufferSize()
{
	Lazarus::FastKTuple<unsigned int> sizes(3);

	std::string size_values = Lazarus::Toolbox::readFileToString("/proc/sys/net/ipv4/tcp_wmem");

	Lazarus::NCSList<std::string>* tokens = Lazarus::Toolbox::splitString(size_values,' ');
	auto next = tokens->getFirst();

	sizes[0] = Lazarus::Toolbox::stringToUInt(next->getm_data_ref());
	sizes[1] = Lazarus::Toolbox::stringToUInt(next->getmp_neighbor()->getm_data_ref());
	sizes[2] = Lazarus::Toolbox::stringToUInt(next->getmp_neighbor()->getmp_neighbor()->getm_data_ref());

	return sizes;
}

int TCPSocket::bindSocket()
{
	if(m_is_bound==true || m_is_connected == true)
	{
		printf("ERROR: socket is already bound to an address OR is connected to a remote target\n");
		return -2;
	}

	if(m_bind_possible == false)
	{
		printf("ERROR: binding not possible due to missing local information\n");
		return -1;
	}

	if(m_address_type == ADDRESS_TYPE_IPV4)
	{
		struct sockaddr_in target_data;

		//set the communication parameters for ipv4 connections
		if(m_local_address.compare("*")!=0)
			inet_pton(AF_INET,m_local_address.c_str(),&target_data.sin_addr); //inet_addr(address);
		else
			target_data.sin_addr.s_addr = htonl(INADDR_ANY);

		if(m_local_port != 0)
			target_data.sin_port = htons(m_local_port);
		else
			target_data.sin_port = htons(0);

		target_data.sin_family = AF_INET;
		memset(&target_data.sin_zero,0,8);

		//connect via the specified interface
		//setsockopt( socket_fd, SOL_SOCKET, SO_BINDTODEVICE, local_interface.c_str(), strlen(local_interface.c_str()) );

		int flag=1;
		if(setsockopt(m_socket_fd, SOL_SOCKET, SO_REUSEADDR, (void*) &flag, sizeof(flag)) < 0)
		{
			printf("could not set reuseaddr option for tcp socket\n");
		}

		//bind the socket to the specified interface, address and port
		//casting of parameter struct is valid due to binary equivalence with 'sockaddr_in'
		if (bind(m_socket_fd,(struct sockaddr *) &target_data, sizeof(target_data)) == -1)
		{
			printf("ERROR: binding of socket failed\n");
			return -1;
		}

		m_is_bound = true;
	}

	if(m_address_type == ADDRESS_TYPE_IPV6)
	{
		printf("ERROR: currently no support for ipv6\n");
		return -1;
	}

	return 0;
}


int TCPSocket::bindSocketToDevice(const std::string& device)
{
	if(m_is_bound==true || m_is_connected == true)
	{
		printf("ERROR: socket is already bound to an address OR is connected to a remote target\n");
		return -2;
	}

	if(m_bind_possible == false)
	{
		printf("ERROR: binding not possible due to missing local information\n");
		return -1;
	}

	if(m_address_type == ADDRESS_TYPE_IPV4)
	{
		struct sockaddr_in target_data;

		//set the communication parameters for ipv4 connections
		if(m_local_address.compare("*")!=0)
			inet_pton(AF_INET,m_local_address.c_str(),&target_data.sin_addr); //inet_addr(address);
		else
			target_data.sin_addr.s_addr = htonl(INADDR_ANY);

		if(m_local_port != 0)
			target_data.sin_port = htons(m_local_port);
		else
			target_data.sin_port = htons(0);

		target_data.sin_family = AF_INET;
		memset(&target_data.sin_zero,0,8);

		int flag=1;
		if(setsockopt(m_socket_fd, SOL_SOCKET, SO_REUSEADDR, (void*) &flag, sizeof(flag)) < 0)
		{
			printf("could not set reuseaddr option for tcp socket\n");
		}

		if(setsockopt(m_socket_fd, SOL_SOCKET, SO_BINDTODEVICE, device.c_str(), strlen(device.c_str()) ) < 0)
		{
			printf("could not bind to device\n");
		}

		//bind the socket to the specified interface, address and port
		//casting of parameter struct is valid due to binary equivalence with 'sockaddr_in'
		if(bind(m_socket_fd,(struct sockaddr *) &target_data, sizeof(target_data)) == -1)
		{
			printf("ERROR: binding of socket failed\n");
			return -1;
		}

		m_is_bound = true;
	}

	if(m_address_type == ADDRESS_TYPE_IPV6)
	{
		printf("ERROR: currently no support for ipv6\n");
		return -1;
	}

	return 0;
}

int TCPSocket::setNoDelay(bool b)
{
	int flag = 1;

	if(b == false)
	{
		flag = 0;
	}

	int result = setsockopt(m_socket_fd,            /* socket affected */
					 IPPROTO_TCP,     /* set option at TCP level */
					 TCP_NODELAY,     /* name of option */
						 (char *) &flag,  /* the cast is historical cruft */
						 sizeof(int));    /* length of option value */
	if(result < 0)
	{
		printf("ERROR: could not set the TCP delay.\n");
		return -1;
	}

	return 0;
}

int TCPSocket::setCork(bool b)
{
	int flag = 1;

	if(b == false)
	{
		flag = 0;
	}

	int result = setsockopt(m_socket_fd,            /* socket affected */
					 IPPROTO_TCP,     /* set option at TCP level */
					 TCP_CORK,     /* name of option */
						 (char *) &flag,  /* the cast is historical cruft */
						 sizeof(int));    /* length of option value */
	if(result < 0)
	{
		printf("ERROR: could not set the TCP delay.\n");
		return -1;
	}

	return 0;
}

int TCPSocket::closeConnection()
{
	if(m_is_connected == false)
	{
		printf("ERROR: there no active TCP connection which can be closed\n");
		return -1;
	}

	if(m_socket_fd != 0)
	{
		close(m_socket_fd);
	}

	m_socket_fd = 0;

	return 0;
}

int TCPSocket::connectIPv4(const std::string& target_address, unsigned int target_port )
{
	if(m_is_connected == true)
	{
		printf("WARNING: there is already an active TCP connection\n");
	}

	struct sockaddr_in target_data;
	struct sockaddr_in local_data;
	unsigned int local_data_length;

	//set the communication parameters for ipv4 connections
	inet_pton(AF_INET,target_address.c_str(),&target_data.sin_addr); //inet_addr(address);
	target_data.sin_port = htons(target_port);
	target_data.sin_family = AF_INET;
	memset(&target_data.sin_zero,0,sizeof(target_data.sin_zero));//,0,8

	//connect to the target
	//casting of parameter struct is valid due to binary equivalence with 'sockaddr_in'
	if(connect(m_socket_fd,(struct sockaddr *) &target_data, sizeof(target_data)) == -1)
	{
		close(m_socket_fd);
		m_socket_fd = 0;
		printf("ERROR: connection to target failed\n");
		return -1;
	}

	//get local information of the completed socket
	getsockname(m_socket_fd,(struct sockaddr *) &local_data,&local_data_length);

	//fill the socket information into the socket object
	char* c_strbuffer = new char[ 500 ];
	inet_ntop(AF_INET,&local_data.sin_addr,c_strbuffer,500);

	m_address_type = Socket::ADDRESS_TYPE_IPV4;
	m_remote_address = target_address;
	m_remote_port = target_port;

	/*
	 * In case of bounded sockets these two line might be redundant:
	 * Yet this can be used for verification purposes (i.e. if a bound socket uses the correct local address information)
	 */
	m_local_address = std::string(c_strbuffer) ;
	m_local_port = ntohs(local_data.sin_port);

	//free the cstring buffer
	delete[] c_strbuffer;

	m_is_connected = true;

	return 0;
}

void TCPSocket::printSocketInformation()
{
	//print information about the current socket
	printf("Local address: %s\n",m_local_address.c_str());
	printf("Local port: %d\n",m_local_port);
	printf("Remote address: %s\n",m_remote_address.c_str());
	printf("Remote port: %d\n",m_remote_port);

	switch(m_address_type)
	{
		case ADDRESS_TYPE_IPV4:
			printf("Address type: IPv4\n");
			break;

		case ADDRESS_TYPE_IPV6:
			printf("Address type: IPv6\n");
			break;

		default:
			printf("No address specified\n");
			break;
	}
}

void TCPSocket::closeSocket()
{
	//kill the socket, i.e. cancel the accept function
	if(shutdown(m_socket_fd,2) == -1)
	{
		printf("ERROR: failed to shutdown the TCP socket\n");
	}
	if(close(m_socket_fd) == -1)
	{
		printf("ERROR: failed to close the TCP socket\n");
	}

}

}
