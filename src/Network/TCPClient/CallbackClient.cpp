/*
 * ClusterNodeManagementClient.cpp
 *
 *  Created on: 22.03.2013
 *      Author: Darius Malysiak
 */

#include "CallbackClient.h"
#include "../../Network/ConnectionManager.h"
#include "../../Toolbox.h"
#include "../../Buffer.h"


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace Lazarus
{

CallbackClient::CallbackClient(std::string remote_address,
		unsigned int remote_port, unsigned int nodeID, GenericCallback* callback,
		unsigned int max_attempts, unsigned int attempt_wait_time)
{
	unsigned int attempts = 1;
	m_current_state = CallbackClient::CALLBACK_CLIENT_STATE_UNINITIALIZED;

	mp_connection = ConnectionManager::connectToTCPIPv4(remote_address,remote_port);

	//if no connection could be made, keep trying a bit longer
	while(mp_connection == NULL && attempts <= max_attempts)
	{
		mp_connection = ConnectionManager::connectToTCPIPv4(remote_address,remote_port);
		Lazarus::Toolbox::sleep_ms(attempt_wait_time);
		printf("CONNECTION ATTEMPT %d\n",attempts);
		attempts++;
	}

	//if a connection has been established
	if(mp_connection != NULL)
	{
		mp_frame = new ClusterLibFrame(mp_connection);
		mp_connection->printConnectionInformation();
	}

	mp_callback = callback;

	//local vars
	m_type = CallbackServer::CALLBACK_SERVER_REQUEST_TYPE_NULL;

	m_node_id = nodeID;

}

CallbackClient::~CallbackClient()
{
	//unregister the client
	unregisterClient();

	//close the connection
	DELETE_NULL_CHECKING(mp_connection);

	//delete the frame
	DELETE_NULL_CHECKING(mp_frame);

}

ClusterLibFrame* CallbackClient::get_mp_frame()
{
	return mp_frame;
}

void CallbackClient::unregisterClient()
{
	//unregister the client
	//if no connection has been established: abort
	if(mp_connection != NULL)
	{
		//if the socket was closed: don't even try to send something
		if(mp_connection->get_m_socket_active() == false)
		{
			return;
		}

		int result = 0;
		unsigned int type_size = sizeof(enum CallbackServer::CALLBACK_SERVER_REQUEST_TYPE);
		Lazarus::Buffer* buffer = new Lazarus::Buffer(type_size);
		enum CallbackServer::CALLBACK_SERVER_REQUEST_TYPE type = CallbackServer::CALLBACK_SERVER_REQUEST_TYPE_NULL;

		//send the unregister request
		printf("sending unregister request\n");
		CallbackServer::setRequestType(buffer,CallbackServer::CALLBACK_SERVER_REQUEST_TYPE_UNREGISTER);
		result = mp_frame->sendFrame(buffer);
		delete buffer;

		if(result==-1)
		{
			printf("ERROR: could not unregister request\n");
			return;
		}

		//wait for a response
		printf("waiting for unregister confirmation\n");
		buffer = mp_frame->readFrame();
		type = CallbackServer::getRequestType(buffer);
		delete buffer;

		//check the type
		if( type != CallbackServer::CALLBACK_SERVER_REQUEST_TYPE_UNREGISTER_CONFIRM)
		{
			printf("ERROR: received not a confirmation\n");
		}

		printf("received confirmation\n");


		//close the connection
		DELETE_NULL_CHECKING(mp_connection);

		//delete the frame
		DELETE_NULL_CHECKING(mp_frame);
	}
}


void CallbackClient::run()
{

	//if no connection has been established: abort
	if(mp_connection == NULL)
	{
		printf("ERROR: no active connection\n");
		return;
	}

	int result = 0;
	unsigned int type_size = sizeof(enum CallbackServer::CALLBACK_SERVER_REQUEST_TYPE);
	enum CallbackServer::CALLBACK_SERVER_REQUEST_TYPE type = CallbackServer::CALLBACK_SERVER_REQUEST_TYPE_PING;

	//---------------------- wait for servers ping
	Lazarus::Buffer* buffer = NULL;
	/*buffer = mp_frame->readFrame();
	memcpy(&type,buffer->get_mp_data(),type_size);

	//check for ping type
	if( type != CallbackServer::CALLBACK_SERVER_REQUEST_TYPE_PING)
	{
		printf("ERROR: received not a ping\n");
		delete buffer;
		return;
	}
	else
	{
		printf("received the servers initial greeting ping\n");
	}
	delete buffer;*/

	//---------------------- send a greeting ping
	printf("sending ping\n");
	buffer = new Lazarus::Buffer(type_size + sizeof(unsigned int));
	type = CallbackServer::CALLBACK_SERVER_REQUEST_TYPE_PING;
	memcpy(buffer->get_mp_data(),&type,type_size);//type
	memcpy(buffer->get_mp_data()+type_size,&m_node_id,sizeof(unsigned int));//node id
	result = mp_frame->sendFrame(buffer);
	delete buffer;

	if(result==-1)
	{
		printf("ERROR: could not send ping greeting\n");
		return;
	}

	//wait for a response
	printf("waiting for pong\n");
	buffer = mp_frame->readFrame();
	memcpy(&type,buffer->get_mp_data(),type_size);

	//check for pong
	if( type != CallbackServer::CALLBACK_SERVER_REQUEST_TYPE_PONG)
	{
		printf("ERROR: received not a pong\n");
		delete buffer;
		return;
	}

	printf("received pong\n");
	buffer->printfBuffer();

	//enter nodes state machine once the node has been registered
	m_current_state = CallbackClient::CALLBACK_CLIENT_STATE_NODE_ACTIVE;

	while(m_shutdown_flag != true)
	{
		switch(m_current_state)
		{
			case CallbackClient::CALLBACK_CLIENT_STATE_NODE_ACTIVE:
			{
				//wait for an arbitrary command (using a timeout enables one to stop this thread via the shutdown method)
				//printf("client waiting\n");
				Lazarus::Buffer* buffer1 = mp_frame->readFrame(10500,500);

				if(buffer1 != NULL)
				{
					m_type = CallbackServer::getRequestType(buffer1);

					//in case of a shutdown request: stop the client
					if(m_type == CallbackServer::CALLBACK_SERVER_REQUEST_TYPE_SHUTDOWN_CLIENT )
					{
						unregisterClient();
						m_shutdown_flag = true;
						m_thread_self_terminated = true;
						DELETE_NULL_CHECKING(buffer1);
						break;
					}

					//as we can't detect a closed socket from just the output of the readFrame method
					if(mp_connection->get_m_socket_active() == false)
					{
						printf("socket was closed shutting down\n");
						m_shutdown_flag = true;
						m_thread_self_terminated = true;
						DELETE_NULL_CHECKING(buffer1);
						break;
					}

					//otherwise call the registered callback
					//the callback method should only be called for request types not defined in 'CallbackServer'!
					CallbackData data;
					data.mp_frame = mp_frame;
					data.mp_message = buffer1;
					data.mp_serving_node = NULL;

					result = mp_callback->call(this,(void*)&data);

					if(result == -1)
					{
						printf("ERROR: callback method returned -1\n");
					}
				}
				else
				{
					//as we can't detect a closed socket from just the output of the readFrame method
					if(mp_connection->get_m_socket_active() == false)
					{
						m_shutdown_flag = true;
						m_thread_self_terminated = true;
						break;
					}
				}

				delete buffer1;

				break;
			}

			default:
				break;
		}

	}

	printf("Callback client is exiting\n");

}

}
