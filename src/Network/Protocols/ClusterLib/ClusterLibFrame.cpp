/*
 * ClusterLibFrame.cpp
 *
 *  Created on: 01.03.2013
 *      Author: Darius Malysiak
 */

#include "ClusterLibFrame.h"
#include "../../../Toolbox.h"

#include "../../TCPConnection.h"

#include <string.h>

namespace Lazarus
{

ClusterLibFrame::ClusterLibFrame(GenericConnection* connection) : Frame(connection){

	mp_local_buffer = new unsigned char[ClusterLibFrameAssembler::HEADER_LENGTH];
	m_local_offset = 0;
	m_incoming_length = 0;
	m_read_header = false;
	m_remaining_header_bytes = ClusterLibFrameAssembler::HEADER_LENGTH;
	m_read_complete_frame = false;
	mp_connection_receive_buffer = connection->get_mp_receive_buffer();
	mp_received_payload_buffer = NULL;
	m_socket_error = false;
	m_total_timeout = false;

}

ClusterLibFrame::~ClusterLibFrame() {

	//we don't delete the received payload buffer as we claim no ownership over the object

	DELETE_ARRAY_NULL_CHECKING(mp_local_buffer);
}

/**
 * Every call to this method releases the ownership of the current 'received_payload_buffer' object.
 * This will attempt to read a frame, the total_timeout value is the maximal amount of time this attempt
 * should take. 'timeout' is the internal time quantum, i.e. the time spend on a socket receive, this
 * quantum will be accumulated until it is >= 'total_timeout'. Timeout units are milliseconds.
 * Returns NULL in case of timeouts, otherwise the frame content.
 */
Lazarus::Buffer* ClusterLibFrame::readFrame(int total_timeout, int timeout)
{
	//reset the indicators
	m_total_timeout = false;
	m_socket_error = false;

	int result = 0;
	//int count = 0;

	//virtually free the current buffer, i.e. give it up
	mp_received_payload_buffer = NULL;

	//setup total timeout
	if(total_timeout != -1)
	{
		m_total_timeout_val = 0;//reset the internal counter;
		m_total_timeout = false;
	}

	while(m_read_complete_frame == false)
	{
		//printf("receive iteration %u\n",count);
		//printf("receive iteration\n");

		//result = ((TCPConnection*)mp_connection)->receiveQuark(timeout);
		result = mp_connection->receive(timeout);
		//printf("recv result %d for sd %d\n",result,((TCPConnection*)mp_connection)->get_mp_socket()->get_m_socket_fd());
		//printf("*************result = %d with timeout %d / total timeout %d\n",result,timeout,total_timeout);
		if(result > 0)//only assemble if something has been received
		{
			assemble( result );
			//printf("assembled %d bytes, local offset %u\n",result,m_local_offset);
		}

		/*if(m_read_complete_frame == false)
		{
			printf("not complete yet %u/%u\n",m_local_offset,m_incoming_length);
			count++;
		}*/

		//received_payload_buffer->printfBuffer();
		//check if the total timeout has occurred
		if(total_timeout != -1 && result == -2)
		{
			m_total_timeout_val += timeout;
			if(m_total_timeout_val >= (unsigned int)total_timeout)
			{
				printf("total timeout in reading frame\n");
				m_total_timeout = true;
				break;
			}
		}

		//check if an error has occurred
		if(result == -1)
		{
			//break out
			m_read_complete_frame = true;
			m_total_timeout = false;
			m_socket_error = true;
			//we are waiting for a new header
			m_read_header = false;
			//update offset
			m_local_offset = 0;
			//update expected size
			m_incoming_length = 0;
		}
	}

	//prepare for next call
	m_read_complete_frame = false;

	//check if something went wrong on the socket layer
	if(m_total_timeout == true || m_socket_error ==true)
	{
		DELETE_NULL_CHECKING(mp_received_payload_buffer);
	}

	return mp_received_payload_buffer;
}

int ClusterLibFrame::readSerializable(Lazarus::Serializable* o, int total_timeout, int timeout)
{
	//reset the indicators
	m_total_timeout = false;
	m_socket_error = false;

	int result = 0;
	//int count = 0;

	//virtually free the current buffer, i.e. give it up
	mp_received_payload_buffer = NULL;

	//setup total timeout
	if(total_timeout != -1)
	{
		m_total_timeout_val = 0;//reset the internal counter;
		m_total_timeout = false;
	}

	while(m_read_complete_frame == false)
	{
		result = mp_connection->receive(timeout);

		if(result > 0)//only assemble if something has been received
		{
			assemble( result );
		}

		//received_payload_buffer->printfBuffer();
		//check if the total timeout has occurred
		if(total_timeout != -1 && result == -2)
		{
			m_total_timeout_val += timeout;
			if(m_total_timeout_val >= (unsigned int)total_timeout)
			{
				printf("total timeout in reading frame\n");
				m_total_timeout = true;
				break;
			}
		}

		//check if an error has occurred
		if(result == -1)
		{
			//break out
			m_read_complete_frame = true;
			m_total_timeout = false;
			m_socket_error = true;
			//we are waiting for a new header
			m_read_header = false;
			//update offset
			m_local_offset = 0;
			//update expected size
			m_incoming_length = 0;
		}
	}

	//prepare for next call
	m_read_complete_frame = false;

	//check if something went wrong on the socket layer
	if(m_total_timeout == true || m_socket_error ==true)
	{
		DELETE_NULL_CHECKING(mp_received_payload_buffer);
		return -1;
	}

	o->set_mp_serialized_data_buffer(mp_received_payload_buffer);
	o->deserialize();

	return 0;
}

bool ClusterLibFrame::checkSocketError()
{
	return m_socket_error;
}

/**
 * This will attempt to send a frame, the total_timeout value is the maximal amount of time this attempt
 * should take. 'timeout' is the internal time quantum, i.e. the time spend on a socket write, this
 * quantum will be accumulated until it is >= 'total_timeout'. Timeout units are milliseconds.
 * Returns -2 in case of timeouts, -1 in case of errors, 0 otherwise.
 */
int ClusterLibFrame::sendFrame(Lazarus::Buffer* buffer,int total_timeout,int timeout)
{
	Lazarus::Buffer* connection_send_buffer = mp_connection->get_mp_send_buffer();
	unsigned int length = buffer->get_m_length();

	//setup total timeout
	if(total_timeout != -1)
	{
		m_total_timeout_val = 0;//reset the internal counter;
		m_total_timeout = false;
	}

	//if buffer must be transmitted via multiple tcp loads
	if(length + ClusterLibFrameAssembler::HEADER_LENGTH > mp_connection->get_mp_send_buffer()->get_m_length())
	{
		//printf("send buffer of connection is not large enough for data\n");

		unsigned char* connection_transmit_buffer = connection_send_buffer->get_mp_data();
		unsigned char* transmit_buffer = buffer->get_mp_data();
		int result = 0;
		unsigned int transmitted_bytes = 0;
		unsigned int transmission_unit = 0;
		unsigned int connection_buffer_size = connection_send_buffer->get_m_length();


		//copy data length into the buffer
		memcpy(connection_transmit_buffer,&length, ClusterLibFrameAssembler::HEADER_LENGTH );

		//send the header first
		if( (result=mp_connection->send_data(ClusterLibFrameAssembler::HEADER_LENGTH)) == -1)
		{
			printf("ERROR: could not send header of BIG PACKET\n");
			return -1;
		}

		//check if the total timeout has occurred
		if(total_timeout != -1 && result == -2)
		{
			m_total_timeout_val += timeout;
			if((long int)m_total_timeout_val >= total_timeout)
			{
				printf("total timeout in sending header of BIG PACKET\n");
				m_total_timeout = true;
				return -2;
			}
		}

		//if the header has been send, send the actual payload
		while(transmitted_bytes < length)
		{	//printf("transmitted before %d\n",transmitted_bytes);
			//determine size of next load
			//if the connection buffer can't hold the remaining bytes, cap the transmission unit at the maximal size
			if(transmitted_bytes + connection_buffer_size < length)
			{
				transmission_unit = connection_buffer_size;
			}
			//last fraction of data
			else
			{
				transmission_unit = length - (transmitted_bytes);
			}

			//copy data into the buffer
			memcpy(connection_transmit_buffer, transmit_buffer+transmitted_bytes, sizeof(unsigned char) * transmission_unit );

			//transmit the buffer
			//printf("going to send %u byte fragment\n",transmission_unit);
			if( (result=mp_connection->send_data(transmission_unit,timeout)) == -1)
			{
				printf("error sending fragment\n");
				return -1;
			}

			//check if the total timeout has occurred
			if(total_timeout != -1 && result == -2)
			{
				m_total_timeout_val += timeout;
				if((long int)m_total_timeout_val >= total_timeout)
				{
					printf("total timeout in sending fragment\n");
					m_total_timeout = true;
					return -2;
				}
			}

			transmitted_bytes += transmission_unit; //printf("transmitted after %d\n",transmitted_bytes);Lazarus::Toolbox::sleep_ms(10);
		}


		return 0;
	}
	//in case package fits entirely into the connection buffer
	else
	{
		//copy data length into the buffer
		memcpy(connection_send_buffer->get_mp_data(),&length, ClusterLibFrameAssembler::HEADER_LENGTH );

		//copy data into the buffer
		memcpy(connection_send_buffer->get_mp_data() + ClusterLibFrameAssembler::HEADER_LENGTH, buffer->get_mp_data(),sizeof(unsigned char)*length );


		int result = mp_connection->send_data(length+ClusterLibFrameAssembler::HEADER_LENGTH,timeout);

		//check if the total timeout has occurred
		if(total_timeout != -1 && result == -2)
		{
			m_total_timeout_val += timeout;
			if(m_total_timeout_val >= (unsigned int)total_timeout)
			{
				printf("total timeout in sending fragment\n");
				m_total_timeout = true;
				return -2;
			}
		}

		return 0;
	}

	return 0;
}


/**
 * This will attempt to send a frame, the total_timeout value is the maximal amount of time this attempt
 * should take. 'timeout' is the internal time quantum, i.e. the time spend on a socket write, this
 * quantum will be accumulated until it is >= 'total_timeout'. Timeout units are milliseconds.
 * Returns -2 in case of timeouts, -1 in case of errors, 0 otherwise.
 */
int ClusterLibFrame::sendSerializable(Lazarus::Serializable* o,int total_timeout,int timeout)
{
	o->serialize();
	Lazarus::Buffer* buffer = o->get_mp_serialized_data_buffer();

	Lazarus::Buffer* connection_send_buffer = mp_connection->get_mp_send_buffer();
	unsigned int length = buffer->get_m_length();

	//setup total timeout
	if(total_timeout != -1)
	{
		m_total_timeout_val = 0;//reset the internal counter;
		m_total_timeout = false;
	}

	//if buffer must be transmitted via multiple tcp loads
	if(length + ClusterLibFrameAssembler::HEADER_LENGTH > mp_connection->get_mp_send_buffer()->get_m_length())
	{
		//printf("send buffer of connection is not large enough for data\n");

		unsigned char* connection_transmit_buffer = connection_send_buffer->get_mp_data();
		unsigned char* transmit_buffer = buffer->get_mp_data();
		int result = 0;
		unsigned int transmitted_bytes = 0;
		unsigned int transmission_unit = 0;
		unsigned int connection_buffer_size = connection_send_buffer->get_m_length();


		//copy data length into the buffer
		memcpy(connection_transmit_buffer,&length, ClusterLibFrameAssembler::HEADER_LENGTH );

		//send the header first
		if( (result=mp_connection->send_data(ClusterLibFrameAssembler::HEADER_LENGTH)) == -1)
		{
			printf("ERROR: could not send header of BIG PACKET\n");
			return -1;
		}

		//check if the total timeout has occurred
		if(total_timeout != -1 && result == -2)
		{
			m_total_timeout_val += timeout;
			if((long int)m_total_timeout_val >= total_timeout)
			{
				printf("total timeout in sending header of BIG PACKET\n");
				m_total_timeout = true;
				return -2;
			}
		}

		//if the header has been send, send the actual payload
		while(transmitted_bytes < length)
		{	//printf("transmitted before %d\n",transmitted_bytes);
			//determine size of next load
			//if the connection buffer can't hold the remaining bytes, cap the transmission unit at the maximal size
			if(transmitted_bytes + connection_buffer_size < length)
			{
				transmission_unit = connection_buffer_size;
			}
			//last fraction of data
			else
			{
				transmission_unit = length - (transmitted_bytes);
			}

			//copy data into the buffer
			memcpy(connection_transmit_buffer, transmit_buffer+transmitted_bytes, sizeof(unsigned char) * transmission_unit );

			//transmit the buffer
			//printf("going to send %u byte fragment\n",transmission_unit);
			if( (result=mp_connection->send_data(transmission_unit,timeout)) == -1)
			{
				printf("error sending fragment\n");
				return -1;
			}

			//check if the total timeout has occurred
			if(total_timeout != -1 && result == -2)
			{
				m_total_timeout_val += timeout;
				if((long int)m_total_timeout_val >= total_timeout)
				{
					printf("total timeout in sending fragment\n");
					m_total_timeout = true;
					return -2;
				}
			}

			transmitted_bytes += transmission_unit; //printf("transmitted after %d\n",transmitted_bytes);Lazarus::Toolbox::sleep_ms(10);
		}


		return 0;
	}
	//in case package fits entirely into the connection buffer
	else
	{
		//copy data length into the buffer
		memcpy(connection_send_buffer->get_mp_data(),&length, ClusterLibFrameAssembler::HEADER_LENGTH );

		//copy data into the buffer
		memcpy(connection_send_buffer->get_mp_data() + ClusterLibFrameAssembler::HEADER_LENGTH, buffer->get_mp_data(),sizeof(unsigned char)*length );


		int result = mp_connection->send_data(length+ClusterLibFrameAssembler::HEADER_LENGTH,timeout);

		//check if the total timeout has occurred
		if(total_timeout != -1 && result == -2)
		{
			m_total_timeout_val += timeout;
			if(m_total_timeout_val >= (unsigned int)total_timeout)
			{
				printf("total timeout in sending fragment\n");
				m_total_timeout = true;
				return -2;
			}
		}

		return 0;
	}

	return 0;
}

enum ClusterLibFrameAssembler::HEADER_STATUS ClusterLibFrame::readHeader(unsigned int bytes_waiting)
{
	//if we can't read reamining_header_size amount of bytes and no header has been found so far
	if(bytes_waiting < m_remaining_header_bytes)
	{
		//copy the header from the receive_buffer into the local_buffer
		memcpy(mp_local_buffer,mp_connection_receive_buffer->get_mp_data(),bytes_waiting*sizeof(unsigned char));

		//remaining header bytes
		m_remaining_header_bytes = m_remaining_header_bytes - bytes_waiting;

		//still not finished
		return ClusterLibFrameAssembler::HEADER_STATUS_INCOMPLETE;
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
		DELETE_ARRAY_NULL_CHECKING(mp_local_buffer);
		mp_local_buffer = new unsigned char[m_incoming_length];

		//printf("allocating local buffer for frame receive of size %d\n",m_incoming_length);
		//printf("READ_HEADER (EXACT): data length: %ld\n",m_incoming_length);

		return ClusterLibFrameAssembler::HEADER_STATUS_COMPLETE;
	}

