/*
 * KTuple.h
 *
 *  Created on: 04.02.2013
 *      Author: Darius Malysiak
 */

#ifndef FASTKTUPLE_H_
#define FASTKTUPLE_H_

#include "GenKTuple.h"
#include "Eraser.h"
#include "../Time/TimeToolbox.h"

#include <string.h>
#include <stdexcept>
#include <type_traits>
#include <limits>
#include <mcheck.h>
#include <iostream>

#include <vector>

namespace Lazarus
{

/**
 * Essentially a std::vector clone with more control over the destruction of contained elements and fancy serialization
 * support :-).
 * One can instantiate the object with a fixed size which might be increased if necessary by the method 'push'.
 * An increase in maximal capacity occurs with a factor of 2. The default size is 10.
 * The user will not see the capacity increase as it happens transparently, it can only be detected by observing
 * 'm_buffer_size'.
 * A call to push will append data after the m_size-th element, i.e. after index m_size-1.
 * We keep a generic interface with both methods 'getElement', 'getElementRef', 'pop' and 'popRef' in order
 * to allow 'KTuple' to remain thread-safe by returning copies instead of references while still being compatible with
 * 'FastKTuple'.
 */
template<class T>
class FastKTuple: public GenKTuple<T> {
public:

	enum INIT_STATE{INIT_STATE_DEFAULTCONSTRUCTOR, INIT_STATE_NONE};

	/**
	 * state determines if the elements of type T should be instantiated with their default constructor,
	 * otherwise only the required memory will be allocated.
	 * */
	FastKTuple(long long int size = 0, INIT_STATE state = INIT_STATE_DEFAULTCONSTRUCTOR)
	{
		//the case size=0 is useful if one plans to use the tuple as a stack
		//technically the tuple will always allocate at least one element
		if(size == 0)
		{
			mp_data = new T[1];

			/*enum mcheck_status status = mprobe(mp_data);
			printf("constructor1 check %d\n",status);*/

			//init the data fields
			if(state == INIT_STATE_DEFAULTCONSTRUCTOR)
				mp_data[0] = T();

			/*status = mprobe(mp_data);
			printf("constructor2 check %d\n",status);*/

			m_size = 0;
			m_buffer_size = 0;
			m_current_position = 0;

			m_initialized_empty = true;
		}
		else
		{
			mp_data = new T[size];

			//init the data fields
			if(state == INIT_STATE_DEFAULTCONSTRUCTOR)
				for(unsigned int i=0;i<size;i++)
				{
					mp_data[i] = T();
				}

			m_size = size;
			m_buffer_size = size;
			m_current_position = size;

			m_initialized_empty = false;
		}

		m_grow_factor = 2.0;

	}

	FastKTuple(const FastKTuple<T>& t)
	{
		if(t.m_initialized_empty == false)
		{
			mp_data = new T[t.m_size];
			//init the data fields
			//memcpy(mp_data,t.mp_data,sizeof(T)*t.getm_size());
			for(long long int i=0;i<t.m_size;i++)
			{
				mp_data[i] = t.mp_data[i];
			}
		}
		else
		{
			mp_data = new T[1];
		}

		m_size = t.m_size;
		m_buffer_size = t.m_buffer_size;

		m_current_position = t.m_current_position;
		m_initialized_empty = t.m_initialized_empty;

		m_grow_factor = t.m_grow_factor;
	}

	/**
	 * Pushing is still possible.
	 * */
	FastKTuple(const T* data, unsigned long long size)
	{
		m_size = size;

		mp_data = new T[m_size];

		//init the data fields
		for(unsigned int i=0;i<m_size;i++)
		{
			mp_data[i] = data[i];
		}

		m_buffer_size = m_size;
		m_current_position = m_size;

		m_initialized_empty = false;
		m_grow_factor = 2.0;
	}

	BaseObject* clone() const
	{
		return new FastKTuple<T>(*this);
	}

	/**
	 * Will compare the tuple elements via != internally.
	 * */
	bool operator==( const FastKTuple& t )
	{
		bool res = true;

		//memcpy(mp_data,t.mp_data,sizeof(T)*t.getm_size());
		for(long long int i=0;i<t.m_size;i++)
		{
			if(mp_data[i] != t.mp_data[i])
			{
				res = false;
				break;
			}
		}

		return res;
	}

	/**
	 * Pushes a new element.
	 * */
	FastKTuple<T>& operator<<(const T& t)
	{
		this->push(t);
		return *this;
	}

