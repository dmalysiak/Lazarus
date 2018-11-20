/*
 * UDPServer.h
 *
 *  Created on: Aug 29, 2013
 *      Author: clustro
 */

#ifndef UDPSERVER_H_
#define UDPSERVER_H_

#include "../Threading/Thread.h"
#include "GenericCallback.h"
#include "UDPSocket.h"

namespace Lazarus {

class UDPServer: public Lazarus::Thread {
public:
	UDPServer(std::string local_ip, std::string netmask, unsigned int local_port, std::string device, enum Socket::ADDRESS_TYPE address_type );
	virtual ~UDPServer();

	void setCallback(GenericCallback* mp_callback);

protected:
	GenericCallback* mp_callback;
	UDPSocket* mp_socket;
	std::string m_device;

	std::string m_local_ip;
	unsigned int m_local_port;
	std::string m_netmask;
	enum Socket::ADDRESS_TYPE m_address_type;
};

} /* namespace Lazarus */
#endif /* UDPSERVER_H_ */
