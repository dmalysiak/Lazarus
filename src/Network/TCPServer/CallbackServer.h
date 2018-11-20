/*
 * CallbackServer.h
 *
 *  Created on: 12.06.2013
 *      Author: darius
 */

#ifndef CALLBACKSERVER_H_
#define CALLBACKSERVER_H_

#include "ClusterLibServer.h"
#include "../Protocols/Frame.h"
#include "../CommunicationFacilities/CallbackServerComFacility.h"
#include "../Nodes/CallbackNode.h"
#include "../../DataStructures/KTuple.h"
#include "../GenericCallback.h"

#include <string>
#include <pthread.h>

namespace Lazarus
{

struct CallbackData
{
	Lazarus::Buffer* mp_message;
	Frame* mp_frame;
	CallbackNode* mp_serving_node;
};

class CallbackServer: public ClusterLibServer {
public:

	static const unsigned int RESERVED_TYPE_SIZE = 100;

	CallbackServer(std::string local_address, unsigned int local_port, unsigned int max_connections,
			unsigned int worker_threads, GenericCallback* callback);
	virtual ~CallbackServer();

	enum CALLBACK_SERVER_REQUEST_TYPE{CALLBACK_SERVER_REQUEST_TYPE_PING=0, CALLBACK_SERVER_REQUEST_TYPE_PONG,
		CALLBACK_SERVER_REQUEST_TYPE_UNREGISTER,
		CALLBACK_SERVER_REQUEST_TYPE_UNREGISTER_CONFIRM,
		CALLBACK_SERVER_REQUEST_TYPE_OK,
		CALLBACK_SERVER_REQUEST_TYPE_SHUTDOWN_CLIENT,
		CALLBACK_SERVER_REQUEST_TYPE_NULL};

	enum CALLBACK_SERVER_STATE{CALLBACK_SERVER_STATE_IDLE,CALLBACK_SERVER_STATE_ACTIVE,
		CALLBACK_SERVER_STATE_UNINITIALIZED};

	static enum CallbackServer::CALLBACK_SERVER_REQUEST_TYPE getRequestType(Lazarus::Buffer* message);
	static void setRequestType(Lazarus::Buffer* message,enum CallbackServer::CALLBACK_SERVER_REQUEST_TYPE type);

	/**
	 * This function can be used to send an acknowledgment in case of a synchronous message
	 * */
	static int sendAck(Frame* frame);

	/**
	 * This method gets externally (i.e. subclass defined) request types or custom types
	 * */
	static int getCRequestType(Lazarus::Buffer* message, unsigned int req_type_size = sizeof(unsigned int));

	/**
	 * This method sets externally (i.e. subclass defined) request types or custom types
	 * */
	static void setCRequestType(Lazarus::Buffer* message,int type, unsigned int req_type_size = sizeof(unsigned int));

	static void setContent(Lazarus::Buffer* message,unsigned char* content, unsigned int size);
	static void getContent(Lazarus::Buffer* message,unsigned char** content, unsigned int &size);

	static void setCContent(Lazarus::Buffer* message,unsigned char* content, unsigned int size, unsigned int type_size = sizeof(unsigned int));
	static void getCContent(Lazarus::Buffer* message,unsigned char** content, unsigned int &size, unsigned int type_size = sizeof(unsigned int));

	/**
	 * This method will not only create the worker threads but also assign the servers current 'disconnect-callback'
	 * to them.
	 * */
	virtual void createWorkerThreads();

	void stopClient(unsigned int node_id);

	/**
	 * Sends a given message type with optional attached data. This method will lock the given node only for the duration
	 * of sending the data out. Returns 0 in case of success, -1 otherwise.
	 */
	int sendCommandAsync(unsigned int node_id, unsigned int type, Lazarus::Buffer* data = NULL);

	/**
	 * Sends a given message type with optional attached data. This method will not only lock the node for the duration
	 * of the dialog, it will also return and unlock the node only after the recipient has answered with a
	 * 'CALLBACK_SERVER_REQUEST_TYPE_OK' message. Returns 0 in case of success, -1 otherwise.
	 * Just to be clear: you have to ensure that the receiver will send a 'CALLBACK_SERVER_REQUEST_TYPE_OK'
	 * message in order for this method to return.
	 */
	int sendCommandSync(unsigned int node_id, unsigned int type, Lazarus::Buffer* data = NULL);

	unsigned int getNodeCount();
	Lazarus::FastKTuple<unsigned int>* getNodeIDs();

	/**
	 * This function is mainly for debug purposes.
	 * */
	void printNodeIDs();

protected:
	CallbackServerComFacility* mp_callback_facility;
	GenericCallback* mp_callback;
};

}
#endif /* CALLBACKSERVER_H_ */
