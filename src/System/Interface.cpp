/*
 * Interface.cpp
 *
 *  Created on: Sep 1, 2013
 *      Author: Darius Malysiak
 */

#include "Interface.h"

namespace Lazarus {

Interface::Interface() {
	// TODO Auto-generated constructor stub

}

Interface::~Interface() {
	// TODO Auto-generated destructor stub
}

void Interface::set_dev_filename(std::string dev_filename)
{
	m_dev_filename = dev_filename;
}

void Interface::set_interface_name(std::string interface_name)
{
	m_interface_name = interface_name;
}

const std::string Interface::get_dev_filename() const
{
	return m_dev_filename;
}

const std::string Interface::get_interface_name() const
{
	return m_interface_name;
}

} /* namespace Lazarus */
