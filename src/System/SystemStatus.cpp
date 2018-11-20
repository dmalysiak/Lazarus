/*
 * SystemStatus.cpp
 *
 *  Created on: 27.06.2013
 *      Author: darius
 */

#include "SystemStatus.h"

#include "../Toolbox.h"

#include <string.h>
#include <stdio.h>

#include <sys/sysinfo.h>

namespace Lazarus {


SystemStatus::SystemStatus(bool update) {

	m_cpu_count = 0;

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

		m_cpu_count++;
		match++;
	}

	mp_cpu_load = new double[m_cpu_count];
	m_total_system_load = 0;
	m_total_cpu_load = 0;


	mp_user1 = new unsigned long[m_cpu_count];
	mp_nice1 = new unsigned long[m_cpu_count];
	mp_system1 = new unsigned long[m_cpu_count];
	mp_idle1 = new unsigned long[m_cpu_count];
	mp_iowait1 = new unsigned long[m_cpu_count];
	mp_irq1 = new unsigned long[m_cpu_count];
	mp_softirq1 = new unsigned long[m_cpu_count];
	mp_steal1 = new unsigned long[m_cpu_count];
	mp_guest1 = new unsigned long[m_cpu_count];
	mp_guest_nice1 = new unsigned long[m_cpu_count];

	mp_user2 = new unsigned long[m_cpu_count];
	mp_nice2 = new unsigned long[m_cpu_count];
	mp_system2 = new unsigned long[m_cpu_count];
	mp_idle2 = new unsigned long[m_cpu_count];
	mp_iowait2 = new unsigned long[m_cpu_count];
	mp_irq2 = new unsigned long[m_cpu_count];
	mp_softirq2 = new unsigned long[m_cpu_count];
	mp_steal2 = new unsigned long[m_cpu_count];
	mp_guest2 = new unsigned long[m_cpu_count];
	mp_guest_nice2 = new unsigned long[m_cpu_count];

	if(update==true)
	{
		getSystemStatus();
	}
	else
	{
		m_uptime=0;
		m_loads[0]=0;
		m_loads[1]=0;
		m_loads[2]=0;
		m_totalram=0;
		m_freeram=0;
		m_sharedram=0;
		m_bufferram=0;
		m_totalswap=0;
		m_freeswap=0;
		m_procs=0;
		m_totalhigh=0;
		m_freehigh=0;
		m_mem_unit=0;
	}

}

SystemStatus::~SystemStatus() {

	DELETE_ARRAY_NULL_CHECKING(mp_cpu_load);

	DELETE_ARRAY_NULL_CHECKING(mp_user1);
	DELETE_ARRAY_NULL_CHECKING(mp_nice1);
	DELETE_ARRAY_NULL_CHECKING(mp_system1);
	DELETE_ARRAY_NULL_CHECKING(mp_idle1);
	DELETE_ARRAY_NULL_CHECKING(mp_iowait1);
	DELETE_ARRAY_NULL_CHECKING(mp_irq1);
	DELETE_ARRAY_NULL_CHECKING(mp_softirq1);
	DELETE_ARRAY_NULL_CHECKING(mp_steal1);
	DELETE_ARRAY_NULL_CHECKING(mp_guest1);
	DELETE_ARRAY_NULL_CHECKING(mp_guest_nice1);

	DELETE_ARRAY_NULL_CHECKING(mp_user2);
	DELETE_ARRAY_NULL_CHECKING(mp_nice2);
	DELETE_ARRAY_NULL_CHECKING(mp_system2);
	DELETE_ARRAY_NULL_CHECKING(mp_idle2);
	DELETE_ARRAY_NULL_CHECKING(mp_iowait2);
	DELETE_ARRAY_NULL_CHECKING(mp_irq2);
	DELETE_ARRAY_NULL_CHECKING(mp_softirq2);
	DELETE_ARRAY_NULL_CHECKING(mp_steal2);
	DELETE_ARRAY_NULL_CHECKING(mp_guest2);
	DELETE_ARRAY_NULL_CHECKING(mp_guest_nice2);

}

