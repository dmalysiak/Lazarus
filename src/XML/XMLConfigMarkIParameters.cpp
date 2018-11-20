/*
 * XMLConfigMarkIParameters.cpp
 *
 *  Created on: Feb 10, 2014
 *      Author: Darius Malysiak
 */

#include "XMLConfigMarkIParameters.h"

namespace Lazarus {

//define the type attributes
const std::string XMLConfigMarkIParameters::XML_TAG_STRING = "STRING";
const std::string XMLConfigMarkIParameters::XML_TAG_DOUBLE = "DOUBLE";
const std::string XMLConfigMarkIParameters::XML_TAG_FLOAT = "FLOAT";
const std::string XMLConfigMarkIParameters::XML_TAG_INT = "INT";
const std::string XMLConfigMarkIParameters::XML_TAG_UINT = "UINT";
const std::string XMLConfigMarkIParameters::XML_TAG_LONG = "LONG";
const std::string XMLConfigMarkIParameters::XML_TAG_ULONG = "ULONG";
const std::string XMLConfigMarkIParameters::XML_TAG_LONGLONG = "LONGLONG";
const std::string XMLConfigMarkIParameters::XML_TAG_ULONGLONG = "ULONGLONG";
const std::string XMLConfigMarkIParameters::XML_TAG_STRING_ARRAY = "STRING_ARRAY";
const std::string XMLConfigMarkIParameters::XML_TAG_DOUBLE_ARRAY = "DOUBLE_ARRAY";
const std::string XMLConfigMarkIParameters::XML_TAG_FLOAT_ARRAY = "FLOAT_ARRAY";
const std::string XMLConfigMarkIParameters::XML_TAG_INT_ARRAY = "INT_ARRAY";
const std::string XMLConfigMarkIParameters::XML_TAG_UINT_ARRAY = "UINT_ARRAY";
const std::string XMLConfigMarkIParameters::XML_TAG_LONG_ARRAY = "LONG_ARRAY";
const std::string XMLConfigMarkIParameters::XML_TAG_ULONG_ARRAY = "ULONG_ARRAY";
const std::string XMLConfigMarkIParameters::XML_TAG_LONGLONG_ARRAY = "LONGLONG_ARRAY";
const std::string XMLConfigMarkIParameters::XML_TAG_ULONGLONG_ARRAY = "ULONGLONG_ARRAY";

//and the xml tags
const std::string XMLConfigMarkIParameters::ROOT_TAG = "config";
const std::string XMLConfigMarkIParameters::SECTION_TAG = "section";
const std::string XMLConfigMarkIParameters::PARAMETER_TAG = "parameter";
const std::string XMLConfigMarkIParameters::SECTION_NAME_ATTRIBUTE = "name";
const std::string XMLConfigMarkIParameters::PARAMETER_NAME_ATTRIBUTE = "name";
const std::string XMLConfigMarkIParameters::PARAMETER_TYPE_ATTRIBUTE = "type";
const std::string XMLConfigMarkIParameters::PARAMETER_STRING_TAG = "string";
const char XMLConfigMarkIParameters::ARRAY_DELIMITER = ',';

XMLConfigMarkIParameters::XMLConfigMarkIParameters() {
	// TODO Auto-generated constructor stub

}

XMLConfigMarkIParameters::~XMLConfigMarkIParameters() {
	// TODO Auto-generated destructor stub
}

enum XMLConfigMarkIParameters::VALUE_TYPE XMLConfigMarkIParameters::translateTagToValueType(const std::string& tag)
{

