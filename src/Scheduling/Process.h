/*
 * Process.h
 *
 *  Created on: Jul 3, 2014
 *      Author: clustro
 */

#ifndef PROCESS_H_
#define PROCESS_H_

#include "../BaseObject.h"
#include "../DataStructures/FastKTuple.h"
#include "../System/SystemUser.h"
#include "../Time/TimeToolbox.h"

#include <vector>

#include <string>
#include <sys/time.h>
#include <sched.h>
#include <sys/resource.h>
#include <grp.h>

namespace Lazarus {

/**
 * This class requires a Linux kernel >= 3.5 !!
 * */

class Process: public Lazarus::BaseObject {
public:

	const double default_averaging_coefficient = 0.1;

	enum TIME_DETAIL{TIME_DETAIL_WITH_CHILDREN, TIME_DETAIL_WITHOUT_CHILDREN};

	/**
	 * once the object is created it will read all process information from /proc/pid/stat.
	 * 'timed' determines if also the cpu usage of all child process should be considered when
	 * calculating the cpu usage of the actual process.
	 * Keep in mind that the Linux kernel does not distinguish between tid and pid, yet the pid will refer
	 * to a group of threads or the 'parent'. In case of only one thread the pid will equal the tid.
	 * Thus one can attach a thread
	 */
	Process(pid_t pid, enum TIME_DETAIL timed = TIME_DETAIL_WITHOUT_CHILDREN);
	Process();

	/**
	 * Deleting a process will not kill the corresponding system process!
	 * */
	virtual ~Process();

	/**
	 * This updates the jiffies values. Note that only a call to this method will update
	 * the averaging 'm_cpu_usage_perc' time values!
	 */
	void updateTimeValues();

	/**
	 * This method will update all fields in the process object.
	 * */
	void update();

	/**
	 * This method effectively associates the object with another process or just updates the
	 * data for the current process (also useful for preallocated process
	 * objects via default constructor).
	 */
	void updatePID(pid_t pid, enum TIME_DETAIL timed = TIME_DETAIL_WITHOUT_CHILDREN);

	/**
	 * This function requires kill-privileges; delivers a hard uncatchable kill.
	 * Returns 0 in case of success, -1 otherwise!
	 */
	int killProcess();

	/**
	 * This function requires kill-privileges; stops a process.
	 * Returns 0 in case of success, -1 otherwise!
	 */
	int stopProcess();

	/**
	 * This function requires kill-privileges; resumes a stopped process.
	 * Returns 0 in case of success, -1 otherwise!
	 */
	int resumeProcess();

	/**
	 * This function requires kill-privileges; delivers a ctrl+c.
	 * Returns 0 in case of success, -1 otherwise!
	 */
	int interruptProcess();

	/**
	 * Prints all process attributes to the CLI.
	 */
	void printProcInfo();

	/**
	 * Prints information about the processes CPU time.
	 */
	void printCPUTime();

	/**
	 * Sets the affinity of the process, returns -1 in case of errors, 0 otherwise.
	 * This method requires root privileges!
	 */
	int setAffinity(FastKTuple<unsigned int> &cpus);

	/**
	 * Sets the process priority, returns -1 in case of errors, 0 otherwise.
	 * This method requires root privileges!
	 * range of prio is [-20,19] with -20 being the highest priority!
	 */
	int setPriority(int prio);

	/**
	 * Creates an instance to itself
	 */
	static Process* getSelf();


	/**
	 * Returns a UID corresponding to the user under which the current process runs.
	 * In other words the effective UID.
	 * */
	static uid_t getEffectiveUserID();

	/**
	 * Returns a UID corresponding to the user under which the current process runs.
	 * In other words the real UID.
	 * */
	static uid_t getRealUserID();

	/**
	 * Returns a group ID corresponding to the group under which the current process runs.
	 * In other words the user according to the processes effective GID.
	 * */
	static gid_t getEffectiveGroupID();

	/**
	 * Returns a group ID corresponding to the group under which the current process was started.
	 * In other words the user according to the processes real GID.
	 * */
	static gid_t getRealGroupID();

	/**
	 * Returns a vector which contains the supplementary GID for the current process.
	 * */
	static std::vector<gid_t> getSupplementaryGroups();


	/**
	 * Sets the effective user ID for the current process, returns 0 on success, -1 otherwise.
	 * */
	static int setEffectiveUserID(uid_t uid);

	/**
	 * Sets the real user ID for the current process, returns 0 on success, -1 otherwise.
	 * */
	static int setRealUserID(uid_t uid);

