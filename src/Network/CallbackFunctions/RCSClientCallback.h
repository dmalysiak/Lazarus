/*
 * RCSClientCallback.h
 *
 *  Created on: Aug 30, 2013
 *      Author: Darius Malysiak
 */

#ifndef RCSCLIENTCALLBACK_H_
#define RCSCLIENTCALLBACK_H_

#include "../../Network/GenericCallback.h"
#include "../../DataStructures/FastVectorN.h"
#include "../../DataStructures/FastKTuple.h"

#include <pthread.h>
#include <string>

namespace Lazarus {


class RCSClientCallback: public Lazarus::GenericCallback {
public:
	RCSClientCallback();
	virtual ~RCSClientCallback();

	virtual int call(Lazarus::Thread* t,	void* var);

	/**
	 * Sets the group ids to which the client belongs.
	 */
	void setGroupIDs(const Lazarus::FastKTuple<unsigned int>& group_ids);

	/**
	 * Will return <m_server_ip, m_server_port, m_advertised_port, m_server_information_update_counter>
	 */
	Lazarus::FastVector4<std::string,unsigned int, unsigned int, unsigned int>* getServerInfo();

private:
	std::string m_server_ip;
	unsigned int m_advertised_port;
	unsigned int m_incoming_rcs_client_group;
	unsigned int m_server_port;
	pthread_mutex_t m_mutex;
	unsigned int m_server_information_update_counter;
	Lazarus::FastKTuple<unsigned int> m_group_ids;

};

} /* namespace Lazarus */
#endif /* RCSCLIENTCALLBACK_H_ */
