/*
 * Task.h
 *
 *  Created on: 28.01.2013
 *      Author: Darius Malysiak
 */

#ifndef TASK_H_
#define TASK_H_

#include "../BaseObject.h"
#include "CallbackQueue.h"
#include "Thread.h"

/*
 * A task object takes absolute ownership of the assigned callback queue, i.e. in case of a new assignment
 * with an already existing queue or in case of an unregistration, the existing queue will be disposed.
 * This class is not thread safe!
 */

namespace Lazarus
{

class Task: public BaseObject {
public:
	Task();
	virtual ~Task();

	/**
	 * The run method should implement the desired functionality.
	 */
	virtual void runTask(Thread* executing_thread = NULL) = 0;
	bool getm_run_callback() const;
	void unregisterCallback();
	void registerCallbackQueue( CallbackQueue* c_queue );

	/**
	 * This method will process all elements in the queue.
	 * The decision if the queue elements should be deleted is up to the behavior of 'CallbackQueue.runElements(.)'.
	 * One can simply assign (registerCallbackQueue) this instance a queue which overrides the default behavior of
	 * 'CallbackQueue.runElements(.)', see e.g. 'PersistentCallbackQueue'.
	 */
	void callback(Thread* executing_thread = NULL);

	CLASS_RTTI(Lazarus::Task)

private:
	bool m_run_callback;
	CallbackQueue* mp_c_queue;

};

struct NO_STATIC_THREAD_REF___
{};


}

#endif /* TASK_H_ */
