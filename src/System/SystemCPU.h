/*
 * SystemCPU.h
 *
 *  Created on: 27.06.2013
 *      Author: darius
 */

#ifndef SYSTEMCPU_H_
#define SYSTEMCPU_H_

#include "../Serializable.h"
#include "../DataStructures/FastKTuple.h"

#include <string>

/**
 * This class should work with all linux kernels >= 3.7.9
 */

namespace Lazarus {

/**
 * Contains information about the CPU cache.
 * */
class CPUCache: public Serializable {
public:
	CPUCache();
	~CPUCache();

	virtual void serialize();
	virtual void deserialize();

	//the cache line size
	unsigned long long m_coherency_line_size;
	//the cache level in the hierarchy
	unsigned int m_cache_level;
	//number of sets
	unsigned int m_number_of_sets;
	//byte stride of physical partition within a cache line
	unsigned long long m_physical_line_partition;
	//list of shared cpus
	std::string m_shared_cpus;
	//map of shared cpus
	std::string m_shared_cpu_map;
	//cache size
	unsigned long long m_cache_size;
	//cache type (description)
	std::string m_cache_type;
	//ways of associativity
	unsigned int m_ways_of_associativity;
};

class SystemCPU: public Serializable {
public:
	enum CPU_GOVERNOR{CPU_GOVERNOR_ONDEMAND /*Dynamically switch between CPU(s) available if at 95% cpu load*/,
		CPU_GOVERNOR_PERFORMANCE /*Run the cpu at max frequency */,
		CPU_GOVERNOR_CONSERVATIVE /*Dynamically switch between CPU(s) available if at 75% load */,
		CPU_GOVERNOR_POWERSAVE /* Run the cpu at the minimum frequency */,
        CPU_GOVERNOR_UNKNOWN /* DUMMY */,
		CPU_GOVERNOR_USERSPACE /*Run the cpu at user specified frequencies */};

	enum CPU_POWERDRIVER{
		CPU_POWERDRIVER_INTEL_PSTATE, /*This driver implements a scaling driver with an internal governor for Intel Core (Sandy Bridge and newer) processors.*/
		CPU_POWERDRIVER_ACPI_CPUFREQ, /*CPUFreq driver which utilizes the ACPI Processor Performance States. This driver also supports the Intel Enhanced SpeedStep (previously supported by the deprecated speedstep-centrino module).*/
		CPU_POWERDRIVER_SPEEDSTEP_LIB, /*CPUFreq driver for Intel SpeedStep-enabled processors (mostly Atoms and older Pentiums (< 3))*/
		CPU_POWERDRIVER_POWERNOW_K8, /*CPUFreq driver for K8/K10 Athlon 64/Opteron/Phenom processors. Since Linux 3.7 'acpi-cpufreq' will automatically be used for more modern CPUs from this family.*/
		CPU_POWERDRIVER_PCC_CPUFREQ, /*This driver supports Processor Clocking Control interface by Hewlett-Packard and Microsoft Corporation which is useful on some ProLiant servers.*/
		CPU_POWERDRIVER_P4_CLOCKMOD, /*CPUFreq driver for Intel Pentium 4/Xeon/Celeron processors which lowers the CPU temperature by skipping clocks. (You probably want to use a SpeedStep driver instead.) */
		CPU_POWERDRIVER_UNKNOWN /* DUMMY */
	};

	SystemCPU(unsigned int cpu_num);
	SystemCPU();
	virtual ~SystemCPU();

	/**
	 * This method will parse the file /proc/cpuinfo for data regarding the CPU indexed by m_cpu_num.
	 * Additionally it will utilize the cpupower lib in order to get information about the cpus power states.
	 * Futhermore it will also read '/sys/devices/system/cpu/cpu<cpu_num>/cache/index*' files in order to obtain
	 * information about the CPUs caches.
	 * In order to gain maximal control over the CPU disable p_states for Intel processors via 'intel_pstate=disable'
	 * as boot parameter, load 'modprobe cpufreq_userspace' and set 'cpupower frequency-set --governor userspace',
	 * as an alternative one can use this class instead of the last step.
	 * */
	void loadInfo();

	void printCPUInfo();

