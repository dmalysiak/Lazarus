/*
 * Socket.cpp
 *
 *  Created on: 28.02.2013
 *      Author: Darius Malysiak
 */

#include "Socket.h"
#include "../Toolbox.h"

#include <fcntl.h>

namespace Lazarus
{

Socket::Socket(int socket_fd) {

	m_socket_fd = socket_fd;

}

Socket::~Socket() {

	if(m_socket_fd != 0)
	{
		//close the socket
		close(m_socket_fd);
	}

}

int getSocketReceiveBufferSize(const Socket& socket)
{
	int n;
	unsigned int m = sizeof(n);
	getsockopt(socket.get_m_socket_fd(),SOL_SOCKET,SO_RCVBUF,(void *)&n, &m);

	return n;
}

int setSocketReceiveBufferSize(const Socket& socket, int size)
{
	return setsockopt(socket.get_m_socket_fd(),SOL_SOCKET,SO_RCVBUF,(void *)&size, sizeof(int));
}

int getSocketSendBufferSize(const Socket& socket)
{
	int n;
	unsigned int m = sizeof(n);
	getsockopt(socket.get_m_socket_fd(),SOL_SOCKET,SO_SNDBUF,(void *)&n, &m);

	return n;
}

int setSocketSendBufferSize(const Socket& socket, int size)
{
	return setsockopt(socket.get_m_socket_fd(),SOL_SOCKET,SO_SNDBUF,(void *)&size, sizeof(int));
}

Lazarus::FastKTuple<unsigned int> Socket::getOSReadBufferSize()
{
	Lazarus::FastKTuple<unsigned int> sizes(2);

	std::string default_ = Lazarus::Toolbox::readFileToString("/proc/sys/net/core/rmem_default");
	std::string max_ = Lazarus::Toolbox::readFileToString("/proc/sys/net/core/rmem_max");

	sizes[0] = Lazarus::Toolbox::stringToUInt(default_);
	sizes[1] = Lazarus::Toolbox::stringToUInt(max_);

	return sizes;
}


Lazarus::FastKTuple<unsigned int> Socket::getOSWriteBufferSize()
{
	Lazarus::FastKTuple<unsigned int> sizes(2);

	std::string default_ = Lazarus::Toolbox::readFileToString("/proc/sys/net/core/wmem_default");
	std::string max_ = Lazarus::Toolbox::readFileToString("/proc/sys/net/core/wmem_max");

	sizes[0] = Lazarus::Toolbox::stringToUInt(default_);
	sizes[1] = Lazarus::Toolbox::stringToUInt(max_);

	return sizes;
}

Lazarus::FastKTuple<unsigned int> Socket::getCurrentSocketBufferSizes()
{
	Lazarus::FastKTuple<unsigned int> sizes(2);

	int n = 0;
	unsigned int m = sizeof(n);
	getsockopt(m_socket_fd,SOL_SOCKET,SO_RCVBUF,(void *)&n, &m);

	sizes[0] = n;

	getsockopt(m_socket_fd,SOL_SOCKET,SO_SNDBUF,(void *)&n, &m);
	sizes[1] = n;

	return sizes;
}

int Socket::get_m_socket_fd() const
{
	return m_socket_fd;
}

void Socket::enableNonblocking()
{
	int flags = fcntl(m_socket_fd, F_GETFL, 0);

	if (flags == -1)
	{
	  printf("ERROR: could not get the socket flags\n");
	  return;
	}

	flags |= O_NONBLOCK;

	flags = fcntl(m_socket_fd, F_SETFL, flags);

	if (flags == -1)
	{
	  printf("ERROR: could not get/set the updated socket flags\n");
	  return;
	}
}

void Socket::disableNonblocking()
{
	int flags = fcntl(m_socket_fd, F_GETFL, 0);

	if (flags == -1)
	{
	  printf("ERROR: could not get the socket flags\n");
	  return;
	}

	flags ^= O_NONBLOCK;

	flags = fcntl(m_socket_fd, F_SETFL, flags);

	if (flags == -1)
	{
	  printf("ERROR: could not get/set the updated socket flags\n");
	  return;
	}
}

}
