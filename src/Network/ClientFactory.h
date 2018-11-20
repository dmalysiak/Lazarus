/*
 * ClientFactory.h
 *
 *  Created on: Aug 30, 2013
 *      Author: Darius Malysiak
 */

#ifndef CLIENTFACTORY_H_
#define CLIENTFACTORY_H_

#include "../BaseObject.h"
#include "UDPClient/UDPRCSClient.h"
#include "TCPClient/ClusterNodeManagementClient.h"
#include "TCPClient/CallbackClient.h"
#include "TCPClient/TCPRCSClient.h"

#include <string>

namespace Lazarus {

class ClientFactory: public Lazarus::BaseObject {
public:
	ClientFactory();
	virtual ~ClientFactory();

	/**
	 * creates an UDPRCSClient instance and binds it to the first found real nic
	 */
	static UDPRCSClient* getUDPRCSClientInstanceIPv4(unsigned int port);

	/**
	 * creates an UDPRCSClient instance and binds it to the loopback device
	 */
	static UDPRCSClient* getUDPRCSClientInstanceLocalIPv4(unsigned int port);

	/**
	 * creates an ClusterNodeManagementClient instance and connects via the first found real nic
	 */
	static ClusterNodeManagementClient* getClusterNodeManagementClientInstanceIPv4(std::string remote_address,
			unsigned int remote_port, const std::string& config_file, unsigned int max_attempts = 20, unsigned int attempt_wait_time = 1000);

	/**
	 * creates an ClusterNodeManagementClient instance and connects via the loopback device
	 */
	static ClusterNodeManagementClient* getClusterNodeManagementClientInstanceLocalIPv4(unsigned int remote_port,
			const std::string& config_file, unsigned int max_attempts = 20, unsigned int attempt_wait_time = 1000);

	/**
	 * creates an CallbackClient instance and connects via the first found real nic
	 */
	static CallbackClient* getClusterNodeManagementClientInstanceIPv4(std::string remote_address,
			unsigned int remote_port, GenericCallback* callback, unsigned int nodeID, unsigned int max_attempts = 20,
			unsigned int attempt_wait_time = 1000);

	/**
	 * creates an CallbackClient instance and connects via the loopback device
	 */
	static CallbackClient* getClusterNodeManagementClientInstanceLocalIPv4(unsigned int remote_port,
			GenericCallback* callback, unsigned int nodeID, unsigned int max_attempts = 20,
			unsigned int attempt_wait_time = 1000);


};

} /* namespace Lazarus */
#endif /* CLIENTFACTORY_H_ */
