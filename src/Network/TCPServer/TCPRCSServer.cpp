/*
 * TCPRCSServer.cpp
 *
 *  Created on: 09.03.2015
 *      Author: Darius Malysiak
 */

#include "TCPRCSServer.h"
#include "../../SerializationStack.h"
#include "../../Toolbox.h"

namespace Lazarus {

TCPRCSServer::TCPRCSServer(std::string local_address, unsigned int local_port,
		unsigned int listen_timeout, unsigned int read_frame_timeout,
		enum TCPSocket::ADDRESS_TYPE address_type, const std::string& interface) {

	m_default_repo_checkout_folder = "/tmp";
	pthread_mutex_init(&m_mutex,NULL);
	m_advertised_port = 0;
	m_incoming_rcs_client_group = 0;
	m_update_counter = 0;

	m_state = TCPRCSSERVER_STATE_WAITING_FOR_CONNECTION;
	mp_client_connection = NULL;
	mp_frame = NULL;
	mp_socket = new TCPSocket(local_address,local_port,address_type);

	int res = 0;
	if(interface.length()==0)
	{
		//printf("1\n");mp_socket->printSocketInformation();
		res = mp_socket->bindSocket();
	}
	else
	{
		//printf("2\n");
		res = mp_socket->bindSocketToDevice(interface);
	}

	if(res != 0)
	{
		printf("ERROR: TCPRCSServer could not bind socket\n");
		DELETE_NULL_CHECKING(mp_socket);
		m_state = TCPRCSSERVER_STATE_UNDEFINED;
	}
	else
	{
		//register within internal poll system
		m_poll_in_fds.fd = this->mp_socket->get_m_socket_fd();
		m_poll_in_fds.events = POLLIN;

		m_local_port = local_port;
		m_local_ip = local_address;
	}

