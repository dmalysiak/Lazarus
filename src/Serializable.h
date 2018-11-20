/*
 * Serializable.h
 *
 *  Created on: 20.02.2013
 *      Author: Darius Malysiak
 */

#ifndef SERIALIZABLE_H_
#define SERIALIZABLE_H_

#include "BaseObject.h"
#include "Buffer.h"

#include <string>
#include <string.h>

namespace Lazarus {


class Serializable: public BaseObject {
public:
	Serializable();
	virtual ~Serializable();

	/**
	 * Save the object to the given file.
	 * */
	void writeToFile(const std::string& path)
	{
		this->serialize();
		this->mp_serialized_data_buffer->writeBufferToBinaryFile(path);
	}

	/**
	 * Read the object from the given file.
	 * */
	void readFromFile(const std::string& path)
	{
		Buffer* b = new Buffer(1);
		b->readBufferFromBinaryFile(path);
		this->set_mp_serialized_data_buffer(b);
		deserialize();
	}

	virtual void serialize() =0;
	virtual void deserialize() =0;

	virtual bool isSerializable()
	{
		return true;
	}

	static bool isSerializableClass()
	{
		return true;
	}

	void createEmptyBuffer(unsigned int size);

	//this method takes ownership of the array
	void set_mp_serialized_data_buffer(unsigned char* buffer, unsigned long long size);
	//this method takes ownership of the buffer
	void set_mp_serialized_data_buffer(Buffer* buffer);
	//this method does a deep copy
	void set_mp_serialized_data_buffer_by_copy(Buffer* buffer);

	Buffer* get_mp_serialized_data_buffer() const;

	/**
	 * This might be useful if one desires only a specific fragment within an assigned
	 * buffer. But beware the danger of this!
	 * */
	void setSerializationOffset(unsigned long offset);

	static const unsigned int FLOAT_SIZE = sizeof(float);
	static const unsigned int DOUBLE_SIZE = sizeof(double);
	static const unsigned int INT_SIZE = sizeof(int);
	static const unsigned int UINT_SIZE = sizeof(unsigned int);
	static const unsigned int SHORT_SIZE = sizeof(short);
	static const unsigned int USHORT_SIZE = sizeof(unsigned short);
	static const unsigned int LONG_SIZE = sizeof(long);
	static const unsigned int ULONG_SIZE = sizeof(unsigned long);
	static const unsigned int LONGLONG_SIZE = sizeof(long long);
	static const unsigned int ULONGLONG_SIZE = sizeof(unsigned long long);
	static const unsigned int CHAR_SIZE = sizeof(char);
	static const unsigned int UCHAR_SIZE = sizeof(unsigned char);
	static const unsigned int STRING_OVERHEAD_SIZE = 1+sizeof(unsigned int);
	static const unsigned int CHAR_ARRAY_OVERHEAD_SIZE = sizeof(unsigned int);
	static const unsigned int UCHAR_ARRAY_OVERHEAD_SIZE = sizeof(unsigned int);
	static const unsigned int SHORT_ARRAY_OVERHEAD_SIZE = sizeof(unsigned int);
	static const unsigned int USHORT_ARRAY_OVERHEAD_SIZE = sizeof(unsigned int);

protected:
	unsigned long long m_required_size;
	Buffer* mp_serialized_data_buffer;
	unsigned long long m_serialization_offset;

	//serialization helper methods
	void addFloat(float value);
	void addDouble(double value);
	void addInt(int value);
	void addUInt(unsigned int value);
	void addLong(long value);
	void addULong(unsigned long value);
	void addLongLong(long long value);
	void addULongLong(unsigned long long value);
	void addChar(char value);
	void addUChar(unsigned char value);
	void addShort(short value);
	void addUShort(unsigned short value);
	void addBool(bool value);

	/**
	 * Will stack up <...|c-string with terminator|c-string length> onto the stack
	 */
	void addString(const std::string& value);
	void addCharA(char* value, unsigned long long length);
	void addUCharA(unsigned char* value, unsigned long long length);

	float getFloat();
	double getDouble();
	int getInt();
	unsigned int getUInt();
	short getShort();
	unsigned short getUShort();
	long getLong();
	unsigned long getULong();
	long long getLongLong();
	unsigned long long getULongLong();
	char getChar();
	unsigned char getUChar();
	std::string getString();
	char* getCharA(unsigned long long& size);
	unsigned char* getUCharA(unsigned long long& size);
	bool getBool();

	/*
	 * do not use the slotted methods with 'registerElement' calls!!
	 * They are only for manual serialization!!
	 * */

