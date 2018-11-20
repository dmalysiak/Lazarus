/*
 * JComTypes.h
 *
 *  Created on: 03.03.2017
 *      Author: malysiak
 */

#ifndef NETWORK_JCOMTYPES_H_
#define NETWORK_JCOMTYPES_H_

#include <string>
#include <type_traits>
#include <iostream>

#include "../../DataStructures/Image.h"

namespace Lazarus {

enum JTYPE{JTYPE_VECTOR=0,JTYPE_CHARARRAY, JTYPE_STRING,JTYPE_INT,JTYPE_LONG,JTYPE_FLOAT,JTYPE_DOUBLE,JTYPE_IMAGE, JTYPE_UNKNOWN};

class JBaseImage
{
public:
	//corresponds to the enum within Lazarus::Image<T>
	enum JIMAGE_DATA_ALIGNMENT{JIMAGE_DATA_ALIGNMENT_YPBPR=0,
		IMAGE_DATA_ALIGNMENT_YCBCR,
		IMAGE_DATA_ALIGNMENT_HSV,
		IMAGE_DATA_ALIGNMENT_HSI,
		IMAGE_DATA_ALIGNMENT_HSL,
		IMAGE_DATA_ALIGNMENT_RGB,
		IMAGE_DATA_ALIGNMENT_BGR,
		IMAGE_DATA_ALIGNMENT_RGBA,
		IMAGE_DATA_ALIGNMENT_GRAY,
		IMAGE_DATA_ALIGNMENT_BGRA,
		IMAGE_DATA_ALIGNMENT_GRAYA,
		IMAGE_DATA_ALIGNMENT_UNSPECIFIED};

	Lazarus::Buffer m_image_data;

};

class JComDataTypeBase : public Lazarus::Serializable{

public:
	JComDataTypeBase(){}
	virtual ~JComDataTypeBase(){}

	virtual void serialize(){ERROR("NOT IMPLEMENTED")}
	virtual void deserialize(){ERROR("NOT IMPLEMENTED")}

};

/**
 * All comtypes make a deep copy of their assigned data.
 * */
template<typename T>
class JComDataType : public JComDataTypeBase{
public:

	JComDataType(const T& data)
	{
		m_data = data;
		m_type = JTYPE_UNKNOWN;
	}

	virtual ~JComDataType()
	{
	}

	enum JTYPE m_type;
	T m_data;
};


template<>
class JComDataType<int> : public JComDataTypeBase{
public:

	JComDataType(const int& data)
	{
		m_type = JTYPE_INT;
		m_data = data;
	}

	JComDataType()
	{
		m_type = JTYPE_INT;
		m_data = 0;
	}

	virtual ~JComDataType()
	{
	}

	void serialize()
	{
		unsigned int type = ( (unsigned int)m_type );

		Serializable::registerElement<unsigned int>(1);
		Serializable::registerElement<int>(1);

		Serializable::allocateBuffer();

		Serializable::addInt(m_data);
		Serializable::addUInt(type);

	}

	void deserialize()
	{
		m_type = (enum JTYPE)( getUInt() );
		m_data = ( getUInt() );

		Serializable::resetBuffer();
	}

	enum JTYPE m_type;
	int m_data;
};


template<>
class JComDataType<long long> : public JComDataTypeBase{
public:

	JComDataType(const long long& data)
	{
		m_type = JTYPE_LONG;
		m_data = data;
	}

	JComDataType()
	{
		m_type = JTYPE_LONG;
		m_data = 0;
	}

	virtual ~JComDataType()
	{
	}

	void serialize()
	{
		unsigned int type = ( (unsigned int)m_type );

		Serializable::registerElement<unsigned int>(1);
		Serializable::registerElement<long long>(1);

		Serializable::allocateBuffer();

		Serializable::addULongLong(m_data);
		Serializable::addUInt(type);

	}

	void deserialize()
	{
		m_type = (enum JTYPE)( getUInt() );
		m_data = ( getLongLong() );

		Serializable::resetBuffer();
	}

	enum JTYPE m_type;
	long long m_data;
};



template<>
class JComDataType<float> : public JComDataTypeBase{
public:

	JComDataType(const float& data)
	{
		m_type = JTYPE_FLOAT;
		m_data = data;
	}

	JComDataType()
	{
		m_type = JTYPE_FLOAT;
		m_data = 0;
	}

	virtual ~JComDataType()
	{
	}

	void serialize()
	{
		unsigned int type = ( (unsigned int)m_type );

		Serializable::registerElement<unsigned int>(1);
		Serializable::registerElement<float>(1);

		Serializable::allocateBuffer();

		Serializable::addFloat(m_data);
		Serializable::addUInt(type);

	}

	void deserialize()
	{
		m_type = (enum JTYPE)( getUInt() );
		m_data = ( getFloat() );

		Serializable::resetBuffer();
	}

	enum JTYPE m_type;
	float m_data;
};


template<>
class JComDataType<double> : public JComDataTypeBase{
public:

	JComDataType(const double& data)
	{
		m_type = JTYPE_DOUBLE;
		m_data = data;
	}