void SystemStatus::printStatus() const
{
	for(unsigned int i=0;i<m_cpu_count;i++)
	{
		printf("*********************** CPU times for cpu %u ************************\n",i);
		printf("mp_user1 %lu mp_nice1 %lu mp_system1 %lu mp_idle1 %lu mp_iowait1 %lu mp_irq1 %lu mp_softirq1 %lu mp_steal1 %lu mp_guest1 %lu mp_guest_nice1 %lu\n",
				mp_user1[i], mp_nice1[i], mp_system1[i], mp_idle1[i], mp_iowait1[i], mp_irq1[i], mp_softirq1[i], mp_steal1[i], mp_guest1[i], mp_guest_nice1[i]);

		printf("mp_user2 %lu mp_nice2 %lu mp_system2 %lu mp_idle2 %lu mp_iowait2 %lu mp_irq2 %lu mp_softirq2 %lu mp_steal2 %lu mp_guest2 %lu mp_guest_nice2 %lu\n",
				mp_user2[i], mp_nice2[i], mp_system2[i], mp_idle2[i], mp_iowait2[i], mp_irq2[i], mp_softirq2[i], mp_steal1[i], mp_guest1[i], mp_guest_nice1[i]);

		printf("CPU load %F\n",mp_cpu_load[i]);
	}

	printf("Total CPU load %F\n",m_total_cpu_load);

	printf("*********************** System attributes ************************\n");
	printf("%ld Seconds since boot \n"
					 "(%lu,%lu,%lu) 1, 5, and 15 minute load averages \n"
					 "%lu Total usable main memory size \n"
					 "%lu Available memory size \n"
					 "%lu Amount of shared memory \n"
					 "%lu Memory used by buffers \n"
					 "%lu Total swap space size \n"
					 "%lu Swap space still available \n"
					 "%hu Number of current processes \n"
					 "%lu Total high memory size \n"
					 "%lu Available high memory size \n"
					 "%u Memory unit size in bytes \n",
					m_uptime,             /* Seconds since boot */
					m_loads[0],m_loads[1],m_loads[2],  /* 1, 5, and 15 minute load averages */
					m_totalram,  /* Total usable main memory size */
					m_freeram,   /* Available memory size */
					m_sharedram, /* Amount of shared memory */
					m_bufferram, /* Memory used by buffers */
					m_totalswap, /* Total swap space size */
					m_freeswap,  /* Swap space still available */
					m_procs,    /* Number of current processes */
					m_totalhigh, /* Total high memory size */
					m_freehigh,  /* Available high memory size */
					m_mem_unit   /* Memory unit size in bytes */
					);
}

void SystemStatus::printShortStatus() const
{
	for(unsigned int i=0;i<m_cpu_count;i++)
	{
		printf("CPU[%u] load %F | ",i,mp_cpu_load[i]);
	}

	printf("\nTotal CPU load %F\n",m_total_cpu_load);

	printf("*********************** System attributes ************************\n");
	printf("%ld Seconds since boot \n"
					 "( %lu, %lu, %lu ) 1, 5, and 15 minute load averages \n"
					 "%lu Total usable main memory size \n"
					 "%lu Available memory size \n"
					 "%lu Amount of shared memory \n"
					 "%lu Memory used by buffers \n"
					 "%lu Total swap space size \n"
					 "%lu Swap space still available \n"
					 "%u Number of current processes \n"
					 "%lu Total high memory size \n"
					 "%lu Available high memory size \n"
					 "%u Memory unit size in bytes \n",
					m_uptime,             /* Seconds since boot */
					m_loads[0],m_loads[1],m_loads[2],  /* 1, 5, and 15 minute load averages */
					m_totalram,  /* Total usable main memory size */
					m_freeram,   /* Available memory size */
					m_sharedram, /* Amount of shared memory */
					m_bufferram, /* Memory used by buffers */
					m_totalswap, /* Total swap space size */
					m_freeswap,  /* Swap space still available */
					m_procs,    /* Number of current processes */
					m_totalhigh, /* Total high memory size */
					m_freehigh,  /* Available high memory size */
					m_mem_unit   /* Memory unit size in bytes */ );
}

