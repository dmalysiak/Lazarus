/*
 * SerializationStack.h
 *
 *  Created on: 04.03.2015
 *      Author: Darius Malysiak
 */

#ifndef SERIALIZATIONSTACK_H_
#define SERIALIZATIONSTACK_H_

#include "BaseObject.h"
#include "Buffer.h"

#include <string.h>

namespace Lazarus {

/**
 * This is essentially a stack of serialized data:
 * -call registerElement(count) for type T and with the associated count of elements to be inserted
 * -for strings call registerString(s) with the string s
 * -call sealStack(), which will allocate the required internal buffer
 * -finally add data to the stack with the appropriate methods e.g. addUInt(...) etc.
 * */

class SerializationStack: public BaseObject {
public:
	SerializationStack()
	{
		mp_serialized_data_buffer = NULL;
		m_stack_sealed = false;
		m_stack_size = 0;
		m_serialization_offset = 0;
	}

	virtual ~SerializationStack()
	{
		DELETE_NULL_CHECKING(mp_serialized_data_buffer);
	}

	/**
	 * Returns a pointer to the stack of serialized data. Never delete this object externally!
	 * */
	Buffer* getStackBuffer()
	{
		return mp_serialized_data_buffer;
	}

	void resetStack()
	{
		DELETE_NULL_CHECKING(mp_serialized_data_buffer);

		mp_serialized_data_buffer = NULL;
		m_stack_sealed = false;
		m_stack_size = 0;
		m_serialization_offset = 0;
	}

	/**
	 * This method takes ownership of the data and resets the internal state of the stack.
	 * Furthermore it seals the new stack, allowing the use of information retrieval via e.g. getUInt() etc.
	 * */
	void setStack(Buffer* buffer)
	{
		DELETE_NULL_CHECKING(mp_serialized_data_buffer);
		mp_serialized_data_buffer = buffer;
		m_stack_size = mp_serialized_data_buffer->get_m_length();
		m_serialization_offset = buffer->get_m_length();

		m_stack_sealed = true;
	}

	/**
	 * This method does not take ownership of the data but still resets the internal state of the stack.
	 * Furthermore it seals the new stack, allowing the use of information retrieval via e.g. getUInt() etc.
	 * */
	void setStackCopy(Buffer* buffer)
	{
		DELETE_NULL_CHECKING(mp_serialized_data_buffer);
		mp_serialized_data_buffer = new Buffer(buffer);
		m_stack_size = mp_serialized_data_buffer->get_m_length();
		m_serialization_offset = buffer->get_m_length();

		m_stack_sealed = true;
	}

	/**
	 * Increments the required buffer size by sizeof(T)*count bytes
	 * */
	template<typename T> void registerElement(unsigned int count)
	{
		if(m_stack_sealed == true)
		{
			printf("ERROR registerElement: serialization stack is sealed\n");
			return;
		}

		m_stack_size += sizeof(T)*count;
		//printf("stack size %u\n",m_stack_size);
	}

	/**
	 * Will register one unsigned int and string_size + 1 bytes.
	 * */
	void registerString(const std::string& s)
	{
		if(m_stack_sealed == true)
		{
			printf("ERROR registerString: serialization stack is sealed\n");
			return;
		}

		m_stack_size += sizeof(unsigned int);
		m_stack_size += s.size()+1;
		//printf("stack size %u\n",m_stack_size);
	}

	/**
	 * Will register one unsigned int and size*sizeof(unsigned char) bytes.
	 * */
	void registerUCharA(unsigned int size)
	{
		if(m_stack_sealed == true)
		{
			printf("ERROR registerUCharA: serialization stack is sealed\n");
			return;
		}

		m_stack_size += sizeof(unsigned int);
		m_stack_size += sizeof(unsigned char)*size;
	}

	/**
	 * Will register one unsigned int and size*sizeof(char) bytes.
	 * */
	void registerCharA(unsigned int size)
	{
		if(m_stack_sealed == true)
		{
			printf("ERROR registerUCharA: serialization stack is sealed\n");
			return;
		}

		m_stack_size += sizeof(unsigned int);
		m_stack_size += sizeof(char)*size;
	}

