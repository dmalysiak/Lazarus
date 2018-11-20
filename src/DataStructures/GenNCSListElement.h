/*
 * GenNCSListElement.h
 *
 *  Created on: Jul 21, 2014
 *      Author: Darius Malysiak
 */

#ifndef GENNCSLISTELEMENT_H_
#define GENNCSLISTELEMENT_H_

#include "GenListElement.h"

namespace Lazarus {

template<class T>
class GenNCSListElement: public Lazarus::GenListElement<T> {
public:
	GenNCSListElement(unsigned long id = 0) :GenListElement<T>(id)
	{
		mp_neighbor = (GenNCSListElement<T>*)NULL;
	}

	virtual ~GenNCSListElement() {
		// TODO Auto-generated destructor stub
	}

	inline void setmp_neighbor(GenNCSListElement<T>* neighbor)
	{
		mp_neighbor = neighbor;
	}

	inline GenNCSListElement<T>* getmp_neighbor() const
	{
		return mp_neighbor;
	}

	inline GenNCSListElement<T>* next() const
	{
		return mp_neighbor;
	}

	GenNCSListElement<T>* mp_neighbor;
};

} /* namespace Lazarus */

#endif /* GENNCSLISTELEMENT_H_ */
