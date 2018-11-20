/*
 * Queue.h
 *
 *  Created on: 28.01.2013
 *      Author: Darius Malysiak
 */

#ifndef FIFO_H_
#define FIFO_H_

#include "GenNCDList.h"

#include <stdio.h>

//#include <stdio.h>
//#include <sys/time.h>

namespace Lazarus
{

/**
 * In case of stack objects a Fifo will work only if the corresponding class contains a default constructor.
 * Once the capacity limit was reached the first element will be dropped from the queue. Keep in mind that
 * in case of pointers this will not delete the object.
 * */
template< class T> class Fifo : public GenNCDList<T> {
public:

	/**
	 * A capacity of 0 indicates an infinite capacity.
	 * */
	Fifo(unsigned int capacity) : GenNCDList<T>()
	{
		pthread_cond_init(&m_has_been_filled,NULL);//PTHREAD_COND_INITIALIZER;
		pthread_mutex_init(&m_mutex,NULL);
		m_is_empty = true;
		m_wait_time_sec = 1;
		m_wait_time_usec = 0;
		m_capacity = capacity;
	}

	Fifo(const Fifo<T>& q) : GenNCDList<T>()
	{
		pthread_cond_init(&m_has_been_filled,NULL);//PTHREAD_COND_INITIALIZER;
		pthread_mutex_init(&m_mutex,NULL);
		m_is_empty = true;
		m_wait_time_sec = 1;
		m_wait_time_usec = 0;

		//clean up
		pthread_mutex_lock(&(this->m_mutex));

		this->mp_first = NULL;
		this->mp_last = NULL;

		this->m_id_counter = 0;
		this->m_size = 0;
		m_capacity = q.m_capacity;

		//add new elements
		for(typename Lazarus::Fifo<T>::Iterator it = q.getStart();
			it != q.getEnd(); ++it)
		{
		  GenNCDListElement<T>* element = it.getElement();
		  T value = element->getm_data();

		  this->pushNoLock(value);
		}

		pthread_mutex_unlock(&(this->m_mutex));
	}


	Fifo(const Fifo<T>* q) : GenNCDList<T>()
	{
		pthread_cond_init(&m_has_been_filled,NULL);//PTHREAD_COND_INITIALIZER;
		pthread_mutex_init(&m_mutex,NULL);
		m_is_empty = true;
		m_wait_time_sec = 1;
		m_wait_time_usec = 0;

		//clean up
		pthread_mutex_lock(&(this->m_mutex));

		this->mp_first = NULL;
		this->mp_last = NULL;

		this->m_id_counter = 0;
		this->m_size = 0;
		m_capacity = q->m_capacity;

		//add new elements
		for(typename Lazarus::Fifo<T>::Iterator it = q->getStart();
			it != q->getEnd(); ++it)
		{
		  GenNCDListElement<T>* element = it.getElement();
		  T value = element->getm_data();

		  this->pushNoLock(value);
		}

		pthread_mutex_unlock(&(this->m_mutex));
	}

	virtual ~Fifo()
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

	CLASS_RTTI(Lazarus::Fifo)

	/**
	 * This will not delete any existing data in case of pointers.
	 * */
	Fifo& operator=( const Fifo& q )
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
		for(typename Lazarus::Fifo<T>::Iterator it = q.getStart();
			it != q.getEnd(); ++it)
		{
		  GenNCDListElement<T>* element = it.getElement();
		  T value = element->getm_data();

		  this->pushNoLock(value);
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
	 * Returns 0 in case of success, -1 otherwise (e.g. when the capacity limit was reached and one element was dropped).
	 * */
	int push(T element)
	{
		int out = 0;

		pthread_mutex_lock(&(this->m_mutex));

		//check if queue has an upper bound for elements, if so, check if it has been reached -> exit
		if(m_capacity > 0)
		{
			if(this->m_size == m_capacity)
			{
				//printf("ERROR: queue is full\n");
				this->removeFirst_();

				//pthread_mutex_unlock(&(this->m_mutex));
				//out = -1;
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

		return out;
	}

	/**
	 * Returns the element in case of success, T() otherwise.
	 * */
	T readBlockingOnEmpty()
	{
		pthread_mutex_lock(&(this->m_mutex));

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

			pthread_mutex_unlock(&(this->m_mutex));

			return data;
		}
		else
		{

			//wait until list has been refilled
			while(m_is_empty)
			{
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
	T read()
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
	T read(int& res)
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

	void removeFirst_()
	{

		GenNCDListElement<T>* element = this->mp_first;

		if(element != NULL)
		{
			this->mp_first = this->mp_first->getmp_pred();
			this->mp_first->setmp_succ(NULL);
		}
		else //list is empty
		{
			return;
		}

		this->m_size--;

		delete element;
	}

	int pushNoLock(T element)
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
