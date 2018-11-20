/*
 * SerializableDummyClass.h
 *
 *  Created on: 21.03.2013
 *      Author: Darius Malysiak
 */

#ifndef SERIALIZABLEDUMMYCLASS_H_
#define SERIALIZABLEDUMMYCLASS_H_

#include "Serializable.h"

namespace Lazarus
{

class SerializableDummyClass: public Serializable {
public:
	SerializableDummyClass(int value, unsigned int array_size);
	virtual ~SerializableDummyClass();

	virtual void serialize();
	virtual void deserialize();

	void printValue() const;

	CLASS_RTTI(Lazarus::SerializableDummyClass)

private:
	int m_value;
	unsigned int m_array_size;
	unsigned char* mp_long_array;
};

}

#endif /* SERIALIZABLEDUMMYCLASS_H_ */
