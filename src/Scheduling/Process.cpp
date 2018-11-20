/*
 * Process.cpp
 *
 *  Created on: Jul 3, 2014
 *      Author: clustro
 */

#include "Process.h"
#include "../DataStructures/FastNCSList.h"
#include "../Toolbox.h"
#include "../Time/TimeToolbox.h"
#include "../Jiffies.h"

#include <errno.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/sysctl.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sched.h>

#include <string.h>
#include <stdio.h>

namespace Lazarus {

Process::Process(pid_t pid, enum TIME_DETAIL timed) {

	updatePID(pid,timed);

}

Process::Process() {

	m_time_detail = TIME_DETAIL_WITHOUT_CHILDREN;

	m_averaging_coefficient = default_averaging_coefficient;
	m_cpu_usage_perc = 0;
	m_utime_last = 0;
	m_stime_last = 0;
	m_cutime_last = 0;
	m_cstime_last = 0;

	m_pid = 0;
	m_state = 0;
	m_ppid = 0;
	m_pgrp = 0;
	m_sid = 0;
	m_tty_nr = 0;
	m_tty_pgrp = 0;
	m_flags = 0;
	m_min_flt = 0;
	m_cmin_flt = 0;
	m_maj_flt = 0;
	m_cmaj_flt = 0;
	m_utime = 0;
	m_stime = 0;
	m_cutime = 0;
	m_cstime = 0;
	m_priority = 0;
	m_nice = 0;
	m_num_threads = 0;
	m_it_real_value = 0;
	m_start_time = 0;
	m_vsize = 0;
	m_rss = 0;
	m_rsslim = 0;
	m_start_code = 0;
	m_end_code = 0;
	m_start_stack = 0;
	m_esp = 0;
	m_eip = 0;
	m_pending = 0;
	m_blocked = 0;
	m_sigign = 0;
	m_sigcatch = 0;
	m_wchan = 0;
	m_exit_signal = 0;
	m_task_cpu = 0;
	m_rt_priority = 0;
	m_policy = 0;
	m_blkio_ticks = 0;
	m_gtime = 0;
	m_cgtime = 0;
	m_start_data = 0;
	m_end_data = 0;
	m_start_brk = 0;
	m_arg_start = 0;
	m_arg_end = 0;
	m_env_start = 0;
	m_env_end = 0;
	m_exit_code = 0;

	m_rlimit_cpu = 0;
	m_rlimit_core = 0;
	m_rlimit_as = 0;
	m_rlimit_data = 0;
	m_rlimit_stack = 0;


	m_ngid=0;
	m_tracerpid=0;
	m_uid_real=0;
	m_uid_effective=0;
	m_uid_saved=0;
	m_uid_filesystem=0;
	m_gid_real=0;
	m_gid_effective=0;
	m_gid_saved=0;
	m_gid_filesystem=0;
	m_fdsize=0;
	m_vmpeak=0;
	m_vmsize=0;
	m_vmlck=0;
	m_vmpin=0;
	m_vmhwm=0;
	m_vmrss=0;
	m_rssanon=0;
	m_rssfile=0;
	m_rssshmem=0;
	m_vmdata=0;
	m_vmstk=0;
	m_vmexe=0;
	m_vmlib=0;
	m_vmpte=0;
	m_vmpmd=0;
	m_vmswap=0;
	m_hugetlbpages=0;
	m_sigq_current=0;
	m_sigq_limit=0;
	m_sigpnd=0;
	m_shdpnd=0;
	m_voluntary_ctxt_switches=0;
	m_nonvoluntary_ctxt_switches=0;
}

Process::~Process() {
	// TODO Auto-generated destructor stub
}

void Process::update()
{
	this->updatePID(this->m_pid,this->m_time_detail);
}

uid_t Process::getEffectiveUserID()
{
	return geteuid();
}

uid_t Process::getRealUserID()
{
	return getuid();
}


gid_t Process::getEffectiveGroupID()
{
	return getegid();
}

gid_t Process::getRealGroupID()
{
	return getgid();
}

std::vector<gid_t> Process::getSupplementaryGroups()
{
	int num=500;
	gid_t groups[num];
	std::vector<gid_t> groups_;

	num = getgroups(num,groups);
	if(num < 0)
	{
		if(errno == EINVAL)
		{
			printf("ERROR:getSupplementaryGroups num was too small.\n");
		}
		else
		{
			printf("ERROR:getSupplementaryGroups unknown error.\n");
		}

		return groups_;
	}

	for(int i=0;i<num;++i)
	{
		groups_.push_back(groups[i]);
	}

	return groups_;
}

int Process::setEffectiveUserID(uid_t uid)
{
	int res = setreuid(-1,uid);

	if(res == -1)
	{
		if(errno == EINVAL)
		{
			printf("ERROR:setEffectiveUserID invalid UID.\n");
		}

		if(errno == EPERM)
		{
			printf("ERROR:setEffectiveUserID insufficient permissions.\n");
		}

		return -1;
	}

	return 0;
}

int Process::setRealUserID(uid_t uid)
{
	int res = setreuid(uid,-1);

	if(res == -1)
	{
		if(errno == EINVAL)
		{
			printf("ERROR:setreuid invalid UID.\n");
		}

		if(errno == EPERM)
		{
			printf("ERROR:setreuid insufficient permissions.\n");
		}

		return -1;
	}

	return 0;
}

int Process::setEffectiveGroupID(gid_t gid)
{
	int res = setregid(-1,gid);

	if(res == -1)
	{
		if(errno == EINVAL)
		{
			printf("ERROR:setEffectiveGroupID invalid GID.\n");
		}

		if(errno == EPERM)
		{
			printf("ERROR:setEffectiveGroupID insufficient permissions.\n");
		}

		return -1;
	}

	return 0;
}

int Process::setRealGroupID(gid_t gid)
{
	int res = setregid(gid,-1);

	if(res == -1)
	{
		if(errno == EINVAL)
		{
			printf("ERROR:setRealGroupID invalid GID.\n");
		}

		if(errno == EPERM)
		{
			printf("ERROR:setRealGroupID insufficient permissions.\n");
		}

		return -1;
	}

	return 0;
}

int Process::setSupplementaryGroupIDs(const std::vector<gid_t>& gid)
{
	gid_t gids[gid.size()];

	int res = setgroups(gid.size(),gids);

	if(res == -1)
	{
		if(errno == EINVAL)
		{
			printf("ERROR:setSupplementaryGroupIDs invalid GID.\n");
		}

		if(errno == EPERM)
		{
			printf("ERROR:setSupplementaryGroupIDs insufficient permissions.\n");
		}

		return -1;
	}

	return 0;
}


void Process::updateTimeValues()
{
	if(m_pid == 0)
	{
		printf("ERROR: invalid PID\n");
		return;
	}

	static char statfile[64];
	sprintf(statfile, "/proc/%d/stat", m_pid);

	FILE* fp;
	char buffer[32000];
	size_t bytes_read;
	/* Read the entire contents of /proc/pid/stat into the buffer. */
	fp = fopen (statfile, "r");
	bytes_read = fread (buffer, 1, sizeof (buffer), fp);
	fclose (fp);

	//get the current time and calculate delta t
	FILL_TICK(&m_time_now);
	TimeToolbox::tickDiff(&m_time_last, &m_time_now,&m_time_diff);
	//printf("------------  ns %lu s %lu\n",m_time_diff.tv_nsec,m_time_diff.tv_sec);
	unsigned long dt = m_time_diff.tv_nsec + m_time_diff.tv_sec*1000000000 ;

	/* Bail if read failed or if buffer is not big enough.
	 * Since we allocate considerable amount of memory, this clause should only
	 * be entered if the process does not exist.
	 * */
	if (bytes_read == 0 || bytes_read == sizeof (buffer))
	{
		printf("ERROR: could not read the process information\n");

		//this ensures that the consumption will decline over time (we assume that the process has exited)
		m_cpu_usage_perc = (1.0-m_averaging_coefficient)*m_cpu_usage_perc + m_averaging_coefficient * 0;

		return;
	}

	/* NUL-terminate the text. */
	buffer[bytes_read] = '\0';

	std::string s(buffer);

	//since the file contains a single line with spaces as separators; split it simply!
	FastNCSList<std::string>* tokens = Toolbox::splitStringF(s, ' ');
	//check compatibility
	if(tokens->getSize() != 52)
	{
		printf("ERROR: file %s does not contain the expected format %lu of 52 args found\n",statfile,tokens->getSize());
	}

	FastNCSList<std::string>::Iterator it = tokens->getStart();

	//skip to the right section
	++it;++it;++it;++it;++it;++it;++it;++it;++it;++it;
	 ++it;++it;++it;

	m_utime = Toolbox::stringToULongLong( it.getElement()->getm_data() ); ++it;
	m_stime = Toolbox::stringToULongLong( it.getElement()->getm_data() ); ++it;
	m_cutime = Toolbox::stringToULongLong( it.getElement()->getm_data() ); ++it;
	m_cstime = Toolbox::stringToULongLong( it.getElement()->getm_data() );

	//check if the process actually exists (we know it has the right PID but does it have the right start time?)
	++it;++it;++it;++it;++it;
	unsigned long long start_time = Toolbox::stringToULongLong( it.getElement()->getm_data() );
	//check if the start times match
	if(start_time != m_start_time)
	{
		printf("Process::doesExist(): start time mismatch\n");

		//this ensures that the consumption will decline over time (since the process does not exist anymore!)
		m_cpu_usage_perc = (1.0-m_averaging_coefficient)*m_cpu_usage_perc + m_averaging_coefficient * 0;

		//clean up
		delete tokens;
		return ;
	}

	//if it exists:

	//update the stat values
	double new_usage = 0;
	if(m_time_detail == TIME_DETAIL_WITH_CHILDREN)
	{
		new_usage = (double)( (long double)Jiffies::getRealTime( m_utime - m_utime_last + m_stime - m_stime_last +
				m_cutime - m_cutime_last + m_cstime - m_cstime_last) / (long double)dt );
	}
	if(m_time_detail == TIME_DETAIL_WITHOUT_CHILDREN)
	{
		new_usage = (double)( (long double)Jiffies::getRealTime( m_utime - m_utime_last + m_stime - m_stime_last ) / (long double)dt );
	}

	//gliding averaging
	//printf("usage old %f new %f, timediff %lu and jiffies_ns %lu\n",m_cpu_usage_perc,new_usage,dt,Jiffies::getRealTime( m_utime - m_utime_last + m_stime - m_stime_last ));
	m_cpu_usage_perc = (1.0-m_averaging_coefficient)*m_cpu_usage_perc + m_averaging_coefficient * new_usage;

	//clean up
	delete tokens;

	//set new reference point
	m_time_last = m_time_now;

	m_utime_last = m_utime;
	m_stime_last = m_stime;
	m_cutime_last = m_cutime;
	m_cstime_last = m_cutime;
}

/**
 * One could use fscanf instead of the tokenizer approach and most likely save a half the useconds,
 * yet this approach was chosen for the sake of readability.
 * */
void Process::updatePID(pid_t pid, enum TIME_DETAIL timed)
{

	m_time_detail = timed;

	static char statfile[64];
	sprintf(statfile, "/proc/%d/stat", pid);

	FILE* fp;
	char buffer[32000];
	size_t bytes_read;
	/* Read the entire contents of /proc/pid/stat into the buffer. */
	fp = fopen(statfile, "r");

	if(fp == NULL)
	{
		printf("ERROR: could not open the process file\n");
		return;
	}

	bytes_read = fread (buffer, 1, sizeof (buffer), fp);
	fclose (fp);

	/* Bail if read failed or if buffer is not big enough. */
	if (bytes_read == 0 || bytes_read == sizeof (buffer))
	{
		printf("ERROR: could not read the process information\n");
		return;
	}

	/* NUL-terminate the text. */
	buffer[bytes_read] = '\0';

	std::string s(buffer);

	//since the file contains a single line with spaces as separators; split it simply!
	FastNCSList<std::string>* tokens = Toolbox::splitStringF(s, ' ');
	//check compatibility
	if(tokens->getSize() < 52)
	{
		printf("ERROR: file %s does not contain the expected format %lu of at least 52 args found\n",statfile,tokens->getSize());
	}

	FastNCSList<std::string>::Iterator it = tokens->getStart();

	m_pid = (pid_t)Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_tcomm = it.getElement()->getm_data_ref(); ++it;
	m_state = it.getElement()->getm_data_ref().c_str()[0]; ++it;
	m_ppid = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_pgrp = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_sid = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_tty_nr = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_tty_pgrp = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_flags = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_min_flt = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_cmin_flt = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_maj_flt = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_cmaj_flt = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_utime = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_stime = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_cutime = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_cstime = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_priority = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_nice = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_num_threads = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_it_real_value = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_start_time = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_vsize = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_rss = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_rsslim = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_start_code = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_end_code = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_start_stack = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_esp = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_eip = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_pending = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_blocked = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_sigign = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_sigcatch = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_wchan = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;++it;++it;
	m_exit_signal = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_task_cpu = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_rt_priority = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_policy = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_blkio_ticks = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_gtime = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_cgtime = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_start_data = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_end_data = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_start_brk = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_arg_start = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_arg_end = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_env_start = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_env_end = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() ); ++it;
	m_exit_code = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() );

	//clean up
	delete tokens;

	//--------------------------------- additional memory information -----------------------
	sprintf(statfile, "/proc/%d/status", pid);

	/* Read the entire contents of /proc/pid/status into the buffer. */
	fp = fopen(statfile, "r");

	if(fp == NULL)
	{
		printf("ERROR: could not open the process file\n");
		return;
	}

	bytes_read = fread (buffer, 1, sizeof (buffer), fp);
	fclose (fp);

	/* Bail if read failed or if buffer is not big enough. */
	if (bytes_read == 0 || bytes_read == sizeof (buffer))
	{
		printf("ERROR: could not read the process information\n");
		return;
	}

	/* NUL-terminate the text. */
	buffer[bytes_read] = '\0';

	s = buffer;
	tokens = Toolbox::splitStringF(s, '\n');
	//check compatibility
	/*if(tokens->getSize() < 51)
	{
		printf("ERROR: file %s does not contain the expected format %lu of at least 51 args found\n",statfile,tokens->getSize());
	}*/

	//iterate over all elements in the splitted line
	Lazarus::GenNCSListElement<std::string>* element = NULL;
	for(it = tokens->getStart();it != tokens->getEnd();++it)
	{
		//get the string and split it into two parts
		Lazarus::FastNCSList<std::string>*  segs = Toolbox::splitStringFSegment(it.getElement()->getm_data_ref(),':');
		if(segs->getFirst()->getm_data_ref().compare("Umask")==0)
			m_umask = segs->getFirst()->getmp_neighbor()->getm_data_ref();
		else if(segs->getFirst()->getm_data_ref().compare("Ngid")==0)
			m_ngid = Toolbox::stringToULongLong( segs->getFirst()->getmp_neighbor()->getm_data_ref() );
		else if(segs->getFirst()->getm_data_ref().compare("TracerPid")==0)
			m_tracerpid = Toolbox::stringToULongLong( segs->getFirst()->getmp_neighbor()->getm_data_ref() );
		else if(segs->getFirst()->getm_data_ref().compare("Uid")==0)
		{
			Lazarus::FastNCSList<std::string>*  segs2 = Toolbox::splitStringFSegment(segs->getFirst()->getmp_neighbor()->getm_data_ref(),'\t');
			element = segs2->getFirst();
			m_uid_real = Toolbox::stringToULongLong(element->getm_data_ref() );
			element = element->getmp_neighbor();
			m_uid_effective = Toolbox::stringToULongLong( element->getm_data_ref() );
			element = element->getmp_neighbor();
			m_uid_saved = Toolbox::stringToULongLong( element->getm_data_ref() );
			element = element->getmp_neighbor();
			m_uid_filesystem = Toolbox::stringToULongLong( element->getm_data_ref() );

			delete segs2;
		}
		else if(segs->getFirst()->getm_data_ref().compare("Gid")==0)
		{
			Lazarus::FastNCSList<std::string>*  segs2 = Toolbox::splitStringFSegment(segs->getFirst()->getmp_neighbor()->getm_data_ref(),'\t');
			element = segs2->getFirst();
			m_gid_real = Toolbox::stringToULongLong( element->getm_data_ref() );
			element = element->getmp_neighbor();
			m_gid_effective = Toolbox::stringToULongLong( element->getm_data_ref() );
			element = element->getmp_neighbor();
			m_gid_saved = Toolbox::stringToULongLong( element->getm_data_ref() );
			element = element->getmp_neighbor();
			m_gid_filesystem = Toolbox::stringToULongLong( element->getm_data_ref() );
			delete segs2;
		}
		else if(segs->getFirst()->getm_data_ref().compare("FDSize")==0)
			m_fdsize = Toolbox::stringToULongLong( segs->getFirst()->getmp_neighbor()->getm_data_ref() );
		else if(segs->getFirst()->getm_data_ref().compare("VmPeak")==0)
			m_vmpeak = Toolbox::stringToULongLong( segs->getFirst()->getmp_neighbor()->getm_data_ref() );
		else if(segs->getFirst()->getm_data_ref().compare("VmSize")==0)
			m_vmsize = Toolbox::stringToULongLong( segs->getFirst()->getmp_neighbor()->getm_data_ref() );
		else if(segs->getFirst()->getm_data_ref().compare("VmLck")==0)
			m_vmlck = Toolbox::stringToULongLong( segs->getFirst()->getmp_neighbor()->getm_data_ref() );
		else if(segs->getFirst()->getm_data_ref().compare("VmPin")==0)
			m_vmpin = Toolbox::stringToULongLong( segs->getFirst()->getmp_neighbor()->getm_data_ref() );
		else if(segs->getFirst()->getm_data_ref().compare("VmHWM")==0)
			m_vmhwm = Toolbox::stringToULongLong( segs->getFirst()->getmp_neighbor()->getm_data_ref() );
		else if(segs->getFirst()->getm_data_ref().compare("VmRSS")==0)
			m_vmrss = Toolbox::stringToULongLong( segs->getFirst()->getmp_neighbor()->getm_data_ref() );
		else if(segs->getFirst()->getm_data_ref().compare("RssAnon")==0)
			m_rssanon = Toolbox::stringToULongLong( segs->getFirst()->getmp_neighbor()->getm_data_ref() );
		else if(segs->getFirst()->getm_data_ref().compare("RssFile")==0)
			m_rssfile = Toolbox::stringToULongLong( segs->getFirst()->getmp_neighbor()->getm_data_ref() );
		else if(segs->getFirst()->getm_data_ref().compare("RssShmem")==0)
			m_rssshmem = Toolbox::stringToULongLong( segs->getFirst()->getmp_neighbor()->getm_data_ref() );
		else if(segs->getFirst()->getm_data_ref().compare("VmData")==0)
			m_vmdata = Toolbox::stringToULongLong( segs->getFirst()->getmp_neighbor()->getm_data_ref() );
		else if(segs->getFirst()->getm_data_ref().compare("VmStk")==0)
			m_vmstk = Toolbox::stringToULongLong( segs->getFirst()->getmp_neighbor()->getm_data_ref() );
		else if(segs->getFirst()->getm_data_ref().compare("VmExe")==0)
			m_vmexe = Toolbox::stringToULongLong( segs->getFirst()->getmp_neighbor()->getm_data_ref() );
		else if(segs->getFirst()->getm_data_ref().compare("VmLib")==0)
			m_vmlib = Toolbox::stringToULongLong( segs->getFirst()->getmp_neighbor()->getm_data_ref() );
		else if(segs->getFirst()->getm_data_ref().compare("VmPTE")==0)
			m_vmpte = Toolbox::stringToULongLong( segs->getFirst()->getmp_neighbor()->getm_data_ref() );
		else if(segs->getFirst()->getm_data_ref().compare("VmPMD")==0)
			m_vmpmd = Toolbox::stringToULongLong( segs->getFirst()->getmp_neighbor()->getm_data_ref() );
		else if(segs->getFirst()->getm_data_ref().compare("VmSwap")==0)
			m_vmswap = Toolbox::stringToULongLong( segs->getFirst()->getmp_neighbor()->getm_data_ref() );
		else if(segs->getFirst()->getm_data_ref().compare("HugetlbPages")==0)
			m_hugetlbpages = Toolbox::stringToULongLong( segs->getFirst()->getmp_neighbor()->getm_data_ref() );
		else if(segs->getFirst()->getm_data_ref().compare("SigQ")==0)
		{
			Lazarus::FastNCSList<std::string>*  segs2 = Toolbox::splitStringFSegment(segs->getFirst()->getmp_neighbor()->getm_data_ref(),'/');
			element = segs2->getFirst();
			m_sigq_current = (pid_t)Toolbox::stringToULongLong( element->getm_data_ref() );
			element = element->getmp_neighbor();
			m_sigq_limit = (pid_t)Toolbox::stringToULongLong( element->getm_data_ref() );
			delete segs2;
		}
		else if(segs->getFirst()->getm_data_ref().compare("CapInh")==0)
			m_capinh =  segs->getFirst()->getmp_neighbor()->getm_data_ref();
		else if(segs->getFirst()->getm_data_ref().compare("CapPrm")==0)
			m_capprm = segs->getFirst()->getmp_neighbor()->getm_data_ref();
		else if(segs->getFirst()->getm_data_ref().compare("CapEff")==0)
			m_capeff = segs->getFirst()->getmp_neighbor()->getm_data_ref();
		else if(segs->getFirst()->getm_data_ref().compare("CapBnd")==0)
			m_capbnd = segs->getFirst()->getmp_neighbor()->getm_data_ref();
		else if(segs->getFirst()->getm_data_ref().compare("CapAmb")==0)
			m_capamb = segs->getFirst()->getmp_neighbor()->getm_data_ref();
		else if(segs->getFirst()->getm_data_ref().compare("voluntary_ctxt_switches")==0)
			m_voluntary_ctxt_switches = Toolbox::stringToULongLong( segs->getFirst()->getmp_neighbor()->getm_data_ref() );
		else if(segs->getFirst()->getm_data_ref().compare("nonvoluntary_ctxt_switches")==0)
			m_nonvoluntary_ctxt_switches = Toolbox::stringToULongLong( segs->getFirst()->getmp_neighbor()->getm_data_ref() );

		delete segs;
		continue;
	}

	//clean up
	delete tokens;

	//get the first point of reference in time
	FILL_TICK(&m_time_last);

	//init the stat values
	m_averaging_coefficient = default_averaging_coefficient;
	m_cpu_usage_perc = 0;
	m_utime_last = m_utime;
	m_stime_last = m_stime;
	m_cutime_last = m_cutime;
	m_cstime_last = m_cutime;

	//get memory limits
	struct rlimit rlim_old;
	if (prlimit(m_pid, RLIMIT_CPU, NULL, &rlim_old) != -1)
		m_rlimit_cpu = rlim_old.rlim_max;
	else
	{
		printf("ERROR: could not get memory limits\n");
		m_rlimit_cpu = 0;
	}

	if (prlimit(m_pid, RLIMIT_CORE, NULL, &rlim_old) != -1)
		m_rlimit_core = rlim_old.rlim_max;
	else
	{
		printf("ERROR: could not get memory limits\n");
		m_rlimit_core = 0;
	}

	if (prlimit(m_pid, RLIMIT_AS, NULL, &rlim_old) != -1)
		m_rlimit_as = rlim_old.rlim_max;
	else
	{
		printf("ERROR: could not get memory limits\n");
		m_rlimit_as = 0;
	}

	if (prlimit(m_pid, RLIMIT_DATA, NULL, &rlim_old) != -1)
		m_rlimit_data = rlim_old.rlim_max;
	else
	{
		printf("ERROR: could not get memory limits\n");
		m_rlimit_data = 0;
	}

	if (prlimit(m_pid, RLIMIT_STACK, NULL, &rlim_old) != -1)
		m_rlimit_stack = rlim_old.rlim_max;
	else
	{
		printf("ERROR: could not get memory limits\n");
		m_rlimit_stack = 0;
	}
}

