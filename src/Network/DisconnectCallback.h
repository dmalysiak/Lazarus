/*
 * DisconnectCallback.h
 *
 *  Created on: 24.02.2015
 *      Author: Darius Malysiak
 */

#ifndef DISCONNECTCALLBACK_H_
#define DISCONNECTCALLBACK_H_

#include "../BaseObject.h"

namespace Lazarus {

class DisconnectCallback: public Lazarus::BaseObject {
public:
	DisconnectCallback();
	virtual ~DisconnectCallback();

	virtual void call(unsigned int id)=0;
};

} /* namespace Lazarus */

#endif /* DISCONNECTCALLBACK_H_ */
