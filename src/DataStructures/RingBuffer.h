/*
 * RingBuffer.h
 *
 *  Created on: Sep 27, 2013
 *      Author: clustro
 */

#ifndef RINGBUFFERO_H_
#define RINGBUFFERO_H_

#include "../BaseObject.h"

#include <pthread.h>

namespace Lazarus {

//pre-declaration of the class (needed for friend relationship)
template<class T> class RingBufferHelper;

template<typename T>
class RingBuffer: public BaseObject
{
public:

	friend class Lazarus::RingBufferHelper<T>;

	RingBuffer(unsigned int size)
	{
		m_size = size;
		mp_data = new T[m_size];

		//init the array
		for(unsigned int i=0;i<m_size;i++)
		{
			mp_data[i]=0;
		}

		m_current_write_index = 0;
		m_current_read_index = 0;

		pthread_mutex_init(&m_mutex,NULL);
		m_element_counter = 0;
	}

	/*
	 * copy constructor which creates a >new< ringbuffer with the size of the original buffer
	 */
	RingBuffer(const RingBuffer<T>& buffer)
	{
		m_size = buffer.getm_size();
		mp_data = new T[m_size];

		//init the array
		for(unsigned int i=0;i<m_size;i++)
		{
			mp_data[i]=0;
		}

		m_current_write_index = 0;
		m_current_read_index = 0;

		pthread_mutex_init(&m_mutex,NULL);
		m_element_counter = 0;
	}

	virtual ~RingBuffer()
	{
		DELETE_ARRAY_NULL_CHECKING(mp_data);

		pthread_mutex_destroy(&m_mutex);
	}

	unsigned int getm_size() const
	{
		return m_size;
	}


	virtual int writeElement(T data)
	{
		pthread_mutex_lock(&m_mutex);

		//if buffer full -> increment read index as well
		if(m_element_counter >= m_size)
		{
			m_current_read_index++;
			m_current_read_index = m_current_read_index % m_size;
		}

		mp_data[ m_current_write_index ] = data;

		//increment the current write index
		m_current_write_index++;
		m_current_write_index = m_current_write_index % m_size;

		//one element more in the buffer
		m_element_counter++;

		pthread_mutex_unlock(&m_mutex);

		return 0;
	}

	virtual int writeElementArray(T* data, unsigned int size)
	{
		pthread_mutex_lock(&m_mutex);

		//if buffer is not large enough
		if( m_size < size )
		{
			pthread_mutex_unlock(&m_mutex);
			return -1;
		}

		//if buffer full -> shift the read index
		if(m_element_counter + size >= m_size)
		{
			m_current_read_index += (m_element_counter + size - m_size);
			m_current_read_index = m_current_read_index % m_size;
		}

		//determine the break around fragment for memcpy
		unsigned int first_frag_size = 0;
		unsigned int second_frag_size = 0;

		//if array does not fit into the internal array in one sequential piece
		if( m_current_write_index + size > m_size )
		{
			first_frag_size = m_size - m_current_write_index;
			second_frag_size = size - first_frag_size;
			memcpy(mp_data + m_current_write_index, data, first_frag_size * sizeof(T));
			memcpy(mp_data, data + first_frag_size, second_frag_size * sizeof(T));
		}//otherwise just do a copy
		else
		{
			memcpy(mp_data + m_current_write_index, data, size * sizeof(T));
		}

		//increment the current write index
		m_current_write_index += size;
		m_current_write_index = m_current_write_index % m_size;

		//one element more in the buffer
		m_element_counter++;

		pthread_mutex_unlock(&m_mutex);

		return 0;
	}

	virtual T readElement()
	{
		T data = 0;

		pthread_mutex_lock(&m_mutex);

		//if buffer empty (<=> read_index == write_index) -> return NULL
		if(m_element_counter == 0)
		{
			pthread_mutex_unlock(&m_mutex);
			return data;
		}

		data = mp_data[m_current_read_index];

		mp_data[m_current_read_index] = 0;

		m_current_read_index++;
		m_current_read_index = m_current_read_index % m_size;

		pthread_mutex_unlock(&m_mutex);

		return data;
	}

	virtual T readElementArray(unsigned int size)
	{
		T* data = 0;

		pthread_mutex_lock(&m_mutex);

		//if buffer does not contain as many items -> return NULL
		if(m_element_counter <= size)
		{
			pthread_mutex_unlock(&m_mutex);
			return data;
		}

		//create the return buffer
		data = new T[size];

		//determine the break around fragment for memcpy
		unsigned int first_frag_size = 0;
		unsigned int second_frag_size = 0;

		//if array does not fit into the internal array in one sequential piece
		if( m_current_read_index + size > m_size )
		{
			first_frag_size = m_size - m_current_read_index;
			second_frag_size = size - first_frag_size;

			//copy the fragments
			memcpy(data, mp_data + m_current_read_index, first_frag_size * sizeof(T));
			memcpy(data + first_frag_size, mp_data, second_frag_size * sizeof(T));

			//mark the slot as empty
			memset(mp_data+m_current_read_index,0,first_frag_size*sizeof(T));
			memset(mp_data,0,second_frag_size*sizeof(T));

		}//otherwise just do a copy
		else
		{
			memcpy(data, mp_data + m_current_read_index, size * sizeof(T));

			//mark the slot as empty
			memset(mp_data+m_current_read_index,0,size*sizeof(T));
		}


		m_current_read_index += size;
		m_current_read_index = m_current_read_index % m_size;

		pthread_mutex_unlock(&m_mutex);

		return data;
	}

protected:
	unsigned int m_size;
	T* mp_data;
	unsigned int m_current_write_index;
	unsigned int m_current_read_index;
	pthread_mutex_t m_mutex;
	unsigned int m_element_counter;

};

template<typename T>
class RingBufferHelper: public Lazarus::BaseObject
{
public:
	RingBufferHelper()
	{
	}

	virtual ~RingBufferHelper()
	{
	}

	/*
	 * destroy each object within the buffer.
	 * !! CAUTION: this only works when the elements in the list are pointers to heap allocated objects!
	 */
	static void eraseWithDestructor(RingBuffer<T>* buffer)
	{
		pthread_rwlock_wrlock(&buffer->m_mutex);

		for(unsigned int i=0; i<buffer->m_size; i++)
		{
			if(buffer->mp_data[i] != 0)
			{
				delete buffer->mp_data[i];
				buffer->mp_data[i] = 0;
			}
		}

		pthread_rwlock_unlock(&buffer->m_mutex);
	}

	/*
	 * destroy each object within the buffer.
	 * !! CAUTION: this only works when the elements in the list are pointers to heap allocated array objects!
	 */
	static void eraseArrayWithDestructor(RingBuffer<T>* buffer)
	{
		pthread_rwlock_wrlock(&buffer->m_mutex);

		for(unsigned int i=0; i<buffer->m_size; i++)
		{
			if(buffer->mp_data[i] != 0)
			{
				delete[] buffer->mp_data[i];
				buffer->mp_data[i] = 0;
			}
		}

		pthread_rwlock_unlock(&buffer->m_mutex);
	}

};

} /* namespace Lazarus */
#endif /* RINGBUFFERO_H_ */
