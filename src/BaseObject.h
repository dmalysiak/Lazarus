/*
 * BaseObject.h
 *
 *  Created on: 23.01.2013
 *      Author: Darius Malysiak
 */

#ifndef BASEOBJECT_H_
#define BASEOBJECT_H_

#include "Config.h"

#include <string>
#include <time.h>

#define CLASS_RTTI(class_name) virtual const std::string& getClassName() const \
{ \
	static const std::string s(#class_name); \
	return s; \
}

namespace Lazarus
{

class BaseObject {
public:
	BaseObject();
	virtual ~BaseObject();

	char* getm_type() const;
	void printType() const;

	bool compareType(BaseObject* o) const;
	bool compareType(BaseObject &o) const;

	/**
	 * This method is useful in the following case:
	 * Let B be a child class of A, let class C have a method doIt(A* pointer) which internally needs
	 * to copy the object behind 'pointer'. Even if A's default copy constructor may be feasible to copy all
	 * of A's attributes, B might still contain additional logic in its constructor, yet sadly 'doIt' does not
	 * now which child class was actually provided! This can be circumvented with a virtual object copy/clone method.
	 * One should implement all copy-constructor logic in this method!
	 * */
	virtual BaseObject* clone() const;

	CLASS_RTTI(Lazarus::BaseObject)

	/**
	 * Indicates if this object can be serialized, i.e. is derived from the class Serializable.
	 * */
	virtual bool isSerializable()
	{
		return false;
	}

	/**
	 * Indicates if objects of this class can be serialized, i.e. is derived from the class Serializable.
	 * */
	static bool isSerializableClass()
	{
		return false;
	}

};


}
#endif /* BASEOBJECT_H_ */
