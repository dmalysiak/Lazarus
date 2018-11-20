/*
 * SocketFactory.cpp
 *
 *  Created on: Jul 2, 2014
 *      Author: clustro
 */

#include "SocketFactory.h"
#include "../System/InterfaceToolbox.h"

namespace Lazarus {

SocketFactory::SocketFactory() {
	// TODO Auto-generated constructor stub

}

SocketFactory::~SocketFactory() {
	// TODO Auto-generated destructor stub
}

UDPSocket* SocketFactory::createUDPSocketLocalIPv4(unsigned int port)
{
	UDPSocket* socket = NULL;

	socket = new UDPSocket("127.0.0.1", port, "lo");

	return socket;
}

UDPSocket* SocketFactory::createUDPSocketFirstNICIPv4(unsigned int port)
{
	UDPSocket* socket = NULL;

	//get the nics
	Lazarus::InterfaceToolbox toolbox;
	Lazarus::FastNCSList<Lazarus::NetworkInterface*>* interfaces = toolbox.getAllRealNICs();

	Lazarus::NetworkInterface* nic = interfaces->getFirst()->getm_data();

	socket = new UDPSocket(nic->get_ip_address(), port, nic->get_interface_name());

	//release all nic objects
	interfaces->destroy();
	delete interfaces;

	return socket;
}

UDPSocket* SocketFactory::createUDPSocket(const std::string& nic_name,unsigned int port)
{
	UDPSocket* socket = NULL;

	//get the nics
	Lazarus::InterfaceToolbox toolbox;
	Lazarus::NetworkInterface* nic = toolbox.getNIC(nic_name);

	socket = new UDPSocket(nic->get_ip_address(), port, nic->get_interface_name());
	delete nic;

	return socket;
}

TCPSocket* SocketFactory::createTCPSocketLocalIPv4(unsigned int port)
{
	TCPSocket* socket = NULL;

	socket = new TCPSocket("127.0.0.1", port);

	return socket;
}

TCPSocket* SocketFactory::createTCPSocketFirstNICIPv4(unsigned int port)
{
	TCPSocket* socket = NULL;

	//get the nics
	Lazarus::InterfaceToolbox toolbox;
	Lazarus::FastNCSList<Lazarus::NetworkInterface*>* interfaces = toolbox.getAllRealNICs();

	Lazarus::NetworkInterface* nic = interfaces->getFirst()->getm_data();

	socket = new TCPSocket(nic->get_ip_address(), port);

	//release all nic objects
	interfaces->destroy();
	delete interfaces;

	return socket;
}

TCPSocket* SocketFactory::createTCPSocket(const std::string& nic_name,unsigned int port)
{
	TCPSocket* socket = NULL;

	//get the nics
	Lazarus::InterfaceToolbox toolbox;
	Lazarus::NetworkInterface* nic = toolbox.getNIC(nic_name);

	socket = new TCPSocket(nic->get_ip_address(), port);

	delete nic;

	return socket;
}

PacketSocket* SocketFactory::createPacketSocketLocal()
{
	PacketSocket* socket = new PacketSocket("lo");

	return socket;
}


PacketSocket* SocketFactory::createPacketSocketFirstNIC()
{
	PacketSocket* socket = NULL;

	//get the nics
	Lazarus::InterfaceToolbox toolbox;
	Lazarus::FastNCSList<Lazarus::NetworkInterface*>* interfaces = toolbox.getAllRealNICs();

	Lazarus::NetworkInterface* nic = interfaces->getFirst()->getm_data();

	socket = new PacketSocket(nic->get_interface_name());

	//release all nic objects
	interfaces->destroy();
	delete interfaces;

	return socket;
}

PacketSocket* SocketFactory::createPacketSocket(const std::string& nic_name)
{
	PacketSocket* socket = NULL;

	//get the nics
	Lazarus::InterfaceToolbox toolbox;
	Lazarus::NetworkInterface* nic = toolbox.getNIC(nic_name);

	socket = new PacketSocket(nic->get_interface_name());

	delete nic;

	return socket;
}

} /* namespace Lazarus */
