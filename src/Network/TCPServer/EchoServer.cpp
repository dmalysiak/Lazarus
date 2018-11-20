/*
 * EchoServer.cpp
 *
 *  Created on: Aug 18, 2013
 *      Author: Darius Malysiak
 */

#include "EchoServer.h"
#include "../../Threading/MutexLocker.h"
#include "../../Toolbox.h"

namespace Lazarus {

EchoServer::EchoServer(unsigned int cpu_count, Lazarus::ClusterLibFrame* frame, pthread_mutex_t *mutex, bool* external_client_error_flag) {

	mp_frame = frame;
	mp_mutex = mutex;

	m_type_size = sizeof(enum ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE);

	mp_status = new Lazarus::SystemStatus();

	m_external_client_error_flag = external_client_error_flag;
}

EchoServer::~EchoServer()
{
	DELETE_NULL_CHECKING(mp_status);
}

void EchoServer::run()
{
	while(m_shutdown_flag != true)
	{

		//acquire exclusive communication rights
		Lazarus::MutexLocker locker(mp_mutex);
		locker.lock();

		//get the system status
		//read the load
		mp_status->getSystemStatus();
		mp_status->serialize();
		//send the status object
		Lazarus::Buffer response(mp_status->get_mp_serialized_data_buffer()->get_m_length()+m_type_size);

		ClusterNodeManagementServer::setRequestType(&response,
				ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_ANSWER_SYSTEM_STATE);

		ClusterNodeManagementServer::setContent(&response,
				mp_status->get_mp_serialized_data_buffer()->get_mp_data(),
				mp_status->get_mp_serialized_data_buffer()->get_m_length());

		//send system state to server, if sending has failed, stop the echo server and indicate the error to the mgmt client
		if(mp_frame->sendFrame(&response) == -1)
		{
			m_shutdown_flag = true;
			m_thread_self_terminated = true;
			*m_external_client_error_flag = true;
			locker.unlock();
			continue;
		}

		//unlock the mutex, thus enabling the management client to communicate
		locker.unlock();

		//wait some time before advertising the status again
		Lazarus::Toolbox::sleep_ms(m_advertise_interval_ms);


	}
}

} /* namespace Lazarus */
