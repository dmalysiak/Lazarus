/*
 * AssembledFrameHandler.h
 *
 *  Created on: 07.03.2013
 *      Author: Darius Malysiak
 */

#ifndef ASSEMBLEDFRAMEHANDLER_H_
#define ASSEMBLEDFRAMEHANDLER_H_

#include "../../BaseObject.h"
#include "../../Buffer.h"
#include "../GenericConnection.h"
#include "../CommunicationFacility.h"
#include "Frame.h"

namespace Lazarus
{

class AssembledFrameHandler: public Lazarus::BaseObject {
public:
	AssembledFrameHandler(GenericConnection*  connection,CommunicationFacility* facility);
	virtual ~AssembledFrameHandler();

	//takes ownership of the payload
	virtual int handlePayload(Lazarus::Buffer* payload) = 0;

	/**
	 * If the handler serves a specific node, then this method will return its ID (>0).
	 * Else it will return 0 (i.e. no serving node).
	 * This is not thread safe!!
	 */
	virtual unsigned int getServingNodeID();

protected:
	Frame* mp_frame;
	GenericConnection* mp_connection;
	CommunicationFacility* mp_facility;
};

}

#endif /* ASSEMBLEDFRAMEHANDLER_H_ */
