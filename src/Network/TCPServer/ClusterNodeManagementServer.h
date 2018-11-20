/*
 * ClusterNodeManagementServer.h
 *
 *  Created on: 12.06.2013
 *      Author: darius
 */

#ifndef CLUSTERNODEMANAGEMENTSERVER_H_
#define CLUSTERNODEMANAGEMENTSERVER_H_

#include "ClusterLibServer.h"
#include "../CommunicationFacilities/ClusterNodeComFacility.h"
#include "../Nodes/ClusterNode.h"
#include "../../DataStructures/FastKTuple.h"
#include "../../DataStructures/FastVectorN.h"

#include <string>
#include <pthread.h>

namespace Lazarus
{

class ClusterNodeManagementServer: public ClusterLibServer {
public:
	ClusterNodeManagementServer(std::string local_address, unsigned int local_port, unsigned int max_connections,
			unsigned int worker_threads);
	virtual ~ClusterNodeManagementServer();

	enum CLUSTERNODE_MANAGEMENT_REQUEST_TYPE{CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_PING=0,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_PONG,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_GET_FILE,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_FILE_RECEIVED,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_FILE_RECV_ERROR,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_FILE_NOT_FOUND,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_GET_LARGE_FILE,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_GET_LARGE_FILE_OK,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_FIRST_INCOMING_GET_FILE_CHUNK,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_NEXT_INCOMING_GET_FILE_CHUNK,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_GET_LARGE_FILE_COMPLETE,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_GET_NEXT_LARGE_FILE_CHUNK,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_INCOMING_CLIENT_FILE,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_INCOMING_FILE,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_INCOMING_LARGE_FILE,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_INCOMING_LARGE_FILE_CHUNK,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_INCOMING_LARGE_FILE_CHUNK_OK,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_INCOMING_LARGE_FILE_FINAL_CHUNK_OK,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_INCOMING_LARGE_FILE_CHUNK_ERROR,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_SYSTEM_COMMAND,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_ANSWER_SYSTEM_COMMAND,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_UNREGISTER,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_UNREGISTER_CONFIRM,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_SYSTEM_COMMAND_ERROR,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_GET_SYSTEM_STATE,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_ANSWER_SYSTEM_STATE,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_GET_SHORT_SYSTEM_CPU_STATE,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_ANSWER_SHORT_SYSTEM_CPU_STATE,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_GET_DUMMY_DATA,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_ANSWER_DUMMY_DATA,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_INCOMING_SHU,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_INCOMING_SHU_SYNC,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_SHU_ERROR,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_SHU_OK,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_GET_SHU_PID,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_ANSWER_SHU_PID,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_ASYNC_SYSTEM_COMMAND,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_ASYNC_SYSTEM_COMMAND_OK,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_ASYNC_SYSTEM_COMMAND_ERROR,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_KILL_ACTIVE_SHU,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_SHU_TERMINATED,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_SHU_TERMINATE_ERROR,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_RAPID_SHU_DEPLOYMENT,
		CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_RAPID_SHU_DEPLOYMENT_SYNC};

	enum CLUSTERNODE_MANAGEMENT_STATE{CLUSTERNODE_MANAGEMENT_STATE_IDLE,CLUSTERNODE_MANAGEMENT_STATE_REGISTER,CLUSTERNODE_MANAGEMENT_STATE_NODE_ACTIVE,
		CLUSTERNODE_MANAGEMENT_STATE_UNINITIALIZED};

	static const long long LARGE_FILE_THRESHOLD = 10000000;//10MB threshold


	/**
	 * This method will not only create the worker threads but also assign the servers current 'disconnect-callback'
	 * to them.
	 * */
	virtual void createWorkerThreads();

	/**
	 * These methods are exposed by the server for external calls to the cluster system.
	 */


	/**
	 * Executes the given command on the client and returns its output. Returns NULL in case of errors.
	 */
	Lazarus::Buffer* sendSystemCommand(unsigned int node_id, std::string command);

