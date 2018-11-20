/*
 * Serializable.cpp
 *
 *  Created on: 20.02.2013
 *      Author: Darius Malysiak
 */

#include "Serializable.h"

#include <string.h>
#include <stdio.h>

namespace Lazarus {

Serializable::Serializable() {

	mp_serialized_data_buffer = NULL;
	m_serialization_offset = 0;
	m_required_size = 0;

}

Serializable::~Serializable() {

	DELETE_NULL_CHECKING(mp_serialized_data_buffer);

}

void Serializable::allocateBuffer()
{
	DELETE_NULL_CHECKING(mp_serialized_data_buffer);
	mp_serialized_data_buffer = new Buffer(m_required_size);
	m_required_size = 0;
}

void Serializable::resetBuffer()
{
	DELETE_NULL_CHECKING(mp_serialized_data_buffer);
	m_serialization_offset = 0;
	m_required_size = 0;
}

void Serializable::createEmptyBuffer(unsigned int size)
{
	DELETE_NULL_CHECKING(mp_serialized_data_buffer);

	mp_serialized_data_buffer = new Buffer(size);
}

//takes ownership of the data buffer
void Serializable::set_mp_serialized_data_buffer(unsigned char* buffer, unsigned long long size)
{
	DELETE_NULL_CHECKING(mp_serialized_data_buffer);
	mp_serialized_data_buffer = new Buffer(buffer,size);
	m_serialization_offset = mp_serialized_data_buffer->get_m_length();
	m_required_size = 0;
}

//takes ownership of the data buffer
void Serializable::set_mp_serialized_data_buffer(Buffer* buffer)
{
	DELETE_NULL_CHECKING(mp_serialized_data_buffer);
	m_serialization_offset = buffer->get_m_length();
	mp_serialized_data_buffer = buffer;
	m_required_size = 0;
}

//does not take ownership of the data buffer
void Serializable::set_mp_serialized_data_buffer_by_copy(Buffer* buffer)
{
	DELETE_NULL_CHECKING(mp_serialized_data_buffer);
	m_serialization_offset = buffer->get_m_length();
	mp_serialized_data_buffer = new Buffer(buffer);
	m_required_size = 0;
}

Buffer* Serializable::get_mp_serialized_data_buffer() const
{
	return mp_serialized_data_buffer;
}


void Serializable::addFloat(float value)
{
	if(m_serialization_offset + sizeof(float) > mp_serialized_data_buffer->get_m_length())
	{
		printf("ERROR addFloat: could not add serialization data\n");
		return;
	}

	memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, &value, sizeof(float));

	m_serialization_offset += sizeof(float);
}

void Serializable::addDouble(double value)
{
	if(m_serialization_offset + sizeof(double) > mp_serialized_data_buffer->get_m_length())
	{
		printf("ERROR addDouble: could not add serialization data\n");
		return;
	}

	memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, &value, sizeof(double));

	m_serialization_offset += sizeof(double);
}

void Serializable::addInt(int value)
{
	if(m_serialization_offset + sizeof(int) > mp_serialized_data_buffer->get_m_length())
	{
		printf("ERROR addInt: could not add serialization data\n");
		return;
	}

	memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, &value, sizeof(int));

	m_serialization_offset += sizeof(int);
}

void Serializable::addUInt(unsigned int value)
{
	if(m_serialization_offset + sizeof(unsigned int) > mp_serialized_data_buffer->get_m_length())
	{
		printf("ERROR addUInt: could not add serialization data\n");
		return;
	}

	memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, &value, sizeof(unsigned int));

	m_serialization_offset += sizeof(unsigned int);
}

void Serializable::addShort(short value)
{
	if(m_serialization_offset + sizeof(short) > mp_serialized_data_buffer->get_m_length())
	{
		printf("ERROR addInt: could not add serialization data\n");
		return;
	}

	memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, &value, sizeof(short));

	m_serialization_offset += sizeof(short);
}

void Serializable::addUShort(unsigned short value)
{
	if(m_serialization_offset + sizeof(unsigned short) > mp_serialized_data_buffer->get_m_length())
	{
		printf("ERROR addUInt: could not add serialization data\n");
		return;
	}

	memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, &value, sizeof(unsigned short));

	m_serialization_offset += sizeof(unsigned short);
}

