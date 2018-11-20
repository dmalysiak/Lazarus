/*
 * SystemData.cpp
 *
 *  Created on: 27.06.2013
 *      Author: darius
 */

#include "SystemInformation.h"

#include "../Toolbox.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

namespace Lazarus {

SystemInformation::SystemInformation() {

	m_system_cpu_count = 0;
	m_system_disk_count = 0;
	mp_system_cpus = NULL;
	mp_system_disks = NULL;
	mp_system_status = NULL;

	grabSystemInformation();
}

SystemInformation::SystemInformation(FastKTuple<std::string>* disks)
{
	m_system_cpu_count = 0;
	m_system_disk_count = 0;
	mp_system_cpus = NULL;
	mp_system_disks = NULL;
	mp_system_status = NULL;

	m_disk_files = *disks;

	grabSystemInformation();
}

SystemInformation::~SystemInformation() {

	//delete the CPU objects
	if(mp_system_cpus != NULL)
	{
		mp_system_cpus->eraseWithDestructor();
		DELETE_NULL_CHECKING(mp_system_cpus);
	}

	//delete the disk objects
	if(mp_system_disks != NULL)
	{
		mp_system_disks->eraseWithDestructor();
		DELETE_NULL_CHECKING(mp_system_disks);
	}

	//delete the system status
	DELETE_NULL_CHECKING(mp_system_status);

}

long SystemInformation::getPageSize()
{
	long out = sysconf(_SC_PAGESIZE);

	return out;
}

unsigned int SystemInformation::get_m_system_cpu_count() const
{
	return m_system_cpu_count;
}

unsigned int SystemInformation::get_m_system_disk_count() const
{
	return m_system_disk_count;
}

std::string SystemInformation::get_m_system_log() const
{
	return m_system_log;
}

FastKTuple<SystemCPU*>* SystemInformation::get_mp_system_cpus() const
{
	return mp_system_cpus;
}

FastKTuple<SystemDisk*>* SystemInformation::get_mp_system_disks() const
{
	return mp_system_disks;
}

SystemStatus* SystemInformation::get_mp_system_status() const
{
	return mp_system_status;
}


void SystemInformation::serialize()
{
	Serializable::resetBuffer();
	//serialize the cpu objects
	for(unsigned int i=0;i<mp_system_cpus->getm_size();i++)
	{
		mp_system_cpus->getElement(i)->serialize();

		registerUCharA(mp_system_cpus->getElement(i)->get_mp_serialized_data_buffer()->get_m_length());
	}

	//serialize the disk objects
	for(unsigned int i=0;i<mp_system_disks->getm_size();i++)
	{
		mp_system_disks->getElement(i)->serialize();

		registerUCharA(mp_system_disks->getElement(i)->get_mp_serialized_data_buffer()->get_m_length());
	}

	//serialize the status object
	mp_system_status->serialize();
	registerUCharA(mp_system_status->get_mp_serialized_data_buffer()->get_m_length());

	registerElement<unsigned int>(2);
	registerString(m_system_log);
	registerString(m_system_name);

	allocateBuffer();

	for(unsigned int i=0;i<mp_system_cpus->getm_size();i++)
	{
		addUCharA(mp_system_cpus->getElement( (mp_system_cpus->getm_size()-1)-i )->get_mp_serialized_data_buffer()->get_mp_data(),
				mp_system_cpus->getElement( (mp_system_cpus->getm_size()-1)-i )->get_mp_serialized_data_buffer()->get_m_length());
	}

	for(unsigned int i=0;i<mp_system_disks->getm_size();i++)
	{
		addUCharA(mp_system_disks->getElement( (mp_system_disks->getm_size()-1)-i )->get_mp_serialized_data_buffer()->get_mp_data(),
				mp_system_disks->getElement( (mp_system_disks->getm_size()-1)-i )->get_mp_serialized_data_buffer()->get_m_length());
	}

	addUCharA(mp_system_status->get_mp_serialized_data_buffer()->get_mp_data(),
			mp_system_status->get_mp_serialized_data_buffer()->get_m_length());

	addString(m_system_log);
	addString(m_system_name);
	addUInt(m_system_cpu_count);
	addUInt(m_system_disk_count);
}

void SystemInformation::deserialize()
{

	//delete the CPU objects
	if(mp_system_cpus != NULL)
	{
		mp_system_cpus->eraseWithDestructor();
		DELETE_NULL_CHECKING(mp_system_cpus);
	}

	//delete the disk objects
	if(mp_system_disks != NULL)
	{
		mp_system_disks->eraseWithDestructor();
		DELETE_NULL_CHECKING(mp_system_disks);
	}

	DELETE_NULL_CHECKING(mp_system_status);

	m_system_disk_count = getUInt();
	m_system_cpu_count = getUInt();

	m_system_name = getString();
	m_system_log = getString();

	unsigned char* data = NULL;
	unsigned long long size = 0;

	//create the system status
	data = getUCharA(size);
	mp_system_status = new SystemStatus(false);
	mp_system_status->set_mp_serialized_data_buffer(new Buffer(data,size));
	mp_system_status->deserialize();

	//create the disk array
	mp_system_disks = new FastKTuple<SystemDisk*>(m_system_disk_count);
	SystemDisk* disk = NULL;
	for(unsigned int i=0;i<m_system_disk_count;i++)
	{
		data = getUCharA(size);

		disk = new SystemDisk();
		disk->set_mp_serialized_data_buffer(new Buffer(data,size));
		disk->deserialize();

		mp_system_disks->setElement(i,disk);
	}

	//create the cpu array
	mp_system_cpus = new FastKTuple<SystemCPU*>(m_system_cpu_count);
	SystemCPU* cpu = NULL;
	for(unsigned int i=0;i<m_system_cpu_count;i++)
	{
		data = getUCharA(size);

		cpu = new SystemCPU();
		cpu->set_mp_serialized_data_buffer(new Buffer(data,size));
		cpu->deserialize();

		mp_system_cpus->setElement(i,cpu);
	}


	resetBuffer();
}

void SystemInformation::printSystemInformation() const
{
	printf("************* SYSTEM INFORMATION *********************\n");
	printf("System disk count: %u \n",m_system_disk_count);
	printf("System cpu count: %u \n",m_system_cpu_count);
	printf("System name %s\n",m_system_name.c_str());

	printf("\n\n");

	printf("+++++++++++++++ Printing CPU information:\n");
	for(unsigned int i=0;i<mp_system_cpus->getm_size();i++)
	{
		mp_system_cpus->getElement(i)->printCPUInfo();
		printf("\n");
	}

	printf("\n\n");

	printf("+++++++++++++++ Printing disk information:\n");
	for(unsigned int i=0;i<mp_system_disks->getm_size();i++)
	{
		mp_system_disks->getElement(i)->printInfo();
		printf("\n");
	}

	printf("\n\n");

	printf("+++++++++++++++ Printing system log\n");


	if(m_system_log.size() < 50000)
	{
		printf("System log %s\n",m_system_log.c_str());
	}
	else //print only the last 5000 lines
	{
		printf("Truncated system log %s\n",m_system_log.substr( m_system_log.size() - 50000 ).c_str());
	}

	printf("\n\n");

	printf("+++++++++++++++ Printing system attributes\n");
	mp_system_status->printStatus();
}

void SystemInformation::grabSystemInformation()
{
	m_system_cpu_count = 0;

	//delete the CPU objects
	if(mp_system_cpus != NULL)
	{
		mp_system_cpus->eraseWithDestructor();
		DELETE_NULL_CHECKING(mp_system_cpus);
	}

	//delete the disk objects
	if(mp_system_disks != NULL)
	{
		mp_system_disks->eraseWithDestructor();
		DELETE_NULL_CHECKING(mp_system_disks);
	}

	DELETE_NULL_CHECKING(mp_system_status);

	//+++++++++++++++++++++++++ CPU
	//determine the number of cpus
	FILE* fp;
	char buffer[32000];
	size_t bytes_read;
	/* Read the entire contents of /proc/cpuinfo into the buffer. */
	fp = fopen("/proc/cpuinfo", "r");
	bytes_read = fread(buffer, 1, sizeof (buffer), fp);
	fclose (fp);

	/* Bail if read failed or if buffer is not big enough. */
	if(bytes_read == 0 || bytes_read == sizeof (buffer))
	{
		printf("ERROR: could not read the CPU information\n");
		return;
	}

	/* NUL-terminate the text. */
	buffer[bytes_read] = '\0';
	char* match = buffer;

	while(1)
	{
		/* Locate the line that starts with 'processor'. */
		//locate the line
		match = strstr(match, "processor");

		if (match == NULL)
		{
			//printf("ERROR: could not locate more processor lines\n");
			break;
		}

		m_system_cpu_count++;
		match++;
	}

	mp_system_cpus = new FastKTuple<SystemCPU*>(m_system_cpu_count);

	//grab the information for each cpu
	SystemCPU* temp = NULL;
	for(unsigned int i=0;i<m_system_cpu_count;i++)
	{
		temp = new SystemCPU(i);
		mp_system_cpus->setElement(i,temp);
	}

	//+++++++++++++++++++++++++++++ System log
	//create the log file
	system("journalctl -k > log.txt");

	fp = fopen("log.txt", "r");

	//get the filesize (as the log file may be very large, i.e. >10MByte, we only allocate as much buffer space as required)
	fseek(fp, 0L, SEEK_END);
	unsigned int actual_filesize = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	char* dynamic_buffer = new char[actual_filesize+1];

	bytes_read = fread(dynamic_buffer, 1, actual_filesize, fp);
	fclose(fp);

	//delete the log file
	system("rm log.txt");

	/* Bail if read failed or if buffer is not big enough. */
	if (bytes_read == 0)
	{
		printf("ERROR: could not read the log file\n");
		delete[] dynamic_buffer;
		return;
	}

	/* NUL-terminate the text. */
	dynamic_buffer[bytes_read] = '\0';

	m_system_log = std::string(dynamic_buffer);

	delete[] dynamic_buffer;

	//+++++++++++++++++++++++++++ System name
	fp = fopen ("/proc/version", "r");
	bytes_read = fread(buffer, 1, sizeof (buffer), fp);
	fclose(fp);

	/* Bail if read failed or if buffer is not big enough. */
	if(bytes_read == 0 || bytes_read == sizeof (buffer))
	{
		printf("ERROR: could not read the version file\n");
		return;
	}

	/* NUL-terminate the text. */
	buffer[bytes_read] = '\0';

	m_system_name = std::string(buffer);

	//++++++++++++++++++++++++++ Disks
	m_system_disk_count = m_disk_files.getm_size();
	mp_system_disks = new FastKTuple<SystemDisk*>(m_system_disk_count);

	for(unsigned int i=0;i<m_disk_files.getm_size();++i)
	{
		mp_system_disks->setElement(i,new SystemDisk(m_disk_files[i]));
	}

	//system status
	mp_system_status = new SystemStatus();
}


void SystemInformation::grabSystemInformationFast()
{
	//grab the information for each cpu
	for(unsigned int i=0;i<m_system_cpu_count;i++)
	{
		mp_system_cpus->getElement(i)->loadInfo();
	}

	//++++++++++++++++++++++++++ Disks
	m_system_disk_count = m_disk_files.getm_size();
	mp_system_disks = new FastKTuple<SystemDisk*>(m_system_disk_count);

	for(unsigned int i=0;i<m_disk_files.getm_size();++i)
	{
		mp_system_disks->setElement(i,new SystemDisk(m_disk_files[i]));
	}

	//system status
	mp_system_status->getSystemStatus();

}


const std::string& SystemInformation::getSystemLog() const {
	return m_system_log;
}

const std::string& SystemInformation::getSystemName() const {
	return m_system_name;
}

} /* namespace Lazarus */