unsigned long* SystemStatus::getCurrentJiffies()
{
	unsigned long* jiffies = new unsigned long[10];

	//----------------------- read /proc/stat
	FILE* fp;
	char buffer[32000];
	size_t bytes_read;

	fp = fopen("/proc/stat", "r");
	bytes_read = fread(buffer, 1, sizeof (buffer), fp);
	fclose(fp);

	/* Bail if read failed or if buffer is not big enough. */
	if(bytes_read == 0 || bytes_read == sizeof(buffer))
	{
		printf("ERROR: could not read the stat file\n");
		delete jiffies;
		return NULL;
	}

	/* NUL-terminate the text. */
	buffer[bytes_read] = '\0';

	//----------------------- extract the values
	char cpuname[500];
	char* match;

	char cpuname_ext[] = " %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu";

	//create the string for each cpu (i.e. first line beginning with 'cpu')
	sprintf(cpuname,"cpu");

	match = strstr(buffer, cpuname);

	if(match == NULL)
	{
		printf("ERROR: could not locate CPU line\n");
		delete jiffies;
		return NULL;
	}

	//extend the cpu string (*)
	memcpy(cpuname+3,cpuname_ext,strlen(cpuname_ext)+1);//append the ext-string incl. the 0 termination

	//printf("%s\n",cpuname);
	/* Parse the line to extract the cpu time values. */
	sscanf(match, cpuname, jiffies,jiffies+1,jiffies+2,jiffies+3,jiffies+4,jiffies+5,jiffies+6,jiffies+7,jiffies+8,jiffies+9);


	return jiffies;
}

void SystemStatus::getSystemStatus()
{

	FILE* fp;
	char buffer[32000];
	size_t bytes_read;

	fp = fopen("/proc/stat", "r");
	bytes_read = fread(buffer, 1, sizeof (buffer), fp);
	fclose(fp);

	/* Bail if read failed or if buffer is not big enough. */
	if(bytes_read == 0 || bytes_read == sizeof(buffer))
	{
		printf("ERROR: could not read the stat file\n");
		return;
	}

	/* NUL-terminate the text. */
	buffer[bytes_read] = '\0';

	//get the first set of cpu times
	fillFirstTimeSet(buffer);

	//wait some time and get the next set
	Toolbox::sleep_ms(period_ms);

	fp = fopen("/proc/stat", "r");
	bytes_read = fread(buffer, 1, sizeof (buffer), fp);
	fclose(fp);

	/* Bail if read failed or if buffer is not big enough. */
	if(bytes_read == 0 || bytes_read == sizeof(buffer))
	{
		printf("ERROR: could not read the stat file\n");
		return;
	}

	/* NUL-terminate the text. */
	buffer[bytes_read] = '\0';

	//get the second set of cpu times
	fillSecondTimeSet(buffer);

	//calculate the cpu loads
	calcLoads();

	//------------------------ misc system specs
	struct sysinfo info;
	sysinfo(&info);

	m_uptime = info.uptime;             /* Seconds since boot */
	m_loads[0] = info.loads[0];  /* 1, 5, and 15 minute load averages */
	m_loads[1] = info.loads[1];  /* 1, 5, and 15 minute load averages */
	m_loads[2] = info.loads[2];  /* 1, 5, and 15 minute load averages */
	m_totalram = info.totalram;  /* Total usable main memory size */
	m_freeram = info.freeram;   /* Available memory size */
	m_sharedram = info.sharedram; /* Amount of shared memory */
	m_bufferram = info.bufferram; /* Memory used by buffers */
	m_totalswap = info.totalswap; /* Total swap space size */
	m_freeswap = info.freeswap;  /* Swap space still available */
	m_procs = info.procs;    /* Number of current processes */
	m_totalhigh = info.totalhigh; /* Total high memory size */
	m_freehigh = info.freehigh;  /* Available high memory size */
	m_mem_unit = info.mem_unit;   /* Memory unit size in bytes */
}