void Serializable::addLong(long value)
{
	if(m_serialization_offset + sizeof(long) > mp_serialized_data_buffer->get_m_length())
	{
		printf("ERROR addLong: could not add serialization data\n");
		return;
	}

	memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, &value, sizeof(long));

	m_serialization_offset += sizeof(long);
}

void Serializable::addULong(unsigned long value)
{
	if(m_serialization_offset + sizeof(unsigned long) > mp_serialized_data_buffer->get_m_length())
	{
		printf("ERROR addULong: could not add serialization data\n");
		return;
	}

	memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, &value, sizeof(unsigned long));

	m_serialization_offset += sizeof(unsigned long);
}

void Serializable::addLongLong(long long value)
{
	if(m_serialization_offset + sizeof(long long) > mp_serialized_data_buffer->get_m_length())
	{
		printf("ERROR addLongLong: could not add serialization data\n");
		return;
	}

	memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, &value, sizeof(long long));

	m_serialization_offset += sizeof(long long);
}

void Serializable::addULongLong(unsigned long long value)
{
	if(m_serialization_offset + sizeof(unsigned long long) > mp_serialized_data_buffer->get_m_length())
	{
		printf("ERROR addULongLong: could not add serialization data\n");
		return;
	}

	memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, &value, sizeof(unsigned long long));

	m_serialization_offset += sizeof(unsigned long long);
}

void Serializable::addChar(char value)
{
	if(m_serialization_offset + sizeof(char) > mp_serialized_data_buffer->get_m_length())
	{
		printf("ERROR addChar: could not add serialization data\n");
		return;
	}

	memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, &value, sizeof(char));

	m_serialization_offset += sizeof(char);
}

void Serializable::addUChar(unsigned char value)
{
	if(m_serialization_offset + sizeof(unsigned char) > mp_serialized_data_buffer->get_m_length())
	{
		printf("ERROR addUChar: could not add serialization data\n");
		return;
	}

	memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, &value, sizeof(unsigned char));

	m_serialization_offset += sizeof(unsigned char);
}

void Serializable::addBool(bool value)
{
	if(m_serialization_offset + sizeof(bool) > mp_serialized_data_buffer->get_m_length())
	{
		printf("ERROR addBool: could not add serialization data\n");
		return;
	}

	memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, &value, sizeof(bool));

	m_serialization_offset += sizeof(bool);
}

void Serializable::addString(const std::string& value)
{
	if(m_serialization_offset + strlen(value.c_str())+1 > mp_serialized_data_buffer->get_m_length())
	{
		printf("ERROR addString: could not add serialization data\n");
		return;
	}

	memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, value.c_str(), strlen(value.c_str())+1);

	m_serialization_offset += strlen(value.c_str())+1;

	unsigned int length = strlen(value.c_str())+1;

	addUInt(length);
}

void Serializable::addCharA(char* value, unsigned long long length)
{
	if(m_serialization_offset + length > mp_serialized_data_buffer->get_m_length())
	{
		printf("ERROR addCharA: could not add serialization data\n");
		return;
	}

	memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, value, length);

	m_serialization_offset += length;

	addULongLong(length);
}

void Serializable::addUCharA(unsigned char* value, unsigned long long length)
{
	if(m_serialization_offset + length > mp_serialized_data_buffer->get_m_length())
	{
		printf("ERROR addUCharA: could not add serialization data\n");
		return;
	}

	memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, value, length);

	m_serialization_offset += length;

	addULongLong(length);
}


float Serializable::getFloat()
{
	if(m_serialization_offset < sizeof(float) )
	{
		printf("ERROR getFloat: could not retrieve serialization data\n");
		return 0;
	}

	float out = 0;

	m_serialization_offset -= sizeof(float);

	memcpy(&out, mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, sizeof(float));

	return out;
}

double Serializable::getDouble()
{
	if(m_serialization_offset < sizeof(double) )
	{
		printf("ERROR getDouble: could not retrieve serialization data\n");
		return 0;
	}

	double out = 0;

	m_serialization_offset -= sizeof(double);

	memcpy(&out, mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, sizeof(double));

	return out;
}

int Serializable::getInt()
{
	if(m_serialization_offset < sizeof(int))
	{
		printf("ERROR getInt: could not retrieve serialization data\n");
		return 0;
	}

	int out = 0;

	m_serialization_offset -= sizeof(int);

	memcpy(&out, mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, sizeof(int));

	return out;
}

