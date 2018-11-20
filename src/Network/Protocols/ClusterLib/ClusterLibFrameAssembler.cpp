/*
 * ClusterlibFrameAssembler.cpp
 *
 *  Created on: 01.03.2013
 *      Author: Darius Malysiak
 */

#include "ClusterLibFrameAssembler.h"
#include "ClusterLibFrame.h"

#include <string.h>

namespace Lazarus
{

ClusterLibFrameAssembler::ClusterLibFrameAssembler(GenericConnection* connection,
		AssembledFrameHandler* assembled_frame_handler_instance)
		: FrameAssembler(connection, assembled_frame_handler_instance) {

	mp_local_buffer = new unsigned char[ClusterLibFrameAssembler::HEADER_LENGTH];
	m_local_offset = 0;
	m_incoming_length = 0;
	m_read_header = false;
	m_remaining_header_bytes = ClusterLibFrameAssembler::HEADER_LENGTH;
}

ClusterLibFrameAssembler::~ClusterLibFrameAssembler() {

	DELETE_ARRAY_NULL_CHECKING(mp_local_buffer);

}

Frame* ClusterLibFrameAssembler::getAssociatedFrame()
{
	if(mp_internal_frame == NULL)
		mp_internal_frame = new ClusterLibFrame(mp_connection);

	return mp_internal_frame;
}

enum ClusterLibFrameAssembler::HEADER_STATUS ClusterLibFrameAssembler::readHeader(unsigned int bytes_waiting)
{
	//if we can't read reamining_header_size amount of bytes and no header has been found so far
	if(bytes_waiting < m_remaining_header_bytes)
	{
		//copy the header from the receive_buffer into the local_buffer
		memcpy(mp_local_buffer,mp_connection_receive_buffer->get_mp_data(),bytes_waiting*sizeof(unsigned char));

		//remaining header bytes
		m_remaining_header_bytes = m_remaining_header_bytes - bytes_waiting;

		//still not finished
		return HEADER_STATUS_INCOMPLETE;
	}

	//if we can read exactly remaining_header_size amount of bytes and no header has been found so far
	if(bytes_waiting == m_remaining_header_bytes)
	{
		//copy the header from the receive_buffer into the local_buffer
		memcpy(mp_local_buffer,mp_connection_receive_buffer->get_mp_data(),m_remaining_header_bytes*sizeof(unsigned char));

		//assemble the length;
		//for ulong
//		m_incoming_length = mp_local_buffer[7];
//		m_incoming_length = (m_incoming_length << 8) | mp_local_buffer[6];
//		m_incoming_length = (m_incoming_length << 8) | mp_local_buffer[5];
//		m_incoming_length = (m_incoming_length << 8) | mp_local_buffer[4];
//		m_incoming_length = (m_incoming_length << 8) | mp_local_buffer[3];
//		m_incoming_length = (m_incoming_length << 8) | mp_local_buffer[2];
//		m_incoming_length = (m_incoming_length << 8) | mp_local_buffer[1];
//		m_incoming_length = (m_incoming_length << 8) | mp_local_buffer[0];

		//for uint
		m_incoming_length = mp_local_buffer[3];
		m_incoming_length = (m_incoming_length << 8) | mp_local_buffer[2];
		m_incoming_length = (m_incoming_length << 8) | mp_local_buffer[1];
		m_incoming_length = (m_incoming_length << 8) | mp_local_buffer[0];

		//prepare buffer for incoming data
		delete[] mp_local_buffer;
		mp_local_buffer = new unsigned char[m_incoming_length];

		return HEADER_STATUS_COMPLETE;
	}

	//if we can read at least remaining_header_size amount of bytes and no header has been found so far
	if(bytes_waiting >= m_remaining_header_bytes)
	{
		//copy the header from the receive_buffer into the local_buffer
		memcpy(mp_local_buffer,mp_connection_receive_buffer->get_mp_data(),m_remaining_header_bytes*sizeof(unsigned char));

		//assemble the length;
		//for ulong
//		m_incoming_length = mp_local_buffer[7];
//		m_incoming_length = (m_incoming_length << 8) | mp_local_buffer[6];
//		m_incoming_length = (m_incoming_length << 8) | mp_local_buffer[5];
//		m_incoming_length = (m_incoming_length << 8) | mp_local_buffer[4];
//		m_incoming_length = (m_incoming_length << 8) | mp_local_buffer[3];
//		m_incoming_length = (m_incoming_length << 8) | mp_local_buffer[2];
//		m_incoming_length = (m_incoming_length << 8) | mp_local_buffer[1];
//		m_incoming_length = (m_incoming_length << 8) | mp_local_buffer[0];

		//for uint
		m_incoming_length = mp_local_buffer[3];
		m_incoming_length = (m_incoming_length << 8) | mp_local_buffer[2];
		m_incoming_length = (m_incoming_length << 8) | mp_local_buffer[1];
		m_incoming_length = (m_incoming_length << 8) | mp_local_buffer[0];

		//printf("data length: %ld\n",m_incoming_length);

		//prepare buffer for incoming data
		delete[] mp_local_buffer;
		mp_local_buffer = new unsigned char[m_incoming_length];

		//copy the payload into the local buffer
		memcpy(mp_local_buffer,mp_connection_receive_buffer->get_mp_data()+ClusterLibFrameAssembler::HEADER_LENGTH,
				(bytes_waiting-m_remaining_header_bytes)*sizeof(unsigned char));
		//update offset
		m_local_offset += (bytes_waiting-m_remaining_header_bytes);


		//overread the header, yet still not finished with the payload
		if( (bytes_waiting-m_remaining_header_bytes) != m_incoming_length )
		{
			return HEADER_STATUS_OVERREAD;
		}
		//a perfect read occurred
		else
		{
			return HEADER_STATUS_PERFECT;
		}
	}