	/**
	 * Sets the effective user GID for the current process, returns 0 on success, -1 otherwise.
	 * */
	static int setEffectiveGroupID(gid_t gid);

	/**
	 * Sets the real user GID for the current process, returns 0 on success, -1 otherwise.
	 * */
	static int setRealGroupID(gid_t gid);

	/**
	 * Sets the supplementary GIDs for the current process, returns 0 on success, -1 otherwise.
	 * */
	static int setSupplementaryGroupIDs(const std::vector<gid_t>& gid);

	/**
	 * Checks if the process is still running. Returns false if the process does not exist anymore.
	 * */
	bool doesExist();

	/**
	 * Sets the corresponding memory / time limits.
	 * */
	int setASLimit(long long limit);
	int setCoreLimit(long long limit);
	int setCPULimit(long long limit);
	int setDataLimit(long long limit);
	int setStackLimit(long long limit);

	double m_cpu_usage_perc;

	pid_t m_pid; //process id
	std::string m_tcomm;         //filename of the executable
	unsigned char m_state;         /*state (R is running, S is sleeping, D is sleeping in an
				uninterruptible wait, Z is zombie, T is traced or stopped)*/
	pid_t m_ppid;          //process id of the parent process
	unsigned long long m_pgrp;          //pgrp of the process
	unsigned long long m_sid;           //session id
	unsigned long long m_tty_nr;        //tty the process uses
	unsigned long long m_tty_pgrp;      //pgrp of the tty
	unsigned long long m_flags;         //task flags
	unsigned long long m_min_flt;       //number of minor faults
	unsigned long long m_cmin_flt;      //number of minor faults with child's
	unsigned long long m_maj_flt;       //number of major faults
	unsigned long long m_cmaj_flt;      //number of major faults with child's
	unsigned long long m_utime;         //user mode jiffies
	unsigned long long m_stime;         //kernel mode jiffies
	unsigned long long m_cutime;        //user mode jiffies with child's (does not include utime)
	unsigned long long m_cstime;        //kernel mode jiffies with child's (does not include stime)
	unsigned long long m_priority;      //priority level
	unsigned long long m_nice;          //nice level
	unsigned long long m_num_threads;   //number of threads
	unsigned long long m_it_real_value;	//(obsolete, always 0)
	unsigned long long m_start_time;    //time the process started after system boot
	unsigned long long m_vsize;         //virtual memory size
	unsigned long long m_rss;           //resident set memory size in kB (used memory by the process)
	unsigned long long m_rsslim;        //current limit in bytes on the rss
	unsigned long long m_start_code;    //address above which program text can run
	unsigned long long m_end_code;      //address below which program text can run
	unsigned long long m_start_stack;   //address of the start of the main process stack
	unsigned long long m_esp;           //current value of ESP
	unsigned long long m_eip;           //current value of EIP
	unsigned long long m_pending;       //bitmap of pending signals
	unsigned long long m_blocked;       //bitmap of blocked signals
	unsigned long long m_sigign;        //bitmap of ignored signals
	unsigned long long m_sigcatch;      //bitmap of caught signals
	unsigned long long m_wchan;         //address where process went to sleep
	//0             //(place holder)
	//0             //(place holder)
	unsigned long long m_exit_signal;   //signal to send to parent thread on exit
	unsigned long long m_task_cpu;      //which CPU the task is scheduled on
	unsigned long long m_rt_priority;   //realtime priority
	unsigned long long m_policy;        //scheduling policy (man sched_setscheduler)
	unsigned long long m_blkio_ticks;   //time spent waiting for block IO
	unsigned long long m_gtime;         //guest time of the task in jiffies
	unsigned long long m_cgtime;        //guest time of the task children in jiffies
	unsigned long long m_start_data;    //address above which program data+bss is placed
	unsigned long long m_end_data;      //address below which program data+bss is placed
	unsigned long long m_start_brk;     //address above which program heap can be expanded with brk()
	unsigned long long m_arg_start;     //address above which program command line is placed
	unsigned long long m_arg_end;       //address below which program command line is placed
	unsigned long long m_env_start;     //address above which program environment is placed
	unsigned long long m_env_end;       //address below which program environment is placed
	unsigned long long m_exit_code;     //the thread's exit_code in the form reported by the waitpid system call
	long long m_rlimit_as; 	//The maximum size of the process's virtual memory (address space) in bytes.
	long long m_rlimit_core; 	/*Maximum size of a core file (see core(5)).  When 0 no core
										dump files are created.  When nonzero, larger dumps are
										truncated to this size.*/
	long long m_rlimit_cpu;	/*CPU time limit in seconds.  When the process reaches the soft
    									limit, it is sent a SIGXCPU signal.*/
	long long m_rlimit_data;   /*The maximum size of the process's data segment (initialized
										  data, uninitialized data, and heap).  This limit affects calls
										  to brk(2) and sbrk(2), which fail with the error ENOMEM upon
										  encountering the soft limit of this resource.*/
	long long m_rlimit_stack;	/*The maximum size of the process stack, in bytes.  Upon
    									reaching this limit, a SIGSEGV signal is generated.*/


