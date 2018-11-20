/*
 * UDPBCSocket.cpp
 *
 *  Created on: Jan 19, 2014
 *      Author: Darius Malysiak
 */

#include "UDPBCListenerSocket.h"

namespace Lazarus {

UDPBCListenerSocket::UDPBCListenerSocket(unsigned int local_port, std::string nic_name, enum Socket::ADDRESS_TYPE address_type)
	: UDPSocket("255.255.255.255",local_port,nic_name,address_type)
{

	//prepare the socket
	unsigned int slen=sizeof(struct sockaddr_in);

	switch(m_address_type)
	{
		case ADDRESS_TYPE_IPV4:

			//set local address information
			memset(&m_local_address_struct,0, slen);
			((struct sockaddr_in*)&m_local_address_struct)->sin_port = htons(m_local_port);
			((struct sockaddr_in*)&m_local_address_struct)->sin_addr.s_addr = INADDR_ANY;


			((struct sockaddr_in*)&m_local_address_struct)->sin_family = AF_INET;
			((struct sockaddr_in*)&m_remote_address_struct)->sin_family = AF_INET;
			break;

		case ADDRESS_TYPE_IPV6:

			//set local address information
			memset(&m_local_address_struct,0, slen);
			((struct sockaddr_in6*)&m_local_address_struct)->sin6_port = htons(m_local_port);
			((struct sockaddr_in6*)&m_local_address_struct)->sin6_addr = in6addr_any;//system selects the bounding address

			((struct sockaddr_in6*)&m_local_address_struct)->sin6_family = AF_INET6;
			((struct sockaddr_in6*)&m_remote_address_struct)->sin6_family = AF_INET6;
			break;

		default:
			printf("No address specified\n");
			break;
	}
}

UDPBCListenerSocket::~UDPBCListenerSocket() {
	// TODO Auto-generated destructor stub
}
/*
int UDPSocket::bindSocket()
{


	int result = bind(m_socket_fd, (struct sockaddr* ) &m_local_address_struct, sizeof(struct sockaddr));

	if (result == -1)
	{
		printf("ERROR: could not bind socket\n");
	}

	m_is_bound = true;

	return result;

}*/

} /* namespace Lazarus */
