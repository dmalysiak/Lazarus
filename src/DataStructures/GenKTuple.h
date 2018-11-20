/*
 * Tuple.h
 *
 *  Created on: May 13, 2014
 *      Author: Darius Malysiak
 */

#ifndef GENKTUPLE_H_
#define GENKTUPLE_H_

#include "../Serializable.h"
#include "FastVectorN.h"

#include <stdio.h>
#include <type_traits>

namespace Lazarus
{

template<class T>
class GenKTuple: public Serializable {
public:

	GenKTuple(){

		//determine the type
		if(std::is_same<T,int>::value == true)
		{
			this->data_type = SH_DATA_TYPE_INT;
			m_is_primitive = true;
		}
		else if(std::is_same<T,unsigned int>::value == true)
		{
			this->data_type = SH_DATA_TYPE_UINT;
			m_is_primitive = true;
		}
		else if(std::is_same<T,char>::value == true)
		{
			this->data_type = SH_DATA_TYPE_CHAR;
			m_is_primitive = true;
		}
		else if(std::is_same<T,unsigned char>::value == true)
		{
			this->data_type = SH_DATA_TYPE_UCHAR;
			m_is_primitive = true;
		}
		else if(std::is_same<T,long int>::value == true)
		{
			this->data_type = SH_DATA_TYPE_LONG;
			m_is_primitive = true;
		}
		else if(std::is_same<T,long long int>::value == true)
		{
			this->data_type = SH_DATA_TYPE_LONGLONG;
			m_is_primitive = true;
		}
		else if(std::is_same<T,unsigned long>::value == true)
		{
			this->data_type = SH_DATA_TYPE_ULONG;
			m_is_primitive = true;
		}
		else if(std::is_same<T,unsigned long long>::value == true)
		{
			this->data_type = SH_DATA_TYPE_ULONGLONG;
			m_is_primitive = true;
		}
		else if(std::is_same<T,float>::value == true)
		{
			this->data_type = SH_DATA_TYPE_FLOAT;
			m_is_primitive = true;
		}
		else if(std::is_same<T,double>::value == true)
		{
			this->data_type = SH_DATA_TYPE_DOUBLE;
			m_is_primitive = true;
		}
		else if(std::is_same<T,long double>::value == true)
		{
			this->data_type = SH_DATA_TYPE_LONG_DOUBLE;
			m_is_primitive = true;
		}
		else if(std::is_same<T,unsigned short>::value == true)
		{
			this->data_type = SH_DATA_TYPE_USHORT;
			m_is_primitive = true;
		}
		else if(std::is_same<T,short>::value == true)
		{
			this->data_type = SH_DATA_TYPE_SHORT;
			m_is_primitive = true;
		}
		else
		{
			this->data_type = SH_DATA_TYPE_ABSTRACT;
			m_is_primitive = false;
		}

	}

	virtual ~GenKTuple(){}

	/**
	 * Returns a copy of the element.
	 * Yes, it is bad practice and a design mistake to name this method identical to one in the
	 * Serialization class. Yet this will not create any problems as long as one is aware of this
	 * method shadowing.
	 * */
	virtual T getElement(long long int index) const
	{
		printf("ERROR: NOT IMPLEMENTED\n");
		return T();
	}


	T& operator[](long long int index)
	{
		return this->op(index);
	}

	const T& operator[](long long int index) const
	{
		return this->op(index);
	}


	virtual T& op(long long int index)=0;

	virtual const T& op(long long int index) const = 0;

	virtual void setElement(long long int index,const T& element)
	{
		printf("NOT IMPLEMENTED\n");
	}

	virtual long long int getm_size() const
	{
		printf("NOT IMPLEMENTED\n");
		return 0;
	}

	virtual long long int size() const
	{
		printf("NOT IMPLEMENTED\n");
		return 0;
	}

	virtual void clear(long long int size = -1)
	{
		printf("NOT IMPLEMENTED\n");
	}

	virtual T* getmp_data()
	{
		printf("NOT IMPLEMENTED\n");
		return NULL;
	}

	/*
	 * destroy each object within the tuple.
	 * !! CAUTION: this only works when the elements in the list are pointers to heap allocated objects!
	 * This does not remove the elements from the tuple, only the objects they point to.
	 */
	virtual void eraseWithDestructor()
	{
		printf("NOT IMPLEMENTED\n");
	}

	/*
	 * destroy each object within the tuple.
	 * !! CAUTION: this only works when the elements in the list are pointers to heap allocated array objects!
	 * This does not remove the elements from the tuple, only the objects they point to.
	 */
	virtual void eraseArrayWithDestructor()
	{
		printf("NOT IMPLEMENTED\n");
	}

protected:
	SH_DATA_TYPE data_type;
	bool m_is_primitive;//indicates if the data type is primitive

};




}


#endif /* GENKTUPLE_H_ */
