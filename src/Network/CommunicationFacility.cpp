/*
 * CommunicationFacility.cpp
 *
 *  Created on: 17.06.2013
 *      Author: darius
 */

#include "CommunicationFacility.h"

namespace Lazarus {

CommunicationFacility::CommunicationFacility() {


}

CommunicationFacility::~CommunicationFacility() {
	// TODO Auto-generated destructor stub
}

void CommunicationFacility::removeNode(unsigned int node_id)
{
	printf("ERROR: CommunicationFacility::removeNode not implemented\n");
}

int CommunicationFacility::sendRequestToNode(unsigned int node_id, Lazarus::Buffer* request, SynchronizationCallback* callback)
{
	printf("ERROR: CommunicationFacility::sendRequestToNode not implemented\n");
	return 0;
}

int CommunicationFacility::sendRequestToMultipleNodes(Lazarus::FastKTuple<unsigned int>& node_ids, Lazarus::Buffer* request, SynchronizationCallback* callback)
{
	printf("ERROR: CommunicationFacility::sendRequestToMultipleNodes not implemented\n");
	return 0;
}

int CommunicationFacility::sendRequestToNodeNL(unsigned int node_id, Lazarus::Buffer* request, SynchronizationCallback* callback)
{
	printf("ERROR: CommunicationFacility::sendRequestToNodeNL not implemented\n");
	return 0;
}

} /* namespace Lazarus */
