/*
 * NCSList.h
 *
 *  Created on: 28.01.2013
 *      Author: Darius Malysiak
 */

#ifndef NCDLIST_H_
#define NCDLIST_H_

#include "GenNCDList.h"
#include "GenNCDListElement.h"

#include <stdio.h>
#include <stdexcept>

/**
 * Efficient class for a non-cyclic doubly linked list. Yet, it has one inherent flaw;
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

template<class T>
class FastNCDList : public GenNCDList<T>{
public:

	FastNCDList(bool simple_ids = true) : GenNCDList<T>(simple_ids)
	{
	}

	/*
	 * the copy constructor creates a >new< instance of the class
	 */

	FastNCDList(const FastNCDList<T>& list,bool simple_ids = true) : GenNCDList<T>(simple_ids)
	{
		//add new elements
		for(typename Lazarus::FastNCDList<T>::Iterator it = list.getStart();
			it != list.getEnd(); ++it)
		{
		  GenNCDListElement<T>* element = it.getElement();
		  T value = element->getm_data();

		  this->appendLast(value);
		}
	}

	/*
	 * if the data type of the list is a non-pointer type, this destructor will clean up memory completely,
	 * i.e. the list (more specifically the list elements) contains only >stack allocated< >copies< of the original data.
	 * In case of pointers one has to manually free the objects behind them (use the list helper).
	 */
	virtual ~FastNCDList()
	{

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

	}


	/**
	 * This will not delete any existing data in case of pointers.
	 * */
	FastNCDList& operator=( const FastNCDList& list )
	{
		//clean up

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

		this->m_simple_ids = list.m_simple_ids;
		this->m_id_counter = 0;
		this->m_size = 0;

		//add new elements
		for(typename Lazarus::FastNCDList<T>::Iterator it = list.getStart();
			it != list.getEnd(); ++it)
		{
		  GenNCDListElement<T>* element = it.getElement();
		  T value = element->getm_data();

		  this->appendLast(value);
		}


		return *this;
	}

	//appends an element at the end of the list
	void appendLast(const T& data)
	{
		GenNCDListElement<T>* element = NULL;

		//generate a unique id
		if(this->m_simple_ids == true)
		{
			element = new GenNCDListElement<T>(this->m_id_counter);
		}


		element->setm_data(data);

		if(this->mp_last!=NULL)
		{
			this->mp_last->setmp_succ(element);
			element->setmp_succ(NULL);
			element->setmp_pred(this->mp_last);
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

		GenNCDListElement<T>* element = NULL;

		//generate a unique id
		if(this->m_simple_ids == true)
		{
			element = new GenNCDListElement<T>(this->m_id_counter);
		}


		element->setm_data(data);

		if(this->mp_first!=NULL)
		{
			this->mp_first->setmp_pred(element);
			element->setmp_succ(this->mp_first);
			element->setmp_pred(NULL);
		}
		else //list is empty
		{
			this->mp_last = element;
		}
		this->mp_first = element;
		this->m_size++;
		this->m_id_counter++;

	}

	void removeLast()
	{
		GenNCDListElement<T>* element = this->mp_last;

		if(element != NULL)
		{
			this->mp_last = this->mp_last->getmp_pred();
			this->mp_last->setmp_succ(NULL);
		}
		else //list is empty
		{
			return;
		}

		this->m_size--;


		delete element;

	}

	void removeFirst()
	{

		GenNCDListElement<T>* element = this->mp_first;

		if(element != NULL)
		{
			this->mp_first = this->mp_first->getmp_succ();
			this->mp_first->setmp_pred(NULL);
		}
		else //list is empty
		{
			return;
		}

		this->m_size--;


		delete element;

	}

	enum GenNCDList<T>::NCD_ERROR_CODE removeElementByPosition(unsigned long position)
	{

	    enum GenNCDList<T>::NCD_ERROR_CODE result = GenNCDList<T>::NCD_ERROR_CODE_ELEMENT_NOT_FOUND;

	    //traverse the list until vector element has been found
	    GenNCDListElement<T>* current_element = this->mp_first;
	    unsigned long position_ = 0;

	    if(position > this->m_size-1)
	    {
	    	std::out_of_range e("");
			throw e;
	    }

	    while(position_ <= position)
	    {
	        //break if an id matching has been found
	        if(position_ == position && current_element != NULL)
	        {
	            //remove it from the list
	            //if current element has a predecessor, i.e. if it is not the first one
	            if(current_element->getmp_pred() != NULL)
	            {
	                current_element->getmp_pred()->setmp_succ( current_element->getmp_succ() );
	            }
	            else
	            {
	                this->mp_first = current_element->getmp_succ();
	            }
	            //if current element has a successor, i.e. if it is not the last one
	            if(current_element->getmp_succ() != NULL)
	            {
	                current_element->getmp_succ()->setmp_pred(  current_element->getmp_pred() );
	            }
	            else
	            {
	            	this->mp_last = current_element->getmp_pred();
	            }
	            this->m_size--;

	            //delete it from memory
	            delete current_element;
	            result = GenNCDList<T>::NCD_ERROR_CODE_NO_ERROR;
	            break;
	        }

	        current_element = current_element->getmp_succ();
	        position_++;

	    }

	    return result;
	}

	/**
	 * The object behind 'element' will be deleted by this function and removed from the list!
	 */
	enum GenNCDList<T>::NCD_ERROR_CODE removeElement(GenNCDListElement<T>* element)
	{

		enum GenNCDList<T>::NCD_ERROR_CODE result = GenNCDList<T>::NCD_ERROR_CODE_ELEMENT_NOT_FOUND;

		//remove it from the list
		//if current element has a predecessor, i.e. if it is not the first one
		if(element->getmp_pred() != NULL)
		{
			element->getmp_pred()->setmp_succ( element->getmp_succ() );
		}
		else
		{
			this->mp_first = element->getmp_succ();
		}
		//if current element has a successor, i.e. if it is not the last one
		if(element->getmp_succ() != NULL)
		{
			element->getmp_succ()->setmp_pred( element->getmp_pred() );
		}
		else
		{
			this->mp_last = element->getmp_pred();
		}

		this->m_size--;

		result = GenNCDList<T>::NCD_ERROR_CODE_NO_ERROR;

		//delete the element
		delete element;

		return result;
	}


	virtual GenNCDListElement<T>* getFirst()
	{
		GenNCDListElement<T>* element = NULL;

		element = this->mp_first;

		return element;
	}

	virtual GenNCDListElement<T>* getLast()
	{
		GenNCDListElement<T>* element = NULL;

		element = this->mp_last;

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
		GenNCDListElement<T>* ve = this->mp_first;

		while(c <= pos)
		{
			if(c==pos)
			{
				break;
			}
			ve = ve->getmp_succ();
			c++;
		}


		return ve->getm_data();
	}

	virtual T& op(unsigned int pos)
	{

		if(pos > this->m_size-1)
		{
			printf("ERROR: index out of bounds\n");
			std::out_of_range e("");
			throw e;
		}

		unsigned int c = 0;
		GenNCDListElement<T>* ve = this->mp_first;

		while(c <= pos)
		{
			if(c==pos)
			{
				break;
			}
			ve = ve->getmp_succ();
			c++;
		}


		return ve->getm_data_ref();
	}

	virtual const T& op(unsigned int pos) const
	{

		if(pos > this->m_size-1)
		{
			printf("ERROR: index out of bounds\n");
			std::out_of_range e("");
			throw e;
		}

		unsigned int c = 0;
		GenNCDListElement<T>* ve = this->mp_first;

		while(c <= pos)
		{
			if(c==pos)
			{
				break;
			}
			ve = ve->getmp_succ();
			c++;
		}


		return ve->getm_data_ref();
	}

	void printListStructure()
	{

		GenNCDListElement<T>* list_element = this->mp_first;
		unsigned long element_counter = 0;

		//traverse the list
		while(list_element != NULL)
		{
			printf("List element no. %ld at address %ld with predecessor %ld and successor %ld\n",element_counter,
					(unsigned long)list_element,(unsigned long)list_element->getmp_pred(),(unsigned long)list_element->getmp_succ());
			list_element = list_element->getmp_succ();
			element_counter++;
		}

	}

protected:

	//note the inefficiency of this code -> TODO: use hashtables for storing used ids
	bool checkNCDListElementIDUniqueness(unsigned long id)
	{
		bool is_unique = true;

		//traverse the list and check for identical ids
		GenNCDListElement<T>* current_element = this->mp_first;

		while(current_element != NULL)
		{
			//break if identical id has been found
			if(current_element->getID() == id)
			{
				is_unique = false;
				break;
			}

			current_element = current_element->getmp_succ();
		}

		return is_unique;
	}

};


}

#endif /* NCDLIST_H_ */
