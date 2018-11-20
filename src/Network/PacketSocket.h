/*
 * PacketSocket.h
 *
 *  Created on: Feb 25, 2014
 *      Author: Darius Malysiak
 */

#ifndef PACKETSOCKET_H_
#define PACKETSOCKET_H_

#include "../BaseObject.h"

#include <string.h>
#include <string>

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/if_ether.h>
#include <netpacket/packet.h>
#include <net/ethernet.h> /* the L2 protocols */
#include <net/if.h>

namespace Lazarus {

class PacketSocket: public Lazarus::BaseObject {
public:
	PacketSocket(std::string interface);
	virtual ~PacketSocket();

	void send(unsigned char* data, unsigned int size);
	void receive(unsigned char* data, unsigned int buffer_size, int& size);

private:
	int m_socket_fd;
};

} /* namespace Lazarus */

#endif /* PACKETSOCKET_H_ */
