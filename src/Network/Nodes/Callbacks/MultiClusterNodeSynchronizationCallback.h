/*
 * MultiClusterNodeSynchronizationCallback.h
 *
 *  Created on: 07.01.2015
 *      Author: Darius Malysiak
 */

#ifndef MULTICLUSTERNODESYNCHRONIZATIONCALLBACK_H_
#define MULTICLUSTERNODESYNCHRONIZATIONCALLBACK_H_

#include "../../SynchronizationCallback.h"
#include "../../../DataStructures/FastKTuple.h"
#include "../../../DataStructures/FastVectorN.h"

#include <semaphore.h>
#include <atomic>
#include <unordered_map>

namespace Lazarus {

/**
 * A mere communication container for synchronizing calls between external threads and the frame handlers.
 * To be more specific: >multiple< frame handler! Yet it can be used for single frame handlers as well,
 * but considering the performance drop due to more complicated data structures, one should take another
 * SynchronizationCallback class!
 * This class should be used only for communication tasks in which the answer is expected after a long time.
 * In such a case also use semaphores in order to wait!
 * Another reason in which this class could be useful is a >>large<< number of heterogenous nodes.
 * Otherwise consider sending the requests sequentially!! The overhead of this class is tremendous for small numbers
 * of clients or fast communication tasks!!
 */

class MultiClusterNodeSynchronizationCallback: public SynchronizationCallback {
public:

	/**
	 * If 'use_semaphore' is set to true, the lock method will use a semaphore to wait for an unlock.
	 * Otherwise a spin-lock is used.
	 * The Tuple of node ids is required in order to compute an internal lookup table!
	 */
	MultiClusterNodeSynchronizationCallback(Lazarus::FastKTuple<unsigned int>& node_ids, bool use_semaphore = false);
	virtual ~MultiClusterNodeSynchronizationCallback();


	/**
	 * Unlocks the waiting.
	 */
	virtual void unlock(unsigned int node_id);

	/**
	 * When a thread needs to wait, he will call this method, only after a call to 'unlock()' will he continue!
	 * Actually the value of node_id has no meaning in the context of this class.
	 */
	virtual void lock(unsigned int node_id);

	/**
	 * If the node encounters an error, it will call this method.
	 */
	virtual void indicateError(unsigned int node_id);

	/**
	 * The node will deliver its data via this method. This method will never claim ownership over the data.
	 */
	virtual void deliverData(Lazarus::Buffer* data, unsigned int node_id);

	/**
	 * If a node finishes a sequence it will call this method.
	 */
	virtual void indicateCompleteSequence(unsigned int node_id);

	/**
	 * This will reset the objects internal state, call it before using the object again.
	 */
	void reset();


	//this buffer will contain any returned data, in the form (nodeID, buffer)
	Lazarus::FastKTuple< Lazarus::FastVector2<unsigned int, Lazarus::Buffer*> >* mp_data;
	//used to indicate that after finished processing also the sequence was completed,
	//bool is ok as we check it after receiving all answers
	Lazarus::FastKTuple< Lazarus::FastVector2<unsigned int, bool> >* mp_sequence_complete;
	//used to indicate a fatal error within the frame handler usually this means a torn down connection
	//bool is ok as we check it after receiving all answers
	Lazarus::FastKTuple< Lazarus::FastVector2<unsigned int, bool> >* mp_fatal_error;

private:
	//used to indicate a finished processing
	std::atomic_int m_done;

	sem_t m_semaphore;

	unsigned int m_node_count;

	/*
	 * This lookup map will be used to find the correct index in the internal FastKTuples, otherwise we would need
	 * KTuples but this would prevent parallel accesses of the frame handlers!
	 */
	std::unordered_map<unsigned int, unsigned int> m_lookup_map;
};

} /* namespace Lazarus */

#endif /* MULTICLUSTERNODESYNCHRONIZATIONCALLBACK_H_ */
