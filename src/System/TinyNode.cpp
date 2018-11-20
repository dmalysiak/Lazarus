/*
 * TinyNode.cpp
 *
 *  Created on: Aug 30, 2013
 *      Author: Darius Malysiak
 */

#include "TinyNode.h"

#include <string.h>

namespace Lazarus {

TinyNode::TinyNode(std::string ip_address, unsigned int port, unsigned int node_id, unsigned int flags) {

	m_node_id = node_id;
	m_flags = flags;
	m_ip_address = ip_address;
	m_port = port;
}

TinyNode::TinyNode() {

	m_node_id = 0;
	m_flags = 0;
	m_port = 0;

}

TinyNode::~TinyNode() {
	// TODO Auto-generated destructor stub
}

unsigned int TinyNode::getNodeID() const
{
	return m_node_id;
}

unsigned int TinyNode::getFlags() const
{
	return m_flags;
}

void TinyNode::serialize()
{
	Serializable::resetBuffer();

	registerElement<unsigned int>(3);
	registerString(m_ip_address);

	allocateBuffer();

	addUInt(m_node_id);
	addUInt(m_flags);
	addUInt(m_port);
	addString(m_ip_address);
}

void TinyNode::deserialize()
{
	m_ip_address = getString();
	m_port = getUInt();
	m_flags = getUInt();
	m_node_id = getUInt();

	resetBuffer();
}

unsigned int TinyNode::get_m_node_id() const
{
	return m_node_id;
}

unsigned int TinyNode::get_m_flags() const
{
	return m_flags;
}

std::string TinyNode::get_m_ip_address() const
{
	return m_ip_address;
}


void TinyNode::set_m_node_id(unsigned int node_id)
{
	m_node_id = node_id;
}

void TinyNode::set_m_flags(unsigned int flags)
{
	m_flags = flags;
}

void TinyNode::set_m_host_order_ip_address(std::string address)
{
	m_ip_address = address;
}


} /* namespace Lazarus */
