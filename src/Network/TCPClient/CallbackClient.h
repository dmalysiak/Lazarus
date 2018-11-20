/*
 * ClusterNodeManagementClient.h
 *
 *  Created on: 22.03.2013
 *      Author: Darius Malysiak
 */

#ifndef CALLBACKCLIENT_H_
#define CALLBACKCLIENT_H_

#include "../../Threading/Thread.h"
#include "../../Network/TCPConnection.h"
#include "../../Network/Protocols/ClusterLib/ClusterLibFrame.h"
#include "../../Network/TCPServer/CallbackServer.h"
#include "../../Network/GenericCallback.h"

#include <string.h>
#include <string>

namespace Lazarus
{

class CallbackClient: public Lazarus::Thread {
public:
	CallbackClient(std::string remote_address,
			unsigned int remote_port, unsigned int nodeID, GenericCallback* callback,
			unsigned int max_attempts = 20, unsigned int attempt_wait_time = 1000);

	virtual ~CallbackClient();

	virtual void run();

	enum CALLBACK_CLIENT_MANAGEMENT_STATE{CALLBACK_CLIENT_STATE_IDLE,CALLBACK_CLIENT_STATE_NODE_ACTIVE,
		CALLBACK_CLIENT_STATE_UNINITIALIZED};

	ClusterLibFrame* get_mp_frame();

private:
	void unregisterClient();

	TCPConnection* mp_connection;
	ClusterLibFrame* mp_frame;
	enum CallbackClient::CALLBACK_CLIENT_MANAGEMENT_STATE m_current_state;

	void parseCommand(Lazarus::Buffer* message);
	GenericCallback* mp_callback;

	unsigned int m_node_id;

	//local vars for faster callback calls
	CallbackServer::CALLBACK_SERVER_REQUEST_TYPE m_type;
};

}

#endif /* CALLBACKCLIENT_H_ */
