/*
 * ClusterNodeManagementClient.cpp
 *
 *  Created on: 22.03.2013
 *      Author: Darius
 */

#include "ClusterNodeManagementClient.h"
#include "../../Network/ConnectionManager.h"
#include "../../Toolbox.h"
#include "../../Scheduling/Process.h"
#include "../../Threading/MutexLocker.h"
#include "../SharedMemSHUComStruct.h"


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

namespace Lazarus
{

/**
 * A cluster client is always started before any SHU!
 */
ClusterNodeManagementClient::ClusterNodeManagementClient(std::string remote_address,
		unsigned int remote_port, std::string system_config_file, unsigned int max_attempts, unsigned int attempt_wait_time)
{
	m_shu_active = false;
	mp_large_incoming_file = NULL;
	mp_large_outgoing_file = NULL;
	m_large_file_incoming_chunk_count = 0;
	m_large_file_outgoing_chunk_count = 0;
	m_large_file_chunks_received = 0;
	m_large_file_chunks_transmitted = 0;
	m_large_file_last_outgoing_chunk_size = 0;

	int res = 0;

	/*just in case something went wrong, i.e. the client was shut down and killed the running shu,
	* attempt to release the previously allocated shared memory. This will work only if the shu hasn't
	* mapped the memory anymore!
	*/
	if(Lazarus::SharedMemoryPOSIX::checkExistence(ClusterNodeManagementClient::SHU_SHARED_MEM_NAME) == 0)
	{
		printf("cleaning up old shared memory \n");
		struct SharedMemSHUComStruct* shu_mem_object = (struct SharedMemSHUComStruct*)mp_shared_mem_raw;

		//get a temporary reference to the old SHM
		Lazarus::SharedMemoryPOSIX shm(0777);
		res = shm.attachMemory(ClusterNodeManagementClient::SHU_SHARED_MEM_NAME,sizeof(struct SharedMemSHUComStruct));
		if(res == -1)
		{
			printf("ERROR: could not attach old shared memory for shu com struct\n");
		}

		res = Lazarus::SharedSemaphore::destroySemInSHM(&shm,(unsigned char*)&(shu_mem_object->m_deployment_sem));
		if(res == -1)
		{
			printf("ERROR: could not delete old shared semaphore, maybe it didn't exist?\n");
		}

		res = shm.detachMemory();
		if(res == -1)
		{
			printf("ERROR: could not detach old shared memory for shu com struct\n");
		}

		/**
		 * We don't initially mark the shared memory as disposable due to the shared semaphore!
		 * If the SHM would be marked disposable, the crash of cluster client and shu would lead to
		 * a freeing of the memory, yet on the other hand it would also blast away any reference
		 * to the shared semaphore,
		 * whose allocated resources would be kept allocated in the system without any chance of
		 * freeing them.
		 */
		res = shm.markDisposable();
		if(res == -1)
		{
			printf("ERROR: could not unlink old shared memory for shu com struct\n");
		}
	}

	//init the shared memory
	mp_shared_memory = new Lazarus::SharedMemoryPOSIX(0777);

	res = mp_shared_memory->allocateMemory(ClusterNodeManagementClient::SHU_SHARED_MEM_NAME,sizeof(struct SharedMemSHUComStruct));
	if(res == -1)
	{
		printf("ERROR: could not allocate shared memory for shu com struct\n");
	}

	mp_shared_mem_raw = mp_shared_memory->getAttachedMemory();

	//create the semaphore
	struct SharedMemSHUComStruct* shu_mem_object = (struct SharedMemSHUComStruct*)mp_shared_mem_raw;
	mp_shu_deployment_semaphore = new Lazarus::SharedSemaphore(mp_shared_memory);
	res = mp_shu_deployment_semaphore->createSemaphore(0,(unsigned char*)&(shu_mem_object->m_deployment_sem));

	if(res == -1)
	{
		printf("ERROR: could not create semaphore in shared memory for shu com struct\n");
	}

	//struct SharedMemSHUComStruct* shu_mem_object = (struct SharedMemSHUComStruct*)mp_shared_mem_raw;
	//printf("SHU DEBUG %u\n",shu_mem_object->m_shu_server_port);

	m_system_config_file = system_config_file;
	unsigned int attempts = 1;
	mp_node = new ExtendedNetworkNode();
	m_current_state = ClusterNodeManagementClient::CLUSTERNODE_MANAGEMENT_STATE_UNINITIALIZED;
	mp_connection = ConnectionManager::connectToTCPIPv4(remote_address,remote_port);

	//if no connection could be made, keep trying a bit longer
	while(mp_connection == NULL && attempts <= max_attempts)
	{
		mp_connection = ConnectionManager::connectToTCPIPv4(remote_address,remote_port);
		Lazarus::Toolbox::sleep_ms(attempt_wait_time);
		printf("CONNECTION ATTEMPT %d\n",attempts);
		attempts++;
	}

	//if a connection has been established
	if(mp_connection != NULL)
	{
		mp_frame = new ClusterLibFrame(mp_connection);
		mp_connection->printConnectionInformation();
		m_connection_active = true;
	}
	else
	{
		m_connection_active = false;
	}

	m_general_com_error_flag = false;

	//get all system information
	mp_node->initializeForCurrentSystem(m_system_config_file);
	mp_node->printNodeInfo();

	/*mp_node->serialize();
	mp_node->deserialize();
	mp_node->printNodeInfo();*/

}

ClusterNodeManagementClient::~ClusterNodeManagementClient()
{
	//graceful shutdown
	shutdown();

	//close the connection
	tearDownConnection();

	//kill any active SHU
	killSHU();

	//----- release the shared memory
	//delete the shared semaphore
	mp_shu_deployment_semaphore->destroySemaphore();
	delete mp_shu_deployment_semaphore;
	//detach the memory
	mp_shared_memory->detachMemory();
	//mark it for deletion
	mp_shared_memory->markDisposable();
	delete mp_shared_memory;

	//and delete the node
	DELETE_NULL_CHECKING(mp_node);
}

void ClusterNodeManagementClient::killSHU()
{
	//kill the existing shu
	struct SharedMemSHUComStruct* shu_mem_object = (struct SharedMemSHUComStruct*)mp_shared_mem_raw;
	if(shu_mem_object->m_shu_pid != 0)
	{
		printf("found shu %s with pid %llu, going to stop it\n",shu_mem_object->m_shu_id,shu_mem_object->m_shu_pid);

		//check if directory with the unit name exists under e.g. /tmp
		struct stat st = {0};
		std::string path = mp_node->m_shu_deployment_path + std::string("/")+std::string((const char*)shu_mem_object->m_shu_id);

		//if yes, delete it
		if(stat(path.c_str(), &st) != -1)
		{
			std::string command = std::string("rm -rf ")+path;
			system(command.c_str());
		}

		Lazarus::Process* shu_process = new Lazarus::Process(shu_mem_object->m_shu_pid);
		int res = shu_process->killProcess();
		if(res == -1)
		{
			printf("ERROR: could not kill the shu process (PID %llu)\n",shu_mem_object->m_shu_pid);
		}
		delete shu_process;

		shu_mem_object->m_shu_pid = 0;
	}
}

void ClusterNodeManagementClient::unregisterClient()
{
	//unregister the client
	//if no connection has been established: abort
	if(mp_connection != NULL)
	{
		//if the socket was closed: don't even try to send something
		if(mp_connection->get_m_socket_active() != false)
		{
			return;
		}

		int result = 0;
		unsigned int type_size = sizeof(enum ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE);
		Lazarus::Buffer* buffer = new Lazarus::Buffer(type_size);
		enum ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE type;

		//send the unregister request
		printf("sending unregister request\n");
		ClusterNodeManagementServer::setRequestType(buffer,ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_UNREGISTER);
		result = mp_frame->sendFrame(buffer);
		delete buffer;

		if(result==-1)
		{
			printf("ERROR: could not unregister request\n");
			return;
		}

		//wait for a response
		printf("waiting for unregister confirmation\n");
		buffer = mp_frame->readFrame();
		type = ClusterNodeManagementServer::getRequestType(buffer);
		delete buffer;

		//check the type
		if( type != ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_UNREGISTER_CONFIRM)
		{
			printf("ERROR: received not a confirmation\n");
		}

		printf("received confirmation\n");
	}
}

void ClusterNodeManagementClient::tearDownConnection()
{
	//close the connection
	DELETE_NULL_CHECKING(mp_connection);

	//delete the frame
	DELETE_NULL_CHECKING(mp_frame);

	m_connection_active = false;
}

void ClusterNodeManagementClient::run()
{
	//if no connection has been established: abort
	if(mp_connection == NULL)
	{
		printf("ERROR: no active connection\n");
		return;
	}

	int result = 0;
	unsigned int type_size = sizeof(enum ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE);
	enum ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE type = ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_PING;

	//---------------------- wait for servers ping
	Lazarus::Buffer* buffer = NULL;
	/*buffer = mp_frame->readFrame();
	memcpy(&type,buffer->get_mp_data(),type_size);

	//check for ping type
	if( type != ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_PING)
	{
		printf("ERROR: received not a ping\n");
		delete buffer;
		return;
	}
	else
	{
		printf("received the servers initial greeting ping\n");
	}
	delete buffer;*/

	//---------------------- send a greeting ping
	printf("sending ping\n");
	buffer = new Lazarus::Buffer(type_size + sizeof(unsigned int));
	type = ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_PING;
	memcpy(buffer->get_mp_data(),&type,type_size);//type
	memcpy(buffer->get_mp_data()+type_size,&(mp_node->m_desired_node_id),sizeof(unsigned int));//desired id
	result = mp_frame->sendFrame(buffer);
	delete buffer;

	if(result==-1)
	{
		printf("ERROR: could not send ping greeting\n");
		return;
	}

	//wait for a response
	printf("waiting for pong\n");
	buffer = mp_frame->readFrame();
	memcpy(&type,buffer->get_mp_data(),type_size);

	//check for pong
	if( type != ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_PONG)
	{
		printf("ERROR: received not a pong\n");
		delete buffer;
		return;
	}

	printf("received pong\n");
	buffer->printfBuffer();

	//if a pong has been received, extract the attached node id
	unsigned int node_id = 0;
	memcpy(&node_id,buffer->get_mp_data()+type_size,sizeof(unsigned int));
	delete buffer;
	printf("Server dictates a node ID of %u\n",node_id);
	mp_node->m_node_id = node_id;
	mp_node->m_node_ip = mp_connection->get_mp_socket()->m_local_address;

	//send the node object to the server
	mp_node->serialize();
	buffer = new Lazarus::Buffer( mp_node->get_mp_serialized_data_buffer()->get_m_length() );
	memcpy(buffer->get_mp_data(),mp_node->get_mp_serialized_data_buffer()->get_mp_data(), mp_node->get_mp_serialized_data_buffer()->get_m_length()* sizeof(unsigned char));

	result = mp_frame->sendFrame(buffer);
	delete buffer;

	if(result==-1)
	{
		printf("ERROR: could not send node object\n");
		return;
	}
	else
	{
		printf("sent node object \n");
	}

	//enter nodes state machine once the node has been registered
	m_current_state = ClusterNodeManagementClient::CLUSTERNODE_MANAGEMENT_STATE_NODE_ACTIVE;

	//register the node id within the shu com struct
	struct SharedMemSHUComStruct* shu_mem_object = (struct SharedMemSHUComStruct*)mp_shared_mem_raw;
	shu_mem_object->m_node_id = node_id;

	while(m_shutdown_flag != true)
	{
		switch(m_current_state)
		{
			case ClusterNodeManagementClient::CLUSTERNODE_MANAGEMENT_STATE_NODE_ACTIVE:
			{
				//wait for an arbitrary command (using a timeout enables one to stop this thread via the shutdown method)
				printf("client waiting\n");
				Lazarus::Buffer* buffer1 = mp_frame->readFrame(10500,500);

				if(buffer1 != NULL)
				{
					//as we can't detect a closed socket from just the output of the readFrame method
					if(mp_connection->get_m_socket_active() == false)
					{
						printf("TCP connection closed\n");
						tearDownConnection();
						killSHU();
						m_shutdown_flag = true;
						m_thread_self_terminated = true;
						delete buffer1;
						break;
					}

					//check for any fatal error of the echo server
					if(m_general_com_error_flag == true)
					{
						printf("TCP connection closed\n");
						tearDownConnection();
						killSHU();
						m_shutdown_flag = true;
						m_thread_self_terminated = true;
						delete buffer1;
						break;//leave the current case
					}

					//if no error occurred parse the received package
					parseCommand(buffer1);
				}
				else
				{
					//as we can't detect a closed socket from just the output of the readFrame method
					if(mp_connection->get_m_socket_active() == false)
					{
						printf("TCP connection closed\n");
						tearDownConnection();
						killSHU();
						m_shutdown_flag = true;
						m_thread_self_terminated = true;
						break;
					}

					//check for any fatal error of the echo server
					if(m_general_com_error_flag == true)
					{
						printf("TCP connection closed\n");
						tearDownConnection();
						killSHU();
						m_shutdown_flag = true;
						m_thread_self_terminated = true;
						break;//leave the current case
					}

				}

				delete buffer1;

				break;
			}

			default:
				printf("ERROR: unknown state\n");
				break;
		}

	}

	printf("CNMC is exiting\n");

}



void ClusterNodeManagementClient::parseCommand(Lazarus::Buffer* message)
{
	enum ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE type = ClusterNodeManagementServer::getRequestType(message);
	unsigned int type_size = sizeof(enum ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE);
	switch(type)
	{
		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_SYSTEM_COMMAND:
		{
			unsigned int command_length = message->get_m_length()-type_size;

			//extract the command
			char* command_buffer = new char[ command_length ];
			memcpy(command_buffer,message->get_mp_data()+type_size, command_length);
			//printf("system command received\n");
			//execute the command
			FILE *fp;
			char shell_output[16192];//16kiB buffer for command output

			/* Open the command for reading. */
			fp = popen(command_buffer, "r");
			delete command_buffer;

			if(fp == NULL)
			{
				printf("Failed to run command\n" );

				//close the pipe
				pclose(fp);

				//send error response
				Lazarus::Buffer buffer( type_size );
				ClusterNodeManagementServer::setRequestType(&buffer,
						ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_SYSTEM_COMMAND_ERROR);

				mp_frame->sendFrame(&buffer);

				break;
			}

			int bytes_read = fread(shell_output, 1, sizeof(shell_output), fp);

			/* Indicate if the buffer is not big enough. */
			if(bytes_read == sizeof(shell_output))
			{
				printf("Command output is too large for buffer: truncating\n" );
			}

			/* NULL-terminate the text. */
			shell_output[bytes_read] = '\0';

			//close the pipe
			pclose(fp);
			//printf("shell result: %s\n",shell_output);
			//send the result back
			Lazarus::Buffer response(bytes_read+type_size);
			ClusterNodeManagementServer::setRequestType(&response,ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_ANSWER_SYSTEM_COMMAND);
			memcpy(response.get_mp_data()+type_size,shell_output,bytes_read);
			mp_frame->sendFrame(&response);

			break;
		}

		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_GET_DUMMY_DATA:
		{
			//get the byte count
			unsigned int bytes = 0;
			memcpy(&bytes, message->get_mp_data()+type_size,sizeof(unsigned int));

			//send the result back
			Lazarus::Buffer response(bytes+type_size);
			//printf("request for dummy data received\n");
			//fill the buffer with dummy data
			unsigned char* buffer_data = response.get_mp_data();
			for(unsigned int i=type_size;i<bytes+type_size;i++)
			{
				buffer_data[i] = rand() % 255;
			}

			ClusterNodeManagementServer::setRequestType(&response,ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_ANSWER_DUMMY_DATA);

			mp_frame->sendFrame(&response);

			break;
		}

		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_GET_SYSTEM_STATE:
		{
			Lazarus::SystemStatus sysState;
			sysState.getSystemStatus();
			sysState.serialize();

			//send the status object
			Lazarus::Buffer response(sysState.get_mp_serialized_data_buffer()->get_m_length()+type_size);

			//printf("size of s data %u\n",sysState.get_mp_serialized_data_buffer()->get_m_length());
			//sysState.get_mp_serialized_data_buffer()->printfRawBuffer();
			//send a corresponding request to the server
			//Lazarus::Buffer response(file_name_length+type_size);
			//printf("request for file_aquisition received\n");

			ClusterNodeManagementServer::setRequestType(&response,ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_ANSWER_SYSTEM_STATE);
			memcpy(response.get_mp_data()+type_size,
					sysState.get_mp_serialized_data_buffer()->get_mp_data(),
					sysState.get_mp_serialized_data_buffer()->get_m_length()*sizeof(unsigned char));

			//response.printfRawBuffer();

			mp_frame->sendFrame(&response);

			break;
		}

		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_GET_SHORT_SYSTEM_CPU_STATE:
		{
			Lazarus::SystemStatus sysState;
			sysState.getSystemStatus();

			//prepare the message
			Lazarus::Buffer response(type_size + sizeof(double)*(mp_node->m_cores_per_cpu*mp_node->m_physical_cpu_count+1) );

			double* data = (double*)(response.get_mp_data() + type_size);
			for(unsigned int i=0;i<mp_node->m_cores_per_cpu*mp_node->m_physical_cpu_count;++i)
			{
				data[i] = sysState.get_m_cpu_load(i);
			}
			data[mp_node->m_cores_per_cpu*mp_node->m_physical_cpu_count] = sysState.get_m_total_cpu_load();

			ClusterNodeManagementServer::setRequestType(&response,ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_ANSWER_SHORT_SYSTEM_CPU_STATE);

			mp_frame->sendFrame(&response);

			break;
		}

		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_GET_SHU_PID:
		{
			struct SharedMemSHUComStruct* shu_mem_object = (struct SharedMemSHUComStruct*)mp_shared_mem_raw;

			//send the status object
			Lazarus::Buffer response(type_size + sizeof(unsigned long long));

			ClusterNodeManagementServer::setRequestType(&response,ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_ANSWER_SHU_PID);
			//straight copy from shared memory
			memcpy(response.get_mp_data()+type_size, &(shu_mem_object->m_shu_pid), sizeof(unsigned long long));
			//printf("sending pid %llu \n", *((unsigned long long*)(response.get_mp_data()+type_size)) );
			mp_frame->sendFrame(&response);

			break;
		}

		//client returns the file
		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_GET_FILE:
		{
			//get the filename
			unsigned int source_file_name_length = message->get_m_length()-type_size;
			char* source_file_string = new char[source_file_name_length];
			memcpy(source_file_string, message->get_mp_data()+type_size, source_file_name_length);

			long long int data_size = Lazarus::Toolbox::getFileSize(std::string(source_file_string));

			//if file not found return an error
			if(data_size == -1)
			{
				delete source_file_string;

				Lazarus::Buffer buffer( type_size );
				ClusterNodeManagementServer::setRequestType(&buffer,
						ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_FILE_NOT_FOUND);

				mp_frame->sendFrame(&buffer);

				break;
			}


			Lazarus::Buffer buffer( type_size + data_size );

			//construct data
			//insert type
			ClusterNodeManagementServer::setRequestType(&buffer,
					ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_INCOMING_CLIENT_FILE);
			//now the file data
			Lazarus::Toolbox::readFileToArray(std::string(source_file_string), buffer.get_mp_data()+type_size);

			delete source_file_string;

			mp_frame->sendFrame(&buffer);

			break;
		}

		//client returns the file
		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_GET_LARGE_FILE:
		{
			//reset from previous attempts
			m_large_file_chunks_transmitted = 0;
			m_large_file_outgoing_chunk_count = 0;
			m_large_file_last_outgoing_chunk_size = 0;

			//get the filename
			unsigned int source_file_name_length = message->get_m_length()-type_size;
			char* source_file_string = new char[source_file_name_length];
			memcpy(source_file_string, message->get_mp_data()+type_size, source_file_name_length);

			long long int data_size = Lazarus::Toolbox::getFileSize(std::string(source_file_string));

			//if file not found return an error
			if(data_size == -1)
			{
				delete source_file_string;

				Lazarus::Buffer buffer( type_size );
				ClusterNodeManagementServer::setRequestType(&buffer,
						ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_FILE_NOT_FOUND);

				mp_frame->sendFrame(&buffer);

				break;
			}

			//determine the amount of chunks
			//calculate the number of expected chunks
			m_large_file_outgoing_chunk_count = data_size / ClusterNodeManagementServer::LARGE_FILE_THRESHOLD;

			if(data_size % ClusterNodeManagementServer::LARGE_FILE_THRESHOLD != 0 )
			{
				m_large_file_outgoing_chunk_count+=1;
			}

			m_large_file_last_outgoing_chunk_size = data_size % ClusterNodeManagementServer::LARGE_FILE_THRESHOLD;

			//abort if file couldn't be opened
			mp_large_outgoing_file = fopen64(source_file_string,"rb");
			if(mp_large_outgoing_file == NULL)
			{
				delete source_file_string;

				Lazarus::Buffer buffer( type_size );
				ClusterNodeManagementServer::setRequestType(&buffer,
						ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_FILE_NOT_FOUND);

				mp_frame->sendFrame(&buffer);
				break;
			}
			delete source_file_string;

			//distinguish the cases of one or multiple chunks
			if(m_large_file_outgoing_chunk_count > 1)
			{
				//in case of multiple chunks, the first will always have maximal size
				Lazarus::Buffer buffer( type_size + ClusterNodeManagementServer::LARGE_FILE_THRESHOLD + sizeof(long long int) );

				//construct data
				//insert type
				ClusterNodeManagementServer::setRequestType(&buffer,
						ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_FIRST_INCOMING_GET_FILE_CHUNK);
				//insert actual file size
				memcpy(buffer.get_mp_data()+type_size, &data_size, sizeof(long long int));
				//now the file data
				Lazarus::Toolbox::readFileToArray(mp_large_outgoing_file, buffer.get_mp_data()+type_size+sizeof(long long int),
						ClusterNodeManagementServer::LARGE_FILE_THRESHOLD,0);

				mp_frame->sendFrame(&buffer);

				m_large_file_chunks_transmitted+=1;
			}
			else //in case of one chunk we are 'nearly' done
			{
				//since this is the one and only chunk
				Lazarus::Buffer buffer( type_size + m_large_file_last_outgoing_chunk_size + sizeof(long long int) );

				//construct data
				//insert type
				ClusterNodeManagementServer::setRequestType(&buffer,
						ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_FIRST_INCOMING_GET_FILE_CHUNK);
				//insert actual file size
				memcpy(buffer.get_mp_data()+type_size, &data_size, sizeof(long long int));
				//now the file data
				Lazarus::Toolbox::readFileToArray(mp_large_outgoing_file, buffer.get_mp_data()+type_size+sizeof(long long int),
						m_large_file_last_outgoing_chunk_size,0);

				mp_frame->sendFrame(&buffer);

				m_large_file_chunks_transmitted+=1;
			}

			break;
		}

		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_GET_NEXT_LARGE_FILE_CHUNK:
		{

			//is this the last file chunk?
			if(m_large_file_chunks_transmitted + 1 == m_large_file_outgoing_chunk_count)
			{
				Lazarus::Buffer buffer( type_size + m_large_file_last_outgoing_chunk_size );

				//construct data
				//insert type
				ClusterNodeManagementServer::setRequestType(&buffer,
						ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_NEXT_INCOMING_GET_FILE_CHUNK);
				//now the file data
				Lazarus::Toolbox::readFileToArray(mp_large_outgoing_file,
						buffer.get_mp_data()+type_size,
						m_large_file_last_outgoing_chunk_size,
						m_large_file_chunks_transmitted * ClusterNodeManagementServer::LARGE_FILE_THRESHOLD);

				mp_frame->sendFrame(&buffer);

				m_large_file_chunks_transmitted+=1;
			}
			else //if not
			{
				Lazarus::Buffer buffer( type_size + ClusterNodeManagementServer::LARGE_FILE_THRESHOLD );

				//construct data
				//insert type
				ClusterNodeManagementServer::setRequestType(&buffer,
						ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_NEXT_INCOMING_GET_FILE_CHUNK);
				//now the file data
				Lazarus::Toolbox::readFileToArray(mp_large_outgoing_file,
						buffer.get_mp_data()+type_size,
						ClusterNodeManagementServer::LARGE_FILE_THRESHOLD,
						m_large_file_chunks_transmitted * ClusterNodeManagementServer::LARGE_FILE_THRESHOLD);

				mp_frame->sendFrame(&buffer);

				m_large_file_chunks_transmitted+=1;
			}

			break;
		}

		//finalize the getting of a large file
		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_GET_LARGE_FILE_COMPLETE:
		{
			fclose(mp_large_outgoing_file);

			Lazarus::Buffer buffer( type_size );

			//construct data
			//insert type
			ClusterNodeManagementServer::setRequestType(&buffer,
					ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_GET_LARGE_FILE_OK);

			mp_frame->sendFrame(&buffer);

			break;
		}

		//client saves the file
		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_INCOMING_FILE:
		{
			char* source_file_string = NULL;
			unsigned int source_file_name_length = 0;
			char* target_file_string = NULL;
			unsigned int target_file_name_length = 0;

			//deserialize the data
			//get source filename length
			memcpy(&source_file_name_length, message->get_mp_data()+type_size, sizeof(unsigned int));
			source_file_string = new char[source_file_name_length];
			//get source filename
			memcpy(source_file_string,message->get_mp_data()+type_size+sizeof(unsigned int), source_file_name_length);
			//get destination filename length
			memcpy(&target_file_name_length, message->get_mp_data()+type_size+sizeof(unsigned int)+source_file_name_length,
					sizeof(unsigned int));
			target_file_string = new char[target_file_name_length];
			//get the destination filename
			memcpy(target_file_string, message->get_mp_data()+type_size+2*sizeof(unsigned int)+source_file_name_length,
					target_file_name_length);

			//printf("File %s received, will be saved as %s\n",source_file_string,target_file_string);

			//now write the file data
			int res = Lazarus::Toolbox::writeArrayToFile(std::string(target_file_string),
					message->get_mp_data()+type_size+2*sizeof(unsigned int)+source_file_name_length+target_file_name_length,
					message->get_m_length()-(type_size+2*sizeof(unsigned int)+source_file_name_length+target_file_name_length) );

			//delete the filenames
			delete[] source_file_string;
			delete[] target_file_string;

			//send a corresponding answer to the server
			Lazarus::Buffer response(type_size);

			if(res == 0)
			{
				ClusterNodeManagementServer::setRequestType(&response,
					ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_FILE_RECEIVED);
			}
			else
			{
				ClusterNodeManagementServer::setRequestType(&response,
					ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_FILE_RECV_ERROR);
			}

			mp_frame->sendFrame(&response);


			break;
		}

		//client saves the large file in increments
		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_INCOMING_LARGE_FILE:
		{
			char* source_file_string = NULL;
			unsigned int source_file_name_length = 0;
			char* target_file_string = NULL;
			unsigned int target_file_name_length = 0;
			long long large_file_size = 0;

			//deserialize the data
			//get source filename length
			memcpy(&source_file_name_length, message->get_mp_data()+type_size, sizeof(unsigned int));
			source_file_string = new char[source_file_name_length];
			//get source filename
			memcpy(source_file_string,message->get_mp_data()+type_size+sizeof(unsigned int), source_file_name_length);
			//get destination filename length
			memcpy(&target_file_name_length, message->get_mp_data()+type_size+sizeof(unsigned int)+source_file_name_length,
					sizeof(unsigned int));
			target_file_string = new char[target_file_name_length];
			//get the destination filename
			memcpy(target_file_string, message->get_mp_data()+type_size+2*sizeof(unsigned int)+source_file_name_length,
					target_file_name_length);
			//get the expected file size
			memcpy(&large_file_size, message->get_mp_data()+type_size+2*sizeof(unsigned int)+source_file_name_length+target_file_name_length,
					sizeof(long long int));

			//calculate the number of expected chunks
			m_large_file_incoming_chunk_count = large_file_size / ClusterNodeManagementServer::LARGE_FILE_THRESHOLD;

			if(large_file_size % ClusterNodeManagementServer::LARGE_FILE_THRESHOLD != 0)
			{
				m_large_file_incoming_chunk_count+=1;
			}

			//printf("File %s received, will be saved as %s\n",source_file_string,target_file_string);

			//delete any previous file
			unlink(target_file_string);

			//open the target file (for appending)
			mp_large_incoming_file = fopen64(target_file_string,"ab");

			//now write the first chunk to the file (it will be of size 'LARGE_FILE_THRESHOLD')
			int res = Lazarus::Toolbox::writeArrayToFile(mp_large_incoming_file,
					message->get_mp_data()+type_size+2*sizeof(unsigned int)+source_file_name_length+target_file_name_length+sizeof(long long int),
					ClusterNodeManagementServer::LARGE_FILE_THRESHOLD );

			//send a corresponding answer to the server
			Lazarus::Buffer response(type_size);

			if(res == 0)
			{
				//in case of large files there will be always more than one chunks!
				ClusterNodeManagementServer::setRequestType(&response,
					ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_INCOMING_LARGE_FILE_CHUNK_OK);

				m_large_file_chunks_received+=1;
				m_large_file_name = std::string(target_file_string);
			}
			else //abort the process
			{
				ClusterNodeManagementServer::setRequestType(&response,
					ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_INCOMING_LARGE_FILE_CHUNK_ERROR);

				m_large_file_incoming_chunk_count = 0;
				m_large_file_chunks_received = 0;
				m_large_file_name = std::string("");
				fclose(mp_large_incoming_file);

				//delete the partial file
				unlink(target_file_string);
			}

			mp_frame->sendFrame(&response);

			//delete the filenames
			delete[] source_file_string;
			delete[] target_file_string;

			break;
		}

		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_INCOMING_LARGE_FILE_CHUNK:
		{

			//printf("File %s received, will be saved as %s\n",source_file_string,target_file_string);

			//now write the first chunk to the file
			int res = Lazarus::Toolbox::writeArrayToFile(mp_large_incoming_file,
					message->get_mp_data()+type_size,
					message->get_m_length()-type_size );


			//send a corresponding answer to the server
			Lazarus::Buffer response(type_size);

			if(res == 0)
			{
				m_large_file_chunks_received+=1;

				//are we done?
				if(m_large_file_chunks_received == m_large_file_incoming_chunk_count)
				{
					fclose(mp_large_incoming_file);
					m_large_file_incoming_chunk_count = 0;
					m_large_file_chunks_received = 0;

					ClusterNodeManagementServer::setRequestType(&response,
						ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_INCOMING_LARGE_FILE_FINAL_CHUNK_OK);
				}
				else
				{
					ClusterNodeManagementServer::setRequestType(&response,
						ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_INCOMING_LARGE_FILE_CHUNK_OK);
				}
			}
			else //abort the process
			{
				ClusterNodeManagementServer::setRequestType(&response,
					ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_INCOMING_LARGE_FILE_CHUNK_ERROR);

				m_large_file_incoming_chunk_count = 0;
				m_large_file_chunks_received = 0;
				m_large_file_name = std::string("");
				fclose(mp_large_incoming_file);

				//delete the partial file
				unlink(m_large_file_name.c_str());
			}

			mp_frame->sendFrame(&response);


			break;
		}

		//client prepares the sh unit
		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_INCOMING_SHU:
		{
			if(m_shu_active == true)
			{
				printf("ERROR: shu already active\n");

				//send a corresponding answer to the server
				Lazarus::Buffer response(type_size);
				ClusterNodeManagementServer::setRequestType(&response,
					ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_SHU_ERROR);
				mp_frame->sendFrame(&response);

				break;
			}

			//extract the shu information
			//shu name length
			unsigned int shu_name_length = 0;
			memcpy(&shu_name_length, message->get_mp_data()+type_size, sizeof(unsigned int));
			//shu name
			char* shu_name = new char[shu_name_length];
			memcpy(shu_name,message->get_mp_data()+type_size+sizeof(unsigned int),shu_name_length);

			//shu server ip length
			unsigned int shu_server_ip_length = 0;
			memcpy(&shu_server_ip_length,
					message->get_mp_data()+type_size+shu_name_length+sizeof(unsigned int),
					sizeof(unsigned int));
			//shu server ip
			char* shu_server_ip = new char[shu_server_ip_length];//printf("server ip length %u \n",shu_server_ip_length);
			memcpy(shu_server_ip,
					message->get_mp_data()+type_size+2*sizeof(unsigned int)+shu_name_length,
					shu_server_ip_length);

			//shu server port
			unsigned int shu_serverport = 0;
			memcpy(&shu_serverport,
					message->get_mp_data()+type_size+2*sizeof(unsigned int)+shu_name_length+shu_server_ip_length,
					sizeof(unsigned int));

			//and now get the data
			Lazarus::Buffer shu_data(message->get_m_length()-(type_size+3*sizeof(unsigned int)+shu_name_length+shu_server_ip_length));
			memcpy(shu_data.get_mp_data(),
				message->get_mp_data()+type_size+3*sizeof(unsigned int)+shu_name_length+shu_server_ip_length,
				shu_data.get_m_length()*sizeof(unsigned char));

			//check if directory with the unit name already exists under e.g. /tmp
			struct stat st = {0};
			std::string path = mp_node->m_shu_deployment_path + std::string("/")+std::string(shu_name);

			//if not create it
			if(stat(path.c_str(), &st) == -1)
			{
			    mkdir(path.c_str(), 0777);
			}
			//if yes, delete it and recreate it
			else
			{
				std::string command = std::string("rm -rf ")+path;
				system(command.c_str());

				mkdir(path.c_str(), 0777);
			}

			//write the file
			std::string file = path + std::string("/");
			file = file + std::string(shu_name) + std::string(".tar.gz");

			printf("going to write the file %s\n",file.c_str());
			//save the file
			Lazarus::Toolbox::writeArrayToFile(file, shu_data.get_mp_data(), shu_data.get_m_length());

			//register the shu and the shu server within the shared memory object
			struct SharedMemSHUComStruct* shu_mem_object = (struct SharedMemSHUComStruct*)mp_shared_mem_raw;
			shu_mem_object->m_shu_server_port = shu_serverport;//shu server port
			shu_mem_object->m_shu_pid = 0;
			shu_mem_object->m_rapid_deployment = 0;
			shu_mem_object->m_synchronized_deployment = 0;
			int l = std::min(512,(int)(mp_node->m_system_db_file.size()+1));
			memcpy(shu_mem_object->m_system_db_filename,mp_node->m_system_db_file.c_str(),l);//sh db
			l = std::min(512,(int)(mp_node->m_shu_deployment_path.size()+1));
			memcpy(shu_mem_object->m_system_deployment_path,mp_node->m_shu_deployment_path.c_str(),l);//deployment path
			memcpy(shu_mem_object->m_shu_id,shu_name,shu_name_length);//shu id
			memcpy(shu_mem_object->mp_shu_server_ip,shu_server_ip,shu_server_ip_length);//shu server ip

			//printf("SHU data %s %u, com data %s %u\n",shu_server_ip,shu_serverport,shu_mem_object->mp_shu_server_ip,shu_mem_object->m_shu_server_port);

			delete[] shu_server_ip;
			delete[] shu_name;


			//extract the unit
			std::string command = std::string("tar -C")+ path + std::string(" -xzvf ")+ file ;
			system(command.c_str());

			//delete the archive
			command = std::string("rm ") + file ;
			system(command.c_str());

			//execute the deployment script in the background
			command = std::string("sh ") + path + std::string("/deployment.sh &");
			system(command.c_str());

			m_shu_active = true;

			//send a corresponding answer to the server
			Lazarus::Buffer response(type_size);
			ClusterNodeManagementServer::setRequestType(&response,
				ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_SHU_OK);
			mp_frame->sendFrame(&response);

			break;
		}

		//client prepares the sh unit
		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_INCOMING_SHU_SYNC:
		{
			if(m_shu_active == true)
			{
				printf("ERROR: shu already active\n");

				//send a corresponding answer to the server
				Lazarus::Buffer response(type_size);
				ClusterNodeManagementServer::setRequestType(&response,
					ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_SHU_ERROR);
				mp_frame->sendFrame(&response);

				break;
			}

			//extract the shu information
			//shu name length
			unsigned int shu_name_length = 0;
			memcpy(&shu_name_length, message->get_mp_data()+type_size, sizeof(unsigned int));
			//shu name
			char* shu_name = new char[shu_name_length];
			memcpy(shu_name,message->get_mp_data()+type_size+sizeof(unsigned int),shu_name_length);

			//shu server ip length
			unsigned int shu_server_ip_length = 0;
			memcpy(&shu_server_ip_length,
					message->get_mp_data()+type_size+shu_name_length+sizeof(unsigned int),
					sizeof(unsigned int));
			//shu server ip
			char* shu_server_ip = new char[shu_server_ip_length];//printf("server ip length %u \n",shu_server_ip_length);
			memcpy(shu_server_ip,
					message->get_mp_data()+type_size+2*sizeof(unsigned int)+shu_name_length,
					shu_server_ip_length);

			//shu server port
			unsigned int shu_serverport = 0;
			memcpy(&shu_serverport,
					message->get_mp_data()+type_size+2*sizeof(unsigned int)+shu_name_length+shu_server_ip_length,
					sizeof(unsigned int));

			//and now get the data
			Lazarus::Buffer shu_data(message->get_m_length()-(type_size+3*sizeof(unsigned int)+shu_name_length+shu_server_ip_length));
			memcpy(shu_data.get_mp_data(),
				message->get_mp_data()+type_size+3*sizeof(unsigned int)+shu_name_length+shu_server_ip_length,
				shu_data.get_m_length()*sizeof(unsigned char));

			//check if directory with the unit name already exists under e.g. /tmp
			struct stat st = {0};
			std::string path = mp_node->m_shu_deployment_path + std::string("/")+std::string(shu_name);

			//if not create it
			if(stat(path.c_str(), &st) == -1)
			{
				mkdir(path.c_str(), 0777);
			}
			//if yes, delete it and recreate it
			else
			{
				std::string command = std::string("rm -rf ")+path;
				system(command.c_str());

				mkdir(path.c_str(), 0777);
			}

			//write the file
			std::string file = path + std::string("/");
			file = file + std::string(shu_name) + std::string(".tar.gz");

			printf("going to write the file %s\n",file.c_str());
			//save the file
			Lazarus::Toolbox::writeArrayToFile(file, shu_data.get_mp_data(), shu_data.get_m_length());

			//register the shu and the shu server within the shared memory object
			struct SharedMemSHUComStruct* shu_mem_object = (struct SharedMemSHUComStruct*)mp_shared_mem_raw;
			shu_mem_object->m_shu_server_port = shu_serverport;//shu server port
			shu_mem_object->m_shu_pid = 0;
			shu_mem_object->m_rapid_deployment = 0;
			shu_mem_object->m_synchronized_deployment = 1;
			int l = std::min(512,(int)(mp_node->m_system_db_file.size()+1));
			memcpy(shu_mem_object->m_system_db_filename,mp_node->m_system_db_file.c_str(),l);//sh db
			l = std::min(512,(int)(mp_node->m_shu_deployment_path.size()+1));
			memcpy(shu_mem_object->m_system_deployment_path,mp_node->m_shu_deployment_path.c_str(),l);//deployment path
			memcpy(shu_mem_object->m_shu_id,shu_name,shu_name_length);//shu id
			memcpy(shu_mem_object->mp_shu_server_ip,shu_server_ip,shu_server_ip_length);//shu server ip

			//printf("SHU data %s %u, com data %s %u\n",shu_server_ip,shu_serverport,shu_mem_object->mp_shu_server_ip,shu_mem_object->m_shu_server_port);

			delete[] shu_server_ip;
			delete[] shu_name;


			//extract the unit
			std::string command = std::string("tar -C")+ path + std::string(" -xzvf ")+ file ;
			system(command.c_str());

			//delete the archive
			command = std::string("rm ") + file ;
			system(command.c_str());

			//execute the deployment script in the background
			command = std::string("sh ") + path + std::string("/deployment.sh &");
			system(command.c_str());

			//wait until it started up
			int res = mp_shu_deployment_semaphore->decSemaphore();
			if(res == -1)
			{
				printf("ERROR: could not decrement the semaphore\n");
			}

			m_shu_active = true;

			//send a corresponding answer to the server
			Lazarus::Buffer response(type_size);
			ClusterNodeManagementServer::setRequestType(&response,
				ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_SHU_OK);printf("sending incoming shu sync OK\n");
			mp_frame->sendFrame(&response);

			break;
		}

		//client prepares the sh unit
		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_RAPID_SHU_DEPLOYMENT:
		{
			if(m_shu_active == true)
			{
				printf("ERROR: shu already active\n");

				//send a corresponding answer to the server
				Lazarus::Buffer response(type_size);
				ClusterNodeManagementServer::setRequestType(&response,
					ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_SHU_ERROR);
				mp_frame->sendFrame(&response);

				break;
			}

			//extract the shu information
			//shu name length
			unsigned int shu_name_length = 0;
			memcpy(&shu_name_length, message->get_mp_data()+type_size, sizeof(unsigned int));
			//shu name
			char* shu_name = new char[shu_name_length];
			memcpy(shu_name,message->get_mp_data()+type_size+sizeof(unsigned int),shu_name_length);

			//shu server ip length
			unsigned int shu_server_ip_length = 0;
			memcpy(&shu_server_ip_length,
					message->get_mp_data()+type_size+shu_name_length+sizeof(unsigned int),
					sizeof(unsigned int));
			//shu server ip
			char* shu_server_ip = new char[shu_server_ip_length];//printf("server ip length %u \n",shu_server_ip_length);
			memcpy(shu_server_ip,
					message->get_mp_data()+type_size+2*sizeof(unsigned int)+shu_name_length,
					shu_server_ip_length);

			//shu server port
			unsigned int shu_serverport = 0;
			memcpy(&shu_serverport,
					message->get_mp_data()+type_size+2*sizeof(unsigned int)+shu_name_length+shu_server_ip_length,
					sizeof(unsigned int));

			//check if directory with the unit name already exists under e.g. /tmp
			struct stat st = {0};
			std::string path = mp_node->m_shu_deployment_path + std::string("/")+std::string(shu_name);

			//if not create it
			if(stat(path.c_str(), &st) == -1)
			{
				mkdir(path.c_str(), 0777);
			}
			//if yes, delete it and recreate it
			else
			{
				std::string command = std::string("rm -rf ")+path;
				system(command.c_str());

				mkdir(path.c_str(), 0777);
			}


			break;
		}

		//client prepares the sh unit
		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_RAPID_SHU_DEPLOYMENT_SYNC:
		{
			if(m_shu_active == true)
			{
				printf("ERROR: shu already active\n");

				//send a corresponding answer to the server
				Lazarus::Buffer response(type_size);
				ClusterNodeManagementServer::setRequestType(&response,
					ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_SHU_ERROR);
				mp_frame->sendFrame(&response);

				break;
			}

			//extract the shu information
			//shu name length
			unsigned int shu_name_length = 0;
			memcpy(&shu_name_length, message->get_mp_data()+type_size, sizeof(unsigned int));
			//shu name
			char* shu_name = new char[shu_name_length];
			memcpy(shu_name,message->get_mp_data()+type_size+sizeof(unsigned int),shu_name_length);

			//shu server ip length
			unsigned int shu_server_ip_length = 0;
			memcpy(&shu_server_ip_length,
					message->get_mp_data()+type_size+shu_name_length+sizeof(unsigned int),
					sizeof(unsigned int));
			//shu server ip
			char* shu_server_ip = new char[shu_server_ip_length];//printf("server ip length %u \n",shu_server_ip_length);
			memcpy(shu_server_ip,
					message->get_mp_data()+type_size+2*sizeof(unsigned int)+shu_name_length,
					shu_server_ip_length);

			//shu server port
			unsigned int shu_serverport = 0;
			memcpy(&shu_serverport,
					message->get_mp_data()+type_size+2*sizeof(unsigned int)+shu_name_length+shu_server_ip_length,
					sizeof(unsigned int));

			//check if directory with the unit name already exists under e.g. /tmp
			struct stat st = {0};
			std::string path = mp_node->m_shu_deployment_path + std::string("/")+std::string(shu_name);

			//if not create it
			if(stat(path.c_str(), &st) == -1)
			{
				mkdir(path.c_str(), 0777);
			}
			//if yes, delete it and recreate it
			else
			{
				std::string command = std::string("rm -rf ")+path;
				system(command.c_str());

				mkdir(path.c_str(), 0777);
			}


			break;
		}

		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_ASYNC_SYSTEM_COMMAND:
		{
			unsigned int command_length = message->get_m_length()-type_size;

			//extract the command
			unsigned char command_buffer[command_length];
			memcpy(command_buffer,message->get_mp_data()+type_size,command_length);

			Lazarus::Buffer buffer( type_size );

			//execute the command
			int res = system((const char*)command_buffer);

			if(res == -1)
			{
				//send error response
				ClusterNodeManagementServer::setRequestType(&buffer,
						ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_ASYNC_SYSTEM_COMMAND_ERROR);

				mp_frame->sendFrame(&buffer);
			}

			//send error response
			ClusterNodeManagementServer::setRequestType(&buffer,
					ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_ASYNC_SYSTEM_COMMAND_OK);

			mp_frame->sendFrame(&buffer);

			break;
		}

		case ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_KILL_ACTIVE_SHU:
		{
			struct SharedMemSHUComStruct* shu_mem_object = (struct SharedMemSHUComStruct*)mp_shared_mem_raw;

			//if no shu has been deployed yet or if none is active
			if(shu_mem_object->m_shu_pid == 0)
			{
				//send a corresponding answer to the server
				Lazarus::Buffer response(type_size);
				ClusterNodeManagementServer::setRequestType(&response,
					ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_SHU_TERMINATE_ERROR);
				mp_frame->sendFrame(&response);

				break;
			}

			//kill the process
			int res = kill(shu_mem_object->m_shu_pid,SIGKILL);

			if(res == -1)
			{
				//send a corresponding answer to the server
				Lazarus::Buffer response(type_size);
				ClusterNodeManagementServer::setRequestType(&response,
					ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_SHU_TERMINATE_ERROR);
				mp_frame->sendFrame(&response);

				break;
			}

			//clean up the shu remains
			shu_mem_object->m_shu_pid = 0;
			m_shu_active = false;
			//check if directory with the unit name exists under e.g. /tmp
			struct stat st = {0};
			std::string path = mp_node->m_shu_deployment_path + std::string("/")+std::string((const char*)shu_mem_object->m_shu_id);

			//if yes, delete it
			if(stat(path.c_str(), &st) != -1)
			{
				std::string command = std::string("rm -rf ")+path;
				system(command.c_str());
			}

			//send a corresponding answer to the server
			Lazarus::Buffer response(type_size);
			ClusterNodeManagementServer::setRequestType(&response,
				ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_SHU_TERMINATED);
			mp_frame->sendFrame(&response);

			break;
		}

		default:
			printf("ERROR: unknown command received in mgmt client\n");
			break;
	}

}

const std::string ClusterNodeManagementClient::SHU_SHARED_MEM_NAME = std::string("CNMClientShared");

}
