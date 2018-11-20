/*
 * UnitTester.h
 *
 *  Created on: 21.01.2013
 *      Author: Darius Malysiak
 */

#ifndef PROCESSWATCHDOG_H_
#define PROCESSWATCHDOG_H_

#include "../Threading/Thread.h"
#include "../Threading/Mutex.h"
#include "../System/SystemInformation.h"
#include "ProcessHelper.h"
#include "../Threading/Callback.h"

#include <string>

#include <sys/time.h>
#include <sys/resource.h>

namespace Lazarus
{

class WatchDogProcessLimit : public Lazarus::BaseObject{

public:

	WatchDogProcessLimit()
	{
		 m_max_user_time_s = -1;
		 m_max_user_time_us = -1;
		 m_max_kernel_time_s = -1;
		 m_max_kernel_time_us = -1;

		 max_rss = -1;
		 max_page_reclaims = -1;
		 max_page_faults = -1;
		 max_swaps = -1;
		 max_block_inputs = -1;
		 max_block_outputs = -1;
		 max_ipc_messages_sent = -1;
		 max_ipc_messages_received = -1;
		 max_signals_received = -1;
		 max_voluntary_context_switches = -1;
		 max_involuntary_context_switches = -1;
	}

	/**
	 * A value of '-1' means that the parameter will be ignored.
	 * */

	long long m_max_user_time_s;
	long long m_max_user_time_us;
	long long m_max_kernel_time_s;
	long long m_max_kernel_time_us;

	long max_rss;/*unit is bytes (effectively it will be quantized to multiples of the systems page size)*/
	long max_page_reclaims;
	long max_page_faults;
	long max_swaps;
	long max_block_inputs;
	long max_block_outputs;
	long max_ipc_messages_sent;
	long max_ipc_messages_received;
	long max_signals_received;
	long max_voluntary_context_switches;
	long max_involuntary_context_switches;

};

class ProcessWatchDog : public Lazarus::Thread {
public:

	CLASS_RTTI(SHUnitTests::ProcessWatchDog)

	enum TIME_UNIT{TIME_UNIT_S,TIME_UNIT_MS,TIME_UNIT_US,TIME_UNIT_NS};

	enum WATCHDOG_ERROR{WATCHDOG_ERROR_USER_TIME,
		WATCHDOG_ERROR_KERNEL_TIME,
		WATCHDOG_ERROR_RSS,
		WATCHDOG_ERROR_PAGE_RECLAIMS,
		WATCHDOG_ERROR_PAGE_FAULTS,
		WATCHDOG_ERROR_SWAPS,
		WATCHDOG_ERROR_BLOCK_INPUTS,
		WATCHDOG_ERROR_BLOCK_OUTPUTS,
		WATCHDOG_ERROR_IPC_SENT,
		WATCHDOG_ERROR_IPC_RECV,
		WATCHDOG_ERROR_SIGNALS_RECV,
		WATCHDOG_ERROR_VCXT_SWITCHES,
		WATCHDOG_ERROR_IVCXT_SWITCHES,
		WATCHDOG_ERROR_NOERROR,
		WATCHDOG_ERROR_UNKNOWN};

	/**
	 * The watchdog thread will run alongside the unit test,
	 * which in turn must call the watchdogs call method to signal
	 * that everything is going well.
	 * Should the process not call the watchdog thread at 'calls_per_poll'
	 * in an poll_time period, the watchdog will terminate the unit test.
	 * 'poll_time' represents the interval in which the process limits will be checked
	 * and in which the watchdog will check for an answer of the process.
	 * */
	ProcessWatchDog(double calls_per_poll,
			unsigned int poll_time, enum TIME_UNIT poll_time_unit,
			const WatchDogProcessLimit& limits)
	{
		m_limits = limits;

		m_answers_per_poll = calls_per_poll;

		m_poll_time = poll_time;
		m_poll_time_unit = poll_time_unit;

		mp_self = Lazarus::Process::getSelf();

		m_page_size = Lazarus::SystemInformation::getPageSize();

		m_time_since_last_answer.exchange(0);
		m_print_info.exchange(false);
		m_call_count = calls_per_poll*m_poll_time;

		m_go.exchange(false);

		mp_callback = NULL;

	}

	virtual ~ProcessWatchDog()
	{}

	/**
	 * This decides if the WD should print the current process resource utilization.
	 * */
	void setPrintInfo(bool val)
	{
		m_print_info.exchange(val);
	}

	void startChecks()
	{
		m_go.exchange(true);
	}

	void stopChecks()
	{
		m_go.exchange(false);
	}

