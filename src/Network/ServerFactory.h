/*
 * ServerFactory.h
 *
 *  Created on: Aug 30, 2013
 *      Author: Darius Malysiak
 */

#ifndef SERVERFACTORY_H_
#define SERVERFACTORY_H_

#include "../BaseObject.h"
#include "TCPServer/ClusterNodeManagementServer.h"
#include "TCPServer/CallbackServer.h"
#include "TCPServer/TCPRCSServer.h"
#include "UDPServer/UDPRCSServer.h"

namespace Lazarus {

class ServerFactory: public Lazarus::BaseObject {
public:
	ServerFactory();
	virtual ~ServerFactory();

	/**
	 * creates an ClusterNodeManagementServer instance and binds it to the first found real nic
	 */
	static ClusterNodeManagementServer* getClusterNodeManagementServerInstanceIPv4(unsigned int port,
			unsigned int worker_threads, unsigned int max_connections);

	/**
	 * creates an ClusterNodeManagementServer instance and binds it to the loopback device
	 */
	static ClusterNodeManagementServer* getClusterNodeManagementServerInstanceLocalIPv4(unsigned int port,
			unsigned int worker_threads, unsigned int max_connections);

	/**
	 * creates an TCPRCSServer instance and binds it to the first found real nic
	 */
	static TCPRCSServer* getTCPRCSServerInstanceIPv4(unsigned int port,
			unsigned int listen_timeout = 250, unsigned int read_frame_timeout = 250);

	/**
	 * creates an TCPRCSServer instance and binds it to the loopback device
	 */
	static TCPRCSServer* getTCPRCSServerInstanceLocalIPv4(unsigned int port,
			unsigned int listen_timeout = 250, unsigned int read_frame_timeout = 250);

	/**
	 * creates an callbackServer instance and binds it to the first found real nic
	 */
	static CallbackServer* getCallbackServerInstanceIPv4(unsigned int port,
			unsigned int worker_threads, unsigned int max_connections, GenericCallback* callback);

	/**
	 * creates an callbackServer instance and binds it to the loopback device
	 */
	static CallbackServer* getCallbackServerInstanceLocalIPv4(unsigned int port,
			unsigned int worker_threads, unsigned int max_connections, GenericCallback* callback);

	/**
	 * creates an UDPRCSServer instance and binds it to the first found real nic
	 */
	static UDPRCSServer* getUDPRCSServerInstanceIPv4(unsigned int local_port,
			unsigned int client_port, unsigned int advertised_port, unsigned int rcs_client_group,
			unsigned int advertise_interval_ms,
			unsigned int advertise_recv_timeout_ms, int poll_count, unsigned int use_RCS_target_list = 0, Lazarus::FastKTuple<std::string> target_list = Lazarus::FastKTuple<std::string>(1));

	/**
	 * creates an UDPRCSServer instance and binds it to the loopback device
	 */
	static UDPRCSServer* getUDPRCSServerInstanceLocalIPv4(unsigned int local_port,
			unsigned int client_port, unsigned int advertised_port, unsigned int rcs_client_group,
			unsigned int advertise_interval_ms,
			unsigned int advertise_recv_timeout_ms, int poll_count, unsigned int use_RCS_target_list = 0, Lazarus::FastKTuple<std::string> target_list = Lazarus::FastKTuple<std::string>(1));
};

} /* namespace Lazarus */
#endif /* SERVERFACTORY_H_ */