int Process::setASLimit(long long limit)
{
	struct rlimit rlim_new;
	rlim_new.rlim_max = limit;
	if (prlimit(m_pid, RLIMIT_AS, &rlim_new, NULL) != -1)
	{
		return 0;
	}
	else
	{
		printf("ERROR: could not set memory limits\n");
		return -1;
	}
}

int Process::setCPULimit(long long limit)
{
	struct rlimit rlim_new;
	rlim_new.rlim_max = limit;
	if (prlimit(m_pid, RLIMIT_CPU, &rlim_new, NULL) != -1)
	{
		return 0;
	}
	else
	{
		printf("ERROR: could not set memory limits\n");
		return -1;
	}
}

int Process::setCoreLimit(long long limit)
{
	struct rlimit rlim_new;
	rlim_new.rlim_max = limit;
	if (prlimit(m_pid, RLIMIT_CORE, &rlim_new, NULL) != -1)
	{
		return 0;
	}
	else
	{
		printf("ERROR: could not set memory limits\n");
		return -1;
	}
}

int Process::setStackLimit(long long limit)
{
	struct rlimit rlim_new;
	rlim_new.rlim_max = limit;
	if (prlimit(m_pid, RLIMIT_STACK, &rlim_new, NULL) != -1)
	{
		return 0;
	}
	else
	{
		printf("ERROR: could not set memory limits\n");
		return -1;
	}
}

