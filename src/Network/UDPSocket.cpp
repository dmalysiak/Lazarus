/*
 * UDPSocket.cpp
 *
 *  Created on: Aug 29, 2013
 *      Author: clustro
 */

#include "UDPSocket.h"
#include "../Toolbox.h"
#include "../System/InterfaceToolbox.h"


namespace Lazarus {

UDPSocket::UDPSocket(const std::string& local_ip, unsigned int local_port, const std::string& nic_name,
		enum Socket::ADDRESS_TYPE address_type ) : Socket(-1) {

	m_address_type = address_type;
	m_nic_name = nic_name;

	//create the socket
	if ((this->m_socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
	{
		printf("ERROR: could not create UDP socket\n");
	}
	else
	{
		//printf("Server : Socket() successful\n");
	}

	m_local_address = local_ip;
	printf("local UDP IP %s\n",m_local_address.c_str());

	m_local_port = local_port;
	m_remote_port = 0;
	m_is_bc_socket = false;

	//prepare the socket
	unsigned int slen=sizeof(struct sockaddr_in);

	switch(m_address_type)
	{
		case ADDRESS_TYPE_IPV4:

			//set local address information
			memset(&m_local_address_struct,0, slen);
			((struct sockaddr_in*)&m_local_address_struct)->sin_port = htons(m_local_port);
			inet_pton(AF_INET,m_local_address.c_str(),&((struct sockaddr_in*)&m_local_address_struct)->sin_addr);

			//((struct sockaddr_in*)&m_local_address_struct)->sin_addr.s_addr = htonl (INADDR_ANY);


			((struct sockaddr_in*)&m_local_address_struct)->sin_family = AF_INET;
			((struct sockaddr_in*)&m_remote_address_struct)->sin_family = AF_INET;
			break;

		case ADDRESS_TYPE_IPV6:

			//set local address information
			memset(&m_local_address_struct,0, slen);
			((struct sockaddr_in6*)&m_local_address_struct)->sin6_port = htons(m_local_port);
			inet_pton(AF_INET6,m_local_address.c_str(),&((struct sockaddr_in6*)&m_local_address_struct)->sin6_addr);

			((struct sockaddr_in6*)&m_local_address_struct)->sin6_family = AF_INET6;
			((struct sockaddr_in6*)&m_remote_address_struct)->sin6_family = AF_INET6;
			break;

		default:
			printf("No address specified\n");
			break;
	}

	//reset remote information
	memset(&m_remote_address_struct,0, slen);

	if(this->m_socket_fd != -1)
	{
		Lazarus::FastKTuple<unsigned int> size = this->getCurrentSocketBufferSizes();
		mp_receive_buffer = new Lazarus::Buffer(size[0]);
		mp_send_buffer = new Lazarus::Buffer(size[1]);
	}
	else
	{
		mp_receive_buffer = new Lazarus::Buffer(UDPSocket::receive_buffer_size);
		mp_send_buffer = new Lazarus::Buffer(UDPSocket::send_buffer_size);
	}

	m_size_sockaddr = sizeof(struct sockaddr_in);
	m_is_bound = false;

//	//prepare the structures for the select call
//	// clear the set
//	FD_ZERO(&m_readfds);
//
//	// add the socket descriptor to the set
//	FD_SET(m_socket_fd, &m_readfds);

	m_poll_fds.fd = this->m_socket_fd;
	m_poll_fds.events = POLLIN;

}

UDPSocket::~UDPSocket() {

	closeSocket();

	DELETE_NULL_CHECKING(mp_receive_buffer);
	DELETE_NULL_CHECKING(mp_send_buffer);

}

void UDPSocket::printSocketInformation()
{
	//print information about the current socket
	printf("Local address: %s\n",m_local_address.c_str());
	printf("Remote address: %s\n",m_remote_address.c_str());

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

void UDPSocket::closeSocket()
{
	if(close(m_socket_fd) == -1)
	{
		printf("ERROR: failed to close the UDP socket\n");
	}
}

int UDPSocket::bindSocketToDevice(const std::string& device)
{
	if(m_is_bound==true)
	{
		printf("ERROR: socket is already bound to an address\n");
		return -2;
	}

	if(m_address_type == ADDRESS_TYPE_IPV4)
	{
		struct sockaddr_in target_data;

		//set the communication parameters for ipv4 connections
		inet_pton(AF_INET,m_local_address.c_str(),&target_data.sin_addr); //inet_addr(address);
		target_data.sin_port = htons(m_local_port);
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

int UDPSocket::bindSocket()
{
	if(m_is_bound == true)
	{
		printf("ERROR: UDP socket is already bound\n");
		return -2;
	}

	int flag=1;
	if(setsockopt(m_socket_fd, SOL_SOCKET, SO_REUSEADDR, (void*) &flag, sizeof(flag)) < 0)
	{
		printf("could not set reuseaddr option for udp socket\n");
		return -1;
	}

	int result = bind(m_socket_fd, (struct sockaddr* ) &m_local_address_struct, sizeof(struct sockaddr));

	if (result == -1)
	{
		printf("ERROR: could not bind socket\n");
		return -1;
	}

	//this requires root privileges and should be used only in rare cases of (system-local) routing problems
	//it will force the socket to use the given interface in every communication attempt!
	/*int res = 0;
	if( (res = setsockopt(m_socket_fd, SOL_SOCKET, SO_BINDTODEVICE, (void *)this->m_nic_name.c_str(), m_nic_name.size()+1) ) < 0)
	{
		printf("could not set SO_BINDTODEVICE option for udp socket (%s) %d\n",m_nic_name.c_str(),res);
		return -1;
	}
	else
	{
		printf("UDP socket bound to device %s\n",m_nic_name.c_str());
	}*/

	m_is_bound = true;

	return 0;

}

unsigned int UDPSocket::getOSUDPMinReadBufferSize()
{

	std::string min_ = Lazarus::Toolbox::readFileToString("/proc/sys/net/ipv4/udp_rmem_min");

	return Lazarus::Toolbox::stringToUInt(min_);

}


unsigned int UDPSocket::getOSUDPMinWriteBufferSize()
{
	std::string min_ = Lazarus::Toolbox::readFileToString("/proc/sys/net/ipv4/udp_wmem_min");

	return Lazarus::Toolbox::stringToUInt(min_);
}

Lazarus::Buffer* UDPSocket::getRecvBuffer()
{
	return mp_receive_buffer;
}

Lazarus::Buffer* UDPSocket::getSendBuffer()
{
	return mp_send_buffer;
}

/**
 * Will receive a data fragment from a single host, the source address and port are temporarily saved in the sockets
 * 'm_remote_address' and 'm_remote_port' attributes
 */
int UDPSocket::receiveFromWho(unsigned int timeout_ms)
{
	//receiving is only possible if the socket has been bound, i.e. has been provided with local network information
	if(m_is_bound == false)
	{
		printf("ERROR: socket is not bound\n");
		return -1;
	}

	//check (with timeout) if data is available
//	m_timeout.tv_sec = timeout_s;
//	m_timeout.tv_usec = timeout_us;

	//int result = select(m_socket_fd + 1, &m_readfds, NULL, NULL, &m_timeout);
	int result = poll(&m_poll_fds, 1, timeout_ms);
	int bytes = 0;

	//if the socket has data
	if(result == 1)
	{
		bytes = recvfrom(m_socket_fd, mp_receive_buffer->get_mp_data(),
				mp_send_buffer->get_m_length(), 0, (struct sockaddr*)&m_remote_address_struct, &m_size_sockaddr);

		if (bytes==-1)
		{
			printf("ERROR: could not receive with UDP socket\n");
		}
		else
		{
			//update local information about the last received udp packet
			char ip[INET6_ADDRSTRLEN];
			Lazarus::InterfaceToolbox::get_ip_str((const struct sockaddr*)&m_remote_address_struct, ip, INET6_ADDRSTRLEN);
			m_remote_address = std::string((const char*)ip);
			m_remote_port = Lazarus::InterfaceToolbox::get_port( (const struct sockaddr*)&m_remote_address_struct );

//			//prepare the structures for the next select call
//			// clear the set
//			FD_ZERO(&m_readfds);
//
//			// add the socket descriptor to the set
//			FD_SET(m_socket_fd, &m_readfds);

		}
	}
	if(result == -1)
	{
		printf("ERROR: something went wrong with the UDP receive\n");
		return -1;
	}
	if(result == 0)
	{
		//printf("ERROR: UDP timeout\n");

//		//prepare the structures for the next select call
//		// clear the set
//		FD_ZERO(&m_readfds);
//
//		// add the socket descriptor to the set
//		FD_SET(m_socket_fd, &m_readfds);

		return 0;
	}


	return bytes;
}

int UDPSocket::receive(unsigned int timeout_ms)
{
	//receiving is only possible if the socket has been bound, i.e. has been provided with local network information
	if(m_is_bound == false)
	{
		printf("ERROR: socket is not bound\n");
		return -1;
	}

	//check (with timeout) if data is available
//	m_timeout.tv_sec = timeout_s;
//	m_timeout.tv_usec = timeout_us;

	//int result = select(m_socket_fd + 1, &m_readfds, NULL, NULL, &m_timeout);
	int result = poll(&m_poll_fds, 1, timeout_ms);
	int bytes = 0;

	//if the socket has data
	if(result == 1)
	{
		bytes = recvfrom(m_socket_fd, mp_receive_buffer->get_mp_data(),
				mp_send_buffer->get_m_length(), 0, (struct sockaddr*)&m_remote_address_struct, &m_size_sockaddr);

		if (bytes==-1)
		{
			printf("ERROR: could not receive with UDP socket\n");
		}
	}
	if(result == -1)
	{
		printf("ERROR: something went wrong with the UDP receive\n");
		return -1;
	}
	if(result == 0)
	{
		//printf("ERROR: UDP timeout\n");

//		//prepare the structures for the next select call
//		// clear the set
//		FD_ZERO(&m_readfds);
//
//		// add the socket descriptor to the set
//		FD_SET(m_socket_fd, &m_readfds);

		return 0;
	}


	return bytes;
}

struct sockaddr_storage* UDPSocket::get_m_remote_address_struct()
{
	return &m_remote_address_struct;
}

int UDPSocket::send(unsigned int target_port, unsigned int data_length, const std::string& target_ip)
{
	//set target information
	if(m_address_type == ADDRESS_TYPE_IPV4)
	{
		if(target_port == 0)
		{
			((struct sockaddr_in*)&m_remote_address_struct)->sin_port = htons(m_remote_port);
		}
		else
		{
			((struct sockaddr_in*)&m_remote_address_struct)->sin_port = htons(target_port);
		}

		if(target_ip.length() == 0)
		{
			inet_pton(AF_INET,m_remote_address.c_str(),&((struct sockaddr_in*)&m_remote_address_struct)->sin_addr);
		}
		else
		{
			inet_pton(AF_INET,target_ip.c_str(),&((struct sockaddr_in*)&m_remote_address_struct)->sin_addr);
		}
	}

	if(m_address_type == ADDRESS_TYPE_IPV6)
	{
		if(target_port == 0)
		{
			((struct sockaddr_in6*)&m_remote_address_struct)->sin6_port = htons(m_remote_port);
		}
		else
		{
			((struct sockaddr_in6*)&m_remote_address_struct)->sin6_port = htons(target_port);
		}

		if(target_ip.length() == 0)
		{
			inet_pton(AF_INET6,m_remote_address.c_str(),&((struct sockaddr_in6*)&m_remote_address_struct)->sin6_addr);
		}
		else
		{
			inet_pton(AF_INET6,target_ip.c_str(),&((struct sockaddr_in6*)&m_remote_address_struct)->sin6_addr);
		}
	}


	int bytes = 0;

	if(data_length != 0)
	{
		bytes = sendto(m_socket_fd, mp_send_buffer->get_mp_data(),
			data_length, 0, (struct sockaddr*)&m_remote_address_struct, sizeof(struct sockaddr_in));
	}
	else
	{
		bytes = sendto(m_socket_fd, mp_send_buffer->get_mp_data(),
			mp_send_buffer->get_m_length(), 0, (struct sockaddr*)&m_remote_address_struct, sizeof(struct sockaddr_in));
	}

	if (bytes ==-1)
	{
		printf("ERROR: could not send UDP paket\n");
	}

	return bytes;
}

int UDPSocket::sendBroadcast(unsigned int target_port, unsigned int data_length, const std::string& target_ip)
{

	//set target information
	if(m_address_type == ADDRESS_TYPE_IPV4)
	{
		if(target_port == 0)
		{
			((struct sockaddr_in*)&m_remote_address_struct)->sin_port = htons(m_remote_port);
		}
		else
		{
			((struct sockaddr_in*)&m_remote_address_struct)->sin_port = htons(target_port);
		}

		if(target_ip.length() == 0)
		{//printf("BC target1 %s\n",m_remote_address.c_str());
			inet_pton(AF_INET,m_remote_address.c_str(),&((struct sockaddr_in*)&m_remote_address_struct)->sin_addr);
		}
		else
		{//printf("BC target2 %s\n",target_ip->c_str());
			inet_pton(AF_INET,target_ip.c_str(),&((struct sockaddr_in*)&m_remote_address_struct)->sin_addr);
		}
	}
	if(m_address_type == ADDRESS_TYPE_IPV6)
	{
		if(target_port == 0)
		{
			((struct sockaddr_in6*)&m_remote_address_struct)->sin6_port = htons(m_remote_port);
		}
		else
		{
			((struct sockaddr_in6*)&m_remote_address_struct)->sin6_port = htons(target_port);
		}

		if(target_ip.length() == 0)
		{
			inet_pton(AF_INET6,m_remote_address.c_str(),&((struct sockaddr_in6*)&m_remote_address_struct)->sin6_addr);
		}
		else
		{
			inet_pton(AF_INET6,target_ip.c_str(),&((struct sockaddr_in6*)&m_remote_address_struct)->sin6_addr);
		}
	}

	//if the socket does not hold the permission to send bc's
	if(m_is_bc_socket == false)
	{
		//set socket to broadcast
		int broadcastPermission = 1;
		if (setsockopt(m_socket_fd, SOL_SOCKET, SO_BROADCAST, (void*) &broadcastPermission, sizeof(int)) < 0)
		{
			printf("ERROR: could not enable broadcast mode for the socket\n");
			return -1;
		}

		m_is_bc_socket = true;
	}

	int bytes =0;

	if(data_length != 0)
	{
		bytes = sendto(m_socket_fd, mp_send_buffer->get_mp_data(), data_length, 0, (struct sockaddr*)&m_remote_address_struct, sizeof(struct sockaddr_in));
	}
	else
	{
		bytes = sendto(m_socket_fd, mp_send_buffer->get_mp_data(), mp_send_buffer->get_m_length(), 0, (struct sockaddr*)&m_remote_address_struct, sizeof(struct sockaddr_in));
	}

	if (bytes ==-1)
	{
		printf("ERROR: could not send UDP paket\n");
	}

	return bytes;
}


} /* namespace Lazarus */
