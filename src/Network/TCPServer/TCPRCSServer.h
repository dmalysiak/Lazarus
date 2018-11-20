/*
 * TCPRCSServer.h
 *
 *  Created on: 09.03.2015
 *      Author: Darius Malysiak
 */

#ifndef TCPRCSSERVER_H_
#define TCPRCSSERVER_H_

#include "../../Threading/Thread.h"
#include "../../DataStructures/FastVectorN.h"
#include "../../DataStructures/FastKTuple.h"

#include "../TCPSocket.h"
#include "../TCPConnectionPoll.h"
#include "../Protocols/ClusterLib/ClusterLibFrame.h"

namespace Lazarus {

/**
 * A class which essentially provides a TCP (poll-based) P2P endpoint (i.e. only one connection will be accepted).
 * Once the thread stopped, it can not be restarted again.
 * */

class TCPRCSServer: public Lazarus::Thread {
public:

	enum TCPRCSSERVER_STATE{TCPRCSSERVER_STATE_WAITING_FOR_CONNECTION,
		TCPRCSSERVER_STATE_ACTIVE,TCPRCSSERVER_STATE_UNDEFINED};

	enum TCPRCS_MESSAGE_TYPE{TCPRCS_MESSAGE_TYPE_CNMG_CON_REQUEST,
		TCPRCS_MESSAGE_TYPE_CNMG_CON_OK,TCPRCS_MESSAGE_TYPE_CNMG_CON_RCSGROUP_ERROR,
		TCPRCS_MESSAGE_TYPE_SH_UPDATE_REQUEST,TCPRCS_MESSAGE_TYPE_SH_UPDATE_RESPONSE_NO_WORKING_COPY,
		TCPRCS_MESSAGE_TYPE_SH_UPDATE_RESPONSE};

	/**
	 * In order to keep the server thread stoppable we force a timeout > 0.
	 * */
	TCPRCSServer(std::string local_address, unsigned int local_port, unsigned int listen_timeout = 250, unsigned int read_frame_timeout = 250,
			enum TCPSocket::ADDRESS_TYPE address_type = TCPSocket::ADDRESS_TYPE_IPV4, const std::string& interface="");
	virtual ~TCPRCSServer();

	/**
	 * Sets the group ids to which the client belongs.
	 */
	void setGroupIDs(const Lazarus::FastKTuple<unsigned int>& group_ids);


	virtual void run();

	/**
	 * Will return <m_server_ip, m_server_port, m_incoming_rcs_client_group,m_update_counter>
	 */
	Lazarus::FastVector4<std::string,unsigned int, unsigned int, unsigned int>* getServerInfo();


	/**
	 * Helper methods which extract/inject headers and content
	 */
	static enum TCPRCSServer::TCPRCS_MESSAGE_TYPE getRequestType(Lazarus::Buffer* message);
	static void setRequestType(Lazarus::Buffer* message,enum TCPRCSServer::TCPRCS_MESSAGE_TYPE type);
	static void setContent(Lazarus::Buffer* message,unsigned char* content, unsigned int size);
	/**
	 * this method will allocate an adequate buffer at 'content' and set 'size' accordingly
	 */
	static void getContent(Lazarus::Buffer* message,unsigned char** content, unsigned int &size);

	void setDefaultCheckoutFolder(const std::string& folder);


private:
	std::string m_advertised_server_ip;
	unsigned int m_advertised_port;
	unsigned int m_incoming_rcs_client_group;
	unsigned int m_update_counter;
	Lazarus::FastKTuple<unsigned int> m_group_ids;
	pthread_mutex_t m_mutex;

	TCPSocket* mp_socket;
	TCPConnectionPoll* mp_client_connection;
	ClusterLibFrame* mp_frame;

	std::string m_local_ip;
	unsigned int m_local_port;
	struct pollfd m_poll_in_fds;
	enum TCPRCSSERVER_STATE m_state;

	unsigned int m_listen_timeout;
	unsigned int m_read_frame_timeout;

	std::string m_default_repo_checkout_folder;

	/**
	 * This blocking method will wait for an incoming client connection, returns -1 in case of timeouts, 0 in case of success and
	 * -2 in case of serious errors!
	 * A timeout value of -1 will wait infinitely.
	 * */
	int waitForClient(int timeout = -1);

	void processMessage(Lazarus::Buffer* message);


};

} /* namespace Lazarus */

#endif /* TCPRCSSERVER_H_ */
