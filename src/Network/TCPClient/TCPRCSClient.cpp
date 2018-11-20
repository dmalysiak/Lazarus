/*
 * TCPRCSServer.cpp
 *
 *  Created on: 09.03.2015
 *      Author: Darius Malysiak
 */

#include "TCPRCSClient.h"

#include "../TCPServer/TCPRCSServer.h"
#include "../../SerializationStack.h"

namespace Lazarus {

TCPRCSClient::TCPRCSClient(int read_frame_timeout) {

	mp_client_connection = NULL;

	mp_frame = NULL;
	m_remote_port = 0;
	m_remote_ip = "";

	m_read_frame_timeout = read_frame_timeout;
	m_connected = false;

}

TCPRCSClient::~TCPRCSClient()
{
	DELETE_NULL_CHECKING(mp_frame);
	DELETE_NULL_CHECKING(mp_client_connection);
}

int TCPRCSClient::connect(std::string remote_address, unsigned int remote_port, const std::string& local_ip,
		unsigned int local_port, const std::string& local_device) {

	DELETE_NULL_CHECKING(mp_frame);
	DELETE_NULL_CHECKING(mp_client_connection);
	mp_client_connection = new TCPConnectionPoll(0);

	int res = mp_client_connection->connect(remote_port,remote_address,local_ip,local_port,local_device);

	if(res != 0)
	{
		DELETE_NULL_CHECKING(mp_frame);
		DELETE_NULL_CHECKING(mp_client_connection);
		m_connected = false;
		return -1;
	}
	else
	{
		mp_frame = new ClusterLibFrame(mp_client_connection);
		m_remote_port = remote_port;
		m_remote_ip = remote_address;
		m_connected = true;

		return 0;
	}

	return 0;
}

int TCPRCSClient::sendMGMTConRequest(const std::string& mgmt_server_address, unsigned int mgmt_server_port, unsigned int rcs_group)
{
	if(m_connected == false)
	{
		printf("ERROR: TCPRCSClient not connected\n");
		return -1;
	}

	Lazarus::SerializationStack data;
	data.registerElement<unsigned int>(2);//port, rcs group
	data.registerElement<enum TCPRCSServer::TCPRCS_MESSAGE_TYPE>(1);//type
	data.registerString(mgmt_server_address);//ip
	data.sealStack();

	data.addElement<enum TCPRCSServer::TCPRCS_MESSAGE_TYPE>(TCPRCSServer::TCPRCS_MESSAGE_TYPE_CNMG_CON_REQUEST);
	data.addUInt(mgmt_server_port);
	data.addUInt(rcs_group);
	data.addString(mgmt_server_address);

	Lazarus::Buffer* buffer = data.getStackBuffer();

	//send the request
	int result = mp_frame->sendFrame(buffer);

	if(result==-1)
	{
		printf("ERROR: could not send mgmt connection request\n");
		return -1;
	}

	//wait for a response
	buffer = mp_frame->readFrame();

	if(buffer != NULL)//if no error occurred
	{
		enum TCPRCSServer::TCPRCS_MESSAGE_TYPE type = TCPRCSServer::getRequestType(buffer);
		delete buffer;

		//check the type
		if( type == TCPRCSServer::TCPRCS_MESSAGE_TYPE_CNMG_CON_RCSGROUP_ERROR)
		{
			printf("ERROR: rcs request declined: declined rcs group\n");
			return -1;
		}
		if( type != TCPRCSServer::TCPRCS_MESSAGE_TYPE_CNMG_CON_OK)
		{
			printf("ERROR: received not a mgmt connection request confirmation\n");
			return -1;
		}
	}
	else
	{
		return -1;
	}

	return 0;
}

int TCPRCSClient::sendUpdateRequest(const std::string& repo_server_address, const std::string& checkout_folder,
		const std::string& repo_user, const std::string& password,
		bool update, bool keep_client_config,
		unsigned int sh_module_level)
{
	if(m_connected == false)
	{
		printf("ERROR: TCPRCSClient not connected\n");
		return -1;
	}

	Lazarus::SerializationStack data;
	data.registerElement<unsigned int>(3);//update,sh_module_level,keep_client_config
	data.registerElement<enum TCPRCSServer::TCPRCS_MESSAGE_TYPE>(1);//type
	data.registerString(repo_server_address);//repo server ip
	data.registerString(checkout_folder);//checkout folder
	data.registerString(repo_user);//repo server ip
	data.registerString(password);//checkout folder
	data.sealStack();

	data.addElement<enum TCPRCSServer::TCPRCS_MESSAGE_TYPE>(TCPRCSServer::TCPRCS_MESSAGE_TYPE_SH_UPDATE_REQUEST);
	data.addUInt(update);
	data.addUInt(sh_module_level);
	data.addUInt(keep_client_config);
	data.addString(repo_server_address);
	data.addString(checkout_folder);
	data.addString(repo_user);
	data.addString(password);

	Lazarus::Buffer* buffer = data.getStackBuffer();

	//send the request
	int result = mp_frame->sendFrame(buffer);

	if(result==-1)
	{
		printf("ERROR: could not send SH update request\n");
		return -1;
	}

	//wait for a response
	buffer = mp_frame->readFrame();

	if(buffer != NULL) //if no error occurred
	{
		enum TCPRCSServer::TCPRCS_MESSAGE_TYPE type = TCPRCSServer::getRequestType(buffer);
		delete buffer;

		//check the type
		if( type == TCPRCSServer::TCPRCS_MESSAGE_TYPE_SH_UPDATE_RESPONSE_NO_WORKING_COPY)
		{
			printf("ERROR: could not update SH because no working copy was found on the host.\n");
			return -1;
		}
	}
	else
	{
		return -1;
	}

	return 0;
}


} /* namespace Lazarus */