	m_listen_timeout = listen_timeout;
	m_read_frame_timeout = read_frame_timeout;
}

TCPRCSServer::~TCPRCSServer()
{
	delete mp_socket;//delete the >listener< socket
	DELETE_NULL_CHECKING(mp_frame);
	DELETE_NULL_CHECKING(mp_client_connection);

	pthread_mutex_destroy(&m_mutex);
}


void TCPRCSServer::setGroupIDs(const Lazarus::FastKTuple<unsigned int>& group_ids)
{
	m_group_ids = group_ids;
}

Lazarus::FastVector4<std::string,unsigned int, unsigned int, unsigned int>* TCPRCSServer::getServerInfo()
{
	Lazarus::FastVector4<std::string,unsigned int, unsigned int, unsigned int>* info;
	pthread_mutex_lock(&m_mutex);

	info = new Lazarus::FastVector4<std::string,unsigned int, unsigned int, unsigned int>();
	info->m_data1 = m_advertised_server_ip;
	info->m_data2 = m_advertised_port;
	info->m_data3 = m_incoming_rcs_client_group;
	info->m_data4 = m_update_counter;

	pthread_mutex_unlock(&m_mutex);

	return info;
}

void TCPRCSServer::processMessage(Lazarus::Buffer* message)
{
	enum TCPRCSServer::TCPRCS_MESSAGE_TYPE type = getRequestType(message);

	switch(type)
	{
	case TCPRCS_MESSAGE_TYPE_CNMG_CON_REQUEST:
	{
		//extract all server information and reply
		Lazarus::SerializationStack data;
		data.setStackCopy(message);
		std::string ip = data.getString();//ip
		unsigned int rcs_group = data.getElement<unsigned int>();//rcs group
		unsigned int port = data.getElement<unsigned int>();//port
		data.getElement<enum TCPRCSServer::TCPRCS_MESSAGE_TYPE>();//omit type

		//check if this client can accept the advertisement
		bool id_ok = false;
		for(unsigned int i=0;i<m_group_ids.getm_size();++i)
		{
			//if at least one of the clients group ids equals the incoming id
			if(m_group_ids[i] == rcs_group)
			{
				printf("Client accepts the TCP RCS beacon, client rcs group %u fits requested group %u\n",
						m_group_ids[i], rcs_group);
				id_ok = true;
				break;
			}
			else
			{
				//printf("%u != %u\n",m_group_ids[i], rcs_group);
			}
		}
		//abort if not
		if(id_ok == false)
		{
			printf("Can't accept the request, incoming group id does not fit the client.\n");

			//send a reply
			Lazarus::Buffer response(sizeof(enum TCPRCSServer::TCPRCS_MESSAGE_TYPE));
			setRequestType(&response,TCPRCS_MESSAGE_TYPE_CNMG_CON_RCSGROUP_ERROR);
			mp_frame->sendFrame(&response);

			break;
		}

		pthread_mutex_lock(&m_mutex);

		m_advertised_server_ip = ip;
		m_advertised_port = port;
		m_incoming_rcs_client_group = rcs_group;
		++m_update_counter;

		pthread_mutex_unlock(&m_mutex);

		//send a reply
		Lazarus::Buffer response(sizeof(enum TCPRCSServer::TCPRCS_MESSAGE_TYPE));
		setRequestType(&response,TCPRCS_MESSAGE_TYPE_CNMG_CON_OK);
		int res = mp_frame->sendFrame(&response);

		if(res != 0)
		{
			printf("ERROR: could not send a response.\n");
		}

		break;
	}

	case TCPRCS_MESSAGE_TYPE_SH_UPDATE_REQUEST:
	{
		//extract all server information and reply
		Lazarus::SerializationStack data;
		data.setStackCopy(message);
		std::string password = data.getString();//password
		std::string repo_user = data.getString();//repo user
		std::string checkout_folder = data.getString();//svn checkout folder
		std::string repo_server_address = data.getString();//svn repo
		unsigned int keep_client_config = data.getElement<unsigned int>();//keep_client_config
		unsigned int sh_module_level = data.getElement<unsigned int>();//sh_module_level
		unsigned int update = data.getElement<unsigned int>();//update
		data.getElement<enum TCPRCSServer::TCPRCS_MESSAGE_TYPE>();//omit type

		//check if the default checkout folder should be used
		if(checkout_folder.size() == 0)
		{
			checkout_folder = m_default_repo_checkout_folder;
		}

		//in case of an svn update check if a working copy exists
		if(update == 1)
		{
			if(Lazarus::Toolbox::checkFileExistence(checkout_folder+"/lazarus") == false ||
					Lazarus::Toolbox::checkFileExistence(checkout_folder+"/LazarusSystemFacilities") == false ||
					Lazarus::Toolbox::checkFileExistence(checkout_folder+"/LazarusClusterClientDaemon") == false)
			{
				//send a reply
				Lazarus::Buffer response(sizeof(enum TCPRCSServer::TCPRCS_MESSAGE_TYPE));
				setRequestType(&response,TCPRCS_MESSAGE_TYPE_SH_UPDATE_RESPONSE_NO_WORKING_COPY);
				mp_frame->sendFrame(&response);

				return;
			}
		}

		//send a reply
		Lazarus::Buffer response(sizeof(enum TCPRCSServer::TCPRCS_MESSAGE_TYPE));
		setRequestType(&response,TCPRCS_MESSAGE_TYPE_SH_UPDATE_RESPONSE);
		mp_frame->sendFrame(&response);

		//actually do the update
		std::string command = "/usr/bin/SHDeployTCPRCS.sh "+repo_server_address+" "+checkout_folder+" "+repo_user+" "+password+
				" "+Lazarus::Toolbox::uintToString(sh_module_level)+" "+Lazarus::Toolbox::uintToString(update) + " " + Lazarus::Toolbox::uintToString(keep_client_config) + " &";

		printf("updating via %s\n",command.c_str());

		system(command.c_str());

		break;
	}

	default:
		break;
	}

}

void TCPRCSServer::setDefaultCheckoutFolder(const std::string& folder)
{
	m_default_repo_checkout_folder = folder;
}

void TCPRCSServer::run()
{
	while(m_shutdown_flag == false)
	{
		switch(m_state)
		{
		case TCPRCSSERVER_STATE_WAITING_FOR_CONNECTION:
		{
			//wait for an incoming connection
			int res = waitForClient(m_listen_timeout);

			if(res == -2)
			{
				m_state = TCPRCSSERVER_STATE_UNDEFINED;
			}

			if(res == 0)
			{
				mp_frame = new ClusterLibFrame(mp_client_connection);
				m_state = TCPRCSSERVER_STATE_ACTIVE;
			}

			break;
		}
		case TCPRCSSERVER_STATE_ACTIVE:
		{
			Lazarus::Buffer* message = mp_frame->readFrame(10*m_read_frame_timeout,m_read_frame_timeout);

			if(message != NULL)
			{
				processMessage(message);

				delete message;
			}
			else //check if the socket produced an error
			{
				if(mp_frame->checkSocketError() == true)
				{
					printf("TCP RCS client disconnected\n");
					DELETE_NULL_CHECKING(mp_frame);
					DELETE_NULL_CHECKING(mp_client_connection);
					m_state = TCPRCSSERVER_STATE_WAITING_FOR_CONNECTION;
				}
			}

			break;
		}

		case TCPRCSSERVER_STATE_UNDEFINED:

			printf("ERROR: TCPRCSServer encountered fatal error\n");
			m_shutdown_flag = true;
			m_thread_self_terminated = true;
			return;

			break;

		default:
			break;
		}
	}

	//this ensures a definitive one-shot server, i.e. it can't be restarted!
	m_state = TCPRCSSERVER_STATE_UNDEFINED;
}

int TCPRCSServer::waitForClient(int timeout)
{
	//delete any existing connection
	if(mp_client_connection == NULL)
	{
		delete mp_client_connection;
		mp_client_connection = NULL;
	}

	int socket_fd;
	struct sockaddr_in connection_data;
	unsigned int size = sizeof(sockaddr_in);

	//listen for new connections
	if(listen(mp_socket->get_m_socket_fd(), 1) == -1)
	{
		printf("ERROR: could not setup a listener socket\n");
		exit(1);
	}

	//wait for a connection
	//printf("waiting for connections\n");

	//wait until a connection arrives (event based)
	int waiting_event_count = poll(&m_poll_in_fds, 1,timeout);

	//timeout
	if(waiting_event_count == 0)
	{
		return -1;
	}

	//error
	if( (m_poll_in_fds.revents & POLLIN) != POLLIN)
	{
		printf("ERROR: expected a POLLIN\n");
		return -2;
	}

	//error
	if(waiting_event_count == -1)
	{
		printf("ERROR: fatal socket problem\n");
		return -2;
	}

	//check if socket has been closed
	if((m_poll_in_fds.revents & POLLRDHUP) != 0 || (m_poll_in_fds.revents & POLLHUP) != 0)
	{
		printf("ERROR: listener socket is closed\n");
		return -2;
	}

	socket_fd = accept(mp_socket->get_m_socket_fd(), (struct sockaddr*) &connection_data, &size);

	if(socket_fd == -1)
	{
		printf("stopping listening: error on accept or socket closed\n");
		return -2;
	}

	printf("connection accepted\n");
	//create a (non-autonomous) connection from socket data
	//TCPConnection* connection = new TCPConnection(0,false);
	TCPSocket* tcpsocket = new TCPSocket(socket_fd);

	//fill the socket information into the socket object
	char* c_strbuffer = new char[500];
	inet_ntop(AF_INET, &connection_data.sin_addr, c_strbuffer, 500);

	tcpsocket->m_address_type = Socket::ADDRESS_TYPE_IPV4;
	tcpsocket->m_local_address = m_local_ip;
	tcpsocket->m_local_port = m_local_port;
	//tcpsocket->m_socket_fd = socket_fd;
	tcpsocket->m_remote_address = std::string(c_strbuffer);
	tcpsocket->m_remote_port = ntohs(connection_data.sin_port);

	//free the cstring buffer
	delete[] c_strbuffer;

	mp_client_connection = new TCPConnectionPoll(0);
	mp_client_connection->set_mp_socket(tcpsocket);

	return 0;

}


enum TCPRCSServer::TCPRCS_MESSAGE_TYPE TCPRCSServer::getRequestType(Lazarus::Buffer* message)
{
	enum TCPRCSServer::TCPRCS_MESSAGE_TYPE type;

