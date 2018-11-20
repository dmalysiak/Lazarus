/*
 * CallbackNode.h
 *
 *  Created on: Aug 24, 2013
 *      Author: Darius Malysiak
 */

#ifndef CALLBACKNODE_H_
#define CALLBACKNODE_H_

#include "../NetworkNode.h"
#include "../Protocols/Frame.h"
#include "../../System/SystemStatus.h"
#include "../SynchronizationCallback.h"

#include <pthread.h>

namespace Lazarus {

class CallbackNode: public Lazarus::NetworkNode {
public:
	CallbackNode(unsigned int nodeID, Frame* frame);
	virtual ~CallbackNode();

	bool isCommunicating();

	Frame* get_mp_frame();
	void lockMutex();
	void unlockMutex();

	//register the callback
	void setCallback(SynchronizationCallback* com_callback);

	//get the callback
	SynchronizationCallback* getCallback();

	void serialize();
	void deserialize();

private:
	bool m_communication_in_progress;
	Frame* mp_frame;
	pthread_mutex_t m_mutex;

	SynchronizationCallback* mp_com_callback;
};

} /* namespace Lazarus */
#endif /* CALLBACKNODE_H_ */
