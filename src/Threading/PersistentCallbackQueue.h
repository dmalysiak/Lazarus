/*
 * PersistentCallbackQueue.h
 *
 *  Created on: 05.02.2013
 *      Author: Darius Malysiak
 */

#ifndef PERSISTENTCALLBACKQUEUE_H_
#define PERSISTENTCALLBACKQUEUE_H_

#include "CallbackQueue.h"
#include "Callback.h"
#include "../DataStructures/Queue.h"
#include "Thread.h"

/*
 * The PersistentCallbackQueue is not designed for multi-threading!
 * It must be ensured that no such object is handled by more than one thread!
 *
 * The PersistentCallbackQueue takes absolute ownership over the callback objects, unlike the CallbackQueue it
 * does not delete them after calling the corresponding callback method. Additionally, during its destruction,
 * the PersistentCallbackQueue also disposes any not invoked callback object (of course without calling the callback method).
 */

namespace Lazarus
{

class PersistentCallbackQueue: public CallbackQueue {
public:
	PersistentCallbackQueue();
	virtual ~PersistentCallbackQueue();

	virtual void runElements(Thread* executing_thread = NULL);

	CLASS_RTTI(Lazarus::PersistentCallbackQueue)
};

}

#endif /* PERSISTENTCALLBACKQUEUE_H_ */
