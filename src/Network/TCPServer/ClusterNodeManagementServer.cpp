/*
 * ClusterNodeManagementServer.cpp
 *
 *  Created on: 12.06.2013
 *      Author: darius
 */

#include "ClusterNodeManagementServer.h"
#include "WorkerThreads/ClusterNodeManagementWorkerThread.h"
#include "../../Toolbox.h"
#include "../Nodes/Callbacks/SingleClusterNodeSynchronizationCallback.h"
#include "../Nodes/Callbacks/MultiClusterNodeSynchronizationCallback.h"

namespace Lazarus
{

ClusterNodeManagementServer::ClusterNodeManagementServer(std::string local_address,
		unsigned int local_port, unsigned int max_connections,
		unsigned int worker_threads) : ClusterLibServer(local_address,local_port, max_connections,worker_threads) {

	mp_cn_node_facility = new ClusterNodeComFacility();

	//************************** worker threads
	createWorkerThreads();

}

ClusterNodeManagementServer::~ClusterNodeManagementServer()
{
}

ClusterNode ClusterNodeManagementServer::getClusterNode(unsigned int nodeID)
{
	ClusterNode* node = ((ClusterNodeComFacility*)mp_cn_node_facility)->getNode(nodeID);

	if(node == NULL)
	{
		node = new ClusterNode(NULL);
	}

	return *node;
}

unsigned int ClusterNodeManagementServer::getNodeCount()
{
	Lazarus::FastKTuple< unsigned int >* node_ids = NULL;

	node_ids = ((ClusterNodeComFacility*)mp_cn_node_facility)->getNodeIDs();

	unsigned int node_count = node_ids->getm_size();
	delete node_ids;

	return node_count;
}

Lazarus::FastKTuple<unsigned int>* ClusterNodeManagementServer::getNodeIDs()
{
	Lazarus::FastKTuple< unsigned int >* node_ids = NULL;

	node_ids = ((ClusterNodeComFacility*)mp_cn_node_facility)->getNodeIDs();

	return node_ids;
}

int ClusterNodeManagementServer::killActiveSHU(unsigned int node_id)
{
	unsigned int type_size = sizeof(enum ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE);
	Lazarus::Buffer* buffer = new Lazarus::Buffer( type_size );

	//construct data
	setRequestType(buffer,ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_KILL_ACTIVE_SHU);

	//the callback
	SingleClusterNodeSynchronizationCallback comCallback;

	//send data via facility
	mp_cn_node_facility->sendRequestToNode(node_id,buffer,&comCallback);
	delete buffer;

	if(comCallback.m_sequence_complete == true)
	{
		return 0;
	}

	return -1;
}

void ClusterNodeManagementServer::createWorkerThreads()
{
	for(unsigned int i=0;i<m_worker_thread_count;i++)
	{
		//Create worker thread, update the pokers and start the thread
		mp_worker_threads[i] = new ClusterNodeManagementWorkerThread(m_max_connections / m_worker_thread_count, (ClusterNodeComFacility*)mp_cn_node_facility);

		mp_worker_threads[i]->setDisconnectCallback( this->mp_disconnect_callback );
		mp_worker_threads[i]->setConnectCallback( this->mp_connect_callback );
		mp_worker_threads[i]->setComFacility(mp_cn_node_facility);
		mp_worker_threads[i]->start(i);
	}
}

enum ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE ClusterNodeManagementServer::getRequestType(Lazarus::Buffer* message)
{
	enum ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE type;

	memcpy(&type,message->get_mp_data(),sizeof(enum ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE));

