/*
 * GenericIterator.h
 *
 *  Created on: Feb 10, 2014
 *      Author: Darius Malysiak
 */

#ifndef GENERICITERATOR_H_
#define GENERICITERATOR_H_

#include "../BaseObject.h"

namespace Lazarus {

template< template<class> class CONTAINER, class C >
class GenericForwardIterator: public Lazarus::BaseObject {
public:
	GenericForwardIterator(const GenericForwardIterator<CONTAINER,C>& it)
	{
		mp_current_element = it.getElement();
	}

	GenericForwardIterator(CONTAINER<C>* current_element)
	{
		mp_current_element = current_element;
	}

	GenericForwardIterator()
	{
		mp_current_element = NULL;
	}


	virtual ~GenericForwardIterator()
	{

	}

	CONTAINER<C>* getElement() const
	{
		return mp_current_element;
	}

	void setElement(CONTAINER<C>* current_element)
	{
		mp_current_element = current_element;
	}

	virtual void operator ++()
	{
		printf("operator ++ not implemented yet\n");
	}

	virtual bool operator !=(const GenericForwardIterator<CONTAINER,C>& it)
	{
		printf("operator -- not implemented yet\n");
		return false;
	}

protected:
	CONTAINER<C>* mp_current_element;

};

template< template<class> class CONTAINER, class C >
class GenericBackwardIterator: public Lazarus::BaseObject {
public:
	GenericBackwardIterator(const GenericBackwardIterator<CONTAINER,C>& it)
	{
		mp_current_element = it.getElement();
	}

	GenericBackwardIterator(CONTAINER<C>* current_element)
	{
		mp_current_element = current_element;
	}

	GenericBackwardIterator()
	{
		mp_current_element = NULL;
	}


	virtual ~GenericBackwardIterator()
	{

	}

	CONTAINER<C>* getElement() const
	{
		return mp_current_element;
	}

	void setElement(CONTAINER<C>* current_element)
	{
		mp_current_element = current_element;
	}

	virtual void operator --()
	{
		printf("operator -- not implemented yet\n");
	}

	virtual bool operator !=(const GenericBackwardIterator<CONTAINER,C>& it)
	{
		printf("operator -- not implemented yet\n");
		return false;
	}

protected:
	CONTAINER<C>* mp_current_element;

};


} /* namespace Lazarus */
#endif /* GENERICITERATOR_H_ */
