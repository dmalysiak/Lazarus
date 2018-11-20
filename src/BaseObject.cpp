/*
 * BaseObject.cpp
 *
 *  Created on: 23.01.2013
 *      Author: Darius Malysiak
 */

#include "BaseObject.h"
#include <stdio.h>

#include <clocale>

namespace Lazarus
{


BaseObject::BaseObject()
{
	static bool locale_has_been_set = false;

	if(locale_has_been_set == false)
	{
		setlocale(LC_ALL,"C");
		locale_has_been_set = true;
	}

}

BaseObject::~BaseObject() {

}

/*
 * Default cloner.
 * */
BaseObject* BaseObject::clone() const
{
	return new BaseObject(*this);
}

bool BaseObject::compareType(BaseObject* o) const
{
	int res = o->getClassName().compare(this->getClassName());

	if(res != 0)
	{
		return false;
	}

	return true;
}

bool BaseObject::compareType(BaseObject &o) const
{
	int res = o.getClassName().compare(this->getClassName());

	if(res != 0)
	{
		return false;
	}

	return true;
}

void BaseObject::printType() const
{
	printf("Object type: %s \n",this->getClassName().c_str());

}


}
