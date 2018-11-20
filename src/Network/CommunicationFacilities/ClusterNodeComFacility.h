/*
 * ClusterNodeComFacility.h
 *
 *  Created on: 17.06.2013
 *      Author: darius
 */

#ifndef CLUSTERNODECOMFACILITY_H_
#define CLUSTERNODECOMFACILITY_H_

#include "../CommunicationFacility.h"
#include "../Nodes/ClusterNode.h"
#include "../../DataStructures/ThreadSafeUnorderedMap.h"

#include <pthread.h>

namespace Lazarus {

class ClusterNodeComFacility: public CommunicationFacility {
public:
	ClusterNodeComFacility();
	virtual ~ClusterNodeComFacility();

	unsigned int acquire_free_node_id(unsigned int desired_id);

	void addNode(ClusterNode* node);
	virtual void removeNode(unsigned int node_id);
	ClusterNode* getNode(unsigned int node_id);

	/**
	 * This method will send a request to the given node. The request must be appropriately created by the caller,
	 * e.g. serializing all required headers and data. The call will attempt to lock the node for other communication requests.
	 * Furthermore it will block until an answer from the node/client has been received.
	 * Returns 0 in case of success, -1 otherwise.
	 */
	virtual int sendRequestToNode(unsigned int node_id, Lazarus::Buffer* request, SynchronizationCallback* callback);

	/**
	 * This method will send a request to the given node. The request must be appropriately created by the caller,
	 * e.g. serializing all required headers and data. The call >won't attempt to lock< the node for other communication requests!!
	 * This is useful for communication sequences more complex than a simple ping-pong! I.e. it should be used after a call
	 * to 'sendRequestToNode(.)' has already locked the node during the first request send.
	 * Furthermore it will block until an answer from the node/client has been received.
	 * Returns 0 in case of success, -1 otherwise.
	 */
	virtual int sendRequestToNodeNL(unsigned int node_id, Lazarus::Buffer* request, SynchronizationCallback* callback);

	/**
	 * This method will send a request to the given nodes. The request must be appropriately created by the caller,
	 * e.g. serializing all required headers and data. The call will attempt lock the nodes for other communication requests.
	 * Furthermore it will block until an answer from all nodes/clients has been received.
	 * Returns 0 in case of success, -1 otherwise.
	 */
	virtual int sendRequestToMultipleNodes(Lazarus::FastKTuple<unsigned int>& node_ids, Lazarus::Buffer* request, SynchronizationCallback* callback);


	unsigned int getNodeCount();
	Lazarus::FastKTuple<unsigned int>* getNodeIDs();

protected:
	Lazarus::ThreadSafeUnorderedMap<unsigned int,ClusterNode*>* mp_active_nodes;
	std::unordered_map<unsigned int, unsigned int> m_used_node_ids;

	unsigned int m_next_free_id;
	pthread_mutex_t m_mutex;

	/**
	 * Returns false if the id is not in use
	 */
	bool check_id_usage(unsigned int id);

};

} /* namespace Lazarus */
#endif /* CLUSTERNODECOMFACILITY_H_ */
