/*
 * CallbackServerComFacility.h
 *
 *  Created on: 17.06.2013
 *      Author: darius
 */

#ifndef CALLBACKSERVERCOMFACILITY_H_
#define CALLBACKSERVERCOMFACILITY_H_

#include "../CommunicationFacility.h"
#include "../Nodes/CallbackNode.h"
#include "../../DataStructures/ThreadSafeUnorderedMap.h"
#include "../../DataStructures/KTuple.h"

namespace Lazarus {

class CallbackServerComFacility: public CommunicationFacility {
public:
	CallbackServerComFacility();
	virtual ~CallbackServerComFacility();

	//unsigned int acquire_free_node_id(unsigned int desired_id);

	void addNode(CallbackNode* node);
	virtual void removeNode(unsigned int node_id);
	CallbackNode* getNode(unsigned int node_id);

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
	 * This method will send a request to the given node. The request must be appropriately created by the caller,
	 * e.g. serializing all required headers and data. This method will only lock the node until the request was
	 * successfully send.
	 * Returns 0 in case of success, -1 otherwise.
	 */
	virtual int sendRequestToNodeAsync(unsigned int node_id, Lazarus::Buffer* request);


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
	Lazarus::ThreadSafeUnorderedMap<unsigned int,CallbackNode*>* mp_active_nodes;
	std::unordered_map<unsigned int, unsigned int> m_used_node_ids;

	unsigned int m_next_free_id;
	pthread_mutex_t m_mutex;

	/**
	 * Returns false if the id is not in use
	 */
	bool check_id_usage(unsigned int id);

};

} /* namespace Lazarus */
#endif /* CALLBACKSERVERCOMFACILITY_H_ */
