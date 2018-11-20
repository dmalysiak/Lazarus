/*
 * ClusterlibFrameAssembler.h
 *
 *  Created on: 01.03.2013
 *      Author: Darius Malysiak
 */

#ifndef CLUSTERLIBFRAMEASSEMBLER_H_
#define CLUSTERLIBFRAMEASSEMBLER_H_

#include "../FrameAssembler.h"
#include "../../../Buffer.h"
#include "../../../Threading/ObjectCallback.h"
#include "../AssembledFrameHandler.h"

namespace Lazarus
{

class ClusterLibFrameAssembler: public FrameAssembler {
public:
	/**
	 * This object takes ownership over the frame handler
	 */
	ClusterLibFrameAssembler(GenericConnection* connection,
			AssembledFrameHandler* assembled_frame_handler_instance);
	virtual ~ClusterLibFrameAssembler();

	/**
	 * Returns -1 in case of errors during assembling or during frame handling, 0 or a framehandler return code otherwise.
	 *
	 * */
	virtual int assemble(unsigned int bytes_waiting);

	enum HEADER_STATUS{HEADER_STATUS_INCOMPLETE,HEADER_STATUS_COMPLETE, HEADER_STATUS_OVERREAD, HEADER_STATUS_PERFECT};

	//we use uint to contain the frame size
	static const unsigned int HEADER_LENGTH = 4;

	/**
	 * This method resets the frame assembler in cases of errors.
	 * */
	virtual void resetAssembler();

	/**
	 * This function shall return a frame which can be used to send data via the underlying protocol.
	 * */
	virtual Frame* getAssociatedFrame();


private:
	unsigned int m_local_offset;
	unsigned char* mp_local_buffer;

	unsigned int m_remaining_header_bytes;
	bool m_read_header;

	unsigned int m_incoming_length;

	enum HEADER_STATUS readHeader(unsigned int bytes_waiting);
};

}

#endif /* CLUSTERLIBFRAMEASSEMBLER_H_ */
