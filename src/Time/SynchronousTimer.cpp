/*
 * SynchronousTimer.cpp
 *
 *  Created on: 02.01.2015
 *      Author: Darius Malysiak
 */

#include "SynchronousTimer.h"

#include <unistd.h>

namespace Lazarus {

SynchronousTimer::SynchronousTimer()
{
	m_timer_type = TIMER_TYPE_SINGLE_SHOT;
	m_timer_execution_type = TIMER_EXECUTION_TYPE_BLOCKING;
	m_timer_run = false;
}

SynchronousTimer::~SynchronousTimer()
{

}

void SynchronousTimer::startTimer()
{
	//actually start the timer
	int res = timerfd_settime(m_timer_fd, 0, &m_new_timer_val, &m_old_timer_val);
	if(res == -1)
	{
		printf("ERROR: could not start timer\n");
		return;
	}

	m_timer_run = true;

	while(m_timer_run == true)
	{
		//wait indefinitely for events
		int res = epoll_wait(m_epoll_fd, m_epoll_found_events, m_epoll_event_size, -1);
		if(res == -1)//something went really wrong
		{
			m_timer_run = false;
			break;
		}

		//printf("trigger fd %d events\n",res);

		//check the received events
		for(int i=0;i<res;++i)
		{
			if((m_epoll_found_events[i].events & EPOLLERR) ||
				  (m_epoll_found_events[i].events & EPOLLHUP) ||
				  (!(m_epoll_found_events[i].events & EPOLLIN)))
			{
				  /* An error has occured on this fd, or the socket is not
					 ready for reading (why were we notified then?) */
			  printf("epoll error\n");
			  m_timer_run = false;
			  break;
			}

			//printf("fd %d | poker %d timer %d\n",m_epoll_found_events[i].data.fd,m_epoll_poker_fd,m_timer_fd);

			//check if it's the timer
			if(m_epoll_found_events[i].data.fd == m_timer_fd)
			{
				uint64_t s = 0;//overrun count
				int i = read(m_timer_fd, &s, sizeof(uint64_t));
				if(i != -1)
				{
					//stop the timer until the task has been done
					stopTimer_();

					//execute the task
					if(mp_task != NULL)
					{
						mp_task->runTask();
					}

					//we are done for a single shot timer
					m_timer_run = false;
					break;

				}
				else //something went wrong
				{
					printf("something went wrong with reading the timer\n");
					m_timer_run = false;
					break;
				}
			}
		}
	}
}

void SynchronousTimer::stopTimer()
{
}


} /* namespace SHNetwork */
