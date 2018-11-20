/*
 * Buffer.h
 *
 *  Created on: 06.03.2013
 *      Author: Darius Malysiak
 */

#ifndef BUFFER_H_
#define BUFFER_H_

#include "BasicBuffer.h"

#include <string>

namespace Lazarus
{

class Buffer: public BasicBuffer {
public:
	//this constructor takes ownership of the buffer if deep_copy == false, otherwise a copy of the data is made!
	Buffer(unsigned char* buffer, unsigned int length, bool deep_copy=false);
	Buffer(unsigned int length);
	//does a deep copy
	Buffer(Buffer* buffer);
	Buffer();
	virtual ~Buffer();

	void resetBuffer(unsigned int length);

	//this method takes ownership of the buffer, any existing data is deleted.
	void set_mp_data(unsigned char* buffer, unsigned int length);
};

}

#endif /* BUFFER_H_ */
