/*
 * InterfaceToolbox.cpp
 *
 *  Created on: Aug 30, 2013
 *      Author: Darius Malysiak
 */

#include "InterfaceToolbox.h"

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
#include <errno.h>

//this is linux specific
#include <linux/sockios.h>
#include <linux/ethtool.h>

namespace Lazarus {

InterfaceToolbox::InterfaceToolbox() {
	// TODO Auto-generated constructor stub

}

InterfaceToolbox::~InterfaceToolbox() {
	// TODO Auto-generated destructor stub
}

char *get_ip_str(const struct sockaddr *sa, char *s, size_t maxlen)
{
  switch(sa->sa_family)
  {
	  case AF_INET:
		inet_ntop(AF_INET, &(((struct sockaddr_in *)sa)->sin_addr),
			  s, maxlen);
		break;

	  case AF_INET6:
		inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)sa)->sin6_addr),
			  s, maxlen);
		break;

	  default:
		strncpy(s, "Unknown AF", maxlen);
		return NULL;
  }

  return s;
}

FastNCSList<NetworkInterface*>* InterfaceToolbox::getAllNICs()
{
	FastNCSList<NetworkInterface*>* nic_list = new FastNCSList<NetworkInterface*>();
	NetworkInterface* nic = NULL;

	char          buf[1024] = {0};
	struct ifconf ifc = {0};
	struct ifreq* ifr = NULL;
	int           socket_fd = 0;
	unsigned int interface_count = 0;
	struct ethtool_cmd eth_data;

	/* Get a socket handle. */
	socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(socket_fd == -1)
	{
		printf("ERROR: could not create socket fd\n");
		return nic_list;
	}

	/* Query available interfaces. */
	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = buf;
	if(ioctl(socket_fd, SIOCGIFCONF, &ifc) < 0)
	{
		printf("ERROR: ioctl(SIOCGIFCONF)\n");
		return nic_list;
	}

	/* Iterate through the list of interfaces. */
	ifr = ifc.ifc_req;
	interface_count = ifc.ifc_len / sizeof(struct ifreq);

	for(unsigned int i = 0; i < interface_count; i++)
	{
		//create the nic object
		nic = new NetworkInterface();

		struct ifreq* item = &ifr[i];

		struct sockaddr* addr = &(item->ifr_addr);


		//----------------- get the interface name
		nic->set_interface_name( std::string((const char*)item->ifr_name) );

		//----------------- get the corresponding device file
		nic->set_dev_filename( std::string("/dev/") + std::string((const char*)item->ifr_name) );

		//----------------- get the ip type
		switch(addr->sa_family)
		{
			case AF_INET:
				nic->set_m_ip_type(NetworkInterface::NIC_IP_TYPE_IPV4);
				break;

			case AF_INET6:
				nic->set_m_ip_type(NetworkInterface::NIC_IP_TYPE_IPV6);
				break;

			default:
				nic->set_m_ip_type(NetworkInterface::NIC_IP_TYPE_UNKNOWN);
				return NULL;
		}


		//----------------- get the ip address
		char ip[INET6_ADDRSTRLEN];

		get_ip_str(addr, ip, INET6_ADDRSTRLEN);

		nic->set_ip_address( std::string((const char*)ip) );

		//----------------- get the net mask
		char net_mask[INET6_ADDRSTRLEN];//this is large enough!

		addr = &(item->ifr_netmask);//although it is a union, we update the attribute reference
		ioctl(socket_fd, SIOCGIFNETMASK, item);//the item
		get_net_mask(addr, net_mask, INET6_ADDRSTRLEN);

		nic->set_net_mask( std::string((const char*)net_mask) );


		//----------------- get the MAC address
		if(ioctl(socket_fd, SIOCGIFHWADDR, item) < 0)
		{
			printf("ERROR: ioctl(SIOCGIFHWADDR)\n");
			return nic_list;
		}

		char mac[18];

		//XX:XX:XX:XX:XX:XX
		sprintf(mac,"%hhX", (unsigned char)item->ifr_hwaddr.sa_data[0]);
		mac[2]=':';
		sprintf(mac+3,"%hhX", (unsigned char)item->ifr_hwaddr.sa_data[1]);
		mac[5]=':';
		sprintf(mac+6,"%hhX", (unsigned char)item->ifr_hwaddr.sa_data[2]);
		mac[8]=':';
		sprintf(mac+9,"%hhX", (unsigned char)item->ifr_hwaddr.sa_data[3]);
		mac[11]=':';
		sprintf(mac+12,"%hhX", (unsigned char)item->ifr_hwaddr.sa_data[4]);
		mac[14]=':';
		sprintf(mac+15,"%hhX", (unsigned char)item->ifr_hwaddr.sa_data[5]);
		mac[17] = '\0';

		nic->set_mac_address( std::string((const char*)mac) );

		//----------------- get the link speed
		item->ifr_data = (char*)&eth_data;

		eth_data.cmd = ETHTOOL_GSET;

		int rc = ioctl(socket_fd, SIOCETHTOOL, item);
		if (rc < 0) {
			printf("ERROR: ioctl %d - could not get speed (loopback device?) %s\n",errno,nic->get_dev_filename().c_str());
		}

		switch (eth_data.speed)
		{
			case SPEED_10:
				nic->set_m_nic_speed(NetworkInterface::NIC_SPEED_10);
				break;
			case SPEED_100:
				nic->set_m_nic_speed(NetworkInterface::NIC_SPEED_100);
				break;
			case SPEED_1000:
				nic->set_m_nic_speed(NetworkInterface::NIC_SPEED_1000);
				break;
			case SPEED_10000:
				nic->set_m_nic_speed(NetworkInterface::NIC_SPEED_10000);
				break;
			default: nic->set_m_nic_speed(NetworkInterface::NIC_SPEED_UNKNOWN);
				break;
		}

		//if everything went well so far, add the interface to the list
		nic_list->appendLast(nic);

	}

	//close the socket
	close(socket_fd);

	return nic_list;
}

