/*
 * ExtendedNetworkNode.cpp
 *
 *  Created on: Aug 24, 2013
 *      Author: Darius Malysiak
 */

#include "ExtendedNetworkNode.h"
#include "../../Toolbox.h"
#include "../../XML/ConfigReaderMarkI.h"

#define EXTNETNODE_UINT_CAST(PARAM) ((Lazarus::XMLConfigMarkIUInt*)(it.getElement()->getm_data()->getParameter(#PARAM)))->getParam()
#define EXTNETNODE_STRING_CAST(PARAM) ((Lazarus::XMLConfigMarkIString*)(it.getElement()->getm_data()->getParameter(#PARAM)))->getParam()
#define EXTNETNODE_STRINGARRAY_CAST(PARAM) ((Lazarus::XMLConfigMarkIStringArray*)(it.getElement()->getm_data()->getParameter(#PARAM)))
#define EXTNETNODE_UINTARRAY_CAST(PARAM) ((Lazarus::XMLConfigMarkIUIntArray*)(it.getElement()->getm_data()->getParameter(#PARAM)))


namespace Lazarus {

ExtendedNetworkNode::ExtendedNetworkNode() {

	m_physical_cpu_count = 0;
	m_cores_per_cpu = 0;
	m_system_ram = 0;
	m_hdd_count = 0;//amount of hdd devices
	mp_hdd_devs = NULL;//list of device names
	m_opencl_capable_devices = 0;
	m_cuda_capable_devices = 0;
	mp_ocl_platform_indices = NULL;//first tuple element (platform,device) for each OCL device
	mp_ocl_device_indices = NULL;//second tuple element
	mp_ocl_types = NULL;//types of OCL devices
	mp_cuda_types = NULL;
	m_desired_node_id = 0;
	mp_system_capabilities = NULL;
	m_system_capability_count = 0;
}

ExtendedNetworkNode::ExtendedNetworkNode(ExtendedNetworkNode* enn)
{
	m_node_ip = enn->m_node_ip;
	m_node_id = enn->m_node_id;
	m_desired_node_id = enn->m_desired_node_id;
	m_shu_deployment_path = enn->m_shu_deployment_path;
	m_opencl_capable_devices = enn->m_opencl_capable_devices;
	m_cuda_capable_devices = enn->m_cuda_capable_devices;
	m_physical_cpu_count = enn->m_physical_cpu_count;
	m_cores_per_cpu = enn->m_cores_per_cpu;
	m_system_ram = enn->m_system_ram;
	m_hdd_count = enn->m_hdd_count;//amount of hdd devices
	m_system_capability_count = enn->m_system_capability_count;

	mp_hdd_devs = new std::string[m_hdd_count];
	mp_ocl_platform_indices = new unsigned int[m_opencl_capable_devices];
	mp_ocl_device_indices = new unsigned int[m_opencl_capable_devices];
	mp_ocl_types = new std::string[m_opencl_capable_devices];
	mp_cuda_types = new std::string[m_cuda_capable_devices];
	mp_system_capabilities = new std::string[m_system_capability_count];

	for(unsigned int i=0;i<m_system_capability_count;++i)
	{
		mp_system_capabilities[i] = enn->mp_system_capabilities[i];
	}

	for(unsigned int i=0;i<m_hdd_count;++i)
	{
		mp_hdd_devs[i] = enn->mp_hdd_devs[i];
	}

	for(unsigned int i=0;i<m_opencl_capable_devices;++i)
	{
		mp_ocl_types[i] = enn->mp_ocl_types[i];
		mp_ocl_platform_indices[i] = enn->mp_ocl_platform_indices[i];
		mp_ocl_device_indices[i] = enn->mp_ocl_device_indices[i];
	}

	for(unsigned int i=0;i<m_cuda_capable_devices;++i)
	{
		mp_cuda_types[i] = enn->mp_cuda_types[i];
	}

}

ExtendedNetworkNode::ExtendedNetworkNode(const ExtendedNetworkNode& enn)
{
	m_node_ip = enn.m_node_ip;
	m_node_id = enn.m_node_id;
	m_desired_node_id = enn.m_desired_node_id;
	m_shu_deployment_path = enn.m_shu_deployment_path;
	m_opencl_capable_devices = enn.m_opencl_capable_devices;
	m_cuda_capable_devices = enn.m_cuda_capable_devices;
	m_physical_cpu_count = enn.m_physical_cpu_count;
	m_cores_per_cpu = enn.m_cores_per_cpu;
	m_system_ram = enn.m_system_ram;
	m_hdd_count = enn.m_hdd_count;//amount of hdd devices
	m_system_capability_count = enn.m_system_capability_count;

	mp_hdd_devs = new std::string[m_hdd_count];
	mp_ocl_platform_indices = new unsigned int[m_opencl_capable_devices];
	mp_ocl_device_indices = new unsigned int[m_opencl_capable_devices];
	mp_ocl_types = new std::string[m_opencl_capable_devices];
	mp_cuda_types = new std::string[m_cuda_capable_devices];
	mp_system_capabilities = new std::string[m_system_capability_count];

	for(unsigned int i=0;i<m_system_capability_count;++i)
	{
		mp_system_capabilities[i] = enn.mp_system_capabilities[i];
	}

	for(unsigned int i=0;i<m_hdd_count;++i)
	{
		mp_hdd_devs[i] = enn.mp_hdd_devs[i];
	}

	for(unsigned int i=0;i<m_opencl_capable_devices;++i)
	{
		mp_ocl_types[i] = enn.mp_ocl_types[i];
		mp_ocl_platform_indices[i] = enn.mp_ocl_platform_indices[i];
		mp_ocl_device_indices[i] = enn.mp_ocl_device_indices[i];
	}

	for(unsigned int i=0;i<m_cuda_capable_devices;++i)
	{
		mp_cuda_types[i] = enn.mp_cuda_types[i];
	}

}

ExtendedNetworkNode::~ExtendedNetworkNode() {

	DELETE_ARRAY_NULL_CHECKING(mp_hdd_devs);
	DELETE_ARRAY_NULL_CHECKING(mp_ocl_platform_indices);
	DELETE_ARRAY_NULL_CHECKING(mp_ocl_device_indices);
	DELETE_ARRAY_NULL_CHECKING(mp_ocl_types);
	DELETE_ARRAY_NULL_CHECKING(mp_cuda_types);
	DELETE_ARRAY_NULL_CHECKING(mp_system_capabilities);
}


void ExtendedNetworkNode::printNodeInfo()
{
	printf("********************Printing cluster node specs:\n");
	printf("Node name: %s\n",m_node_name.c_str());
	printf("Node os name: %s\n",m_os_name.c_str());
	printf("Node ID: %d\n",m_node_id);
	printf("Node IP: %s\n",m_node_ip.c_str());
	printf("Desired node ID: %d\n",m_desired_node_id);
	printf("Node CPU count: %d\n",m_physical_cpu_count);
	printf("Node cores per CPU: %d\n",m_cores_per_cpu);
	printf("Node logical CPU count: %d\n",m_physical_cpu_count*m_cores_per_cpu);
	printf("Node system RAM: %llu\n",m_system_ram);
	printf("Node OpenCL device count: %d\n",m_opencl_capable_devices);
	printf("Node Cuda device count: %d\n",m_cuda_capable_devices);
	printf("SHU deployment path: %s\n",m_shu_deployment_path.c_str());
	printf("SH DB path: %s\n",m_system_db_file.c_str());

	for(unsigned int i=0;i<m_system_capability_count;++i)
	{
		printf("System capability %s, \n",mp_system_capabilities[i].c_str());
	}

	for(unsigned int i=0;i<m_hdd_count;++i)
	{
		printf("HDD %s, \n",mp_hdd_devs[i].c_str());
	}

	for(unsigned int i=0;i<m_opencl_capable_devices;++i)
	{
		printf("OCL Dev %s (%u,%u)\n",mp_ocl_types[i].c_str(),mp_ocl_platform_indices[i],mp_ocl_device_indices[i]);
	}

	for(unsigned int i=0;i<m_cuda_capable_devices;++i)
	{
		printf("CUDA Dev %s \n",mp_cuda_types[i].c_str());
	}

	printf("********************\n");

}

void ExtendedNetworkNode::serialize()
{

	//total size of required serialization buffer
	unsigned long total_serialized_data_size =
			getRequiredStringBufferSize(m_node_name) //string
			+getRequiredStringBufferSize(m_os_name)//string
			+getRequiredStringBufferSize(m_node_ip)//string
			+getRequiredStringBufferSize(m_architecture)//string
			+getRequiredStringBufferSize(m_shu_deployment_path)//string
			+getRequiredStringBufferSize(m_system_db_file)//string
			+(Serializable::UINT_SIZE*8)//all uints
			+Serializable::ULONGLONG_SIZE//all ulonglongs
			+(Serializable::UINT_SIZE*2*m_opencl_capable_devices)//the (platform,dev) index tuple
			;

	//now add the sizes of all strings within the arrays
	for(unsigned int i=0;i<m_system_capability_count;++i)
	{
		total_serialized_data_size += getRequiredStringBufferSize(mp_system_capabilities[i]);
	}

	for(unsigned int i=0;i<m_hdd_count;++i)
	{
		total_serialized_data_size += getRequiredStringBufferSize(mp_hdd_devs[i]);
	}

	for(unsigned int i=0;i<m_opencl_capable_devices;++i)
	{
		total_serialized_data_size += getRequiredStringBufferSize(mp_ocl_types[i]);
	}

	for(unsigned int i=0;i<m_cuda_capable_devices;++i)
	{
		total_serialized_data_size += getRequiredStringBufferSize(mp_cuda_types[i]);
	}

	//allocate the serialization buffer
	DELETE_NULL_CHECKING(mp_serialized_data_buffer);
	resetSerializationOffset();
	this->mp_serialized_data_buffer = new Lazarus::Buffer(total_serialized_data_size);

	//stack the data up

	//first the string arrays
	for(unsigned int i=0;i<m_system_capability_count;++i)
	{
		addString(mp_system_capabilities[i]);
	}

	for(unsigned int i=0;i<m_hdd_count;++i)
	{
		addString(mp_hdd_devs[i]);
	}

	for(unsigned int i=0;i<m_opencl_capable_devices;++i)
	{
		addString(mp_ocl_types[i]);
	}

	for(unsigned int i=0;i<m_opencl_capable_devices;++i)
	{
		addUInt(mp_ocl_platform_indices[i]);
		addUInt(mp_ocl_device_indices[i]);
	}

	for(unsigned int i=0;i<m_cuda_capable_devices;++i)
	{
		addString(mp_cuda_types[i]);
	}

	addString(m_node_ip);
	addString(m_system_db_file);
	addString(m_shu_deployment_path);
	addString(m_node_name);
	addString(m_os_name);
	addString(m_architecture);
	addUInt(m_physical_cpu_count);
	addUInt(m_cores_per_cpu);
	addUInt(m_opencl_capable_devices);
	addUInt(m_cuda_capable_devices);
	addUInt(m_hdd_count);
	addUInt(m_desired_node_id);
	addUInt(m_node_id);
	addUInt(m_system_capability_count);
	addULongLong(m_system_ram);

}

void ExtendedNetworkNode::deserialize()
{

	m_system_ram = getULongLong();
	m_system_capability_count = getUInt();
	m_node_id = getUInt();
	m_desired_node_id = getUInt();
	m_hdd_count = getUInt();
	m_cuda_capable_devices = getUInt();
	m_opencl_capable_devices = getUInt();
	m_cores_per_cpu = getUInt();
	m_physical_cpu_count = getUInt();
	m_architecture = getString();
	m_os_name = getString();
	m_node_name = getString();
	m_shu_deployment_path = getString();
	m_system_db_file = getString();
	m_node_ip = getString();

	DELETE_ARRAY_NULL_CHECKING(mp_hdd_devs);
	DELETE_ARRAY_NULL_CHECKING(mp_ocl_platform_indices);
	DELETE_ARRAY_NULL_CHECKING(mp_ocl_device_indices);
	DELETE_ARRAY_NULL_CHECKING(mp_ocl_types);
	DELETE_ARRAY_NULL_CHECKING(mp_cuda_types);
	DELETE_ARRAY_NULL_CHECKING(mp_system_capabilities);

	mp_hdd_devs = new std::string[m_hdd_count];
	mp_ocl_platform_indices = new unsigned int[m_opencl_capable_devices];
	mp_ocl_device_indices = new unsigned int[m_opencl_capable_devices];
	mp_ocl_types = new std::string[m_opencl_capable_devices];
	mp_cuda_types = new std::string[m_cuda_capable_devices];
	mp_system_capabilities = new std::string[m_system_capability_count];

	for(unsigned int i=0;i<m_cuda_capable_devices;++i)
	{
		mp_cuda_types[i] = getString();
	}

	for(unsigned int i=0;i<m_opencl_capable_devices;++i)
	{
		mp_ocl_device_indices[i] = getUInt();
		mp_ocl_platform_indices[i] = getUInt();
	}

	for(unsigned int i=0;i<m_opencl_capable_devices;++i)
	{
		mp_ocl_types[i] = getString();
	}

	for(unsigned int i=0;i<m_hdd_count;++i)
	{
		mp_hdd_devs[i] = getString();
	}

	for(unsigned int i=0;i<m_system_capability_count;++i)
	{
		mp_system_capabilities[i] = getString();
	}

	//free the serialization buffer
	DELETE_NULL_CHECKING(mp_serialized_data_buffer);

}



void ExtendedNetworkNode::initializeForCurrentSystem(const std::string& config_file)
{
	//read the local system specs
	m_system_ram = 0;
	//printf("system ram = %ld\n",m_system_ram);

	//if a config file has been given: fill out all other data fields
	if(config_file.size()!=0)
	{
		//read the params
		Lazarus::ConfigReaderMarkI conf_reader(config_file);
		conf_reader.readAllSections();

		//iterate over the sections
		Lazarus::FastNCSList<Lazarus::XMLConfigMarkISection*>* sections = conf_reader.getSections();
		for(Lazarus::FastNCSList<Lazarus::XMLConfigMarkISection*>::Iterator it = sections->getStart(); it != sections->getEnd(); ++it)
		{
			std::string section_type = it.getElement()->getm_data()->getName();

			//Get only the system information
			if(section_type.compare("System") == 0)
			{

				m_system_db_file = EXTNETNODE_STRING_CAST(SystemDB);
				m_shu_deployment_path = EXTNETNODE_STRING_CAST(SHUDeploymentPath);
				m_node_name = EXTNETNODE_STRING_CAST(CNMClientName);
				m_os_name = EXTNETNODE_STRING_CAST(OSName);
				m_architecture = EXTNETNODE_STRING_CAST(Architecture);
				m_physical_cpu_count = EXTNETNODE_UINT_CAST(PhysicalCPUCount);
				m_cores_per_cpu = EXTNETNODE_UINT_CAST(CoresPerCPU);
				m_desired_node_id = EXTNETNODE_UINT_CAST(DesiredNodeID);
				m_hdd_count = EXTNETNODE_UINT_CAST(HDDCount);//amount of hdd devices
				m_opencl_capable_devices = EXTNETNODE_UINT_CAST(OCLDevCount);
				m_cuda_capable_devices = EXTNETNODE_UINT_CAST(CUDADevCount);

				//get list of hdd device names
				unsigned int counter = 0;
				Lazarus::FastNCSList<std::string>* strings = EXTNETNODE_STRINGARRAY_CAST(HDDDevs)->getParam();
				mp_hdd_devs = new std::string[ strings->getSize() ];
				for(Lazarus::FastNCSList<std::string>::Iterator it = strings->getStart(); it != strings->getEnd(); ++it)
				{
					mp_hdd_devs[counter] = it.getElement()->getm_data();
					++counter;
				}
				counter = 0;

				//get list of OCL device types
				strings = EXTNETNODE_STRINGARRAY_CAST(OCLTypes)->getParam();
				mp_ocl_types = new std::string[ strings->getSize() ];
				for(Lazarus::FastNCSList<std::string>::Iterator it = strings->getStart(); it != strings->getEnd(); ++it)
				{
					mp_ocl_types[counter] = it.getElement()->getm_data();
					++counter;
				}
				counter = 0;

				//get list of system capabilities
				strings = EXTNETNODE_STRINGARRAY_CAST(SystemCapabilities)->getParam();
				m_system_capability_count = strings->getSize();
				mp_system_capabilities = new std::string[ strings->getSize() ];
				for(Lazarus::FastNCSList<std::string>::Iterator it = strings->getStart(); it != strings->getEnd(); ++it)
				{
					mp_system_capabilities[counter] = it.getElement()->getm_data();
					++counter;
				}
				counter = 0;

				//get list of CUDA device types
				strings = EXTNETNODE_STRINGARRAY_CAST(CUDATypes)->getParam();
				mp_cuda_types = new std::string[ strings->getSize() ];
				for(Lazarus::FastNCSList<std::string>::Iterator it = strings->getStart(); it != strings->getEnd(); ++it)
				{
					mp_cuda_types[counter] = it.getElement()->getm_data();
					++counter;
				}
				counter = 0;

				//get list of OCL platform indices
				unsigned int* int_array = EXTNETNODE_UINTARRAY_CAST(CLPlatforms)->getParam();
				mp_ocl_platform_indices = new unsigned int[ EXTNETNODE_UINTARRAY_CAST(CLPlatforms)->getSize() ];
				for(unsigned int i=0;i<EXTNETNODE_UINTARRAY_CAST(CLPlatforms)->getSize();++i)
				{
					mp_ocl_platform_indices[i] = int_array[i];
				}

				//get list of OCL device indices
				int_array = EXTNETNODE_UINTARRAY_CAST(CLDevices)->getParam();
				mp_ocl_device_indices = new unsigned int[ EXTNETNODE_UINTARRAY_CAST(CLDevices)->getSize() ];
				for(unsigned int i=0;i<EXTNETNODE_UINTARRAY_CAST(CLDevices)->getSize();++i)
				{
					mp_ocl_device_indices[i] = int_array[i];
				}

			}
		}
	}
}



} /* namespace Lazarus */
