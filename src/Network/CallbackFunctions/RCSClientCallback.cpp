/*
 * RCSClientCallback.cpp
 *
 *  Created on: Aug 30, 2013
 *      Author: Darius Malysiak
 */

#include "RCSClientCallback.h"
#include "../../Network/UDPSocket.h"
#include "../../Network/UDPServer/UDPRCSServer.h"
#include "../../System/TinyNode.h"

namespace Lazarus {

RCSClientCallback::RCSClientCallback() {

	m_server_port = 0;
	m_server_information_update_counter = 0;
	m_advertised_port = 0;
	m_incoming_rcs_client_group = 0;

	pthread_mutex_init(&m_mutex,NULL);

}

RCSClientCallback::~RCSClientCallback() {

	pthread_mutex_unlock(&m_mutex);
	pthread_mutex_destroy(&m_mutex);

}

void RCSClientCallback::setGroupIDs(const Lazarus::FastKTuple<unsigned int>& group_ids)
{
	m_group_ids = group_ids;
}

Lazarus::FastVector4<std::string,unsigned int, unsigned int, unsigned int>* RCSClientCallback::getServerInfo()
{
	Lazarus::FastVector4<std::string,unsigned int, unsigned int, unsigned int>* info;
	pthread_mutex_lock(&m_mutex);

	info = new Lazarus::FastVector4<std::string,unsigned int, unsigned int, unsigned int>();
	info->m_data1 = m_server_ip;
	info->m_data2 = m_server_port;
	info->m_data3 = m_advertised_port;
	info->m_data4 = m_server_information_update_counter;

	pthread_mutex_unlock(&m_mutex);

	return info;
}


int RCSClientCallback::call(Lazarus::Thread* t,	void* var)
{
	struct RCSCallbackData* cdata = (struct RCSCallbackData*) var;

	enum UDPRCSServer::RCS_MESSAGE_TYPE type = UDPRCSServer::getType(cdata->mp_data_buffer);
	unsigned int size = sizeof(UDPRCSServer::RCS_MESSAGE_TYPE);
	UDPSocket* udp_socket = cdata->mp_socket;
	Lazarus::TinyNode* node = NULL;

	switch(type)
	{
	case UDPRCSServer::RCS_MESSAGE_TYPE_SERVER_ADVERTISEMENT:
	{
		printf("received server ad \n");
		//create the node
		node = new Lazarus::TinyNode(udp_socket->m_local_address,udp_socket->m_local_port, 0,0);
		node->serialize();

		printf("Server info: %s %d\n",udp_socket->m_remote_address.c_str(), udp_socket->m_remote_port);


		//check if this client can accept the advertisement
		bool id_ok = false;
		for(unsigned int i=0;i<m_group_ids.getm_size();++i)
		{
			//if at least one of the clients group ids equals the incoming id
			if(m_group_ids[i] == m_incoming_rcs_client_group)
			{
				printf("Client accepts the beacon, client rcs group %u fits requested group %u\n",
						m_group_ids[i], m_incoming_rcs_client_group);
				id_ok = true;
				break;
			}
		}
		//abort if not
		if(id_ok == false)
		{
			printf("Can't accept the request, incoming group id does not fit the client.\n");
			break;
		}

		pthread_mutex_lock(&m_mutex);

		//get the ad info
		m_server_ip = udp_socket->m_remote_address;
		m_server_port = udp_socket->m_remote_port;
		memcpy(&m_advertised_port, cdata->mp_data_buffer->get_mp_data()+ sizeof(UDPRCSServer::RCS_MESSAGE_TYPE),
				sizeof(unsigned int));printf("RCS advertised port %u\n",m_advertised_port);
		memcpy(&m_incoming_rcs_client_group,
				cdata->mp_data_buffer->get_mp_data() + sizeof(UDPRCSServer::RCS_MESSAGE_TYPE) +sizeof(unsigned int),
				sizeof(unsigned int) );printf("RCS incoming client group %u\n",m_incoming_rcs_client_group);
		m_server_information_update_counter++;
		pthread_mutex_unlock(&m_mutex);

		//send the response
		UDPRCSServer::setType(udp_socket->getSendBuffer(),UDPRCSServer::RCS_MESSAGE_TYPE_CLIENT_AD_RESPONSE);
		UDPRCSServer::setContent(udp_socket->getSendBuffer(),node->get_mp_serialized_data_buffer()->get_mp_data(),
				node->get_mp_serialized_data_buffer()->get_m_length() );

		udp_socket->send(udp_socket->m_remote_port,size+node->get_mp_serialized_data_buffer()->get_m_length(),m_server_ip);

		delete node;

		break;
	}

	case UDPRCSServer::RCS_MESSAGE_TYPE_SERVER_ADVERTISEMENT_NO_CONNECT:
	{
		printf("received server ad (no connect) \n");
		//create the node
		node = new Lazarus::TinyNode(udp_socket->m_local_address,udp_socket->m_local_port, 0,0);
		node->serialize();

		printf("Server info: %s %d\n",udp_socket->m_remote_address.c_str(), udp_socket->m_remote_port);

		//check if this client can accept the advertisement
		bool id_ok = false;
		for(unsigned int i=0;i<m_group_ids.getm_size();++i)
		{
			//if at least one of the clients group ids equals the incoming id
			if(m_group_ids[i] == m_incoming_rcs_client_group)
			{
				printf("Client accepts the no-connect beacon, client rcs group %u fits requested group %u\n",
										m_group_ids[i], m_incoming_rcs_client_group);
				id_ok = true;
				break;
			}
		}
		//abort if not
		if(id_ok == false)
		{
			printf("Can't accept the request, incoming group id does not fit the client.\n");
			break;
		}

		//send the response
		UDPRCSServer::setType(udp_socket->getSendBuffer(),UDPRCSServer::RCS_MESSAGE_TYPE_CLIENT_AD_NO_CONNECT_RESPONSE);
		UDPRCSServer::setContent(udp_socket->getSendBuffer(),node->get_mp_serialized_data_buffer()->get_mp_data(),
				node->get_mp_serialized_data_buffer()->get_m_length() );

		udp_socket->send(udp_socket->m_remote_port,size+node->get_mp_serialized_data_buffer()->get_m_length(),m_server_ip);

		delete node;

		break;
	}

	default:
		printf("ERROR: unknown RCS message\n");
		break;
	}

	return 0;
}

} /* namespace Lazarus */
