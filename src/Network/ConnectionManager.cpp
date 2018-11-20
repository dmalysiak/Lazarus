/*
 * ConnectionManager.cpp
 *
 *  Created on: 27.02.2013
 *      Author: Darius Malysiak
 */

#include "ConnectionManager.h"
#include "TCPConnection.h"

#include <string.h>

namespace Lazarus
{

ConnectionManager::ConnectionManager()
{
}

ConnectionManager::~ConnectionManager() {


}

TCPConnection* ConnectionManager::connectToTCPIPv4(TCPSocket* socket, std::string target_address, unsigned int target_port,
			unsigned int receive_buffer_size, unsigned int send_buffer_size)
{
	int res = socket->bindSocket();

	if(res == -1)
	{
		printf("ERROR: something bad happened during binding\n");
		return NULL;
	}

	res = socket->connectIPv4(target_address, target_port);

	if(res == -1)
	{
		printf("ERROR: could not connect TCP socket\n");
		return NULL;
	}

	TCPConnection* connection = new TCPConnection(0,receive_buffer_size,send_buffer_size);
	connection->set_mp_socket(socket);


	return connection;
}


TCPConnection* ConnectionManager::connectToTCPIPv4(std::string target_address,
		unsigned int target_port, unsigned int receive_buffer_size, unsigned int send_buffer_size)
{
	TCPSocket* tcpsocket = new TCPSocket();

	int res = tcpsocket->connectIPv4(target_address, target_port);

	if(res == -1)
	{
		printf("ERROR: could not connect TCP socket\n");
		delete tcpsocket;
		return NULL;
	}

	TCPConnection* connection = new TCPConnection(0,receive_buffer_size,send_buffer_size);
	connection->set_mp_socket(tcpsocket);

	return connection;
}

TCPSocket* ConnectionManager::openSocketTCPIPv4(std::string local_address, unsigned int local_port)
{
	TCPSocket* tcpsocket = new TCPSocket(local_address,local_port,Socket::ADDRESS_TYPE_IPV4);

	tcpsocket->bindSocket();

	return tcpsocket;
}


}
