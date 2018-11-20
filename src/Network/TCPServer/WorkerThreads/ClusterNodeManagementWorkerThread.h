/*
 * ClusterNodeManagementWorkerThread.h
 *
 *  Created on: 12.06.2013
 *      Author: darius
 */

#ifndef CLUSTERNODEMANAGEMENTWORKERTHREAD_H_
#define CLUSTERNODEMANAGEMENTWORKERTHREAD_H_

#include "../../TCPServerWorkerThread.h"
#include "../../CommunicationFacilities/ClusterNodeComFacility.h"

namespace Lazarus{

class ClusterNodeManagementWorkerThread: public TCPServerWorkerThread {
public:
	ClusterNodeManagementWorkerThread(unsigned int max_connections,ClusterNodeComFacility* com_facility);
	virtual ~ClusterNodeManagementWorkerThread();

	virtual void addConnection(TCPSocket* tcpsocket);

private:
	ClusterNodeComFacility* mp_cn_node_facility;
};

}

#endif /* CLUSTERNODEMANAGEMENTWORKERTHREAD_H_ */
