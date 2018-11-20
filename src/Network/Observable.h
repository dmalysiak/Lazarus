/*
 * Socket.h
 *
 *  Created on: 28.02.2013
 *      Author: Darius Malysiak
 */

#ifndef Lazarus_OBSERVABLE_H_
#define Lazarus_OBSERVABLE_H_

#include "../BaseObject.h"
#include "TCPServer/ReportServer.h"


#include <string>

namespace Lazarus
{

class Observable: public Lazarus::BaseObject {
public:

	virtual ~Observable(){}

	Observable()
	{

	}

	Observable(int timeout, const std::string& local_address, unsigned int local_port, const std::string& local_device="")
	{
		ReportServer::instance(timeout,local_address,local_port,local_device)->start();
	}

};

}

#endif /* SOCKET_H_ */
