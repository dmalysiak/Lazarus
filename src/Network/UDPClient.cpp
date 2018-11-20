/*
 * UDPClient.cpp
 *
 *  Created on: Aug 29, 2013
 *      Author: clustro
 */

#include "UDPClient.h"

namespace Lazarus {

UDPClient::UDPClient(std::string local_ip, unsigned int local_port, std::string device, enum Socket::ADDRESS_TYPE address_type) {

	mp_callback = NULL;
	mp_socket = NULL;

	m_device = device;
	m_local_ip = local_ip;
	m_local_port = local_port;
	m_address_type = address_type;
}

UDPClient::~UDPClient() {

	DELETE_NULL_CHECKING(mp_socket);

}

void UDPClient::setCallback(GenericCallback* callback)
{
	mp_callback = callback;
}

} /* namespace Lazarus */