FastNCSList<NetworkInterface*>* InterfaceToolbox::getAllRealNICs()
{
	FastNCSList<NetworkInterface*>* nic_list = new FastNCSList<NetworkInterface*>();
	NetworkInterface* nic = NULL;

	char          buf[1024] = {0};
	struct ifconf ifc = {0};
	struct ifreq* ifr = NULL;
	int           socket_fd = 0;
	unsigned int interface_count = 0;
	struct ethtool_cmd eth_data;

	/* Get a socket handle. */
	socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(socket_fd == -1)
	{
		printf("ERROR: could not create socket fd\n");
		return nic_list;
	}

	/* Query available interfaces. */
	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = buf;
	if(ioctl(socket_fd, SIOCGIFCONF, &ifc) < 0)
	{
		printf("ERROR: ioctl(SIOCGIFCONF)\n");
		return nic_list;
	}

	/* Iterate through the list of interfaces. */
	ifr = ifc.ifc_req;
	interface_count = ifc.ifc_len / sizeof(struct ifreq);

	for(unsigned int i = 0; i < interface_count; i++)
	{
		//check if interface name equals the loopback device, if so, skip the iteration
		if( strcmp( ((struct ifreq*)&ifr[i])->ifr_name , "lo" ) == 0 )
		{
			continue;
		}

		//create the nic object
		nic = new NetworkInterface();

		struct ifreq* item = &ifr[i];

		struct sockaddr* addr = &(item->ifr_addr);


		//----------------- get the interface name
		nic->set_interface_name( std::string((const char*)item->ifr_name) );

		//----------------- get the corresponding device file
		nic->set_dev_filename( std::string("/dev/") + std::string((const char*)item->ifr_name) );

		//----------------- get the ip type
		switch(addr->sa_family)
		{
			case AF_INET:
				nic->set_m_ip_type(NetworkInterface::NIC_IP_TYPE_IPV4);
				break;

			case AF_INET6:
				nic->set_m_ip_type(NetworkInterface::NIC_IP_TYPE_IPV6);
				break;

			default:
				nic->set_m_ip_type(NetworkInterface::NIC_IP_TYPE_UNKNOWN);
				return NULL;
		}


		//----------------- get the ip address
		char ip[INET6_ADDRSTRLEN];

		get_ip_str(addr, ip, INET6_ADDRSTRLEN);

		nic->set_ip_address( std::string((const char*)ip) );

		//----------------- get the net mask
		char net_mask[INET6_ADDRSTRLEN];//this is large enough!

		addr = &(item->ifr_netmask);//although it is a union, we update the attribute reference
		ioctl(socket_fd, SIOCGIFNETMASK, item);//the item
		get_net_mask(addr, net_mask, INET6_ADDRSTRLEN);

		nic->set_net_mask( std::string((const char*)net_mask) );


		//----------------- get the MAC address
		if(ioctl(socket_fd, SIOCGIFHWADDR, item) < 0)
		{
			printf("ERROR: ioctl(SIOCGIFHWADDR)\n");
			return nic_list;
		}

		char mac[18];

		//XX:XX:XX:XX:XX:XX
		sprintf(mac,"%hhX", (unsigned char)item->ifr_hwaddr.sa_data[0]);
		mac[2]=':';
		sprintf(mac+3,"%hhX", (unsigned char)item->ifr_hwaddr.sa_data[1]);
		mac[5]=':';
		sprintf(mac+6,"%hhX", (unsigned char)item->ifr_hwaddr.sa_data[2]);
		mac[8]=':';
		sprintf(mac+9,"%hhX", (unsigned char)item->ifr_hwaddr.sa_data[3]);
		mac[11]=':';
		sprintf(mac+12,"%hhX", (unsigned char)item->ifr_hwaddr.sa_data[4]);
		mac[14]=':';
		sprintf(mac+15,"%hhX", (unsigned char)item->ifr_hwaddr.sa_data[5]);
		mac[17] = '\0';

		nic->set_mac_address( std::string((const char*)mac) );

		//----------------- get the link speed
		item->ifr_data = (char*)&eth_data;

		eth_data.cmd = ETHTOOL_GSET;

		int rc = ioctl(socket_fd, SIOCETHTOOL, item);
		if (rc < 0) {
			printf("ERROR: ioctl %d - could not get speed (loopback device?) %s\n",errno,nic->get_dev_filename().c_str());
		}

		switch (eth_data.speed)
		{
			case SPEED_10:
				nic->set_m_nic_speed(NetworkInterface::NIC_SPEED_10);
				break;
			case SPEED_100:
				nic->set_m_nic_speed(NetworkInterface::NIC_SPEED_100);
				break;
			case SPEED_1000:
				nic->set_m_nic_speed(NetworkInterface::NIC_SPEED_1000);
				break;
			case SPEED_10000:
				nic->set_m_nic_speed(NetworkInterface::NIC_SPEED_10000);
				break;
			default: nic->set_m_nic_speed(NetworkInterface::NIC_SPEED_UNKNOWN);
				break;
		}

		//if everything went well so far, add the interface to the list
		nic_list->appendLast(nic);

	}

	//close the socket
	close(socket_fd);

	return nic_list;
}

