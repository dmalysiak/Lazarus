/*
 * ClusterLibNodeManagementFrameHandler.h
 *
 *  Created on: 12.06.2013
 *      Author: Darius Malysiak
 */

#ifndef CLUSTERLIBNODEMANAGEMENTFRAMEHANDLER_H_
#define CLUSTERLIBNODEMANAGEMENTFRAMEHANDLER_H_

#include "../AssembledFrameHandler.h"
#include "../../TCPServer/ClusterNodeManagementServer.h"
#include "../../../Buffer.h"
#include "ClusterLibFrame.h"
#include "../../Nodes/ClusterNode.h"
#include "../../../DataStructures/ThreadSafeUnorderedMap.h"

#include <pthread.h>

namespace Lazarus {

/**
 * This frame handler will unlock any pending sequences for external threads and indicate an unfinished sequence
 * to them via the cluster nodes communication callback struct! Futhermore it will set the var 'm_fatal_error' to true.
 */
class ClusterLibNodeManagementFrameHandler: public AssembledFrameHandler {
public:
	ClusterLibNodeManagementFrameHandler(GenericConnection*  connection,ClusterNodeComFacility* com_facility);
	virtual ~ClusterLibNodeManagementFrameHandler();

	enum CLUSTERNODE_MANAGEMENT_FRAME_PROCESSING_RESULT{ CLUSTERNODE_MANAGEMENT_FRAME_PROCESSING_RESULT_PING_ERROR = 20,
		CLUSTERNODE_MANAGEMENT_FRAME_PROCESSING_RESULT_PONG_ERROR,
		CLUSTERNODE_MANAGEMENT_FRAME_PROCESSING_RESULT_NODE_DATA_ERROR,
		CLUSTERNODE_MANAGEMENT_FRAME_PROCESSING_RESULT_FINISHED_REGISTRATION,
		CLUSTERNODE_MANAGEMENT_FRAME_PROCESSING_RESULT_OK,
		CLUSTERNODE_MANAGEMENT_FRAME_PROCESSING_RESULT_OVER_AND_OUT
	};

	//takes ownership of the payload
	virtual int handlePayload(Lazarus::Buffer* payload);

	/**
	 * If the handler serves a specific node, then this method will return its ID (>=0).
	 * Else it will return -1.
	 * This is not thread safe!!
	 */
	virtual unsigned int getServingNodeID();

private:
	ClusterNodeComFacility* mp_node_com_facility;
	enum ClusterNodeManagementServer::CLUSTERNODE_MANAGEMENT_STATE m_node_management_state;
	int parseCommand(Lazarus::Buffer* message);
	unsigned int m_serving_node_id;
	ClusterNode* mp_serving_node;

	//this var will be used only during registration
	unsigned int m_obtained_node_id;

};

} /* namespace Lazarus */
#endif /* CLUSTERLIBNODEMANAGEMENTFRAMEHANDLER_H_ */
