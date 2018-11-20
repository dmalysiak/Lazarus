/*
 * WOLFacility.h
 *
 *  Created on: Jul 2, 2014
 *      Author: clustro
 */

#ifndef WOLFACILITY_H_
#define WOLFACILITY_H_

#include "../BaseObject.h"

#include "UDPSocket.h"

#include <string>

namespace Lazarus {

class WOLFacility: public Lazarus::BaseObject {
public:
	/**
	 * Associates the object with the first found NIC and the given source port
	 * */
	WOLFacility(unsigned int local_udp_port = 1600);

	/**
	 * Associates the object with the given NIC and the given source port
	 * */
	WOLFacility(const std::string& nic_name, unsigned int local_udp_port = 1600);

	virtual ~WOLFacility();

	/*
	 * sends out a WOL packet to the given port. The MAC must have the form of
	 * "A:B:C:D:E:F" where A-F are byte value in hexadecimal form, a prefix padding
	 * for values 0-9 is not required. Broadcast will be netwide, i.e. 255.255.255.255
	 */
	void sendWOLPacketUDP(std::string targetMAC, unsigned int port = 4);

private:
	unsigned int m_local_udp_port;
	UDPSocket* m_udp_socket;
	unsigned char* wol_packet;
};

} /* namespace Lazarus */
#endif /* WOLFACILITY_H_ */
