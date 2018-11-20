/*
 * RingBuffer.h
 *
 *  Created on: Sep 27, 2013
 *      Author: clustro
 */

#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

#include "RingBuffer.h"

namespace Lazarus {

template<typename T>
class RingBufferO : public RingBuffer<T>
{
public:

	enum RINGBUFFERO_DATA_TYPE{DATA_TYPE_PRIMITIVE, DATA_TYPE_ABSTRACT, DATA_TYPE_ABSTRACT_ARRAY};

	RingBufferO(unsigned int size, enum DATA_TYPE type) : RingBuffer(size)
	{
		m_buffer_type = type;
	}

	/*
	 * the copy constructor creates a >new< instance of the class with the same type as the original object
	 */
	RingBufferO(const RingBufferO<T>& buffer) : RingBuffer(buffer.getm_size())
	{
		m_buffer_type = buffer.getm_buffer_type();
	}

	virtual ~RingBufferO()
	{
	}

	enum RINGBUFFERO_DATA_TYPE getm_buffer_type() const
	{
		return m_buffer_type;
	}

	//this method takes the ownership over any abstract type
	virtual int writeElement(T data)
	{
		pthread_mutex_lock(&m_mutex);

		//if buffer full -> increment read index as well
		if(m_element_counter >= m_size)
		{
			m_current_read_index++;
			m_current_read_index = m_current_read_index % m_size;
		}

		//delete any existing abstract data
		if(m_type == DATA_TYPE_ABSTRACT)
		{
			if(mp_data[ m_current_write_index ] != 0)
			{
				delete mp_data[ m_current_write_index ];
			}
		}
		if(m_type == DATA_TYPE_ABSTRACT_ARRAY)
		{
			if(mp_data[ m_current_write_index ] != 0)
			{
				delete[] mp_data[ m_current_write_index ];
			}
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

	//this method takes the ownership over any abstract type
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

			//free any existing memory
			for(unsigned int i=0;i<first_frag_size;i++)
			{
				if(m_type == DATA_TYPE_ABSTRACT)
				{
					if(mp_data[m_current_write_index + i] != 0)
					{
						delete mp_data[m_current_write_index + i];
					}
				}
				if(m_type == DATA_TYPE_ABSTRACT_ARRAY)
				{
					if(mp_data[m_current_write_index +i] != 0)
					{
						delete[] mp_data[m_current_write_index + i];
					}
				}
			}

			for(unsigned int i=0;i<second_frag_size;i++)
			{
				if(m_type == DATA_TYPE_ABSTRACT)
				{
					if(mp_data[i] != 0)
					{
						delete mp_data[i];
					}
				}
				if(m_type == DATA_TYPE_ABSTRACT_ARRAY)
				{
					if(mp_data[i] != 0)
					{
						delete[] mp_data[i];
					}
				}
			}

			memcpy(mp_data + m_current_write_index, data, first_frag_size * sizeof(T));
			memcpy(mp_data, data + first_frag_size, second_frag_size * sizeof(T));
		}//otherwise just do a copy
		else
		{
			//free any existing memory
			for(unsigned int i=0;i<size;i++)
			{
				if(m_type == DATA_TYPE_ABSTRACT)
				{
					if(mp_data[m_current_write_index + i] != 0)
					{
						delete mp_data[m_current_write_index + i];
					}
				}
				if(m_type == DATA_TYPE_ABSTRACT_ARRAY)
				{
					if(mp_data[m_current_write_index +i] != 0)
					{
						delete[] mp_data[m_current_write_index + i];
					}
				}
			}

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


private:
	enum RINGBUFFERO_DATA_TYPE m_buffer_type;

};

} /* namespace Lazarus */
#endif /* RINGBUFFER_H_ */
