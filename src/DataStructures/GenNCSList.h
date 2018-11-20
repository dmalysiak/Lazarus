/*
 * List.h
 *
 *  Created on: May 13, 2014
 *      Author: Darius Malysiak
 */

#ifndef GENNCSLIST_H_
#define GENNCSLIST_H_

#include "GenList.h"
#include "GenericIterator.h"
#include "GenNCSListElement.h"
#include "Eraser.h"

#include <type_traits>

namespace Lazarus
{

template<class T> class GenNCSList : public GenList<T>{
public:

	//a forward iterator class, this iterator is not thread safe!
	class Iterator : public GenericForwardIterator<GenNCSListElement,T>
	{
	public:
		Iterator()
		{
			this->mp_current_element = NULL;
		}

		Iterator(const Iterator& it)
		{
			this->mp_current_element = it.getElement();
		}

		Iterator(GenNCSListElement<T>* current_element)
		{
			this->mp_current_element = current_element;
		}

		~Iterator(){}

		virtual void operator ++()
		{
			if(this->mp_current_element != NULL)
			{
				this->mp_current_element = this->mp_current_element->getmp_neighbor();
			}
		}

		virtual bool operator !=(const Iterator it)
		{
			if( this->mp_current_element != it.getElement() )
			{
				return true;
			}

			return false;
		}

	};

	const unsigned long NCSLISTELEMENT_ID_MAX = 100000000L;

	GenNCSList(bool simple_ids = true)
	{
		m_simple_ids = simple_ids;
		m_id_counter = 0;
		m_size = 0;
		mp_first = NULL;
		mp_last = NULL;
	}

	/*
	 * the copy constructor creates a >new< instance of the class
	 */

	GenNCSList(const GenNCSList<T>& list,bool simple_ids=true)
	{
		m_simple_ids = simple_ids;
		m_id_counter = 0;
		m_size = 0;
		mp_first = NULL;
		mp_last = NULL;
	}

	virtual ~GenNCSList()
	{}

	//returns an iterator to the first element
	Iterator getStart() const
	{
		Iterator start_(mp_first);
		return start_;
	}

	//returns an iterator to the list terminator
	Iterator getEnd() const
	{
		Iterator end_(NULL);
		return end_;
	}

	virtual T& op(unsigned int pos)
	{
		static T a;
		printf("ERROR: GenNCSList::op(uint pos) not implemented!\n");
		return a;
	}

	virtual const T& op(unsigned int pos) const
	{
		static T a;
		printf("ERROR: GenNCSList::op(uint pos) not implemented!\n");
		return a;
	}

	/**
	 * this function removes every object from the list
	 */
	virtual void clear()
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
		this->m_size = 0;
	}

	/**
	 * this function removes every object from the list and deletes it in case of pointers
	 */
	virtual void destroy()
	{
		//free each list object
		GenNCSListElement<T>* list_element = this->mp_first;
		GenNCSListElement<T>* next_list_element = NULL;

		T data;

		//traverse the list
		while(list_element != NULL)
		{
			next_list_element = list_element->getmp_neighbor();

			data = list_element->getm_data();
			Eraser<T>::erase(data);
			/*if( std::is_pointer<T>::value == true )
			{
				data = list_element->getm_data();
				delete (*(T**)&(data));
			}
			else
			{
				printf("ERROR: can't call destructor on stack objects\n");
			}*/

			delete list_element;
			list_element = next_list_element;
		}

		this->mp_first = NULL;
		this->mp_last = NULL;
		this->m_size = 0;
	}


	/**
	 * this function removes every object from the list and deletes it in case of pointers
	 */
	virtual void destroyArrays()
	{
		//free each list object
		GenNCSListElement<T>* list_element = this->mp_first;
		GenNCSListElement<T>* next_list_element = NULL;

		T data;

		//traverse the list
		while(list_element != NULL)
		{
			next_list_element = list_element->getmp_neighbor();

			data = list_element->getm_data();
			Eraser<T>::eraseArray(data);
			/*if( std::is_pointer<T>::value == true )
			{
				data = list_element->getm_data();
				delete[] (*(T**)&(data));
			}
			else
			{
				printf("ERROR: can't call destructor on stack objects\n");
			}*/

			delete list_element;
			list_element = next_list_element;
		}

		this->mp_first = NULL;
		this->mp_last = NULL;
		this->m_size = 0;
	}


protected:
	//members
	unsigned long m_id_counter;
	unsigned long m_size;
	GenNCSListElement<T>* mp_first;
	GenNCSListElement<T>* mp_last;
	bool m_simple_ids;

};


}


#endif /* GENNCSLIST_H_ */
