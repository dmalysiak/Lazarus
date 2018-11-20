/*
 * ConfigReaderMarkI.h
 *
 *  Created on: Feb 9, 2014
 *      Author: Darius Malysiak
 */

#ifndef CONFIGREADERMARKI_H_
#define CONFIGREADERMARKI_H_

#include "ConfigReader.h"
#include "XMLConfigMarkIParameters.h"
#include "../DataStructures/NCSList.h"

#include "XMLConfigMarkISection.h"

#include <unordered_map>

namespace Lazarus {


class ConfigReaderMarkI: public Lazarus::ConfigReader {
public:

	ConfigReaderMarkI(const std::string& config_file);
	virtual ~ConfigReaderMarkI();

	//will read out all sections including all contained parameters
	void readAllSections();

	//returns a section according to its name
	XMLConfigMarkISection* getSection(const std::string& name);

	Lazarus::FastNCSList<XMLConfigMarkISection*>* getSections();


private:
	Lazarus::FastNCSList<XMLConfigMarkISection*> m_sections;

	XMLConfigParameter* createParameter(XMLConfigMarkIParameters::VALUE_TYPE parameter_type, std::string& name, std::string& content);

	void removeEncasingBreaks(std::string& string);

	std::unordered_map<std::string, XMLConfigMarkISection*> m_section_map;

};

} /* namespace Lazarus */
#endif /* CONFIGREADERMARKI_H_ */