unsigned int Serializable::getUInt()
{
	if(m_serialization_offset < sizeof(unsigned int) )
	{
		printf("ERROR getUInt: could not retrieve serialization data\n");
		return 0;
	}

	unsigned int out = 0;

	m_serialization_offset -= sizeof(unsigned int);

	memcpy(&out, mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, sizeof(unsigned int));

	return out;
}


short Serializable::getShort()
{
	if(m_serialization_offset < sizeof(short))
	{
		printf("ERROR getInt: could not retrieve serialization data\n");
		return 0;
	}

	int out = 0;

	m_serialization_offset -= sizeof(short);

	memcpy(&out, mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, sizeof(short));

	return out;
}

unsigned short Serializable::getUShort()
{
	if(m_serialization_offset < sizeof(unsigned short) )
	{
		printf("ERROR getUInt: could not retrieve serialization data\n");
		return 0;
	}

	unsigned int out = 0;

	m_serialization_offset -= sizeof(unsigned short);

	memcpy(&out, mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, sizeof(unsigned short));

	return out;
}

long Serializable::getLong()
{
	if(m_serialization_offset < sizeof(long) )
	{
		printf("ERROR getLong: could not retrieve serialization data\n");
		return 0;
	}

	long out = 0;

	m_serialization_offset -= sizeof(long);

	memcpy(&out, mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, sizeof(long));

	return out;
}

unsigned long Serializable::getULong()
{
	if(m_serialization_offset<  sizeof(unsigned long) )
	{
		printf("ERROR getULong: could not retrieve serialization data\n");
		return 0;
	}

	unsigned long out = 0;

	m_serialization_offset -= sizeof(unsigned long);

	memcpy(&out, mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, sizeof(unsigned long));

	return out;
}

long long Serializable::getLongLong()
{
	if(m_serialization_offset <  sizeof(long long))
	{
		printf("ERROR getLongLong: could not retrieve serialization data\n");
		return 0;
	}

	long long out = 0;

	m_serialization_offset -= sizeof(long long);

	memcpy(&out, mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, sizeof(long long));

	return out;
}

unsigned long long Serializable::getULongLong()
{
	if(m_serialization_offset< sizeof(unsigned long long) )
	{
		printf("ERROR getULongLong: could not retrieve serialization data\n");
		return 0;
	}

	unsigned long long out = 0;

	m_serialization_offset -= sizeof(unsigned long long);

	memcpy(&out, mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, sizeof(unsigned long long));

	return out;
}

char Serializable::getChar()
{
	if(m_serialization_offset<  sizeof(char) )
	{
		printf("ERROR getChar: could not retrieve serialization data\n");
		return 0;
	}

	char out = 0;

	m_serialization_offset -= sizeof(char);

	memcpy(&out, mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, sizeof(char));

	return out;
}

unsigned char Serializable::getUChar()
{
	if(m_serialization_offset < sizeof(unsigned char) )
	{
		printf("ERROR getUChar: could not retrieve serialization data\n");
		return 0;
	}

	unsigned char out = 0;

	m_serialization_offset -= sizeof(unsigned char);

	memcpy(&out, mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, sizeof(unsigned char));

	return out;
}

bool Serializable::getBool()
{
	if(m_serialization_offset<  sizeof(bool) )
	{
		printf("ERROR getBool: could not retrieve serialization data\n");
		return 0;
	}

	char out = 0;

	m_serialization_offset -= sizeof(bool);

	memcpy(&out, mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, sizeof(bool));

	return out;
}

std::string Serializable::getString()
{
	unsigned int length = getUInt();

	char* buff = new char[length];

	//case length=0: always fail (string always has a termination \0)! case length>0: trivial
	if(m_serialization_offset < length )
	{
		printf("ERROR getString: could not retrieve serialization data\n");
		return "";
	}

	m_serialization_offset -= length;

	memcpy(buff,mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, length);

	std::string s(buff);

	delete[] buff;

	return s;
}

char* Serializable::getCharA(unsigned long long& size)
{
	size = getULongLong();

	//case length=0: no harm done! case length>0: trivial
	if(m_serialization_offset < size && size>0 )
	{
		printf("ERROR getCharA: could not retrieve serialization data\n");
		return NULL;
	}

	char* buff = new char[size];

	m_serialization_offset -= size;

	memcpy(buff,mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, size);

	return buff;
}

