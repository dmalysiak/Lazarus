/*
 * TaskQueueThread.h
 *
 *  Created on: 28.01.2013
 *      Author: Darius Malysiak
 */

#ifndef TASKQUEUETHREAD_H_
#define TASKQUEUETHREAD_H_

#include "Thread.h"
#include "../DataStructures/Queue.h"
#include "Task.h"

namespace Lazarus
{

/**
 * This class will process any available task in the given queue, after a task has been processed it will be deleted!
 */
class TaskQueueThread: public Thread {
public:
	/**
	 * Does not take ownership of 'queue'.
	 */
	TaskQueueThread(Queue<Task*>* queue);

	/**
	 * Does not take ownership of 'queue'.
	 */
	TaskQueueThread(Queue<Task*>* queue, int threadpool_id = -1);
	virtual ~TaskQueueThread();

	void run();

	//void* run(pthread_t* system_thread_id, unsigned int local_thread_id, ThreadPool* tp);
	void setmp_queue(Queue<Task*>* queue);


	virtual void postFlag();

	CLASS_RTTI(Lazarus::TaskQueueThread)


	static void dummyThreadTask(Thread*, void* data);

private:
	Queue<Task*>* mp_queue;
};

}

#endif /* TASKQUEUETHREAD_H_ */
