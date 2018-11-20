/*
 * P2PServer.h
 *
 *  Created on: 20.03.2013
 *      Author: Darius Malysiak
 */

#ifndef P2PFRAMESERVER_H_
#define P2PFRAMESERVER_H_

#include "P2PServer.h"
#include "../../Buffer.h"
#include "../Protocols/FrameAssembler.h"

namespace Lazarus {

/**
 * This class allows direct TCP connection between two parties.
 * */
class P2PFrameServer: public P2PServer {
public:
	/**
	 * If both default values are replaced by different data,
	 * the underlying socket will be bound to that address information.
	 * */
	P2PFrameServer(const std::string& local_address, unsigned int local_port, const std::string& local_device="");
	virtual ~P2PFrameServer();

	/**
	 * This will attempt to send a frame, the total_timeout value is the maximal amount of time this attempt
	 * should take. 'timeout' is the internal time quantum, i.e. the time spend on a socket write, this
	 * quantum will be accumulated until it is >= 'total_timeout'. Timeout units are milliseconds.
	 * Returns -2 in case of timeouts, -1 in case of errors, 0 otherwise.
	 */
	int sendFrame(const Lazarus::Buffer& data, int total_timeout = -1, int timeout=-1);

	/**
	 * This will attempt to read a frame, the total_timeout value is the maximal amount of time this attempt
	 * should take. 'timeout' is the internal time quantum, i.e. the time spend on a socket receive, this
	 * quantum will be accumulated until it is >= 'total_timeout'. Timeout units are milliseconds.
	 * Returns NULL in case of timeouts, otherwise the frame content.
	 */
	Lazarus::Buffer* receiveFrame(int total_timeout = -1, int timeout=-1);


protected:

};

} /* namespace Lazarus */
#endif /* P2PSERVER_H_ */

