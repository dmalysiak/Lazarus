/*
 * Buffer.h
 *
 *  Created on: 06.03.2013
 *      Author: Darius Malysiak
 */

#ifndef BASICBUFFER_H_
#define BASICBUFFER_H_

#include "BaseObject.h"

#include <string>

namespace Lazarus
{

class BasicBuffer: public BaseObject {
public:

	enum BUFFER_DATA_TYPE{BUFFER_DATA_TYPE_UCHAR, BUFFER_DATA_TYPE_UINT,BUFFER_DATA_TYPE_FLOAT,BUFFER_DATA_TYPE_DOUBLE,BUFFER_DATA_TYPE_ABSTRACT};

	BasicBuffer();
	BasicBuffer(unsigned int length);
	virtual ~BasicBuffer();

	virtual unsigned int get_m_length() const;
	virtual unsigned char* get_mp_data() const;

	/**
	 * This method increases/decreases the buffers size via 'memcpy'.
	 * Shrinking: will keep the first bytes of original data.
	 * Growing: will keep all original bytes and leave the new ones uninitialized.
	 */
	void changeSize(unsigned int length);

	/**
	 * This method increases/decreases the buffers size via 'memcpy'.
	 * Shrinking: will keep the first bytes of original data.
	 * Growing: will keep all original bytes and will initialize the new ones to zero.
	 */
	void changeSizeZero(unsigned int length);

	/**
	 * Prints the buffer with the assumption of mp_data containing type-values. This might lead to serious out of bounds
	 * errors, i.e. segmentation faults. The format will be 'data value ' for each element.
	 */
	virtual void printfBuffer(enum BUFFER_DATA_TYPE type = BUFFER_DATA_TYPE_UCHAR) const;

	/**
	 * Prints each element of the buffer as char without spaces.
	 */
	virtual void printfBufferChar() const;

	/**
	 * Prints the buffer in form of 0x?? hex-segments
	 */
	virtual void printfRawBuffer() const;

	/**
	 * Prints the buffer in form of ?? hex-segments
	 */
	virtual void printfRawBufferHex() const;

	/**
	 * These methods will read/write the data straight to a file just as it is. The text-methods will read/write the data as
	 * unsigned chars!
	 */
	virtual void writeBufferToBinaryFile(const std::string& filename) const;
	virtual void readBufferFromBinaryFile(const std::string& filename);

	virtual void writeBufferToTextFile(const std::string& filename) const;
	virtual void readBufferFromTextFile(const std::string& filename);

protected:
	unsigned char* mp_data;
	unsigned int m_length;
};

}

#endif /* BASICBUFFER_H_ */
