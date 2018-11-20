/*
 * CallbackQueue.cpp
 *
 *  Created on: 05.02.2013
 *      Author: Darius Malysiak
 */

#include "PersistentCallbackQueue.h"
#include "../DataStructures/FastKTuple.h"
#include <stdio.h>

namespace Lazarus
{


PersistentCallbackQueue::PersistentCallbackQueue() {

}

PersistentCallbackQueue::~PersistentCallbackQueue() {

}

void PersistentCallbackQueue::runElements(Thread* executing_thread)
{
	//call each object from the queue
	Callback* call_element = NULL;

	FastKTuple<Callback*> callbacks(mp_queue->getSize());
	unsigned int i= 0;

	while(mp_queue->getm_is_empty() == false)
	{
		//get an item
		call_element = mp_queue->dequeue();

		//call the items callback method
		call_element->callbackMethod(executing_thread);

		//save the item for later re-enqueuing
		callbacks[i] = call_element;
		++i;
	}

	unsigned int size = callbacks.getm_size();
	for(unsigned int j=0;j<size;++j)
	{
		mp_queue->enqueue(callbacks[size-1-j]);
	}
}

}