	/**
	 * Will compare the tuple elements via != internally.
	 * */
	bool operator==( FastKTuple& t )
	{
		bool res = true;

		//memcpy(mp_data,t.mp_data,sizeof(T)*t.getm_size());
		for(long long int i=0;i<t.m_size;i++)
		{
			if(mp_data[i] != t.mp_data[i])
			{
				res = false;
				break;
			}
		}

		return res;
	}

	/**
	 * Will compare the tuple elements via != internally.
	 * */
	bool operator!=( const FastKTuple& t )
	{
		bool res = false;

		//memcpy(mp_data,t.mp_data,sizeof(T)*t.getm_size());
		for(long long int i=0;i<t.m_size;i++)
		{
			if(mp_data[i] != t.mp_data[i])
			{
				res = true;
				break;
			}
		}

		return res;
	}

	/**
	 * Will compare the tuple elements via != internally.
	 * */
	bool operator!=( FastKTuple& t )
	{
		bool res = false;

		//memcpy(mp_data,t.mp_data,sizeof(T)*t.getm_size());
		for(long long int i=0;i<t.m_size;i++)
		{
			if(mp_data[i] != t.mp_data[i])
			{
				res = true;
				break;
			}
		}

		return res;
	}

	/**
	 * This will not delete any existing data in case of pointers.
	 * */
	FastKTuple& operator=( const FastKTuple& t )
	{
		DELETE_ARRAY_NULL_CHECKING(mp_data);

		if(t.m_initialized_empty == false)
		{
			mp_data = new T[t.m_size];
			//init the data fields
			//memcpy(mp_data,t.mp_data,sizeof(T)*t.getm_size());
			for(long long int i=0;i<t.m_size;i++)
			{
				mp_data[i] = t.mp_data[i];
			}
		}
		else
		{
			mp_data = new T[1];
		}


		m_size = t.m_size;
		m_buffer_size = t.m_buffer_size;

		m_current_position = t.m_current_position;
		m_initialized_empty = t.m_initialized_empty;

		m_grow_factor = t.m_grow_factor;

		return *this;
	}

	/**
	 * This will not delete any existing data in case of pointers.
	 * */
	FastKTuple& operator=( FastKTuple& t )
	{
		DELETE_ARRAY_NULL_CHECKING(mp_data);

		if(t.m_initialized_empty == false)
		{
			mp_data = new T[t.m_size];
			//init the data fields
			//memcpy(mp_data,t.mp_data,sizeof(T)*t.getm_size());
			for(long long int i=0;i<t.m_size;i++)
			{
				mp_data[i] = t.mp_data[i];
			}
		}
		else
		{
			mp_data = new T[1];
		}


		m_size = t.m_size;
		m_buffer_size = t.m_buffer_size;

		m_current_position = t.m_current_position;
		m_initialized_empty = t.m_initialized_empty;

		m_grow_factor = t.m_grow_factor;

		return *this;
	}

	/*
	 * this should only be used for debug purposes and especially for small tuples, thus we don't care
	 * about the switch within each iteration of the for loop.
	 */
	void printData() const
	{
		for(unsigned int i=0;i<m_size;++i)
		{
		  switch(this->data_type)
		  {
			  case SH_DATA_TYPE_DOUBLE:
				  printf("%F ", (double)mp_data[i]);
				  break;
			  case SH_DATA_TYPE_FLOAT:
				  printf("%f ", (float)mp_data[i]);
				  break;
			  case SH_DATA_TYPE_LONG_DOUBLE:
				  printf("%Lf ", (long double)mp_data[i]);
				  break;
			  case SH_DATA_TYPE_INT:
				  printf("%d ", (int)mp_data[i]);
				  break;
			  case SH_DATA_TYPE_LONGLONG:
				  printf("%lld ", (long long int)mp_data[i]);
				  break;
			  case SH_DATA_TYPE_ULONGLONG:
				  printf("%llu ", (unsigned long long)mp_data[i]);
				  break;
			  case SH_DATA_TYPE_LONG:
				  printf("%ld ", ( long int)mp_data[i]);
				  break;
			  case SH_DATA_TYPE_ULONG:
				  printf("%lu ", (unsigned long)mp_data[i]);
				  break;
			  case SH_DATA_TYPE_UINT:
				  printf("%u ", (unsigned int)mp_data[i]);
				  break;
			  case SH_DATA_TYPE_UCHAR:
				  printf("%u ", (unsigned char)mp_data[i]);
				  break;
			  case SH_DATA_TYPE_CHAR:
				  printf("%d ", (char)mp_data[i]);
				  break;
			  case SH_DATA_TYPE_SHORT:
				  printf("%hd ", (short)mp_data[i]);
				  break;
			  case SH_DATA_TYPE_USHORT:
				  printf("%hu ", (unsigned short)mp_data[i]);
				  break;
			  case SH_DATA_TYPE_ABSTRACT:
				  printf("ABSTRACT ");
				  break;
			  case SH_DATA_TYPE_UNSPECIFIED:
				  printf(" UNKNOWN ");
				  break;
		  }
		  printf("\n");
		}
	}

