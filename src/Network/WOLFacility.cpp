/*
 * WOLFacility.cpp
 *
 *  Created on: Jul 2, 2014
 *      Author: clustro
 */

#include "WOLFacility.h"
#include "SocketFactory.h"

#include <cstdlib>

namespace Lazarus {

WOLFacility::WOLFacility(unsigned int local_udp_port)
{

	m_local_udp_port = local_udp_port;
	m_udp_socket = SocketFactory::createUDPSocketFirstNICIPv4(m_local_udp_port);
	wol_packet = new unsigned char[17*6];//16 times the MAC address preceded by 6 0xFF bytes

}

WOLFacility::WOLFacility(const std::string& nic_name, unsigned int local_udp_port)
{

	m_local_udp_port = local_udp_port;
	m_udp_socket = SocketFactory::createUDPSocket(nic_name,m_local_udp_port);
	wol_packet = new unsigned char[17*6];//16 times the MAC address preceded by 6 0xFF bytes

}

WOLFacility::~WOLFacility() {

	DELETE_NULL_CHECKING(m_udp_socket);
	DELETE_ARRAY_NULL_CHECKING(wol_packet);

}

void WOLFacility::sendWOLPacketUDP(std::string targetMAC, unsigned int port)
{
	//---------------------------------- split the mac string into 6 fragments
	unsigned char macBytes[6];
	unsigned int string_pos = 0; //indicates the first pos in a MAC segment
	char* cstring = const_cast<char*>(targetMAC.c_str());
	unsigned char byte_[3];

	for(unsigned int i = 0;i<6;++i)
	{
		//---------------- read one segment
		byte_[0] = cstring[string_pos];//left nibble

		//do this check only if string pos is legal!
		if( string_pos+1 < strlen(cstring) )
		{
			if(cstring[string_pos+1] != ':')
			{
				byte_[1] = cstring[string_pos+1];//right nibble
				byte_[2] = '\0';

				string_pos += 3;
			}
			else
			{
				byte_[1] = '\0';
				string_pos +=2;
			}
		}
		else //this else block can only be reached if the last segment contains a single nibble
		{
			byte_[1] = '\0';
		}

		//printf("byte %d: %s \n",i,byte_);

		macBytes[i] = strtol((char*)byte_,NULL,16);
	}

	/*printf("Will send a WOL packet to %d %d %d %d %d %d \n",macBytes[0],macBytes[1],
			macBytes[2],macBytes[3],macBytes[4],macBytes[5]);*/

	//---------------------------------- send the data
	//assemble the wol packet
	memset(wol_packet,0xFF,6);

	for(unsigned int i=0;i<16;++i)
	{
		memcpy(wol_packet+6+i*6,macBytes,6);
	}

	//copy it into the send buffer
	memcpy(m_udp_socket->getSendBuffer()->get_mp_data(),wol_packet,17*6);

	//send the packet
	std::string ip("255.255.255.255");
	m_udp_socket->sendBroadcast(port,17*6,ip);

}

} /* namespace Lazarus */