void SystemStatus::fillFirstTimeSet(char* proc_buffer)
{
	//get the individual load
	char cpuname[500];
	char* match;

	//this works only for systems with a cpu count of at most two digits (see (*) below )!
	char cpuname_ext[] = " %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu";
	for(unsigned int i=0;i<m_cpu_count;i++)
	{
		//create the string for each cpu (i.e. cpuXX)
		sprintf(cpuname,"cpu%u",i);

		match = strstr(proc_buffer, cpuname);

		if(match == NULL)
		{
			printf("ERROR: could not locate CPU%u line\n",i);
			return;
		}

		//extend the cpu string (*)
		if(i<10)
		{
			memcpy(cpuname+4,cpuname_ext,strlen(cpuname_ext)+1);//append the ext-string incl. the 0 termination
		}
		else
		{
			memcpy(cpuname+5,cpuname_ext,strlen(cpuname_ext)+1);//append the ext-string incl. the 0 termination
		}
		//printf("%s\n",cpuname);
		/* Parse the line to extract the cpu time values. */
		sscanf(match, cpuname, mp_user1+i,mp_nice1+i,mp_system1+i,mp_idle1+i,mp_iowait1+i,mp_irq1+i,mp_softirq1+i,mp_steal1+i,mp_guest1+i,mp_guest_nice1+i);
	}
}

void SystemStatus::fillSecondTimeSet(char* proc_buffer)
{
	//get the individual load
	char cpuname[500];
	char* match;

	//this works only for systems with a cpu count of at most two digits!
	char cpuname_ext[] = " %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu";
	for(unsigned int i=0;i<m_cpu_count;i++)
	{
		//create the string for each cpu
		sprintf(cpuname,"cpu%u",i);

		match = strstr(proc_buffer, cpuname);

		if(match == NULL)
		{
			printf("ERROR: could not locate CPU%u line\n",i);
			return;
		}

		//extend the cpu string
		if(i<10)
		{
			memcpy(cpuname+4,cpuname_ext,strlen(cpuname_ext)+1);//append the ext-string incl. the 0 termination
		}
		else
		{
			memcpy(cpuname+5,cpuname_ext,strlen(cpuname_ext)+1);//append the ext-string incl. the 0 termination
		}

		/* Parse the line to extract the cpu time values. */
		sscanf(match, cpuname, mp_user2+i,mp_nice2+i,mp_system2+i,mp_idle2+i,mp_iowait2+i,mp_irq2+i,mp_softirq2+i,mp_steal2+i,mp_guest2+i,mp_guest_nice2+i);
	}
}

void SystemStatus::calcLoads()
{
	unsigned long total_jiffies1 = 0;
	unsigned long work_jiffies1 = 0;
	unsigned long total_jiffies2 = 0;
	unsigned long work_jiffies2 = 0;
	double temp=0;

	for(unsigned int i=0;i<m_cpu_count;i++)
	{
		total_jiffies1 = mp_user1[i]+mp_nice1[i]+mp_system1[i]+mp_idle1[i]+mp_iowait1[i]+mp_irq1[i]+mp_softirq1[i]+mp_steal1[i]+mp_guest1[i]+mp_guest_nice1[i];
		work_jiffies1 = mp_user1[i]+mp_nice1[i]+mp_system1[i];

		total_jiffies2 = mp_user2[i]+mp_nice2[i]+mp_system2[i]+mp_idle2[i]+mp_iowait2[i]+mp_irq2[i]+mp_softirq2[i]+mp_steal2[i]+mp_guest2[i]+mp_guest_nice2[i];
		work_jiffies2 = mp_user2[i]+mp_nice2[i]+mp_system2[i];

		mp_cpu_load[i] = (double) ( (long double)(work_jiffies2 - work_jiffies1) / (long double)(total_jiffies2 - total_jiffies1) * (long double)100.0 );
		temp += mp_cpu_load[i];
	}

	m_total_cpu_load = temp/(double)m_cpu_count;
}

