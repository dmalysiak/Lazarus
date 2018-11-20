/*
 * ConnectionManager.h
 *
 *  Created on: 27.02.2013
 *      Author: Darius Malysiak
 */

#ifndef CONNECTIONMANAGER_H_
#define CONNECTIONMANAGER_H_

#include "../BaseObject.h"
#include "../DataStructures/KTuple.h"
#include "TCPConnection.h"
#include "TCPSocket.h"

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#include <string>

namespace Lazarus
{

class ConnectionManager: public Lazarus::BaseObject {
public:
	ConnectionManager();
	virtual ~ConnectionManager();

	//---------------------------- static helper methods --------------------------------------

	/*
	 * creates a new socket and establishes a connection to the given address.
	 * The socket will not be bound to any predefined address or port.
	 */
	static TCPConnection* connectToTCPIPv4(std::string target_address, unsigned int target_port,
			unsigned int receive_buffer_size= DEFAULT_RECEIVE_BUFFER_SIZE,
			unsigned int send_buffer_size= DEFAULT_SEND_BUFFER_SIZE);

	/*
	 * establishes a connection to the given address but uses an already parameterized socket.
	 * Use this method when a specific local address and port is desired, specify this address in the given socket.
	 * The socket will be bound to this address information (it can also be already bound to any address).
	 */
	static TCPConnection* connectToTCPIPv4(TCPSocket* socket, std::string target_address, unsigned int target_port,
			unsigned int receive_buffer_size= DEFAULT_RECEIVE_BUFFER_SIZE,
			unsigned int send_buffer_size= DEFAULT_SEND_BUFFER_SIZE);


	/*
	 * creates a TCPSocket and binds it to the specified local address and port, i.e. creates a potential listener socket.
	 */
	static TCPSocket* openSocketTCPIPv4(std::string local_address, unsigned int local_port);

};

}
#endif /* CONNECTIONMANAGER_H_ */
