/*
 * ServerFactory.cpp
 *
 *  Created on: Aug 30, 2013
 *      Author: Darius Malysiak
 */

#include "ServerFactory.h"
#include "../System/InterfaceToolbox.h"

namespace Lazarus {

ServerFactory::ServerFactory() {
	// TODO Auto-generated constructor stub

}

ServerFactory::~ServerFactory() {
	// TODO Auto-generated destructor stub
}

ClusterNodeManagementServer* ServerFactory::getClusterNodeManagementServerInstanceIPv4(unsigned int port,
		unsigned int worker_threads, unsigned int max_connections)
{
	ClusterNodeManagementServer* server = NULL;

	//get the nics
	Lazarus::InterfaceToolbox toolbox;
	Lazarus::FastNCSList<Lazarus::NetworkInterface*>* interfaces = toolbox.getAllRealNICs();

	Lazarus::NetworkInterface* nic = interfaces->getFirst()->getm_data();
	printf("NIC: %s\n",nic->get_dev_filename().c_str());

	server = new ClusterNodeManagementServer(nic->get_ip_address(),
			port,max_connections,worker_threads);


	//release all nic objects
	interfaces->destroy();
	delete interfaces;

	return server;

}

ClusterNodeManagementServer* ServerFactory::getClusterNodeManagementServerInstanceLocalIPv4(unsigned int port,
		unsigned int worker_threads, unsigned int max_connections)
{
	ClusterNodeManagementServer* server = new ClusterNodeManagementServer("127.0.0.1",
			port,max_connections,worker_threads);

	return server;
}



TCPRCSServer* ServerFactory::getTCPRCSServerInstanceIPv4(unsigned int port,
		unsigned int listen_timeout, unsigned int read_frame_timeout)
{
	TCPRCSServer* server = NULL;

	//get the nics
	Lazarus::InterfaceToolbox toolbox;
	Lazarus::FastNCSList<Lazarus::NetworkInterface*>* interfaces = toolbox.getAllRealNICs();

	Lazarus::NetworkInterface* nic = interfaces->getFirst()->getm_data();
	printf("NIC: %s\n",nic->get_dev_filename().c_str());

	server = new TCPRCSServer(nic->get_ip_address(),
			port,listen_timeout,read_frame_timeout);


	//release all nic objects
	interfaces->destroy();
	delete interfaces;

	return server;
}

TCPRCSServer* ServerFactory::getTCPRCSServerInstanceLocalIPv4(unsigned int port,
		unsigned int listen_timeout, unsigned int read_frame_timeout)
{
	TCPRCSServer* server = NULL;

	server = new TCPRCSServer("127.0.0.1",
			port,listen_timeout,read_frame_timeout);


	return server;
}


CallbackServer* ServerFactory::getCallbackServerInstanceIPv4(unsigned int port,
		unsigned int worker_threads, unsigned int max_connections, GenericCallback* callback)
{
	CallbackServer* server = NULL;

	//get the nics
	Lazarus::InterfaceToolbox toolbox;
	Lazarus::FastNCSList<Lazarus::NetworkInterface*>* interfaces = toolbox.getAllRealNICs();

	Lazarus::NetworkInterface* nic = interfaces->getFirst()->getm_data();
	printf("NIC: %s\n",nic->get_dev_filename().c_str());

	server = new CallbackServer(nic->get_ip_address(),
			port,max_connections,worker_threads,callback);


	//release all nic objects
	interfaces->destroy();
	delete interfaces;

	return server;

}

CallbackServer* ServerFactory::getCallbackServerInstanceLocalIPv4(unsigned int port,
		unsigned int worker_threads, unsigned int max_connections, GenericCallback* callback)
{
	CallbackServer* server = new CallbackServer("127.0.0.1",
			port,max_connections,worker_threads,callback);

	return server;
}

UDPRCSServer* ServerFactory::getUDPRCSServerInstanceIPv4(unsigned int local_port,
		unsigned int client_port, unsigned int advertised_port, unsigned int rcs_client_group,
		unsigned int advertise_interval_ms,
		unsigned int advertise_recv_timeout_ms, int poll_count, unsigned int use_RCS_target_list, Lazarus::FastKTuple<std::string> target_list)
{
	UDPRCSServer* server = NULL;

	//get the nics
	Lazarus::InterfaceToolbox toolbox;
	Lazarus::FastNCSList<Lazarus::NetworkInterface*>* interfaces = toolbox.getAllRealNICs();

	Lazarus::NetworkInterface* nic = interfaces->getFirst()->getm_data();
	printf("NIC: %s\n",nic->get_dev_filename().c_str());

	std::string nic_ip = nic->get_ip_address();

	server = new UDPRCSServer(nic->get_ip_address(),nic->get_net_mask(), local_port,nic->get_interface_name(),
			Socket::ADDRESS_TYPE_IPV4,client_port,advertised_port,rcs_client_group,
			advertise_interval_ms,
			advertise_recv_timeout_ms,poll_count,use_RCS_target_list, target_list);

	//release all nic objects
	interfaces->destroy();
	delete interfaces;

	return server;

}

UDPRCSServer* ServerFactory::getUDPRCSServerInstanceLocalIPv4(unsigned int local_port,
		unsigned int client_port, unsigned int advertised_port, unsigned int rcs_client_group,
		unsigned int advertise_interval_ms,
		unsigned int advertise_recv_timeout_ms, int poll_count, unsigned int use_RCS_target_list, Lazarus::FastKTuple<std::string> target_list)
{
	UDPRCSServer* server = new UDPRCSServer("127.0.0.1","255.0.0.0", local_port,"lo",
			Socket::ADDRESS_TYPE_IPV4,client_port,advertised_port,rcs_client_group,
			advertise_interval_ms,
			advertise_recv_timeout_ms, poll_count,use_RCS_target_list, target_list);

	return server;
}

} /* namespace Lazarus */
