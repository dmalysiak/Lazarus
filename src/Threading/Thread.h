/*
 * Thread.h
 *
 *  Created on: 24.01.2013
 *      Author: Darius Malysiak
 */

#ifndef THREAD_H_
#define THREAD_H_

#include "../BaseObject.h"
#include "../DataStructures/FastKTuple.h"

#include <atomic>

#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>

#include <sys/time.h>
#include <sys/resource.h>

/**
 * These macros should only be used for quick and dirty hacks! Never attempt to use them in different contexts
 * for the same unique_const_name!!
 */
#define QUICK_THREAD_START(unique_const_name) struct unique_const_name##_struct{ \
		static void* unique_const_name##_function(void* parameter__) {\
			pid_t tid = syscall(SYS_gettid);

#define QUICK_THREAD_END(unique_const_name,parameter) } \
	pthread_attr_t unique_const_name##_thread_attr__; \
	pthread_t unique_const_name##_system_thread_id__; \
	}unique_const_name##_struct_instance;\
	pthread_attr_init(&unique_const_name##_struct_instance.unique_const_name##_thread_attr__); \
	pthread_attr_setdetachstate(&unique_const_name##_struct_instance.unique_const_name##_thread_attr__,\
								PTHREAD_CREATE_JOINABLE); \
	pthread_create(&unique_const_name##_struct_instance.unique_const_name##_system_thread_id__, \
				   &unique_const_name##_struct_instance.unique_const_name##_thread_attr__, \
				   &unique_const_name##_struct::unique_const_name##_function, parameter);

#define QUICK_THREAD_JOIN(unique_const_name) pthread_join(unique_const_name##_struct_instance.unique_const_name##_system_thread_id__, NULL); \
	pthread_attr_destroy(&unique_const_name##_struct_instance.unique_const_name##_thread_attr__);

#define QUICK_THREAD_GET_ID tid

/**
 * Use this macro in order to exit the thread within the while loop.
 * */
#define SELF_TERMINATE this->m_thread_self_terminated=true;this->m_shutdown_flag=true;return;

namespace Lazarus
{

/**
 * A simple 'recipe'(or tips) for coding safe thread classes:
 * - Always call 'shutdown()' as the >first< operation in your derived class's destructor, otherwise the system thread
 * won't terminate (except you have called 'shutdown' yourself before deleting the object).
 * - If you use a loop inside the run function, always control its life with the 'm_shutdown_flag' member, e.g.
 * 'while(m_shutdown_flag==false){}'
 * - If the loop is exited from within due to some special events, always set 'm_thread_self_terminated=true' (use SELF_TERMINATE macro in order to exit swiftly from within).
 * - If a simple setting of 'm_shutdown_flag=true' does not ensure the loop exit, override 'preFlag()' or 'postFlag'.
 * These methods will be executed by an external caller before and after setting 'm_shutdown_flag=true', respectively.
 * - Never manage a Thread object with multiple threads, i.e. a Thread object itself is not thread safe.
 * - Don't override 'run' more than once! (Otherwise you should really know what you're doing by studying this class's
 * source in detail).
 * - Don't manipulate any Thread class members from your derived class!!
 *
 * All threads are joinable, thus if one starts a thread an forgets to join it, it will continue to run/exist and still
 * occupy (although terminated) the assigned system resources (e.g. on RHEL system 10MB will be assigned for each created POSIX thread).
 *
 *                                             !!! Always join threads!!!!!
 */

/**
 * A thread instance can always be restarted once it has shut down.
 * NOTE: always use the member variable 'm_shutdown_flag' to control the life of your 'run()' implementation,
 * it is crucial that by setting 'm_shutdown_flag' to false one can bring the thread execution to a "swift" hold.
 * If the loop-exit in a 'run()' implementation can not be controlled by a simple boolean value, simply
 * override 'preFlag()' or 'postFlag()'!
 * Don't kill a thread by doing some fancy stuff in the derived class's destructor to exit the run-loop. Yet it's almost
 * fine to exit the run-loop from within (it's even not necessary to set 'm_shutdown_flag' during this exit)! BUT: an
 * internal termination must be indicated by setting 'm_thread_self_terminated = true'  (use SELF_TERMINATE macro in order to exit swiftly from within)!!!
 * Also keep in mind that a thread object itself is not thread safe, i.e. do not manage a thread with multiple threads!
 */
class Thread : public BaseObject{
public:
	Thread(int threadpool_id = -1);
	Thread(const Thread& t);

	/**
	* Use 'shutdown'/'shutdownJoin' before deleting a running thread!
	* If not, this destructor won't stop a running thread! (otherwise the system consistency would be broken as
	* 'preFlag'/'postFlag' would be called after a child classes destructor, this would make both of these overridden
	* methods useless!).
	* A simple way to ensure this is by calling 'shutdown()' at the >beginning< of a child objects destructor.
	*/
	virtual ~Thread();

	/**
	 * These methods 'shutdown'/'shutdownJoin' will shutdown the thread and block until the calling thread successfully joined
	 * the dying thread!
	 * Both methods do the same and are provided for compatibility reasons.
	 * Always, and I mean ALWAYS, use 'm_shutdown_flag' to control the flow in your 'run()' implementation.
	 * The clean deletion of a thread, in terms of system resources while the process exists, depends on a
	 * Thread-object's capability to unquestionably stop the 'run()' method by setting 'm_shutdown_flag' to
	 * false!
	 * These methods may exhibit a small latency as they will wait for the system thread to successfully start up.
	 */
	void shutdown();

	/**
	 * These methods 'shutdown'/'shutdownJoin' will shutdown the thread and block until the calling thread successfully joined
	 * the dying thread!
	 * Both methods do the same and are provided for compatibility reasons.
	 * Always, and I mean ALWAYS, use 'm_shutdown_flag' to control the flow in your 'run()' implementation.
	 * The clean deletion of a thread, in terms of system resources while the process exists, depends on a
	 * Thread-object's capability to unquestionably stop the 'run()' method by setting 'm_shutdown_flag' to
	 * false!
	 * These methods may exhibit a small latency as they will wait for the system thread to successfully start up.
	 */
	void shutdownJoin();

	void start(unsigned int local_thread_id=0);

	virtual void run() = 0;

	/**
	 * Anything which should must be executed before setting 'm_shutdown_flag' to false, must be implemented in this method.
	 * This method will be called by 'shutdown'/'shutdownJoin' just before setting 'm_shutdown_flag' to false.
	 * With these methods it should be no problem to ensure that a thread can be stopped by simply setting the flag
	 * variable.
	 */
	virtual void preFlag();

	/**
	 * Anything which should must be executed after setting 'm_shutdown_flag' to false, must be implemented in this method.
	 * This method will be called by 'shutdown'/'shutdownJoin' right after setting 'm_shutdown_flag' to false.
	 * With these methods it should be no problem to ensure that a thread can be stopped by simply setting the flag
	 * variable.
	 */
	virtual void postFlag();

	pthread_t getm_system_thread_id() const;

	unsigned int getm_local_thread_id() const;

	/**
	 * Sets the affinity to the specified CPUs, i.e. allows the thread to be run only on the given CPUs.
	 * The maximal amount of CPUs is limited by CPU_SETSIZE.
	 * */
	void setAffinity(FastKTuple<unsigned int>& cpus);

	/**
	 * Gets the affinity of the thread, i.e. returns a list of CPUs onto which the thread is allowed to run.
	 * The maximal amount of CPUs is limited by CPU_SETSIZE.
	 * */
	FastKTuple<unsigned int> getAffinity();

	/**
	 * This method does what the name says; calling this method will try to join the running or just starting thread.
	 * Beware: calling this method without starting the thread beforehand, will result in a deadlock for the calling thread!
	 */
	void join();

	/**
	 * Prints some short information about the thread.
	 */
	void printThreadInfo() const;

	enum THREAD_STATE {THREAD_STATE_RUNNING, THREAD_STATE_IDLE, THREAD_STATE_STOPPED, THREAD_STATE_UNINITIALIZED};

	CLASS_RTTI(Lazarus::Thread)

	/**
	 * Never evar call this method or dire things may happen!!
	 */
	void run_();

	/**
	 * Never evar call this method or dire things may happen!!
	 */
	static void* runFromStaticContext(void* thread_params);

	//integral thread ID, i.e. no opaque datatype problems!!
	pid_t m_tid;

protected:
	unsigned int m_local_thread_id;
	pthread_t m_system_thread_id;
	enum THREAD_STATE m_thread_state;
	std::atomic_bool m_shutdown_flag;
	std::atomic_bool m_thread_active;
	std::atomic_bool m_thread_self_terminated;
	bool m_threadpool_thread;
	int m_parent_thread_pool_id;
	pthread_attr_t m_thread_attr;

	//this mutex will be used for the join-method.
	pthread_mutex_t m_mutex;

	/**
	 * thread resources.
	 *
	 * ru_utime
     *         This is the total amount of time spent executing in user mode,
     *         expressed in a timeval structure (seconds plus microseconds).
     *
     *  ru_stime
     *         This is the total amount of time spent executing in kernel
     *         mode, expressed in a timeval structure (seconds plus
     *         microseconds).
	 *
     *  ru_maxrss (since Linux 2.6.32)
     *         This is the maximum resident set size used (in kilobytes).
     *         For RUSAGE_CHILDREN, this is the resident set size of the
     *         largest child, not the maximum resident set size of the
     *         process tree.
	 *
     *  ru_ixrss (unmaintained)
     *         This field is currently unused on Linux.
	 *
     *  ru_idrss (unmaintained)
     *         This field is currently unused on Linux.
	 *
     *  ru_isrss (unmaintained)
     *         This field is currently unused on Linux.
	 *
     *  ru_minflt
     *         The number of page faults serviced without any I/O activity;
     *         here I/O activity is avoided by reclaiming a page frame from
     *         the list of pages awaiting reallocation.
	 *
     *  ru_majflt
     *         The number of page faults serviced that required I/O activity.
	 *
     *  ru_nswap (unmaintained)
     *         This field is currently unused on Linux.
	 *
     *  ru_inblock (since Linux 2.6.22)
     *         The number of times the filesystem had to perform input.
 	 *
     *  ru_oublock (since Linux 2.6.22)
     *         The number of times the filesystem had to perform output.
 	 *
     *  ru_msgsnd (unmaintained)
     *         This field is currently unused on Linux.
 	 *
     *  ru_msgrcv (unmaintained)
     *         This field is currently unused on Linux.
	 *
     *  ru_nsignals (unmaintained)
     *         This field is currently unused on Linux.
	 *
     *  ru_nvcsw (since Linux 2.6)
     *         The number of times a context switch resulted due to a process
     *         voluntarily giving up the processor before its time slice was
     *         completed (usually to await availability of a resource).
	 *
     *  ru_nivcsw (since Linux 2.6)
     *         The number of times a context switch resulted due to a higher
     *         priority process becoming runnable or because the current
     *         process exceeded its time slice.
	 *
	 *
	 * */
	struct rusage m_thread_resources;

	void updateResourceInfo();

};

struct ThreadParameterContainer
{
	unsigned int local_thread_id;
	Thread* thread_object;
	pthread_t* system_thread_id;
};

struct ThreadParameterContainer* create_C_struct_ThreadParameterContainer();

}

#endif /* THREAD_H_ */
