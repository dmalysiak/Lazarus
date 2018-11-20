/*
 * MetaClusterNode.cpp
 *
 *  Created on: 13.06.2013
 *      Author: darius
 */

#include "ClusterNode.h"

namespace Lazarus {

ClusterNode::ClusterNode(Frame* frame) {

	m_communication_in_progress = false;
	mp_frame = frame;
	mp_com_callback = NULL;
	pthread_mutex_init(&m_mutex,NULL);

	mp_system_status = new Lazarus::SystemStatus();

}

//uses the implicit copy constructor of the parent class
ClusterNode::ClusterNode(ExtendedNetworkNode* node, Frame* frame) : ExtendedNetworkNode(node) {

	m_communication_in_progress = false;
	mp_frame = frame;
	mp_com_callback = NULL;
	pthread_mutex_init(&m_mutex,NULL);


	mp_system_status = new Lazarus::SystemStatus();
}

ClusterNode::~ClusterNode() {

	DELETE_NULL_CHECKING(mp_system_status);
	pthread_mutex_destroy(&m_mutex);

}

void ClusterNode::setCallback(SynchronizationCallback* com_callback)
{
	mp_com_callback = com_callback;
}

SynchronizationCallback* ClusterNode::getCallback()
{
	return mp_com_callback;
}

Lazarus::SystemStatus* ClusterNode::get_mp_system_status()
{

	return mp_system_status;

}

bool ClusterNode::isCommunicating()
{
	return m_communication_in_progress;
}


Frame* ClusterNode::get_mp_frame()
{
	return mp_frame;
}

void ClusterNode::lockMutex()
{
	pthread_mutex_lock(&m_mutex);
	m_communication_in_progress = true;
}

void ClusterNode::unlockMutex()
{
	m_communication_in_progress = false;
	pthread_mutex_unlock(&m_mutex);
}

} /* namespace Lazarus */
