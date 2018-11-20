/*
 * ClusterNodeManagementClient.h
 *
 *  Created on: 22.03.2013
 *      Author: Darius Malysiak
 */

#ifndef CLUSTERNODEMANAGEMENTCLIENT_H_
#define CLUSTERNODEMANAGEMENTCLIENT_H_

#include "../../Threading/Thread.h"
#include "../TCPConnection.h"
#include "../Protocols/ClusterLib/ClusterLibFrame.h"
#include "../TCPServer/ClusterNodeManagementServer.h"
#include "../Nodes/ExtendedNetworkNode.h"
#include "../TCPServer/EchoServer.h"
#include "../../IPC/SharedMemoryPOSIX.h"
#include "../../IPC/SharedSemaphore.h"

#include <string.h>
#include <string>
#include <pthread.h>

#include <atomic>

namespace Lazarus
{

class ClusterNodeManagementClient: public Lazarus::Thread {
public:
	ClusterNodeManagementClient(std::string remote_address,
			unsigned int remote_port, std::string system_config_file, unsigned int max_attempts = 20, unsigned int attempt_wait_time = 1000);

	virtual ~ClusterNodeManagementClient();

	virtual void run();

	enum CLUSTERNODE_MANAGEMENT_STATE{CLUSTERNODE_MANAGEMENT_STATE_IDLE,CLUSTERNODE_MANAGEMENT_STATE_NODE_ACTIVE,
			CLUSTERNODE_MANAGEMENT_STATE_UNINITIALIZED};


	std::atomic_bool m_connection_active;//indicates an active mgmt connection

	const static std::string SHU_SHARED_MEM_NAME;

private:
	void unregisterClient();

	void tearDownConnection();

	void killSHU();

	TCPConnection* mp_connection;
	ClusterLibFrame* mp_frame;
	ExtendedNetworkNode* mp_node;
	enum ClusterNodeManagementClient::CLUSTERNODE_MANAGEMENT_STATE m_current_state;

	void parseCommand(Lazarus::Buffer* message);
	//pthread_mutex_t m_mutex;
	bool m_general_com_error_flag;

	std::string m_system_config_file;

	Lazarus::SharedMemoryPOSIX* mp_shared_memory;
	void* mp_shared_mem_raw;
	bool m_shu_active;

	Lazarus::SharedSemaphore* mp_shu_deployment_semaphore;

	//these vars are used in case of incoming large files
	FILE* mp_large_incoming_file;
	std::string m_large_file_name;
	long long int m_large_file_incoming_chunk_count;
	long long int m_large_file_chunks_received;

	//these vars are used in case of outgoing large files
	FILE* mp_large_outgoing_file;
	long long int m_large_file_outgoing_chunk_count;
	long long int m_large_file_chunks_transmitted;
	long long int m_large_file_last_outgoing_chunk_size;

};

}

#endif /* CLUSTERNODEMANAGEMENTCLIENT_H_ */
