/*
 * FrameAssembler.h
 *
 *  Created on: 07.03.2013
 *      Author: Darius Malysiak
 */

#ifndef FRAMEASSEMBLER_H_
#define FRAMEASSEMBLER_H_

#include "../../BaseObject.h"
#include "../../Buffer.h"
#include "../GenericConnection.h"
#include "AssembledFrameHandler.h"
#include "../../Time/TimeToolbox.h"

namespace Lazarus
{

class FrameAssembler: public Lazarus::BaseObject {
public:

	/**
	 * This object takes ownership over the frame handler
	 */
	FrameAssembler(GenericConnection* connection,
			AssembledFrameHandler* assembled_frame_handler_instance);
	virtual ~FrameAssembler();

	virtual int assemble(unsigned int bytes_waiting) = 0;

	AssembledFrameHandler* getFrameHandler();

	/**
	 * This method resets the frame assembler in cases of errors.
	 * */
	virtual void resetAssembler() = 0;

	/**
	 * This function shall return a frame which can be used to send data via the underlying protocol.
	 * */
	virtual Frame* getAssociatedFrame() = 0;

protected:
	GenericConnection* mp_connection;
	Lazarus::Buffer* mp_connection_receive_buffer;
	AssembledFrameHandler* mp_assembled_frame_handler_instance;
	Frame* mp_internal_frame;

};

}

#endif /* FRAMEASSEMBLER_H_ */
