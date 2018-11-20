/*
 * SystemCPU.cpp
 *
 *  Created on: 27.06.2013
 *      Author: darius
 */

#include "SystemCPU.h"
#include "../Toolbox.h"
#include "../FileSystem/Directory.h"

#include <string.h>
#include <stdio.h>
#include <algorithm>

//#include </usr/local/linux_source/linux-4.1.10/tools/power/cpupower/lib/cpufreq.h>


#ifdef USECPUPOWER
#include <cpufreq.h>
#endif

namespace Lazarus {

SystemCPU::SystemCPU(unsigned int cpu_num) {

	m_cpu_num = cpu_num;
	m_processor = 0;
	m_cpu_family = 0;
	m_model = 0;
	m_stepping = 0;
	m_cpu_mhz = 0;
	m_cache_size = 0;
	m_physical_id = 0;
	m_siblings = 0;
	m_core_id = 0;
	m_cpu_cores = 0;
	m_apicid = 0;
	m_initial_apicid = 0;
	m_cpuid_level = 0;
	m_bogomips = 0;
	m_clflush_size = 0;
	m_cache_alignment = 0;

	loadInfo();
}

SystemCPU::SystemCPU() {

	m_cpu_num = 0;
	m_processor = 0;
	m_cpu_family = 0;
	m_model = 0;
	m_stepping = 0;
	m_cpu_mhz = 0;
	m_cache_size = 0;
	m_physical_id = 0;
	m_siblings = 0;
	m_core_id = 0;
	m_cpu_cores = 0;
	m_apicid = 0;
	m_initial_apicid = 0;
	m_cpuid_level = 0;
	m_bogomips = 0;
	m_clflush_size = 0;
	m_cache_alignment = 0;
	m_time_in_current_state = 0;
	m_minimal_frequency = 0;
	m_maximal_frequency = 0;
	m_transition_latency_ns = 0;
	m_total_cpu_time = 0;
	m_current_governor = CPU_GOVERNOR_UNKNOWN;
	m_powerdriver = CPU_POWERDRIVER_UNKNOWN;
	m_state_transition_count = 0;
	m_current_frequency = 0;
	m_current_governor_minimal_frequency = 0;
	m_current_governor_maximal_frequency = 0;

}

SystemCPU::~SystemCPU() {
	// TODO Auto-generated destructor stub
}

void SystemCPU::loadInfo()
{
	FILE* fp;
	char buffer[32000];
	size_t bytes_read;
	char* match;
	/* Read the entire contents of /proc/cpuinfo into the buffer. */
	fp = fopen ("/proc/cpuinfo", "r");
	bytes_read = fread (buffer, 1, sizeof (buffer), fp);
	fclose (fp);

	/* Bail if read failed or if buffer is not big enough. */
	if (bytes_read == 0 || bytes_read == sizeof (buffer))
	{
		printf("ERROR: could not read the CPU information\n");
		return;
	}

	/* NUL-terminate the text. */
	buffer[bytes_read] = '\0';


	/* Locate the line that starts with 'processor: ID'. */
	//create the search phrase
	char phrase[50];
	sprintf(phrase,"processor\t: %d",m_cpu_num);
	//locate the line
	match = strstr(buffer, phrase);

	if (match == NULL)
	{
		printf("ERROR: could not locate processor line\n");
		return;
	}
	/* Parse the line to extract the processor id */
	sscanf (match, "processor : %u", &m_processor);

	//++++++++++++ VENDORID
	//locate the line
	match = strstr(match, "vendor_id");

	if (match == NULL)
	{
		printf("ERROR: could not locate vendor line\n");
		return;
	}
	/* Parse the line to extract the vendor id. */
	char temp[1024];
	sscanf (match, "vendor_id : %[^\t\n]", temp);
	m_vendor_id = std::string(temp);

	//++++++++++++ CPU family
	//locate the line
	match = strstr(match, "cpu family");

	if(match == NULL)
	{
		printf("ERROR: could not locate cpu family line\n");
		return;
	}
	/* Parse the line to extract the vendor id. */
	sscanf (match, "cpu family : %u", &m_cpu_family);

	//++++++++++++ model
	//locate the line
	match = strstr(match, "model");

	if(match == NULL)
	{
		printf("ERROR: could not locate model line\n");
		return;
	}
	/* Parse the line to extract the vendor id. */
	sscanf (match, "model : %u", &m_model);

	//++++++++++++ model name
	//locate the line
	match = strstr(match, "model name");

	if(match == NULL)
	{
		printf("ERROR: could not locate model name line\n");
		return;
	}
	/* Parse the line to extract the vendor id. */
	sscanf (match, "model name : %[^\t\n]", temp);
	m_model_name = std::string(temp);

	//++++++++++++ stepping
	//locate the line
	match = strstr(match, "stepping");

	if(match == NULL)
	{
		printf("ERROR: could not locate stepping line\n");
		return;
	}
	/* Parse the line to extract the vendor id. */
	sscanf (match, "stepping : %u", &m_stepping);

	//++++++++++++ microcode
	//locate the line
	match = strstr(match, "microcode");

	if(match == NULL)
	{
		printf("ERROR: could not locate microcode line\n");
		return;
	}
	/* Parse the line to extract the vendor id. */
	sscanf (match, "microcode : %[^\t\n]", temp);
	m_microcode = std::string(temp);

	//++++++++++++ mhz
	//locate the line
	match = strstr(match, "cpu MHz");

	if(match == NULL)
	{
		printf("ERROR: could not locate cpu mhz line\n");
		return;
	}
	/* Parse the line to extract the vendor id. */
	sscanf (match, "cpu MHz : %f", &m_cpu_mhz);

	//++++++++++++ cache size
	//locate the line
	match = strstr(match, "cache size");

	if(match == NULL)
	{
		printf("ERROR: could not locate cache size line\n");
		return;
	}
	/* Parse the line to extract the vendor id. */
	sscanf (match, "cache size : %u", &m_cache_size);

	//++++++++++++ physical id
	//locate the line
	match = strstr(match, "physical id");

	if(match == NULL)
	{
		printf("ERROR: could not locate physical id line\n");
		return;
	}
	/* Parse the line to extract the vendor id. */
	sscanf (match, "physical id : %u", &m_physical_id);

	//++++++++++++ siblings
	//locate the line
	match = strstr(match, "siblings");

	if(match == NULL)
	{
		printf("ERROR: could not locate siblings line\n");
		return;
	}
	/* Parse the line to extract the vendor id. */
	sscanf (match, "siblings : %u", &m_siblings);

	//++++++++++++ core id
	//locate the line
	match = strstr(match, "core id");

	if(match == NULL)
	{
		printf("ERROR: could not locate core id line\n");
		return;
	}
	/* Parse the line to extract the vendor id. */
	sscanf (match, "core id : %u", &m_core_id);

	//++++++++++++ cpu cores
	//locate the line
	match = strstr(match, "cpu cores");

	if(match == NULL)
	{
		printf("ERROR: could not locate cpu cores line\n");
		return;
	}
	/* Parse the line to extract the vendor id. */
	sscanf (match, "cpu cores : %u", &m_cpu_cores);

	//++++++++++++ apicid
	//locate the line
	match = strstr(match, "apicid");

	if(match == NULL)
	{
		printf("ERROR: could not locate apicid line\n");
		return;
	}
	/* Parse the line to extract the vendor id. */
	sscanf (match, "apicid : %u", &m_apicid);

	//++++++++++++ initial apicid
	//locate the line
	match = strstr(match, "initial apicid");

	if(match == NULL)
	{
		printf("ERROR: could not locate initial apicid line\n");
		return;
	}
	/* Parse the line to extract the vendor id. */
	sscanf (match, "initial apicid : %u", &m_initial_apicid);

	//++++++++++++ fpu
	//locate the line
	match = strstr(match, "fpu");

	if(match == NULL)
	{
		printf("ERROR: could not locate fpu line\n");
		return;
	}
	/* Parse the line to extract the vendor id. */
	sscanf (match, "fpu : %[^\t\n]", temp);
	m_fpu = std::string(temp);

	//++++++++++++ fpu exception
	//locate the line
	match = strstr(match, "fpu_exception");

	if(match == NULL)
	{
		printf("ERROR: could not locate fpu_exception line\n");
		return;
	}
	/* Parse the line to extract the vendor id. */
	sscanf (match, "fpu exception : %[^\t\n]", temp);
	m_fpu_exception = std::string(temp);

	//++++++++++++ cpu id level
	//locate the line
	match = strstr(match, "cpuid level");

	if(match == NULL)
	{
		printf("ERROR: could not locate cpuid level line\n");
		return;
	}
	/* Parse the line to extract the vendor id. */
	sscanf (match, "cpuid level : %u", &m_cpuid_level);

	//++++++++++++ wp
	//locate the line
	match = strstr(match, "wp");

	if(match == NULL)
	{
		printf("ERROR: could not locate wp line\n");
		return;
	}
	/* Parse the line to extract the vendor id. */
	sscanf (match, "wp : %[^\t\n]", temp);
	m_wp = std::string(temp);

	//++++++++++++ fpu exception
	//locate the line
	match = strstr(match, "flags");

	if(match == NULL)
	{
		printf("ERROR: could not locate flags line\n");
		return;
	}
	/* Parse the line to extract the vendor id. */
	sscanf (match, "flags : %[^\t\n]", temp);
	m_flags = std::string(temp);

	//++++++++++++ bogomips
	//locate the line
	match = strstr(match, "bogomips");

	if(match == NULL)
	{
		printf("ERROR: could not locate bogomips line\n");
		return;
	}
	/* Parse the line to extract the vendor id. */
	sscanf (match, "bogomips : %f", &m_bogomips);

	//++++++++++++ clflush size
	//locate the line
	match = strstr(match, "clflush size");

	if(match == NULL)
	{
		printf("ERROR: could not locate clflush size line\n");
		return;
	}
	/* Parse the line to extract the vendor id. */
	sscanf (match, "clflush size : %u", &m_clflush_size);

	//++++++++++++ cache_alignment
	//locate the line
	match = strstr(match, "cache_alignment");

	if(match == NULL)
	{
		printf("ERROR: could not locate cache_alignment line\n");
		return;
	}
	/* Parse the line to extract the vendor id. */
	sscanf (match, "cache_alignment : %u", &m_cache_alignment);

	//++++++++++++ address sizes
	//locate the line
	match = strstr(match, "address sizes");

	if(match == NULL)
	{
		printf("ERROR: could not locate address sizes line\n");
		return;
	}
	/* Parse the line to extract the vendor id. */
	sscanf (match, "address sizes : %[^\t\n]", temp);
	m_address_sizes = std::string(temp);

	//++++++++++++ power management
	//locate the line
	match = strstr(match, "power management");

	if(match == NULL)
	{
		printf("ERROR: could not locate power management line\n");
		return;
	}
	/* Parse the line to extract the vendor id. */
	sscanf (match, "power management: %[^\t\n]", temp);
	m_power_management = std::string(temp);
	//printf("~~~~~~~~~~~~~~~~~~~~~~ %s\n",m_power_management.c_str());

#ifdef USECPUPOWER
	//--------------- libcpupower stuff ---------------------------
	struct cpufreq_available_governors* governors = cpufreq_get_available_governors(m_cpu_num);
	struct cpufreq_available_governors* governors_first = NULL;

	if(governors != NULL)
		governors_first = governors->first;
	else
		printf("ERROR: cpufreq_get_available_governors\n");

	m_available_governors.clear();
	while(governors != NULL)
	{
		if( std::string(governors->governor).compare("ondemand") == 0 )
			m_available_governors.push(CPU_GOVERNOR_ONDEMAND);

		else if( std::string(governors->governor).compare("performance") == 0 )
			m_available_governors.push(CPU_GOVERNOR_PERFORMANCE);

		else if( std::string(governors->governor).compare("conservative") == 0 )
			m_available_governors.push(CPU_GOVERNOR_CONSERVATIVE);

		else if( std::string(governors->governor).compare("powersave") == 0 )
			m_available_governors.push(CPU_GOVERNOR_POWERSAVE);

		else if( std::string(governors->governor).compare("userspace") == 0 )
			m_available_governors.push(CPU_GOVERNOR_USERSPACE);

		governors = governors->next;
	}

	//free
	cpufreq_put_available_governors(governors_first);

	//----------------------------------------------------------------------------

	struct cpufreq_available_frequencies* freqs = cpufreq_get_available_frequencies(m_cpu_num);
	struct cpufreq_available_frequencies* freqs_first = NULL;

	if(freqs != NULL)
		freqs_first = freqs->first;
	else
		printf("ERROR: cpufreq_get_available_frequencies\n");

	m_available_freqs.clear();
	while(freqs != NULL)
	{
		m_available_freqs.push((unsigned long long)(freqs->frequency));
		freqs = freqs->next;
	}

	//free
	cpufreq_put_available_frequencies(freqs_first);

	//----------------------------------------------------------------------------

	struct cpufreq_affected_cpus* affected = cpufreq_get_affected_cpus(m_cpu_num);
	struct cpufreq_affected_cpus* affected_first = NULL;

	if(affected != NULL)
		affected_first = affected->first;
	else
		printf("ERROR: cpufreq_get_affected_cpus\n");

	m_affected_cpus.clear();
	while(affected != NULL)
	{
		m_affected_cpus.push((unsigned long long)(affected->cpu));
		affected = affected->next;
	}

	//free
	cpufreq_put_affected_cpus(affected_first);

	//----------------------------------------------------------------------------

	struct cpufreq_affected_cpus* related = cpufreq_get_related_cpus(m_cpu_num);
	struct cpufreq_affected_cpus* related_first = NULL;

	if(related != NULL)
		related_first = related->first;
	else
		printf("ERROR: cpufreq_get_related_cpus\n");

	m_related_cpus.clear();
	while(related != NULL)
	{
		m_related_cpus.push((unsigned long long)(related->cpu));
		related = related->next;
	}

	//free
	cpufreq_put_affected_cpus(related_first);

	//----------------------------------------------------------------------------

	struct cpufreq_stats* stats = cpufreq_get_stats(m_cpu_num,&m_total_cpu_time);

	if(stats != NULL)
	{
		m_current_frequency = stats->frequency;
		m_time_in_current_state = stats->time_in_state;
	}
	else
	{
		m_current_frequency = 0;
		m_time_in_current_state = 0;
		m_total_cpu_time = 0;
		printf("ERROR: cpufreq_get_stats\n");
	}
	//free
	cpufreq_put_stats(stats);

	//----------------------------------------------------------------------------

	struct cpufreq_policy* policy = cpufreq_get_policy(m_cpu_num);

	if(policy != NULL)
	{
		if( std::string(policy->governor).compare("ondemand") == 0 )
			m_current_governor = CPU_GOVERNOR_ONDEMAND;

		else if( std::string(policy->governor).compare("performance") == 0 )
			m_current_governor = CPU_GOVERNOR_PERFORMANCE;

		else if( std::string(policy->governor).compare("conservative") == 0 )
			m_current_governor = CPU_GOVERNOR_CONSERVATIVE;

		else if( std::string(policy->governor).compare("powersave") == 0 )
			m_current_governor = CPU_GOVERNOR_POWERSAVE;

		else if( std::string(policy->governor).compare("userspace") == 0 )
			m_current_governor = CPU_GOVERNOR_USERSPACE;


		m_current_governor_minimal_frequency = policy->min;
		m_current_governor_maximal_frequency = policy->max;
	}
	else
	{
		m_current_governor = CPU_GOVERNOR_UNKNOWN;
		m_current_governor_minimal_frequency = 0;
		m_current_governor_maximal_frequency = 0;
		printf("ERROR: cpufreq_get_policy\n");
	}

	//free
	cpufreq_put_policy(policy);

	//----------------------------------------------------------------------------

	char* driver = cpufreq_get_driver(m_cpu_num);

	if(policy != NULL)
	{
		if( std::string(driver).compare("intel_pstate") == 0 )
			m_powerdriver = CPU_POWERDRIVER_INTEL_PSTATE;

		else if( std::string(driver).compare("acpi-cpufreq") == 0 )
			m_powerdriver = CPU_POWERDRIVER_ACPI_CPUFREQ;

		else if( std::string(driver).compare("speedstep-lib") == 0 )
			m_powerdriver = CPU_POWERDRIVER_SPEEDSTEP_LIB;

		else if( std::string(driver).compare("powernow-k8") == 0 )
			m_powerdriver = CPU_POWERDRIVER_POWERNOW_K8;

		else if( std::string(driver).compare("pcc-cpufreq") == 0 )
			m_powerdriver = CPU_POWERDRIVER_PCC_CPUFREQ;

		else if( std::string(driver).compare("p4-clockmod") == 0 )
			m_powerdriver = CPU_POWERDRIVER_P4_CLOCKMOD;
	}
	else
	{
		m_powerdriver = CPU_POWERDRIVER_UNKNOWN;
		printf("ERROR: cpufreq_get_driver\n");
	}
	//free
	cpufreq_put_driver(driver);

	//----------------------------------------------------------------------------

	m_state_transition_count = cpufreq_get_transitions(m_cpu_num);

	//----------------------------------------------------------------------------

	int res = cpufreq_get_hardware_limits(m_cpu_num,
					&m_minimal_frequency,
					&m_maximal_frequency);

	if(res != 0)
	{
		m_minimal_frequency = 0;
		m_maximal_frequency = 0;
		printf("ERROR: could not get hardware limits\n");
	}

	//----------------------------------------------------------------------------

	m_transition_latency_ns = cpufreq_get_transition_latency(m_cpu_num);
#endif
#ifdef NOCPUPOWER
        m_current_frequency = 0;
        m_time_in_current_state = 0;
        m_current_governor = CPU_GOVERNOR_UNKNOWN;

        m_current_governor_minimal_frequency = 0;
        m_current_governor_maximal_frequency = 0;

        m_powerdriver = CPU_POWERDRIVER_INTEL_PSTATE;

        m_state_transition_count = 0;
        m_transition_latency_ns = 0;
#endif

    //------------------------- read cache information

    //create file string for current cpu
    std::string cache_path = "/sys/devices/system/cpu/cpu"+Toolbox::intToString(m_cpu_num)+"/cache/";

    //determine the number of caches
    unsigned int cache_count = 0;
    Directory dir(cache_path);

    dir.readDirectoryDirectories();

    DirectoryEntry* entry = dir.getNextFile();
    while( entry != NULL)
    {
    	if(entry->m_filename.size()>=5)
			if(entry->m_filename.substr(0,5).compare("index")==0)
			{
				++cache_count;
			}

    	entry = dir.getNextFile();
    }//printf("found %d caches\n",cache_count);

    //resize cache tuple
    m_cpu_caches.clear(cache_count);
    m_cpu_caches = FastKTuple<CPUCache>(cache_count);
    std::string text = "";
    std::string path2 = "";

    //read config for each cache
    for(unsigned int i=0;i<cache_count;++i)
    {
    	CPUCache cache;
    	path2 = cache_path+"index"+Toolbox::intToString(i);

    	text = Toolbox::readFileToString(path2+"/coherency_line_size");
    	cleanText(text);
    	cache.m_coherency_line_size = Toolbox::stringToULongLong(text);
    	text = Toolbox::readFileToString(path2+"/level");
    	cleanText(text);
    	cache.m_cache_level = Toolbox::stringToUInt(text);
    	text = Toolbox::readFileToString(path2+"/number_of_sets");
    	cleanText(text);
    	cache.m_number_of_sets = Toolbox::stringToUInt(text);
    	text = Toolbox::readFileToString(path2+"/physical_line_partition");
    	cleanText(text);
    	cache.m_physical_line_partition = Toolbox::stringToULongLong(text);
    	text = Toolbox::readFileToString(path2+"/shared_cpu_list");
    	cache.m_shared_cpus = text;
    	text = Toolbox::readFileToString(path2+"/shared_cpu_map");
    	cache.m_shared_cpu_map = text;
    	text = Toolbox::readFileToString(path2+"/size");
    	cleanText(text);
    	cache.m_cache_size = Toolbox::getExpMetricULongLong(text);
    	text = Toolbox::readFileToString(path2+"/type");
    	cache.m_cache_type = text;
    	text = Toolbox::readFileToString(path2+"/ways_of_associativity");
    	cache.m_ways_of_associativity = Toolbox::stringToUInt(text);

    	m_cpu_caches[i] = cache;
    }

	return;
}

void SystemCPU::cleanText(std::string& s)
{
	std::replace(s.begin(),s.end(),'\n',' ');
	s.erase( std::remove_if(s.begin(), s.end(), isspace),s.end() );
}

int SystemCPU::setFrequency(unsigned long long freq)
{
#ifdef USECPUPOWER
	return cpufreq_set_frequency(m_cpu_num,	freq);
#endif
#ifdef NOCPUPOWER
	return 0;
#endif
}

int SystemCPU::setGovernor(enum CPU_GOVERNOR gov, unsigned long long min_freq,unsigned long long max_freq)
{
#ifdef USECPUPOWER
	struct cpufreq_policy policy;

	if(gov == CPU_GOVERNOR_ONDEMAND)
		policy.governor = (char*) std::string("ondemand").c_str();

	else if(gov == CPU_GOVERNOR_PERFORMANCE)
		policy.governor = (char*)std::string("performance").c_str();

	else if(gov == CPU_GOVERNOR_CONSERVATIVE)
		policy.governor = (char*)std::string("conservative").c_str();

	else if(gov == CPU_GOVERNOR_POWERSAVE)
		policy.governor = (char*) std::string("powersave").c_str();

	else if(gov == CPU_GOVERNOR_USERSPACE)
		policy.governor = (char*) std::string("userspace").c_str();

	policy.max = max_freq;
	policy.min = min_freq;

	return cpufreq_set_policy(m_cpu_num, &policy);
#endif
#ifdef NOCPUPOWER
	return 0;
#endif
}

int SystemCPU::setGovernor(enum CPU_GOVERNOR gov)
{
#ifdef USECPUPOWER
	struct cpufreq_policy policy;

	if(gov == CPU_GOVERNOR_ONDEMAND)
		policy.governor = (char*)std::string("ondemand").c_str();

	else if(gov == CPU_GOVERNOR_PERFORMANCE)
		policy.governor = (char*)std::string("performance").c_str();

	else if(gov == CPU_GOVERNOR_CONSERVATIVE)
		policy.governor = (char*)std::string("conservative").c_str();

	else if(gov == CPU_GOVERNOR_POWERSAVE)
		policy.governor = (char*)std::string("powersave").c_str();

	else if(gov == CPU_GOVERNOR_USERSPACE)
		policy.governor = (char*)std::string("userspace").c_str();

	policy.max = m_maximal_frequency;
	policy.min = m_minimal_frequency;

	return cpufreq_set_policy(m_cpu_num, &policy);
#endif
#ifdef NOCPUPOWER
	return 0;
#endif
}

unsigned int SystemCPU::get_m_processor() const
{
	return m_processor;
}

std::string SystemCPU::get_m_vendor_id() const
{
	return m_vendor_id;
}

unsigned int SystemCPU::get_m_cpu_family() const
{
	return m_cpu_family;
}

unsigned int SystemCPU::get_m_model() const
{
	return m_model;
}

std::string SystemCPU::get_m_model_name() const
{
	return m_model_name;
}

unsigned int SystemCPU::get_m_stepping() const
{
	return m_stepping;
}

std::string SystemCPU::get_m_microcode() const
{
	return m_microcode;
}

double SystemCPU::get_m_cpu_mhz() const
{
	return m_cpu_mhz;
}

unsigned int SystemCPU::get_m_cache_size() const
{
	return m_cache_size;
}

unsigned int SystemCPU::get_m_physical_id() const
{
	return m_physical_id;
}

unsigned int SystemCPU::get_m_siblings() const
{
	return m_siblings;
}

unsigned int SystemCPU::get_m_core_id() const
{
	return m_core_id;
}

unsigned int SystemCPU::get_m_cpu_cores() const
{
	return m_cpu_cores;
}

unsigned int SystemCPU::get_m_apicid() const
{
	return m_apicid;
}

unsigned int SystemCPU::get_m_initial_apicid() const
{
	return m_initial_apicid;
}

std::string SystemCPU::get_m_fpu() const
{
	return m_fpu;
}

std::string SystemCPU::get_m_fpu_exception() const
{
	return m_fpu_exception;
}

unsigned int SystemCPU::get_m_cpuid_level() const
{
	return m_cpuid_level;
}

std::string SystemCPU::get_m_wp() const
{
	return m_wp;
}

std::string SystemCPU::get_m_flags() const
{
	return m_flags;
}

double SystemCPU::get_m_bogomips() const
{
	return m_bogomips;
}

unsigned int SystemCPU::get_m_clflush_size() const
{
	return m_clflush_size;
}

unsigned int SystemCPU::get_m_cache_alignment() const
{
	return m_cache_alignment;
}

std::string SystemCPU::get_m_address_sizes() const
{
	return m_address_sizes;
}

std::string SystemCPU::get_m_power_management() const
{
	return m_power_management;
}

const FastKTuple<unsigned int>& SystemCPU::getAffectedCpus() const {
	return m_affected_cpus;
}

const FastKTuple<unsigned long long>& SystemCPU::getAvailableFreqs() const {
	return m_available_freqs;
}

const FastKTuple<enum SystemCPU::CPU_GOVERNOR>& SystemCPU::getAvailableGovernors() const {
	return m_available_governors;
}

unsigned long long SystemCPU::getCurrentFrequency() const {
	return m_current_frequency;
}

enum SystemCPU::CPU_GOVERNOR SystemCPU::getCurrentGovernor() const {
	return m_current_governor;
}

unsigned long SystemCPU::getCurrentGovernorMaximalFrequency() const {
	return m_current_governor_maximal_frequency;
}

unsigned long SystemCPU::getCurrentGovernorMinimalFrequency() const {
	return m_current_governor_minimal_frequency;
}

unsigned long SystemCPU::getMaximalFrequency() const {
	return m_maximal_frequency;
}

unsigned long SystemCPU::getMinimalFrequency() const {
	return m_minimal_frequency;
}

const FastKTuple<unsigned int>& SystemCPU::getRelatedCpus() const {
	return m_related_cpus;
}

unsigned long long SystemCPU::getStateTransitionCount() const {
	return m_state_transition_count;
}

unsigned long long SystemCPU::getTimeInCurrentState() const {
	return m_time_in_current_state;
}

unsigned long long SystemCPU::getTotalCpuTime() const {
	return m_total_cpu_time;
}

unsigned long SystemCPU::getTransitionLatencyNs() const {
	return m_transition_latency_ns;
}

void SystemCPU::printCPUInfo()
{
	printf("********************* Printing CPU information:\n");

	printf("Processor %u\n",m_processor);
	printf("Vendor ID %s\n",m_vendor_id.c_str());
	printf("CPU family %u\n",m_cpu_family);
	printf("Model %u\n",m_model);
	printf("Model name %s\n",m_model_name.c_str());
	printf("Stepping %u\n",m_stepping);
	printf("Microcode %s\n",m_microcode.c_str());
	printf("CPU MHz %f\n",m_cpu_mhz);
	printf("Cache size %u\n",m_cache_size);
	printf("Physical ID %u\n",m_physical_id);
	printf("Siblings %u\n",m_siblings);
	printf("Core ID %u\n",m_core_id);
	printf("CPU cores %u\n",m_cpu_cores);
	printf("APIC ID %u\n",m_apicid);
	printf("Initial APIC ID %u\n",m_initial_apicid);
	printf("FPU %s\n",m_fpu.c_str());
	printf("FPU exception %s\n",m_fpu_exception.c_str());
	printf("CPU ID level %u\n",m_cpuid_level);
	printf("WP %s\n",m_wp.c_str());
	printf("Flags %s\n",m_flags.c_str());
	printf("Bogomips %f\n",m_bogomips);
	printf("CLFlush size %u\n",m_clflush_size);
	printf("Cache alignment %u \n",m_cache_alignment);
	printf("Address sizes %s\n",m_address_sizes.c_str());
	printf("Power management %s\n",m_power_management.c_str());

	printf("Power state driver: ");
	switch(m_powerdriver)
	{
	case CPU_POWERDRIVER_INTEL_PSTATE:
		printf("\t intel_pstate \n");
		break;

	case CPU_POWERDRIVER_ACPI_CPUFREQ:
		printf("\t acpi-cpufreq \n");
		break;

	case CPU_POWERDRIVER_SPEEDSTEP_LIB:
		printf("\t speedstep-lib \n");
		break;

	case CPU_POWERDRIVER_POWERNOW_K8:
		printf("\t powernow-k8 \n");
		break;

	case CPU_POWERDRIVER_PCC_CPUFREQ:
		printf("\t pcc-cpufreq \n");
		break;

	case CPU_POWERDRIVER_P4_CLOCKMOD:
		printf("\t p4-clockmod \n");
		break;
	case CPU_POWERDRIVER_UNKNOWN:
		printf("\t UNKNOWN POWER DRIVER ... \n");
		break;
	}

	printf("Power state transition latency %lu ns\n",m_transition_latency_ns);
	printf("Power state minimal HW frequency %lu Hz\n",m_minimal_frequency);
	printf("Power state maximal HW frequency %lu Hz\n",m_maximal_frequency);
	printf("Power state current frequency %llu Hz\n",m_current_frequency);
	printf("Power state time in current state %llu\n",m_time_in_current_state);

	printf("Power state affected CPUs: \n");
	for(unsigned int i=0;i<m_affected_cpus.getm_size();++i)
	{
		printf("\t %u \n",m_affected_cpus[i]);
	}

	printf("Power state related CPUs: \n");
	for(unsigned int i=0;i<m_related_cpus.getm_size();++i)
	{
		printf("\t %u \n",m_related_cpus[i]);
	}

	printf("Power state %lld available Governors: \n",m_available_governors.getm_size());
	for(unsigned int i=0;i<m_available_governors.getm_size();++i)
	{
		switch(m_available_governors[i])
		{
		case CPU_GOVERNOR_ONDEMAND:
			printf("\t ondemand \n");
			break;

		case CPU_GOVERNOR_PERFORMANCE:
			printf("\t performance \n");
			break;

		case CPU_GOVERNOR_CONSERVATIVE:
			printf("\t conservative \n");
			break;

		case CPU_GOVERNOR_POWERSAVE:
			printf("\t powersave \n");
			break;

		case CPU_GOVERNOR_USERSPACE:
			printf("\t userspace \n");
			break;
                        
		case CPU_GOVERNOR_UNKNOWN:
				printf("\t UNKNOWN (no cpupower support?) \n");
				break;
		}
	}

	printf("Power state current governor %u \n",m_current_governor);
	printf("Power state current governor minimal frequency %lu \n",m_current_governor_minimal_frequency);
	printf("Power state current governor maximal frequency %lu \n",m_current_governor_maximal_frequency);

	printf("Power state available frequencies: \n");
	for(unsigned int i=0;i<m_available_freqs.getm_size();++i)
	{
		printf("\t %llu \n",m_available_freqs[i]);
	}

	printf("Power state total cpu time %llu \n",m_total_cpu_time);
	printf("Power state state transition count %llu \n",m_state_transition_count);

	printf("\t\t******** Printing CPU cache information:\n");
	printf("\t\t******** CPU has %lld caches\n",m_cpu_caches.getm_size());
	for(unsigned int i=0;i<m_cpu_caches.getm_size();++i)
	{
		CPUCache& cache = m_cpu_caches[i];
		printf("\t\t******** CPU cache %u\n",i);
		printf("\t\tCache level: %u\n",cache.m_cache_level);
		printf("\t\tCache size: %llu\n",cache.m_cache_size);
		printf("\t\tCache type: %s\n",cache.m_cache_type.c_str());
		printf("\t\tCache line size: %llu\n",cache.m_coherency_line_size);
		printf("\t\tCache number of sets: %u\n",cache.m_number_of_sets);
		printf("\t\tCache physical line partition: %llu\n",cache.m_physical_line_partition);
		printf("\t\tCache shared_cpu_map: %s\n",cache.m_shared_cpu_map.c_str());
		printf("\t\tCache shared cpus: %s\n",cache.m_shared_cpus.c_str());
		printf("\t\tCache ways of associativity: %u\n",cache.m_ways_of_associativity);
	}

}

void SystemCPU::serialize()
{
	Serializable::resetBuffer();
	registerElement<unsigned int>(14+1);
	registerElement<float>(2);
	registerString(m_vendor_id);
	registerString(m_model_name);
	registerString(m_microcode);
	registerString(m_fpu);
	registerString(m_fpu_exception);
	registerString(m_wp);
	registerString(m_flags);
	registerString(m_address_sizes);
	registerString(m_power_management);

	for(unsigned int i=0;i<m_cpu_caches.getm_size();i++)
	{
		m_cpu_caches[i].serialize();
		registerUCharA(m_cpu_caches[i].get_mp_serialized_data_buffer()->get_m_length());
	}

	allocateBuffer();

	addUInt(m_processor);
	addString(m_vendor_id);
	addUInt(m_cpu_family);
	addUInt(m_model);
	addString(m_model_name);
	addUInt(m_stepping);
	addString(m_microcode);
	addFloat(m_cpu_mhz);
	addUInt(m_cache_size);
	addUInt(m_physical_id);
	addUInt(m_siblings);
	addUInt(m_core_id);
	addUInt(m_cpu_cores);
	addUInt(m_apicid);
	addUInt(m_initial_apicid);
	addString(m_fpu);
	addString(m_fpu_exception);
	addUInt(m_cpuid_level);
	addString(m_wp);
	addString(m_flags);
	addFloat(m_bogomips);
	addUInt(m_clflush_size);
	addUInt(m_cache_alignment);
	addString(m_address_sizes);
	addString(m_power_management);

	for(unsigned int i=0;i<m_cpu_caches.getm_size();i++)
	{
		addUCharA(m_cpu_caches[ (m_cpu_caches.getm_size()-1)-i ].get_mp_serialized_data_buffer()->get_mp_data(),
				m_cpu_caches[ (m_cpu_caches.getm_size()-1)-i ].get_mp_serialized_data_buffer()->get_m_length());
	}

	addUInt(m_cpu_caches.getm_size());

}

void SystemCPU::deserialize()
{
	unsigned int cache_count = getUInt();

	unsigned char* data = NULL;
	unsigned long long size = 0;

	m_cpu_caches.clear(cache_count);

	for(unsigned int i=0;i<cache_count;i++)
	{
		data = getUCharA(size);

		CPUCache cache;
		cache.set_mp_serialized_data_buffer(new Buffer(data,size));
		cache.deserialize();

		m_cpu_caches[cache_count-1-i] = cache;
	}

	m_power_management = getString();
	m_address_sizes = getString();
	m_cache_alignment = getUInt();
	m_clflush_size = getUInt();
	m_bogomips = getFloat();
	m_flags = getString();
	m_wp = getString();
	m_cpuid_level = getUInt();
	m_fpu_exception = getString();
	m_fpu = getString();
	m_initial_apicid = getUInt();
	m_apicid = getUInt();
	m_cpu_cores = getUInt();
	m_core_id = getUInt();
	m_siblings = getUInt();
	m_physical_id = getUInt();
	m_cache_size = getUInt();
	m_cpu_mhz = getFloat();
	m_microcode = getString();
	m_stepping = getUInt();
	m_model_name = getString();
	m_model = getUInt();
	m_cpu_family = getUInt();
	m_vendor_id = getString();
	m_processor = getUInt();

	resetBuffer();
}


void CPUCache::serialize()
{
	registerElement<unsigned long long>(3);
	registerElement<unsigned int>(3);
	registerString(m_shared_cpus);
	registerString(m_shared_cpu_map);
	registerString(m_cache_type);

	allocateBuffer();

	addUInt(m_cache_level);
	addUInt(m_number_of_sets);
	addUInt(m_ways_of_associativity);
	addULongLong(m_coherency_line_size);
	addULongLong(m_physical_line_partition);
	addULongLong(m_cache_size);
	addString(m_shared_cpus);
	addString(m_shared_cpu_map);
	addString(m_cache_type);
}

void CPUCache::deserialize()
{
	m_cache_type=getString();
	m_shared_cpu_map=getString();
	m_shared_cpus=getString();
	m_cache_size=getULongLong();
	m_physical_line_partition=getULongLong();
	m_coherency_line_size=getULongLong();
	m_ways_of_associativity=getUInt();
	m_number_of_sets=getUInt();
	m_cache_level=getUInt();

	resetBuffer();
}

CPUCache::CPUCache()
{
	m_cache_level=0;
	m_number_of_sets=0;
	m_ways_of_associativity=0;
	m_coherency_line_size=0;
	m_physical_line_partition=0;
	m_cache_size=0;
}

CPUCache::~CPUCache()
{
}

} /* namespace Lazarus */
