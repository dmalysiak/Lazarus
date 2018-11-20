/*
 * Buffer.cpp
 *
 *  Created on: 04.08.2011
 *      Author: nao
 */

#include "PrimitiveBuffer.h"

namespace Lazarus
{

PrimitiveBuffer::PrimitiveBuffer(unsigned char* buffer, unsigned int length)
{
	//local attributes
	mp_data_ = buffer;
	m_length_ = length;

	//parent attributes
	mp_data = buffer;
	m_length = length;
}

PrimitiveBuffer::PrimitiveBuffer()
{
	mp_data_ = NULL;
	m_length_ = 0;
}

PrimitiveBuffer::~PrimitiveBuffer()
{
	//this will prevent the parent destructor from deleting the data
	mp_data = NULL;
}

}