	if(tag.compare(XML_TAG_STRING) == 0)
	{
		return VALUE_TYPE_STRING;
	}
	if(tag.compare(XML_TAG_DOUBLE) == 0)
	{
		return VALUE_TYPE_DOUBLE;
	}
	if(tag.compare(XML_TAG_FLOAT) == 0)
	{
		return VALUE_TYPE_FLOAT;
	}
	if(tag.compare(XML_TAG_INT) == 0)
	{
		return VALUE_TYPE_INT;
	}
	if(tag.compare(XML_TAG_LONG) == 0)
	{
		return VALUE_TYPE_LONG;
	}
	if(tag.compare(XML_TAG_LONGLONG) == 0)
	{
		return VALUE_TYPE_LONGLONG;
	}
	if(tag.compare(XML_TAG_UINT) == 0)
	{
		return VALUE_TYPE_UINT;
	}
	if(tag.compare(XML_TAG_ULONG) == 0)
	{
		return VALUE_TYPE_ULONG;
	}
	if(tag.compare(XML_TAG_ULONGLONG) == 0)
	{
		return VALUE_TYPE_ULONGLONG;
	}
	if(tag.compare(XML_TAG_STRING_ARRAY) == 0)
	{
		return VALUE_TYPE_STRING_ARRAY;
	}
	if(tag.compare(XML_TAG_DOUBLE_ARRAY) == 0)
	{
		return VALUE_TYPE_DOUBLE_ARRAY;
	}
	if(tag.compare(XML_TAG_FLOAT_ARRAY) == 0)
	{
		return VALUE_TYPE_FLOAT_ARRAY;
	}
	if(tag.compare(XML_TAG_INT_ARRAY) == 0)
	{
		return VALUE_TYPE_INT_ARRAY;
	}
	if(tag.compare(XML_TAG_LONG_ARRAY) == 0)
	{
		return VALUE_TYPE_LONG_ARRAY;
	}
	if(tag.compare(XML_TAG_LONGLONG_ARRAY) == 0)
	{
		return VALUE_TYPE_LONGLONG_ARRAY;
	}
	if(tag.compare(XML_TAG_UINT_ARRAY) == 0)
	{
		return VALUE_TYPE_UINT_ARRAY;
	}
	if(tag.compare(XML_TAG_ULONG_ARRAY) == 0)
	{
		return VALUE_TYPE_ULONG_ARRAY;
	}
	if(tag.compare(XML_TAG_ULONGLONG_ARRAY) == 0)
	{
		return VALUE_TYPE_ULONGLONG_ARRAY;
	}