	/**
	 * This will not delete any existing data in case of pointers.
	 * */
	/*FastKTuple& operator=( FastKTuple&& t )
	{
		DELETE_ARRAY_NULL_CHECKING(mp_data);

		mp_data = new T[t.m_size];printf("--------------------- ASSIGNMENT OP2\n");

		//init the data fields
		//memcpy(mp_data,t.mp_data,sizeof(T)*t.getm_size());
		for(long long int i=0;i<t.m_size;i++)
		{
			mp_data[i] = t.mp_data[i];
		}

		m_size = t.m_size;
		m_buffer_size = t.m_buffer_size;

		m_current_position = t.m_current_position;
		m_initialized_empty = t.m_initialized_empty;

		m_grow_factor = t.m_grow_factor;

	    return *this;
	}*/

	virtual ~FastKTuple()
	{
		DELETE_ARRAY_NULL_CHECKING(mp_data);
	}

	/**
	 * returns a std::vector with copies of the elements of this KTuple
	 * */
	std::vector<T>* getStdVector()
	{
		std::vector<T>* vec = NULL;

		vec = new std::vector<T>(this->m_size);

		for(long long int i=0;i<m_size;i++)
		{
			vec[i] = mp_data[i];
		}

		return vec;
	}

	/**
	 * Might throw an out-of-bounds exception.
	 * Yes, it is bad practice and a design mistake to name this method identical to one in the
	 * Serialization class. Yet this will not create any problems as long as one is aware of this
	 * method shadowing.
	 */
	inline virtual T getElement(long long int index) const
	{

		//in case of an out-of-bounds case: throw exception
		if(index>m_size-1 || m_size == 0)
		{
			std::out_of_range e("");
			throw e;
		}

		return mp_data[index];
	}

	/**
	 * Might throw an out-of-bounds exception
	 */
	inline virtual T& getElementRef(long long int index)
	{

		//in case of an out-of-bounds case: throw exception
		if(index>m_size-1 || m_size == 0)
		{
			std::out_of_range e("");
			throw e;
		}


		return mp_data[index];
	}

	/**
	 * Might throw an out-of-bounds exception
	 */
	virtual T& op(long long int index)
	{

		//in case of an out-of-bounds case: throw exception
		if(index>m_size-1 || m_size == 0)
		{
			std::out_of_range e("");
			throw e;
		}

		return mp_data[index];
	}

	/**
	 * Might throw an out-of-bounds exception
	 */
	virtual const T& op(long long int index) const
	{

		//in case of an out-of-bounds case: throw exception
		if(index>m_size-1 || m_size == 0)
		{
			std::out_of_range e("");
			throw e;
		}

		return mp_data[index];
	}

	/**
	 * Might throw an out-of-bounds exception
	 */
	inline void setElement(long long int index,const T& element)
	{
		//in case of an out-of-bounds case: throw exception
		if(index>m_size-1 || m_size == 0)
		{
			std::out_of_range e("");
			throw e;
		}

		mp_data[index] = element;
	}

	/**
	 * clears the tuple of all elements.
	 * A size value of -1 will keep the size and reset all values to their default (a size of 0 is the default value).
	 */
	virtual void clear(long long int size = 0)
	{
		DELETE_ARRAY_NULL_CHECKING(mp_data);

		if(size == -1)
		{
			size = m_size;
		}

		//the case size=0 is useful if one plans to use the tuple as a stack
		//technically the tuple will always allocate at least one element
		if(size == 0)
		{
			mp_data = new T[1];

			//init the data fields
			mp_data[0] = T();

			m_size = 0;
			m_buffer_size = 0;
			m_current_position = 0;

			m_initialized_empty = true;
		}
		else
		{
			mp_data = new T[size];

			//init the data fields
			for(unsigned int i=0;i<size;i++)
			{
				mp_data[i] = T();
			}

			m_size = size;
			m_buffer_size = size;
			m_current_position = size;

			m_initialized_empty = false;
		}
	}


	/**
	 * This essentially drops all elements beyond the given count.
	 * */
	void shrink(unsigned int count)
	{
		if(count == 0)
		{
			clear(0);
		}
		else
		{
			T* new_data = new T[count];

			for(long long int i=0;i<count;i++)
			{
				new_data[i] = mp_data[i];
			}

			DELETE_ARRAY_NULL_CHECKING(mp_data);

			mp_data = new_data;

			m_size = count;
			m_buffer_size = count;
			m_current_position = count;

			m_initialized_empty = false;
		}
	}


