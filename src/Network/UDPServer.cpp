/*
 * UDPServer.cpp
 *
 *  Created on: Aug 29, 2013
 *      Author: clustro
 */

#include "UDPServer.h"

namespace Lazarus {

UDPServer::UDPServer(std::string local_ip, std::string netmask, unsigned int local_port, std::string device, enum Socket::ADDRESS_TYPE address_type) {

	mp_callback = NULL;
	mp_socket = NULL;

	m_device = device;
	m_local_ip = local_ip;
	m_local_port = local_port;
	m_address_type = address_type;
	m_netmask = netmask;

	mp_socket = new UDPSocket(local_ip, local_port,device, address_type);
	mp_socket->bindSocket(); //we bind the socket because we need the sender address information in calls to sendto
	mp_socket->enableNonblocking();

}

UDPServer::~UDPServer() {

	DELETE_NULL_CHECKING(mp_socket);

}

void UDPServer::setCallback(GenericCallback* callback)
{
	mp_callback = callback;
}

} /* namespace Lazarus */
