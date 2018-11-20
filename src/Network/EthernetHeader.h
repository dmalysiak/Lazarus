/*
 * EthernetHeader.h
 *
 *  Created on: Feb 25, 2014
 *      Author: Darius Malysiak
 */

#ifndef ETHERNETHEADER_H_
#define ETHERNETHEADER_H_

#include "../BaseObject.h"

namespace Lazarus {

class EthernetHeader: public Lazarus::BaseObject {
public:
	EthernetHeader();
	virtual ~EthernetHeader();

	void setIP_PROT();
	void setARP_PROT();
	void setRARP_PROT();

	unsigned char m_destination[6];
	unsigned char m_source[6];
	unsigned short m_protocol;
};

} /* namespace Lazarus */

#endif /* ETHERNETHEADER_H_ */
