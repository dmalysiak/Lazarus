/*
 * DummyCallbackClient.h
 *
 *  Created on: Aug 25, 2013
 *      Author: Darius Malysiak
 */

#ifndef DUMMYCALLBACKCLIENT_H_
#define DUMMYCALLBACKCLIENT_H_

#include "../GenericCallback.h"

namespace Lazarus {

class DummyCallbackClient: public Lazarus::GenericCallback {
public:
	DummyCallbackClient();
	virtual ~DummyCallbackClient();

	virtual int call(Lazarus::Thread* t,	void* var);

private:
	unsigned int m_reply_count;
};

} /* namespace Lazarus */
#endif /* DUMMYCALLBACKCLIENT_H_ */
