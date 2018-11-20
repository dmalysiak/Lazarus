/*
 * Thread.cpp
 *
 *  Created on: 24.01.2013
 *      Author: Darius Malysiak
 */

#include "Thread.h"
#include "../Toolbox.h"

#include <string.h>
#include <stdio.h>

namespace Lazarus
{

Thread::Thread(int threadpool_id) {
	m_tid = 0;
	m_thread_state = THREAD_STATE_UNINITIALIZED;
	m_system_thread_id = 0;
	m_local_thread_id = 0;
	m_shutdown_flag = false;
	m_threadpool_thread = false;
	m_thread_active = 0;

	m_parent_thread_pool_id = threadpool_id;
	//member of a thread pool
	if(threadpool_id != -1)
	{
		m_threadpool_thread = true;
	}
	else //not a member
	{
		m_threadpool_thread = false;
	}

	m_thread_state = THREAD_STATE_IDLE;

	m_thread_self_terminated = false;

	pthread_mutex_init(&m_mutex,NULL);
}

Thread::Thread(const Thread& t) {
	m_tid = 0;
	m_thread_state = THREAD_STATE_UNINITIALIZED;
	m_system_thread_id = 0;
	m_local_thread_id = 0;
	m_shutdown_flag = false;
	m_threadpool_thread = false;

	m_parent_thread_pool_id = -1;
	//a copied thread is not a member of a thread pool
	m_threadpool_thread = false;

	m_thread_self_terminated = false;
	pthread_mutex_init(&m_mutex,NULL);
}

/**
* Use 'shutdown'/'shutdownJoin' before deleting a running thread!
* If not, this destructor won't stop a running thread! (otherwise the system consistency would be broken as
* 'preFlag'/'postFlag' would be called after a child classes destructor, this would make both of these overridden
* methods useless!).
* A simple way to ensure this is by calling 'shutdown()' at the >beginning< of a child objects destructor.
*/
Thread::~Thread()
{
	pthread_mutex_destroy(&m_mutex);
	pthread_attr_destroy(&m_thread_attr);//free the object
}

pthread_t Thread::getm_system_thread_id() const
{
	return m_system_thread_id;
}

unsigned int Thread::getm_local_thread_id() const
{
	return m_local_thread_id;
}

void Thread::preFlag()
{

}

void Thread::postFlag()
{

}

void Thread::shutdown()
{
	shutdownJoin();
}

void Thread::setAffinity(FastKTuple<unsigned int>& cpus)
{
	cpu_set_t cpuset;

	/* Set affinity mask to include CPUs 0 to 7 */
	CPU_ZERO(&cpuset);

	for(int j = 0; j < cpus.getm_size(); j++)
	{
	   const unsigned int k = cpus[j];
	   CPU_SET(k, &cpuset);
	}

	int s = pthread_setaffinity_np(m_system_thread_id, sizeof(cpu_set_t), &cpuset);
	if (s != 0)
	{
	   printf("ERROR: could not set affinity\n");
	}
}

FastKTuple<unsigned int> Thread::getAffinity()
{
	cpu_set_t cpuset;
	FastKTuple<unsigned int> cpus;

	int s = pthread_getaffinity_np(m_system_thread_id, sizeof(cpu_set_t), &cpuset);
	if (s != 0)
	{
	   printf("ERROR: could not get affinity\n");
	}

	for(int j = 0; j < CPU_SETSIZE; j++)
	{
	   if(CPU_ISSET(j, &cpuset))
	   {
		   cpus.push(j);
	   }
	}

	return cpus;
}

void Thread::shutdownJoin()
{
	//don't shutdown if the thread isn't active (this checks if the thread object has started or terminated the system thread)
	if(m_thread_state == THREAD_STATE_IDLE || m_thread_state == THREAD_STATE_STOPPED)
	{
		//printf("ERROR: thread not running!\n");
		return;
	}

	/**
	 * in case the thread terminated itself, skip the waiting.
	 * >>>Keep in mind that the thread object is managed by a single context/thread<<<, if the thread itself exited the
	 * 'run()' method it won't update 'm_thread_state', which would still indicate a running thread!
	 * Thus we check if the thread may have terminated itself.
	 */
	if(m_thread_self_terminated != true)// if it hasn't terminated itself: we are still waiting for it to start up
	{
		//this spin lock will wait until the thread has actually started
		while(m_thread_active == false)
		{
			Toolbox::sleep_ms(100);

			/*
			 * this is required due to the following possible situation:
			 * the thread is started and afterwards 'join()' is called, 'm_thread_self_terminated' evaluates
			 * to false as the thread still runs, yet before entering the while loop the thread terminates itself!
			 * This termination will set 'm_thread_active' to false and a deadlock for the caller occurs as he
			 * waits for an already dead thread to start up.
			 */
			if(m_thread_self_terminated == true)
			{
				break;
			}
		}
	}

	//if thread still runs or has self terminated
	if(m_thread_active == true || m_thread_self_terminated == true)
	{
		//only if the thread really runs:
		if(m_thread_active == true)
		{
			preFlag();
			m_shutdown_flag = true;
			postFlag();
		}
		else
		{
			m_shutdown_flag = true;
		}

		pthread_join(m_system_thread_id,NULL);
	}

	pthread_attr_destroy(&m_thread_attr);

	m_thread_state = THREAD_STATE_STOPPED;
	m_thread_self_terminated = false;
}

void Thread::printThreadInfo() const
{
	if(m_threadpool_thread == true)
	{
		printf("**THREAD INFORMATION **\n **Thread system id(pointer value?): %ld | Thread local id: %u || Belonging to thread pool %d**\n",
				m_system_thread_id,m_local_thread_id,m_parent_thread_pool_id);
	}
	else
	{
		printf("**THREAD INFORMATION **\n **Thread system id(pointer value?): %ld | Thread local id: %u **\n",m_system_thread_id,m_local_thread_id);
	}
}

void Thread::updateResourceInfo()
{
	getrusage(RUSAGE_THREAD,&m_thread_resources);
}

void Thread::start(unsigned int local_thread_id)
{
	//don't start if the thread already runs
	if(m_thread_state != THREAD_STATE_IDLE && m_thread_state != THREAD_STATE_STOPPED)
	{
		printf("ERROR: thread already started! %d\n",m_thread_state);
		return;
	}

	m_local_thread_id = local_thread_id;
	m_shutdown_flag = false;

	//setup the thread parameters
	pthread_attr_init(&m_thread_attr);
	pthread_attr_setdetachstate(&m_thread_attr,PTHREAD_CREATE_JOINABLE);

	//create the POSIX threads
	struct ThreadParameterContainer* thread_params = create_C_struct_ThreadParameterContainer();

	//pack the parameters
	thread_params->thread_object = this;

	pthread_create(&m_system_thread_id,&m_thread_attr,&Thread::runFromStaticContext,thread_params);

	m_thread_state = THREAD_STATE_RUNNING;
}

void* Thread::runFromStaticContext(void* thread_params)
{
	//unpack the parameters
	struct ThreadParameterContainer* parameters = (ThreadParameterContainer*)thread_params;

	Thread* thread_object = parameters->thread_object;

	thread_object->m_tid = syscall(SYS_gettid);

	thread_object->run_();

	free(parameters);

	//end the thread
	return NULL;
}

void Thread::join()
{
	/**
	 * in case the thread terminated itself, skip the waiting.
	 * >>>Keep in mind that the thread object is managed by a single context/thread<<<, if the thread itself exited the
	 * 'run()' method, the spin-lock would wait indefinitely (the caller would assume that the thread still runs
	 * as he calls 'join()').
	 */
	if(m_thread_self_terminated != true)// if it hasn't terminated itself: we are still waiting for it to start up
	{
		//this spin lock will wait until the thread has actually started
		while(m_thread_active == false)
		{
			Toolbox::sleep_ms(100);

			/*
			 * this is required due to the following possible situation:
			 * the thread is started and afterwards 'join()' is called, 'm_thread_self_terminated' evaluates
			 * to false as the thread still runs, yet before entering the while loop the thread terminates itself!
			 * This termination will set 'm_thread_active' to false and a deadlock for the caller occurs as he
			 * waits for an already dead thread to start up.
			 */
			if(m_thread_self_terminated == true)
			{
				break;
			}
		}
	}

	pthread_join(m_system_thread_id,NULL);

	pthread_attr_destroy(&m_thread_attr);//free the object

}

void Thread::run_()
{
	pthread_mutex_lock(&m_mutex);
	m_thread_active = true;
	run();
	this->m_thread_self_terminated=true;
	m_thread_active = false;
	pthread_attr_destroy(&m_thread_attr);//free the object
	pthread_mutex_unlock(&m_mutex);
}

struct ThreadParameterContainer* create_C_struct_ThreadParameterContainer()
{
	return (struct ThreadParameterContainer*)calloc(1,sizeof(struct ThreadParameterContainer));
}

}

