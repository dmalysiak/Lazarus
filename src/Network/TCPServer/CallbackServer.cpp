/*
 * CallbackServer.cpp
 *
 *  Created on: 12.06.2013
 *      Author: darius
 */

#include "CallbackServer.h"
#include "WorkerThreads/CallbackServerWorkerThread.h"
#include "../Nodes/Callbacks/SingleClusterNodeSynchronizationCallback.h"
#include "../../Time/TimeToolbox.h"

namespace Lazarus
{

CallbackServer::CallbackServer(std::string local_address,
		unsigned int local_port, unsigned int max_connections,
		unsigned int worker_threads, GenericCallback* callback) : ClusterLibServer(local_address,local_port, max_connections,worker_threads) {

	//mp_callback_facility = new CallbackServerComFacility();

	mp_cn_node_facility = new CallbackServerComFacility();
	mp_callback_facility = (CallbackServerComFacility*)mp_cn_node_facility;

	mp_callback = callback;

	//************************** worker threads
	createWorkerThreads();

}

CallbackServer::~CallbackServer()
{
}

void CallbackServer::createWorkerThreads()
{
	for(unsigned int i=0;i<m_worker_thread_count;i++)
	{
		//Create worker thread, update the pokers and start the thread
		mp_worker_threads[i] = new CallbackServerWorkerThread(m_max_connections / m_worker_thread_count,
				(CallbackServerComFacility*)mp_cn_node_facility, mp_callback);
		mp_worker_threads[i]->setDisconnectCallback( this->mp_disconnect_callback );
		mp_worker_threads[i]->setConnectCallback( this->mp_connect_callback );
		mp_worker_threads[i]->setComFacility(mp_cn_node_facility);
		mp_worker_threads[i]->start(i);
	}
}

void CallbackServer::stopClient(unsigned int node_id)
{
	unsigned int type_size = sizeof(enum CallbackServer::CALLBACK_SERVER_REQUEST_TYPE);
	Lazarus::Buffer buffer(type_size);

	//construct data
	setRequestType(&buffer,CallbackServer::CALLBACK_SERVER_REQUEST_TYPE_SHUTDOWN_CLIENT);

	//send data via facility
	//mp_callback_facility->sendDataToNodeSimpleMonologue(node_id,&buffer);

}

int CallbackServer::sendAck(Frame* frame)
{
	Lazarus::Buffer response(sizeof(Lazarus::CallbackServer::CALLBACK_SERVER_REQUEST_TYPE));
	setCRequestType(&response, CallbackServer::CALLBACK_SERVER_REQUEST_TYPE_OK,sizeof(CallbackServer::CALLBACK_SERVER_REQUEST_TYPE));
	int res = frame->sendFrame(&response);
	return res;
}

unsigned int CallbackServer::getNodeCount()
{
	Lazarus::FastKTuple< unsigned int >* node_ids = NULL;

	node_ids = mp_callback_facility->getNodeIDs();

	unsigned int node_count = node_ids->getm_size();
	delete node_ids;

	return node_count;
}

Lazarus::FastKTuple<unsigned int>* CallbackServer::getNodeIDs()
{
	Lazarus::FastKTuple< unsigned int >* node_ids = NULL;

	node_ids = mp_callback_facility->getNodeIDs();

	return node_ids;
}

void CallbackServer::printNodeIDs()
{
	Lazarus::FastKTuple< unsigned int >* node_ids = NULL;

	node_ids = mp_callback_facility->getNodeIDs();

	for(unsigned int i=0;i<node_ids->getm_size();++i)
	{
		printf("Node with id %d\n",node_ids->getElement(i));
	}
}

enum CallbackServer::CALLBACK_SERVER_REQUEST_TYPE CallbackServer::getRequestType(Lazarus::Buffer* message)
{
	enum CallbackServer::CALLBACK_SERVER_REQUEST_TYPE type;

	memcpy(&type,message->get_mp_data(),sizeof(enum CallbackServer::CALLBACK_SERVER_REQUEST_TYPE));

