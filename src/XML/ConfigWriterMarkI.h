/*
 * ConfigWriterMarkI.h
 *
 *  Created on: Feb 11, 2014
 *      Author: clustro
 */

#ifndef CONFIGWRITERMARKI_H_
#define CONFIGWRITERMARKI_H_

#include "SimpleXML.h"
#include "XMLConfigMarkIParameters.h"
#include "../DataStructures/FastNCSList.h"

#include "XMLConfigMarkISection.h"

//#include <unordered_map>
#include <libxml/xmlwriter.h>

namespace Lazarus {

class ConfigWriterMarkI: public Lazarus::SimpleXML {
public:
	ConfigWriterMarkI();
	virtual ~ConfigWriterMarkI();

	//this method takes ownership of the section instance
	void addSection(XMLConfigMarkISection* section);

	/**
	 * Saves the config content as an XML document.
	 * This will erase all added sections!
	 * */
	void writeConfig(const std::string& config_file);

	/**
	 * Saves the config content as an XML document in the given string.
	 * This will erase all added sections!
	 * */
	void writeConfigToString(std::string& s);

private:
	Lazarus::FastNCSList<XMLConfigMarkISection*> m_sections;

	void writeParameters(XMLConfigMarkISection* section);

	void writeSpace();

	xmlTextWriterPtr m_writer;

	xmlBufferPtr m_buf;

	void reset();

};

} /* namespace Lazarus */
#endif /* CONFIGWRITERMARKI_H_ */