	return type;
}

void ClusterNodeManagementServer::setRequestType(Lazarus::Buffer* message,enum ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE type)
{
	memcpy(message->get_mp_data(),&type,sizeof(enum ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE));
}

void ClusterNodeManagementServer::setContent(Lazarus::Buffer* message,unsigned char* content, unsigned int size)
{
	unsigned int type_size = sizeof(enum ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE);

	memcpy(message->get_mp_data()+type_size,content,size*sizeof(unsigned char));
}

/*
 * this method will allocate an adequate buffer at 'content' and set 'size' accordingly
 */
void ClusterNodeManagementServer::getContent(Lazarus::Buffer* message,unsigned char** content, unsigned int &size)
{
	unsigned int type_size = sizeof(enum ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE);

	size = message->get_m_length()-type_size;

	*content = new unsigned char[size];

	memcpy(*content,message->get_mp_data()+type_size,size*sizeof(unsigned char));
}

Lazarus::Buffer* ClusterNodeManagementServer::sendSystemCommand(unsigned int node_id, std::string command)
{
	const char* command_string = command.c_str();
	unsigned int command_length = strlen(command_string)+1;
	unsigned int type_size = sizeof(enum ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE);
	Lazarus::Buffer* buffer = new Lazarus::Buffer( type_size + command_length );

	//construct data
	setRequestType(buffer,ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_SYSTEM_COMMAND);
	memcpy(buffer->get_mp_data()+type_size, command_string, command_length);

	//the callback
	SingleClusterNodeSynchronizationCallback comCallback;

	//send data via facility
	mp_cn_node_facility->sendRequestToNode(node_id,buffer,&comCallback);
	delete buffer;

	if(comCallback.m_sequence_complete == true)
	{
		return comCallback.mp_data;
	}

	return NULL;
}

int ClusterNodeManagementServer::sendAsyncSystemCommand(unsigned int node_id, std::string command)
{

	const char* command_string = command.c_str();
	unsigned int command_length = strlen(command_string)+1;
	unsigned int type_size = sizeof(enum ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE);
	Lazarus::Buffer* buffer = new Lazarus::Buffer( type_size + command_length );

	//construct data
	setRequestType(buffer,ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_ASYNC_SYSTEM_COMMAND);
	memcpy(buffer->get_mp_data()+type_size, command_string, command_length);

	//the callback
	SingleClusterNodeSynchronizationCallback comCallback;

	//send data via facility
	mp_cn_node_facility->sendRequestToNode(node_id,buffer,&comCallback);
	delete buffer;

	if(comCallback.m_sequence_complete == true)
	{
		return 0;
	}

	return -1;
}

Lazarus::Buffer* ClusterNodeManagementServer::requestDummyData(unsigned int node_id, unsigned int bytes)
{

	unsigned int type_size = sizeof(enum ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE);
	Lazarus::Buffer* buffer = new Lazarus::Buffer( type_size + sizeof(unsigned int) );

	//construct data
	setRequestType(buffer,ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_GET_DUMMY_DATA);
	memcpy(buffer->get_mp_data()+type_size, &bytes, sizeof(unsigned int));

	//send data via facility
	//printf("requesting dummy data\n");

	//the callback
	SingleClusterNodeSynchronizationCallback comCallback;

	//send data via facility
	mp_cn_node_facility->sendRequestToNode(node_id,buffer,&comCallback);
	delete buffer;

	//return
	return comCallback.mp_data;
}

long long ClusterNodeManagementServer::getSHUPID(unsigned int node_id)
{
	unsigned int type_size = sizeof(enum ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE);
	Lazarus::Buffer* buffer = new Lazarus::Buffer( type_size );

	//construct data
	setRequestType(buffer,ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_GET_SHU_PID);

	//the callback
	SingleClusterNodeSynchronizationCallback comCallback;

	//send data via facility
	mp_cn_node_facility->sendRequestToNode(node_id,buffer,&comCallback);
	delete buffer;

	if(comCallback.m_sequence_complete == true)
	{
		unsigned long long pid = 0;
		memcpy(&pid, comCallback.mp_data->get_mp_data(), sizeof(unsigned long long));
		//printf("got pid %llu | %llu\n", *((unsigned long long*)(comCallback.mp_data->get_mp_data())),pid );
		delete comCallback.mp_data;
		return pid;
	}

	//return
	return -1;
}

Lazarus::SystemStatus* ClusterNodeManagementServer::requestSystemReport(unsigned int node_id)
{

	unsigned int type_size = sizeof(enum ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE);
	Lazarus::Buffer* buffer = new Lazarus::Buffer( type_size );

	//construct data
	setRequestType(buffer,ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_GET_SYSTEM_STATE);

	//send data via facility
	//printf("requesting system state\n");

	//the callback
	SingleClusterNodeSynchronizationCallback comCallback;

	//send data via facility
	mp_cn_node_facility->sendRequestToNode(node_id,buffer,&comCallback);
	delete buffer;

	if(comCallback.m_sequence_complete == false)
	{
		return NULL;
	}

	//print&free received dummy data
	Lazarus::SystemStatus* system_status = new Lazarus::SystemStatus(false);

	//reconstruct the system object
	system_status->set_mp_serialized_data_buffer(comCallback.mp_data);
	system_status->deserialize();

	return system_status;
}

Lazarus::FastKTuple<double>* ClusterNodeManagementServer::requestShortCPUReport(unsigned int node_id)
{
	unsigned int type_size = sizeof(enum ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE);
	Lazarus::Buffer* buffer = new Lazarus::Buffer( type_size );

	//construct data
	setRequestType(buffer,ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_GET_SHORT_SYSTEM_CPU_STATE);

	//the callback
	SingleClusterNodeSynchronizationCallback comCallback;

	//send data via facility
	mp_cn_node_facility->sendRequestToNode(node_id,buffer,&comCallback);
	delete buffer;

	if(comCallback.m_sequence_complete == false)
	{
		return NULL;
	}

	Lazarus::FastKTuple<double>* loads = new Lazarus::FastKTuple<double>( comCallback.mp_data->get_m_length() / sizeof(double) );

	double* dat = (double*)comCallback.mp_data->get_mp_data();
	for(long long int i=0;i<loads->getm_size();++i)
	{
		(*loads)[i] = dat[i];
	}

	return loads;
}

int ClusterNodeManagementServer::sendFile(unsigned int node_id,  std::string local_file, std::string destination_file)
{
	const char* source_file_string = local_file.c_str();
	unsigned int source_file_name_length = strlen(source_file_string)+1;
	const char* target_file_string = destination_file.c_str();
	unsigned int target_file_name_length = strlen(target_file_string)+1;

	long long int data_size = Lazarus::Toolbox::getFileSize(local_file);

	//if file not found
	if(data_size == -1)
	{
		return -1;
	}

	//check if large file support is needed
	if(data_size <= LARGE_FILE_THRESHOLD)//small files
	{
		unsigned int type_size = sizeof(enum ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE);
		Lazarus::Buffer* buffer = new Lazarus::Buffer( type_size + source_file_name_length +
				target_file_name_length + 2*sizeof(unsigned int) + data_size );


		//construct data
		//insert type
		setRequestType(buffer,ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_INCOMING_FILE);
		//insert source filename length
		memcpy(buffer->get_mp_data()+type_size, &source_file_name_length, sizeof(unsigned int));
		//insert source filename
		memcpy(buffer->get_mp_data()+type_size+sizeof(unsigned int), source_file_string, source_file_name_length);
		//insert destination filename length
		memcpy(buffer->get_mp_data()+type_size+sizeof(unsigned int)+source_file_name_length,
				&target_file_name_length, sizeof(unsigned int));
		//insert the destination filename
		memcpy(buffer->get_mp_data()+type_size+2*sizeof(unsigned int)+source_file_name_length,
				target_file_string, target_file_name_length);
		//now the file data
		Lazarus::Toolbox::readFileToArray(local_file,
				buffer->get_mp_data()+type_size+2*sizeof(unsigned int)+source_file_name_length+target_file_name_length);

		//the callback
		SingleClusterNodeSynchronizationCallback comCallback;

		//send data via facility
		mp_cn_node_facility->sendRequestToNode(node_id,buffer,&comCallback);
		delete buffer;

		if(comCallback.m_sequence_complete == true)
		{
			return 0;
		}

		return -1;
	}
	else //large file: will be send in chunks of sizes up to 'LARGE_FILE_THRESHOLD'
	{
		//the callback
		SingleClusterNodeSynchronizationCallback comCallback;

		//determine the amount of chunks
		long long chunk_count = data_size / LARGE_FILE_THRESHOLD;
		long long last_chunk_size = data_size % LARGE_FILE_THRESHOLD;

		if(data_size % LARGE_FILE_THRESHOLD != 0)
		{
			chunk_count+=1;
		}

		//get the file reader ready
		FILE* large_file = fopen64(local_file.c_str(),"rb");

		unsigned int type_size = sizeof(enum ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE);

		//--------- send the first chunk(this one includes the filesize which the client can use to determine the amount of chunks)
		//we will always have at least one chunk, the first one will always be of size 'LARGE_FILE_THRESHOLD'!
		Lazarus::Buffer* buffer = new Lazarus::Buffer( type_size + source_file_name_length +
				target_file_name_length + 2*sizeof(unsigned int) + LARGE_FILE_THRESHOLD + sizeof(long long int) );

		//construct data
		//insert type
		setRequestType(buffer,ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_INCOMING_LARGE_FILE);
		//insert source filename length
		memcpy(buffer->get_mp_data()+type_size, &source_file_name_length, sizeof(unsigned int));
		//insert source filename
		memcpy(buffer->get_mp_data()+type_size+sizeof(unsigned int), source_file_string, source_file_name_length);
		//insert destination filename length
		memcpy(buffer->get_mp_data()+type_size+sizeof(unsigned int)+source_file_name_length,
				&target_file_name_length, sizeof(unsigned int));
		//insert the destination filename
		memcpy(buffer->get_mp_data()+type_size+2*sizeof(unsigned int)+source_file_name_length,
				target_file_string, target_file_name_length);
		//the file size
		memcpy(buffer->get_mp_data()+type_size+2*sizeof(unsigned int)+source_file_name_length + target_file_name_length,
						&data_size, sizeof(long long int));
		//now the chunk data
		Lazarus::Toolbox::readFileToArray(large_file,
				buffer->get_mp_data()+type_size+2*sizeof(unsigned int)+source_file_name_length+target_file_name_length+sizeof(long long int),
				LARGE_FILE_THRESHOLD,0);

		comCallback.reset();

		//send data via facility (and wait for confirmation)
		mp_cn_node_facility->sendRequestToNode(node_id,buffer,&comCallback);
		delete buffer;

		//check if an error occurred
		if(comCallback.m_sequence_complete != true)
		{
			//close the file
			fclose(large_file);
			return -1;
		}


		//--------- send out the remaining chunks, one after the other
		for(long long int i = 1; i<chunk_count; ++i)
		{
			//last iteration?
			if(i == chunk_count-1)
			{
				buffer = new Lazarus::Buffer( type_size + last_chunk_size );

				//now the file data
				Lazarus::Toolbox::readFileToArray(large_file,
					buffer->get_mp_data()+type_size,
					last_chunk_size,i*LARGE_FILE_THRESHOLD);
			}
			else//in case it's not the last iteration
			{
				buffer = new Lazarus::Buffer( type_size + LARGE_FILE_THRESHOLD );

				//now the file data
				Lazarus::Toolbox::readFileToArray(large_file,
					buffer->get_mp_data()+type_size,
					LARGE_FILE_THRESHOLD,i*LARGE_FILE_THRESHOLD);
			}

			//construct data
			//insert type
			setRequestType(buffer,ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_INCOMING_LARGE_FILE_CHUNK);

			comCallback.reset();

			//send data via facility (and wait for feedback)
			mp_cn_node_facility->sendRequestToNodeNL(node_id,buffer,&comCallback);
			delete buffer;

			//check if an error occurred
			if(comCallback.m_sequence_complete != true)
			{
				//close the file
				fclose(large_file);
				return -1;
			}
		}

		//close the file
		fclose(large_file);

		return 0;
	}

	return -1;
}

Lazarus::Buffer* ClusterNodeManagementServer::getFile(unsigned int node_id, std::string remote_file)
{
	unsigned int type_size = sizeof(enum ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE);
	Lazarus::Buffer* buffer = new Lazarus::Buffer( type_size + remote_file.size() + 1 );

	//construct data
	setRequestType(buffer,ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_GET_FILE);
	memcpy(buffer->get_mp_data()+type_size, remote_file.c_str(), remote_file.size()+1);

	//send data via facility

	//the callback
	SingleClusterNodeSynchronizationCallback comCallback;

	//send data via facility
	mp_cn_node_facility->sendRequestToNode(node_id,buffer,&comCallback);
	delete buffer;

	if(comCallback.m_sequence_complete == true)
	{
		return comCallback.mp_data;
	}

	return NULL;
}

int ClusterNodeManagementServer::getLargeFile(unsigned int node_id, std::string remote_file, std::string local_file)
{
	unsigned int type_size = sizeof(enum ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE);
	Lazarus::Buffer* buffer = new Lazarus::Buffer( type_size + remote_file.size() + 1 );

	//construct data
	setRequestType(buffer,ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_GET_LARGE_FILE);
	memcpy(buffer->get_mp_data()+type_size, remote_file.c_str(), remote_file.size()+1);

	//send data via facility

	//the callback
	SingleClusterNodeSynchronizationCallback comCallback;

	//request the first chunk (and wait for it)
	mp_cn_node_facility->sendRequestToNode(node_id,buffer,&comCallback);
	delete buffer;

	//in case of an error: abort
	if(comCallback.m_sequence_complete != true)
	{
		return -1;
	}

	//get the amount expected data
	long long data_size = 0;
	memcpy(&data_size, comCallback.mp_data->get_mp_data(), sizeof(long long int));

	//calculate the number of expected chunks
	long long int chunk_count = data_size / ClusterNodeManagementServer::LARGE_FILE_THRESHOLD;

	if(data_size % ClusterNodeManagementServer::LARGE_FILE_THRESHOLD != 0 )
	{
		chunk_count+=1;
	}

	//prepare the file
	unlink(local_file.c_str());//delete any existing file
	FILE* large_file = fopen64(local_file.c_str(),"ab");

	//write the first chunk
	Lazarus::Toolbox::writeArrayToFile(large_file,
			comCallback.mp_data->get_mp_data()+sizeof(long long int),
			comCallback.mp_data->get_m_length()-+sizeof(long long int));

	//free the the memory
	delete comCallback.mp_data;

	//there may be more chunks (the server won't ask for more if there arent't any left!)
	for(long long int i=1;i<chunk_count;++i)
	{
		comCallback.reset();

		buffer = new Lazarus::Buffer( type_size );

		setRequestType(buffer,ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_GET_NEXT_LARGE_FILE_CHUNK);

		//send data via facility (and wait for the next chunk)
		mp_cn_node_facility->sendRequestToNodeNL(node_id,buffer,&comCallback);
		delete buffer;

		//if the next chunk could not be delivered
		if(comCallback.m_sequence_complete != true)
		{
			fclose(large_file);
			unlink(local_file.c_str());//delete the incomplete file
			return -1;
		}

		//save the file chunk
		Lazarus::Toolbox::writeArrayToFile(large_file,
				comCallback.mp_data->get_mp_data(),
				comCallback.mp_data->get_m_length());

		//free the the memory
		delete comCallback.mp_data;
	}

	//we are nearly done
	comCallback.reset();

	//once all chunks have been received: terminate the sequence
	buffer = new Lazarus::Buffer( type_size );

	setRequestType(buffer,ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_GET_LARGE_FILE_COMPLETE);

	//send data via facility (and wait for a confirmation, i.e. 'CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_GET_LARGE_FILE_OK')
	//this response will also unlock the node for further com functionality!
	mp_cn_node_facility->sendRequestToNodeNL(node_id,buffer,&comCallback);
	delete buffer;

	//even though we have received all chunks; in case of a final error the file will be dropped
	if(comCallback.m_sequence_complete != true)
	{
		fclose(large_file);
		unlink(local_file.c_str());//delete the incomplete file
		return -1;
	}

	//finally close the file
	fclose(large_file);

	return 0;
}

Lazarus::FastKTuple< Lazarus::FastVector2<unsigned int, Lazarus::FastKTuple<double>*> >*
	ClusterNodeManagementServer::getNodeLoads(bool use_semaphore_wait)
{
	Lazarus::FastKTuple< unsigned int >* node_ids = ((ClusterNodeComFacility*)mp_cn_node_facility)->getNodeIDs();

	Lazarus::FastKTuple< Lazarus::FastVector2<unsigned int, Lazarus::FastKTuple<double>*>  >* loads =
			new Lazarus::FastKTuple< Lazarus::FastVector2<unsigned int, Lazarus::FastKTuple<double>*>  >( 0 );

	unsigned int type_size = sizeof(enum ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE);
	Lazarus::Buffer* buffer = new Lazarus::Buffer( type_size );

	//the callback
	MultiClusterNodeSynchronizationCallback comCallback(*node_ids,use_semaphore_wait);

	//construct data
	setRequestType(buffer,ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_GET_SHORT_SYSTEM_CPU_STATE);

	//send data via facility
	mp_cn_node_facility->sendRequestToMultipleNodes(*node_ids,buffer,&comCallback);
	delete buffer;

	//now extract all received answers
	Lazarus::Buffer* data = NULL;
	double* load_data = NULL;
	unsigned int node_id = 0;
	unsigned int entry_count = 0;
	Lazarus::FastKTuple<double>* cpu_loads = NULL;

	for(long long int i=0;i < node_ids->getm_size();++i)
	{
		if((*comCallback.mp_sequence_complete)[i].m_data2 == false)
		{
			//data will be NULL
			printf("ERROR: node %u did not answer completely\n",(*comCallback.mp_sequence_complete)[i].m_data1);
		}
		else
		{
			node_id = (*comCallback.mp_data)[i].m_data1;
			data = (*comCallback.mp_data)[i].m_data2;
			load_data = (double*)data->get_mp_data();

			//how many load entries are there?
			entry_count = data->get_m_length() / sizeof(double);

			cpu_loads = new Lazarus::FastKTuple<double>(entry_count);

			//add the entries to the tuple
			for(unsigned int j=0;j<entry_count;++j)
			{
				(*cpu_loads)[j] = load_data[j];
			}

			loads->push( Lazarus::FastVector2<unsigned int, Lazarus::FastKTuple<double>*>(node_id,cpu_loads) );

			delete data;
		}
	}

	return loads;
}


int ClusterNodeManagementServer::sendSHUnit(unsigned int node_id, const std::string& file, const std::string& sh_id,
		const std::string& shu_server_ip, unsigned int shu_server_port, bool use_semaphore_wait)
{
	const char* id_string = sh_id.c_str();
	unsigned int id_name_length = strlen(id_string)+1;

	const char* shu_ip_string = shu_server_ip.c_str();
	unsigned int shu_ip_length = strlen(shu_ip_string)+1;

	long int file_size = Lazarus::Toolbox::getFileSize(file);
	if(file_size == -1)//in case of file errors
	{
		return -1;
	}

	unsigned int type_size = sizeof(enum ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE);
	Lazarus::Buffer* buffer = new Lazarus::Buffer( type_size + 3*sizeof(unsigned int) + id_name_length + shu_ip_length + file_size );

	//construct data
	setRequestType(buffer,ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_INCOMING_SHU);
	//insert the id length
	memcpy(buffer->get_mp_data()+type_size, &id_name_length, sizeof(unsigned int));
	//insert the id
	memcpy(buffer->get_mp_data()+type_size + sizeof(unsigned int), id_string, id_name_length);
	//insert the ip length
	memcpy(buffer->get_mp_data()+type_size + sizeof(unsigned int) + id_name_length,
			&shu_ip_length, sizeof(unsigned int));
	//insert the ip
	memcpy(buffer->get_mp_data()+type_size + 2*sizeof(unsigned int) + id_name_length,
			shu_ip_string, shu_ip_length);
	//insert the port
	memcpy(buffer->get_mp_data()+type_size + 2*sizeof(unsigned int) + id_name_length + shu_ip_length,
			&shu_server_port, sizeof(unsigned int));
	//and the file data
	Lazarus::Toolbox::readFileToArray(file,
			buffer->get_mp_data()+type_size + 3*sizeof(unsigned int) + id_name_length + shu_ip_length);

	//the callback
	SingleClusterNodeSynchronizationCallback comCallback(use_semaphore_wait);

	//send data via facility
	mp_cn_node_facility->sendRequestToNode(node_id,buffer,&comCallback);
	delete buffer;

	if(comCallback.m_sequence_complete == true)
	{
		return 0;
	}

	return -1;

}


int ClusterNodeManagementServer::sendSHUnitSync(unsigned int node_id, const std::string& file, const std::string& sh_id,
		const std::string& shu_server_ip, unsigned int shu_server_port, bool use_semaphore_wait)
{
	const char* id_string = sh_id.c_str();
	unsigned int id_name_length = strlen(id_string)+1;

	const char* shu_ip_string = shu_server_ip.c_str();
	unsigned int shu_ip_length = strlen(shu_ip_string)+1;

	long int file_size = Lazarus::Toolbox::getFileSize(file);
	if(file_size == -1)//in case of file errors
	{
		return -1;
	}

	unsigned int type_size = sizeof(enum ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE);
	Lazarus::Buffer* buffer = new Lazarus::Buffer( type_size + 3*sizeof(unsigned int) + id_name_length + shu_ip_length + file_size );

	//construct data
	setRequestType(buffer,ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_INCOMING_SHU_SYNC);
	//insert the id length
	memcpy(buffer->get_mp_data()+type_size, &id_name_length, sizeof(unsigned int));
	//insert the id
	memcpy(buffer->get_mp_data()+type_size + sizeof(unsigned int), id_string, id_name_length);
	//insert the ip length
	memcpy(buffer->get_mp_data()+type_size + sizeof(unsigned int) + id_name_length,
			&shu_ip_length, sizeof(unsigned int));
	//insert the ip
	memcpy(buffer->get_mp_data()+type_size + 2*sizeof(unsigned int) + id_name_length,
			shu_ip_string, shu_ip_length);
	//insert the port
	memcpy(buffer->get_mp_data()+type_size + 2*sizeof(unsigned int) + id_name_length + shu_ip_length,
			&shu_server_port, sizeof(unsigned int));
	//and the file data
	Lazarus::Toolbox::readFileToArray(file,
			buffer->get_mp_data()+type_size + 3*sizeof(unsigned int) + id_name_length + shu_ip_length);

	//the callback
	SingleClusterNodeSynchronizationCallback comCallback(use_semaphore_wait);

	//send data via facility
	mp_cn_node_facility->sendRequestToNode(node_id,buffer,&comCallback);
	delete buffer;

	if(comCallback.m_sequence_complete == true)
	{
		return 0;
	}

	return -1;

}


int ClusterNodeManagementServer::rapidSHUnit(unsigned int node_id, const std::string& sh_id,
		const std::string& shu_server_ip, unsigned int shu_server_port, bool use_semaphore_wait)
{
	const char* id_string = sh_id.c_str();
	unsigned int id_name_length = strlen(id_string)+1;

	const char* shu_ip_string = shu_server_ip.c_str();
	unsigned int shu_ip_length = strlen(shu_ip_string)+1;

	unsigned int type_size = sizeof(enum ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE);
	Lazarus::Buffer* buffer = new Lazarus::Buffer( type_size + 3*sizeof(unsigned int) + id_name_length + shu_ip_length );

	//construct data
	setRequestType(buffer,ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_RAPID_SHU_DEPLOYMENT);
	//insert the id length
	memcpy(buffer->get_mp_data()+type_size, &id_name_length, sizeof(unsigned int));
	//insert the id
	memcpy(buffer->get_mp_data()+type_size + sizeof(unsigned int), id_string, id_name_length);
	//insert the ip length
	memcpy(buffer->get_mp_data()+type_size + sizeof(unsigned int) + id_name_length,
			&shu_ip_length, sizeof(unsigned int));
	//insert the ip
	memcpy(buffer->get_mp_data()+type_size + 2*sizeof(unsigned int) + id_name_length,
			shu_ip_string, shu_ip_length);
	//insert the port
	memcpy(buffer->get_mp_data()+type_size + 2*sizeof(unsigned int) + id_name_length + shu_ip_length,
			&shu_server_port, sizeof(unsigned int));

	//the callback
	SingleClusterNodeSynchronizationCallback comCallback(use_semaphore_wait);

	//send data via facility
	mp_cn_node_facility->sendRequestToNode(node_id,buffer,&comCallback);
	delete buffer;

	if(comCallback.m_sequence_complete == true)
	{
		return 0;
	}

	return -1;

}

int ClusterNodeManagementServer::rapidSHUnitSync(unsigned int node_id, const std::string& sh_id,
		const std::string& shu_server_ip, unsigned int shu_server_port, bool use_semaphore_wait)
{
	const char* id_string = sh_id.c_str();
	unsigned int id_name_length = strlen(id_string)+1;

	const char* shu_ip_string = shu_server_ip.c_str();
	unsigned int shu_ip_length = strlen(shu_ip_string)+1;

	unsigned int type_size = sizeof(enum ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE);
	Lazarus::Buffer* buffer = new Lazarus::Buffer( type_size + 3*sizeof(unsigned int) + id_name_length + shu_ip_length );

	//construct data
	setRequestType(buffer,ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_REQUEST_TYPE_RAPID_SHU_DEPLOYMENT_SYNC);
	//insert the id length
	memcpy(buffer->get_mp_data()+type_size, &id_name_length, sizeof(unsigned int));
	//insert the id
	memcpy(buffer->get_mp_data()+type_size + sizeof(unsigned int), id_string, id_name_length);
	//insert the ip length
	memcpy(buffer->get_mp_data()+type_size + sizeof(unsigned int) + id_name_length,
			&shu_ip_length, sizeof(unsigned int));
	//insert the ip
	memcpy(buffer->get_mp_data()+type_size + 2*sizeof(unsigned int) + id_name_length,
			shu_ip_string, shu_ip_length);
	//insert the port
	memcpy(buffer->get_mp_data()+type_size + 2*sizeof(unsigned int) + id_name_length + shu_ip_length,
			&shu_server_port, sizeof(unsigned int));

	//the callback
	SingleClusterNodeSynchronizationCallback comCallback(use_semaphore_wait);

	//send data via facility
	mp_cn_node_facility->sendRequestToNode(node_id,buffer,&comCallback);
	delete buffer;

	if(comCallback.m_sequence_complete == true)
	{
		return 0;
	}

	return -1;

}

}
