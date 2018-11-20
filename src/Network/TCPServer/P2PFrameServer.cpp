/*
 * P2PServer.cpp
 *
 *  Created on: 20.03.2013
 *      Author: Darius Malysiak
 */

#include "P2PFrameServer.h"
#include "../Protocols/ClusterLib/ClusterLibFrame.h"
#include "../../Toolbox.h"

namespace Lazarus {

P2PFrameServer::P2PFrameServer(const std::string& local_address, unsigned int local_port, const std::string& local_device) :
		P2PServer(local_address,local_port,local_device)
{

}

P2PFrameServer::~P2PFrameServer()
{

}

int P2PFrameServer::sendFrame(const Lazarus::Buffer& data, int total_timeout, int timeout)
{
	if(mp_client_connection == NULL)
	{
		ERROR("Connection not set")
		return -1;
	}

	if(mp_frame == NULL)
		mp_frame = new ClusterLibFrame(mp_client_connection);

	return mp_frame->sendFrame((Lazarus::Buffer*)&data, total_timeout, timeout);
}

Lazarus::Buffer* P2PFrameServer::receiveFrame(int total_timeout, int timeout)
{
	if(mp_client_connection == NULL)
	{
		ERROR("Connection not set")
		return NULL;
	}

	if(mp_frame == NULL)
		mp_frame = new ClusterLibFrame(mp_client_connection);

	return mp_frame->readFrame(total_timeout, timeout);
}

}