	/**
	 * Requests 'bytes' bytes of random dummy data from the client.
	 */
	Lazarus::Buffer* requestDummyData(unsigned int node_id, unsigned int bytes);

	/**
	 * Returns a system status object or NULL in case of errors.
	 */
	Lazarus::SystemStatus* requestSystemReport(unsigned int node_id);

	/**
	 * Returns cpu loads in the form:
	 * <core 1 load| ... | core n load | total_load>.
	 * In case of errors NULL will be returned.
	 */
	Lazarus::FastKTuple<double>* requestShortCPUReport(unsigned int node_id);

	/**
	 * Any existing file will be overwritten. 0 on success, -1 otherwise.
	 */
	int sendFile(unsigned int node_id, std::string local_file, std::string destination_file);

	/**
	 * Will return the remote files content within a buffer or NULL in case of errors.
	 */
	Lazarus::Buffer* getFile(unsigned int node_id, std::string remote_file);

	/**
	 * Will write the remote file to the given file, any existing data will be deleted.
	 * 0 on success, -1 otherwise.
	 *
	 */
	int getLargeFile(unsigned int node_id, std::string remote_file, std::string local_file);

	/**
	 * A SHU will be buffered in client memory, thus do not distribute tremendously large SHUs.
	 * TODO: automatic chunking (like in 'sendFile(.)') for large SHUs.
	 */

	/**
	 * Will send the SHU which in turn will be deployed by the client. 0 on success, -1 otherwise.
	 * This method will commence a deployment of the SHU, which in turn will attempt to connect to the SHU server
	 * via 'shu_server_ip' and 'shu_server_port'.
	 * If it's foreseeable that the deployment will take some time, one should set 'use_semaphore_wait=true',
	 * by doing so the calling thread will wait on a semaphore instead on a spin-lock. Take into account:
	 * Semaphore wait: +does not waste cpu time esp. during long waits
	 *                 -needs a kernel call and exhibits a high latency for resuming work
	 * Spin-lock wait: +exhibits a very small latency for resuming work
	 *                 -wastes clock cycles as the thread does not sleep
	 */
	int sendSHUnit(unsigned int node_id, const std::string& file, const std::string& sh_id,
			const std::string& shu_server_ip, unsigned int shu_server_port, bool use_semaphore_wait = false);

	/**
	 * Will send a request to quickly deploy an already compiled shu from the database. 0 on success, -1 otherwise.
	 * This method will commence a deployment of the SHU, which in turn will attempt to connect to the SHU server
	 * via 'shu_server_ip' and 'shu_server_port'.
	 * If it's foreseeable that the deployment will take some time, one should set 'use_semaphore_wait=true',
	 * by doing so the calling thread will wait on a semaphore instead on a spin-lock. Take into account:
	 * Semaphore wait: +does not waste cpu time esp. during long waits
	 *                 -needs a kernel call and exhibits a high latency for resuming work
	 * Spin-lock wait: +exhibits a very small latency for resuming work
	 *                 -wastes clock cycles as the thread does not sleep
	 */
	int rapidSHUnit(unsigned int node_id, const std::string& sh_id,
			const std::string& shu_server_ip, unsigned int shu_server_port, bool use_semaphore_wait = false);

	/**
	 * Will send the SHU which in turn will be deployed by the client. 0 on success, -1 otherwise.
	 * This method will commence a deployment of the SHU, which in turn will attempt to connect to the SHU server
	 * via 'shu_server_ip' and 'shu_server_port'.
	 * This method will return once the 'SHU_INIT' macro within the SHU has finished! Beware that it might not very
	 * feasible to use this method for complex deployment procedures, e.g. long compilation times!
	 * If it's foreseeable that the deployment will take some time, one should set 'use_semaphore_wait=true',
	 * by doing so the calling thread will wait on a semaphore instead on a spin-lock. Take into account:
	 * Semaphore wait: +does not waste cpu time esp. during long waits
	 *                 -needs a kernel call and exhibits a high latency for resuming work
	 * Spin-lock wait: +exhibits a very small latency for resuming work
	 *                 -wastes clock cycles as the thread does not sleep
	 */
	int sendSHUnitSync(unsigned int node_id, const std::string& file, const std::string& sh_id,
			const std::string& shu_server_ip, unsigned int shu_server_port, bool use_semaphore_wait = false);