	/**
	 * Will register one unsigned int and string_size + 1 bytes.
	 * */
	void registerCString(const char* s)
	{
		if(m_stack_sealed == true)
		{
			printf("ERROR registerCString: serialization stack is sealed\n");
			return;
		}

		m_stack_size += sizeof(unsigned int);
		m_stack_size += strlen(s)+1;
	}

	void sealStack()
	{
		if(m_stack_sealed == true)
		{
			printf("ERROR: serialization stack is already sealed\n");
			return;
		}

		m_stack_sealed = true;
		mp_serialized_data_buffer = new Buffer(m_stack_size);
	}

	/**
	 * This is a generic method, yet one should attempt to avoid it for better error tracing and especially for arrays
	 * and strings!
	 * */
	template<typename T> void addElement(T value)
	{
		if(m_stack_sealed == false)
		{
			printf("ERROR addElement: serialization stack is not sealed\n");
			return;
		}

		if(m_serialization_offset + sizeof(T) > mp_serialized_data_buffer->get_m_length())
		{
			printf("ERROR addElement: could not add serialization data\n");
			return;
		}

		memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, &value, sizeof(T));

		m_serialization_offset += sizeof(T);
	}

	/**
	 * This is a generic method, yet one should attempt to avoid it for better error tracing and especially for arrays
	 * and strings!
	 * */
	template<typename T> T getElement()
	{
		if(m_stack_sealed == false)
		{
			printf("ERROR getElement: serialization stack is not sealed\n");
			return T();
		}

		if(m_serialization_offset < sizeof(T))
		{
			printf("ERROR getElement: could not retrieve serialization data\n");
			return T();
		}

		T out = T();

		m_serialization_offset -= sizeof(T);

		memcpy(&out, mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, sizeof(T));

		return out;
	}

	void addFloat(float value)
	{
		if(m_stack_sealed == false)
		{
			printf("ERROR addFloat: serialization stack is not sealed\n");
			return;
		}

		if(m_serialization_offset + sizeof(float) > mp_serialized_data_buffer->get_m_length())
		{
			printf("ERROR addFloat: could not add serialization data\n");
			return;
		}

		memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, &value, sizeof(float));

