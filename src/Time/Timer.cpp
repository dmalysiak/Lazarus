/*
 * Timer.cpp
 *
 *  Created on: 01.01.2015
 *      Author: Darius Malysiak
 */

#include "Timer.h"

#include <unistd.h>

namespace Lazarus {

Timer::Timer() {

	m_timer_type = TIMER_TYPE_UNKNOWN;
	m_timer_execution_type = TIMER_EXECUTION_TYPE_UNKNOWN;
	m_timer_fd = -1;
	m_timer_ready = false;
	m_epoll_fd = -1;
	m_epoll_event_size = 20;
	mp_task = NULL;
	m_epoll_poker_fd = -1;

	//create the timer with the realtime clock
	m_timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);

	if(m_timer_fd == -1)
	{
		printf("ERROR: could not create timer\n");
		return;
	}

	//create the epoll fd
	m_epoll_fd = epoll_create1(0);
	if(m_epoll_fd == -1)
	{
		printf("ERROR: could not create epoll instance\n");
		close(m_timer_fd);
		return;
	}

	//create a poker
	m_epoll_poker_fd = eventfd(0,EFD_NONBLOCK);
	if(m_epoll_poker_fd == -1)
	{
		printf("ERROR: could not create poker\n");
		close(m_timer_fd);
		close(m_epoll_fd);
		return;
	}

	//register the timer fd in epoll
	epoll_event timer_event;
	timer_event.events = EPOLLIN;//only read events
	timer_event.data.fd = m_timer_fd;

	int res = epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, m_timer_fd, &timer_event);
	if(res == -1)
	{
		printf("ERROR: could not register timer fd in epoll\n");
		close(m_timer_fd);
		close(m_epoll_fd);
		close(m_epoll_poker_fd);
		return;
	}

	//and the poker
	timer_event.data.fd = m_epoll_poker_fd;
	res = epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, m_epoll_poker_fd, &timer_event);
	if(res == -1)
	{
		printf("ERROR: could not register poker fd in epoll\n");
		close(m_timer_fd);
		close(m_epoll_fd);
		close(m_epoll_poker_fd);
		return;
	}

	m_timer_ready = true;
}

Timer::~Timer()
{
	close(m_timer_fd);
	close(m_epoll_fd);
	close(m_epoll_poker_fd);

	DELETE_NULL_CHECKING(mp_task);
}

void Timer::setTask(Task* task)
{
	//delete any existing task
	DELETE_NULL_CHECKING(mp_task);

	mp_task = task;
}

int Timer::setPeriodicTime(long int seconds, long int nano_secs)
{
	if(m_timer_type != TIMER_TYPE_PERIODIC)
	{
		printf("ERROR: timer is not periodic.\n");
		return -1;
	}

	m_new_timer_val.it_interval.tv_sec = seconds;
	m_new_timer_val.it_interval.tv_nsec = nano_secs;
	m_new_timer_val.it_value.tv_sec = seconds;
	m_new_timer_val.it_value.tv_nsec = nano_secs;

	return 0;
}

int Timer::setOneShotTime(long int seconds, long int nano_secs)
{
	if(m_timer_type != TIMER_TYPE_SINGLE_SHOT)
	{
		printf("ERROR: timer is not a single shot timer.\n");
		return -1;
	}

	m_new_timer_val.it_interval.tv_sec = 0;
	m_new_timer_val.it_interval.tv_nsec = 0;
	m_new_timer_val.it_value.tv_sec = seconds;
	m_new_timer_val.it_value.tv_nsec = nano_secs;

	return 0;
}

int Timer::stopTimer_()
{
	if(m_timer_ready == false)
	{
		return -1;
	}

	int res = timerfd_settime(m_timer_fd, 0, &m_timer_stop_val, &m_timer_stop_val_old);
	if(res == -1)
	{
		printf("ERROR: could not temporarily stop timer\n");
		return -1;
	}

	return 0;
}

int Timer::startTimer_()
{
	if(m_timer_ready == false)
	{
		return -1;
	}

	int res = timerfd_settime(m_timer_fd, 0, &m_new_timer_val, &m_old_timer_val);
	if(res == -1)
	{
		printf("ERROR: could not start timer\n");
		return -1;
	}

	return 0;
}

void Timer::startTimer()
{
}

void Timer::stopTimer()
{
}

//static members
struct timespec it_interval = {0,0};
struct timespec it_value = {0,0};
struct itimerspec Timer::m_timer_stop_val = {it_interval, it_value};
struct itimerspec Timer::m_timer_stop_val_old = {it_interval, it_value};


} /* namespace SHNetwork */
