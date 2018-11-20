/*
 * CallbackQueue.cpp
 *
 *  Created on: 05.02.2013
 *      Author: Darius Malysiak
 */

#include "CallbackQueue.h"
#include "../DataStructures/GenNCSListElement.h"
#include <stdio.h>

namespace Lazarus
{


CallbackQueue::CallbackQueue() {

	mp_queue = new Queue<Callback*>();

}

CallbackQueue::~CallbackQueue() {

	//delete each object from the queue without calling it
	Callback* call_element = NULL;

	while(mp_queue->getm_is_empty() == false)
	{
		//get an item
		call_element = mp_queue->dequeue();

		//dispose the item
		delete call_element;
	}


	//delete the queue
	delete mp_queue;
}

void CallbackQueue::runElements(Thread* executing_thread)
{
	//call each object from the queue
	Callback* call_element = NULL;

	while(mp_queue->getm_is_empty() == false)
	{
		//get an item
		call_element = mp_queue->dequeue();

		//call the items callback method
		call_element->callbackMethod(executing_thread);

		//dispose the item
		delete call_element;
	}

}

void CallbackQueue::enqueueCallback(Callback* callback_object)
{
	mp_queue->enqueue(callback_object);
}

}
