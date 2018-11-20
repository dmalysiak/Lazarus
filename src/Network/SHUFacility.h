/*
 * SHUFacility.h
 *
 *  Created on: 10.01.2015
 *      Author: Darius Malysiak
 */

#ifndef SHUFACILITY_H_
#define SHUFACILITY_H_

//system paths as this is a header file for all, and only, cluster apps !
#include <Network/SharedMemSHUComStruct.h>
#include <Network/TCPClient/ClusterNodeManagementClient.h>
#include <Core/IPC/SharedMemoryPOSIX.h>
#include <Core/IPC/SharedSemaphore.h>
#include <Core/Scheduling/Process.h>
#include <Core/Toolbox.h>
#include <Core/DataBase/SHUDB.h>

#include <unistd.h>
#include <string>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

#define SHU_INIT /**/ \
	/* this step is mandatory for each SHU  */ \
 \
	/*get the com struct through shared memory.*/ \
	Lazarus::SharedMemoryPOSIX shared_memory(0777); \
	int res____ = shared_memory.attachMemory(Lazarus::ClusterNodeManagementClient::SHU_SHARED_MEM_NAME,sizeof(struct Lazarus::SharedMemSHUComStruct)); \
	if(res____ == -1) \
	{ \
		printf("ERROR: could not attach shared memory for shu com struct\n"); \
	} \
	\
	struct Lazarus::SharedMemSHUComStruct* shu_com_struct = (struct Lazarus::SharedMemSHUComStruct*)shared_memory.getAttachedMemory(); \
\
	/*get the semaphore*/ \
	Lazarus::SharedSemaphore shu_sem(&shared_memory); \
	res____ = shu_sem.loadSemaphore((unsigned char*)&(shu_com_struct->m_deployment_sem)); \
	if(res____ == -1) \
	{ \
		printf("ERROR: could not load semaphore from shared memory for shu com struct\n"); \
	} \
\
	/*register within the com struct*/ \
	Lazarus::Process* self_process = Lazarus::Process::getSelf(); \
	shu_com_struct->m_shu_pid = self_process->m_pid; \
\
	/*get shu server information*/ \
	unsigned int node_id = shu_com_struct->m_node_id; \
	std::string shu_server_ip((const char*)shu_com_struct->mp_shu_server_ip); \
	unsigned int shu_server_port = shu_com_struct->m_shu_server_port; \
	int string_length = std::min((int)strlen(argv[0])+1,512);/*any path longer than 512 chars will be truncated*/ \
	memcpy(shu_com_struct->m_shu_binary_filename,argv[0],string_length); \
 \
	if(shu_com_struct->m_rapid_deployment == 0) \
	{ \
		/*save the deployed shu in the database for later rapid deployment*/ \
		Lazarus::SHUDB db( std::string((const char*)shu_com_struct->m_system_db_filename) ); \
 \
		/*delete any previous entry*/ \
		db.deleteEntry(std::string((const char*) shu_com_struct->m_shu_id )); \
 \
		/*read the compiled package*/ \
		Lazarus::Buffer compiled_package; \
		compiled_package.readBufferFromBinaryFile( std::string((const char*)shu_com_struct->m_system_deployment_path) \
				+ std::string("/") + std::string((const char*) shu_com_struct->m_shu_id ) +std::string("/rapid.tar.gz") ); \
 \
		/*add it to the db*/ \
		Lazarus::SHUDBEntry entry; \
 \
		entry.m_Name = std::string((const char*) shu_com_struct->m_shu_id ); \
		entry.m_BinarySize = 0; \
		entry.m_PackageSize = compiled_package.get_m_length(); \
		entry.m_PackageFilename = std::string((const char*) shu_com_struct->m_shu_id ) + std::string(".tar.gz"); \
		entry.m_DeploymentPath = std::string((const char*)shu_com_struct->m_system_deployment_path) ; \
		entry.m_RequiredLibs = std::string("-"); \
		entry.mp_SHUPackage = compiled_package.get_mp_data(); \
		entry.m_packageBlobSize = entry.m_PackageSize; \
		entry.mp_Binary = NULL; \
		entry.m_binaryBlobSize = 0; \
		entry.m_ConfigFileContent = std::string(""); \
		entry.m_ConfigFileName = std::string(""); \
		entry.m_VAR1 = std::string(""); \
		entry.mp_VAR2 = NULL; \
		entry.m_VAR2Size = 0; \
 \
		db.addEntry(&entry); \
 \
		/*abandon the data pointers as they are managed by other entities, otherwise the destruction of 'entry' would free them*/ \
		entry.abandonData(); \
	} \
	\
	/*indicate a finished deployment*/ \
	if(shu_com_struct->m_synchronized_deployment == 1) \
	{ \
		res____ = shu_sem.incSemaphore(); \
		if(res____ == -1) \
		{ \
			printf("ERROR: could not increment semaphore in shared memory for shu com struct\n"); \
		} \
	}

