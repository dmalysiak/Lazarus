/*
 * ConfigReader.cpp
 *
 *  Created on: Feb 9, 2014
 *      Author: Darius Malysiak
 */

#include "ConfigReader.h"

namespace Lazarus
{

ConfigReader::ConfigReader(const std::string& config_file) {

	int res = parseFile(config_file);

	m_file_read = res;
}

ConfigReader::~ConfigReader() {
	// TODO Auto-generated destructor stub
}

int ConfigReader::getStatus()
{
	return m_file_read;
}

}