void SystemStatus::serialize()
{
	DELETE_NULL_CHECKING(mp_serialized_data_buffer);
	resetSerializationOffset();

	unsigned int size = sizeof(unsigned long)*(20*m_cpu_count) + sizeof(double)*(m_cpu_count+2);

	size += sizeof(unsigned long)*11 + sizeof(long) + sizeof(unsigned short) + 2*sizeof(unsigned int);

	mp_serialized_data_buffer = new Buffer(size);

	for(unsigned int i=0;i<m_cpu_count;i++)
	{

		addULong(*(mp_user1+i));
		addULong(*(mp_nice1+i));
		addULong(*(mp_system1+i));
		addULong(*(mp_idle1+i));
		addULong(*(mp_iowait1+i));
		addULong(*(mp_irq1+i));
		addULong(*(mp_softirq1+i));
		addULong(*(mp_steal1+i));
		addULong(*(mp_guest1+i));
		addULong(*(mp_guest_nice1+i));

		addULong(*(mp_user2+i));
		addULong(*(mp_nice2+i));
		addULong(*(mp_system2+i));
		addULong(*(mp_idle2+i));
		addULong(*(mp_iowait2+i));
		addULong(*(mp_irq2+i));
		addULong(*(mp_softirq2+i));
		addULong(*(mp_steal2+i));
		addULong(*(mp_guest2+i));
		addULong(*(mp_guest_nice2+i));

		addDouble(*(mp_cpu_load+i));

	}

	addDouble(m_total_system_load);
	addDouble(m_total_cpu_load);
	addUInt(m_cpu_count);

	addLong(m_uptime);
	addULong(m_loads[0]);
	addULong(m_loads[1]);
	addULong(m_loads[2]);
	addULong(m_totalram);
	addULong(m_freeram);
	addULong(m_sharedram);
	addULong(m_bufferram);
	addULong(m_totalswap);
	addULong(m_freeswap);
	addUShort(m_procs);
	addULong(m_totalhigh);
	addULong(m_freehigh);
	addUInt(m_mem_unit);

}

