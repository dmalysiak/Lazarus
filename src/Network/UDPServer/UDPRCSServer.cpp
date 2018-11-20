/*
 * UDPRCSServer.cpp
 *
 *  Created on: Aug 30, 2013
 *      Author: Darius Malysiak
 */

#include "UDPRCSServer.h"
#include "../../Toolbox.h"

#include <string.h>

namespace Lazarus {

UDPRCSServer::UDPRCSServer(std::string local_ip, std::string netmask, unsigned int local_port, std::string device, enum Socket::ADDRESS_TYPE address_type,
		unsigned int client_port, unsigned int advertised_port, unsigned int rcs_client_group, unsigned int advertise_interval_ms,
		unsigned int advertise_recv_timeout_ms, int poll_count, unsigned int use_RCS_target_list, Lazarus::FastKTuple<std::string> target_list):
		UDPServer(local_ip,netmask, local_port, device, address_type)
{

	m_current_state = RCS_SERVER_STATE_ADVERTISE;
	m_client_port = client_port;
	m_advertised_port = advertised_port;
	m_rcs_client_group = rcs_client_group;
	m_poll_count = poll_count;
	m_use_connection_beacons = true;
	m_is_active = false;

	//this is IPv4 specific!
	struct in_addr host, mask, bc_temp;

	if(inet_pton(AF_INET, local_ip.c_str(), &host) == 1 &&
		inet_pton(AF_INET, netmask.c_str(), &mask) == 1)
	{
		bc_temp.s_addr = host.s_addr | ~mask.s_addr;
	}

	char bc_ip[INET6_ADDRSTRLEN];

	inet_ntop(AF_INET, &bc_temp, bc_ip, INET_ADDRSTRLEN);

	m_broadcast_address = std::string(bc_ip);
	printf("Calculated BC address: %s\n",m_broadcast_address.c_str());

	m_advertise_interval_ms = advertise_interval_ms;
	m_advertise_recv_timeout_ms = advertise_recv_timeout_ms;

	m_target_list = target_list;

	if(use_RCS_target_list == 1)
	{
		m_use_target_list = true;
		m_poll_count = poll_count*m_target_list.getm_size();
	}
	else
	{
		m_use_target_list = false;
	}
}

UDPRCSServer::~UDPRCSServer() {
	// TODO Auto-generated destructor stub
}

const Lazarus::LogBook* UDPRCSServer::get_log() const
{
	return &m_logbook;
}

void UDPRCSServer::setUseConnectRequestBeacons(bool v)
{
	m_use_connection_beacons = v;
}

bool UDPRCSServer::isActive()
{
	if(m_is_active == true)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/*
 * the state machine can be described as follows:
 * - the server begins by sending an advertisement
 * - it waits advertise_recv_timeout_ms ms for a response
 * - if it received a response the registered callback object will be called
 * - otherwise the server waits for advertise_interval_ms ms and sends a new advertisement
 *
 * in case of an activated single poll mode the server will exit the state machine, this will
 * commence a shutdown of the executing thread. Keep in mind that the server can be started
 * again by a simple call to the thread-class's start() method. As the server object is not destroyed,
 * all registered nodes remain persistent even in the case of a restart. Keep in mind, >while< a server object
 * is in single poll mode; never attempt to shut down the server with thread-joining methods! The
 * reason lies in the fact that used thread ids might be reclaimed by the OS. Thus in the scenario where a
 * single-poll server terminated its executing thread, calling a 'shutdownJoin' might cause the caller to wait
 * infinitely long as the old thread id (i.e. the id of the exited thread) might have been reclaimed for
 * another system thread (which might run forever).
 */
void UDPRCSServer::run()
{
	int result = 0;
	int poll_counter = 0;

	m_is_active = true;

	unsigned int size = sizeof(UDPRCSServer::RCS_MESSAGE_TYPE);

	while(m_shutdown_flag == false)
	{
		switch(m_current_state)
		{
			case RCS_SERVER_STATE_ADVERTISE:

				Lazarus::Toolbox::sleep_ms(m_advertise_interval_ms);

				if(m_use_target_list == false)
				{
					//printf("sending UDP data with ad port %u\n",m_advertised_port);
					if(m_use_connection_beacons == true)
					{
						setType(mp_socket->getSendBuffer(),UDPRCSServer::RCS_MESSAGE_TYPE_SERVER_ADVERTISEMENT);
					}
					else
					{
						setType(mp_socket->getSendBuffer(),UDPRCSServer::RCS_MESSAGE_TYPE_SERVER_ADVERTISEMENT_NO_CONNECT);
					}
					memcpy(mp_socket->getSendBuffer()->get_mp_data()+size,
							&m_advertised_port,sizeof(unsigned int));
					memcpy(mp_socket->getSendBuffer()->get_mp_data()+size+sizeof(unsigned int),
							&m_rcs_client_group,sizeof(unsigned int));
					mp_socket->sendBroadcast(m_client_port,size+2*sizeof(unsigned int),m_broadcast_address);
					int res = m_current_state = RCS_SERVER_STATE_WAIT_FOR_DATA;
					if(res > 0)
					{
						//printf("Sent UDP broadcast\n");
						++poll_counter;
					}
					else
					{
						printf("ERROR: could not send UDP broadcast\n");
					}
				}
				else//poll all elements from the list
				{
					for(unsigned int i=0;i<m_target_list.getm_size();++i)
					{
						//printf("sending UDP data with ad port %u\n",m_advertised_port);
						if(m_use_connection_beacons == true)
						{
							setType(mp_socket->getSendBuffer(),UDPRCSServer::RCS_MESSAGE_TYPE_SERVER_ADVERTISEMENT);
						}
						else
						{
							setType(mp_socket->getSendBuffer(),UDPRCSServer::RCS_MESSAGE_TYPE_SERVER_ADVERTISEMENT_NO_CONNECT);
						}
						memcpy(mp_socket->getSendBuffer()->get_mp_data()+size,
								&m_advertised_port,sizeof(unsigned int));
						memcpy(mp_socket->getSendBuffer()->get_mp_data()+size+sizeof(unsigned int),
								&m_rcs_client_group,sizeof(unsigned int));
						printf("Sending unicast UDP beacon to %s\n",m_target_list[i].c_str());
						int res = mp_socket->send(m_client_port,size+2*sizeof(unsigned int),(m_target_list[i]));
						if(res > 0)
						{
							//printf("Sent UDP unicast\n");
							++poll_counter;
						}
						else
						{
							printf("ERROR: could not send UDP unicast\n");
						}
					}
					m_current_state = RCS_SERVER_STATE_WAIT_FOR_DATA;
				}
				//m_logbook.addEntry("sent an advertisement broadcast -> will wait for answers");
				break;

			case RCS_SERVER_STATE_WAIT_FOR_DATA:
				result = mp_socket->receiveFromWho(m_advertise_recv_timeout_ms);
				if(result > 0)
				{
					//we assume that the callback object is an UDPRCS callback
					m_data.m_actual_buffer_content_length = result;
					m_data.mp_socket = mp_socket;
					m_data.mp_data_buffer = mp_socket->getRecvBuffer();

					mp_callback->call(this,&m_data);//the server is a thread itself

					//m_logbook.addEntry("received a response -> will wait for more messages");

					//exit the server in case of the finite poll mode
					if(m_poll_count != -1 && poll_counter >= m_poll_count)
					{
						m_shutdown_flag = true;
						m_thread_self_terminated = true;
						break;
					}

					//exits the state handler context and will reenter the same context in the next iteration of the while loop
					break;
				}
				else
				{
					//m_logbook.addEntry("timeout or error receiving an udp response -> will send another advertisement");
					m_current_state = RCS_SERVER_STATE_ADVERTISE;

					//even in the unsuccessful case; exit the server in case of the finite poll mode
					if(m_poll_count != -1 && poll_counter >= m_poll_count)
					{
						m_shutdown_flag = true;
						m_thread_self_terminated = true;
						break;
					}
				}

				break;

			default:
				printf("ERROR: something went horribly wrong\n");
				break;
		}
	}
}

void UDPRCSServer::getContent( Lazarus::Buffer* message, unsigned char** data, unsigned int &length )
{
	unsigned int type_size = sizeof(enum UDPRCSServer::RCS_MESSAGE_TYPE);

	length = message->get_m_length()-type_size;

	*data = new unsigned char[length];

	memcpy(*data,message->get_mp_data()+type_size,length*sizeof(unsigned char));
}

void UDPRCSServer::setContent( Lazarus::Buffer* message, unsigned char* data, unsigned int length )
{
	unsigned int type_size = sizeof(enum UDPRCSServer::RCS_MESSAGE_TYPE);

	memcpy(message->get_mp_data()+type_size,data,length*sizeof(unsigned char));
}

enum UDPRCSServer::RCS_MESSAGE_TYPE UDPRCSServer::getType( Lazarus::Buffer* message )
{
	unsigned int size = sizeof(enum UDPRCSServer::RCS_MESSAGE_TYPE);
	enum UDPRCSServer::RCS_MESSAGE_TYPE type = RCS_MESSAGE_TYPE_DUMMY;

	memcpy(&type,message->get_mp_data(),size);

	return type;
}

void UDPRCSServer::setType( Lazarus::Buffer* message, enum UDPRCSServer::RCS_MESSAGE_TYPE type )
{
	unsigned int size = sizeof(enum UDPRCSServer::RCS_MESSAGE_TYPE);

	memcpy(message->get_mp_data(),&type,size);

}

} /* namespace Lazarus */
