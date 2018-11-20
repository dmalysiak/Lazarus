/*
 * NCSList.h
 *
 *  Created on: 28.01.2013
 *      Author: Darius Malysiak
 */

#ifndef FASTNCSLIST_H_
#define FASTNCSLIST_H_

#include "GenNCSList.h"
#include "GenNCSListElement.h"
//#include "GenericIterator.h"

#include <stdio.h>
#include <stdexcept>


/**
 * Efficient class for a non-cyclic singly linked list. Yet, it has one inherent flaw;
 * it is not thread safe in the following sense. Regarding the speed we have not implemented
 * any mutexes for reading the listelement objects. I.e. if the user gets a grip onto the first
 * list object, with the desire to traverse the list, he should ensure that no other thread is removing
 * any listelement object!
 * A solution would be to implement an increment-reader function, which expects a listelement
 * and outputs the neighbor listelement. Additionally it locks the list methods with an additional mutex.
 * Yet this would imply a huge lock/unlock overhead for many simultaneous read operations.
 * Otherwise, no inherent problems exist.
 */

namespace Lazarus
{


template<class T> class FastNCSList : public GenNCSList<T>{
public:

	FastNCSList(bool simple_ids = true) : GenNCSList<T>(simple_ids)
	{
	}

	/*
	 * the copy constructor creates a >new< instance of the class
	 */

	FastNCSList(const FastNCSList<T>& list,bool simple_ids=true): GenNCSList<T>(simple_ids)
	{
		//add new elements
		for(typename Lazarus::FastNCSList<T>::Iterator it = list.getStart();
			it != list.getEnd(); ++it)
		{
		  GenNCSListElement<T>* element = it.getElement();
		  T value = element->getm_data();

		  this->appendLast(value);
		}
	}

	/*
	 * if the data type of the list is a non-pointer type, this destructor will clean up memory completely,
	 * i.e. the list (more specifically the list elements) contains only >stack allocated< >copies< of the original data.
	 * In case of pointers one has to manually free the objects behind them (use the list helper).
	 */
	virtual ~FastNCSList()
	{
		//free each list object
		GenNCSListElement<T>* list_element = this->mp_first;
		GenNCSListElement<T>* next_list_element = NULL;

		//traverse the list
		while(list_element != NULL)
		{
			next_list_element = list_element->getmp_neighbor();
			delete list_element;
			list_element = next_list_element;
		}

		this->mp_first = NULL;
		this->mp_last = NULL;
	}

	/**
	 * This will not delete any existing data in case of pointers.
	 * */
	FastNCSList& operator=( const FastNCSList& list )
	{
		//clean up

		//free each list object
		GenNCSListElement<T>* list_element = this->mp_first;
		GenNCSListElement<T>* next_list_element = NULL;

		//traverse the list
		while(list_element != NULL)
		{
			next_list_element = list_element->getmp_neighbor();
			delete list_element;
			list_element = next_list_element;
		}

		this->mp_first = NULL;
		this->mp_last = NULL;

		this->m_simple_ids = list.m_simple_ids;
		this->m_id_counter = 0;
		this->m_size = 0;

		//add new elements
		for(typename Lazarus::FastNCSList<T>::Iterator it = list.getStart();
			it != list.getEnd(); ++it)
		{
		  GenNCSListElement<T>* element = it.getElement();
		  T value = element->getm_data();

		  this->appendLast(value);
		}


		return *this;
	}

	//appends an element at the end of the list
	void appendLast(const T& data)
	{
		GenNCSListElement<T>* element = NULL;

		//generate a unique id
		if(this->m_simple_ids == true)
		{
			element = new GenNCSListElement<T>(this->m_id_counter);
		}

		element->setm_data(data);

		if(this->mp_last!=NULL)
		{
			this->mp_last->setmp_neighbor(element);
		}
		else //list is empty
		{
			this->mp_first=element;
		}

		this->mp_last = element;
		this->m_size++;
		this->m_id_counter++;
	}

	//appends an element at the beginning of the list
	void appendFirst(const T& data)
	{
		GenNCSListElement<T>* element = NULL;

		//generate a unique id
		if(this->m_simple_ids == true)
		{
			element = new GenNCSListElement<T>(this->m_id_counter);
		}

		element->setm_data(data);

		if(this->mp_first!=NULL)
		{
			element->setmp_neighbor(this->mp_first);
		}
		else //list is empty
		{
			this->mp_last = element;
		}
		this->mp_first = element;
		this->m_size++;
		this->m_id_counter++;

	}

	void removeFirst()
	{
		GenNCSListElement<T>* element = this->mp_first;

		if(this->mp_first != NULL)
		{
			this->mp_first = this->mp_first->getmp_neighbor();
		}
		else //list is empty
		{
			return;
		}

		this->m_size--;

		delete element;
	}

	GenNCSListElement<T>* getFirst()
	{
		GenNCSListElement<T>* element = NULL;

		element = this->mp_first;

		return element;
	}

	unsigned long getSize()
	{
		unsigned long size = this->m_size;

		return size;
	}

	virtual T getListElementAtPos(unsigned int pos)
	{

		if(pos > this->m_size-1)
		{
			printf("ERROR: index out of bounds\n");
			std::out_of_range e("");
			throw e;
		}

		unsigned int c = 0;
		GenNCSListElement<T>* ve = this->mp_first;

		while(c <= pos)
		{
			if(c==pos)
			{
				break;
			}
			ve = ve->getmp_neighbor();
			c++;
		}


		return ve->getm_data();
	}

	/**
	 * the complexity of this method is linear in the list size
	 */
	virtual T& op(unsigned int pos)
	{

		if(pos > this->m_size-1)
		{
			printf("ERROR: index out of bounds\n");
			std::out_of_range e("");
			throw e;
		}

		unsigned int c = 0;
		GenNCSListElement<T>* ve = this->mp_first;

		while(c <= pos)
		{
			if(c==pos)
			{
				break;
			}
			ve = ve->getmp_neighbor();
			c++;
		}

		//unlock access for other threads

		return ve->getm_data_ref();
	}

	/**
	 * the complexity of this method is linear in the list size
	 */
	virtual const T& op(unsigned int pos) const
	{

		if(pos > this->m_size-1)
		{
			printf("ERROR: index out of bounds\n");
			std::out_of_range e("");
			throw e;
		}

		unsigned int c = 0;
		GenNCSListElement<T>* ve = this->mp_first;

		while(c <= pos)
		{
			if(c==pos)
			{
				break;
			}
			ve = ve->getmp_neighbor();
			c++;
		}

		//unlock access for other threads

		return ve->getm_data_ref();
	}

	void printListStructure()
	{

		GenNCSListElement<T>* list_element = this->mp_first;
		unsigned long element_counter = 0;

		//traverse the list
		while(list_element != NULL)
		{
			printf("List element no. %ld at address %ld with neighbor %ld\n",element_counter,
					(unsigned long)list_element,(unsigned long)list_element->getmp_neighbor());
			list_element = list_element->getmp_neighbor();
			element_counter++;
		}

	}


	//note the inefficiency of this code -> TODO: use hashtables for storing used ids
	bool checkNCSListElementIDUniqueness(unsigned long id)
	{
		bool is_unique = true;

		//traverse the list and check for identical ids
		GenNCSListElement<T>* current_element = this->mp_first;

		while(current_element != NULL)
		{
			//break if identical id has been found
			if(current_element->getID() == id)
			{
				is_unique = false;
				break;
			}

			current_element = current_element->getmp_neighbor();
		}

		return is_unique;
	}

};


}

#endif /* FASTNCSLIST_H_ */