void SystemStatus::deserialize()
{

	m_mem_unit=getUInt();
	m_freehigh=getULong();
	m_totalhigh=getULong();
	m_procs=getUShort();
	m_freeswap=getULong();
	m_totalswap=getULong();
	m_bufferram=getULong();
	m_sharedram=getULong();
	m_freeram=getULong();
	m_totalram=getULong();
	m_loads[2]=getULong();
	m_loads[1]=getULong();
	m_loads[0]=getULong();
	m_uptime=getLong();

	m_cpu_count = getUInt();
	m_total_cpu_load = getDouble();
	m_total_system_load =getDouble();

	//free any old memory blocks
	DELETE_ARRAY_NULL_CHECKING(mp_cpu_load);

	DELETE_ARRAY_NULL_CHECKING(mp_user1);
	DELETE_ARRAY_NULL_CHECKING(mp_nice1);
	DELETE_ARRAY_NULL_CHECKING(mp_system1);
	DELETE_ARRAY_NULL_CHECKING(mp_idle1);
	DELETE_ARRAY_NULL_CHECKING(mp_iowait1);
	DELETE_ARRAY_NULL_CHECKING(mp_irq1);
	DELETE_ARRAY_NULL_CHECKING(mp_softirq1);
	DELETE_ARRAY_NULL_CHECKING(mp_steal1);
	DELETE_ARRAY_NULL_CHECKING(mp_guest1);
	DELETE_ARRAY_NULL_CHECKING(mp_guest_nice1);

	DELETE_ARRAY_NULL_CHECKING(mp_user2);
	DELETE_ARRAY_NULL_CHECKING(mp_nice2);
	DELETE_ARRAY_NULL_CHECKING(mp_system2);
	DELETE_ARRAY_NULL_CHECKING(mp_idle2);
	DELETE_ARRAY_NULL_CHECKING(mp_iowait2);
	DELETE_ARRAY_NULL_CHECKING(mp_irq2);
	DELETE_ARRAY_NULL_CHECKING(mp_softirq2);
	DELETE_ARRAY_NULL_CHECKING(mp_steal2);
	DELETE_ARRAY_NULL_CHECKING(mp_guest2);
	DELETE_ARRAY_NULL_CHECKING(mp_guest_nice2);

	//allocate new blocks
	mp_cpu_load = new double[m_cpu_count];

	mp_user1 = new unsigned long[m_cpu_count];
	mp_nice1 = new unsigned long[m_cpu_count];
	mp_system1 = new unsigned long[m_cpu_count];
	mp_idle1 = new unsigned long[m_cpu_count];
	mp_iowait1 = new unsigned long[m_cpu_count];
	mp_irq1 = new unsigned long[m_cpu_count];
	mp_softirq1 = new unsigned long[m_cpu_count];
	mp_steal1 = new unsigned long[m_cpu_count];
	mp_guest1 = new unsigned long[m_cpu_count];
	mp_guest_nice1 = new unsigned long[m_cpu_count];

	mp_user2 = new unsigned long[m_cpu_count];
	mp_nice2 = new unsigned long[m_cpu_count];
	mp_system2 = new unsigned long[m_cpu_count];
	mp_idle2 = new unsigned long[m_cpu_count];
	mp_iowait2 = new unsigned long[m_cpu_count];
	mp_irq2 = new unsigned long[m_cpu_count];
	mp_softirq2 = new unsigned long[m_cpu_count];
	mp_steal2 = new unsigned long[m_cpu_count];
	mp_guest2 = new unsigned long[m_cpu_count];
	mp_guest_nice2 = new unsigned long[m_cpu_count];


	for(unsigned int i=m_cpu_count;i>0;i--)
	{

		*(mp_cpu_load+i-1) = getDouble();

		*(mp_guest_nice2+i-1) = getULong();
		*(mp_guest2+i-1) = getULong();
		*(mp_steal2+i-1) = getULong();
		*(mp_softirq2+i-1) = getULong();
		*(mp_irq2+i-1) = getULong();
		*(mp_iowait2+i-1) = getULong();
		*(mp_idle2+i-1) = getULong();
		*(mp_system2+i-1) = getULong();
		*(mp_nice2+i-1) = getULong();
		*(mp_user2+i-1) = getULong();

		*(mp_guest_nice1+i-1) = getULong();
		*(mp_guest1+i-1) = getULong();
		*(mp_steal1+i-1) = getULong();
		*(mp_softirq1+i-1) = getULong();
		*(mp_irq1+i-1) = getULong();
		*(mp_iowait1+i-1) = getULong();
		*(mp_idle1+i-1) = getULong();
		*(mp_system1+i-1) = getULong();
		*(mp_nice1+i-1) = getULong();
		*(mp_user1+i-1) = getULong();

	}

	DELETE_NULL_CHECKING(mp_serialized_data_buffer);
	resetSerializationOffset();

}

unsigned int SystemStatus::get_m_cpu_count() const
{
	return m_cpu_count;
}

double SystemStatus::get_m_cpu_load(unsigned int index) const
{
	return mp_cpu_load[index];
}

double SystemStatus::get_m_total_system_load() const
{
	return m_total_system_load;
}

double SystemStatus::get_m_total_cpu_load() const
{
	return m_total_cpu_load;
}


unsigned long SystemStatus::getBufferram() const {
	return m_bufferram;
}

unsigned long SystemStatus::getFreehigh() const {
	return m_freehigh;
}

unsigned long SystemStatus::getFreeram() const {
	return m_freeram;
}

unsigned long SystemStatus::getFreeswap() const {
	return m_freeswap;
}

const unsigned long *SystemStatus::getLoads() const {
	return m_loads;
}

unsigned int SystemStatus::getMemUnit() const {
	return m_mem_unit;
}

unsigned short SystemStatus::getProcs() const {
	return m_procs;
}

unsigned long SystemStatus::getSharedram() const {
	return m_sharedram;
}

unsigned long SystemStatus::getTotalhigh() const {
	return m_totalhigh;
}

unsigned long SystemStatus::getTotalram() const {
	return m_totalram;
}

unsigned long SystemStatus::getTotalswap() const {
	return m_totalswap;
}

long SystemStatus::getUptime() const {
	return m_uptime;
}

} /* namespace Lazarus */
