/*
 * SingleClusterNodeSynchronizationCallback.h
 *
 *  Created on: 07.01.2015
 *      Author: Darius Malysiak
 */

#ifndef SINGLECLUSTERNODESYNCHRONIZATIONCALLBACK_H_
#define SINGLECLUSTERNODESYNCHRONIZATIONCALLBACK_H_

#include "../../SynchronizationCallback.h"

#include <semaphore.h>
#include <atomic>

namespace Lazarus {

/**
 * A mere communication container for synchronizing calls between external threads and the frame handlers.
 * To be more specific: exactly >one< frame handler!
 */

class SingleClusterNodeSynchronizationCallback: public SynchronizationCallback {
public:

	/**
	 * If 'use_semaphore' is set to true, the lock method will use a semaphore to wait for an unlock.
	 * Otherwise a spin-lock is used.
	 */
	SingleClusterNodeSynchronizationCallback(bool use_semaphore = false);
	virtual ~SingleClusterNodeSynchronizationCallback();


	/**
	 * Unlocks the waiting.
	 */
	virtual void unlock(unsigned int node_id);

	/**
	 * When a thread needs to wait, he will call this method, only after a call to 'unlock()' will he continue!
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


	//this buffer will contain any returned data
	Lazarus::Buffer* mp_data;
	//used to indicate that after finished processing also the sequence was completed
	std::atomic_bool m_sequence_complete;
	//used to indicate a fatal error within the frame handler usually this means a torn down connection
	std::atomic_bool m_fatal_error;

private:
	//used to indicate a finished processing
	std::atomic_bool m_done;

	sem_t m_semaphore;
};

} /* namespace Lazarus */

#endif /* SINGLECLUSTERNODESYNCHRONIZATIONCALLBACK_H_ */
