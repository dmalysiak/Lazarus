/*
 * SharedMemSHUComStruct.h
 *
 *  Created on: 09.01.2015
 *      Author: Darius Malysiak
 */

#ifndef SHAREDMEMSHUCOMSTRUCT_H_
#define SHAREDMEMSHUCOMSTRUCT_H_

#include <pthread.h>
#include <semaphore.h>

namespace Lazarus {

/**
 * This com struct will be filled in tandem by the clusternode management client and the deployed shu!
 */

struct SharedMemSHUComStruct
{
	//the shu unique id (c-string)
	unsigned char m_shu_id[512];

	//the complete path to the shu binary (c-string)
	unsigned char m_shu_binary_filename[512];

	//the complete path to the system sqlite database (c-string)
	unsigned char m_system_db_filename[512];

	//the complete deployment path (c-string)
	unsigned char m_system_deployment_path[512];

	//the pid of the deployed shu
	unsigned long long m_shu_pid;

	//indicate a rapid deployment
	unsigned char m_rapid_deployment;

	//indicate a synchronized deployment
	unsigned char m_synchronized_deployment;

	//the node ID
	unsigned int m_node_id;

	//ip and port of the shu server
	unsigned char mp_shu_server_ip[64]; //(c-string)
	unsigned int m_shu_server_port;

	//a synchronization semaphore
	sem_t m_deployment_sem;

};

} /* namespace Lazarus */

#endif /* SHAREDMEMSHUCOMSTRUCT_H_ */