	std::string m_umask; /*Process umask, expressed in octal with a leading zero; see umask(2).  (Since Linux 4.7.)*/

    unsigned long long m_ngid;// NUMA group ID (0 if none; since Linux 3.13).

    unsigned long long m_tracerpid;/* PID of process tracing this process (0 if not being traced).*/

    //Real, effective, saved set, and filesystem UIDs (GIDs).
    unsigned long long m_uid_real,m_uid_effective,m_uid_saved,m_uid_filesystem, m_gid_real,m_gid_effective,m_gid_saved,m_gid_filesystem;

    unsigned long long m_fdsize;// Number of file descriptor slots currently allocated.

    unsigned long long m_vmpeak;// Peak virtual memory size.

	unsigned long long m_vmsize;// Virtual memory size.

	unsigned long long m_vmlck;// Locked memory size (see mlock(3)).

	unsigned long long m_vmpin;/* Pinned memory size (since Linux 3.2).  These are
	pages that can't be moved because something needs to
	directly access physical memory.*/

	unsigned long long m_vmhwm;// Peak resident set size ("high water mark").

	unsigned long long m_vmrss;/* Resident set size.  Note that the value here is the
	sum of RssAnon, RssFile, and RssShmem.*/

	unsigned long long m_rssanon;// Size of resident anonymous memory.  (since Linux 4.5).

	unsigned long long m_rssfile;// Size of resident file mappings.  (since Linux 4.5).

	unsigned long long m_rssshmem;/* Size of resident shared memory (includes System V
	shared memory, mappings from tmpfs, and shared anonymous
	mappings).  (since Linux 4.5).*/

	unsigned long long m_vmdata, m_vmstk, m_vmexe;// Size of data, stack, and text segments.

	unsigned long long m_vmlib;// Shared library code size.

	unsigned long long m_vmpte;// Page table entries size (since Linux 2.6.10).

	unsigned long long m_vmpmd;// Size of second-level page tables (since Linux 4.0).

	unsigned long long m_vmswap;/* Swapped-out virtual memory size by anonymous private
	pages; shmem swap usage is not included (since Linux
	2.6.34).*/

	unsigned long long m_hugetlbpages;// Size of hugetlb memory portions.  (since Linux 4.4).

    unsigned long long m_sigq_current, m_sigq_limit;/* This field contains two slash-separated numbers that
	relate to queued signals for the real user ID of this
	process.  The first of these is the number of currently
	queued signals for this real user ID, and the second is the
	resource limit on the number of queued signals for this
	process (see the description of RLIMIT_SIGPENDING in
	getrlimit(2)).*/

    unsigned long long m_sigpnd, m_shdpnd;/* Number of signals pending for thread and for
	process as a whole (see pthreads(7) and signal(7)).*/

    std::string m_capinh, m_capprm, m_capeff;/* Masks of capabilities enabled in
	inheritable, permitted, and effective sets (see
	capabilities(7)).*/

	std::string m_capbnd;/* Capability Bounding set (since Linux 2.6.26, see
	capabilities(7)).*/

	std::string m_capamb;/* Ambient capability set (since Linux 4.3, see
	capabilities(7)).*/

    unsigned long long m_voluntary_ctxt_switches, m_nonvoluntary_ctxt_switches; /*Number
	of voluntary and involuntary context switches (since Linux
	2.6.23).*/

private:
	enum TIME_DETAIL m_time_detail;

	struct timespec m_time_now;
	struct timespec m_time_last;
	struct timespec m_time_diff;
	double m_averaging_coefficient;

	unsigned long long m_utime_last;
	unsigned long long m_stime_last;
	unsigned long long m_cutime_last;
	unsigned long long m_cstime_last;
};

} /* namespace Lazarus */
#endif /* PROCESS_H_ */
