/*
 * Integer.h
 *
 *  Created on: 16.03.2017
 *      Author: malysiak
 */

#ifndef CORE_DATASTRUCTURES_PRIMITIVE_H_
#define CORE_DATASTRUCTURES_PRIMITIVE_H_

#include "../Serializable.h"
#include <sstream>

namespace Lazarus {

template<typename T>
class Primitive: public Serializable {
public:
	Primitive(T a){m_data = a;}
	virtual ~Primitive(){}

	/**
	 * Writes/updates the element into the primitive.
	 **/
	Primitive& operator <<(const std::string& s)
	{
		std::stringstream ss;
		ss << s;
		ss >> m_data;
		return *this;
	}

	void serialize()
	{
		resetBuffer();
		registerElement<T>(1);
		allocateBuffer();
		addElement<T>(m_data);
	}

	void deserialize()
	{
		m_data = getElement<T>();
		resetBuffer();
	}

	T m_data;
};

} /* namespace Lazarus */

#endif /* CORE_DATASTRUCTURES_PRIMITIVE_H_ */
