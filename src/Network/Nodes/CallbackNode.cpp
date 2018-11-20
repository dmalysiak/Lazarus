/*
 * CallbackNode.cpp
 *
 *  Created on: Aug 24, 2013
 *      Author: Darius Malysiak
 */

#include "CallbackNode.h"

namespace Lazarus {

CallbackNode::CallbackNode(unsigned int nodeID, Frame* frame) {

	m_communication_in_progress = false;
	mp_frame = frame;
	mp_com_callback = NULL;
	pthread_mutex_init(&m_mutex,NULL);
	m_node_id = nodeID;

}

CallbackNode::~CallbackNode()
{

	pthread_mutex_destroy(&m_mutex);

}

void CallbackNode::setCallback(SynchronizationCallback* com_callback)
{
	mp_com_callback = com_callback;
}

SynchronizationCallback* CallbackNode::getCallback()
{
	return mp_com_callback;
}


bool CallbackNode::isCommunicating()
{
	return m_communication_in_progress;
}


Frame* CallbackNode::get_mp_frame()
{
	return mp_frame;
}

void CallbackNode::lockMutex()
{
	pthread_mutex_lock(&m_mutex);
	m_communication_in_progress = true;
}

void CallbackNode::unlockMutex()
{
	m_communication_in_progress = false;
	pthread_mutex_unlock(&m_mutex);
}

void CallbackNode::serialize()
{

}

void CallbackNode::deserialize()
{

}

} /* namespace Lazarus */
