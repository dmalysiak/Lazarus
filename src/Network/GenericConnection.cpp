/*
 * GenericConnection.cpp
 *
 *  Created on: 02.03.2015
 *      Author: Darius Malysiak
 */

#include "GenericConnection.h"

namespace Lazarus {

GenericConnection::GenericConnection() {

	mp_send_buffer = NULL;
	mp_receive_buffer = NULL;
	mp_socket = NULL;
	m_connection_id = 0;
	m_is_active = false;
}

GenericConnection::~GenericConnection() {

	if(mp_socket != NULL)
	{
		mp_socket->closeSocket();
	}

	DELETE_NULL_CHECKING(mp_socket);

}


int GenericConnection::disconnect()
{
	if(mp_socket != NULL)
	{
		mp_socket->closeSocket();
	}

	DELETE_NULL_CHECKING(mp_socket);

	m_is_active = false;

	return 0;
}

unsigned int GenericConnection::get_m_connection_id()
{
	return m_connection_id;
}

bool GenericConnection::isActive()
{
	return m_is_active;
}


Lazarus::Buffer* GenericConnection::get_mp_receive_buffer()
{
	return mp_receive_buffer;
}

Lazarus::Buffer* GenericConnection::get_mp_send_buffer()
{
	return mp_send_buffer;
}

} /* namespace Lazarus */
