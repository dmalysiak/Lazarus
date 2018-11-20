/*
 * TCPServer.h
 *
 *  Created on: 28.02.2013
 *      Author: Darius Malysiak
 */

#ifndef TCPSERVER_H_
#define TCPSERVER_H_

#include "TCPSocket.h"
#include "../Threading/Thread.h"
#include "TCPServerWorkerThread.h"
#include "CommunicationFacility.h"


#include <sys/epoll.h>
#include <stdio.h>
#include <string>

namespace Lazarus
{

class TCPServer: public Lazarus::Thread {
public:

	TCPServer(std::string local_address, unsigned int local_port, unsigned int max_connections,
			unsigned int worker_threads);

	virtual ~TCPServer();

	void startServer();

	/*
	 * This is equivalent with calling 'shutdown()' of the 'Thread' class, i.e. this method will only call 'shutdown()',
	 * nothing more than that!
	 */
	void stopServer();

	/*
	* This method should handle all incoming connection >requests<,
	* it is the only method which will be executed by the thread
	*/
	virtual void listener_method() = 0;

	virtual void createWorkerThreads() = 0;

	virtual void run();

	virtual void preFlag();


	static const int LISTENER_QUEUE_SIZE = 20;
	static const int SHUTDOWN_DELAY_MS = 1000;
	static const int MAX_EPOLL_EVENTS = 64;

	/**
	 * Returns the servers current IP address.
	 * */
	std::string getAddress();

	/**
	 * Returns the servers current port.
	 * */
	unsigned int getPort();

	void setConnectCallback(ConnectCallback* connect_callback);
	void setDisconnectCallback(DisconnectCallback* disconnect_callback);

protected:
	std::string m_local_address;
	unsigned int m_local_port;
	TCPSocket* mp_socket;

	unsigned int m_current_connections;
	unsigned int m_max_connections;

	TCPServerWorkerThread** mp_worker_threads;
	unsigned int m_worker_thread_count;

	//********************* epoll system
	int m_epoll_in_fd;
	struct epoll_event* mp_active_in_events;
	int m_epoll_read_poker;//poker for the listener

	unsigned int m_run_count;

	/**
	 * The communication facility will be used to enable all threads to communicate among each other.
	 * It will be forwarded up to the frame handler!
	 * Ownership will never be forwarded!! I.e. this class will destroy it! (even if a far child class created it!)
	 */
	CommunicationFacility* mp_cn_node_facility;

	//callback object for disconnect
	DisconnectCallback* mp_disconnect_callback;

	//callback object for disconnect
	ConnectCallback* mp_connect_callback;
};

}

#endif /* TCPSERVER_H_ */
