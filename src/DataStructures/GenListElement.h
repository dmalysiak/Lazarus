/*
 * GenListElement.h
 *
 *  Created on: Jul 22, 2014
 *      Author: clustro
 */

#ifndef GENLISTELEMENT_H_
#define GENLISTELEMENT_H_

#include "../BaseObject.h"

namespace Lazarus {

template<class T>
class GenListElement: public Lazarus::BaseObject {
public:
	GenListElement(unsigned long id = 0)
	{
		this->m_id = id;
	}

	virtual ~GenListElement()
	{

	}

	inline void setm_data(const T& data)
	{
		m_data = data;
	}

	inline T getm_data() const
	{
		return m_data;
	}

	inline T& getm_data_ref()
	{
		return m_data;
	}

	inline unsigned long getID()
	{
		return m_id;
	}

	unsigned long m_id;
	T m_data;
};

} /* namespace Lazarus */
#endif /* GENLISTELEMENT_H_ */
