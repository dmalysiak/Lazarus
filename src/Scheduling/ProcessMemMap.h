/*
 * ProcessMemMap.h
 *
 *  Created on: 16.11.2016
 *      Author: Darius Malysiak
 */

#ifndef CORE_SCHEDULING_PROCESSMEMMAP_H_
#define CORE_SCHEDULING_PROCESSMEMMAP_H_

#include "../BaseObject.h"
#include "../DataStructures/FastNCSList.h"
#include "../System/SystemUser.h"

namespace Lazarus {

class MemMapEntry: public BaseObject
{
public:
	MemMapEntry();

	unsigned long long m_address_start;//start within the process's address space
	unsigned long long m_address_end;//end within the process's address space
	std::string m_perms;//permissions for the region
	unsigned long long m_offset;//if the region was mapped from a file, this is the offset within the file
	std::string m_dev;//major and minor device number if mapped from a file
	unsigned long long m_inode;//inode on the device
	std::string m_pathname; //path to the mapped file
};

/**
 * Keep in mind that all numbers are determined in a non-simultaneous way, i.e. the underlying file may change
 * while being read. Thus handle all values with caution.
 * */
class ProcessMemMap: public BaseObject {
public:
	ProcessMemMap(pid_t pid);
	virtual ~ProcessMemMap();

	/**
	 * Will update the memory map for the process.
	 * */
	int updateMap();

	void printMemMappings();

	/**
	 * Returns the current memory maps for the process.
	 * */
	const Lazarus::FastNCSList<MemMapEntry>& getEntries();

private:
	pid_t m_pid;
	unsigned long long m_total_mapped;
	unsigned long long m_private_writable;
	unsigned long long m_shared;
	Lazarus::FastNCSList<MemMapEntry> m_maps;

    /**
     * The number of thread stacks for this process; compare this number with num_threads from the Process class,
     * if they differ one has lost a threads memory i.e. terminated a joinable thread which
     * has not been joined.
     * Keep in mind that both numbers are determined in a non-simultaneous way, thus one should use this approach with
     * caution especially for highly dynamic processes.
     * */
	unsigned int m_thread_stacks;
};

} /* namespace Lazarus */

#endif /* CORE_SCHEDULING_PROCESSMEMMAP_H_ */
