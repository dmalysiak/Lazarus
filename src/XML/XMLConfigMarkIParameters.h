/*
 * XMLConfigMarkIParameters.h
 *
 *  Created on: Feb 10, 2014
 *      Author: Darius Malysiak
 */

#ifndef XMLCONFIGMARKIPARAMETERS_H_
#define XMLCONFIGMARKIPARAMETERS_H_

#define XML_ENCODING "UTF-8"
//#define XML_ENCODING "ISO-8859-1"

#include "XMLConfigParameter.h"
#include "../DataStructures/FastNCSList.h"

#include <unordered_map>

namespace Lazarus {

class XMLConfigMarkIParameters : public Lazarus::BaseObject{
public:

	static const std::string XML_TAG_STRING;
	static const std::string XML_TAG_DOUBLE;
	static const std::string XML_TAG_FLOAT;
	static const std::string XML_TAG_INT;
	static const std::string XML_TAG_UINT;
	static const std::string XML_TAG_LONG;
	static const std::string XML_TAG_ULONG;
	static const std::string XML_TAG_LONGLONG;
	static const std::string XML_TAG_ULONGLONG;
	static const std::string XML_TAG_STRING_ARRAY;
	static const std::string XML_TAG_DOUBLE_ARRAY;
	static const std::string XML_TAG_FLOAT_ARRAY;
	static const std::string XML_TAG_INT_ARRAY;
	static const std::string XML_TAG_UINT_ARRAY;
	static const std::string XML_TAG_LONG_ARRAY;
	static const std::string XML_TAG_ULONG_ARRAY;
	static const std::string XML_TAG_LONGLONG_ARRAY;
	static const std::string XML_TAG_ULONGLONG_ARRAY;

	enum VALUE_TYPE{VALUE_TYPE_STRING=1,VALUE_TYPE_DOUBLE,VALUE_TYPE_FLOAT,
		VALUE_TYPE_INT,VALUE_TYPE_LONG,VALUE_TYPE_UINT,VALUE_TYPE_ULONG,
		VALUE_TYPE_LONGLONG,VALUE_TYPE_ULONGLONG,
		VALUE_TYPE_STRING_ARRAY, VALUE_TYPE_DOUBLE_ARRAY,VALUE_TYPE_FLOAT_ARRAY,
		VALUE_TYPE_INT_ARRAY,VALUE_TYPE_LONG_ARRAY,VALUE_TYPE_UINT_ARRAY,VALUE_TYPE_ULONG_ARRAY,
		VALUE_TYPE_LONGLONG_ARRAY,VALUE_TYPE_ULONGLONG_ARRAY,VALUE_TYPE_UNKNOWN};


	static const std::string ROOT_TAG;
	static const std::string SECTION_TAG;
	static const std::string PARAMETER_TAG;
	static const std::string SECTION_NAME_ATTRIBUTE;
	static const std::string PARAMETER_NAME_ATTRIBUTE;
	static const std::string PARAMETER_TYPE_ATTRIBUTE;
	static const std::string PARAMETER_STRING_TAG;
	static const char ARRAY_DELIMITER;

	XMLConfigMarkIParameters();
	virtual ~XMLConfigMarkIParameters();

	static enum VALUE_TYPE translateTagToValueType(const std::string& tag);

};

/*
template<typename T>
class XMLConfigMarkIParam : public XMLConfigParameter
{
public:
	XMLConfigMarkIParam(std::string name, T p);
	~XMLConfigMarkIParam();

	T getParam() const;
	void setParam(T p);

private:
	std::string m_string;

};*/

//scalars
class XMLConfigMarkIString : public XMLConfigParameter
{
public:
	XMLConfigMarkIString(const std::string& name, const std::string& string="");
	~XMLConfigMarkIString();

	const std::string& getParam() const;
	void setParam(const std::string& string);

private:
	std::string m_string;

};


class XMLConfigMarkIDouble : public XMLConfigParameter
{
public:
	XMLConfigMarkIDouble(const std::string& name, double d=0);
	~XMLConfigMarkIDouble();

	double getParam() const;
	void setParam(double d);

private:
	double m_d;

};

class XMLConfigMarkIFloat : public XMLConfigParameter
{
public:
	XMLConfigMarkIFloat(const std::string& name, float d=0);
	~XMLConfigMarkIFloat();

	float getParam() const;
	void setParam(float d);

private:
	float m_d;

};

class XMLConfigMarkIInt : public XMLConfigParameter
{
public:
	XMLConfigMarkIInt(const std::string& name, int d=0);
	~XMLConfigMarkIInt();

	int getParam() const;
	void setParam(int d);

private:
	int m_d;

};

class XMLConfigMarkILong : public XMLConfigParameter
{
public:
	XMLConfigMarkILong(const std::string& name, long d=0);
	~XMLConfigMarkILong();

	long getParam() const;
	void setParam(long d);

private:
	long m_d;

};

class XMLConfigMarkILongLong : public XMLConfigParameter
{
public:
	XMLConfigMarkILongLong(const std::string& name, long long d=0);
	~XMLConfigMarkILongLong();

	long long getParam() const;
	void setParam(long long d);

private:
	long long m_d;

};

class XMLConfigMarkIUInt : public XMLConfigParameter
{
public:
	XMLConfigMarkIUInt(const std::string& name, unsigned int d=0);
	~XMLConfigMarkIUInt();

