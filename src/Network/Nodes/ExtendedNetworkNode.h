/*
 * ExtendedNetworkNode.h
 *
 *  Created on: Aug 24, 2013
 *      Author: Darius Malysiak
 */

#ifndef EXTENDEDNETWORKNODE_H_
#define EXTENDEDNETWORKNODE_H_

#include "../NetworkNode.h"

namespace Lazarus {

class ExtendedNetworkNode: public Lazarus::NetworkNode {
public:
	ExtendedNetworkNode();
	//all copy constructors do deep copies of the object
	ExtendedNetworkNode(ExtendedNetworkNode* enn);
	ExtendedNetworkNode(const ExtendedNetworkNode& enn);
	virtual ~ExtendedNetworkNode();

	void initializeForCurrentSystem(const std::string& config_file);

	void serialize();
	void deserialize();

	void printNodeInfo();

	//system attributes (once set they will never change!)
	std::string m_node_name;
	std::string m_os_name;
	std::string m_architecture;
	std::string m_shu_deployment_path;
	std::string* mp_system_capabilities;//list of system capabilities
	unsigned int m_system_capability_count;
	unsigned int m_physical_cpu_count;
	unsigned int m_cores_per_cpu;
	unsigned long long m_system_ram;
	unsigned int m_hdd_count;//amount of hdd devices
	std::string* mp_hdd_devs;//list of device names
	unsigned int m_opencl_capable_devices;
	unsigned int m_cuda_capable_devices;
	unsigned int* mp_ocl_platform_indices;//first tuple element (platform,device) for each OCL device
	unsigned int* mp_ocl_device_indices;//second tuple element
	std::string* mp_ocl_types;//types of OCL devices
	std::string* mp_cuda_types;//types of CUDA devices
	std::string m_system_db_file;//the path to the system's SH database
	unsigned int m_desired_node_id;

};

} /* namespace Lazarus */
#endif /* EXTENDEDNETWORKNODE_H_ */
