/*
 * TCPServerWorkerThread.h
 *
 *  Created on: 06.03.2013
 *      Author: Darius Malysiak
 */

#ifndef TCPSERVERWORKERTHREAD_H_
#define TCPSERVERWORKERTHREAD_H_

#include "../Threading/Thread.h"
#include "../DataStructures/FastVectorN.h"
#include "../DataStructures/Queue.h"
#include "TCPConnection.h"
#include "TCPSocket.h"
#include "DisconnectCallback.h"
#include "ConnectCallback.h"
#include "ConnectionManager.h"
#include "Protocols/FrameAssembler.h"
#include "Protocols/AssembledFrameHandler.h"
#include "../Buffer.h"
#include "../DataStructures/ThreadSafeUnorderedMap.h"
#include "CommunicationFacility.h"


namespace Lazarus
{

class TCPServerWorkerThread: public Lazarus::Thread {
public:
	TCPServerWorkerThread(unsigned int max_connections);
	virtual ~TCPServerWorkerThread();


	virtual void addConnection(TCPSocket* tcpsocket) = 0;
	//void setPoker(int epoll_read_poker);
	unsigned int get_m_connection_count();

	virtual void run();

	static const int MAX_EPOLL_EVENTS = 64;


	virtual void preFlag();

	void setComFacility(CommunicationFacility* com_facility);


	//size for the shared receive buffer
	static const unsigned int WT_RECV_BUFFER_SIZE = 640000;
	//size for the shared send buffer
	static const unsigned int WT_SEND_BUFFER_SIZE = 640000;

	void setConnectCallback(ConnectCallback* connect_callback);
	void setDisconnectCallback(DisconnectCallback* disconnect_callback);

protected:
	//Vector4 contains recv-buffer, send-buffer, the corresponding frame assembler and the tcp connection
	Lazarus::ThreadSafeUnorderedMap< int,Lazarus::FastVector4<Lazarus::Buffer*,Lazarus::Buffer*,Lazarus::FrameAssembler*,Lazarus::TCPConnection*>* > m_socket_container;

	unsigned int m_max_connections;

	//epoll system
	int m_epoll_in_fd;
	int m_epoll_out_fd;

	struct epoll_event* mp_active_in_events;
	struct epoll_event* mp_active_out_events;

	int m_epoll_read_poker;

	unsigned int m_connection_count;

	Lazarus::Buffer* mp_send_buffer;
	Lazarus::Buffer* mp_receive_buffer;

	void killAllConnections();

	//callback object for disconnect
	DisconnectCallback* mp_disconnect_callback;

	//callback object for disconnect
	ConnectCallback* mp_connect_callback;

private:
	void initEpoll();
	int m_run_count;

	CommunicationFacility* mp_cn_node_facility;
};

}

#endif /* TCPSERVERWORKERTHREAD_H_ */