int Process::setDataLimit(long long limit)
{
	struct rlimit rlim_new;
	rlim_new.rlim_max = limit;
	if (prlimit(m_pid, RLIMIT_DATA, &rlim_new, NULL) != -1)
	{
		return 0;
	}
	else
	{
		printf("ERROR: could not set memory limits\n");
		return -1;
	}
}


void Process::printProcInfo()
{
	printf("--------------------- Process info ------------------------\n");

	printf(""
		"m_pid: %d \n"
		"m_tcomm %s \n"
		"m_state: %c \n"
		"m_ppid : %d \n"
		"m_pgrp : %llu \n"
		"m_sid : %llu \n"
		"m_tty_nr : %llu \n"
		"m_tty_pgrp : %llu \n"
		"m_flags : %llu \n"
		"m_min_flt : %llu \n"
		"m_cmin_flt : %llu \n"
		"m_maj_flt : %llu \n"
		"m_cmaj_flt : %llu \n"
		"m_utime : %llu \n"
		"m_stime : %llu \n"
		"m_cutime : %llu \n"
		"m_cstime : %llu \n"
		"m_priority : %llu \n"
		"m_nice : %llu \n"
		"m_num_threads : %llu \n"
		"m_it_real_value : %llu \n"
		"m_start_time : %llu \n"
		"m_vsize : %llu \n"
		"m_rss : %llu \n"
		"m_rsslim : %llu \n"
		"m_start_code : %llu \n"
		"m_end_code : %llu \n"
		"m_start_stack : %llu \n"
		"m_esp : %llu \n"
		"m_eip : %llu \n"
		"m_pending : %llu \n"
		"m_blocked : %llu \n"
		"m_sigign : %llu \n"
		"m_sigcatch : %llu \n"
		"m_wchan : %llu \n"
		"m_exit_signal : %llu \n"
		"m_task_cpu : %llu \n"
		"m_rt_priority : %llu \n"
		"m_policy : %llu \n"
		"m_blkio_ticks : %llu \n"
		"m_gtime : %llu \n"
		"m_cgtime : %llu \n"
		"m_start_data : %llu \n"
		"m_end_data : %llu \n"
		"m_start_brk : %llu \n"
		"m_arg_start : %llu \n"
		"m_arg_end : %llu \n"
		"m_env_start : %llu \n"
		"m_env_end : %llu \n"
		"m_exit_code : %llu \n"
		"m_rlimit_as : %lld \n"
		"m_rlimit_core : %lld \n"
		"m_rlimit_cpu : %lld \n"
		"m_rlimit_stack : %lld \n"
		"m_rlimit_data : %lld \n"
		"m_umask: %s\n"
		"m_ngid: %llu\n"
		"m_tracerpid: %llu\n"
		"m_uid_real: %llu\n"
		"m_uid_effective: %llu\n"
		"m_uid_saved: %llu\n"
		"m_uid_filesystem: %llu\n"
		"m_gid_real: %llu\n"
		"m_gid_effective: %llu\n"
		"m_gid_saved: %llu\n"
		"m_gid_filesystem: %llu\n"
		"m_fdsize: %llu\n"
		"m_vmpeak: %llu\n"
		"m_vmsize: %llu\n"
		"m_vmlck: %llu\n"
		"m_vmpin: %llu\n"
		"m_vmhwm: %llu\n"
		"m_vmrss: %llu\n"
		"m_rssanon: %llu\n"
		"m_rssfile: %llu\n"
		"m_rssshmem: %llu\n"
		"m_vmdata: %llu\n"
		"m_vmstk: %llu\n"
		"m_vmexe: %llu\n"
		"m_vmlib: %llu\n"
		"m_vmpte: %llu\n"
		"m_vmpmd: %llu\n"
		"m_vmswap: %llu\n"
		"m_hugetlbpages: %llu\n"
		"m_sigq_current: %llu\n"
		"m_sigq_limit: %llu\n"
		"m_sigpnd: %llu \n"
		"m_shdpnd: %llu\n"
		"m_capinh: %s \n"
		"m_capprm: %s\n"
		"m_capeff: %s\n"
		"m_capbnd: %s\n"
		"m_capamb: %s\n"
		"m_voluntary_ctxt_switches: %llu\n"
		"m_nonvoluntary_ctxt_switches: %llu\n",
			m_pid,
			m_tcomm.c_str(),
			m_state,
			m_ppid,
			m_pgrp,
			m_sid,
			m_tty_nr,
			m_tty_pgrp,
			m_flags,
			m_min_flt,
			m_cmin_flt,
			m_maj_flt,
			m_cmaj_flt,
			m_utime,
			m_stime,
			m_cutime,
			m_cstime,
			m_priority,
			m_nice,
			m_num_threads,
			m_it_real_value,
			m_start_time,
			m_vsize,
			m_rss,
			m_rsslim,
			m_start_code,
			m_end_code,
			m_start_stack,
			m_esp,
			m_eip,
			m_pending,
			m_blocked,
			m_sigign,
			m_sigcatch,
			m_wchan,
			m_exit_signal,
			m_task_cpu,
			m_rt_priority,
			m_policy,
			m_blkio_ticks,
			m_gtime,
			m_cgtime,
			m_start_data,
			m_end_data,
			m_start_brk,
			m_arg_start,
			m_arg_end,
			m_env_start,
			m_env_end,
			m_exit_code,
			m_rlimit_as,
			m_rlimit_core,
			m_rlimit_cpu,
			m_rlimit_stack,
			m_rlimit_data,

			m_umask.c_str(),
			m_ngid,
			m_tracerpid,
			m_uid_real,
			m_uid_effective,
			m_uid_saved,
			m_uid_filesystem,
			m_gid_real,
			m_gid_effective,
			m_gid_saved,
			m_gid_filesystem,
			m_fdsize,
			m_vmpeak,
			m_vmsize,
			m_vmlck,
			m_vmpin,
			m_vmhwm,
			m_vmrss,
			m_rssanon,
			m_rssfile,
			m_rssshmem,
			m_vmdata,
			m_vmstk,
			m_vmexe,
			m_vmlib,
			m_vmpte,
			m_vmpmd,
			m_vmswap,
			m_hugetlbpages,
			m_sigq_current,
			m_sigq_limit,
			m_sigpnd,
			m_shdpnd,
			m_capinh.c_str(),
			m_capprm.c_str(),
			m_capeff.c_str(),
			m_capbnd.c_str(),
			m_capamb.c_str(),
			m_voluntary_ctxt_switches,
			m_nonvoluntary_ctxt_switches );

	printf("-----------------------------------------------------------\n");
}


