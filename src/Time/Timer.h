/*
 * Timer.h
 *
 *  Created on: 01.01.2015
 *      Author: Darius Malysiak
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "../BaseObject.h"
#include "../Threading/Task.h"

#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <sys/eventfd.h>

namespace Lazarus {

/**
 * Timers are not thread safe.
 */
class Timer: public Lazarus::BaseObject {
public:
	enum TIMER_TYPE{TIMER_TYPE_SINGLE_SHOT, TIMER_TYPE_PERIODIC, TIMER_TYPE_UNKNOWN};
	enum TIMER_EXECUTION_TYPE{TIMER_EXECUTION_TYPE_BLOCKING, TIMER_EXECUTION_TYPE_NONBLOCKING, TIMER_EXECUTION_TYPE_UNKNOWN};

	Timer();
	virtual ~Timer();

	/**
	 * Sets the periodic time if the timer is to be run periodically.
	 * The first execution will be after 'seconds'+'nano_seconds' after the timer has been started!
	 * Returns 0 on success, -1 otherwise.
	 */
	virtual int setPeriodicTime(long int seconds, long int nano_secs);

	/**
	 * Sets the time for a single instruction.
	 * The first execution will be after 'seconds'+'nano_seconds' after the timer has been started!
	 * Returns 0 on success, -1 otherwise.
	 */
	virtual int setOneShotTime(long int seconds, long int nano_secs);

	virtual void startTimer();
	virtual void stopTimer();

	/**
	 * This method takes absolute ownership of the given task and will eventually delete it.
	 */
	void setTask(Task* task);

protected:
	int m_timer_fd;
	int m_epoll_fd;
	int m_epoll_poker_fd;
	epoll_event m_epoll_found_events[20];
	int m_epoll_event_size;//must be the size of 'm_epoll_found_events'
	bool m_timer_ready;

	struct itimerspec m_new_timer_val;
	struct itimerspec m_old_timer_val;

	//these are needed to stop and restart the timer
	static struct itimerspec m_timer_stop_val;
	static struct itimerspec m_timer_stop_val_old;//this one will be spammed with garbage from multiple threads

	Task* mp_task;

	enum TIMER_TYPE m_timer_type;
	enum TIMER_EXECUTION_TYPE m_timer_execution_type;

	//internal helper methods
	int stopTimer_();
	int startTimer_();
};


} /* namespace SHNetwork */

#endif /* TIMER_H_ */
