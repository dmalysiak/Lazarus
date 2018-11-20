/*
 * ConfigReader.h
 *
 *  Created on: Feb 9, 2014
 *      Author: Darius Malysiak
 */

#ifndef CONFIGREADER_H_
#define CONFIGREADER_H_

#include "SimpleXML.h"
#include <string>

namespace Lazarus
{

class ConfigReader: public SimpleXML {
public:
	ConfigReader(const std::string& config_file);
	virtual ~ConfigReader();

	/**
	 * Returns -1 in case of a failed file opening/parsing, 0 otherwise
	 * */
	int getStatus();

private:
	//flag variable for the file status
	int m_file_read;

};

}
#endif /* CONFIGREADER_H_ */
