/*
 * ThreadPool.h
 *
 *  Created on: 24.01.2013
 *      Author: Darius Malysiak
 */

#ifndef THREADPOOL_H_
#define THREADPOOL_H_

#include "../BaseObject.h"
#include "TaskQueueThread.h"
#include "../DataStructures/Queue.h"

namespace Lazarus
{

/**
 * This class will process the enqueued tasks through multiple threads, after a task has been processed it will be deleted!
 * Deleting the threadpool will commence a graceful shutdown of all associated threads.
 */
class ThreadPool: public BaseObject {
public:
	/*
	 * a pool id of 0 indicates a thread pool created by a the constructor
	 */
	ThreadPool(unsigned long pool_size, unsigned int threadpool_id = 1);

	/*
	 * creates a new thread pool with the size of the original thread pool and an id of 0
	 */
	ThreadPool(const ThreadPool& pool);

	virtual ~ThreadPool();

	/**
	 * Once the task has been enqueued the thread pool takes ownership and will delete the task after execution
	 */
	void scheduleTask(Task* t);
	int get_m_threadpool_id() const;
	unsigned long getm_pool_size() const;


	CLASS_RTTI(Lazarus::ThreadPool)

private:
	int m_threadpool_id;
	unsigned long m_pool_size;
	TaskQueueThread** mp_threads;
	Queue<Task*>* mp_queue;

};

}


#endif /* THREADPOOL_H_ */