	//if we can read at least remaining_header_size amount of bytes and no header has been found so far
	if(bytes_waiting > m_remaining_header_bytes)
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
		DELETE_ARRAY_NULL_CHECKING(mp_local_buffer);
		mp_local_buffer = new unsigned char[m_incoming_length];

		//copy the payload into the local buffer
		memcpy(mp_local_buffer, mp_connection_receive_buffer->get_mp_data()+ClusterLibFrameAssembler::HEADER_LENGTH,
				(bytes_waiting-m_remaining_header_bytes)*sizeof(unsigned char));
		//update offset
		m_local_offset += (bytes_waiting - m_remaining_header_bytes);


		//overread the header, yet still not finished with the payload
		if( (bytes_waiting - m_remaining_header_bytes) != m_incoming_length )
		{
			//printf("READ_HEADER (OVERREAD - NOT FINISHED): data length: %ld, read %d\n",m_incoming_length,m_local_offset);
			return ClusterLibFrameAssembler::HEADER_STATUS_OVERREAD;
		}
		//a perfect read occurred
		else
		{
			//printf("READ_HEADER (OVERREAD -PERFECT): data length: %ld\n",m_incoming_length);
			return ClusterLibFrameAssembler::HEADER_STATUS_PERFECT;
		}
	}

	return ClusterLibFrameAssembler::HEADER_STATUS_INCOMPLETE;
}