	JComDataType()
	{
		m_type = JTYPE_DOUBLE;
		m_data = 0;
	}

	virtual ~JComDataType()
	{
	}

	void serialize()
	{
		unsigned int type = ( (unsigned int)m_type );

		Serializable::registerElement<unsigned int>(1);
		Serializable::registerElement<double>(1);

		Serializable::allocateBuffer();

		Serializable::addDouble(m_data);
		Serializable::addUInt(type);

	}

	void deserialize()
	{
		m_type = (enum JTYPE)( getUInt() );
		m_data = ( getDouble() );

		Serializable::resetBuffer();
	}

	enum JTYPE m_type;
	double m_data;
};

template<>
class JComDataType<std::string> : public JComDataTypeBase{
public:

	JComDataType(const std::string& data)
	{
		m_type = JTYPE_STRING;
		m_data = data;
	}

	JComDataType()
	{
		m_type = JTYPE_STRING;
		m_data = "";
	}

	virtual ~JComDataType()
	{
	}

	void serialize()
	{
		unsigned int type = ( (unsigned int)m_type );

		Serializable::registerElement<unsigned int>(1);
		Serializable::registerString(m_data);

		Serializable::allocateBuffer();

		Serializable::addString(m_data);
		Serializable::addUInt(type);

	}

	void deserialize()
	{
		m_type = (enum JTYPE)( getUInt() );
		m_data = getString();

		Serializable::resetBuffer();
	}

	enum JTYPE m_type;
	std::string m_data;
};


template<>
class JComDataType<unsigned char*> : public JComDataTypeBase{
public:

	/**
	 * creates a deep copy of the data.
	 * */
	JComDataType(unsigned char* data, unsigned int size)
	{
		m_type = JTYPE_CHARARRAY;
		m_data = new unsigned char[size];
		memcpy(m_data,data,size);
		m_size = size;
	}

	JComDataType()
	{
		m_type = JTYPE_CHARARRAY;
		m_data = NULL;
		m_size = 0;
	}

	JComDataType<unsigned char*>& operator=(const JComDataType<unsigned char*>& other) // copy assignment
	{
		DELETE_ARRAY_NULL_CHECKING(m_data);

		m_type = JTYPE_CHARARRAY;
		m_data = new unsigned char[other.m_size];
		memcpy(m_data,other.m_data,other.m_size);
		m_size = other.m_size;

		return *this;
	}

	virtual ~JComDataType()
	{
		DELETE_ARRAY_NULL_CHECKING(m_data);
	}

	void serialize()
	{
		unsigned int type = ( (unsigned int)m_type );

		Serializable::registerElement<unsigned int>(1);
		Serializable::registerUCharA(m_size);

		Serializable::allocateBuffer();

		Serializable::addUCharA(m_data,m_size);
		Serializable::addUInt(type);

	}

	void deserialize()
	{
		m_type = (enum JTYPE)( getUInt() );
		unsigned long long size = 0;
		m_data = getUCharA(size);
		m_size = size;

		Serializable::resetBuffer();
	}

	enum JTYPE m_type;
	unsigned char* m_data;
	unsigned int m_size;
};



template<>
class JComDataType<Lazarus::Image<unsigned char>*> : public JComDataTypeBase{
public:

	/**
	 * Creates a deep copy of the image.
	 * */
	JComDataType(Lazarus::Image<unsigned char>* data)
	{
		m_type = JTYPE_IMAGE;
		m_data = new Lazarus::Image<unsigned char>(data);

		if(m_data->channels() != 3 && m_data->channels() != 1)
		{
			ERROR("Image type not supported for Java serialization")
		}
	}

	JComDataType()
	{
		m_type = JTYPE_IMAGE;
		m_data = NULL;
	}

	JComDataType<Lazarus::Image<unsigned char>*>& operator=(const JComDataType<Lazarus::Image<unsigned char>*>& other) // copy assignment
	{
		DELETE_NULL_CHECKING(m_data);

		m_type = JTYPE_IMAGE;
		m_data = new Lazarus::Image<unsigned char>(other.m_data);

		return *this;
	}

	virtual ~JComDataType()
	{
		DELETE_NULL_CHECKING(m_data);
	}

	void serialize()
	{
		unsigned int type = ( (unsigned int)m_type );

		Serializable::registerElement<unsigned int>(1+3);//type,width,height,channels
		Serializable::registerUCharA( m_data->width() * m_data->height() * m_data->channels() );

		Serializable::allocateBuffer();

		Serializable::addUInt(m_data->width());
		Serializable::addUInt(m_data->height());
		Serializable::addUInt(m_data->channels());
		Serializable::addUCharA(m_data->getmp_image_data(), m_data->width() * m_data->height() * m_data->channels());
		Serializable::addUInt(type);
	}

