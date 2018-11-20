/*
 * GenList.h
 *
 *  Created on: Jul 22, 2014
 *      Author: clustro
 */

#ifndef GENLIST_H_
#define GENLIST_H_

#include "../BaseObject.h"
#include "GenListElement.h"

#include <stdio.h>

namespace Lazarus {

template<class T>
class GenList: public Lazarus::BaseObject {
public:
	GenList(){}
	virtual ~GenList(){}

	/**
	 * this function removes every object from the list
	 */
	virtual void clear()
	{
		printf("NOT IMPLEMENTED\n");
	}

	/**
	 * this function removes every object from the list and deletes it in case of pointers
	 */
	virtual void destroy()
	{
		printf("NOT IMPLEMENTED\n");
	}

	/**
	 * this function removes every object from the list and deletes it in case of pointers to arrays.
	 * Warning: this function does not check if the array objects are indeed pointing to arrays
	 */
	virtual void destroyArrays()
	{
		printf("NOT IMPLEMENTED\n");
	}

	//appends an element at the end of the list
	virtual void appendLast(const T& data)
	{
		printf("NOT IMPLEMENTED\n");
	}

	//appends an element at the beginning of the list
	virtual void appendFirst(const T& data)
	{
		printf("NOT IMPLEMENTED\n");
	}

	virtual void removeFirst()
	{
		printf("NOT IMPLEMENTED\n");
	}
/*
	virtual GenListElement<T>* getFirst()
	{
		printf("NOT IMPLEMENTED\n");
		return NULL;
	}

	virtual GenListElement<T>* getLast()
	{
		printf("NOT IMPLEMENTED\n");
		return NULL;
	}*/

	virtual unsigned long getSize()
	{
		printf("NOT IMPLEMENTED\n");
		return 0;
	}

	virtual T getListElementAtPos(unsigned int pos)
	{
		printf("NOT IMPLEMENTED\n");
		return T();
	}

	T& operator[](unsigned int pos)
	{
		return this->op(pos);
	}

	const T& operator[](unsigned int pos) const
	{
		return this->op(pos);
	}

	/**
	 * All efficiency for operator '[]' lies in the details of the overridden 'op' method.
	 */
	virtual T& op(unsigned int pos)=0;

	/**
	 * All efficiency for operator '[]' lies in the details of the overridden 'op' method.
	 */
	virtual const T& op(unsigned int pos) const =0;

	virtual void printListStructure()
	{
		printf("NOT IMPLEMENTED\n");
	}

};

} /* namespace Lazarus */
#endif /* GENLIST_H_ */
