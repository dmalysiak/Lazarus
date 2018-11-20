/*
 * Buffer.cpp
 *
 *  Created on: 06.03.2013
 *      Author: Darius Malysiak
 */

#include "Buffer.h"

#include <stdio.h>
#include <string.h>

namespace Lazarus
{

Buffer::Buffer(unsigned char* buffer, unsigned int length, bool deep_copy) {
	m_length = length;

	if(deep_copy == false)
	{
		mp_data = buffer;
	}
	else
	{
		mp_data = new unsigned char[length];
		memcpy(mp_data,buffer,length);
	}
}

Buffer::Buffer(unsigned int length) {

	m_length = length;
	mp_data = new unsigned char[length];

}

Buffer::Buffer(Buffer* buffer) {

	m_length = buffer->get_m_length();

	mp_data = new unsigned char[m_length];
	memcpy(mp_data,buffer->get_mp_data(),m_length);

}

Buffer::Buffer()
{
	m_length = 0;
	mp_data = NULL;
}

Buffer::~Buffer() {
	//BasicBuffers destructor will delete the data
}

void Buffer::set_mp_data(unsigned char* buffer, unsigned int length)
{
	DELETE_ARRAY_NULL_CHECKING(mp_data);
	mp_data = buffer;
	m_length = length;
}

void Buffer::resetBuffer(unsigned int length)
{
	DELETE_ARRAY_NULL_CHECKING(mp_data);

	m_length = length;
	mp_data = new unsigned char[length];

}

}
