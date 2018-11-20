/*
 * Callback.h
 *
 *  Created on: 05.02.2013
 *      Author: Darius Malysiak
 */

#ifndef CALLBACK_H_
#define CALLBACK_H_

#include "../BaseObject.h"
#include "Thread.h"

namespace Lazarus
{

class Callback: public BaseObject {
public:

	Callback();
	virtual ~Callback();

	virtual void callbackMethod(Thread* executing_thread = NULL) = 0;

};

struct CALLBACK_NO_THREAD_REF____;

}

#endif /* CALLBACK_H_ */
