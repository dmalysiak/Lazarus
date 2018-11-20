/*
 * NCSListElement.h
 *
 *  Created on: 28.01.2013
 *      Author: Darius Malysiak
 */

#ifndef GENNCDLISTELEMENT_H_
#define GENNCDLISTELEMENT_H_

#include "GenListElement.h"

namespace Lazarus
{

template<class T> class GenNCDListElement: public GenListElement<T> {
public:

	GenNCDListElement(unsigned long id = 0) :GenListElement<T>(id)
	{
		mp_successor = (GenNCDListElement<T>*)NULL;
		mp_predecessor = (GenNCDListElement<T>*)NULL;
	}

	~GenNCDListElement() {
		// TODO Auto-generated destructor stub
	}

	inline void setmp_succ(GenNCDListElement<T>* succ)
	{
		mp_successor = succ;
	}

	inline void setmp_pred(GenNCDListElement<T>* pred)
	{
		mp_predecessor = pred;
	}

	inline GenNCDListElement<T>* getmp_succ() const
	{
		return mp_successor;
	}

	inline GenNCDListElement<T>* getmp_pred() const
	{
		return mp_predecessor;
	}

	GenNCDListElement<T>* mp_successor;
	GenNCDListElement<T>* mp_predecessor;
};

}

#endif /* NCDLISTELEMENT_H_ */