NetworkInterface* InterfaceToolbox::getFirstRealNIC()
{
	NetworkInterface* first_interface = NULL;

	FastNCSList<NetworkInterface*>* list = InterfaceToolbox::getAllRealNICs();

	first_interface = list->getFirst()->getm_data();

	//delete all remaining interfaces
	GenNCSListElement<NetworkInterface*>* element = list->getFirst();

	if(element != NULL)
	{
		//skip the first (existing) element
		element = element->getmp_neighbor();

		while(element != NULL)
		{
			//delete the contained network interface
			delete element->getm_data();

			//get next interface
			element = element->getmp_neighbor();
		}
	}

	return first_interface;
}

NetworkInterface* InterfaceToolbox::getNIC(const std::string& nic_name)
{
	NetworkInterface* interface = NULL;

	FastNCSList<NetworkInterface*>* list = InterfaceToolbox::getAllNICs();

	//iterate over interfaces
	for(auto it = list->getStart();it!= list->getEnd();++it)
	{
		interface = it.getElement()->getm_data();
		//break if found
		if(interface->get_interface_name().compare(nic_name)==0)
		{
			break;
		}
	}

	//delete all remaining interfaces
	GenNCSListElement<NetworkInterface*>* element = list->getFirst();

	if(element != NULL)
	{
		//skip the first (existing) element
		element = element->getmp_neighbor();

		while(element != NULL)
		{
			//delete the contained network interface
			delete element->getm_data();

			//get next interface
			element = element->getmp_neighbor();
		}
	}

	return interface;
}

char* InterfaceToolbox::get_ip_str(const struct sockaddr* sa, char* s, size_t maxlen)
{
  switch(sa->sa_family)
  {
	  case AF_INET:
		inet_ntop(AF_INET, &(((struct sockaddr_in*)sa)->sin_addr), s, maxlen);
		break;

	  case AF_INET6:
		inet_ntop(AF_INET6, &(((struct sockaddr_in6*)sa)->sin6_addr), s, maxlen);
		break;

	  default:
		strncpy(s, "Unknown AF", maxlen);
		return NULL;
  }

  return s;
}

char* InterfaceToolbox::get_net_mask(const struct sockaddr* sa, char* s, size_t maxlen)
{
  switch(sa->sa_family)
  {
	  case AF_INET:
		  inet_ntop(AF_INET, &(((struct sockaddr_in*)sa)->sin_addr), s, maxlen);
		  break;

	  case AF_INET6:
		  memset(s,0,maxlen);
		  s[maxlen-1] = '\0'; //ipv6 uses no netmasks
		  break;

	  default:
		  strncpy(s, "Unknown AF", maxlen);
		  return NULL;
  }

  return s;
}

unsigned int InterfaceToolbox::get_port(const struct sockaddr *sa)
{
	unsigned int port = 0;

	switch(sa->sa_family)
	{
	  case AF_INET:
		  port = ntohs(((struct sockaddr_in*)sa)->sin_port);
		  break;

	  case AF_INET6:
		  port = ntohs(((struct sockaddr_in6*)sa)->sin6_port);
		  break;

	  default:
		  break;
	}

	return port;
}

} /* namespace Lazarus */
