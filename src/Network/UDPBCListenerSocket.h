/*
 * UDPBCSocket.h
 *
 *  Created on: Jan 19, 2014
 *      Author: Darius Malysiak
 */

#ifndef UDPBCLISTENERSOCKET_H_
#define UDPBCLISTENERSOCKET_H_

#include "UDPSocket.h"

namespace Lazarus {

class UDPBCListenerSocket: public Lazarus::UDPSocket {
public:
	UDPBCListenerSocket(unsigned int local_port, std::string nic_name, enum Socket::ADDRESS_TYPE address_type = ADDRESS_TYPE_IPV4);
	virtual ~UDPBCListenerSocket();

	//virtual int bindSocket();
};

} /* namespace Lazarus */

#endif /* UDPBCLISTENERSOCKET_H_ */
