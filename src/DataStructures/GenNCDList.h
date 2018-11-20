/*
 * GenNCDList.h
 *
 *  Created on: Jul 21, 2014
 *      Author: Darius Malysiak
 */

#ifndef GENNCDLIST_H_
#define GENNCDLIST_H_

#include "GenList.h"
#include "GenericIterator.h"
#include "GenNCDListElement.h"
#include "Eraser.h"

#include <type_traits>

namespace Lazarus {

template<class T>
class GenNCDList : public GenList<T>{
public:

	//a forward iterator class, this iterator is not thread safe!
	class Iterator : public GenericForwardIterator<GenNCDListElement,T>
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

		Iterator(GenNCDListElement<T>* current_element)
		{
			this->mp_current_element = current_element;
		}

		~Iterator(){}

		virtual void operator ++()
		{
			if(this->mp_current_element != NULL)
			{
				this->mp_current_element = this->mp_current_element->getmp_pred();
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


	//a forward iterator class, this iterator is not thread safe!
	class ReverseIterator : public GenericBackwardIterator<GenNCDListElement,T>
	{
	public:
		ReverseIterator()
		{
			this->mp_current_element = NULL;
		}

		ReverseIterator(const Iterator& it)
		{
			this->mp_current_element = it.getElement();
		}

		ReverseIterator(GenNCDListElement<T>* current_element)
		{
			this->mp_current_element = current_element;
		}

		~ReverseIterator(){}

		virtual void operator --()
		{
			if(this->mp_current_element != NULL)
			{
				this->mp_current_element = this->mp_current_element->getmp_succ();
			}
		}

		virtual bool operator !=(const ReverseIterator it)
		{
			if( this->mp_current_element != it.getElement() )
			{
				return true;
			}

			return false;
		}

	};

	const unsigned long NCDLISTELEMENT_ID_MAX = 100000000L;

	enum NCD_ERROR_CODE {NCD_ERROR_CODE_NO_ERROR,NCD_ERROR_CODE_INDEX_OUT_OF_BOUNDS, NCD_ERROR_CODE_ELEMENT_NOT_FOUND};

	GenNCDList(bool simple_ids = true)
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

	GenNCDList(const GenNCDList<T>& list,bool simple_ids = true)
	{
		m_simple_ids = simple_ids;
		m_id_counter = 0;
		m_size = 0;
		mp_first = NULL;
		mp_last = NULL;
	}

	virtual ~GenNCDList()
	{
	}

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

	//returns an iterator to the first element
	ReverseIterator getRStart() const
	{
		ReverseIterator start_(mp_last);
		return start_;
	}

	//returns an iterator to the list terminator
	ReverseIterator getREnd() const
	{
		ReverseIterator end_(NULL);
		return end_;
	}

	virtual GenNCDListElement<T>* getFirst()
	{
		printf("NOT IMPLEMENTED\n");

		return NULL;
	}

	virtual GenNCDListElement<T>* getLast()
	{
		printf("NOT IMPLEMENTED\n");

		return NULL;
	}

	virtual void removeLast()
	{
		printf("NOT IMPLEMENTED\n");
	}

	virtual enum NCD_ERROR_CODE removeElementByPosition(unsigned long position)
	{
		printf("NOT IMPLEMENTED\n");
		return NCD_ERROR_CODE_NO_ERROR;
	}

	virtual enum NCD_ERROR_CODE removeElement(GenNCDListElement<T>* element)
	{
		printf("NOT IMPLEMENTED\n");
		return NCD_ERROR_CODE_NO_ERROR;
	}

	/**
	 * this function removes every object from the list
	 */
	virtual void clear()
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
		this->m_size = 0;
	}

	/**
	 * this function removes every object from the list and deletes it in case of pointers
	 */
	virtual void destroy()
	{
		//free each list object
		GenNCDListElement<T>* list_element = this->mp_first;
		GenNCDListElement<T>* next_list_element = NULL;

		T data;

		//traverse the list
		while(list_element != NULL)
		{
			next_list_element = list_element->getmp_succ();

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
		GenNCDListElement<T>* list_element = this->mp_first;
		GenNCDListElement<T>* next_list_element = NULL;

		T data;

		//traverse the list
		while(list_element != NULL)
		{
			next_list_element = list_element->getmp_succ();

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

	virtual T& op(unsigned int pos)
	{
		static T a;
		printf("ERROR: GenNCDList::op(uint pos) not implemented!\n");
		return a;
	}

	virtual const T& op(unsigned int pos) const
	{
		static T a;
		printf("ERROR: GenNCDList::op(uint pos) not implemented!\n");
		return a;
	}

protected:
	bool m_simple_ids;
	unsigned long m_id_counter;
	unsigned long m_size;
	GenNCDListElement<T>* mp_first;
	GenNCDListElement<T>* mp_last;

};


} /* namespace Lazarus */

#endif /* GENNCDLIST_H_ */
