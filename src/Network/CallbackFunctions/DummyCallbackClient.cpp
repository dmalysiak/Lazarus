/*
 * DummyCallbackClient.cpp
 *
 *  Created on: Aug 25, 2013
 *      Author: Darius Malysiak
 */

#include "DummyCallbackClient.h"

#include "../TCPServer/CallbackServer.h"
#include "DummyCallbackServer.h"

namespace Lazarus {

DummyCallbackClient::DummyCallbackClient() {

	m_reply_count = 0;

}

DummyCallbackClient::~DummyCallbackClient() {
	// TODO Auto-generated destructor stub
}

int DummyCallbackClient::call(Lazarus::Thread* t, void* var)
{
	CallbackData* data = (CallbackData*)var;

	unsigned int type_size = sizeof(enum DummyCallbackServer::DUMMY_COM_REQUEST_TYPE);
	enum DummyCallbackServer::DUMMY_COM_REQUEST_TYPE type =
			(enum DummyCallbackServer::DUMMY_COM_REQUEST_TYPE)CallbackServer::getCRequestType(data->mp_message,type_size);

	switch(type)
	{

		case DummyCallbackServer::DUMMY_COM_REQUEST_TYPE_INCOMING_DATA_UNIT:
		{
			printf("Dummy callback function received some raw data:\n");
			Lazarus::Buffer* data_unit = new Lazarus::Buffer(data->mp_message->get_m_length()-type_size);
			printf("%d bytes\n",data_unit->get_m_length()-type_size);
			data_unit->printfBuffer();
			delete data_unit;

			Lazarus::Buffer response(sizeof(CallbackServer::CALLBACK_SERVER_REQUEST_TYPE));
			CallbackServer::setCRequestType(&response,
					CallbackServer::CALLBACK_SERVER_REQUEST_TYPE_OK,type_size);
			data->mp_frame->sendFrame(&response);

			//the sequence is over (others may send data to the node)
			/*if(data->mp_serving_node != NULL)
			{
				data->mp_serving_node->unlockMutex();
			}*/

			break;
		}

		case DummyCallbackServer::DUMMY_COM_REQUEST_TYPE_START_PROCESSING:
		{
			//do something
			printf("I did something\n");

			//send a reply
			++m_reply_count;
			Lazarus::Buffer response(type_size+sizeof(unsigned int));
			CallbackServer::setCRequestType(&response,
					DummyCallbackServer::DUMMY_COM_REQUEST_TYPE_INCOMING_RESULT_UNIT,type_size);
			CallbackServer::setCContent(&response,(unsigned char*)&m_reply_count,sizeof(unsigned int),type_size);
			data->mp_frame->sendFrame(&response);

			break;
		}

		default:
			printf("ERROR: dummy callback function received an unknown type\n");
			break;
	}

	return 0;
}

} /* namespace Lazarus */
