/*
 * UDPRCSClient.cpp
 *
 *  Created on: Aug 30, 2013
 *      Author: Darius Malysiak
 */

#include "UDPRCSClient.h"
#include "../../Network/UDPServer/UDPRCSServer.h"
#include "../../Network/UDPBCListenerSocket.h"

namespace Lazarus {

UDPRCSClient::UDPRCSClient(unsigned int local_port, std::string device, enum Socket::ADDRESS_TYPE address_type )
: UDPClient("255.255.255.255", local_port,device, address_type)
{
	//init the socket
	mp_socket = new UDPBCListenerSocket(local_port,device, address_type);
	mp_socket->bindSocket();
	mp_socket->enableNonblocking();

	m_current_state = UDPRCSClient::RCS_CLIENT_STATE_WAIT_FOR_DATA;
}

UDPRCSClient::~UDPRCSClient() {

	mp_socket->closeSocket();

}

void UDPRCSClient::run()
{
	int result = 0;

	while(m_shutdown_flag == false)
	{
		switch(m_current_state)
		{
			case UDPRCSClient::RCS_CLIENT_STATE_WAIT_FOR_DATA:
				result = mp_socket->receiveFromWho(250);//wait until some data arrives
				if(result > 0)
				{
					//we assume that the callback object is an UDPRCS callback
					m_data.m_actual_buffer_content_length = result;
					m_data.mp_socket = mp_socket;
					m_data.mp_data_buffer = mp_socket->getRecvBuffer();

					printf("UDPRCS client received %u bytes\n",result);

					/*for(int i=0;i<result;++i)printf("%u ",mp_socket->getRecvBuffer()->get_mp_data()[i]);
					printf("\n");*/

					mp_callback->call(this, &m_data);
				}
				else
				{
					//printf("UDPRCS client receive timeout\n");
				}
				break;

			default:
				printf("ERROR: something went horribly wrong (RCSClient)\n");
				break;
		}
	}
}

} /* namespace Lazarus */
