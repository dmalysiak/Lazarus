/*
 * DummyCallbackServer.h
 *
 *  Created on: Aug 25, 2013
 *      Author: Darius Malysiak
 */

#ifndef DUMMYCALLBACKSERVER_H_
#define DUMMYCALLBACKSERVER_H_

#include "../GenericCallback.h"
#include "../TCPServer/CallbackServer.h"

namespace Lazarus {

class DummyCallbackServer: public Lazarus::GenericCallback {
public:

	/**
	 * We start at 'RESERVED_TYPE_SIZE' at there are 'RESERVED_TYPE_SIZE' reserved message type
	 * slots for the callback server itself.
	 * */
	enum DUMMY_COM_REQUEST_TYPE{DUMMY_COM_REQUEST_TYPE_INCOMING_DATA_UNIT=CallbackServer::RESERVED_TYPE_SIZE,
		DUMMY_COM_REQUEST_TYPE_START_PROCESSING,
		DUMMY_COM_REQUEST_TYPE_INCOMING_RESULT_UNIT, DUMMY_COM_REQUEST_TYPE_OK};

	DummyCallbackServer();
	virtual ~DummyCallbackServer();

	virtual int call(Lazarus::Thread* t,	void* var);
};

} /* namespace Lazarus */
#endif /* DUMMYCALLBACKSERVER_H_ */
