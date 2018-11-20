/*
 * Buffer.h
 *
 *  Created on: 04.08.2011
 *      Author: nao
 */

#ifndef PRIMITIVEBUFFER_H_
#define PRIMITIVEBUFFER_H_

#include "BasicBuffer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


namespace Lazarus
{

/**
 * This buffer class is mainly a wrapper for char array, i.e. while providing all basic buffer functionality it will never take
 * ownership over the actual data! The attributes 'data' and 'length' of the parent class are exposed!
 * Use this class only for fast data transport to methods with 'Buffer' parameters!!
 */

class PrimitiveBuffer : public BasicBuffer {
public:

	PrimitiveBuffer(unsigned char* buffer, unsigned int length);
	PrimitiveBuffer();

	virtual ~PrimitiveBuffer();

	unsigned char* mp_data_;
	unsigned int m_length_;
};

}

#endif /* PRIMITIVEBUFFER_H_ */