	unsigned int getParam() const;
	void setParam(unsigned int d);

private:
	unsigned int m_d;

};

class XMLConfigMarkIULong : public XMLConfigParameter
{
public:
	XMLConfigMarkIULong(const std::string& name, unsigned long d=0);
	~XMLConfigMarkIULong();

	unsigned long getParam() const;
	void setParam(unsigned long d);

private:
	unsigned long m_d;

};

class XMLConfigMarkIULongLong : public XMLConfigParameter
{
public:
	XMLConfigMarkIULongLong(const std::string& name, unsigned long long d=0);
	~XMLConfigMarkIULongLong();

	unsigned long long getParam() const;
	void setParam(unsigned long long d);

private:
	unsigned long long m_d;

};

/**
 * Array return types: all returned arrays or lists are owned by the param object! Even 'getParam()' will not
 * give up ownership!
 */
class XMLConfigMarkIStringArray : public XMLConfigParameter
{
public:
	//this constructor takes ownership over the given string list
	XMLConfigMarkIStringArray(const std::string& name,  Lazarus::FastNCSList<std::string>* string = NULL);
	~XMLConfigMarkIStringArray();

	Lazarus::FastNCSList<std::string>* getParam() const;
	unsigned int getSize() const;
	//this method takes ownership over the given string list
	void setParam(Lazarus::FastNCSList<std::string>* string);

private:
	//keep in mind that the strings are stack allocated, i.e. they don't need to be deleted before deleting the list
	Lazarus::FastNCSList<std::string>* mp_string;
	unsigned int m_size;

};

class XMLConfigMarkIDoubleArray : public XMLConfigParameter
{
public:
	//this constructor takes ownership over the given array
	XMLConfigMarkIDoubleArray(const std::string& name, double* d= NULL, unsigned int size = 0);
	~XMLConfigMarkIDoubleArray();

	double* getParam() const;
	unsigned int getSize() const;
	//this method takes ownership over the given array
	void setParam(double* d, unsigned int size);

private:
	double* mp_d;
	unsigned int m_size;

};

class XMLConfigMarkIFloatArray : public XMLConfigParameter
{
public:
	//this constructor takes ownership over the given array
	XMLConfigMarkIFloatArray(const std::string& name, float* d= NULL, unsigned int size = 0);
	~XMLConfigMarkIFloatArray();

	float* getParam() const;
	unsigned int getSize() const;
	//this method takes ownership over the given array
	void setParam(float* d, unsigned int size);

private:
	float* mp_d;
	unsigned int m_size;

};

class XMLConfigMarkIIntArray : public XMLConfigParameter
{
public:
	//this constructor takes ownership over the given array
	XMLConfigMarkIIntArray(const std::string& name, int* d= NULL, unsigned int size = 0);
	~XMLConfigMarkIIntArray();

	int* getParam() const;
	unsigned int getSize() const;
	//this method takes ownership over the given array
	void setParam(int* d, unsigned int size);

private:
	int* mp_d;
	unsigned int m_size;

};

class XMLConfigMarkILongArray : public XMLConfigParameter
{
public:
	//this constructor takes ownership over the given array
	XMLConfigMarkILongArray(const std::string& name, long* d= NULL, unsigned int size = 0);
	~XMLConfigMarkILongArray();

	long* getParam() const;
	unsigned int getSize() const;
	//this method takes ownership over the given array
	void setParam(long* d, unsigned int size);

private:
	long* mp_d;
	unsigned int m_size;

};

class XMLConfigMarkILongLongArray : public XMLConfigParameter
{
public:
	//this constructor takes ownership over the given array
	XMLConfigMarkILongLongArray(const std::string& name, long long* d= NULL, unsigned int size = 0);
	~XMLConfigMarkILongLongArray();

	long long* getParam() const;
	unsigned int getSize() const;
	//this method takes ownership over the given array
	void setParam(long long* d, unsigned int size);

private:
	long long* mp_d;
	unsigned int m_size;

};

class XMLConfigMarkIUIntArray : public XMLConfigParameter
{
public:
	//this constructor takes ownership over the given array
	XMLConfigMarkIUIntArray(const std::string& name, unsigned int* d= NULL, unsigned int size = 0);
	~XMLConfigMarkIUIntArray();

	unsigned int* getParam() const;
	unsigned int getSize() const;
	//this method takes ownership over the given array
	void setParam(unsigned int* d, unsigned int size);

private:
	unsigned int* mp_d;
	unsigned int m_size;

};

class XMLConfigMarkIULongArray : public XMLConfigParameter
{
public:
	//this constructor takes ownership over the given array
	XMLConfigMarkIULongArray(const std::string& name, unsigned long* d= NULL, unsigned int size = 0);
	~XMLConfigMarkIULongArray();

	unsigned long* getParam() const;
	unsigned int getSize() const;
	//this method takes ownership over the given array
	void setParam(unsigned long* d, unsigned int size);

private:
	unsigned long* mp_d;
	unsigned int m_size;

};

class XMLConfigMarkIULongLongArray : public XMLConfigParameter
{
public:
	//this constructor takes ownership over the given array
	XMLConfigMarkIULongLongArray(const std::string& name, unsigned long long* d= NULL, unsigned int size = 0);
	~XMLConfigMarkIULongLongArray();

	unsigned long long* getParam() const;
	unsigned int getSize() const;
	//this method takes ownership over the given array
	void setParam(unsigned long long* d, unsigned int size);

private:
	unsigned long long* mp_d;
	unsigned int m_size;

};

} /* namespace Lazarus */
#endif /* XMLCONFIGMARKIPARAMETERS_H_ */
