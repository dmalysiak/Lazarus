/*
 * ClusterNodeComFacility.cpp
 *
 *  Created on: 17.06.2013
 *      Author: darius
 */

#include "ClusterNodeComFacility.h"
#include "../../Toolbox.h"

namespace Lazarus {

ClusterNodeComFacility::ClusterNodeComFacility() {

	mp_active_nodes = new Lazarus::ThreadSafeUnorderedMap<unsigned int,ClusterNode*>();

	m_next_free_id = 2;
	pthread_mutex_init(&m_mutex,NULL);
}

ClusterNodeComFacility::~ClusterNodeComFacility() {

	pthread_mutex_destroy(&m_mutex);

	//delete the list of nodes (the nodes are owned by the frame handlers, thus they are not destroyed)
	delete mp_active_nodes;

}

void ClusterNodeComFacility::addNode(ClusterNode* node)
{
	//insert the node into the list
	mp_active_nodes->insertElement(node->m_node_id,node);
}

void ClusterNodeComFacility::removeNode(unsigned int node_id)
{
	//remove the element
	mp_active_nodes->eraseElement(node_id);

	pthread_mutex_lock(&m_mutex);

	m_used_node_ids.erase(node_id);//remove the id. marking it as unused

	pthread_mutex_unlock(&m_mutex);
}

ClusterNode* ClusterNodeComFacility::getNode(unsigned int node_id)
{
	ClusterNode* node = NULL;

	try
	{
		node = mp_active_nodes->getElement(node_id);
	}
	catch(const std::out_of_range &oe)
	{
		return NULL;
	}

	return node;
}

int ClusterNodeComFacility::sendRequestToNode(unsigned int node_id, Lazarus::Buffer* request, SynchronizationCallback* callback)
{
	int result = 0;

	//get the node
	ClusterNode* node = NULL;

	try
	{
		node = mp_active_nodes->getElement(node_id);
	}
	catch(const std::out_of_range &oe)
	{
		return -1;
	}

	if(node == NULL)
	{
		printf("ERROR: could not send data to node with id %d\n", node_id);
		return -1;
	}

	//Wait until any active connection ceased. Afterwards lock the node for other communication.
	node->lockMutex();

	//Set the callback
	node->setCallback(callback);

	//Send data
	result = node->get_mp_frame()->sendFrame(request);

	//wait for result
	callback->lock(node_id);

	return result;
}

int ClusterNodeComFacility::sendRequestToMultipleNodes(Lazarus::FastKTuple<unsigned int>& node_ids, Lazarus::Buffer* request,
		SynchronizationCallback* callback)
{
	int result = 0;

	//get the node
	ClusterNode* node = NULL;

	//iterate over all nodes and send the request to them
	for(long long int i=0;i<node_ids.getm_size();++i)
	{
		try
		{
			node = mp_active_nodes->getElement(node_ids[i]);
		}
		catch(const std::out_of_range &oe)
		{
			return -1;
		}

		if(node == NULL)
		{
			printf("ERROR: could not send data to node with id %d\n", node_ids[i]);
			return -1;
		}

		//Wait until any active connection ceased. Afterwards lock the node for other communication.
		node->lockMutex();

		//Set the callback
		node->setCallback(callback);

		//Send data
		result = node->get_mp_frame()->sendFrame(request);
	}

	//wait for result
	callback->lock(2);

	return result;
}

int ClusterNodeComFacility::sendRequestToNodeNL(unsigned int node_id, Lazarus::Buffer* request, SynchronizationCallback* callback)
{
	int result = 0;

	//get the node
	ClusterNode* node = NULL;

	try
	{
		node = mp_active_nodes->getElement(node_id);
	}
	catch(const std::out_of_range &oe)
	{
		return -1;
	}

	if(node == NULL)
	{
		printf("ERROR: could not send data to node with id %d\n", node_id);
		return -1;
	}

	//Wait until any active connection ceased. Afterwards lock the node for other communication.
	//node->lockMutex();

	//Set the callback
	node->setCallback(callback);

	//Send data
	result = node->get_mp_frame()->sendFrame(request);

	//wait for result
	callback->lock(node_id);

	return result;
}


unsigned int ClusterNodeComFacility::getNodeCount()
{
	return mp_active_nodes->getMap()->size();
}

Lazarus::FastKTuple<unsigned int>* ClusterNodeComFacility::getNodeIDs()
{

	mp_active_nodes->externalRDLock();

	Lazarus::FastKTuple<unsigned int>* ids = new Lazarus::FastKTuple<unsigned int>( getNodeCount() );
	unsigned int counter = 0;

	for( std::unordered_map<unsigned int,ClusterNode*>::iterator it = mp_active_nodes->getMap()->begin();
		 it != mp_active_nodes->getMap()->end();
		 ++it )
	{
		ids->setElement(counter ,it->first);

		counter++;
	}

	mp_active_nodes->externalRDUnlock();

	return ids;
}


unsigned int ClusterNodeComFacility::acquire_free_node_id(unsigned int desired_id)
{
	/**
	 * The mutex is required as this operation will be called from multiple worker threads, i.e. it
	 * is a global operation!
	 */
	unsigned int candidate_id = 0;

	pthread_mutex_lock(&m_mutex);

	candidate_id = m_next_free_id;

	//first check if the desired id is available
	if(check_id_usage(desired_id) == false)
	{
		m_used_node_ids[desired_id] = desired_id;
		pthread_mutex_unlock(&m_mutex);
		return desired_id;
	}

	//otherwise try to iteratively find the next free id
	while( check_id_usage(m_next_free_id) == true )
	{
		++m_next_free_id;
	}

	candidate_id = m_next_free_id;
	m_used_node_ids[candidate_id] = candidate_id;//insert the id. marking it as used

	pthread_mutex_unlock(&m_mutex);

	return candidate_id;

}

bool ClusterNodeComFacility::check_id_usage(unsigned int id)
{
	std::unordered_map<unsigned int, unsigned int>::iterator it = m_used_node_ids.find(id);

	if( it == m_used_node_ids.end() )
	{
		return false;
	}

	return true;
}


} /* namespace Lazarus */
