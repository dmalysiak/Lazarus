/*
 * UsageLimiter.cpp
 *
 *  Created on: Jul 3, 2014
 *      Author: clustro
 */

#include "UsageLimiter.h"
#include "../Toolbox.h"

namespace Lazarus {

UsageLimiter::UsageLimiter(double perc) {

	m_perc = perc;
	m_time_slot = m_default_time_slot;
}

UsageLimiter::~UsageLimiter() {

	//iterate over all registered processes and delete them
	for(std::unordered_map<pid_t,Process*>::iterator it = m_p_map.begin(); it != m_p_map.end();++it)
	{
		delete it->second;
	}

}

void UsageLimiter::registerProcess(pid_t pid)
{
	if(m_thread_active == true)
	{
		printf("ERROR: can't register process once the limiter has started\n");
		return;
	}

	Process* p = new Process(pid);

	//insert item
	m_p_map[pid] = p;
}

void UsageLimiter::setTimeSlotSize(unsigned long long timeslot_size)
{
	m_time_slot = timeslot_size;
}

void UsageLimiter::setPerc(double perc)
{
	m_perc = perc;
}

void UsageLimiter::run()
{
	double process_set_cpu_time = 0;
	unsigned long long work_time = 0;
	unsigned long long sleep_time = 0;
	double work_rate = m_perc;
	unsigned int active_processes = 0;
	int res = 0;

	while(m_shutdown_flag == false)
	{

		//iterate over all registered processes and resume them
		for(std::unordered_map<pid_t,Process*>::iterator it = m_p_map.begin(); it != m_p_map.end();++it)
		{
			//skip removed objects
			if(it->second == NULL)
			{
				continue;
			}

			res = it->second->resumeProcess();

			//in case of an error: remove the process from the list
			if(res == -1)
			{
				m_p_map.erase(it->second->m_pid);//remove it from the list

				delete it->second; //delete the process object
			}
		}

		//give the processes the time to work
		work_time = (double)( (long double)work_rate*(long double)m_time_slot );
		sleep_time = (double)( (long double)m_time_slot - (long double)work_rate*(long double)m_time_slot );
		Toolbox::sleep_ns(work_time);


		//iterate over all registered processes and hold them
		for(std::unordered_map<pid_t,Process*>::iterator it = m_p_map.begin(); it != m_p_map.end();++it)
		{
			//skip removed objects
			if(it->second == NULL)
			{
				continue;
			}

			res = it->second->stopProcess();

			//in case of an error: remove the process from the list
			if(res == -1)
			{
				m_p_map.erase(it->second->m_pid);//remove it from the list

				delete it->second; //delete the process object
			}
		}

		//after working, let them rest
		Toolbox::sleep_ns(sleep_time);

		//iterate over all registered processes and calculate the deltas for the passed time.
		for(std::unordered_map<pid_t,Process*>::iterator it = m_p_map.begin(); it != m_p_map.end();++it)
		{
			//skip removed objects
			if(it->second == NULL)
			{
				continue;
			}

			++active_processes;

			//compute deltas within the object
			it->second->updateTimeValues();

			//accumulate the percentage of used cpu time
			process_set_cpu_time += it->second->m_cpu_usage_perc;
		}



		/*
		 * update the work rate, if the actual time consumption exceeds m_perc, the new limit will be smaller
		 * than m_perc, otherwise the limit will be raised. I.e. the system will try to adept itself to provide a fair
		 * trade over time
		 */
		work_rate = std::min(work_rate / process_set_cpu_time * m_perc, m_perc);//maybe min(x,1) ?
		process_set_cpu_time = 0;
		active_processes = 0;
	}

}

} /* namespace SHNetwork */