unsigned char* Serializable::getUCharA(unsigned long long& size)
{
	size = getULongLong();

	//case length=0: no harm done! case length>0: trivial
	if(m_serialization_offset < size && size>0 )
	{
		printf("ERROR getUCharA: could not retrieve serialization data\n");
		return NULL;
	}

	unsigned char* buff = new unsigned char[size];

	m_serialization_offset -= size;

	memcpy(buff,mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, size);

	return buff;
}

//**************** slotted helper methods **************************

void Serializable::addFloatSlot(float value,unsigned int slot_size)
{
	//check if slot fits into the buffer and if slot is large enough for the data
	if(m_serialization_offset + slot_size > mp_serialized_data_buffer->get_m_length() ||
			slot_size < sizeof(float))
	{
		printf("ERROR addFloatSlot: could not add serialization data\n");
		return;
	}

	memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, &value, sizeof(float));

	m_serialization_offset += slot_size;
}

void Serializable::addDoubleSlot(double value,unsigned int slot_size)
{
	//check if slot fits into the buffer and if slot is large enough for the data
	if(m_serialization_offset + slot_size > mp_serialized_data_buffer->get_m_length() ||
			slot_size < sizeof(double))
	{
		printf("ERROR addDoubleSlot: could not add serialization data\n");
		return;
	}

	memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, &value, sizeof(double));

	m_serialization_offset += slot_size;
}

void Serializable::addIntSlot(int value,unsigned int slot_size)
{
	//check if slot fits into the buffer and if slot is large enough for the data
	if(m_serialization_offset + slot_size > mp_serialized_data_buffer->get_m_length() ||
			slot_size < sizeof(int))
	{
		printf("ERROR addIntSlot: could not add serialization data\n");
		return;
	}

	memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, &value, sizeof(int));

	m_serialization_offset += slot_size;
}

void Serializable::addUIntSlot(unsigned int value,unsigned int slot_size)
{
	//check if slot fits into the buffer and if slot is large enough for the data
	if(m_serialization_offset + slot_size > mp_serialized_data_buffer->get_m_length() ||
			slot_size < sizeof(unsigned int))
	{
		printf("ERROR addUIntSlot: could not add serialization data\n");
		return;
	}

	memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, &value, sizeof(unsigned int));

	m_serialization_offset += slot_size;
}

void Serializable::addLongSlot(long value,unsigned int slot_size)
{
	//check if slot fits into the buffer and if slot is large enough for the data
	if(m_serialization_offset + slot_size > mp_serialized_data_buffer->get_m_length() ||
			slot_size < sizeof(long))
	{
		printf("ERROR addLongSlot: could not add serialization data\n");
		return;
	}

	memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, &value, sizeof(long));

	m_serialization_offset += slot_size;
}

void Serializable::addULongSlot(unsigned long value,unsigned int slot_size)
{
	//check if slot fits into the buffer and if slot is large enough for the data
	if(m_serialization_offset + slot_size > mp_serialized_data_buffer->get_m_length() ||
			slot_size < sizeof(unsigned long))
	{
		printf("ERROR addULongSlot: could not add serialization data\n");
		return;
	}

	memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, &value, sizeof(unsigned long));

	m_serialization_offset += slot_size;
}

void Serializable::addLongLongSlot(long long value,unsigned int slot_size)
{
	//check if slot fits into the buffer and if slot is large enough for the data
	if(m_serialization_offset + slot_size > mp_serialized_data_buffer->get_m_length() ||
			slot_size < sizeof(long long))
	{
		printf("ERROR addLongLongSlot: could not add serialization data\n");
		return;
	}

	memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, &value, sizeof(long long));

	m_serialization_offset += slot_size;
}

void Serializable::addULongLongSlot(unsigned long long value,unsigned int slot_size)
{
	//check if slot fits into the buffer and if slot is large enough for the data
	if(m_serialization_offset + slot_size > mp_serialized_data_buffer->get_m_length() ||
			slot_size < sizeof(unsigned long long))
	{
		printf("ERROR addULongLongSlot: could not add serialization data\n");
		return;
	}

	memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, &value, sizeof(unsigned long long));

	m_serialization_offset += slot_size;
}

void Serializable::addCharSlot(char value,unsigned int slot_size)
{
	//check if slot fits into the buffer and if slot is large enough for the data
	if(m_serialization_offset + slot_size > mp_serialized_data_buffer->get_m_length() ||
			slot_size < sizeof(char))
	{
		printf("ERROR addCharSlot: could not add serialization data\n");
		return;
	}

	memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, &value, sizeof(char));

	m_serialization_offset += slot_size;
}

