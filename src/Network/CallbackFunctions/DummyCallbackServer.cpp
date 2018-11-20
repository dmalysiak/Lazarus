/*
 * DummyCallbackServer.cpp
 *
 *  Created on: Aug 25, 2013
 *      Author: Darius Malysiak
 */

#include "DummyCallbackServer.h"

#include "../TCPServer/CallbackServer.h"

namespace Lazarus {

DummyCallbackServer::DummyCallbackServer() {
	// TODO Auto-generated constructor stub

}

DummyCallbackServer::~DummyCallbackServer() {
	// TODO Auto-generated destructor stub
}

int DummyCallbackServer::call(Lazarus::Thread* t, void* var)
{
	CallbackData* data = (CallbackData*)var;

	unsigned int type_size = sizeof(enum DummyCallbackServer::DUMMY_COM_REQUEST_TYPE);
	enum DummyCallbackServer::DUMMY_COM_REQUEST_TYPE type =
			(enum DummyCallbackServer::DUMMY_COM_REQUEST_TYPE)CallbackServer::getCRequestType(data->mp_message,type_size);

	switch(type)
	{

		case DummyCallbackServer::DUMMY_COM_REQUEST_TYPE_INCOMING_RESULT_UNIT:
		{
			printf("Slave node send some results:\n");
			Lazarus::Buffer* data_unit = new Lazarus::Buffer(data->mp_message->get_m_length()-type_size);
			printf("%d bytes\n",data_unit->get_m_length());
			data_unit->printfBuffer();

			delete data_unit;

			//the sequence is over (others may send data to the node)
			//data->mp_serving_node->unlockMutex();

			break;
		}

		default:
			printf("ERROR: dummy callback function received an unknown type\n");
			break;
	}

	return 0;
}

} /* namespace Lazarus */