#define SHU_CLOSE \
		/* this step is mandatory for each SHU */ \
		/*--------------------- clean up the shu information */ \
		shu_com_struct->m_shu_pid = 0; \
		shu_com_struct->m_shu_server_port = 0; \
		memset(shu_com_struct->m_shu_binary_filename,0,512); \
		shu_com_struct->m_shu_binary_filename[0] = '\0'; \
		memset(shu_com_struct->mp_shu_server_ip,0,64); \
		shu_com_struct->mp_shu_server_ip[0] = '\0'; \
		memset(shu_com_struct->m_system_db_filename,0,512); \
		shu_com_struct->m_system_db_filename[0] = '\0'; \
		memset(shu_com_struct->m_system_deployment_path,0,512); \
		shu_com_struct->m_system_deployment_path[0] = '\0'; \
		shu_com_struct->m_rapid_deployment = 0; \
		shu_com_struct->m_synchronized_deployment = 0; \
		res____ = shared_memory.detachMemory(); \
		if(res____ == -1) \
		{ \
			printf("ERROR: could not detach shared memory for shu com struct\n"); \
		} \
		delete self_process; \


#define SHU_INIT_EXTERNAL(PID) /**/ \
	/* this step is mandatory for each SHU  */ \
 \
	/*get the com struct through shared memory.*/ \
	Lazarus::SharedMemoryPOSIX shared_memory(0777); \
	int res____ = shared_memory.attachMemory(Lazarus::ClusterNodeManagementClient::SHU_SHARED_MEM_NAME,sizeof(struct Lazarus::SharedMemSHUComStruct)); \
	if(res____ == -1) \
	{ \
		printf("ERROR: could not attach shared memory for shu com struct\n"); \
	} \
	\
	struct Lazarus::SharedMemSHUComStruct* shu_com_struct = (struct Lazarus::SharedMemSHUComStruct*)shared_memory.getAttachedMemory(); \
\
	/*get the semaphore*/ \
	Lazarus::SharedSemaphore shu_sem(&shared_memory); \
	res____ = shu_sem.loadSemaphore((unsigned char*)&(shu_com_struct->m_deployment_sem)); \
	if(res____ == -1) \
	{ \
		printf("ERROR: could not load semaphore from shared memory for shu com struct\n"); \
	} \
\
	/*register within the com struct*/ \
	shu_com_struct->m_shu_pid = PID; \
