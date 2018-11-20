/*
 * Queue.h
 *
 *  Created on: 28.01.2013
 *      Author: Darius Malysiak
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include "GenNCDList.h"

#include <stdio.h>

//#include <stdio.h>
//#include <sys/time.h>

namespace Lazarus
{

/**
 * In case of stack objects a queue will work only if the corresponding class contains a default constructor.
 * */
template< class T> class Queue : public GenNCDList<T> {
public:

	/**
	 * A capacity of 0 indicates an infinite capacity.
	 * */
	Queue(unsigned int capacity = 0) : GenNCDList<T>()
	{
		pthread_cond_init(&m_has_been_filled,NULL);//PTHREAD_COND_INITIALIZER;
		pthread_mutex_init(&m_mutex,NULL);
		m_is_empty = true;
		m_wait_time_sec = 1;
		m_wait_time_usec = 0;
		m_capacity = capacity;
	}

	/**
	 * the copy constructor creates a >new< instance of the class with the capacity of the original object
	 */
	Queue(const Queue<T>& q) : GenNCDList<T>()
	{
		pthread_cond_init(&m_has_been_filled,NULL);//PTHREAD_COND_INITIALIZER;
		pthread_mutex_init(&m_mutex,NULL);
		m_is_empty = true;
		m_wait_time_sec = 1;
		m_wait_time_usec = 0;
		m_capacity = q.getm_capacity();

		//clean up
		pthread_mutex_lock(&(this->m_mutex));

		this->mp_first = NULL;
		this->mp_last = NULL;

		this->m_id_counter = 0;
		this->m_size = 0;

		//add new elements
		for(typename Lazarus::Queue<T>::Iterator it = q.getStart();
			it != q.getEnd(); ++it)
		{
		  GenNCDListElement<T>* element = it.getElement();
		  T value = element->getm_data();

		  this->enqueueNoLock(value);
		}

		pthread_mutex_unlock(&(this->m_mutex));
	}


	/**
	 * the copy constructor creates a >new< instance of the class with the capacity of the original object
	 */
	Queue(const Queue<T>* q) : GenNCDList<T>()
	{
		pthread_cond_init(&m_has_been_filled,NULL);//PTHREAD_COND_INITIALIZER;
		pthread_mutex_init(&m_mutex,NULL);
		m_is_empty = true;
		m_wait_time_sec = 1;
		m_wait_time_usec = 0;
		m_capacity = q->getm_capacity();

		//clean up
		pthread_mutex_lock(&(this->m_mutex));

		this->mp_first = NULL;
		this->mp_last = NULL;

		this->m_id_counter = 0;
		this->m_size = 0;

		//add new elements
		for(typename Lazarus::Queue<T>::Iterator it = q->getStart();
			it != q->getEnd(); ++it)
		{
		  GenNCDListElement<T>* element = it.getElement();
		  T value = element->getm_data();

		  this->enqueueNoLock(value);
		}

		pthread_mutex_unlock(&(this->m_mutex));
	}


	virtual ~Queue()
	{
		pthread_mutex_lock(&(this->m_mutex));

		//free each list object
		GenNCDListElement<T>* list_element = this->mp_first;
		GenNCDListElement<T>* next_list_element = NULL;

		//traverse the list
		while(list_element != NULL)
		{
			next_list_element = list_element->getmp_succ();
			delete list_element;
			list_element = next_list_element;
		}

		this->mp_first = NULL;
		this->mp_last = NULL;

		pthread_mutex_unlock(&(this->m_mutex));

		pthread_mutex_destroy(&m_mutex);
	}

	CLASS_RTTI(Lazarus::Queue)

	/**
	 * This will not delete any existing data in case of pointers.
	 * */
	Queue& operator=( const Queue& q )
	{
		//clean up
		pthread_mutex_lock(&(this->m_mutex));

		//free each list object
		GenNCDListElement<T>* list_element = this->mp_first;
		GenNCDListElement<T>* next_list_element = NULL;

		//traverse the list
		while(list_element != NULL)
		{
			next_list_element = list_element->getmp_succ();
			delete list_element;
			list_element = next_list_element;
		}

		this->mp_first = NULL;
		this->mp_last = NULL;

		this->m_id_counter = 0;
		this->m_size = 0;
		m_capacity = q.m_capacity;
		m_is_empty = true;

		//add new elements
		for(typename Lazarus::Queue<T>::Iterator it = q.getStart();
			it != q.getEnd(); ++it)
		{
		  GenNCDListElement<T>* element = it.getElement();
		  T value = element->getm_data();

		  this->enqueueNoLock(value);
		}

		pthread_mutex_unlock(&(this->m_mutex));

		return *this;
	}

	unsigned int getm_capacity() const
	{
		return m_capacity;
	}

	virtual unsigned long getSize()
	{
		unsigned long size = this->m_size;

		return size;
	}

	/**
	 * this function removes every object from the queue
	 */
	virtual void clear()
	{
		pthread_mutex_lock(&(this->m_mutex));

		//free each list object
		GenNCDListElement<T>* list_element = this->mp_first;
		GenNCDListElement<T>* next_list_element = NULL;

		//traverse the list
		while(list_element != NULL)
		{
			next_list_element = list_element->getmp_succ();
			delete list_element;
			list_element = next_list_element;
		}

		this->mp_first = NULL;
		this->mp_last = NULL;
		this->m_size = 0;

		pthread_mutex_unlock(&(this->m_mutex));
	}

	/**
	 * Returns 0 in case of success, -1 otherwise.
	 * */
	int enqueue(T element)
	{
		pthread_mutex_lock(&(this->m_mutex));

		//check if queue has an upper bound for elements, if so, check if it has been reached -> exit
		if(m_capacity > 0)
		{
			if(this->m_size == m_capacity)
			{
				printf("ERROR: queue is full\n");
				pthread_mutex_unlock(&(this->m_mutex));
				return -1;
			}
		}

		//enqueue the element
		GenNCDListElement<T>* element_ = new GenNCDListElement<T>();

		element_->setm_data(element);

		if(this->mp_first != NULL)//if not empty append a new element behind the last member
		{
			element_->setmp_succ(this->mp_last);//we have a last member
			element_->setmp_pred(NULL);

			//update the old last member
			this->mp_last->setmp_pred(element_);

			this->mp_last = element_;//and update the last member pointer
		}
		else //list is empty
		{
			this->mp_last = element_;
			this->mp_first = element_;
		}
		this->m_size++;

		//check if an empty list has been augmented with an item
		if(this->mp_first == this->mp_last)//address comparison
		{
			m_is_empty = false;
			//signal all the possibly waiting threads that the list has been refilled
			//printf("signal away\n");
			//pthread_cond_signal(&m_has_been_filled);
			pthread_cond_broadcast(&m_has_been_filled);
		}
	//	else{ //this 'else' is purely for debug purposes
	//		printf("no signal\n");
	//	}
		pthread_mutex_unlock(&(this->m_mutex));

		return 0;
	}

	/**
	 * Returns the element in case of success, T() otherwise.
	 * */
	T dequeueBlockingOnEmpty()
	{
		pthread_mutex_lock(&(this->m_mutex));

	//	struct timespec   absolute_timeout;
	//	struct timeval    current_time;

		GenNCDListElement<T>* element = NULL;
		T data = T();


		if(!m_is_empty)
		{
			element = this->mp_first;
			data = element->getm_data();

			this->mp_first = element->getmp_pred();

			this->m_size--;
			delete element;

			//check if queue is empty
			if(this->mp_first == NULL)
			{
				m_is_empty = true;
			}
			else
			{
				this->mp_first->setmp_succ(NULL);
			}

			//printf("the queue contains now %ld elements\n",this->m_size);
			pthread_mutex_unlock(&(this->m_mutex));

			return data;
		}
		else
		{
			//set wait time
	//		gettimeofday(&current_time, NULL);
	//		absolute_timeout.tv_sec  = current_time.tv_sec + m_wait_time_sec;
	//		absolute_timeout.tv_nsec = current_time.tv_usec * 1000 + m_wait_time_usec;

			//wait until list has been refilled
			while(m_is_empty)
			{
				/*
				 * a timed wait is necessary because of the following scenario:
				 * n threads are started, afterwards a m tasks are scheduled,
				 * the starting of the threads can overlap the scheduling(enqueuing) of the tasks,
				 * thus k threads may be already started and wait because of an empty list, now the
				 * first task is being schedules, it signals a cond-signal, yet the >full< processing of
				 * it may be postponed by the system scheduler. I.e. one thread will become runnable, yet it won't
				 * lock the mutex or in other words; the thread is not continuing yet. The remaining tasks
				 * are scheduled, each enqueuing yields not signal.
				 * The list now contains all the tasks, finally the signal is being processed and wakes one of
				 * the waiting threads. Yet, k-1 threads are still waiting and not being waked (as no enqueue operation
				 * occurs anymore).
				 * afterwards the remaining threads may be fully initialized.
				 */


				//SOLUTION: use a broadcast-signal to wake all the waiting threads and change their state to runnable

				//pthread_cond_timedwait(&(this->m_has_been_filled),&(this->m_mutex),&absolute_timeout);
				pthread_cond_wait(&(this->m_has_been_filled),&(this->m_mutex));

			}

			element = this->mp_first;
			data = element->getm_data();

			this->mp_first = element->getmp_pred();

			this->m_size--;
			delete element;

			//check if queue is empty
			if(this->mp_first == NULL)
			{
				m_is_empty = true;
			}
			else
			{
				this->mp_first->setmp_succ(NULL);
			}

			//printf("BLOCKED: the queue contains now %ld elements\n",this->m_size);
			pthread_mutex_unlock(&(this->m_mutex));

			return data;
		}

		return T();

	}

	/**
	 * Returns the element in case of success, T() otherwise (i.e. in case of an empty queue)..
	 * */
	T dequeue()
	{
		pthread_mutex_lock(&(this->m_mutex));

		if(!m_is_empty)
		{
			GenNCDListElement<T>* element = this->mp_first;
			T data = element->getm_data();

			this->mp_first = element->getmp_pred();

			this->m_size--;
			delete element;

			//check if queue is empty
			if(this->mp_first == NULL)
			{
				m_is_empty = true;
			}
			else
			{
				this->mp_first->setmp_succ(NULL);
			}

			pthread_mutex_unlock(&(this->m_mutex));

			return data;
		}

		//implicit else: list is empty
		pthread_mutex_unlock(&(this->m_mutex));
		return T();

	}

	/**
	 * Returns the element in case of success, T() otherwise (i.e. in case of an empty queue).
	 * If the list was empty 'res' will be set to -1, otherwise it will be set to 0.
	 * */
	T dequeue(int& res)
	{
		pthread_mutex_lock(&(this->m_mutex));

		if(!m_is_empty)
		{
			GenNCDListElement<T>* element = this->mp_first;
			T data = element->getm_data();

			this->mp_first = element->getmp_pred();

			this->m_size--;
			delete element;

			//check if queue is empty
			if(this->mp_first == NULL)
			{
				m_is_empty = true;
			}
			else
			{
				this->mp_first->setmp_succ(NULL);
			}

			pthread_mutex_unlock(&(this->m_mutex));

			res = 0;
			return data;
		}

		//implicit else: list is empty
		pthread_mutex_unlock(&(this->m_mutex));

		res = -1;
		return T();

	}

	bool getm_is_empty()
	{
		return this->m_is_empty;
	}


private:
	pthread_cond_t m_has_been_filled;
	bool m_is_empty;

	unsigned long m_wait_time_sec;
	unsigned long m_wait_time_usec;

	pthread_mutex_t m_mutex;

	unsigned int m_capacity;

	int enqueueNoLock(T element)
	{
		GenNCDListElement<T>* element_ = new GenNCDListElement<T>();

		element_->setm_data(element);

		if(this->mp_first != NULL)//if not empty append a new element behind the last member
		{
			element_->setmp_succ(this->mp_last);//we have a last member
			element_->setmp_pred(NULL);

			//update the old last member
			this->mp_last->setmp_pred(element_);

			this->mp_last = element_;//and update the last member pointer
		}
		else //list is empty
		{
			this->mp_last = element_;
			this->mp_first = element_;
		}
		this->m_size++;

		//check if an empty list has been augmented with an item
		if(this->mp_first == this->mp_last)//address comparison
		{
			m_is_empty = false;
			//signal all the possibly waiting threads that the list has been refilled
			//printf("signal away\n");
			//pthread_cond_signal(&m_has_been_filled);
			pthread_cond_broadcast(&m_has_been_filled);
		}
	//	else{ //this 'else' is purely for debug purposes
	//		printf("no signal\n");
	//	}

		return 0;
	}

};

}

#endif /* QUEUE_H_ */
