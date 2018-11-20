/*
 * RCSServerCallback.cpp
 *
 *  Created on: Aug 30, 2013
 *      Author: Darius Malysiak
 */

#include "RCSServerCallback.h"
#include "../UDPServer/UDPRCSServer.h"
#include "../UDPSocket.h"


#include <string.h>

namespace Lazarus {

RCSServerCallback::RCSServerCallback() {

	mp_nodes = new Lazarus::NCSList<Lazarus::TinyNode*>();

}

RCSServerCallback::~RCSServerCallback() {

	//delete the nodes
	mp_nodes->destroy();

	DELETE_NULL_CHECKING(mp_nodes);

}

Lazarus::NCSList<Lazarus::TinyNode*>* RCSServerCallback::getNodes()
{
	return mp_nodes;
}

/*
 * iterates over all registered tinynode objects and compares the corresponding ip addresses
 */
bool RCSServerCallback::checkIfNodeExists(std::string ip)
{
	bool indicator = false;

	//yeah its slow but the ip address is the only unique feature here
	Lazarus::GenNCSListElement<Lazarus::TinyNode*>* list_element = mp_nodes->getFirst();

	while(list_element != NULL)
	{
		//check if ip addresses match
		if( ip.compare(list_element->getm_data()->get_m_ip_address()) == 0 )
		{
			indicator = true;
			break;
		}

		list_element = list_element->getmp_neighbor();
	}

	return indicator;
}

int RCSServerCallback::call(Lazarus::Thread* t, void* var)
{

	/*
	 * pthread_t t_id, unsigned int local_thread_id, unsigned int actual_buffer_content_length,
		Lazarus::Buffer* buffer, void* socket_
	 * */

	struct RCSCallbackData* cdata = (struct RCSCallbackData*) var;

	enum UDPRCSServer::RCS_MESSAGE_TYPE type = UDPRCSServer::getType(cdata->mp_data_buffer);
	unsigned int size = sizeof(UDPRCSServer::RCS_MESSAGE_TYPE);
	Lazarus::TinyNode* node = NULL;

	switch(type)
	{
	case UDPRCSServer::RCS_MESSAGE_TYPE_CLIENT_AD_RESPONSE:
		printf("RCSServer-callback received client ad-response: %d bytes\n",cdata->m_actual_buffer_content_length);

		//deserialize the node
		node = new Lazarus::TinyNode();

		node->set_mp_serialized_data_buffer( new Lazarus::Buffer(cdata->mp_data_buffer->get_mp_data()+size, cdata->m_actual_buffer_content_length-size,true) );

		node->deserialize();

		//check if the node exists
		if( checkIfNodeExists( node->get_m_ip_address() ) == true)
		{
			printf("ERROR: will not add new tiny node (already exists)\n");
			delete node;
			return -1;
		}
		else
		{
			mp_nodes->appendLast(node);
		}

		break;

	case UDPRCSServer::RCS_MESSAGE_TYPE_CLIENT_AD_NO_CONNECT_RESPONSE:
		printf("RCSServer-callback received client ad-NO_CONNECT-response: %d bytes\n",cdata->m_actual_buffer_content_length);

		//deserialize the node
		node = new Lazarus::TinyNode();

		node->set_mp_serialized_data_buffer( new Lazarus::Buffer(cdata->mp_data_buffer->get_mp_data()+size, cdata->m_actual_buffer_content_length-size,true) );

		node->deserialize();

		//check if the node exists
		if( checkIfNodeExists( node->get_m_ip_address() ) == true)
		{
			printf("ERROR: will not add new tiny node (already exists)\n");
			delete node;
			return -1;
		}
		else
		{
			mp_nodes->appendLast(node);
		}

		break;

	default:
		printf("ERROR: unknown RCS message\n");
		break;
	}

	return 0;
}

} /* namespace Lazarus */
