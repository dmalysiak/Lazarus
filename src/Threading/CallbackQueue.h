/*
 * CallbackQueue.h
 *
 *  Created on: 05.02.2013
 *      Author: Darius Malysiak
 */

#ifndef CALLBACKQUEUE_H_
#define CALLBACKQUEUE_H_

#include "../BaseObject.h"
#include "Callback.h"
#include "../DataStructures/Queue.h"
#include "Thread.h"

/*
 * The CallbackQueue is not designed for multi-threading!
 * It must be ensured that no such object is handled by more than one thread!
 *
 * The CallbackQueue takes absolute ownership over the callback objects, i.e. it
 * deletes them after calling the corresponding callback method. Additionally, during its destruction,
 * the CallbackQueue also disposes any not invoked callback object (of course without calling the callback method).
 */

namespace Lazarus
{

class CallbackQueue: public BaseObject {
public:
	CallbackQueue();
	virtual ~CallbackQueue();

	virtual void runElements(Thread* executing_thread = NULL);

	void enqueueCallback(Callback* callback_object);

	CLASS_RTTI(Lazarus::CallbackQueue)

protected:
	Queue<Callback*>* mp_queue;
};

}

#endif /* CALLBACKQUEUE_H_ */