\
	/*get shu server information*/ \
	unsigned int node_id = shu_com_struct->m_node_id; \
	std::string shu_server_ip((const char*)shu_com_struct->mp_shu_server_ip); \
	unsigned int shu_server_port = shu_com_struct->m_shu_server_port; \
	int string_length = std::min((int)strlen(argv[0])+1,512);/*any path longer than 512 chars will be truncated*/ \
	memcpy(shu_com_struct->m_shu_binary_filename,argv[0],string_length); \
 \
	if(shu_com_struct->m_rapid_deployment == 0) \
	{ \
		/*save the deployed shu in the database for later rapid deployment*/ \
		Lazarus::SHUDB db( std::string((const char*)shu_com_struct->m_system_db_filename) ); \
 \
		/*delete any previous entry*/ \
		db.deleteEntry(std::string((const char*) shu_com_struct->m_shu_id )); \
 \
		/*read the compiled package*/ \
		Lazarus::Buffer compiled_package; \
		compiled_package.readBufferFromBinaryFile( std::string((const char*)shu_com_struct->m_system_deployment_path) \
				+ std::string("/") + std::string((const char*) shu_com_struct->m_shu_id ) +std::string("/rapid.tar.gz") ); \
 \
		/*add it to the db*/ \
		Lazarus::SHUDBEntry entry; \
 \
		entry.m_Name = std::string((const char*) shu_com_struct->m_shu_id ); \
		entry.m_BinarySize = 0; \
		entry.m_PackageSize = compiled_package.get_m_length(); \
		entry.m_PackageFilename = std::string((const char*) shu_com_struct->m_shu_id ) + std::string(".tar.gz"); \
		entry.m_DeploymentPath = std::string((const char*)shu_com_struct->m_system_deployment_path) ; \
		entry.m_RequiredLibs = std::string("-"); \
		entry.mp_SHUPackage = compiled_package.get_mp_data(); \
		entry.m_packageBlobSize = entry.m_PackageSize; \
		entry.mp_Binary = NULL; \
		entry.m_binaryBlobSize = 0; \
		entry.m_ConfigFileContent = std::string(""); \
		entry.m_ConfigFileName = std::string(""); \
		entry.m_VAR1 = std::string(""); \
		entry.mp_VAR2 = NULL; \
		entry.m_VAR2Size = 0; \
 \
		db.addEntry(&entry); \
 \
		/*abandon the data pointers as they are managed by other entities, otherwise the destruction of 'entry' would free them*/ \
		entry.abandonData(); \
	} \
	\
	/*indicate a finished deployment*/ \
	if(shu_com_struct->m_synchronized_deployment == 1) \
	{ \
		res____ = shu_sem.incSemaphore(); \
		if(res____ == -1) \
		{ \
			printf("ERROR: could not increment semaphore in shared memory for shu com struct\n"); \
		} \
	} \
\
	res____ = shared_memory.detachMemory(); \
	if(res____ == -1) \
	{ \
		printf("ERROR: could not detach shared memory for shu com struct\n"); \
	} \

#define SHU_CLOSE_EXTERNAL \
	/*get the com struct through shared memory.*/ \
	Lazarus::SharedMemoryPOSIX shared_memory(0777); \
	int res____ = shared_memory.attachMemory(Lazarus::ClusterNodeManagementClient::SHU_SHARED_MEM_NAME,sizeof(struct Lazarus::SharedMemSHUComStruct)); \
	if(res____ == -1) \
	{ \
		printf("ERROR: could not attach shared memory for shu com struct\n"); \
	} \
	\
	struct Lazarus::SharedMemSHUComStruct* shu_com_struct = (struct Lazarus::SharedMemSHUComStruct*)shared_memory.getAttachedMemory(); \
\
	/* this step is mandatory for each SHU */ \
	/*--------------------- clean up the shu information */ \
	shu_com_struct->m_shu_pid = 0; \
	shu_com_struct->m_shu_server_port = 0; \
	memset(shu_com_struct->m_shu_binary_filename,0,512); \
	shu_com_struct->m_shu_binary_filename[0] = '\0'; \
	memset(shu_com_struct->mp_shu_server_ip,0,64); \
	shu_com_struct->mp_shu_server_ip[0] = '\0'; \
	memset(shu_com_struct->m_system_db_filename,0,512); \
	shu_com_struct->m_system_db_filename[0] = '\0'; \
	memset(shu_com_struct->m_system_deployment_path,0,512); \
	shu_com_struct->m_system_deployment_path[0] = '\0'; \
	shu_com_struct->m_rapid_deployment = 0; \
	shu_com_struct->m_synchronized_deployment = 0; \
	res____ = shared_memory.detachMemory(); \
	if(res____ == -1) \
	{ \
		printf("ERROR: could not detach shared memory for shu com struct\n"); \
	} \


#define PRINT_SHU_SHM /**/ \
 \
	/*get the com struct through shared memory.*/ \
	Lazarus::SharedMemoryPOSIX shared_memory(0777); \
	int res____ = shared_memory.attachMemory(Lazarus::ClusterNodeManagementClient::SHU_SHARED_MEM_NAME,sizeof(struct Lazarus::SharedMemSHUComStruct)); \
	if(res____ == -1) \
	{ \
		printf("ERROR: could not attach shared memory for shu com struct\n"); \
	} \
	\
	struct Lazarus::SharedMemSHUComStruct* shu_com_struct = (struct Lazarus::SharedMemSHUComStruct*)shared_memory.getAttachedMemory(); \
