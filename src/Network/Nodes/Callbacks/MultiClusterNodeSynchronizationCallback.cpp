/*
 * ClusterNodeCommunicationCallback.cpp
 *
 *  Created on: 07.01.2015
 *      Author: Darius Malysiak
 */

#include "MultiClusterNodeSynchronizationCallback.h"

#include "../../../Toolbox.h"

namespace Lazarus {

MultiClusterNodeSynchronizationCallback::MultiClusterNodeSynchronizationCallback(Lazarus::FastKTuple<unsigned int>& node_ids,
		bool use_semaphore)
	: SynchronizationCallback(use_semaphore)
{
	m_done = node_ids.getm_size();
	m_node_count = node_ids.getm_size();

	//we are addressing directly due to constant size
	mp_data = new Lazarus::FastKTuple< Lazarus::FastVector2<unsigned int, Lazarus::Buffer*> >(m_node_count);
	mp_sequence_complete = new Lazarus::FastKTuple< Lazarus::FastVector2<unsigned int, bool> >(m_node_count);
	mp_fatal_error = new Lazarus::FastKTuple< Lazarus::FastVector2<unsigned int, bool> >(m_node_count);


	//compute the lookup table
	for(long long int i=0;i<node_ids.getm_size();++i )
	{
		m_lookup_map[ node_ids[i] ] = i;
	}

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

MultiClusterNodeSynchronizationCallback::~MultiClusterNodeSynchronizationCallback() {

	if(m_use_semaphore == true)
	{
		int res = sem_destroy(&m_semaphore);
		if(res == -1)
		{
			printf("ERROR: could not destroy semaphore\n");
		}
	}

	delete mp_data;
	delete mp_sequence_complete;
	delete mp_fatal_error;

}

void MultiClusterNodeSynchronizationCallback::indicateError(unsigned int node_id)
{
	(*mp_fatal_error)[ m_lookup_map[ node_id ] ] = Lazarus::FastVector2<unsigned int, bool>(node_id,true );
}

void MultiClusterNodeSynchronizationCallback::indicateCompleteSequence(unsigned int node_id)
{
	(*mp_sequence_complete)[ m_lookup_map[ node_id ] ] = Lazarus::FastVector2<unsigned int, bool>(node_id, true);
}

void MultiClusterNodeSynchronizationCallback::reset()
{
	m_done = m_node_count;

	delete mp_data;
	delete mp_sequence_complete;
	delete mp_fatal_error;

	//we are addressing directly due to constant size
	mp_data = new Lazarus::FastKTuple< Lazarus::FastVector2<unsigned int, Lazarus::Buffer*> >(m_node_count);
	mp_sequence_complete = new Lazarus::FastKTuple< Lazarus::FastVector2<unsigned int, bool> >(m_node_count);
	mp_fatal_error = new Lazarus::FastKTuple< Lazarus::FastVector2<unsigned int, bool> >(m_node_count);


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

void MultiClusterNodeSynchronizationCallback::unlock(unsigned int node_id)
{
	//unlock via exiting the spin lock
	if(m_use_semaphore == false)
	{
		m_done -= 1;
	}
	else
	{
		m_done -=1;

		int res = sem_post(&m_semaphore);
		if(res == -1)
		{
			printf("ERROR: could not unlock semaphore\n");
		}

	}
}

void MultiClusterNodeSynchronizationCallback::lock(unsigned int node_id)
{

	if(m_use_semaphore == false)
	{
		/*Now wait until the sequence ended, this will be indicated by 'callback.m_done == true' which will be
		set by some framehandler. A spinlock may seem to be a wasteful approach, yet the average waiting time
		is expected to be very short.
		This method will introduce a 50us latency!
		*/
		while(m_done != 0)
		{
			Lazarus::Toolbox::sleep_us(50);
		}
	}
	else
	{
		int res = 0;

		/**
		 * Here even the semaphore itself wouldn't work:
		 * The requests are being send out, one by one, at the i-th sending one of the previous requests is being answered,
		 * the semaphore is incremented, possibly more answers arrive before all requests were send out, the sending thread
		 * calls 'lock' and passes right through it, although only a few requests have been answered.
		 */
		while(m_done != 0)
		{
			res = sem_wait(&m_semaphore);
			if(res == -1)
			{
				printf("ERROR: could not lock semaphore\n");
			}
		}
	}

}

void MultiClusterNodeSynchronizationCallback::deliverData(Lazarus::Buffer* data, unsigned int node_id)
{
	(*mp_data)[ m_lookup_map[ node_id ] ] = Lazarus::FastVector2<unsigned int, Lazarus::Buffer*>(node_id, data);
}


} /* namespace Lazarus */
