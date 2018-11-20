/*
 * XMLConfigParameter.cpp
 *
 *  Created on: Feb 10, 2014
 *      Author: Darius Malysiak
 */

#include "XMLConfigParameter.h"

namespace Lazarus {

XMLConfigParameter::XMLConfigParameter(int type, std::string name) {

	m_type = type;
	m_name = name;

}

XMLConfigParameter::XMLConfigParameter() {

	m_type = 0;
	m_name = "";

}

XMLConfigParameter::~XMLConfigParameter() {
	// TODO Auto-generated destructor stub
}

const std::string& XMLConfigParameter::getName() const
{
	return m_name;
}

int XMLConfigParameter::getType()
{
	return m_type;
}

} /* namespace Lazarus */
