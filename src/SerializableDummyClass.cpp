/*
 * SerializableDummyClass.cpp
 *
 *  Created on: 21.03.2013
 *      Author: Darius Malysiak
 */

#include "SerializableDummyClass.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

namespace Lazarus
{

SerializableDummyClass::SerializableDummyClass(int value, unsigned int array_size) {

	m_array_size = array_size;
	m_value = value;
	mp_long_array = new unsigned char[array_size];

	//init with random data
	for(unsigned int i=0;i < m_array_size;i++)
	{
		mp_long_array[i] = 'a'+rand()%26;
	}


}

SerializableDummyClass::~SerializableDummyClass() {

	delete[] mp_long_array;

}

void SerializableDummyClass::serialize()
{

	mp_serialized_data_buffer = new Buffer(sizeof(int)+sizeof(unsigned int)+m_array_size);

	memcpy(mp_serialized_data_buffer->get_mp_data(),&m_value,sizeof(int));
	memcpy(mp_serialized_data_buffer->get_mp_data() + sizeof(int),&m_array_size,sizeof(unsigned int));
	memcpy(mp_serialized_data_buffer->get_mp_data() + sizeof(int) + sizeof(unsigned int),mp_long_array,m_array_size);

}

void SerializableDummyClass::deserialize()
{

	memcpy(&m_value,mp_serialized_data_buffer->get_mp_data(),sizeof(int));
	memcpy(&m_array_size,mp_serialized_data_buffer->get_mp_data() + sizeof(int),sizeof(unsigned int));

	//allocate memory for array
	DELETE_ARRAY_NULL_CHECKING(mp_long_array);
	mp_long_array = new unsigned char[m_array_size];

	memcpy(mp_long_array,mp_serialized_data_buffer->get_mp_data() + sizeof(int) + sizeof(unsigned int),m_array_size);

}

void SerializableDummyClass::printValue() const
{
	printf("Serializable dummy with value %d, array size: %d and following data\n",m_value,m_array_size);

	for(unsigned int i=0;i<m_array_size;i++)
	{
		printf("%c ",mp_long_array[i]);
		if(i % 50 == 0 && i!=0)
		{
			printf("\n");
		}
	}
	printf("\n");
}

}