	/**
	 * Pushes one element into the tuple, if the allocated buffer is insufficient this call will double the capacity.
	 * Initially it starts at the beginning of the tuple, should the internal position index grow larger than the
	 * initial (virtual) size, then the virtual size will be increased by one.
	 * Keep in mind that an increase (and any implied data copies) occurs only if the internal position index grows
	 * beyond the current buffer size. In the beginning the virtual size equals the buffer size, thus the first increase
	 * or doubling of buffer capacity occurs with the first push.
	 *
	 * Example: initial size = 3; -> buffer_size = 3, m_size = 3, current_position= 3
	 * push -> double buffer_size, increment m_size
	 *
	 * Diagram: |....||..................||........................| <--KTuple
	 *               ^-current_position  ^-m_size                  ^-buffer_size
	 *
	 */
	void push(const T& element)
	{
		//if the tuple was initialized with at least one element
		if(m_initialized_empty == false)
		{
			/*enum mcheck_status status = mprobe(mp_data);
			printf("push2 check %d\n",status);*/

			//in case of an out-of-bounds case: increase buffer size
			if(m_current_position > m_buffer_size-1)
			{
				long long int old_buffer_size = m_buffer_size;
				m_buffer_size = (float)m_buffer_size * m_grow_factor;//double the space

				//safety check (this ensures a strictly monotonic growth)
				if(m_buffer_size <= old_buffer_size)
				{
					m_buffer_size = m_buffer_size+1;
				}

				T* new_data = new T[m_buffer_size];

				//backup all existing data
				//memcpy(new_data,mp_data,sizeof(T)*old_buffer_size);
				for(long long int i=0;i<m_size;++i)
				{
					new_data[i] = mp_data[i];
				}

				//clean up
				DELETE_ARRAY_NULL_CHECKING(mp_data);
				mp_data = new_data;
			}

			//if the current position surpasses the virtual size
			//(after the first buffer doubling this does not happen with every 'push')
			if(m_current_position > m_size-1)
			{
				++m_size; //increase the virtual size
			}

			mp_data[m_current_position] = element;
			++m_current_position;

		}
		//if the list was initialized empty, i.e. this else-block will be entered only once when the tuple contains virtually 0 elements
		//afterwards only the if-block will be used.
		else
		{
			/*enum mcheck_status status = mprobe(mp_data);
			printf("push1 checkA %d\n",status);*/

			mp_data[0] = element;

			m_initialized_empty = false;
			m_current_position = 1;
			m_size = 1;
			m_buffer_size = 1;

			/*status = mprobe(mp_data);
			printf("push1 checkB %d\n",status);*/
		}
	}

	/**
	 * The counterpart to push, yet this does not change the buffer size or the virtual size.
	 * Might throw an out-of-range exception.
	 */
	T pop()
	{
		//in case of beginning: out-of-bounds exception
		if(m_current_position == 0)
		{
			std::out_of_range e("");
			throw e;
		}

		--m_current_position;
		return mp_data[m_current_position];
	}

	/**
	 * The counterpart to push, yet this does not change the buffer size or the virtual size.
	 * Might throw an out-of-range exception.
	 */
	T& popRef()
	{
		//in case of beginning: out-of-bounds exception
		if(m_current_position == 0)
		{
			std::out_of_range e("");
			throw e;
		}

		--m_current_position;
		return mp_data[m_current_position];
	}

	/**
	 * Returns the data
	 */
	inline T* getmp_data()
	{
		return mp_data;
	}

	/**
	 * Returns the virtual size
	 */
	inline long long int getm_size() const
	{
	   return m_size;
	}

	/**
	 * Returns the virtual size
	 */
	inline long long int size() const
	{
	   return m_size;
	}

	/**
	 * returns the index of the current insertion position
	 */
	inline long long int getm_current_position() const
	{
	   return m_current_position;
	}

	/**
	 * updates the index of the current insertion position
	 */
	inline void setm_current_position(long long int pos)
	{
	   if(pos < 0)
	   {
		   printf("ERROR: position must be >=0\n");
		   return;
	   }

	   if(pos > m_size)
	   {
		   printf("ERROR: position must be <= m_size\n");
		   return;
	   }

	   m_current_position = pos;
	}



	/**
	 * Returns the amount of allocated memory
	 */
	inline long long int getm_buffer_size() const
	{
	   return m_buffer_size;
	}

