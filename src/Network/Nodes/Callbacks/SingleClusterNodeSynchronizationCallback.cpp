/*
 * ClusterNodeCommunicationCallback.cpp
 *
 *  Created on: 07.01.2015
 *      Author: Darius Malysiak
 */

#include "SingleClusterNodeSynchronizationCallback.h"

#include "../../../Toolbox.h"

namespace Lazarus {

SingleClusterNodeSynchronizationCallback::SingleClusterNodeSynchronizationCallback(bool use_semaphore)
	: SynchronizationCallback(use_semaphore)
{
	m_done = false;
	m_sequence_complete= false;
	m_fatal_error = false;
	mp_data = NULL;

	if(use_semaphore == true)
	{
		int res = sem_init(&m_semaphore,0,0);
		if(res == -1)
		{
			printf("ERROR: could not init semaphore, will use spin-lock\n");
			m_use_semaphore = false;
		}
	}

}

SingleClusterNodeSynchronizationCallback::~SingleClusterNodeSynchronizationCallback() {

	if(m_use_semaphore == true)
	{
		int res = sem_destroy(&m_semaphore);
		if(res == -1)
		{
			printf("ERROR: could not destroy semaphore\n");
		}
	}

}

void SingleClusterNodeSynchronizationCallback::indicateError(unsigned int node_id)
{
	m_fatal_error = true;
}

void SingleClusterNodeSynchronizationCallback::indicateCompleteSequence(unsigned int node_id)
{
	m_sequence_complete= true;
}

void SingleClusterNodeSynchronizationCallback::reset()
{
	m_done = false;
	m_sequence_complete= false;
	m_fatal_error = false;
	mp_data = NULL;

	//if required reset the semaphore
	if(m_use_semaphore == true)
	{
		int res = sem_destroy(&m_semaphore);
		if(res == -1)
		{
			printf("ERROR: could not destroy semaphore\n");
		}

		res = sem_init(&m_semaphore,0,0);
		if(res == -1)
		{
			printf("ERROR: could not init semaphore, will use spin-lock\n");
			m_use_semaphore = false;
		}
	}
}

void SingleClusterNodeSynchronizationCallback::unlock(unsigned int node_id)
{
	//unlock via exiting the spin lock
	if(m_use_semaphore == false)
	{
		m_done = true;
	}
	else
	{
		int res = sem_post(&m_semaphore);
		if(res == -1)
		{
			printf("ERROR: could not increment semaphore\n");
		}
	}
}

void SingleClusterNodeSynchronizationCallback::lock(unsigned int node_id)
{

	if(m_use_semaphore == false)
	{
		/*Now wait until the sequence ended, this will be indicated by 'callback.m_done == true' which will be
		set by some framehandler. A spinlock may seem to be a wasteful approach, yet the average waiting time
		is expected to be very short.
		This method will introduce a 50us latency!
		*/
		while(m_done == false)
		{
			Lazarus::Toolbox::sleep_us(50);
		}
	}
	else
	{
		/**
		 * Keep in mind that a mutex wouldn't work:
		 * The frame is send out, the sending thread is delayed, the response comes in, the frame handler thread unlocks,
		 * the sending thread continues, he calls mutex_lock -> dead lock!
		 * Only the semaphore can indicate a previous unlock, just as with the boolean variable.
		 */
		int res = sem_wait(&m_semaphore);
		if(res == -1)
		{
			printf("ERROR: could not decrement semaphore\n");
		}
	}

}

void SingleClusterNodeSynchronizationCallback::deliverData(Lazarus::Buffer* data, unsigned int node_id)
{
	mp_data = data;
}


} /* namespace Lazarus */