void Serializable::addUCharSlot(unsigned char value,unsigned int slot_size)
{
	//check if slot fits into the buffer and if slot is large enough for the data
	if(m_serialization_offset + slot_size > mp_serialized_data_buffer->get_m_length() ||
			slot_size < sizeof(unsigned char))
	{
		printf("ERROR addUCharSlot: could not add serialization data\n");
		return;
	}

	memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, &value, sizeof(unsigned char));

	m_serialization_offset += slot_size;
}

void Serializable::addStringSlot(const std::string& value,unsigned int slot_size)
{
	//check if slot fits into the buffer and if slot is large enough for the data
	if(m_serialization_offset + slot_size > mp_serialized_data_buffer->get_m_length() ||
			slot_size < strlen(value.c_str())+1)
	{
		printf("ERROR addStringSlot: could not add serialization data\n");
		return;
	}

	memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, value.c_str(), strlen(value.c_str())+1);

	m_serialization_offset += slot_size;

}

void Serializable::addCharASlot(char* value, unsigned long long length,unsigned long long slot_size)
{
	//check if slot fits into the buffer and if slot is large enough for the data
	if(m_serialization_offset + slot_size > mp_serialized_data_buffer->get_m_length() ||
			slot_size < sizeof(char)*length)
	{
		printf("ERROR addCharASlot: could not add serialization data\n");
		return;
	}

	memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, value, length);

	m_serialization_offset += slot_size;
}

void Serializable::addUCharASlot(unsigned char* value, unsigned long long length,unsigned long long slot_size)
{
	//check if slot fits into the buffer and if slot is large enough for the data
	if(m_serialization_offset + slot_size > mp_serialized_data_buffer->get_m_length() ||
			slot_size < sizeof(unsigned char)*length)
	{
		printf("ERROR addUCharASlot: could not add serialization data\n");
		return;
	}

	memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, value, length);

	m_serialization_offset += slot_size;
}


float Serializable::getFloatSlot(unsigned int slot_size)
{
	//check if slot fits into the buffer and if slot is large enough for the data
	if(m_serialization_offset < slot_size ||
			slot_size < sizeof(float))
	{
		printf("ERROR getFloatSlot: could not retrieve serialization data\n");
		return 0;
	}

	float out = 0;

	m_serialization_offset -= slot_size;

	memcpy(&out, mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, sizeof(float));

	return out;
}

double Serializable::getDoubleSlot(unsigned int slot_size)
{
	//check if slot fits into the buffer and if slot is large enough for the data
	if(m_serialization_offset < slot_size ||
			slot_size < sizeof(double))
	{
		printf("ERROR getDoubleSlot: could not retrieve serialization data\n");
		return 0;
	}

	double out = 0;

	m_serialization_offset -= slot_size;

	memcpy(&out, mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, sizeof(double));

	return out;
}

int Serializable::getIntSlot(unsigned int slot_size)
{
	//check if slot fits into the buffer and if slot is large enough for the data
	if(m_serialization_offset < slot_size ||
			slot_size < sizeof(int))
	{
		printf("ERROR getIntSlot: could not retrieve serialization data\n");
		return 0;
	}

	int out = 0;

	m_serialization_offset -= slot_size;

	memcpy(&out, mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, sizeof(int));

	return out;
}

unsigned int Serializable::getUIntSlot(unsigned int slot_size)
{
	//check if slot fits into the buffer and if slot is large enough for the data
	if(m_serialization_offset < slot_size ||
			slot_size < sizeof(unsigned int))
	{
		printf("ERROR getUIntSlot: could not retrieve serialization data\n");
		return 0;
	}

	unsigned int out = 0;

	m_serialization_offset -= slot_size;

	memcpy(&out, mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, sizeof(unsigned int));

	return out;
}

long Serializable::getLongSlot(unsigned int slot_size)
{
	//check if slot fits into the buffer and if slot is large enough for the data
	if(m_serialization_offset < slot_size ||
			slot_size < sizeof(long))
	{
		printf("ERROR getLongSlot: could not retrieve serialization data\n");
		return 0;
	}

	long out = 0;

	m_serialization_offset -= slot_size;

	memcpy(&out, mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, sizeof(long));

	return out;
}

