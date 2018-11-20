/*
 * UDPRCSClient.h
 *
 *  Created on: Aug 30, 2013
 *      Author: Darius Malysiak
 */

#ifndef UDPRCSCLIENT_H_
#define UDPRCSCLIENT_H_

#include "../UDPClient.h"
#include "../CallbackFunctions/RCSServerCallback.h"

namespace Lazarus {

class UDPRCSClient: public UDPClient {
public:
	UDPRCSClient(unsigned int local_port, std::string device, enum Socket::ADDRESS_TYPE address_type );
	virtual ~UDPRCSClient();

	virtual void run();


	enum RCS_CLIENT_STATE{RCS_CLIENT_STATE_WAIT_FOR_DATA,RCS_CLIENT_STATE_DUMMY };

private:
	enum UDPRCSClient::RCS_CLIENT_STATE m_current_state;
	RCSCallbackData m_data;

};

} /* namespace Lazarus */
#endif /* UDPRCSCLIENT_H_ */
