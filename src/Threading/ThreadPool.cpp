/*
 * ThreadPool.cpp
 *
 *  Created on: 24.01.2013
 *      Author: Darius Malysiak
 */

#include "ThreadPool.h"
#include "../Toolbox.h"
#include "FunctionTask.h"

#include <pthread.h>
#include <stdio.h>

namespace Lazarus
{

ThreadPool::ThreadPool(unsigned long pool_size, unsigned int threadpool_id)
{

	//if the given thread pool id is 0, set it to 1 (alternative: create a global threadpool_id management)
	if(threadpool_id == 0)
	{
		threadpool_id = 1;
	}

	m_threadpool_id = threadpool_id;

	m_pool_size = pool_size;
	mp_queue = new Queue<Task*>();
	//mp_threads = (TaskQueueThread**)malloc(m_pool_size * sizeof(TaskQueueThread*));
	mp_threads = new TaskQueueThread*[m_pool_size];

	//update the threads references to the queue
	for(unsigned long i = 0; i< pool_size;i++)
	{
		mp_threads[i] = new TaskQueueThread(mp_queue,threadpool_id);
	}

	//create the POSIX threads
	for(unsigned long i = 0;i<m_pool_size;i++)
	{
		mp_threads[i]->start(i);
	}

}

ThreadPool::ThreadPool(const ThreadPool& pool)
{

	//indicate a copied thread pool
	m_threadpool_id = 0;

	m_pool_size = pool.getm_pool_size();
	mp_queue = new Queue<Task*>();
	//mp_threads = (TaskQueueThread**)malloc(m_pool_size * sizeof(TaskQueueThread*));
	mp_threads = new TaskQueueThread*[m_pool_size];

	//update the threads references to the queue
	for(unsigned long i = 0; i< m_pool_size;i++)
	{
		mp_threads[i] = new TaskQueueThread(mp_queue,m_threadpool_id);
	}

	//create the POSIX threads
	for(unsigned long i = 0;i<m_pool_size;i++)
	{
		mp_threads[i]->start(i);
	}
}


ThreadPool::~ThreadPool() {

	//--------------- commence a graceful shutdown

	for(unsigned long i = 0;i<m_pool_size;i++)
	{
		/*
		 * This will work despite the fact that 'mp_queue' is shared between multiple threads!
		 * Even as the enqueued dummy task in 'TaskQueueThread' 'postFlag()' method may be executed by
		 * another thread in the pool; no thread will process more than one dummy task before terminating!
		 * As each thread launches a dummy task -> okili dokili!
		 */
		mp_threads[i]->shutdown();
		delete(mp_threads[i]);
	}

	//free the memory
	delete(mp_queue);
	delete[] mp_threads;
}


unsigned long ThreadPool::getm_pool_size() const
{
	return m_pool_size;
}

void ThreadPool::scheduleTask(Task* t)
{
	this->mp_queue->enqueue(t);
}

int ThreadPool::get_m_threadpool_id() const
{
	return m_threadpool_id;
}

}
