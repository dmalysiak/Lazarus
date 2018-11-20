/*
 * SocketFactory.h
 *
 *  Created on: Jul 2, 2014
 *      Author: clustro
 */

#ifndef SOCKETFACTORY_H_
#define SOCKETFACTORY_H_

#include "../BaseObject.h"

#include "TCPSocket.h"
#include "UDPSocket.h"
#include "PacketSocket.h"

namespace Lazarus {

class SocketFactory: public Lazarus::BaseObject {
public:
	SocketFactory();
	virtual ~SocketFactory();

	/*
	 * creates an UDPSocket and associates it to the loopback interface
	 */
	static UDPSocket* createUDPSocketLocalIPv4(unsigned int port);
	/*
	 * creates an UDPSocket and associates it to the first found real interface
	 */
	static UDPSocket* createUDPSocketFirstNICIPv4(unsigned int port);
	/*
	 * creates an UDPSocket and associates it to the given NIC
	 */
	static UDPSocket* createUDPSocket(const std::string& nic_name,unsigned int port);



	/*
	 * creates a TCPSocket and associates it to the loopback interface.
	 */
	static TCPSocket* createTCPSocketLocalIPv4(unsigned int port);

	/*
	 * creates a TCPSocket and associates it to the first found real interface
	 */
	static TCPSocket* createTCPSocketFirstNICIPv4(unsigned int port);
	/*
	 * creates a TCPSocket and associates it to the given NIC
	 */
	static TCPSocket* createTCPSocket(const std::string& nic_name,unsigned int port);


	/*
	 * creates a PacketSocket (link layer header will be kept) and associates it to the loopback interface
	 */
	static PacketSocket* createPacketSocketLocal();
	/*
	 * creates a PacketSocket (link layer header will be kept) and associates it to the first found real nic
	 */
	static PacketSocket* createPacketSocketFirstNIC();
	/*
	 * creates a PacketSocket (link layer header will be kept) and associates it to the given NIC
	 */
	static PacketSocket* createPacketSocket(const std::string& nic_name);

private:


};

} /* namespace Lazarus */
#endif /* SOCKETFACTORY_H_ */
