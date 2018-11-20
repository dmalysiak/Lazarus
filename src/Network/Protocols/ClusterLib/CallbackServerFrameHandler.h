/*
 * CallbackServerFrameHandler.h
 *
 *  Created on: 12.06.2013
 *      Author: Darius Malysiak
 */

#ifndef CALLBACKSERVERFRAMEHANDLER_H_
#define CALLBACKSERVERFRAMEHANDLER_H_

#include "../AssembledFrameHandler.h"
#include "../../TCPServer/CallbackServer.h"
#include "../../../Buffer.h"
#include "../../../Threading/Thread.h"
#include "ClusterLibFrame.h"
#include "../../Nodes/ClusterNode.h"
#include "../../GenericCallback.h"
#include "../../CommunicationFacilities/CallbackServerComFacility.h"
#include "../../../DataStructures/ThreadSafeUnorderedMap.h"

#include <pthread.h>

namespace Lazarus {

class CallbackServerFrameHandler: public AssembledFrameHandler {
public:
	CallbackServerFrameHandler(GenericConnection*  connection,CallbackServerComFacility* com_facility,
			GenericCallback* callback,Lazarus::Thread* worker_thread);
	virtual ~CallbackServerFrameHandler();

	enum CALLBACK_SERVER_FRAME_PROCESSING_RESULT{ CALLBACK_SERVER_FRAME_PROCESSING_RESULT_PING_ERROR = 20,
		CALLBACK_SERVER_FRAME_PROCESSING_RESULT_PONG_ERROR,
		CALLBACK_SERVER_FRAME_PROCESSING_RESULT_DATA_ERROR,
		CALLBACK_SERVER_FRAME_PROCESSING_RESULT_FINISHED_REGISTRATION,
		CALLBACK_SERVER_FRAME_PROCESSING_RESULT_OK,
		CALLBACK_SERVER_FRAME_PROCESSING_RESULT_OVER_AND_OUT
	};

	//takes ownership of the payload
	virtual int handlePayload(Lazarus::Buffer* payload);

private:
	CallbackServerComFacility* mp_callback_facility;
	enum CallbackServer::CALLBACK_SERVER_STATE m_node_management_state;
	unsigned int m_serving_node_id;
	GenericCallback* mp_callback;
	CallbackNode* mp_serving_node;
	Lazarus::Thread* mp_worker_thread;
	enum CallbackServer::CALLBACK_SERVER_REQUEST_TYPE m_type;
	unsigned int m_type_size;

};

} /* namespace Lazarus */
#endif /* CALLBACKSERVERFRAMEHANDLER_H_ */
