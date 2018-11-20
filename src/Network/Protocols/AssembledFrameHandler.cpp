/*
 * AssembledFrameHandler.cpp
 *
 *  Created on: 07.03.2013
 *      Author: Darius Malysiak
 */

#include "AssembledFrameHandler.h"

namespace Lazarus
{

AssembledFrameHandler::AssembledFrameHandler(GenericConnection*  connection,CommunicationFacility* facility) {

	mp_connection = connection;
	mp_facility = facility;
	mp_frame = NULL;
}

AssembledFrameHandler::~AssembledFrameHandler() {

	delete mp_frame;

}

unsigned int AssembledFrameHandler::getServingNodeID()
{
	return -1;
}

}
