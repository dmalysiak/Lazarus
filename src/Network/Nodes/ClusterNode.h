/*
 * MetaClusterNode.h
 *
 *  Created on: 13.06.2013
 *      Author: darius
 */

#ifndef CLUSTERNODE_H_
#define CLUSTERNODE_H_

#include "ExtendedNetworkNode.h"
#include "../Protocols/Frame.h"
#include "../../System/SystemStatus.h"
#include "../SynchronizationCallback.h"

#include <pthread.h>

namespace Lazarus {

/**
 * This object will be looked up in an indexed data structure.
 */
class ClusterNode: public ExtendedNetworkNode {
public:
	ClusterNode(Frame* frame);
	ClusterNode(ExtendedNetworkNode* node, Frame* frame);
	virtual ~ClusterNode();

	bool isCommunicating();

	Frame* get_mp_frame();
	void lockMutex();
	void unlockMutex();

	//register the callback
	void setCallback(SynchronizationCallback* com_callback);

	//get the callback
	SynchronizationCallback* getCallback();

	Lazarus::SystemStatus* get_mp_system_status();

private:
	bool m_communication_in_progress;
	Frame* mp_frame;
	pthread_mutex_t m_mutex;

	Lazarus::SystemStatus* mp_system_status;
	SynchronizationCallback* mp_com_callback;
};

} /* namespace Lazarus */
#endif /* CLUSTERNODE_H_ */