	/**
	 * Destroys each object within the tuple.
	 * This does not remove the elements from the tuple, only the objects they point to. All pointer elements will be nulled.
	 */
	virtual void eraseWithDestructor()
	{
		for(long long int i=0; i< m_size; i++)
		{
			/*if( std::is_pointer<T>::value == true )
			{
				delete (*(T**)&(mp_data[i]));
				*(T**)&(mp_data[i]) = NULL;
			}
			else
			{
				printf("ERROR: can't call destructor on stack objects\n");
			}*/

			Eraser<T>::erase(mp_data[i]);
		}
	}

	/**
	 * Destroy each object within the tuple.
	 * This does not remove the elements from the tuple, only the objects they point to. All pointer elements will be nulled.
	 */
	virtual void eraseArrayWithDestructor()
	{
		for(long long int i=0; i< m_size; i++)
		{
			/*if( std::is_pointer<T>::value == true )
			{
				delete[] (*(T**)&(mp_data[i]));
				*(T**)&(mp_data[i]) = NULL;
			}
			else
			{
				printf("ERROR: can't call destructor on stack objects\n");
			}*/

			Eraser<T>::eraseArray(mp_data[i]);
		}
	}

	/**
	 * Only change this if you really understand what's going on behind the curtain. The grow factor must be large
	 * enough to ensure a strictly monotonic growth. One very dangerous situation occurs when a KTuple was instantiated
	 * with a size of 0 and the grow factor X was set to < 2.0 before the second call to push; the increase of the buffer
	 * would encounter a new size value of 1*X < 2.0 which might result in an integer value of 1 and a segmentation faults
	 * would occur! In order to prevent this a safety measure was put in place which ensures a monotonic growth even if
	 * the user screwed up the grow factor. In case of potential errors, the size will be increased by just one slot!
	 * */
	void setGrowFactor(float factor)
	{
		m_grow_factor = factor;
	}

	/**
	 * Note that in case of elements which are non-primitive and non-serializable this method will
	 * abort.
	 * */
	void serialize()
	{
		Serializable::resetBuffer();
		Serializable::registerElement<unsigned long long int>(3);
		Serializable::registerElement<bool>(1);
		Serializable::registerElement<float>(1);

		//in case of primitive types register them via element count
		if(std::is_fundamental<T>::value == true)
		{
			Serializable::registerElement<T>(m_size);
		}
		//in case of complex types serialize them and register the byte array
		else
		{
			for(long long int i = 0; i<m_size; ++i)
			{
				//abort if objects are not serializable
				if(std::is_base_of<Serializable,T>::value == false)
				{
					Serializable::resetBuffer();
					ERROR("Can not serialize fastktuple with non serializable objects");
					return;
				}

				//serialize if possible
				AutonomSerializer< std::is_base_of<Serializable,T>::value, T> a(mp_data[i]);
				//register
				Serializable::registerUCharA(a.getSerializedData()->get_m_length());

			}
		}

		Serializable::allocateBuffer();

		//in case of primitives types do a binary copy
		if(std::is_fundamental<T>::value == true)
		{
			for(long long int i = 0; i<m_size; ++i)
			{
				Serializable::addElement<T>(mp_data[i]);
			}
		}
		else //complex types
		{
			for(long long int i = 0; i<m_size; ++i)
			{
				AutonomSerializer< std::is_base_of<Serializable,T>::value, T> a(mp_data[i],false);
				Serializable::addUCharA(a.getSerializedData()->get_mp_data(),
						a.getSerializedData()->get_m_length());
			}
		}

		Serializable::addULongLong(m_current_position);
		Serializable::addULongLong(m_size);
		Serializable::addULongLong(m_buffer_size);
		Serializable::addFloat(m_grow_factor);
		Serializable::addBool(m_initialized_empty);
	}

	void deserialize()
	{
		m_initialized_empty = Serializable::getBool();
		m_grow_factor = Serializable::getFloat();
		m_buffer_size = Serializable::getULongLong();
		m_size = Serializable::getULongLong();
		m_current_position = Serializable::getULongLong();

		DELETE_ARRAY_NULL_CHECKING(mp_data);
		mp_data = new T[m_buffer_size];

		for(long long int i = m_size-1; i>=0; --i)
		{
			Serializable::getElement<T>(mp_data[i]);
		}

		Serializable::resetBuffer();
	}

protected:
	long long int m_current_position;
	long long int m_size;
	long long int m_buffer_size;
	bool m_initialized_empty;
	T* mp_data;

	float m_grow_factor;

};


}

#endif /* KTUPLE_H_ */