	int setFrequency(unsigned long long freq);
	int setGovernor(enum CPU_GOVERNOR gov, unsigned long long min_freq,unsigned long long max_freq);

	/**
	 * Will use the HW limits for frequency values.
	 * */
	int setGovernor(enum CPU_GOVERNOR gov);

	virtual void serialize();
	virtual void deserialize();

	unsigned int get_m_processor() const;
	std::string get_m_vendor_id() const;
	unsigned int get_m_cpu_family() const;
	unsigned int get_m_model() const;
	std::string get_m_model_name() const;
	unsigned int get_m_stepping() const;
	std::string get_m_microcode() const;
	double get_m_cpu_mhz() const;
	unsigned int get_m_cache_size() const;
	unsigned int get_m_physical_id() const;
	unsigned int get_m_siblings() const;
	unsigned int get_m_core_id() const;
	unsigned int get_m_cpu_cores() const;
	unsigned int get_m_apicid() const;
	unsigned int get_m_initial_apicid() const;
	std::string get_m_fpu() const;
	std::string get_m_fpu_exception() const;
	unsigned int get_m_cpuid_level() const;
	std::string get_m_wp() const;
	std::string get_m_flags() const;
	double get_m_bogomips() const;
	unsigned int get_m_clflush_size() const;
	unsigned int get_m_cache_alignment() const;
	std::string get_m_address_sizes() const;
	std::string get_m_power_management() const;

	const FastKTuple<unsigned int>& getAffectedCpus() const;
	const FastKTuple<unsigned long long>& getAvailableFreqs() const;
	const FastKTuple<enum CPU_GOVERNOR>& getAvailableGovernors() const;
	unsigned long long getCurrentFrequency() const ;
	enum CPU_GOVERNOR getCurrentGovernor() const;
	enum CPU_POWERDRIVER getDriver() const;
	unsigned long getCurrentGovernorMaximalFrequency() const;
	unsigned long getCurrentGovernorMinimalFrequency() const;
	unsigned long getMaximalFrequency() const;
	unsigned long getMinimalFrequency() const;
	const FastKTuple<unsigned int>& getRelatedCpus() const;
	unsigned long long getStateTransitionCount() const;
	unsigned long long getTimeInCurrentState() const;
	unsigned long long getTotalCpuTime() const;
	unsigned long getTransitionLatencyNs() const;

private:
	unsigned int m_processor;
	std::string m_vendor_id;
	unsigned int m_cpu_family;
	unsigned int m_model;
	std::string m_model_name;
	unsigned int m_stepping;
	std::string m_microcode;
	float m_cpu_mhz;
	unsigned int m_cache_size;
	unsigned int m_physical_id;
	unsigned int m_siblings;
	unsigned int m_core_id;
	unsigned int m_cpu_cores;
	unsigned int m_apicid;
	unsigned int m_initial_apicid;
	std::string m_fpu;
	std::string m_fpu_exception;
	unsigned int m_cpuid_level;
	std::string m_wp;
	std::string m_flags;
	float m_bogomips;
	unsigned int m_clflush_size;
	unsigned int m_cache_alignment;
	std::string m_address_sizes;
	std::string m_power_management;

	//cache information
	FastKTuple<CPUCache> m_cpu_caches;

	//extended frequency settings
	unsigned long m_transition_latency_ns;
	unsigned long m_minimal_frequency;
	unsigned long m_maximal_frequency;
	unsigned long long m_current_frequency;
	unsigned long long m_time_in_current_state;
	FastKTuple<unsigned int> m_affected_cpus;//which cpus are affected by a frequency change to the current cpu.
	FastKTuple<unsigned int> m_related_cpus;//which cpus are related to the current cpu.
	FastKTuple<enum CPU_GOVERNOR> m_available_governors;
	enum CPU_GOVERNOR m_current_governor;
	unsigned long m_current_governor_minimal_frequency;
	unsigned long m_current_governor_maximal_frequency;
	FastKTuple<unsigned long long> m_available_freqs;
	unsigned long long m_total_cpu_time;
	unsigned long long m_state_transition_count;
	enum CPU_POWERDRIVER m_powerdriver;

	unsigned int m_cpu_num;

	void cleanText(std::string& s);
};

} /* namespace Lazarus */
#endif /* SYSTEMCPU_H_ */
