/*
 * CallbackServerFrameHandler.cpp
 *
 *  Created on: 12.06.2013
 *      Author: Darius Malysiak
 */

#include "CallbackServerFrameHandler.h"
#include "../../../System/SystemStatus.h"

namespace Lazarus {

CallbackServerFrameHandler::CallbackServerFrameHandler(GenericConnection*  connection,
		CallbackServerComFacility* com_facility, GenericCallback* callback, Lazarus::Thread* worker_thread)
	: AssembledFrameHandler(connection,com_facility){

	//create a frame for sending packages (this frame object should be used for communication only together with the com-facility,
	//i.e. check if a communication is already in progress before sending data )
	mp_frame = new ClusterLibFrame(connection);

	//register the external active node list
	mp_callback_facility = com_facility;

	//set the management state into idle
	m_node_management_state = CallbackServer::CALLBACK_SERVER_STATE_IDLE;

	//currently serving no node
	m_serving_node_id = 0;

	//generic callback function
	mp_callback = callback;

	mp_serving_node = NULL;

	mp_worker_thread = worker_thread;

	m_type = CallbackServer::CALLBACK_SERVER_REQUEST_TYPE_NULL;

	m_type_size = sizeof(enum CallbackServer::CALLBACK_SERVER_REQUEST_TYPE);
}

CallbackServerFrameHandler::~CallbackServerFrameHandler()
{
	if(mp_callback_facility != NULL)
	{
		mp_callback_facility->removeNode(m_serving_node_id);
	}
	else
	{
		printf("ERROR: could not add node object to the listener-threads container of nodes\n");
	}

	DELETE_NULL_CHECKING(mp_serving_node);

}



int CallbackServerFrameHandler::handlePayload(Lazarus::Buffer* payload)
{
	//this lock only covers a simple ping pong communication with the client, i.e.
	/*
	 * any external thread is only allowed to send messages but not to receive any (because this would result in a
	 * race condition with the listening worker thread), thus it must be prevented that during a client request, no external thread will intervene
	 * by sending a (false) response before this handler does
	 */

	m_type = CallbackServer::getRequestType(payload);
	int result = 0;

	switch(m_node_management_state)
	{

		//case of an inactive cluster node -> commence registration
		case CallbackServer::CALLBACK_SERVER_STATE_IDLE:
		{
			/*
			 * the frame handler will now commence the registration process for the currently inbound node
			 */
			unsigned int node_id = 0;
			Lazarus::Buffer* m_buffer = NULL;
			enum CallbackServer::CALLBACK_SERVER_REQUEST_TYPE type = CallbackServer::CALLBACK_SERVER_REQUEST_TYPE_NULL;


			//************** get the initial data for a ping
			type = CallbackServer::getRequestType(payload);

			//check if a ping has been received
			if( type != CallbackServer::CALLBACK_SERVER_REQUEST_TYPE_PING)
			{
				printf("ERROR: received not a ping, yet expected a ping!\n");
				return CALLBACK_SERVER_FRAME_PROCESSING_RESULT_PING_ERROR;
			}

			//*************extract the attached node id
			memcpy(&node_id,payload->get_mp_data()+m_type_size,sizeof(unsigned int));
			//printf("shu client send ID %d\n",node_id);
			delete payload;

			//************* send an answer
			m_buffer = new Lazarus::Buffer(m_type_size);
			CallbackServer::setRequestType(payload,CallbackServer::CALLBACK_SERVER_REQUEST_TYPE_PONG);
			result = mp_frame->sendFrame(m_buffer);
			delete m_buffer;

			if(result==-1)
			{
				printf("ERROR: could not send a pong response\n");
				return CALLBACK_SERVER_FRAME_PROCESSING_RESULT_PONG_ERROR;
			}

			//register the corresponding meta-node
			mp_serving_node = new CallbackNode(node_id,mp_frame);

			if(mp_callback_facility != NULL)
			{
				mp_callback_facility->addNode(mp_serving_node);
			}
			else
			{
				printf("ERROR: could not add node object to the listener-threads container of nodes\n");
			}

			//change the state
			m_node_management_state = CallbackServer::CALLBACK_SERVER_STATE_ACTIVE;

			//register the node id
			m_serving_node_id = node_id;

			//return success
			return CALLBACK_SERVER_FRAME_PROCESSING_RESULT_FINISHED_REGISTRATION;
			break;
		}

		case CallbackServer::CALLBACK_SERVER_STATE_ACTIVE:
		{
			if( m_type == CallbackServer::CALLBACK_SERVER_REQUEST_TYPE_UNREGISTER ) //unregister the node
			{
				mp_serving_node->lockMutex();
				printf("CLIENT REQUESTS AN UNREGISTRATION\n");

				Lazarus::Buffer response(m_type_size);
				CallbackServer::setRequestType(&response,CallbackServer::CALLBACK_SERVER_REQUEST_TYPE_UNREGISTER_CONFIRM);

				printf("SENDING CONFIRMATION\n");
				mp_frame->sendFrame(&response);

				//remove the node from the com facility
				mp_serving_node->unlockMutex();

				if(mp_callback_facility != NULL)
				{
					mp_callback_facility->removeNode(m_serving_node_id);
				}
				else
				{
					printf("ERROR: could not add node object to the listener-threads container of nodes\n");
				}

			}

			if( m_type == CallbackServer::CALLBACK_SERVER_REQUEST_TYPE_OK ) //just unlock the node and close the sequence
			{

				//the sequence is over (others may send data to the node)
				mp_serving_node->unlockMutex();

				SynchronizationCallback* sync_callback = mp_serving_node->getCallback();

				//sequence was complete
				sync_callback->indicateCompleteSequence(m_serving_node_id);

				//this will release the lock
				sync_callback->unlock(m_serving_node_id);

			}

			else //normal payload type
			{
				//mp_serving_node->lockMutex();

				//the callback method should only be called for request types not defined in 'CallbackServer'!
				CallbackData data;
				data.mp_frame = mp_frame;
				data.mp_message = payload;
				data.mp_serving_node = mp_serving_node;

				result = mp_callback->call(mp_worker_thread,(void*)&data);

				delete payload;

				if(result == -1)
				{
					printf("ERROR: callback function returned -1\n");
				}
			}

			return CALLBACK_SERVER_FRAME_PROCESSING_RESULT_OK;
			break;
		}

		default:

			printf("ERROR: CallbackServer frame handler could not process payload\n");

			return CALLBACK_SERVER_FRAME_PROCESSING_RESULT_DATA_ERROR;
			break;

	}


	return CALLBACK_SERVER_FRAME_PROCESSING_RESULT_OVER_AND_OUT;
}


} /* namespace Lazarus */