int Process::killProcess()
{
	int res = kill(m_pid, SIGKILL);

	if(res != 0)
	{
		return -1;
	}

	return 0;
}

bool Process::doesExist()
{
	static char statfile[64];
	sprintf(statfile, "/proc/%d/stat", m_pid);

	//check existence
	if(Lazarus::Toolbox::checkFileExistence(std::string(statfile)) == false)
	{
		printf("Process::doesExist(): stat file not found\n");
		return false;
	}

	//if it exists, check the name and creation date
	FILE* fp;
	char buffer[32000];
	size_t bytes_read;
	/* Read the entire contents of /proc/pid/stat into the buffer. */
	fp = fopen (statfile, "r");
	bytes_read = fread (buffer, 1, sizeof (buffer), fp);
	fclose (fp);

	/* Bail if read failed or if buffer is not big enough. */
	if (bytes_read == 0 || bytes_read == sizeof (buffer))
	{
		printf("ERROR: could not read the process information\n");
		return false;
	}

	/* NUL-terminate the text. */
	buffer[bytes_read] = '\0';

	std::string s(buffer);

	//since the file contains a single line with spaces as separators; split it simply!
	FastNCSList<std::string>* tokens = Toolbox::splitStringF(s, ' ');
	//check compatibility
	if(tokens->getSize() != 52)
	{
		printf("ERROR: file %s does not contain the expected format %lu of 52 args found\n",statfile,tokens->getSize());
	}

	FastNCSList<std::string>::Iterator it = tokens->getStart();

	++it;
	std::string tcomm = it.getElement()->getm_data(); ++it;

	/* THIS IS UNRELIABLE:
	 * if a spawned process replaces itself with another application, its name will change, yet
	 * this happens asynchronously, the parent process might not register the new application name.
	 * Thus, a name check would require a synchronization during the spawn process.
	 *  */
	//check if the filenames match
	/*if(tcomm.compare(m_tcomm) != 0)
	{
		printf("Process::doesExist(): filename mismatch %s/%s \n",tcomm.c_str(),m_tcomm.c_str());

		//clean up
		delete tokens;
		return false;
	}*/

	//do a final check for the creation date

	++it;
	++it;
	++it;
	++it;
	++it;
	++it;
	++it;
	++it;
	++it;
	++it;
	++it;
	++it;
	++it;
	++it;
	++it;
	++it;
	++it;
	++it;
	++it;
	unsigned long long start_time = Toolbox::stringToULongLong( it.getElement()->getm_data_ref() );

	//check if the start times match
	if(start_time != m_start_time)
	{
		printf("Process::doesExist(): start time mismatch\n");
		//clean up
		delete tokens;
		return false;
	}

	//clean up
	delete tokens;

	return true;

}