\
	/*print all information within the struct*/ \
	printf("SHU ID (name): %s\n",shu_com_struct->m_shu_id); \
	printf("SHU binary: %s\n",shu_com_struct->m_shu_binary_filename); \
	printf("SHU database: %s\n",shu_com_struct->m_system_db_filename); \
	printf("SHU deployment path: %s\n",shu_com_struct->m_system_deployment_path); \
	printf("SHU PID: %llu\n",shu_com_struct->m_shu_pid); \
	printf("SHU rapid deployment: %d\n",shu_com_struct->m_rapid_deployment); \
	printf("SHU synchronized deployment: %d\n",shu_com_struct->m_synchronized_deployment); \
	printf("SHU node ID: %u\n",shu_com_struct->m_node_id); \
	printf("SHU server IP: %s\n",shu_com_struct->mp_shu_server_ip); \
	printf("SHU server port: %u\n",shu_com_struct->m_shu_server_port); \
\
\
	/*--------------------- clean up the shu information */ \
	res____ = shared_memory.detachMemory(); \
	if(res____ == -1) \
	{ \
		printf("ERROR: could not detach shared memory for shu com struct\n"); \
	} \


#define PRINT_SHU_NODE_ID /**/ \
 \
	/*get the com struct through shared memory.*/ \
	Lazarus::SharedMemoryPOSIX shared_memory(0777); \
	int res____ = shared_memory.attachMemory(Lazarus::ClusterNodeManagementClient::SHU_SHARED_MEM_NAME,sizeof(struct Lazarus::SharedMemSHUComStruct)); \
	if(res____ == -1) \
	{ \
		printf("ERROR: could not attach shared memory for shu com struct\n"); \
	} \
	\
	struct Lazarus::SharedMemSHUComStruct* shu_com_struct = (struct Lazarus::SharedMemSHUComStruct*)shared_memory.getAttachedMemory(); \
\
	/*print all information within the struct*/ \
	printf("%u\n",shu_com_struct->m_node_id); \
\
\
	/*--------------------- clean up the shu information */ \
	res____ = shared_memory.detachMemory(); \
	if(res____ == -1) \
	{ \
		printf("ERROR: could not detach shared memory for shu com struct\n"); \
	} \

#define PRINT_SHU_SERVER_IP /**/ \
 \
	/*get the com struct through shared memory.*/ \
	Lazarus::SharedMemoryPOSIX shared_memory(0777); \
	int res____ = shared_memory.attachMemory(Lazarus::ClusterNodeManagementClient::SHU_SHARED_MEM_NAME,sizeof(struct Lazarus::SharedMemSHUComStruct)); \
	if(res____ == -1) \
	{ \
		printf("ERROR: could not attach shared memory for shu com struct\n"); \
	} \
	\
	struct Lazarus::SharedMemSHUComStruct* shu_com_struct = (struct Lazarus::SharedMemSHUComStruct*)shared_memory.getAttachedMemory(); \
\
	/*print all information within the struct*/ \
	printf("%s\n",shu_com_struct->mp_shu_server_ip); \
\
\
	/*--------------------- clean up the shu information */ \
	res____ = shared_memory.detachMemory(); \
	if(res____ == -1) \
	{ \
		printf("ERROR: could not detach shared memory for shu com struct\n"); \
	} \

#define PRINT_SHU_SERVER_PORT /**/ \
 \
	/*get the com struct through shared memory.*/ \
	Lazarus::SharedMemoryPOSIX shared_memory(0777); \
	int res____ = shared_memory.attachMemory(Lazarus::ClusterNodeManagementClient::SHU_SHARED_MEM_NAME,sizeof(struct Lazarus::SharedMemSHUComStruct)); \
	if(res____ == -1) \
	{ \
		printf("ERROR: could not attach shared memory for shu com struct\n"); \
	} \
	\
	struct Lazarus::SharedMemSHUComStruct* shu_com_struct = (struct Lazarus::SharedMemSHUComStruct*)shared_memory.getAttachedMemory(); \
\
	/*print all information within the struct*/ \
	printf("%u\n",shu_com_struct->m_shu_server_port); \
\
\
	/*--------------------- clean up the shu information */ \
	res____ = shared_memory.detachMemory(); \
	if(res____ == -1) \
	{ \
		printf("ERROR: could not detach shared memory for shu com struct\n"); \
	} \


#endif /* SHUFACILITY_H_ */
