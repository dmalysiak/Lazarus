/*
 * ProcessMemMap.cpp
 *
 *  Created on: 16.11.2016
 *      Author: Darius Malysiak
 */

#include "ProcessMemMap.h"

namespace Lazarus {

MemMapEntry::MemMapEntry()
{
	m_address_start = 0;//start within the process's address space
	m_address_end = 0;//end within the process's address space
	m_offset = 0;//if the region was mapped from a file, this is the offset within the file
	m_inode = 0;//inode on the device
}

ProcessMemMap::ProcessMemMap(pid_t pid) {

	m_pid = pid;
	m_total_mapped = 0;
	m_private_writable = 0;
	m_shared = 0;
	m_thread_stacks = 0;

	updateMap();
}

ProcessMemMap::~ProcessMemMap() {
	// TODO Auto-generated destructor stub
}

void ProcessMemMap::printMemMappings()
{
	printf("******************* Printing mapped memory regions for pid %d\n *****************",m_pid);
	for(auto it = m_maps.getStart(); it != m_maps.getEnd(); ++it)
	{
		printf("%llu - %llu %llu %llu %s %s %s\n",it.getElement()->getm_data_ref().m_address_start,
				it.getElement()->getm_data_ref().m_address_end,
				it.getElement()->getm_data_ref().m_offset,
				it.getElement()->getm_data_ref().m_inode,
				it.getElement()->getm_data_ref().m_pathname.c_str(),
				it.getElement()->getm_data_ref().m_perms.c_str(),
				it.getElement()->getm_data_ref().m_dev.c_str());

		printf("\n");
	}

	printf("Total shared memory %llu, total writable private memory %llu, total shared memory %llu\n",m_total_mapped,
	m_private_writable,
	m_shared);
}

int ProcessMemMap::updateMap()
{
	std::string dummy("[stack");
	m_thread_stacks = 0;

	//clear all entries
	m_maps.clear();

	//open the map file
	char filename[2048];
	sprintf(filename, "/proc/%lld/maps", (long long)m_pid);
	FILE* file = fopen(filename, "r");

	if(file == NULL)
	{
		printf("ERROR: could not open file\n");
		return -1;
	}

	char buff[8192];
	char perms[5];
	char dev[6];
	char path[2048];

	//traverse the file line by line
	while(feof(file) == 0)
	{
		//attempt to read a new line, break in case the eof flag is set
		if(fgets(buff, sizeof(buff), file) == NULL)
			break;

		MemMapEntry entry;

		path[0] = '\0';
		sscanf(buff, "%llx-%llx %4s %llx %5s %lld %s",&(entry.m_address_start), &(entry.m_address_end),
				perms, &(entry.m_offset), dev, &(entry.m_inode), path);

		entry.m_pathname = path;
		entry.m_perms = perms;
		entry.m_dev = dev;

		//count the thread stacks
		if(entry.m_pathname.compare(0, dummy.size(),dummy) == 0)
			m_thread_stacks++;

		m_total_mapped += entry.m_address_end- entry.m_address_start;
		if(perms[3]=='p' && perms[1] == 'w')
			m_private_writable += entry.m_address_end- entry.m_address_start;
		else if(perms[3]=='s')
			m_shared += entry.m_address_end- entry.m_address_start;

		m_maps.appendLast(entry);
	}


	return 0;
}

/**
 * Returns the current memory maps for the process.
 * */
const Lazarus::FastNCSList<MemMapEntry>& ProcessMemMap::getEntries()
{
	return m_maps;
}

} /* namespace Lazarus */
