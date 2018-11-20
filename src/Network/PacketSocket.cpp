/*
 * PacketSocket.cpp
 *
 *  Created on: Feb 25, 2014
 *      Author: Darius Malysiak
 */

#include "PacketSocket.h"

namespace Lazarus {

PacketSocket::PacketSocket(std::string interface) {

	//a pure packet socket: receives all protocols and keeps the link layer header
	m_socket_fd = socket(PF_PACKET,SOCK_RAW,ETH_P_ALL);

	//get the interface
	struct ifreq ifr;
	strncpy((char *)ifr.ifr_name, interface.c_str(), strlen(interface.c_str()));
	ioctl(m_socket_fd, SIOCGIFINDEX, &ifr);

	//set the interface into promiscouse mode
	struct sockaddr_ll sll;
	sll.sll_family = AF_PACKET;
	sll.sll_ifindex = ifr.ifr_ifindex;
	sll.sll_protocol = htons(ETH_P_ALL);
	bind(m_socket_fd, (struct sockaddr *) &sll, sizeof (sll) );

	struct packet_mreq mr;
	memset (&mr, 0, sizeof (mr));
	mr.mr_ifindex = ifr.ifr_ifindex;
	mr.mr_type = PACKET_MR_PROMISC;
	setsockopt(m_socket_fd, SOL_PACKET,PACKET_ADD_MEMBERSHIP, &mr, sizeof (mr));

}

PacketSocket::~PacketSocket()
{
	//close the socket
	close(m_socket_fd);
}

void PacketSocket::send(unsigned char* data, unsigned int size)
{
	write(m_socket_fd,data,size);
}

void PacketSocket::receive(unsigned char* data, unsigned int buffer_size, int& size)
{
	size = read(m_socket_fd,data,buffer_size);
}

} /* namespace Lazarus */