		m_serialization_offset += sizeof(float);
	}

	void addDouble(double value)
	{
		if(m_stack_sealed == false)
		{
			printf("ERROR addDouble: serialization stack is not sealed\n");
			return;
		}

		if(m_serialization_offset + sizeof(double) > mp_serialized_data_buffer->get_m_length())
		{
			printf("ERROR addDouble: could not add serialization data\n");
			return;
		}

		memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, &value, sizeof(double));

		m_serialization_offset += sizeof(double);
	}

	void addInt(int value)
	{
		if(m_stack_sealed == false)
		{
			printf("ERROR addInt: serialization stack is not sealed\n");
			return;
		}

		if(m_serialization_offset + sizeof(int) > mp_serialized_data_buffer->get_m_length())
		{
			printf("ERROR addInt: could not add serialization data\n");
			return;
		}

		memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, &value, sizeof(int));

		m_serialization_offset += sizeof(int);
	}

	void addUInt(unsigned int value)
	{
		if(m_stack_sealed == false)
		{
			printf("ERROR addUInt: serialization stack is not sealed\n");
			return;
		}

		if(m_serialization_offset + sizeof(unsigned int) > mp_serialized_data_buffer->get_m_length())
		{
			printf("ERROR addUInt: could not add serialization data\n");
			return;
		}

		memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, &value, sizeof(unsigned int));

		m_serialization_offset += sizeof(unsigned int);
	}

	void addLong(long value)
	{
		if(m_stack_sealed == false)
		{
			printf("ERROR addLong: serialization stack is not sealed\n");
			return;
		}

		if(m_serialization_offset + sizeof(long) > mp_serialized_data_buffer->get_m_length())
		{
			printf("ERROR addLong: could not add serialization data\n");
			return;
		}

		memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, &value, sizeof(long));

		m_serialization_offset += sizeof(long);
	}

	void addULong(unsigned long value)
	{
		if(m_stack_sealed == false)
		{
			printf("ERROR addULong: serialization stack is not sealed\n");
			return;
		}

		if(m_serialization_offset + sizeof(unsigned long) > mp_serialized_data_buffer->get_m_length())
		{
			printf("ERROR addULong: could not add serialization data\n");
			return;
		}

		memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, &value, sizeof(unsigned long));

		m_serialization_offset += sizeof(unsigned long);
	}

	void addLongLong(long long value)
	{
		if(m_stack_sealed == false)
		{
			printf("ERROR addLongLong: serialization stack is not sealed\n");
			return;
		}

		if(m_serialization_offset + sizeof(long long) > mp_serialized_data_buffer->get_m_length())
		{
			printf("ERROR addLongLong: could not add serialization data\n");
			return;
		}

		memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, &value, sizeof(long long));

		m_serialization_offset += sizeof(long long);
	}

	void addULongLong(unsigned long long value)
	{
		if(m_stack_sealed == false)
		{
			printf("ERROR addULongLong: serialization stack is not sealed\n");
			return;
		}

		if(m_serialization_offset + sizeof(unsigned long long) > mp_serialized_data_buffer->get_m_length())
		{
			printf("ERROR addULongLong: could not add serialization data\n");
			return;
		}

		memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, &value, sizeof(unsigned long long));

		m_serialization_offset += sizeof(unsigned long long);
	}

	void addChar(char value)
	{
		if(m_stack_sealed == false)
		{
			printf("ERROR addChar: serialization stack is not sealed\n");
			return;
		}

		if(m_serialization_offset + sizeof(char) > mp_serialized_data_buffer->get_m_length())
		{
			printf("ERROR addChar: could not add serialization data\n");
			return;
		}

		memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, &value, sizeof(char));

		m_serialization_offset += sizeof(char);
	}

	void addUChar(unsigned char value)
	{
		if(m_stack_sealed == false)
		{
			printf("ERROR addUChar: serialization stack is not sealed\n");
			return;
		}

		if(m_serialization_offset + sizeof(unsigned char) > mp_serialized_data_buffer->get_m_length())
		{
			printf("ERROR addUChar: could not add serialization data\n");
			return;
		}

		memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, &value, sizeof(unsigned char));

		m_serialization_offset += sizeof(unsigned char);
	}

	void addBool(bool value)
	{
		if(m_stack_sealed == false)
		{
			printf("ERROR addBool: serialization stack is not sealed\n");
			return;
		}

		if(m_serialization_offset + sizeof(bool) > mp_serialized_data_buffer->get_m_length())
		{
			printf("ERROR addBool: could not add serialization data\n");
			return;
		}

		memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, &value, sizeof(bool));

		m_serialization_offset += sizeof(bool);
	}

	void addString(const std::string& value)
	{
		if(m_stack_sealed == false)
		{
			printf("ERROR addString: serialization stack is not sealed\n");
			return;
		}

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

	void addCharA(char* value, unsigned int length)
	{
		if(m_stack_sealed == false)
		{
			printf("ERROR addCharA: serialization stack is not sealed\n");
			return;
		}

		if(m_serialization_offset + length > mp_serialized_data_buffer->get_m_length())
		{
			printf("ERROR addCharA: could not add serialization data\n");
			return;
		}

		memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, value, length);

		m_serialization_offset += length;

		addUInt(length);
	}

	void addUCharA(unsigned char* value, unsigned int length)
	{
		if(m_stack_sealed == false)
		{
			printf("ERROR addUCharA: serialization stack is not sealed\n");
			return;
		}

		if(m_serialization_offset + length > mp_serialized_data_buffer->get_m_length())
		{
			printf("ERROR addUCharA: could not add serialization data\n");
			return;
		}

		memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, value, length);

		m_serialization_offset += length;

		addUInt(length);
	}


	float getFloat()
	{
		if(m_stack_sealed == false)
		{
			printf("ERROR getFloat: serialization stack is not sealed\n");
			return 0;
		}

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

	double getDouble()
	{
		if(m_stack_sealed == false)
		{
			printf("ERROR getDouble: serialization stack is not sealed\n");
			return 0;
		}

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

	int getInt()
	{
		if(m_stack_sealed == false)
		{
			printf("ERROR getInt: serialization stack is not sealed\n");
			return 0;
		}

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

	unsigned int getUInt()
	{
		if(m_stack_sealed == false)
		{
			printf("ERROR getUInt: serialization stack is not sealed\n");
			return 0;
		}

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

	long getLong()
	{
		if(m_stack_sealed == false)
		{
			printf("ERROR getLong: serialization stack is not sealed\n");
			return 0;
		}

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

	unsigned long getULong()
	{
		if(m_stack_sealed == false)
		{
			printf("ERROR getULong: serialization stack is not sealed\n");
			return 0;
		}

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

	long long getLongLong()
	{
		if(m_stack_sealed == false)
		{
			printf("ERROR getLongLong: serialization stack is not sealed\n");
			return 0;
		}

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

	unsigned long long getULongLong()
	{
		if(m_stack_sealed == false)
		{
			printf("ERROR getULongLong: serialization stack is not sealed\n");
			return 0;
		}

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

	char getChar()
	{
		if(m_stack_sealed == false)
		{
			printf("ERROR getChar: serialization stack is not sealed\n");
			return 0;
		}

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

	unsigned char getUChar()
	{
		if(m_stack_sealed == false)
		{
			printf("ERROR getUChar: serialization stack is not sealed\n");
			return 0;
		}

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

	bool getBool()
	{
		if(m_stack_sealed == false)
		{
			printf("ERROR getBool: serialization stack is not sealed\n");
			return 0;
		}

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


	std::string getString()
	{
		if(m_stack_sealed == false)
		{
			printf("ERROR getString: serialization stack is not sealed\n");
			return "";
		}

		unsigned int length = getUInt();

		//case length=0: always fail (string always has a termination \0)! case length>0: trivial
		if(m_serialization_offset < length )
		{
			printf("ERROR getString: could not retrieve serialization data\n");
			return "";
		}

		char* buff = new char[length];

		m_serialization_offset -= length;

		memcpy(buff,mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, length);

		std::string s(buff);

		delete[] buff;

		return s;
	}

	/**
	 * The arrays length will be stored in the provided parameter.
	 * */
	char* getCharA(unsigned int& length_)
	{
		if(m_stack_sealed == false)
		{
			printf("ERROR getCharA: serialization stack is not sealed\n");
			return NULL;
		}

		unsigned int length = getUInt();

		//case length=0: no harm done! case length>0: trivial
		if(m_serialization_offset < length && length>0 )
		{
			printf("ERROR getCharA: could not retrieve serialization data\n");
			return NULL;
		}

		char* buff = new char[length];

		m_serialization_offset -= length;

		memcpy(buff,mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, length);

		length_ = length;

		return buff;
	}

	/**
	 * The arrays length will be stored in the provided parameter.
	 * */
	unsigned char* getUCharA(unsigned int& length_)
	{
		if(m_stack_sealed == false)
		{
			printf("ERROR getUCharA: serialization stack is not sealed\n");
			return NULL;
		}

		unsigned int length = getUInt();

		//case length=0: no harm done! case length>0: trivial
		if(m_serialization_offset < length && length>0 )
		{
			printf("ERROR getUCharA: could not retrieve serialization data\n");
			return NULL;
		}

		unsigned char* buff = new unsigned char[length];

		m_serialization_offset -= length;

		memcpy(buff,mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, length);

		length_ = length;

		return buff;
	}

private:
	unsigned int m_stack_size;
	bool m_stack_sealed;
	Buffer* mp_serialized_data_buffer;
	unsigned long long m_serialization_offset;
};

} /* namespace Lazarus */

#endif /* SERIALIZATIONSTACK_H_ */