	void deserialize()
	{
		m_type = (enum JTYPE)( getUInt() );
		unsigned long long size = 0;
		unsigned char* image_data = getUCharA(size);

		unsigned int channels = Serializable::getUInt();
		unsigned int height = Serializable::getUInt();
		unsigned int width = Serializable::getUInt();


		if(channels == 3)
			m_data = new Lazarus::Image<unsigned char>(image_data, width, height,
					Lazarus::Image<unsigned char>::IMAGE_DATA_ALIGNMENT_RGB);

		if(channels == 3)
			m_data = new Lazarus::Image<unsigned char>(image_data, width, height,
					Lazarus::Image<unsigned char>::IMAGE_DATA_ALIGNMENT_GRAY);


		Serializable::resetBuffer();
	}

	enum JTYPE m_type;
	Lazarus::Image<unsigned char>* m_data;
};

/**
 * This vector class owns all its assigned ComTypes, i.e. it will delete the objects pointed to!
 * */
template<>
class JComDataType<Lazarus::FastKTuple<JComDataTypeBase*>> : public JComDataTypeBase{
public:

	JComDataType(Lazarus::FastKTuple<JComDataTypeBase*> data)
	{
		m_type = JTYPE_VECTOR;
		m_data = data;
	}

	virtual ~JComDataType()
	{
		m_data.eraseWithDestructor();
	}

	virtual void serialize()
	{
		unsigned int type = ( (unsigned int)m_type );

		//first serialize all elements and register their size
		for(unsigned int i=0;i<m_data.getm_size();i++)
		{
			m_data[i]->serialize();
			Serializable::registerUCharA(m_data[i]->get_mp_serialized_data_buffer()->get_m_length());
		}

		Serializable::registerElement<unsigned int>(2);//type and element count

		Serializable::allocateBuffer();

		//now copy all the arrays
		for(unsigned int i=0;i<m_data.getm_size();i++)
		{
			Serializable::addUCharA(m_data[i]->get_mp_serialized_data_buffer()->get_mp_data(),
					m_data[i]->get_mp_serialized_data_buffer()->get_m_length());
		}

		Serializable::addUInt(m_data.getm_size());
		Serializable::addUInt(type);

	}

	virtual void deserialize()
	{
		m_data.eraseWithDestructor();
		m_type = (enum JTYPE)( getUInt() );//vector type :-)

		unsigned long long array_size = 0;
		unsigned char* serialized_data = NULL;

		unsigned int elementCount = Serializable::getUInt(); //get amount of elements

		for(unsigned int i=0;i<elementCount;i++)
		{
			serialized_data = getUCharA(array_size);//get the serialized data for an element

			//peek for the type
			enum JTYPE type = (enum JTYPE)( *( (unsigned int*)(serialized_data+array_size-4) ) );
			switch(type)
			{
				case JTYPE_VECTOR:
					INFO("cascaded vectors are not supported")
					break;
				case JTYPE_CHARARRAY:
				{
					JComDataType<unsigned char*>* t = new JComDataType<unsigned char*>();
					t->set_mp_serialized_data_buffer(new Lazarus::Buffer(serialized_data,array_size));
					t->deserialize();
					m_data.push(t);
					break;
				}
				case JTYPE_STRING:
				{
					JComDataType<std::string>* t = new JComDataType<std::string>();
					t->set_mp_serialized_data_buffer(new Lazarus::Buffer(serialized_data,array_size));
					t->deserialize();
					m_data.push(t);
					break;
				}
				case JTYPE_INT:
				{
					JComDataType<int>* t = new JComDataType<int>();
					t->set_mp_serialized_data_buffer(new Lazarus::Buffer(serialized_data,array_size));
					t->deserialize();
					m_data.push(t);
					break;
				}
				case JTYPE_LONG:
				{
					JComDataType<long long>* t = new JComDataType<long long>();
					t->set_mp_serialized_data_buffer(new Lazarus::Buffer(serialized_data,array_size));
					t->deserialize();
					m_data.push(t);
					break;
				}
				case JTYPE_FLOAT:
				{
					JComDataType<float>* t = new JComDataType<float>();
					t->set_mp_serialized_data_buffer(new Lazarus::Buffer(serialized_data,array_size));
					t->deserialize();
					m_data.push(t);
					break;
				}
				case JTYPE_DOUBLE:
				{
					JComDataType<double>* t = new JComDataType<double>();
					t->set_mp_serialized_data_buffer(new Lazarus::Buffer(serialized_data,array_size));
					t->deserialize();
					m_data.push(t);
					break;
				}
				case JTYPE_IMAGE:
				{
					JComDataType<Lazarus::Image<unsigned char>*>* t = new JComDataType<Lazarus::Image<unsigned char>*>();
					t->set_mp_serialized_data_buffer(new Lazarus::Buffer(serialized_data,array_size));
					t->deserialize();
					m_data.push(t);
					break;
				}
				case JTYPE_UNKNOWN:
				default:
					ERROR("UNKNOWN COM TYPE")
					break;
			}

		}


		Serializable::resetBuffer();
	}

	enum JTYPE m_type;
	Lazarus::FastKTuple<JComDataTypeBase*> m_data;
};



} /* namespace TKSKBTracker */

#endif /* NETWORK_JCOMTYPES_H_ */
