/*
 * ClusterLibFrame.h
 *
 *  Created on: 01.03.2013
 *      Author: Darius Malysiak
 */

#ifndef CLUSTERLIBFRAME_H_
#define CLUSTERLIBFRAME_H_

#include "../../../BaseObject.h"
#include "../Frame.h"
#include "ClusterLibFrameAssembler.h"

namespace Lazarus
{

class ClusterLibFrame: public Frame {
public:
	ClusterLibFrame(GenericConnection* connection);
	virtual ~ClusterLibFrame();

	/**
	 * This will attempt to read a frame, the total_timeout value is the maximal amount of time this attempt
	 * should take. 'timeout' is the internal time quantum, i.e. the time spend on a socket receive, this
	 * quantum will be accumulated until it is >= 'total_timeout'. Timeout units are milliseconds.
	 * Returns NULL in case of timeouts or errors, otherwise the frame content.
	 */
	virtual Lazarus::Buffer* readFrame( int total_timeout = -1, int timeout=-1);

	/**
	 * This will attempt to read a frame, the total_timeout value is the maximal amount of time this attempt
	 * should take. 'timeout' is the internal time quantum, i.e. the time spend on a socket receive, this
	 * quantum will be accumulated until it is >= 'total_timeout'. Timeout units are milliseconds.
	 * Returns -1 in case of timeouts or errors, otherwise 0.
	 */
	virtual int readSerializable(Lazarus::Serializable* o, int total_timeout = -1, int timeout=-1);


	/**
	 * This will attempt to send a frame, the total_timeout value is the maximal amount of time this attempt
	 * should take. 'timeout' is the internal time quantum, i.e. the time spend on a socket write, this
	 * quantum will be accumulated until it is >= 'total_timeout'. Timeout units are milliseconds.
	 * Returns -2 in case of timeouts, -1 in case of errors, 0 otherwise.
	 */
	virtual int sendFrame(Lazarus::Buffer* buffer,int total_timeout = -1,int timeout=-1);

	/**
	 * This will attempt to send a frame, the total_timeout value is the maximal amount of time this attempt
	 * should take. 'timeout' is the internal time quantum, i.e. the time spend on a socket write, this
	 * quantum will be accumulated until it is >= 'total_timeout'. Timeout units are milliseconds.
	 * Returns -2 in case of timeouts, -1 in case of errors, 0 otherwise.
	 */
	virtual int sendSerializable(Lazarus::Serializable* o,int total_timeout = -1,int timeout=-1);

	/**
	 * Returns true if a socket error occurred during the last send/receive operation
	 * */
	bool checkSocketError();

private:
	void assemble(int bytes_waiting);
	enum ClusterLibFrameAssembler::HEADER_STATUS readHeader(unsigned int bytes_waiting);

	unsigned int m_local_offset;
	unsigned char* mp_local_buffer;

	unsigned int m_remaining_header_bytes;
	bool m_read_header;
	bool m_read_complete_frame;
	bool m_socket_error;
	bool m_total_timeout;

	unsigned int m_incoming_length;

	Lazarus::Buffer* mp_connection_receive_buffer;
	Lazarus::Buffer* mp_received_payload_buffer;

};

}

#endif /* CLUSTERLIBFRAME_H_ */
