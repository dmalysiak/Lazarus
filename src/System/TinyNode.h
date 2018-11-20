/*
 * TinyNode.h
 *
 *  Created on: Aug 30, 2013
 *      Author: Darius Malysiak
 */

#ifndef TINYNODE_H_
#define TINYNODE_H_

#include "../Serializable.h"

namespace Lazarus {

class TinyNode: public Lazarus::Serializable {
public:
	TinyNode(std::string ip_address, unsigned int port, unsigned int node_id, unsigned int flags);
	TinyNode();
	virtual ~TinyNode();

	unsigned int getNodeID() const;
	unsigned int getFlags() const;

	virtual void serialize();
	virtual void deserialize();

	unsigned int get_m_node_id() const;
	unsigned int get_m_flags() const;
	unsigned int get_m_port() const;
	std::string get_m_ip_address() const;

	void set_m_node_id(unsigned int node_id);
	void set_m_flags(unsigned int flags);
	void set_m_port(unsigned int port);
	void set_m_host_order_ip_address(std::string address);

	static const unsigned int IPV4_FLAG = 1;

private:
	unsigned int m_node_id;
	unsigned int m_flags;
	unsigned int m_port;
	std::string m_ip_address;
};

} /* namespace Lazarus */
#endif /* TINYNODE_H_ */
