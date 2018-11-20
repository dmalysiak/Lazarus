/*
 * IPv4Header.h
 *
 *  Created on: Feb 25, 2014
 *      Author: Darius Malysiak
 */

#ifndef IPV4HEADER_H_
#define IPV4HEADER_H_

#include "../BaseObject.h"

#include <netinet/ip.h>

namespace Lazarus {

class IPv4Header: public Lazarus::BaseObject {
public:
	IPv4Header();
	virtual ~IPv4Header();

	unsigned short int m_version;
	unsigned short int m_header_length;
	unsigned short int m_type_of_service;
	unsigned int m_total_length;
	unsigned int m_identification;
	unsigned char m_flags;
	unsigned int m_fragment_offset;
	unsigned char m_ttl;
	unsigned char m_protocol;
	unsigned short int m_checksum;
	unsigned int m_source_address;
	unsigned int m_destination_address;

};

} /* namespace Lazarus */

#endif /* IPV4HEADER_H_ */
