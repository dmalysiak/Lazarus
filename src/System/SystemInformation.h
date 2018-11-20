/*
 * SystemData.h
 *
 *  Created on: 27.06.2013
 *      Author: darius
 */

#ifndef SYSTEMINFORMATION_H_
#define SYSTEMINFORMATION_H_

#include "../Serializable.h"
#include "../DataStructures/FastKTuple.h"

#include "SystemCPU.h"
#include "SystemDisk.h"
#include "SystemStatus.h"

#include <unistd.h>

namespace Lazarus {

/**
 * This class provides system summaries which contain: a list of CPUs, disks and available system RAM.
 * */

class SystemInformation: public Serializable {
public:
	/**
	 * Expects a list of device files e.g. /dev/sda, does not take ownership of the list!
	 * */
	SystemInformation(FastKTuple<std::string>* disks);

	/**
	 * Use this constructor if either no disks are required or the object should be populated with data
	 * later on via deserialization.
	 * */
	SystemInformation();
	virtual ~SystemInformation();

	/**
	 * This method will update the system attributes.
	 * Use this method only after a serious hardware change, e.g. memory hot plugging or change of the system name,
	 * since its complexity is quiet high.
	 * */
	void grabSystemInformation();

	/**
	 * This method will update the system attributes leaving out the system logs.
	 * */
	void grabSystemInformationFast();

	virtual void serialize();
	virtual void deserialize();

	void printSystemInformation() const;

	unsigned int get_m_system_cpu_count() const;
	unsigned int get_m_system_disk_count() const;


	const std::string& getSystemLog() const;
	const std::string& getSystemName() const;

	/**
	 * This string contains only the systemd log for all kernel messages
	 * */
	std::string get_m_system_log() const;

	/**
	 * Do not delete the returned list.
	 * */
	FastKTuple<SystemCPU*>* get_mp_system_cpus() const;

	/**
	 * Do not delete the returned list.
	 * */
	FastKTuple<SystemDisk*>* get_mp_system_disks() const;

	/**
	 * Do not delete the returned object.
	 * */
	SystemStatus* get_mp_system_status() const;

	static long getPageSize();

private:

	unsigned int m_system_cpu_count;
	unsigned int m_system_disk_count;
	std::string m_system_log;
	std::string m_system_name;
	FastKTuple<SystemCPU*>* mp_system_cpus;
	FastKTuple<SystemDisk*>* mp_system_disks;
	SystemStatus* mp_system_status;

	FastKTuple<std::string> m_disk_files;

};

} /* namespace Lazarus */
#endif /* SYSTEMINFORMATION_H_ */
