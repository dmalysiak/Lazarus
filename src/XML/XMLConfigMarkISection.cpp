/*
 * XMLConfigMarkISection.cpp
 *
 *  Created on: Feb 11, 2014
 *      Author: clustro
 */

#include "XMLConfigMarkISection.h"

#include <stdexcept>

namespace Lazarus {

XMLConfigMarkISection::XMLConfigMarkISection(const std::string& name)
{
	m_name = name;
}

XMLConfigMarkISection::~XMLConfigMarkISection()
{
	for(auto it : m_parameter_map)
		delete it.second;
}

void XMLConfigMarkISection::addParameter(XMLConfigParameter* param)
{
	m_params.appendLast(param);

	//register the parameter in the lookup map
	m_parameter_map[param->getName()]=param;
}

XMLConfigParameter* XMLConfigMarkISection::getParameter(const std::string& name)
{
	XMLConfigParameter* r = NULL;

	try
	{
		r = m_parameter_map.at(name);
	}
	catch(std::out_of_range e)
	{
		printf("ERROR: Parameter '%s' has not been found in section map\n",name.c_str());
		throw;
	}

	return r;
}

Lazarus::FastNCSList<XMLConfigParameter*>* XMLConfigMarkISection::getParameters()
{
	return &m_params;
}

const std::string& XMLConfigMarkISection::getName()
{
	return m_name;
}

} /* namespace Lazarus */