	return type;
}

void CallbackServer::setRequestType(Lazarus::Buffer* message,enum CallbackServer::CALLBACK_SERVER_REQUEST_TYPE type)
{
	memcpy(message->get_mp_data(),&type,sizeof(enum CallbackServer::CALLBACK_SERVER_REQUEST_TYPE));
}

int CallbackServer::getCRequestType(Lazarus::Buffer* message, unsigned int req_type_size)
{
	enum CallbackServer::CALLBACK_SERVER_REQUEST_TYPE type;

	memcpy(&type,message->get_mp_data(),req_type_size);

	return type;
}

void CallbackServer::setCRequestType(Lazarus::Buffer* message,int type, unsigned int req_type_size)
{
	memcpy(message->get_mp_data(),&type,req_type_size);
}

void CallbackServer::setContent(Lazarus::Buffer* message,unsigned char* content, unsigned int size)
{
	unsigned int type_size = sizeof(enum CallbackServer::CALLBACK_SERVER_REQUEST_TYPE);

	memcpy(message->get_mp_data()+type_size,content,size*sizeof(unsigned char));
}

/*
 * this method will allocate an adequate buffer at 'content' and set 'size' accordingly
 */
void CallbackServer::getContent(Lazarus::Buffer* message,unsigned char** content, unsigned int &size)
{
	unsigned int type_size = sizeof(enum CallbackServer::CALLBACK_SERVER_REQUEST_TYPE);

	size = message->get_m_length()-type_size;

	*content = new unsigned char[size];

	memcpy(*content,message->get_mp_data()+type_size,size*sizeof(unsigned char));
}


void CallbackServer::setCContent(Lazarus::Buffer* message,unsigned char* content, unsigned int size, unsigned int type_size)
{
	memcpy(message->get_mp_data()+type_size,content,size*sizeof(unsigned char));
}

/*
 * this method will allocate an adequate buffer at 'content' and set 'size' accordingly
 */
void CallbackServer::getCContent(Lazarus::Buffer* message,unsigned char** content, unsigned int &size, unsigned int type_size)
{
	size = message->get_m_length()-type_size;

	*content = new unsigned char[size];

	memcpy(*content,message->get_mp_data()+type_size,size*sizeof(unsigned char));
}

int CallbackServer::sendCommandAsync(unsigned int node_id, unsigned int type, Lazarus::Buffer* data)
{
	Lazarus::Buffer* buffer = NULL;

	if(data != NULL)
	{
		buffer = new Lazarus::Buffer( sizeof(unsigned int) + data->get_m_length());
	}
	else
	{
		buffer = new Lazarus::Buffer( sizeof(unsigned int) );
	}

	//construct data
	setCRequestType(buffer,type, sizeof(unsigned int));
	//if data was provided
	if(data != NULL)
	{
		memcpy(buffer->get_mp_data()+sizeof(unsigned int), data->get_mp_data(), data->get_m_length());
	}

	//send data via facility
	int res = mp_callback_facility->sendRequestToNodeAsync(node_id,buffer);
	delete buffer;


	return res;
}

int CallbackServer::sendCommandSync(unsigned int node_id, unsigned int type, Lazarus::Buffer* data)
{
	Lazarus::Buffer* buffer = NULL;

	if(data != NULL)
	{
		buffer = new Lazarus::Buffer( sizeof(unsigned int) + data->get_m_length());
	}
	else
	{
		buffer = new Lazarus::Buffer( sizeof(unsigned int) );
	}

	//construct data
	setCRequestType(buffer,type, sizeof(unsigned int));
	//if data was provided
	if(data != NULL)
	{
		memcpy(buffer->get_mp_data()+sizeof(unsigned int), data->get_mp_data(), data->get_m_length());
	}

	//the callback
	SingleClusterNodeSynchronizationCallback comCallback;

	//send data via facility
	mp_callback_facility->sendRequestToNode(node_id,buffer,&comCallback);
	delete buffer;

	if(comCallback.m_sequence_complete == true)
	{
		return 0;
	}

	return -1;
}


}
