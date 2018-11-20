/*
 * ConnectCallback.h
 *
 *  Created on: 24.02.2015
 *      Author: Darius Malysiak
 */

#ifndef CONNECTCALLBACK_H_
#define CONNECTCALLBACK_H_

#include "../BaseObject.h"

namespace Lazarus {

class ConnectCallback: public Lazarus::BaseObject {
public:
	ConnectCallback();
	virtual ~ConnectCallback();

	virtual void call(unsigned int id)=0;
};

} /* namespace Lazarus */

#endif /* DISCONNECTCALLBACK_H_ */
