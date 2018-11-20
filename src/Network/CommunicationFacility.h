/*
 * CommunicationFacility.h
 *
 *  Created on: 17.06.2013
 *      Author: darius
 */

#ifndef COMMUNICATIONFACILITY_H_
#define COMMUNICATIONFACILITY_H_

#include "../BaseObject.h"
#include "../Buffer.h"
#include "SynchronizationCallback.h"
#include "../DataStructures/FastKTuple.h"

namespace Lazarus {

class CommunicationFacility: public Lazarus::BaseObject {
public:
	CommunicationFacility();
	virtual ~CommunicationFacility();

	virtual void removeNode(unsigned int node_id);

	/**
	 * This method will send a request to the given node. The request must be appropriately created by the caller,
	 * e.g. serializing all required headers and data. The call will attempt lock the node for other communication requests.
	 * Furthermore it will block until an answer from the node/client has been received.
	 * Returns 0 in case of success, -1 otherwise.
	 */
	virtual int sendRequestToNode(unsigned int node_id, Lazarus::Buffer* request, SynchronizationCallback* callback);

	/**
	 * This method will send a request to the given nodes. The request must be appropriately created by the caller,
	 * e.g. serializing all required headers and data. The call will attempt lock the nodes for other communication requests.
	 * Furthermore it will block until an answer from all nodes/clients has been received.
	 * Returns 0 in case of success, -1 otherwise.
	 */
	virtual int sendRequestToMultipleNodes(Lazarus::FastKTuple<unsigned int>& node_ids, Lazarus::Buffer* request, SynchronizationCallback* callback);

	/**
	 * This method will send a request to the given node. The request must be appropriately created by the caller,
	 * e.g. serializing all required headers and data. The call >won't attempt to lock< the node for other communication requests!!
	 * This is useful for communication sequences more complex than a simple ping-pong! I.e. it should be used after a call
	 * to 'sendRequestToNode(.)' has already locked the node during the first request send.
	 * Furthermore it will block until an answer from the node/client has been received.
	 * Returns 0 in case of success, -1 otherwise.
	 */
	virtual int sendRequestToNodeNL(unsigned int node_id, Lazarus::Buffer* request, SynchronizationCallback* callback);
};

} /* namespace Lazarus */
#endif /* COMMUNICATIONFACILITY_H_ */
