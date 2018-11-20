/*
 * SynchronizationCallback.h
 *
 *  Created on: 07.01.2015
 *      Author: Darius Malysiak
 */

#ifndef SYNCHRONIZATIONCALLBACK_H_
#define SYNCHRONIZATIONCALLBACK_H_

#include "../BaseObject.h"
#include "../Buffer.h"

namespace Lazarus {

class SynchronizationCallback : public Lazarus::BaseObject {
public:
	/**
	 * If 'use_semaphore' is set to true, the lock method will use a semaphore to wait for an unlock.
	 * Otherwise a spin-lock is used.
	 */
	SynchronizationCallback(bool use_semaphore = false);
	virtual ~SynchronizationCallback();

	/**
	 * Unlocks the waiting.
	 */
	virtual void unlock(unsigned int node_id) = 0;

	/**
	 * When a thread needs to wait, he will call this method, only after a call to 'unlock()' will he continue!
	 */
	virtual void lock(unsigned int node_id) = 0;

	/**
	 * If the node encounters an error, it will call this method.
	 */
	virtual void indicateError(unsigned int node_id) = 0;

	/**
	 * If the node finishes a complete sequence, it will call this method.
	 */
	virtual void indicateCompleteSequence(unsigned int node_id) = 0;

	/**
	 * The node will deliver its data via this method. This method will never claim ownership over the data.
	 */
	virtual void deliverData(Lazarus::Buffer* data, unsigned int node_id) = 0;

	/**
	 * This will reset the state of object, call this method before using the object again.
	 */
	virtual void reset() = 0;

protected:
	bool m_use_semaphore;

};

} /* namespace Lazarus */

#endif /* SYNCHRONIZATIONCALLBACK_H_ */