	/**
	 * slotted helper methods, these will insert the element on top of the current offset and add
	 * 'slot_size' to the current offset. Thus one should chose 'slot_size' to be of at least the object size
	 * in order to fit it into the serialization buffer.
	 */
	void addFloatSlot(float value,unsigned int slot_size);
	void addDoubleSlot(double value,unsigned int slot_size);
	void addIntSlot(int value,unsigned int slot_size);
	void addUIntSlot(unsigned int value,unsigned int slot_size);
	void addLongSlot(long value,unsigned int slot_size);
	void addULongSlot(unsigned long value,unsigned int slot_size);
	void addLongLongSlot(long long value,unsigned int slot_size);
	void addULongLongSlot(unsigned long long value,unsigned int slot_size);
	void addCharSlot(char value,unsigned int slot_size);
	void addUCharSlot(unsigned char value,unsigned int slot_size);
	void addStringSlot(const std::string& value,unsigned int slot_size);
	void addCharASlot(char* value, unsigned long long length,unsigned long long slot_size);
	void addUCharASlot(unsigned char* value, unsigned long long length,unsigned long long slot_size);

	float getFloatSlot(unsigned int slot_size);
	double getDoubleSlot(unsigned int slot_size);
	int getIntSlot(unsigned int slot_size);
	unsigned int getUIntSlot(unsigned int slot_size);
	long getLongSlot(unsigned int slot_size);
	unsigned long getULongSlot(unsigned int slot_size);
	long long getLongLongSlot(unsigned int slot_size);
	unsigned long long getULongLongSlot(unsigned int slot_size);
	char getCharSlot(unsigned int slot_size);
	unsigned char getUCharSlot(unsigned int slot_size);
	std::string getStringSlot(unsigned int slot_size);
	char* getCharASlot(unsigned long long length,unsigned long long slot_size);
	unsigned char* getUCharASlot(unsigned long long length,unsigned long long slot_size);

	/**
	 * If anything went wrong, this method will reset the serialization state. This will also reset any
	 * calculated (via 'registerElement') buffer size.
	 * */
	void resetBuffer();

	/**
	 * Call this if you decided to use the type-registration methods instead of manual size calculation.
	 * If you chose the later, use createEmptyBuffer(size)!
	 */
	void allocateBuffer();

	/**
	 * Call this before serializing!
	 */
	void resetSerializationOffset();

	/**
	 * Call this before deserializing!
	 */
	void initSerializationOffsetForDeserialization();

	/**
	 * This will return the required amount of bytes to store the null terminated c-string in the buffer,
	 * including the overhead for length information.
	 */
	unsigned long long getRequiredStringBufferSize(const std::string& s);

	/*
	 * These register-methods can be used to calculate the required size of the serialization buffer
	 * */

	/**
	 * Increments the required buffer size by sizeof(T)*count bytes
	 * */
	template<typename T> void registerElement(unsigned int count)
	{
		m_required_size += sizeof(T)*count;
	}

	/**
	 * Increments the required buffer size by sizeof(T)*count bytes, essentially the same as
	 * 'registerElement', i.e. no size information about the array will be stored along the data!!
	 * Use getElementArray<T> to retrieve the data.
	 * */
	template<typename T> void registerElementArray(unsigned int count)
	{
		m_required_size += sizeof(T)*count;
	}

	/**
	 * Will register one unsigned int and string_size + 1 bytes.
	 * */
	void registerString(const std::string& s)
	{
		m_required_size += sizeof(unsigned int);
		m_required_size += s.size()+1;
	}

	/**
	 * Will register one unsigned int and string_size + 1 bytes.
	 * */
	void registerCString(const char* s)
	{
		m_required_size += sizeof(unsigned int);
		m_required_size += strlen(s)+1;
	}

	/**
	 * Will register one unsigned long long + size*sizeof(unsigned char) bytes.
	 * */
	void registerUCharA(unsigned long long size)
	{
		m_required_size += sizeof(unsigned long long);
		m_required_size += size*sizeof(unsigned char);
	}

	/**
	 * Will register one unsigned long long + size*sizeof(char) bytes.
	 * */
	void registerCharA(unsigned long long size)
	{
		m_required_size += sizeof(unsigned long long);
		m_required_size += size*sizeof(char);
	}

	/**
	 * Will implicitly call the parameters serialize method.
	 **/
	void registerSerializable(Serializable& ser)
	{
		ser.serialize();
		registerUCharA(ser.get_mp_serialized_data_buffer()->get_m_length());
	}