	memcpy(&type,message->get_mp_data(),sizeof(enum TCPRCSServer::TCPRCS_MESSAGE_TYPE));

	return type;
}

void TCPRCSServer::setRequestType(Lazarus::Buffer* message,TCPRCSServer::TCPRCS_MESSAGE_TYPE type)
{
	memcpy(message->get_mp_data(),&type,sizeof(enum TCPRCSServer::TCPRCS_MESSAGE_TYPE));
}

void TCPRCSServer::setContent(Lazarus::Buffer* message,unsigned char* content, unsigned int size)
{
	unsigned int type_size = sizeof(enum TCPRCSServer::TCPRCS_MESSAGE_TYPE);

	memcpy(message->get_mp_data()+type_size,content,size*sizeof(unsigned char));
}

/*
 * this method will allocate an adequate buffer at 'content' and set 'size' accordingly
 */
void TCPRCSServer::getContent(Lazarus::Buffer* message,unsigned char** content, unsigned int &size)
{
	unsigned int type_size = sizeof(enum TCPRCSServer::TCPRCS_MESSAGE_TYPE);

	size = message->get_m_length()-type_size;

	*content = new unsigned char[size];

	memcpy(*content,message->get_mp_data()+type_size,size*sizeof(unsigned char));
}

} /* namespace Lazarus */
