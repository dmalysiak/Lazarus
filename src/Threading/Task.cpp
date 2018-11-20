/*
 * Task.cpp
 *
 *  Created on: 28.01.2013
 *      Author: Darius Malysiak
 */

#include "Task.h"

#include <stdio.h>

namespace Lazarus
{

Task::Task() {

	m_run_callback = false;
	mp_c_queue = NULL;
}

Task::~Task() {
	DELETE_NULL_CHECKING(mp_c_queue);
}

bool Task::getm_run_callback() const
{
	return m_run_callback;
}

void Task::unregisterCallback()
{
	DELETE_NULL_CHECKING(mp_c_queue);
	m_run_callback = false;
}

/**
 * Each Task has a callback queue. The elements in the queue might be executed at arbitrary positions in time.
 * Any existing queue will be deleted (due to the queue destructor this also deletes all enqueued actions).
 * The actual execution order depends on the class which inherits from this class.
 */
void Task::registerCallbackQueue( CallbackQueue* c_queue )
{
	DELETE_NULL_CHECKING(mp_c_queue);

	mp_c_queue = c_queue;
	m_run_callback = true;
}

/**
 * The decision if the queue elements should be deleted is up to the behavior of 'runElements'.
 * One can simply assign this instance a queue which overrides the default behavior of 'CallbackQueue.runElements(.)'.
 */
void Task::callback(Thread* executing_thread)
{
	if(m_run_callback == true)
	{
		mp_c_queue->runElements(executing_thread);
	}

	//DELETE_NULL_CHECKING(mp_c_queue);
	//m_run_callback = false;
}

}
