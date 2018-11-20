/*
 * FrameAssembler.cpp
 *
 *  Created on: 07.03.2013
 *      Author: Darius Malysiak
 */

#include "FrameAssembler.h"

namespace Lazarus
{

FrameAssembler::FrameAssembler(GenericConnection* connection,
		AssembledFrameHandler* assembled_frame_handler_instance) {

	mp_connection = connection;
	mp_connection_receive_buffer = connection->get_mp_receive_buffer();
	mp_assembled_frame_handler_instance = assembled_frame_handler_instance;
	mp_internal_frame = NULL;

}

FrameAssembler::~FrameAssembler()
{
	DELETE_NULL_CHECKING(mp_assembled_frame_handler_instance);
	DELETE_NULL_CHECKING(mp_internal_frame);
}

AssembledFrameHandler* FrameAssembler::getFrameHandler()
{
	return mp_assembled_frame_handler_instance;
}

}

