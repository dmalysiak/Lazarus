/*
 * SystemStatus.h
 *
 *  Created on: 27.06.2013
 *      Author: darius
 */

#ifndef SYSTEMSTATUS_H_
#define SYSTEMSTATUS_H_

#include "../Serializable.h"

namespace Lazarus {

/**
 * A short summary of the systems current CPU loads.
 * This class can handle systems with at most 99 CPUs.
 * Requires a kernel >= 2.6.33
 */

class SystemStatus: public Serializable {
public:
	SystemStatus(bool update = true);
	virtual ~SystemStatus();

	virtual void serialize();
	virtual void deserialize();

	void getSystemStatus();

	void printStatus() const;
	void printShortStatus() const;

	unsigned int get_m_cpu_count() const;
	double get_m_cpu_load(unsigned int index) const;
	double get_m_total_cpu_load() const;
	double get_m_total_system_load() const;

	unsigned long getBufferram() const;
	unsigned long getFreehigh() const;
	unsigned long getFreeram() const;
	unsigned long getFreeswap() const;
	const unsigned long *getLoads() const;
	unsigned int getMemUnit() const;
	unsigned short getProcs() const;
	unsigned long getSharedram() const;
	unsigned long getTotalhigh() const;
	unsigned long getTotalram() const;
	unsigned long getTotalswap() const;
	long getUptime() const;

	const unsigned int period_ms = 100;

	/*
	 * this function returns the current total jiffie count of /proc/stat in form of 10 ulong values.
	 * In case of failure it returns NULL.
	 */
	static unsigned long* getCurrentJiffies();

private:
	unsigned int m_cpu_count;
	double m_total_system_load;
	double m_total_cpu_load;
	double* mp_cpu_load;

	//vars for /proc/stat
	unsigned long* mp_user1;
	unsigned long* mp_nice1;
	unsigned long* mp_system1;
	unsigned long* mp_idle1;
	unsigned long* mp_iowait1;
	unsigned long* mp_irq1;
	unsigned long* mp_softirq1;
	unsigned long* mp_steal1;
	unsigned long* mp_guest1;
	unsigned long* mp_guest_nice1;

	unsigned long* mp_user2;
	unsigned long* mp_nice2;
	unsigned long* mp_system2;
	unsigned long* mp_idle2;
	unsigned long* mp_iowait2;
	unsigned long* mp_irq2;
	unsigned long* mp_softirq2;
	unsigned long* mp_steal2;
	unsigned long* mp_guest2;
	unsigned long* mp_guest_nice2;


	long m_uptime;             /* Seconds since boot */
    unsigned long m_loads[3];  /* 1, 5, and 15 minute load averages */
    unsigned long m_totalram;  /* Total usable main memory size */
    unsigned long m_freeram;   /* Available memory size */
    unsigned long m_sharedram; /* Amount of shared memory */
    unsigned long m_bufferram; /* Memory used by buffers */
    unsigned long m_totalswap; /* Total swap space size */
    unsigned long m_freeswap;  /* Swap space still available */
    unsigned short m_procs;    /* Number of current processes */
    unsigned long m_totalhigh; /* Total high memory size */
    unsigned long m_freehigh;  /* Available high memory size */
    unsigned int m_mem_unit;   /* Memory unit size in bytes */

	void fillFirstTimeSet(char* proc_buffer);
	void fillSecondTimeSet(char* proc_buffer);
	void calcLoads();
};

} /* namespace Lazarus */
#endif /* SYSTEMSTATUS_H_ */