	return HEADER_STATUS_INCOMPLETE;
}

void ClusterLibFrameAssembler::resetAssembler()
{
	//update offset
	m_local_offset = 0;

	//we are waiting for a new header
	m_read_header = false;
	m_incoming_length = 0;

	//create new local buffer which can hold the next header
	DELETE_ARRAY_NULL_CHECKING(mp_local_buffer);
	mp_local_buffer = new unsigned char[ClusterLibFrameAssembler::HEADER_LENGTH];
}

int ClusterLibFrameAssembler::assemble(unsigned int bytes_waiting)
{
	Lazarus::Buffer* buffer = NULL;

	//********************* beginning payload assembly
	if(m_read_header == false)
	{
		enum ClusterLibFrameAssembler::HEADER_STATUS header_status = readHeader(bytes_waiting);

		switch(header_status)
		{
			case HEADER_STATUS_INCOMPLETE:
				//printf("INCOMPLETE\n");
				m_read_header = false;
				return 0;
			case HEADER_STATUS_COMPLETE:
				m_read_header = true;
				//prepare for next header
				m_remaining_header_bytes = ClusterLibFrameAssembler::HEADER_LENGTH;
				//printf("COMPLETE incoming frame length %u\n",m_incoming_length);
				return 0;
			case HEADER_STATUS_OVERREAD:
				m_read_header = true;
				//prepare for next header
				m_remaining_header_bytes = ClusterLibFrameAssembler::HEADER_LENGTH;
				//printf("OVERREAD incoming frame length %u\n",m_incoming_length);
				return 0;
			case HEADER_STATUS_PERFECT:
				//printf("PERFECT incoming frame length %u\n",m_incoming_length);
				//reset the local offset
				m_local_offset=0;

				m_read_header = false;
				//prepare for next header
				m_remaining_header_bytes = ClusterLibFrameAssembler::HEADER_LENGTH;
				//>hand< the local memory over the output buffer
				buffer = new Lazarus::Buffer(mp_local_buffer, m_incoming_length);
				//create a new buffer, as the old buffer is owned by the frame handler
				mp_local_buffer = new unsigned char[ClusterLibFrameAssembler::HEADER_LENGTH];
				//call the handler with the payload
				return mp_assembled_frame_handler_instance->handlePayload(buffer);
			default:
				printf("ERROR\n");
				return -1;
		}
	}

	//printf("CLUSTER_FRAME ASSEMBLER (accumulator): bytes waiting %ld\n",bytes_waiting);

	//************************** normal payload assembly

	//in case we still not finished after this method call
	if(m_local_offset + bytes_waiting < m_incoming_length)
	{
		//copy the waiting bytes into the local buffer
		memcpy(mp_local_buffer+m_local_offset,mp_connection_receive_buffer->get_mp_data(),bytes_waiting*sizeof(unsigned char));

		//update offset
		m_local_offset += bytes_waiting;

		return 0;
	}

	//in case we are perfectly finished after this method call
	if(m_local_offset + bytes_waiting == m_incoming_length)
	{
		//copy the waiting bytes into the local buffer
		memcpy(mp_local_buffer+m_local_offset,mp_connection_receive_buffer->get_mp_data(),bytes_waiting*sizeof(unsigned char));

		//update offset
		m_local_offset = 0;

		//we are waiting for a new header
		m_read_header = false;

		//>hand< the local memory over the output buffer
		buffer = new Lazarus::Buffer(mp_local_buffer, m_incoming_length);

		//create new local buffer which can hold the next header
		mp_local_buffer = new unsigned char[ClusterLibFrameAssembler::HEADER_LENGTH];

		//call the handler with the payload
		return mp_assembled_frame_handler_instance->handlePayload(buffer);

	}

	return -1;

}

}
