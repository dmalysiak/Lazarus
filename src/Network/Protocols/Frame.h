/*
 * Frame.h
 *
 *  Created on: 19.03.2013
 *      Author: Darius Malysiak
 */

#ifndef FRAME_H_
#define FRAME_H_

#include "../../BaseObject.h"

#include "../GenericConnection.h"
#include "../../Time/TimeToolbox.h"

namespace Lazarus
{

class Frame: public Lazarus::BaseObject {
public:
	Frame(GenericConnection* connection);
	virtual ~Frame();

	/**
	 * This will attempt to read a frame, the total_timeout value is the maximal amount of time this attempt
	 * should take. 'timeout' is the internal time quantum, i.e. the time spend on a socket receive, this
	 * quantum will be accumulated until it is >= 'total_timeout'. Timeout units are milliseconds.
	 * Returns NULL in case of timeouts, otherwise the frame content.
	 */
	virtual Lazarus::Buffer* readFrame( int total_timeout = -1, int timeout=-1) = 0;

	/**
	 * This will attempt to read a frame, the total_timeout value is the maximal amount of time this attempt
	 * should take. 'timeout' is the internal time quantum, i.e. the time spend on a socket receive, this
	 * quantum will be accumulated until it is >= 'total_timeout'. Timeout units are milliseconds.
	 * Returns -1 in case of timeouts or errors, otherwise 0.
	 */
	virtual int readSerializable(Lazarus::Serializable* o, int total_timeout = -1, int timeout=-1) = 0;


	/**
	 * This will attempt to send a frame, the total_timeout value is the maximal amount of time this attempt
	 * should take. 'timeout' is the internal time quantum, i.e. the time spend on a socket write, this
	 * quantum will be accumulated until it is >= 'total_timeout'. Timeout units are milliseconds.
	 * Returns -2 in case of timeouts, -1 in case of errors, 0 otherwise.
	 */
	virtual int sendFrame(Lazarus::Buffer* buffer, int total_timeout = -1, int timeout=-1) = 0;

	/**
	 * This will attempt to send a frame, the total_timeout value is the maximal amount of time this attempt
	 * should take. 'timeout' is the internal time quantum, i.e. the time spend on a socket write, this
	 * quantum will be accumulated until it is >= 'total_timeout'. Timeout units are milliseconds.
	 * Returns -2 in case of timeouts, -1 in case of errors, 0 otherwise.
	 */
	virtual int sendSerializable(Lazarus::Serializable* o,int total_timeout = -1,int timeout=-1) = 0;

protected:
	GenericConnection* mp_connection;
	unsigned int m_total_timeout_val;

	/*struct timespec* m_a;
	struct timespec* m_b;
	struct timespec* m_diff;*/
};

}

#endif /* FRAME_H_ */