	/**
	 * Will send a request to quickly deploy an already compiled shu from the database. 0 on success, -1 otherwise.
	 * This method will commence a deployment of the SHU, which in turn will attempt to connect to the SHU server
	 * via 'shu_server_ip' and 'shu_server_port'.
	 * This method will return once the 'SHU_INIT' macro within the SHU has finished! Beware that, although a precompiled binary
	 * will be deployed, it might not very feasible to use this method, as e.g. a heavy system load might delay the
	 * rapid deployment.
	 * If it's foreseeable that the deployment will take some time, one should set 'use_semaphore_wait=true',
	 * by doing so the calling thread will wait on a semaphore instead on a spin-lock. Take into account:
	 * Semaphore wait: +does not waste cpu time esp. during long waits
	 *                 -needs a kernel call and exhibits a high latency for resuming work
	 * Spin-lock wait: +exhibits a very small latency for resuming work
	 *                 -wastes clock cycles as the thread does not sleep
	 */
	int rapidSHUnitSync(unsigned int node_id, const std::string& sh_id,
			const std::string& shu_server_ip, unsigned int shu_server_port, bool use_semaphore_wait = false);

	/**
	 * Executes the given command on the client without waiting for its completion. Returns -1 in case of errors,
	 * 0 otherwise.
	 */
	int sendAsyncSystemCommand(unsigned int node_id, std::string command);

	/**
	 * Returns the PID of the active shu on the specified node.Returns -1 in case of errors,
	 * the PID otherwise (a PID of 0 indicates that no SHU is currently registered on the client, which does not
	 * imply that no SHU at all exists, as it might just have not started up yet e.g. it still deploys(compiles)).
	 */
	long long getSHUPID(unsigned int node_id);

	/**
	 * This will kill the currently active shu on the given node. Returns -1 in case of errors,
	 * 0 otherwise.
	 */
	int killActiveSHU(unsigned int node_id);

	/**
	 * This method queries all connected nodes and returns a list of tuples which contain the cpu loads in the form:
	 * <core 1 load| ... | core n load | total_load>. If a node couldn't deliver its data, it simply won't be included
	 * in the output.
	 * !!!!! Use this method only for humongous amounts of nodes with complex evaluations of the cpu loads!!!!
	 * I.e. evaluations which take a serious amount of time. In this case its also smart to use semaphores!
	 */
	Lazarus::FastKTuple< Lazarus::FastVector2<unsigned int, Lazarus::FastKTuple<double>*> >* getNodeLoads(bool use_semaphore_wait = false);


	/**
	 * Helper methods which extract/inject headers and content
	 */
	static enum ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE getRequestType(Lazarus::Buffer* message);
	static void setRequestType(Lazarus::Buffer* message,enum ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE type);
	static void setContent(Lazarus::Buffer* message,unsigned char* content, unsigned int size);
	/**
	 * this method will allocate an adequate buffer at 'content' and set 'size' accordingly
	 */
	static void getContent(Lazarus::Buffer* message,unsigned char** content, unsigned int &size);

	unsigned int getNodeCount();
	Lazarus::FastKTuple<unsigned int>* getNodeIDs();

	/**
	 * This method returns a copy of a currently existing cluster node object. If no object with the given node ID exists,
	 * a new object will be returned with default values, thus one has to check if the object is valid by e.g. checking if
	 * the nodeID equals the specified one. A default object will contain the node ID 0.
	 * */
	ClusterNode getClusterNode(unsigned int nodeID);

};

}
#endif /* CLUSTERNODEMANAGEMENTSERVER_H_ */
