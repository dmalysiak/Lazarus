/*
 * GenericCallback.h
 *
 *  Created on: Aug 24, 2013
 *      Author: Darius Malysiak
 */

#ifndef GENERICCALLBACK_H_
#define GENERICCALLBACK_H_

#include "../BaseObject.h"
#include "../Buffer.h"
#include "../Threading/Thread.h"

namespace Lazarus {

class GenericCallback: public Lazarus::BaseObject {
public:
	GenericCallback();
	virtual ~GenericCallback();

	virtual int call(Lazarus::Thread* t,	void* var) = 0;

};

} /* namespace Lazarus */
#endif /* GENERICCALLBACK_H_ */
