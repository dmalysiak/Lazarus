/*
 * ClusterLibNodeManagementFrameHandler.cpp
 *
 *  Created on: 12.06.2013
 *      Author: Darius Malysiak
 */

#include "ClusterLibNodeManagementFrameHandler.h"
#include "../../../System/SystemStatus.h"
#include "../../Nodes/ExtendedNetworkNode.h"

namespace Lazarus {

ClusterLibNodeManagementFrameHandler::ClusterLibNodeManagementFrameHandler(GenericConnection*  connection,
		ClusterNodeComFacility* com_facility)
	: AssembledFrameHandler(connection,com_facility){

	//create a frame for sending packages (this frame object should be used for communication only together with the com-facility,
	//i.e. check if a communication is already in progress before sending data )
	mp_frame = new ClusterLibFrame(connection);

	//register the external active node list
	mp_node_com_facility = com_facility;

	//set the management state into idle
	m_node_management_state = ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_STATE_IDLE;

	//currently serving no node
	m_serving_node_id = 0;

	mp_serving_node = NULL;

	m_obtained_node_id = 0;
}

ClusterLibNodeManagementFrameHandler::~ClusterLibNodeManagementFrameHandler()
{

	//first in case of an active sequence indicate an error
	SynchronizationCallback* sync_callback = mp_serving_node->getCallback();

	//no data
	sync_callback->deliverData(NULL,m_serving_node_id);

	//indicate the error
	sync_callback->indicateError(m_serving_node_id);

	//unlock
	sync_callback->unlock(m_serving_node_id);


	if(mp_node_com_facility != NULL)
	{
		mp_node_com_facility->removeNode(m_serving_node_id);
	}
	else
	{
		printf("ERROR: could not add node object to the listener-threads container of nodes\n");
	}


	DELETE_NULL_CHECKING(mp_serving_node);
}


unsigned int ClusterLibNodeManagementFrameHandler::getServingNodeID()
{
	/* no mutex required as the node id is fused with this object,
	* i.e. it never changes other than from 0 to >0; furthermore if the node dies, this object dies as well!
	* Furthermore it will only be called within the workerthread it has been assigned to!!
	*/
	 return m_serving_node_id;
}


int ClusterLibNodeManagementFrameHandler::handlePayload(Lazarus::Buffer* payload)
{
	/*
	 * any external thread is only allowed to send messages but not to receive any (because this would result in a
	 * race condition with the listening worker thread), thus it must be prevented that during a client request, no external thread will intervene
	 * by sending a (false) response before this handler does
	 */

	switch(m_node_management_state)
	{
		//case of an inactive cluster node -> commence registration
		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_STATE_IDLE:
		{
			/*
			 * the frame handler will now commence the registration process for the currently inbound node
			 */

			int result = 0;
			unsigned int desired_node_id;
			unsigned int type_size = sizeof(enum ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE);
			Lazarus::Buffer* buffer = NULL;
			enum ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE type = ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_PONG;


			//************** get the initial data for a ping
			memcpy(&type,payload->get_mp_data(),type_size);

			//check if a ping has been received
			if( type != ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_PING)
			{
				printf("ERROR: received not a ping, yet expected a ping!\n");
				delete payload;
				return CLUSTERNODE_MANAGEMENT_FRAME_PROCESSING_RESULT_PING_ERROR;
			}
			else
			{
				printf("received ping\n");
			}
			//get the desired node id
			memcpy(&desired_node_id,payload->get_mp_data()+type_size,sizeof(unsigned int));
			delete payload;

			//*************send a response pong (with a attached free node id)
			//get a free node id but with a wish for a desired one
			m_obtained_node_id = mp_node_com_facility->acquire_free_node_id(desired_node_id);//get a free node id

			buffer = new Lazarus::Buffer(type_size+sizeof(unsigned int));
			type = ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_PONG;
			memcpy(buffer->get_mp_data(),&type,type_size);
			memcpy(buffer->get_mp_data()+type_size, &m_obtained_node_id ,sizeof(unsigned int));
			result = mp_frame->sendFrame(buffer);
			delete buffer;

			if(result==-1)
			{
				printf("ERROR: could not send a pong response\n");
				return CLUSTERNODE_MANAGEMENT_FRAME_PROCESSING_RESULT_PONG_ERROR;
			}

			//change the state -> latent register state
			m_node_management_state = ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_STATE_REGISTER;

			return CLUSTERNODE_MANAGEMENT_FRAME_PROCESSING_RESULT_OK;
			break;
		}

		//second phase of registration (essentially this is a sourced out second read operation)
		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_STATE_REGISTER:
		{
			printf("Processing registration phase 2 for node %u\n",m_obtained_node_id);
			//wait for a node object and deserialize it
			ExtendedNetworkNode* p_node = new ExtendedNetworkNode();

			//handle the node object
			Lazarus::Buffer* buffer = new Lazarus::Buffer(payload->get_mp_data(),
					payload->get_m_length(),true);

			delete payload;

			p_node->set_mp_serialized_data_buffer(buffer);
			p_node->deserialize();

			//extract the nodes data
			printf("received node object has the node id %d and the expected id was %d\n",p_node->m_node_id,m_obtained_node_id);
			p_node->printNodeInfo();

			//register the corresponding meta-node
			//keep in mind that the frame handler own this object
			mp_serving_node = new ClusterNode(p_node,mp_frame);
			delete p_node;

			if(mp_node_com_facility != NULL)
			{
				mp_node_com_facility->addNode(mp_serving_node);
			}
			else
			{
				printf("ERROR: could not add node object to the listener-threads container of nodes\n");
				//return to the initial state
				DELETE_NULL_CHECKING(mp_serving_node);
				m_node_management_state = ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_STATE_IDLE;
			}

			//change the state
			m_node_management_state = ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_STATE_NODE_ACTIVE;

			//register the node id
			m_serving_node_id = m_obtained_node_id;


			//return success
			return CLUSTERNODE_MANAGEMENT_FRAME_PROCESSING_RESULT_FINISHED_REGISTRATION;
			break;
		}

		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_STATE_NODE_ACTIVE:

			parseCommand(payload);
			delete payload;
			return CLUSTERNODE_MANAGEMENT_FRAME_PROCESSING_RESULT_OK;
			break;

		default:
			return CLUSTERNODE_MANAGEMENT_FRAME_PROCESSING_RESULT_NODE_DATA_ERROR;
			break;

	}


	return CLUSTERNODE_MANAGEMENT_FRAME_PROCESSING_RESULT_OVER_AND_OUT;
}

/*
 * Message parser and second part of the state automaton.
 */
int ClusterLibNodeManagementFrameHandler::parseCommand(Lazarus::Buffer* message)
{
	enum ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE type = ClusterNodeManagementServer::getRequestType(message);
	unsigned int type_size = sizeof(enum ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE);

	switch(type)
	{

		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_ANSWER_DUMMY_DATA:
		{
			//the sequence is over (others may send data to the node)
			mp_serving_node->unlockMutex();

			SynchronizationCallback* sync_callback = mp_serving_node->getCallback();

			//return it to the caller (deep data copy)
			sync_callback->deliverData(new Lazarus::Buffer(  message->get_mp_data() + type_size, message->get_m_length()-type_size, true),
					m_serving_node_id);

			//sequence was complete
			sync_callback->indicateCompleteSequence(m_serving_node_id);

			//this will release the lock
			sync_callback->unlock(m_serving_node_id);

			break;
		}

		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_ANSWER_SHU_PID:
		{
			//the sequence is over
			mp_serving_node->unlockMutex();

			SynchronizationCallback* sync_callback = mp_serving_node->getCallback();

			//return it to the caller (deep data copy)
			sync_callback->deliverData(new Lazarus::Buffer(  message->get_mp_data() + type_size, message->get_m_length()-type_size, true),
					m_serving_node_id);

			//sequence was complete
			sync_callback->indicateCompleteSequence(m_serving_node_id);

			//this will release the lock
			sync_callback->unlock(m_serving_node_id);

			break;
		}

		//the first file chunk of the clients requested large file
		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_FIRST_INCOMING_GET_FILE_CHUNK:
		{
			SynchronizationCallback* sync_callback = mp_serving_node->getCallback();

			//return it to the caller (deep data copy)
			sync_callback->deliverData(new Lazarus::Buffer(  message->get_mp_data() + type_size, message->get_m_length()-type_size, true),
					m_serving_node_id);

			//sequence was complete
			sync_callback->indicateCompleteSequence(m_serving_node_id);

			//this will release the lock
			sync_callback->unlock(m_serving_node_id);

			break;
		}

		//another incoming file chunk of the clients requested large file
		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_NEXT_INCOMING_GET_FILE_CHUNK:
		{
			SynchronizationCallback* sync_callback = mp_serving_node->getCallback();

			//return it to the caller (deep data copy)
			sync_callback->deliverData(new Lazarus::Buffer(  message->get_mp_data() + type_size, message->get_m_length()-type_size, true),
					m_serving_node_id);

			//sequence was complete
			sync_callback->indicateCompleteSequence(m_serving_node_id);

			//this will release the lock
			sync_callback->unlock(m_serving_node_id);

			break;
		}

		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_FILE_RECEIVED:
		{
			//the sequence is over
			mp_serving_node->unlockMutex();

			SynchronizationCallback* sync_callback = mp_serving_node->getCallback();

			//no return data
			sync_callback->deliverData(NULL, m_serving_node_id);

			//sequence was complete
			sync_callback->indicateCompleteSequence(m_serving_node_id);

			//this will release the lock
			sync_callback->unlock(m_serving_node_id);

			break;
		}

		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_GET_LARGE_FILE_OK:
		{
			//the sequence is over
			mp_serving_node->unlockMutex();

			SynchronizationCallback* sync_callback = mp_serving_node->getCallback();

			//no return data
			sync_callback->deliverData(NULL, m_serving_node_id);

			//sequence was complete
			sync_callback->indicateCompleteSequence(m_serving_node_id);

			//this will release the lock
			sync_callback->unlock(m_serving_node_id);

			break;
		}

		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_SHU_TERMINATED:
		{
			//the sequence is over
			mp_serving_node->unlockMutex();

			SynchronizationCallback* sync_callback = mp_serving_node->getCallback();

			//no return data
			sync_callback->deliverData(NULL, m_serving_node_id);

			//sequence was complete
			sync_callback->indicateCompleteSequence(m_serving_node_id);

			//this will release the lock
			sync_callback->unlock(m_serving_node_id);

			break;
		}

		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_INCOMING_LARGE_FILE_FINAL_CHUNK_OK:
		{
			//the sequence is over
			mp_serving_node->unlockMutex();

			SynchronizationCallback* sync_callback = mp_serving_node->getCallback();

			//no return data
			sync_callback->deliverData(NULL, m_serving_node_id);

			//sequence was complete
			sync_callback->indicateCompleteSequence(m_serving_node_id);

			//this will release the lock
			sync_callback->unlock(m_serving_node_id);

			break;
		}

		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_INCOMING_LARGE_FILE_CHUNK_OK:
		{
			SynchronizationCallback* sync_callback = mp_serving_node->getCallback();

			//no return data
			sync_callback->deliverData(NULL, m_serving_node_id);

			//sequence still as planned
			sync_callback->indicateCompleteSequence(m_serving_node_id);

			//this will release the lock
			sync_callback->unlock(m_serving_node_id);

			break;
		}

		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_INCOMING_LARGE_FILE_CHUNK_ERROR:
		{
			//the sequence is over
			mp_serving_node->unlockMutex();

			SynchronizationCallback* sync_callback = mp_serving_node->getCallback();

			//no return data
			sync_callback->deliverData(NULL, m_serving_node_id);

			//this will release the lock
			sync_callback->unlock(m_serving_node_id);

			break;
		}

		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_SHU_TERMINATE_ERROR:
		{
			//the sequence is over
			mp_serving_node->unlockMutex();

			SynchronizationCallback* sync_callback = mp_serving_node->getCallback();

			//no return data
			sync_callback->deliverData(NULL, m_serving_node_id);

			//unlock
			sync_callback->unlock(m_serving_node_id);

			break;
		}

		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_FILE_RECV_ERROR:
		{
			//the sequence is over
			mp_serving_node->unlockMutex();

			SynchronizationCallback* sync_callback = mp_serving_node->getCallback();

			//no return data
			sync_callback->deliverData(NULL, m_serving_node_id);

			//this will release the lock
			sync_callback->unlock(m_serving_node_id);

			break;
		}

		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_FILE_NOT_FOUND:
		{
			//the sequence is over
			mp_serving_node->unlockMutex();

			SynchronizationCallback* sync_callback = mp_serving_node->getCallback();

			//no return data
			sync_callback->deliverData(NULL, m_serving_node_id);

			//this will release the lock
			sync_callback->unlock(m_serving_node_id);

			break;
		}

		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_SYSTEM_COMMAND_ERROR:
		{
			//the sequence is over
			mp_serving_node->unlockMutex();

			SynchronizationCallback* sync_callback = mp_serving_node->getCallback();

			//no return data
			sync_callback->deliverData(NULL, m_serving_node_id);

			//this will release the lock
			sync_callback->unlock(m_serving_node_id);

			break;
		}

		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_ASYNC_SYSTEM_COMMAND_ERROR:
		{
			//the sequence is over
			mp_serving_node->unlockMutex();

			SynchronizationCallback* sync_callback = mp_serving_node->getCallback();

			//no return data
			sync_callback->deliverData(NULL, m_serving_node_id);

			//this will release the lock
			sync_callback->unlock(m_serving_node_id);

			break;
		}

		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_SHU_ERROR:
		{
			//the sequence is over
			mp_serving_node->unlockMutex();

			SynchronizationCallback* sync_callback = mp_serving_node->getCallback();

			//no return data
			sync_callback->deliverData(NULL, m_serving_node_id);

			//this will release the lock
			sync_callback->unlock(m_serving_node_id);

			break;
		}

		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_INCOMING_CLIENT_FILE:
		{

			//the sequence is over
			mp_serving_node->unlockMutex();

			SynchronizationCallback* sync_callback = mp_serving_node->getCallback();

			//return it to the caller (deep data copy)
			sync_callback->deliverData(new Lazarus::Buffer(  message->get_mp_data() + type_size, message->get_m_length()-type_size, true),
					m_serving_node_id);

			//sequence was complete
			sync_callback->indicateCompleteSequence(m_serving_node_id);

			//this will release the lock
			sync_callback->unlock(m_serving_node_id);

			break;
		}

		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_ANSWER_SYSTEM_COMMAND:
		{
			//the sequence is over
			mp_serving_node->unlockMutex();

			SynchronizationCallback* sync_callback = mp_serving_node->getCallback();

			//return it to the caller (deep data copy)
			sync_callback->deliverData(new Lazarus::Buffer(  message->get_mp_data() + type_size, message->get_m_length()-type_size, true),
					m_serving_node_id);

			//sequence was complete
			sync_callback->indicateCompleteSequence(m_serving_node_id);

			//this will release the lock
			sync_callback->unlock(m_serving_node_id);

			break;
		}


		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_SHU_OK:
		{
			//the sequence is over
			mp_serving_node->unlockMutex();

			SynchronizationCallback* sync_callback = mp_serving_node->getCallback();

			//no return data
			sync_callback->deliverData(NULL, m_serving_node_id);

			//sequence was complete
			sync_callback->indicateCompleteSequence(m_serving_node_id);

			//this will release the lock
			sync_callback->unlock(m_serving_node_id);

			break;
		}

		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_ASYNC_SYSTEM_COMMAND_OK:
		{
			//the sequence is over
			mp_serving_node->unlockMutex();

			SynchronizationCallback* sync_callback = mp_serving_node->getCallback();

			//no return data
			sync_callback->deliverData(NULL, m_serving_node_id);

			//sequence was complete
			sync_callback->indicateCompleteSequence(m_serving_node_id);

			//this will release the lock
			sync_callback->unlock(m_serving_node_id);

			break;
		}

		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_UNREGISTER:
		{
			printf("CLIENT REQUESTS AN UNREGISTRATION\n");

			Lazarus::Buffer response(type_size);
			ClusterNodeManagementServer::setRequestType(&response,ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_UNREGISTER_CONFIRM);

			//remove the node from the com facility
			mp_serving_node->unlockMutex();

			if(mp_node_com_facility != NULL)
			{
				mp_node_com_facility->removeNode(m_serving_node_id);
			}
			else
			{
				printf("ERROR: could not add node object to the listener-threads container of nodes\n");
			}

			printf("SENDING CONFIRMATION\n");
			int result = mp_frame->sendFrame(&response);

			if(result != 0)
			{
				return -1;
			}

			break;
		}

		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_ANSWER_SYSTEM_STATE:
		{
			//the sequence is over
			mp_serving_node->unlockMutex();

			SynchronizationCallback* sync_callback = mp_serving_node->getCallback();

			//return it to the caller (deep data copy)
			sync_callback->deliverData(new Lazarus::Buffer(  message->get_mp_data() + type_size, message->get_m_length()-type_size, true),
					m_serving_node_id);

			//sequence was complete
			sync_callback->indicateCompleteSequence(m_serving_node_id);

			//this will release the lock
			sync_callback->unlock(m_serving_node_id);

			break;
		}

		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_ANSWER_SHORT_SYSTEM_CPU_STATE:
		{
			//the sequence is over
			mp_serving_node->unlockMutex();

			SynchronizationCallback* sync_callback = mp_serving_node->getCallback();

			//return it to the caller (deep data copy)
			sync_callback->deliverData(new Lazarus::Buffer(  message->get_mp_data() + type_size, message->get_m_length()-type_size, true),
					m_serving_node_id);

			//sequence was complete
			sync_callback->indicateCompleteSequence(m_serving_node_id);

			//this will release the lock
			sync_callback->unlock(m_serving_node_id);

			break;
		}

		default:
			break;
	}

	return 0;

}

} /* namespace Lazarus */
