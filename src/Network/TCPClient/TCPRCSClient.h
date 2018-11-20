/*
 * TCPRCSServer.h
 *
 *  Created on: 09.03.2015
 *      Author: Darius Malysiak
 */

#ifndef TCPRCSCLIENT_H_
#define TCPRCSCLIENT_H_

#include "../../BaseObject.h"

#include "../TCPSocket.h"
#include "../TCPConnectionPoll.h"
#include "../Protocols/ClusterLib/ClusterLibFrame.h"

namespace Lazarus {

/**
 * A class which essentially provides a TCP (poll-based) P2P endpoint (i.e. only one connection will be accepted).
 * Once the thread stopped, it can not be restarted again.
 * */

class TCPRCSClient: public Lazarus::BaseObject {
public:

	TCPRCSClient(int read_frame_timeout = -1);
	virtual ~TCPRCSClient();

	/**
	 * Attempts to connect to the given host, returns 0 in case of success, -1 otherwise.
	 * */
	int connect(std::string remote_address, unsigned int remote_port,
			const std::string& local_ip="", unsigned int local_port=0, const std::string& local_device="");

	/**
	 * Request the mgmt client to connect to the server. Returns -1 in case of errors,
	 * */
	int sendMGMTConRequest(const std::string& mgmt_server_address, unsigned int mgmt_server_port, unsigned int rcs_group);

	/**
	 * Request an update of SH. The semantics are as follows:
	 * - Once the SH client has acknowledged the update request, this method will return with 0,
	 * in case of errors with -1.
	 * - The SH client will commence the update procedure, which after some time will lead to a global connection
	 * loss with the SH client.
	 * - The user should attempt to contact the client(s) afterwards, in other words, it's the users
	 * responsibility to check if the SH client(s) are accessible again. This could be done with e.g. UDP broad-/uni-casts
	 * or TCPRCS connection attempts.
	 * The repo_server_address string should be of the form https://repo.de:PORT/.../trunk.
	 * The repo folder string should be a persistent and writable >absolute< folder on the TCPRCSServer host, if one desires to use the
	 * hosts default value, then he should specify a value of "" for this string!
	 * The update value determines if the svn access will be an update or a checkout, an update requires that the
	 * repo folder contains an already checked out version. In case of 'false' the content of the update folder will be purged
	 * before the checkout!
	 * sh_module_level determines the amount of modules which will be built during the update:
	 * 0 = minimal required module configuration (Core, Network, XML)
	 * 1 = minimal + OpenCL + image processing + OpenCL image processing
	 * 2 = full SH except for the unit tests
	 * The value of keep_client_config determines if the clients config file should be kept after the update, in case of
	 * 'false' the config file will be replaced with the default file.
	 * */
	int sendUpdateRequest(const std::string& repo_server_address, const std::string& checkout_folder,
			const std::string& repo_user, const std::string& password,
			bool update, bool keep_client_config,
			unsigned int sh_module_level=0);

private:
	TCPConnectionPoll* mp_client_connection;
	ClusterLibFrame* mp_frame;

	std::string m_remote_ip;
	unsigned int m_remote_port;

	int m_read_frame_timeout;
	bool m_connected;
};

} /* namespace Lazarus */

#endif /* TCPRCSSERVER_H_ */
