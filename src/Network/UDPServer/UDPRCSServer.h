/*
 * UDPRCSServer.h
 *
 *  Created on: Aug 30, 2013
 *      Author: Darius Malysiak
 */

#ifndef UDPRCSSERVER_H_
#define UDPRCSSERVER_H_

#include "../UDPServer.h"
#include "../../Buffer.h"
#include "../../DataStructures/FastKTuple.h"
#include "../../LogBook.h"
#include "../CallbackFunctions/RCSServerCallback.h"

#include <string>
#include <atomic>

namespace Lazarus {

/*
 * Remote Configuration Services Server
 *
 * Advertises himself (ip and 'advertised_port'), thus provides configuration services to clients
 *
 */

class UDPRCSServer: public Lazarus::UDPServer {
public:
	UDPRCSServer(std::string local_ip, std::string netmask, unsigned int local_port, std::string device, enum Socket::ADDRESS_TYPE address_type,
			unsigned int client_port, unsigned int advertised_port, unsigned int rcs_client_group, unsigned int advertise_interval_ms = 5000,
			unsigned int advertise_recv_timeout_ms = 500, int poll_count = 50, unsigned int use_RCS_target_list=0,
			Lazarus::FastKTuple<std::string> target_list = Lazarus::FastKTuple<std::string>(1));
	virtual ~UDPRCSServer();

	const Lazarus::LogBook* get_log() const;
	virtual void run();

	/**
	 * By default the server will broadcast/unicast beacons with connect requests, i.e. the recipient
	 * shall connect via a TCP mgmt client. Setting this to false will result in survey beacons, i.e.
	 * broadcast/unicast beacons which do not induce a connection attempt by the recipient!
	 * Yet this prevention of connection attempts is done indirectly; the RCS client simply won't keep track of
	 * servers from whom he received a beacon. Thus upon query the client won't show the server (except the server was
	 * registered before).
	 * */
	void setUseConnectRequestBeacons(bool v);

	enum RCS_MESSAGE_TYPE{ RCS_MESSAGE_TYPE_SERVER_ADVERTISEMENT=1,RCS_MESSAGE_TYPE_CLIENT_AD_RESPONSE,
		RCS_MESSAGE_TYPE_DUMMY, RCS_MESSAGE_TYPE_SERVER_ADVERTISEMENT_NO_CONNECT, RCS_MESSAGE_TYPE_CLIENT_AD_NO_CONNECT_RESPONSE};

	enum RCS_SERVER_STATE{ RCS_SERVER_STATE_ADVERTISE, RCS_SERVER_STATE_WAIT_FOR_DATA,RCS_SERVER_STATE_DUMMY };

	static enum UDPRCSServer::RCS_MESSAGE_TYPE getType( Lazarus::Buffer* message );
	static void setType( Lazarus::Buffer* message, enum UDPRCSServer::RCS_MESSAGE_TYPE type );

	static void getContent( Lazarus::Buffer* message, unsigned char** data, unsigned int &length );
	static void setContent( Lazarus::Buffer* message, unsigned char* data, unsigned int length );

	/**
	 * Indicates whether the server thread has started
	 * */
	bool isActive();

private:
	std::atomic_bool m_is_active;
	enum RCS_SERVER_STATE m_current_state;
	unsigned int m_client_port;
	unsigned int m_advertised_port;
	unsigned int m_rcs_client_group;
	std::string m_broadcast_address;
	Lazarus::LogBook m_logbook;
	RCSCallbackData m_data;
	int m_poll_count;
	bool m_use_connection_beacons;

	unsigned int m_advertise_interval_ms;
	unsigned int m_advertise_recv_timeout_ms;
	bool m_use_target_list;
	Lazarus::FastKTuple<std::string> m_target_list;
};

} /* namespace Lazarus */
#endif /* UDPRCSSERVER_H_ */
