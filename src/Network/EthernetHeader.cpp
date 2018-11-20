/*
 * EthernetHeader.cpp
 *
 *  Created on: Feb 25, 2014
 *      Author: Darius Malysiak
 */

#include "EthernetHeader.h"

#include <linux/if_ether.h>

namespace Lazarus {

EthernetHeader::EthernetHeader()
{

	m_protocol = 0;

}

EthernetHeader::~EthernetHeader() {
	// TODO Auto-generated destructor stub
}

void EthernetHeader::setIP_PROT()
{
	m_protocol = ETH_P_IP;
}

void EthernetHeader::setARP_PROT()
{
	m_protocol = ETH_P_ARP;
}

void EthernetHeader::setRARP_PROT()
{
	m_protocol = ETH_P_RARP;
}

} /* namespace Lazarus */