	void run()
	{
//		while(m_go == false && m_first_run == true)
//		{
//			Lazarus::Toolbox::sleep_ms(50);
//		}
//
//		m_first_run.exchange(false);

		while(m_shutdown_flag == false)
		{//printf("check\n");
			m_mutex.lock();

			//if WD was disabled: idle and reset
			if(m_go == false)
			{
				Lazarus::Toolbox::sleep_ms(50);
				m_call_count = m_poll_time*m_poll_time;
				m_mutex.unlock();
				continue;
			}

			//check for limit breaks
			enum WATCHDOG_ERROR error = checkLimits();
			if(error != WATCHDOG_ERROR_NOERROR)
			{
				ERROR("watchdog limits exceeded, unit test has gone awall")
				printResInfo();

				if(mp_callback != NULL)
				{	//commit suicide
					this->mp_self->killProcess();
				}
				else
					mp_callback->callbackMethod(this);
			}

			//check if an answer was received;
			if((double)m_call_count/(double)m_poll_time < m_answers_per_poll )
			{
				printf("%f calls \n",(double)m_call_count/(double)m_poll_time );
				ERROR("watchdog was not called, unit test has gone awall")

				if(mp_callback != NULL)
				{	//commit suicide
					this->mp_self->killProcess();
				}
				else
					mp_callback->callbackMethod(this);
			}

			m_call_count = 0;//reset the call count

			m_mutex.unlock();

			//sleep for some time
			switch(m_poll_time_unit)
			{
			case TIME_UNIT_S:
				Lazarus::Toolbox::sleep_s(m_poll_time);
				break;
			case TIME_UNIT_MS:
				Lazarus::Toolbox::sleep_ms(m_poll_time);
				break;
			case TIME_UNIT_US:
				Lazarus::Toolbox::sleep_us(m_poll_time);
				break;
			case TIME_UNIT_NS:
				Lazarus::Toolbox::sleep_ns(m_poll_time);
				break;
			}
		}

	}

	/**
	 * The unit test should call the watchdog once in a while...
	 * */
	void call()
	{
		m_mutex.lock();
		m_call_count++;
		m_mutex.unlock();
	}

	/**
	 * Once a callback has been set, the watchdog will call the this objects callback method
	 * in case of errors. This will disable the watchdogs default kill approach. One can disable this
	 * by calling 'setCallback(NULL)' (this is not threadsafe! ... the watchdog runs in a separate thread :-).
	 * */
	void setCallback(Callback* callback)
	{
		mp_callback = callback;
	}

protected:
	WatchDogProcessLimit m_limits;
	double m_answers_per_poll;
	unsigned int m_poll_time;
	enum TIME_UNIT m_poll_time_unit;
	Lazarus::Process* mp_self;
	Lazarus::Mutex m_mutex;
	std::atomic<bool> m_go;
	Callback* mp_callback;


	long m_page_size;
	std::atomic<unsigned int> m_time_since_last_answer;
	unsigned long long m_call_count;
	std::atomic<bool> m_print_info;

	struct rusage m_usage;

	void printResInfo()
	{
		printf("user time %ld:%ld \n"
				"kernel time %ld:%ld \n"
				"rss %ld \n"
				"swaps %ld \n"
				"block inputs %ld \n"
				"block outputs %ld \n"
				"page reclaims %ld \n"
				"page faults %ld \n"
				"ipc msgs sent %ld \n"
				"ipc msgs recv %ld \n"
				"vol cxt switches %ld \n"
				"invol cxt switches %ld \n",
				m_usage.ru_utime.tv_sec,
				m_usage.ru_utime.tv_usec,
				m_usage.ru_stime.tv_sec,
				m_usage.ru_stime.tv_usec,
				mp_self->m_rss * m_page_size,
				m_usage.ru_nswap,
				m_usage.ru_inblock,
				m_usage.ru_oublock,
				m_usage.ru_minflt,
				m_usage.ru_majflt,
				m_usage.ru_msgsnd,
				m_usage.ru_msgrcv,
				m_usage.ru_nvcsw,
				m_usage.ru_nivcsw);

		printf("***********************************\n");
	}

	enum WATCHDOG_ERROR checkLimits()
	{
		int res = getrusage(RUSAGE_SELF, &m_usage);

		mp_self->updatePID(mp_self->m_pid);

		if(res != 0)
			ERROR("getrusage error!")

		//printResInfo();

		enum WATCHDOG_ERROR error = WATCHDOG_ERROR_NOERROR;

