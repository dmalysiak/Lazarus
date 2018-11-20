/*
 * UDPClient.h
 *
 *  Created on: Aug 29, 2013
 *      Author: clustro
 */

#ifndef UDPCLIENT_H_
#define UDPCLIENT_H_

#include "../Threading/Thread.h"
#include "../Network/GenericCallback.h"
#include "../Network/UDPSocket.h"

#include <string>

namespace Lazarus {

class UDPClient: public Lazarus::Thread {
public:
	UDPClient(std::string local_ip, unsigned int local_port, std::string device, enum Socket::ADDRESS_TYPE address_type );
	virtual ~UDPClient();

	void setCallback(GenericCallback* mp_callback);

protected:
	GenericCallback* mp_callback;
	UDPSocket* mp_socket;

	std::string m_device;
	std::string m_local_ip;
	unsigned int m_local_port;
	enum Socket::ADDRESS_TYPE m_address_type;

};

} /* namespace Lazarus */
#endif /* UDPCLIENT_H_ */
