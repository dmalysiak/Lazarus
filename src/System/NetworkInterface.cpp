/*
 * NetworkInterface.cpp
 *
 *  Created on: Aug 30, 2013
 *      Author: Darius Malysiak
 */

#include "NetworkInterface.h"

namespace Lazarus {

NetworkInterface::NetworkInterface() {

	m_nic_speed = NetworkInterface::NIC_SPEED_UNKNOWN;
	m_nic_ip_type = NetworkInterface::NIC_IP_TYPE_UNKNOWN;

}

NetworkInterface::~NetworkInterface() {

}

void NetworkInterface::set_m_nic_speed(enum NetworkInterface::NIC_SPEED nic_speed)
{
	m_nic_speed = nic_speed;
}

enum NetworkInterface::NIC_SPEED NetworkInterface::get_m_nic_speed() const
{
	return m_nic_speed;
}

void NetworkInterface::set_mac_address(std::string mac)
{
	m_mac_address = mac;
}

void NetworkInterface::set_ip_address(std::string ip)
{
	m_ip_address = ip;
}

void NetworkInterface::set_net_mask(std::string mask)
{
	m_net_mask = mask;
}

std::string NetworkInterface::get_mac_address() const
{
	return m_mac_address;
}

std::string NetworkInterface::get_ip_address() const
{
	return m_ip_address;
}

std::string NetworkInterface::get_net_mask() const
{
	return m_net_mask;
}

void NetworkInterface::set_m_ip_type(enum NetworkInterface::NIC_IP_TYPE ip_type)
{
	m_nic_ip_type = ip_type;
}

enum NetworkInterface::NIC_IP_TYPE NetworkInterface::get_m_ip_type() const
{
	return m_nic_ip_type;
}

void NetworkInterface::printInformation() const
{
	printf("----------------\n");
	printf("NIC name: %s\n",m_interface_name.c_str());
	printf("NIC device: %s\n",m_dev_filename.c_str());
	printf("NIC MAC: %s\n",m_mac_address.c_str());

	switch(m_nic_ip_type)
	{
		case NetworkInterface::NIC_IP_TYPE_IPV4:
			printf("NIC IP type: IPv4\n");
			break;
		case NetworkInterface::NIC_IP_TYPE_IPV6:
			printf("NIC IP type: IPv6\n");
			break;
		case NetworkInterface::NIC_IP_TYPE_UNKNOWN:
			printf("NIC IP type: UNKNOWN\n");
			break;
	}

	printf("NIC IP: %s\n",m_ip_address.c_str());
	printf("NIC net mask: %s\n",m_net_mask.c_str());

	switch(m_nic_speed)
	{
		case NetworkInterface::NIC_SPEED_10:
			printf("NIC speed: 10MBit\n");
			break;
		case NetworkInterface::NIC_SPEED_100:
			printf("NIC speed: 100MBit\n");
			break;
		case NetworkInterface::NIC_SPEED_1000:
			printf("NIC speed: 1000MBit\n");
			break;
		case NetworkInterface::NIC_SPEED_10000:
			printf("NIC speed: 10000MBit\n");
			break;
		case NetworkInterface::NIC_SPEED_UNKNOWN:
			printf("NIC speed: UNKNOWN\n");
			break;
	}
	printf("----------------\n");
}

} /* namespace Lazarus */
