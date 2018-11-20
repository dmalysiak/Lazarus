/*
 * ClientFactory.cpp
 *
 *  Created on: Aug 30, 2013
 *      Author: Darius Malysiak
 */

#include "ClientFactory.h"
#include "../System/InterfaceToolbox.h"

namespace Lazarus {

ClientFactory::ClientFactory() {
	// TODO Auto-generated constructor stub

}

ClientFactory::~ClientFactory() {
	// TODO Auto-generated destructor stub
}

UDPRCSClient* ClientFactory::getUDPRCSClientInstanceIPv4(unsigned int port)
{
	Lazarus::InterfaceToolbox toolbox;
	Lazarus::FastNCSList<Lazarus::NetworkInterface*>* interfaces = toolbox.getAllRealNICs();

	Lazarus::NetworkInterface* nic = interfaces->getFirst()->getm_data();

	UDPRCSClient* client = new UDPRCSClient(port,nic->get_interface_name(), Socket::ADDRESS_TYPE_IPV4);

	//release all nic objects
	interfaces->destroy();
	delete interfaces;

	return client;
}

UDPRCSClient* ClientFactory::getUDPRCSClientInstanceLocalIPv4(unsigned int port)
{
	UDPRCSClient* client = new UDPRCSClient(port,"lo", Socket::ADDRESS_TYPE_IPV4);

	return client;
}

ClusterNodeManagementClient* ClientFactory::getClusterNodeManagementClientInstanceIPv4(std::string remote_address,
		unsigned int remote_port, const std::string& config_file, unsigned int max_attempts, unsigned int attempt_wait_time)
{
	ClusterNodeManagementClient* client = NULL;

//	//get the nics
//	Lazarus::InterfaceToolbox toolbox;
//	Lazarus::NCSList<Lazarus::NetworkInterface*>* interfaces = toolbox.getAllRealNICs();
//
//	Lazarus::NetworkInterface* nic = interfaces->getFirst()->getm_data();

	client = new ClusterNodeManagementClient(remote_address, remote_port, config_file,
			max_attempts, attempt_wait_time);

//	//release all nic objects
//	interfaces->destroy();
//	delete interfaces;

	return client;

}

ClusterNodeManagementClient* ClientFactory::getClusterNodeManagementClientInstanceLocalIPv4(unsigned int remote_port,
		const std::string& config_file, unsigned int max_attempts, unsigned int attempt_wait_time)
{
	ClusterNodeManagementClient* client = new ClusterNodeManagementClient("127.0.0.1", remote_port,config_file,
			max_attempts, attempt_wait_time);

	return client;
}

CallbackClient* ClientFactory::getClusterNodeManagementClientInstanceIPv4(std::string remote_address,
			unsigned int remote_port, GenericCallback* callback, unsigned int nodeID, unsigned int max_attempts,
			unsigned int attempt_wait_time)
{
	CallbackClient* client = NULL;

//	//get the nics
//	Lazarus::InterfaceToolbox toolbox;
//	Lazarus::NCSList<Lazarus::NetworkInterface*>* interfaces = toolbox.getAllRealNICs();
//
//	Lazarus::NetworkInterface* nic = interfaces->getFirst()->getm_data();

	client = new CallbackClient(remote_address, remote_port, nodeID,callback,max_attempts, attempt_wait_time);

	//release all nic objects
//	interfaces->destroy();
//	delete interfaces;

	return client;

}


CallbackClient* ClientFactory::getClusterNodeManagementClientInstanceLocalIPv4(unsigned int remote_port,
		GenericCallback* callback, unsigned int nodeID, unsigned int max_attempts,
		unsigned int attempt_wait_time)
{
	CallbackClient* client = new CallbackClient("127.0.0.1", remote_port, nodeID,callback,max_attempts, attempt_wait_time);

	return client;
}


} /* namespace Lazarus */
