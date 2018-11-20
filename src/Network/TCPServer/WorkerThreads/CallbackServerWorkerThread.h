/*
 * CallbackServerWorkerThread.h
 *
 *  Created on: 12.06.2013
 *      Author: darius
 */

#ifndef CALLBACKSERVERWORKERTHREAD_H_
#define CALLBACKSERVERWORKERTHREAD_H_

#include "../../TCPServerWorkerThread.h"
#include "../../CommunicationFacilities/CallbackServerComFacility.h"
#include "../../GenericCallback.h"

#include <pthread.h>

namespace Lazarus{

class CallbackServerWorkerThread: public TCPServerWorkerThread {
public:
	CallbackServerWorkerThread(unsigned int max_connections,CallbackServerComFacility* com_facility,
			GenericCallback* callback);
	virtual ~CallbackServerWorkerThread();

	virtual void addConnection(TCPSocket* tcpsocket);

private:
	CallbackServerComFacility* mp_callback_facility;
	GenericCallback* mp_callback;

};

}

#endif /* CALLBACKSERVERWORKERTHREAD_H_ */
