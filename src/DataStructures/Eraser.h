/*
 * Eraser.h
 *
 *  Created on: 05.10.2016
 *      Author: Darius Malysiak
 */

#ifndef CORE_DATASTRUCTURES_ERASER_H_
#define CORE_DATASTRUCTURES_ERASER_H_

#include "../BaseObject.h"

namespace Lazarus {

/**
 * This class has the following use case. If one desires to delete objects without aliasing tricks
 * as e.g.
 *  if( std::is_pointer<T>::value == true )
 *	{
 *		delete (*(T**)&(mp_data[i]));
 *		*(T**)&(mp_data[i]) = NULL;
 *	}
 *	else
 *	{
 *		printf("ERROR: can't call destructor on stack objects\n");
 *	}
 *
 *	This will could be used in order to dynamically delete elements from a container.
 *	!! CAUTION: this trick only works when the elements in the list are pointers to heap allocated objects!
 *
 *	Yet with the Eraser class one can avoid this by writing
 *
 *	Erase<T>::erase(data);
 *
 *	In case of non-pointer vars the element will be left unchanged, while
 *	in case of pointers the element behind it will be deleted and the variable will be nulled.
 *	The same holds for the array variant which calls delete[] via eraseArray on each element.
 *	An example application can be seen within the FastTuples classes.
 *
 * */

template<class T>
class Eraser: public BaseObject {
public:
	Eraser();
	virtual ~Eraser();

	static void erase(T& data)
	{
		//data = T();
	}

	static void eraseArray(T& data)
	{
		//data = T();
	}
};

template<class T>
class Eraser<T*>: public BaseObject {
public:
	Eraser();
	virtual ~Eraser();

	static void erase(T*& data)
	{
		DELETE_NULL(data);
	}

	static void eraseArray(T*& data)
	{
		DELETE_ARRAY_NULL(data);
	}
};

} /* namespace Lazarus */

#endif /* CORE_DATASTRUCTURES_ERASER_H_ */
