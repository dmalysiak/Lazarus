/*
 * AsynchronousTimer.cpp
 *
 *  Created on: 02.01.2015
 *      Author: Darius Malysiak
 */

#include "AsynchronousTimer.h"

#include <unistd.h>

namespace Lazarus {

AsynchronousTimer::AsynchronousTimer(enum TIMER_TYPE timer_type)
{
	m_timer_type = timer_type;
	m_timer_execution_type = TIMER_EXECUTION_TYPE_NONBLOCKING;
}

AsynchronousTimer::~AsynchronousTimer()
{
	shutdown();
}

void AsynchronousTimer::startTimer()
{
	if(m_timer_ready == false)
	{
		printf("ERROR: can't start timer, as it isn't ready\n");
		return;
	}

	//deploy the thread
	this->start();
}

void AsynchronousTimer::stopTimer()
{
	if(m_timer_ready == false)
	{
		printf("ERROR: can't stop timer, as it isn't ready\n");
		return;
	}

	//join the thread with a graceful shutdown
	shutdown();
}

void AsynchronousTimer::preFlag()
{
	//just poke it :-)
	int bytes = eventfd_write(m_epoll_poker_fd,1);

	if(bytes==-1)
	{
		printf("ERROR: could not poke the timer thread %d\n",bytes);
	}
}

void AsynchronousTimer::run()
{
	//actually start the timer
	int res = timerfd_settime(m_timer_fd, 0, &m_new_timer_val, &m_old_timer_val);
	if(res == -1)
	{
		printf("ERROR: could not start timer\n");
		return;
	}


	while(m_shutdown_flag == false)
	{
		//wait indefinitely for events
		int res = epoll_wait(m_epoll_fd, m_epoll_found_events, m_epoll_event_size, -1);
		if(res == -1)//something went really wrong
		{
			m_shutdown_flag = true;
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
			  m_shutdown_flag = true;
			  m_thread_self_terminated = true;
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
						mp_task->runTask(this);
					}

					//reset the timer for periodic tasks
					if(m_timer_type == TIMER_TYPE_PERIODIC)
					{
						startTimer_();
					}
					else//we are done for a single shot timer
					{
						m_shutdown_flag = true;
						m_thread_self_terminated = true;
						break;
					}
				}
				else //something went wrong
				{
					printf("something went wrong with reading the timer\n");
					m_shutdown_flag = true;
					m_thread_self_terminated = true;
					break;
				}
			}
			//if it's the poker: terminate
			if (m_epoll_found_events[i].data.fd == m_epoll_poker_fd)
			{
				size_t s = 0;
				int i = read (m_epoll_fd, &s, sizeof (s));
				if(i != -1)
				{
					//printf("poker fd\n");
				}
				m_shutdown_flag = true;
				m_thread_self_terminated = true;
				break;
			}
		}
	}
}

} /* namespace SHNetwork */
