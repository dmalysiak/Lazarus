/*
 * ClusterLibAssembledFrameHandler.h
 *
 *  Created on: 07.03.2013
 *      Author: Darius Malysiak
 */

#ifndef CLUSTERLIBASSEMBLEDFRAMEHANDLER_H_
#define CLUSTERLIBASSEMBLEDFRAMEHANDLER_H_

#include "../AssembledFrameHandler.h"
#include "../../../Buffer.h"
#include "ClusterLibFrame.h"

namespace Lazarus
{

class ClusterLibAssembledFrameHandler: public AssembledFrameHandler {
public:
	ClusterLibAssembledFrameHandler(GenericConnection*  connection, CommunicationFacility* facility);
	virtual ~ClusterLibAssembledFrameHandler();

	//takes ownership of the payload
	virtual int handlePayload(Lazarus::Buffer* payload);

private:

};

}

#endif /* CLUSTERLIBASSEMBLEDFRAMEHANDLER_H_ */