	return VALUE_TYPE_UNKNOWN;
}


//scalars
XMLConfigMarkIString::XMLConfigMarkIString(const std::string& name, const std::string& string)
{
	m_type = (int)XMLConfigMarkIParameters::VALUE_TYPE_STRING;
	m_name = name;
	m_string = string;
}

XMLConfigMarkIString::~XMLConfigMarkIString()
{
}

const std::string& XMLConfigMarkIString::getParam() const
{
	return m_string;
}

void XMLConfigMarkIString::setParam(const std::string& string)
{
	m_string = string;
}



XMLConfigMarkIDouble::XMLConfigMarkIDouble(const std::string& name, double d)
{
	m_type = (int)XMLConfigMarkIParameters::VALUE_TYPE_DOUBLE;
	m_name = name;
	m_d = d;
}

XMLConfigMarkIDouble::~XMLConfigMarkIDouble()
{
}

double XMLConfigMarkIDouble::getParam() const
{
	return m_d;
}

void XMLConfigMarkIDouble::setParam(double d)
{
	m_d = d;
}


XMLConfigMarkIFloat::XMLConfigMarkIFloat(const std::string& name, float d)
{
	m_type = (int)XMLConfigMarkIParameters::VALUE_TYPE_FLOAT;
	m_name = name;
	m_d = d;
}

XMLConfigMarkIFloat::~XMLConfigMarkIFloat()
{
}

float XMLConfigMarkIFloat::getParam() const
{
	return m_d;
}

void XMLConfigMarkIFloat::setParam(float d)
{
	m_d = d;
}


XMLConfigMarkIInt::XMLConfigMarkIInt(const std::string& name, int d)
{
	m_type = (int)XMLConfigMarkIParameters::VALUE_TYPE_INT;
	m_name = name;
	m_d = d;
}

XMLConfigMarkIInt::~XMLConfigMarkIInt()
{
}

int XMLConfigMarkIInt::getParam() const
{
	return m_d;
}

void XMLConfigMarkIInt::setParam(int d)
{
	m_d = d;
}


XMLConfigMarkILong::XMLConfigMarkILong(const std::string& name, long d)
{
	m_type = (int)XMLConfigMarkIParameters::VALUE_TYPE_LONG;
	m_name = name;
	m_d = d;
}

XMLConfigMarkILong::~XMLConfigMarkILong()
{
}

long XMLConfigMarkILong::getParam() const
{
	return m_d;
}

void XMLConfigMarkILong::setParam(long d)
{
	m_d = d;
}



XMLConfigMarkILongLong::XMLConfigMarkILongLong(const std::string& name, long long d)
{
	m_type = (int)XMLConfigMarkIParameters::VALUE_TYPE_LONGLONG;
	m_name = name;
	m_d = d;
}

XMLConfigMarkILongLong::~XMLConfigMarkILongLong()
{
}

long long XMLConfigMarkILongLong::getParam() const
{
	return m_d;
}

void XMLConfigMarkILongLong::setParam(long long d)
{
	m_d = d;
}


XMLConfigMarkIUInt::XMLConfigMarkIUInt(const std::string& name, unsigned int d)
{
	m_type = (int)XMLConfigMarkIParameters::VALUE_TYPE_UINT;
	m_name = name;
	m_d = d;
}

XMLConfigMarkIUInt::~XMLConfigMarkIUInt()
{
}

unsigned int XMLConfigMarkIUInt::getParam() const
{
	return m_d;
}

void XMLConfigMarkIUInt::setParam(unsigned int d)
{
	m_d = d;
}


XMLConfigMarkIULong::XMLConfigMarkIULong(const std::string& name, unsigned long d)
{
	m_type = (int)XMLConfigMarkIParameters::VALUE_TYPE_ULONG;
	m_name = name;
	m_d = d;
}

XMLConfigMarkIULong::~XMLConfigMarkIULong()
{
}

unsigned long XMLConfigMarkIULong::getParam() const
{
	return m_d;
}

void XMLConfigMarkIULong::setParam(unsigned long d)
{
	m_d = d;
}

XMLConfigMarkIULongLong::XMLConfigMarkIULongLong(const std::string& name, unsigned long long d)
{
	m_type = (int)XMLConfigMarkIParameters::VALUE_TYPE_ULONGLONG;
	m_name = name;
	m_d = d;
}

XMLConfigMarkIULongLong::~XMLConfigMarkIULongLong()
{
}

unsigned long long XMLConfigMarkIULongLong::getParam() const
{
	return m_d;
}

void XMLConfigMarkIULongLong::setParam(unsigned long long d)
{
	m_d = d;
}


//arrays
XMLConfigMarkIStringArray::XMLConfigMarkIStringArray(const std::string& name, Lazarus::FastNCSList<std::string>* string)
{
	m_type = (int)XMLConfigMarkIParameters::VALUE_TYPE_STRING_ARRAY;
	m_name = name;
	mp_string = string;

	if(string == NULL)
	{
		m_size = string->getSize();
	}
}

XMLConfigMarkIStringArray::~XMLConfigMarkIStringArray()
{
	DELETE_NULL_CHECKING(mp_string);
}

unsigned int XMLConfigMarkIStringArray::getSize() const
{
	return m_size;
}

Lazarus::FastNCSList<std::string>* XMLConfigMarkIStringArray::getParam() const
{
	return mp_string;
}

void XMLConfigMarkIStringArray::setParam(Lazarus::FastNCSList<std::string>* string)
{
	DELETE_NULL_CHECKING(mp_string);
	mp_string = string;
}


XMLConfigMarkIDoubleArray::XMLConfigMarkIDoubleArray(const std::string& name, double* d, unsigned int size)
{
	m_type = (int)XMLConfigMarkIParameters::VALUE_TYPE_DOUBLE_ARRAY;
	m_name = name;
	mp_d = d;
	m_size = size;
}

XMLConfigMarkIDoubleArray::~XMLConfigMarkIDoubleArray()
{
	DELETE_ARRAY_NULL_CHECKING(mp_d);
}

double* XMLConfigMarkIDoubleArray::getParam() const
{
	return mp_d;
}

unsigned int XMLConfigMarkIDoubleArray::getSize() const
{
	return m_size;
}

void XMLConfigMarkIDoubleArray::setParam(double* d, unsigned int size)
{
	DELETE_ARRAY_NULL_CHECKING(mp_d);
	mp_d = d;
	m_size = size;
}


XMLConfigMarkIFloatArray::XMLConfigMarkIFloatArray(const std::string& name, float* d, unsigned int size)
{
	m_type = (int)XMLConfigMarkIParameters::VALUE_TYPE_FLOAT_ARRAY;
	m_name = name;
	mp_d = d;
	m_size = size;
}

XMLConfigMarkIFloatArray::~XMLConfigMarkIFloatArray()
{
	DELETE_ARRAY_NULL_CHECKING(mp_d);
}

float* XMLConfigMarkIFloatArray::getParam() const
{
	return mp_d;
}

unsigned int XMLConfigMarkIFloatArray::getSize() const
{
	return m_size;
}

void XMLConfigMarkIFloatArray::setParam(float* d, unsigned int size)
{
	DELETE_ARRAY_NULL_CHECKING(mp_d);
	mp_d = d;
	m_size = size;
}


XMLConfigMarkIIntArray::XMLConfigMarkIIntArray(const std::string& name, int* d, unsigned int size)
{
	m_type = (int)XMLConfigMarkIParameters::VALUE_TYPE_INT_ARRAY;
	m_name = name;
	mp_d = d;
	m_size = size;
}

XMLConfigMarkIIntArray::~XMLConfigMarkIIntArray()
{
	DELETE_ARRAY_NULL_CHECKING(mp_d);
}

int* XMLConfigMarkIIntArray::getParam() const
{
	return mp_d;
}

unsigned int XMLConfigMarkIIntArray::getSize() const
{
	return m_size;
}

void XMLConfigMarkIIntArray::setParam(int* d, unsigned int size)
{
	DELETE_ARRAY_NULL_CHECKING(mp_d);
	mp_d = d;
	m_size = size;
}

XMLConfigMarkILongArray::XMLConfigMarkILongArray(const std::string& name, long* d, unsigned int size)
{
	m_type = (int)XMLConfigMarkIParameters::VALUE_TYPE_LONG_ARRAY;
	m_name = name;
	mp_d = d;
	m_size = size;
}

XMLConfigMarkILongArray::~XMLConfigMarkILongArray()
{
	DELETE_ARRAY_NULL_CHECKING(mp_d);
}

long* XMLConfigMarkILongArray::getParam() const
{
	return mp_d;
}

unsigned int XMLConfigMarkILongArray::getSize() const
{
	return m_size;
}

void XMLConfigMarkILongArray::setParam(long* d, unsigned int size)
{
	DELETE_ARRAY_NULL_CHECKING(mp_d);
	mp_d = d;
	m_size = size;
}



XMLConfigMarkILongLongArray::XMLConfigMarkILongLongArray(const std::string& name, long long* d, unsigned int size)
{
	m_type = (int)XMLConfigMarkIParameters::VALUE_TYPE_LONGLONG_ARRAY;
	m_name = name;
	mp_d = d;
	m_size = size;
}

XMLConfigMarkILongLongArray::~XMLConfigMarkILongLongArray()
{
	DELETE_ARRAY_NULL_CHECKING(mp_d);
}

long long* XMLConfigMarkILongLongArray::getParam() const
{
	return mp_d;
}

unsigned int XMLConfigMarkILongLongArray::getSize() const
{
	return m_size;
}

void XMLConfigMarkILongLongArray::setParam(long long* d, unsigned int size)
{
	DELETE_ARRAY_NULL_CHECKING(mp_d);
	mp_d = d;
	m_size = size;
}


XMLConfigMarkIUIntArray::XMLConfigMarkIUIntArray(const std::string& name,unsigned int* d, unsigned int size)
{
	m_type = (int)XMLConfigMarkIParameters::VALUE_TYPE_UINT_ARRAY;
	m_name = name;
	mp_d = d;
	m_size = size;
}

XMLConfigMarkIUIntArray::~XMLConfigMarkIUIntArray()
{
	DELETE_ARRAY_NULL_CHECKING(mp_d);
}

unsigned int* XMLConfigMarkIUIntArray::getParam() const
{
	return mp_d;
}

unsigned int XMLConfigMarkIUIntArray::getSize() const
{
	return m_size;
}

void XMLConfigMarkIUIntArray::setParam(unsigned int* d, unsigned int size)
{
	DELETE_ARRAY_NULL_CHECKING(mp_d);
	mp_d = d;
	m_size = size;
}


XMLConfigMarkIULongArray::XMLConfigMarkIULongArray(const std::string& name, unsigned long* d, unsigned int size)
{
	m_type = (int)XMLConfigMarkIParameters::VALUE_TYPE_ULONG_ARRAY;
	m_name = name;
	mp_d = d;
	m_size = size;
}

XMLConfigMarkIULongArray::~XMLConfigMarkIULongArray()
{
	DELETE_ARRAY_NULL_CHECKING(mp_d);
}

unsigned long* XMLConfigMarkIULongArray::getParam() const
{
	return mp_d;
}

unsigned int XMLConfigMarkIULongArray::getSize() const
{
	return m_size;
}

void XMLConfigMarkIULongArray::setParam(unsigned long* d, unsigned int size)
{
	DELETE_ARRAY_NULL_CHECKING(mp_d);
	mp_d = d;
	m_size = size;
}



XMLConfigMarkIULongLongArray::XMLConfigMarkIULongLongArray(const std::string& name, unsigned long long* d, unsigned int size)
{
	m_type = (int)XMLConfigMarkIParameters::VALUE_TYPE_ULONG_ARRAY;
	m_name = name;
	mp_d = d;
	m_size = size;
}

XMLConfigMarkIULongLongArray::~XMLConfigMarkIULongLongArray()
{
	DELETE_ARRAY_NULL_CHECKING(mp_d);
}

unsigned long long* XMLConfigMarkIULongLongArray::getParam() const
{
	return mp_d;
}

unsigned int XMLConfigMarkIULongLongArray::getSize() const
{
	return m_size;
}

void XMLConfigMarkIULongLongArray::setParam(unsigned long long* d, unsigned int size)
{
	DELETE_ARRAY_NULL_CHECKING(mp_d);
	mp_d = d;
	m_size = size;
}


} /* namespace Lazarus */