	/**
	 * Will NOT implicitly call the parameters serialize method, i.e. the object must already be
	 * serialized!!
	 **/
	void addSerializable(Serializable& ser)
	{
		//serialize the attributes
		this->addUCharA(ser.get_mp_serialized_data_buffer()->get_mp_data(),
				ser.get_mp_serialized_data_buffer()->get_m_length());
	}

	/**
	 * This is a generic method, yet one should attempt to avoid it for better error tracing and especially for arrays
	 * and strings! use it only for primitive datatypes.
	 * */
	template<typename T> void addElement(const T& value)
	{

		if(m_serialization_offset + sizeof(T) > mp_serialized_data_buffer->get_m_length())
		{
			printf("ERROR addElement: could not add serialization data\n");
			return;
		}

		memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, &value, sizeof(T));

		m_serialization_offset += sizeof(T);
	}

	/**
	 * This is a generic method, use it only for primitive datatypes.
	 * */
	template<typename T> void addElementArray(const T* values, unsigned int count)
	{

		if(m_serialization_offset + count*sizeof(T) > mp_serialized_data_buffer->get_m_length())
		{
			printf("ERROR addElementArray: could not add serialization data\n");
			return;
		}

		memcpy(mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, values, sizeof(T)*count);

		m_serialization_offset += sizeof(T)*count;
	}

	/**
	 * This is a generic method, yet one should attempt to avoid it for better error tracing and especially for arrays
	 * and strings!
	 * */
	template<typename T> T getElement()
	{
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

	/**
	 * This is a generic method, yet one should attempt to avoid it for better error tracing and especially for arrays
	 * and strings! This will copy the binary dump into val.
	 * */
	template<typename T> void getElement(T& val)
	{
		if(m_serialization_offset < sizeof(T))
		{
			printf("ERROR getElement: could not retrieve serialization data\n");
			return;
		}

		m_serialization_offset -= sizeof(T);

		memcpy(&val, mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, sizeof(T));
	}

	/**
	 * This is a generic method, returns NULL in case of errors.
	 * */
	template<typename T> T* getElementArray(unsigned int count)
	{
		if(m_serialization_offset < sizeof(T)*count)
		{
			printf("ERROR getElementArray: could not retrieve serialization data\n");
			return NULL;
		}

		T* out = new T[count];

		m_serialization_offset -= sizeof(T)*count;

		memcpy(out, mp_serialized_data_buffer->get_mp_data() + m_serialization_offset, sizeof(T)*count);

		return out;
	}

	/**
	 * Will deserialize the given object from the current stack.
	 * */
	void getSerializable(Serializable& ser)
	{
		unsigned long long array_size = 0;
		unsigned char* serialized_data = getUCharA(array_size);

		ser.set_mp_serialized_data_buffer(new Lazarus::Buffer(serialized_data,array_size));
		ser.deserialize();
	}

};

/**
 * Helper class for various containers.
 * */
template<bool SERIALIZABLE, typename T>
class AutonomSerializer : public BaseObject
{
};

/**
 * Helper class for various containers.
 * */
template<typename T>
class AutonomSerializer<true,T> : public BaseObject
{
public:

	/**
	 * Will call the objects serialization method if 'serialize==true'.
	 * */
	AutonomSerializer(T* s, bool serialize = true)
	{
		mp_s = s;
		if(serialize == true)
			s->serialize();
	}

	/**
	 * Will call the objects serialization method if 'serialize==true'.
	 * */
	AutonomSerializer(T& s, bool serialize = true)
	{
		mp_s = &s;
		if(serialize == true)
			s.serialize();
	}


	Buffer* getSerializedData()
	{
		return mp_s->get_mp_serialized_data_buffer();
	}

	bool isSerializable_(){return true;}

	T* getPointer(){return mp_s;}

private:
	Serializable* mp_s;
};

/**
 * Helper class for various containers.
 * */
template<typename T>
class AutonomSerializer<false,T> : public BaseObject
{
public:

	/**
	 * Will call the objects serialization method if 'serialize==true'.
	 * */
	AutonomSerializer(T* s, bool serialize = true)
	{
		mp_s = s;
	}

	/**
	 * Will call the objects serialization method if 'serialize==true'.
	 * */
	AutonomSerializer(T& s, bool serialize = true)
	{
		mp_s = &s;
	}


	Buffer* getSerializedData()
	{
		return NULL;
	}

	bool isSerializable_(){return false;}

	T* getPointer(){return mp_s;}

private:
	T* mp_s;
};


}
#endif /* SERIALIZABLE_H_ */
