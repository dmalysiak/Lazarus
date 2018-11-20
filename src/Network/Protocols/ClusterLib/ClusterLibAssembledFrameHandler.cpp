/*
 * ClusterLibAssembledFrameHandler.cpp
 *
 *  Created on: 07.03.2013
 *      Author: Darius Malysiak
 */

#include "ClusterLibAssembledFrameHandler.h"
#include "ClusterLibFrame.h"
#include "../../../Toolbox.h"

#include <stdio.h>

namespace Lazarus
{

ClusterLibAssembledFrameHandler::ClusterLibAssembledFrameHandler(GenericConnection*  connection,CommunicationFacility* facility)
	: AssembledFrameHandler(connection,facility) {

	//create a frame for sending packages
	mp_frame = new ClusterLibFrame(connection);

}

ClusterLibAssembledFrameHandler::~ClusterLibAssembledFrameHandler() {
	// TODO Auto-generated destructor stub
}

int ClusterLibAssembledFrameHandler::handlePayload(Lazarus::Buffer* payload)
{
	//debug
	printf("received a payload of length %u\n",payload->get_m_length());

	for(unsigned int i=0;i < payload->get_m_length();i++)
	{
		printf("%c\n",payload->get_mp_data()[i]);
	}

	//free memory
	delete payload;

	//respond
	Lazarus::Buffer send_content(5);

    send_content.get_mp_data()[0] = 'a';
    send_content.get_mp_data()[1] = 'b';
    send_content.get_mp_data()[2] = 'c';
    send_content.get_mp_data()[3] = 'Z';
    send_content.get_mp_data()[4] = 'Y';

    return mp_frame->sendFrame(&send_content);
}

}
