/*
 * ClusterLibServer.h
 *
 *  Created on: 20.03.2013
 *      Author: Darius Malysiak
 */

#ifndef CLUSTERLIBSERVER_H_
#define CLUSTERLIBSERVER_H_

#include "../TCPServer.h"

#include <atomic>

namespace Lazarus
{

class ClusterLibServer: public TCPServer
{

public:
	//careful regarding the connection_threads number, there are in total connection_treads+1 threads (one for the listener method)
	//the threads are started during the construction of a TCPServer
	//max_connections must be a multiple of connection threads

	ClusterLibServer(std::string local_address, unsigned int local_port, unsigned int max_connections,
			unsigned int worker_threads);
	virtual ~ClusterLibServer();

	//this method should handle incoming connection >requests<
	virtual void listener_method();

	/**
	 * Indicates whether the listener thread has started.
	 * */
	bool isActive();

protected:
	long int findNextBestWorkerThread();
	std::atomic_bool m_is_active;
};

}

#endif /* CLUSTERLIBSERVER_H_ */