void ClusterLibFrame::assemble(int bytes_waiting)
{//printf("%d\n",bytes_waiting);
	//check if everything is ok
	/*if(bytes_waiting == -1)
	{
		printf("ASSEMBLE ERROR: something went wrong with the socket\n");
		m_read_complete_frame = true;
		m_socket_error = true;
		return;
	}

	if(bytes_waiting == -2)
	{
		printf("ASSEMBLE ERROR: socket receive timeout\n");
		return;
	}*/

	//********************* beginning payload assembly
	if(m_read_header == false)
	{
		enum ClusterLibFrameAssembler::HEADER_STATUS header_status = readHeader(bytes_waiting);

		switch(header_status)
		{
			case ClusterLibFrameAssembler::HEADER_STATUS_INCOMPLETE:
				m_read_header = false;
				//printf("INCOMPLETE2 %u\n",m_incoming_length);
				return;
			case ClusterLibFrameAssembler::HEADER_STATUS_COMPLETE:
				m_read_header = true;//printf("header has been read %d\n",m_local_offset);
				//printf("COMPLETE2 %u\n",m_incoming_length);
				//prepare for next header
				m_remaining_header_bytes = ClusterLibFrameAssembler::HEADER_LENGTH;
				return;
			case ClusterLibFrameAssembler::HEADER_STATUS_OVERREAD:
				m_read_header = true;//printf("HEADER OVERREAD: incoming data size %u\n",m_incoming_length);
				//printf("OVERREAD2 %u\n",m_incoming_length);
				//prepare for the header of the next stream
				m_remaining_header_bytes = ClusterLibFrameAssembler::HEADER_LENGTH;
				return;
			case ClusterLibFrameAssembler::HEADER_STATUS_PERFECT:
				m_read_header = false;//printf("ALL OF THE EXPECTED DATA WAS READ\n");
				//printf("PERFECT2 %u\n",m_incoming_length);

				//reset the local offset
				m_local_offset = 0;

				//prepare for next header
				m_remaining_header_bytes = ClusterLibFrameAssembler::HEADER_LENGTH;
				//>hand< the local memory over the output buffer
				mp_received_payload_buffer = new Lazarus::Buffer(mp_local_buffer, m_incoming_length);
				//create a new buffer, as the old buffer is owned by the frame handler
				mp_local_buffer = new unsigned char[ClusterLibFrameAssembler::HEADER_LENGTH];
				//indicate a complete frame
				m_read_complete_frame = true;
				return;
			default:
				printf("ERROR2 %u\n",m_incoming_length);
				return;
		}
	}

	//printf("CLUSTERLIBFRAME ASSEMBLER: data length: %ld, read so far %d, after iteration %d\n",m_incoming_length,m_local_offset,m_local_offset+bytes_waiting);

	//************************** normal payload assembly

	//in case we are still not finished after this method call
	if(m_local_offset + bytes_waiting < m_incoming_length)
	{//printf("local receive buffer offset %d\n",m_local_offset);
		//printf("INCREMENT ASSEMBLY buffer size %d and %d waiting bytes\n",mp_connection_receive_buffer->get_m_length(),bytes_waiting);
		//printf("INCREMENT ASSEMBLY m_local_offset %d \n",m_local_offset);
		//copy the waiting bytes into the local buffer
		memcpy(mp_local_buffer+m_local_offset,mp_connection_receive_buffer->get_mp_data(),bytes_waiting*sizeof(unsigned char));

		//update offset
		m_local_offset += bytes_waiting;

		return;
	}

	//in case we are perfectly finished after this method call
	if(m_local_offset + bytes_waiting == m_incoming_length)
	{
		//copy the waiting bytes into the local buffer
		memcpy(mp_local_buffer+m_local_offset,mp_connection_receive_buffer->get_mp_data(),bytes_waiting*sizeof(unsigned char));
		//printf("PERFECT FINISH\n");
		//update offset
		m_local_offset = 0;

		//we are waiting for a new header
		m_read_header = false;

		//>hand< the local memory over the output buffer
		mp_received_payload_buffer = new Lazarus::Buffer(mp_local_buffer, m_incoming_length);

		//create new local buffer which can hold the next header
		mp_local_buffer = new unsigned char[ClusterLibFrameAssembler::HEADER_LENGTH];

		//indicate a complete frame
		m_read_complete_frame = true;

		return;
	}

}

}