int Process::stopProcess()
{
	int res = kill(m_pid, SIGSTOP);

	if(res != 0)
	{
		return -1;
	}

	return 0;
}


int Process::resumeProcess()
{
	int res = kill(m_pid, SIGCONT);

	if(res != 0)
	{
		return -1;
	}

	return 0;
}

int Process::interruptProcess()
{
	int res = kill(m_pid, SIGINT);

	if(res != 0)
	{
		return -1;
	}

	return 0;
}

int Process::setAffinity(FastKTuple<unsigned int> &cpus)
{
	cpu_set_t mask;
	CPU_ZERO(&mask);

	for(unsigned int i=0;i<cpus.getm_size();++i)
	{
		CPU_SET( cpus.getElement(i) , &mask);
	}

	int result = sched_setaffinity(m_pid, sizeof(mask), &mask);

	if(result != 0)
	{
		return -1;
	}
	return 0;
}

int Process::setPriority(int prio)
{
	int res = setpriority(PRIO_PROCESS, m_pid, -20);

	if(res != 0)
	{
		printf("ERROR: could not set process priority\n");
	}

	return res;
}

Process* Process::getSelf()
{
	Process* p = new Process(getpid());

	return p;
}


void Process::printCPUTime()
{
	printf("Process consumed %llu jiffies or %llu ns or %llu us, a total %f cpu time\n",m_utime+m_stime,Jiffies::getRealTime(m_utime+m_stime),
			Jiffies::getRealTime(m_utime+m_stime)/1000ul,m_cpu_usage_perc);
}


} /* namespace Lazarus */
