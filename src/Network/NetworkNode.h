/*
 * ClusterNode.h
 *
 *  Created on: 22.03.2013
 *      Author: Darius Malysiak
 */

#ifndef NETWORKNODE_H_
#define NETWORKNODE_H_

#include "../Serializable.h"

#include <string.h>
#include <string>

namespace Lazarus
{

class NetworkNode: public Lazarus::Serializable {
public:
	NetworkNode();
	virtual ~NetworkNode();

	std::string m_node_ip;
	unsigned int m_node_id;

};

}

#endif /* NETWORKNODE_H_ */
