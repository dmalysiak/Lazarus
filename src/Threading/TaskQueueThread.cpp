/*
 * TaskQueueThread.cpp
 *
 *  Created on: 28.01.2013
 *      Author: Darius Malysiak
 */

#include "TaskQueueThread.h"
#include "FunctionTask.h"

#include <stdio.h>

namespace Lazarus
{

TaskQueueThread::TaskQueueThread(Queue<Task*>* queue) : Thread(-1){

	mp_queue = queue;

}

TaskQueueThread::TaskQueueThread(Queue<Task*>* queue, int threadpool_id) : Thread(threadpool_id)
{

	mp_queue = queue;

}

TaskQueueThread::~TaskQueueThread() {
	// TODO Auto-generated destructor stub
}


void TaskQueueThread::dummyThreadTask(Thread*, void* data)
{
	printf("DUMMY TASK\n");
	return;
}

void TaskQueueThread::postFlag()
{
	//enqueue a dummy task before setting the shutdown flag
	FunctionTask<void,void*>* ftask = new FunctionTask<void,void*>();
	ftask->setmp_function( &TaskQueueThread::dummyThreadTask );
	mp_queue->enqueue(ftask);
}

void TaskQueueThread::run() {

	//-------------------- begin the processing of tasks
	Task* task = NULL;

	//check if a task is available
	while(!m_shutdown_flag)
	{
		//printf("Thread %ld waiting for tasks\n",m_system_thread_id);
		task = mp_queue->dequeueBlockingOnEmpty();
		//printf("Thread %ld found a task\n",m_system_thread_id);
		task->runTask(this);

		delete task;
	}


}

}

