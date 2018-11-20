/*
 * NetworkInterface.h
 *
 *  Created on: Aug 30, 2013
 *      Author: Darius Malysiak
 */

#ifndef NETWORKINTERFACE_H_
#define NETWORKINTERFACE_H_

#include "Interface.h"

#include <sys/ioctl.h>
#include <net/if.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

namespace Lazarus {

class NetworkInterface: public Lazarus::Interface {
public:
	NetworkInterface();
	virtual ~NetworkInterface();

	enum NIC_SPEED{ NIC_SPEED_10, NIC_SPEED_100, NIC_SPEED_1000, NIC_SPEED_10000, NIC_SPEED_UNKNOWN };
	enum NIC_IP_TYPE{ NIC_IP_TYPE_IPV6, NIC_IP_TYPE_IPV4, NIC_IP_TYPE_UNKNOWN };

	void set_m_nic_speed(enum NetworkInterface::NIC_SPEED nic_speed);
	enum NetworkInterface::NIC_SPEED get_m_nic_speed() const;

	void set_m_ip_type(enum NetworkInterface::NIC_IP_TYPE ip_type);
	enum NetworkInterface::NIC_IP_TYPE get_m_ip_type() const;

	std::string get_mac_address() const;
	std::string get_ip_address() const;
	std::string get_net_mask() const;

	void set_mac_address(std::string mac);
	void set_ip_address(std::string ip);
	void set_net_mask(std::string mask);

	void printInformation() const;


private:
	std::string m_mac_address;
	std::string m_ip_address;
	std::string m_net_mask;
	enum NetworkInterface::NIC_SPEED m_nic_speed;
	enum NetworkInterface::NIC_IP_TYPE m_nic_ip_type;

};

} /* namespace Lazarus */
#endif /* NETWORKINTERFACE_H_ */