		if(m_limits.m_max_kernel_time_s != -1)
			if(m_usage.ru_stime.tv_sec > m_limits.m_max_kernel_time_s)
			{
				error = WATCHDOG_ERROR_KERNEL_TIME;
				ERROR("WATCHDOG_ERROR_KERNEL_TIME");
				return error;
			}

		if(m_limits.m_max_kernel_time_us != -1)
			if(m_usage.ru_stime.tv_usec > m_limits.m_max_kernel_time_us)
			{
				error = WATCHDOG_ERROR_KERNEL_TIME;
				ERROR("WATCHDOG_ERROR_KERNEL_TIME");
				return error;
			}

		if(m_limits.m_max_user_time_s != -1)
			if(m_usage.ru_utime.tv_sec > m_limits.m_max_user_time_s)
			{
				error = WATCHDOG_ERROR_USER_TIME;
				ERROR("WATCHDOG_ERROR_USER_TIME");
				return error;
			}

		if(m_limits.m_max_user_time_us != -1)
			if(m_usage.ru_utime.tv_usec > m_limits.m_max_user_time_us)
			{
				error = WATCHDOG_ERROR_USER_TIME;
				ERROR("WATCHDOG_ERROR_USER_TIME");
				return error;
			}

		if(m_limits.max_block_inputs != -1)
			if(m_usage.ru_inblock > m_limits.max_block_inputs)
			{
				error = WATCHDOG_ERROR_BLOCK_INPUTS;
				ERROR("WATCHDOG_ERROR_BLOCK_INPUTS");
				return error;
			}

		if(m_limits.max_block_outputs != -1)
			if(m_usage.ru_oublock > m_limits.max_block_outputs)
			{
				error = WATCHDOG_ERROR_BLOCK_OUTPUTS;
				ERROR("WATCHDOG_ERROR_BLOCK_OUTPUTS");
				return error;
			}

		if(m_limits.max_involuntary_context_switches != -1)
			if(m_usage.ru_nivcsw > m_limits.max_involuntary_context_switches)
			{
				error = WATCHDOG_ERROR_IVCXT_SWITCHES;
				ERROR("WATCHDOG_ERROR_IVCXT_SWITCHES");
				return error;
			}

		if(m_limits.max_voluntary_context_switches != -1)
			if(m_usage.ru_nvcsw > m_limits.max_voluntary_context_switches)
			{
				error = WATCHDOG_ERROR_VCXT_SWITCHES;
				ERROR("WATCHDOG_ERROR_VCXT_SWITCHES");
				return error;
			}

		if(m_limits.max_ipc_messages_sent != -1)
			if(m_usage.ru_msgsnd > m_limits.max_ipc_messages_sent)
			{
				error = WATCHDOG_ERROR_IPC_SENT;
				ERROR("WATCHDOG_ERROR_IPC_SENT");
				return error;
			}

		if(m_limits.max_ipc_messages_received != -1)
			if(m_usage.ru_msgrcv > m_limits.max_ipc_messages_received)
			{
				error = WATCHDOG_ERROR_IPC_RECV;
				ERROR("WATCHDOG_ERROR_IPC_RECV");
				return error;
			}

		if(m_limits.max_page_faults != -1)
			if(m_usage.ru_majflt > m_limits.max_page_faults)
			{
				error = WATCHDOG_ERROR_PAGE_FAULTS;
				ERROR("WATCHDOG_ERROR_PAGE_FAULTS");
				return error;
			}

		if(m_limits.max_page_reclaims != -1)
			if(m_usage.ru_minflt > m_limits.max_page_reclaims)
			{
				error = WATCHDOG_ERROR_PAGE_RECLAIMS;
				ERROR("WATCHDOG_ERROR_PAGE_RECLAIMS");
				return error;
			}

		if(m_limits.max_rss != -1)
			if(mp_self->m_rss*m_page_size > m_limits.max_rss)
			{
				error = WATCHDOG_ERROR_RSS;
				ERROR("WATCHDOG_ERROR_RSS");
				return error;
			}

		if(m_limits.max_signals_received != -1)
			if(m_usage.ru_nsignals > m_limits.max_signals_received)
			{
				error = WATCHDOG_ERROR_SIGNALS_RECV;
				ERROR("WATCHDOG_ERROR_SIGNALS_RECV");
				return error;
			}

		if(m_limits.max_swaps != -1)
			if(m_usage.ru_nswap > m_limits.max_swaps)
			{
				error = WATCHDOG_ERROR_SWAPS;
				ERROR("WATCHDOG_ERROR_SWAPS");
				return error;
			}

		return error;
	}

};


}
#endif /*  */