unsigned long Serializable::getULongSlot(unsigned int slot_size)
{
	//check if slot fits into the buffer and if slot is large enough for the data
	if(m_serialization_offset < slot_size ||
			slot_size < sizeof(unsigned long))
	{
		printf("ERROR getULongSlot: could not retrieve serialization data\n");
		return 0;
	}

	unsigned long out = 0;

	m_serialization_offset -= slot_size;

	memcpy(&out, mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, sizeof(unsigned long));

	return out;
}


long long Serializable::getLongLongSlot(unsigned int slot_size)
{
	//check if slot fits into the buffer and if slot is large enough for the data
	if(m_serialization_offset < slot_size ||
			slot_size < sizeof(long long))
	{
		printf("ERROR getLongLongSlot: could not retrieve serialization data\n");
		return 0;
	}

	long long out = 0;

	m_serialization_offset -= slot_size;

	memcpy(&out, mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, sizeof(long long));

	return out;
}

unsigned long long Serializable::getULongLongSlot(unsigned int slot_size)
{
	//check if slot fits into the buffer and if slot is large enough for the data
	if(m_serialization_offset < slot_size ||
			slot_size < sizeof(unsigned long long))
	{
		printf("ERROR getULongLongSlot: could not retrieve serialization data\n");
		return 0;
	}

	unsigned long long out = 0;

	m_serialization_offset -= slot_size;

	memcpy(&out, mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, sizeof(unsigned long long));

	return out;
}

char Serializable::getCharSlot(unsigned int slot_size)
{
	//check if slot fits into the buffer and if slot is large enough for the data
	if(m_serialization_offset < slot_size ||
			slot_size < sizeof(char))
	{
		printf("ERROR getCharSlot: could not retrieve serialization data\n");
		return 0;
	}

	char out = 0;

	m_serialization_offset -= slot_size;

	memcpy(&out, mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, sizeof(char));

	return out;
}

unsigned char Serializable::getUCharSlot(unsigned int slot_size)
{
	//check if slot fits into the buffer and if slot is large enough for the data
	if(m_serialization_offset < slot_size ||
			slot_size < sizeof(unsigned char))
	{
		printf("ERROR getUCharSlot: could not retrieve serialization data\n");
		return 0;
	}

	unsigned char out = 0;

	m_serialization_offset -= slot_size;

	memcpy(&out, mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, sizeof(unsigned char));

	return out;
}

std::string Serializable::getStringSlot(unsigned int slot_size)
{
	//check if slot fits into the buffer and if slot is large enough for the data
	if(m_serialization_offset < slot_size)
	{
		printf("ERROR getStringSlot: could not retrieve serialization data\n");
		return "";
	}

	char* buff = new char[slot_size];

	m_serialization_offset -= slot_size;

	memcpy(buff,mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, slot_size);

	std::string s(buff);

	delete[] buff;

	return s;
}

char* Serializable::getCharASlot(unsigned long long length,unsigned long long slot_size)
{
	//check if slot fits into the buffer and if slot is large enough for the data
	if(m_serialization_offset < slot_size ||
			slot_size < length)
	{
		printf("ERROR getCharASlot: could not retrieve serialization data\n");
		return 0;
	}

	char* buff = new char[length];

	m_serialization_offset -= slot_size;

	memcpy(buff,mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, length);

	return buff;
}

unsigned char* Serializable::getUCharASlot(unsigned long long length,unsigned long long slot_size)
{
	//check if slot fits into the buffer and if slot is large enough for the data
	if(m_serialization_offset < slot_size ||
			slot_size < length)
	{
		printf("ERROR getUCharASlot: could not retrieve serialization data\n");
		return 0;
	}

	unsigned char* buff = new unsigned char[length];

	m_serialization_offset -= slot_size;

	memcpy(buff,mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, length);

	return buff;
}



void Serializable::resetSerializationOffset()
{
	m_serialization_offset = 0;
}

unsigned long long Serializable::getRequiredStringBufferSize(const std::string& s)
{
	return s.size() + STRING_OVERHEAD_SIZE;
}

void Serializable::initSerializationOffsetForDeserialization()
{
	if(mp_serialized_data_buffer == NULL)
	{
		printf("ERROR: could not execute 'initSerializationOffsetForDeserialization'\n");
		return;
	}
	m_serialization_offset = mp_serialized_data_buffer->get_m_length();
}

void Serializable::setSerializationOffset(unsigned long offset)
{
	m_serialization_offset = offset;
}


} /* namespace MPCLib */
