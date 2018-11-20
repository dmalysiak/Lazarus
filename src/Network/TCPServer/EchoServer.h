/*
 * EchoServer.h
 *
 *  Created on: Aug 18, 2013
 *      Author: Darius Malysiak
 */

#ifndef ECHOSERVER_H_
#define ECHOSERVER_H_

#include "../../Threading/Thread.h"
#include "../TCPConnection.h"
#include "../Protocols/ClusterLib/ClusterLibFrame.h"
#include "ClusterNodeManagementServer.h"
#include "../../System/SystemStatus.h"

namespace Lazarus {

class EchoServer: public Lazarus::Thread {
public:
	EchoServer(unsigned int cpu_count, Lazarus::ClusterLibFrame* frame, pthread_mutex_t *mutex, bool* external_client_error_flag);
	virtual ~EchoServer();

	virtual void run();

	static const int m_advertise_interval_ms = 3000;

private:
	Lazarus::ClusterLibFrame* mp_frame;
	pthread_mutex_t* mp_mutex;
	unsigned int m_type_size;

	Lazarus::SystemStatus* mp_status;
	bool* m_external_client_error_flag;
};

} /* namespace Lazarus */
#endif /* ECHOSERVER_H_ */
