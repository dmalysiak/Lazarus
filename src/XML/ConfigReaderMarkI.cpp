/*
 * ConfigReaderMarkI.cpp
 *
 *  Created on: Feb 9, 2014
 *      Author: Darius Malysiak
 */

#include "ConfigReaderMarkI.h"
#include "../Toolbox.h"
#include "../DataStructures/FastNCSList.h"

#include <stdexcept>

namespace Lazarus {

ConfigReaderMarkI::ConfigReaderMarkI(const std::string& config_file) : ConfigReader(config_file) {
	// TODO Auto-generated constructor stub

}

ConfigReaderMarkI::~ConfigReaderMarkI() {

	for(auto it : m_section_map)
		delete it.second;

}

Lazarus::FastNCSList<XMLConfigMarkISection*>* ConfigReaderMarkI::getSections()
{
	return &m_sections;
}

void ConfigReaderMarkI::readAllSections()
{
	m_sections.clear();

	for(auto it : m_section_map)
		delete it.second;

	m_section_map.clear();

	//remove the constness
	std::string SECTION_TAG_ = XMLConfigMarkIParameters::SECTION_TAG;
	std::string PARAMETER_TAG_ = XMLConfigMarkIParameters::PARAMETER_TAG;
	std::string PARAMETER_STRING_TAG_ = XMLConfigMarkIParameters::PARAMETER_STRING_TAG;
	std::string SECTION_NAME_ATTRIBUTE_ = XMLConfigMarkIParameters::SECTION_NAME_ATTRIBUTE;
	std::string PARAMETER_NAME_ATTRIBUTE_ = XMLConfigMarkIParameters::PARAMETER_NAME_ATTRIBUTE;
	std::string PARAMETER_TYPE_ATTRIBUTE_ = XMLConfigMarkIParameters::PARAMETER_TYPE_ATTRIBUTE;

	//iterate over the section until the last one has been found
	int section_result = 0;
	int parameter_result = 0;
	int string_array_result = 0;
	unsigned int section_node_counter = 0;
	unsigned int parameter_node_counter = 0;
	unsigned int string_node_counter = 0;
	xmlNodePtr section_node = NULL;
	xmlNodePtr parameter_node = NULL;
	xmlNodePtr string_node = NULL;
	XMLConfigMarkISection* section = NULL;
	XMLConfigParameter* parameter = NULL;
	std::string section_name;
	std::string parameter_name;
	std::string parameter_type;
	std::string parameter_content;
	Lazarus::FastNCSList<std::string>* string_array = NULL;
	enum Lazarus::XMLConfigMarkIParameters::VALUE_TYPE type = Lazarus::XMLConfigMarkIParameters::VALUE_TYPE_UNKNOWN;

	std::vector<std::string> path;
	std::vector<std::string> empty_path;
	path.push_back(XMLConfigMarkIParameters::ROOT_TAG);

	while(section_result == 0)
	{
		section_result = findNode(path, m_root_element, SECTION_TAG_,section_node, section_node_counter);

		//if a section has been found, iterate over all child nodes in the same manor
		if(section_result == 0)
		{
			section_node_counter++;

			//get the section name and instantiate a section
			getNodeAttribute(section_node,SECTION_NAME_ATTRIBUTE_,section_name);
			section = new XMLConfigMarkISection(section_name);

			//if a parameter has been found, extract the information and add it to the current section
			while(parameter_result == 0)
			{
				//look for parameters in the node
				parameter_result = findNode(empty_path, section_node, PARAMETER_TAG_,parameter_node, parameter_node_counter);

				//if a parameter has been found
				if(parameter_result == 0)
				{

					parameter_node_counter++;

					//get the parameter name, type and content, instantiate the parameter object
					getNodeAttribute(parameter_node,PARAMETER_NAME_ATTRIBUTE_,parameter_name);
					getNodeAttribute(parameter_node,PARAMETER_TYPE_ATTRIBUTE_,parameter_type);

					//lookup the type
					type = XMLConfigMarkIParameters::translateTagToValueType(parameter_type);

					//in case of a string array we have to iterate over one more layer in the xml tree
					if(type == Lazarus::XMLConfigMarkIParameters::VALUE_TYPE_STRING_ARRAY)
					{
						string_array = new Lazarus::FastNCSList<std::string>();

						while(string_array_result == 0)
						{
							//look for the next string element
							string_array_result = findNode(empty_path, parameter_node, PARAMETER_STRING_TAG_,string_node, string_node_counter);

							//if a string array element was found
							if(string_array_result == 0)
							{
								string_node_counter++;

								getTextContent(string_node, parameter_content);
								removeEncasingBreaks(parameter_content);//delete breaks
								string_array->appendLast(parameter_content);
							}

						}

						//finally save the extracted strings as a section parameter
						parameter = new XMLConfigMarkIStringArray(parameter_name,string_array);
					}
					else //for everything else than a string array
					{
						getTextContent(parameter_node, parameter_content);
						parameter = createParameter( type ,parameter_name,parameter_content );
					}
					//add the parameter to the current section
					section->addParameter(parameter);
				}

				//clear the string array counter
				string_node_counter = 0;
				string_array_result = 0;
			}

			//add the section to the list
			m_sections.appendLast(section);

			//and register it in the lookup map
			m_section_map[section_name] = section;

		}

		parameter_result =0;
		parameter_node_counter = 0;

	}
}

void ConfigReaderMarkI::removeEncasingBreaks(std::string& string)
{
	if(string.at(0) == '\n')
	{
		string.erase(0,1);
	}

	if(string.at(string.length()-1) == '\n')
	{
		string.erase(string.length()-1,1);
	}
}

XMLConfigParameter* ConfigReaderMarkI::createParameter(XMLConfigMarkIParameters::VALUE_TYPE parameter_type, std::string& name, std::string& content)
{
	XMLConfigParameter* r = NULL;
	Lazarus::FastNCSList<std::string>* split_container = NULL;
	unsigned int counter = 0;

	switch(parameter_type)
	{
	//scalars
	case XMLConfigMarkIParameters::VALUE_TYPE_DOUBLE:
		removeEncasingBreaks(content);//delete breaks
		Lazarus::Toolbox::deleteChar(content,' ');//delete spaces
		r = new XMLConfigMarkIDouble(name,Lazarus::Toolbox::stringToDouble(content));
		break;

	case XMLConfigMarkIParameters::VALUE_TYPE_FLOAT:
		removeEncasingBreaks(content);//delete breaks
		Lazarus::Toolbox::deleteChar(content,' ');//delete spaces
		r = new XMLConfigMarkIFloat(name,Lazarus::Toolbox::stringToFloat(content));
		break;

	case XMLConfigMarkIParameters::VALUE_TYPE_INT:
		removeEncasingBreaks(content);//delete breaks
		Lazarus::Toolbox::deleteChar(content,' ');//delete spaces
		r = new XMLConfigMarkIInt(name,Lazarus::Toolbox::stringToInt(content));
		break;

	case XMLConfigMarkIParameters::VALUE_TYPE_LONG:
		removeEncasingBreaks(content);//delete breaks
		Lazarus::Toolbox::deleteChar(content,' ');//delete spaces
		r = new XMLConfigMarkILong(name,Lazarus::Toolbox::stringToLong(content));
		break;

	case XMLConfigMarkIParameters::VALUE_TYPE_LONGLONG:
		removeEncasingBreaks(content);//delete breaks
		Lazarus::Toolbox::deleteChar(content,' ');//delete spaces
		r = new XMLConfigMarkILongLong(name,Lazarus::Toolbox::stringToLongLong(content));
		break;

	case XMLConfigMarkIParameters::VALUE_TYPE_UINT:
		removeEncasingBreaks(content);//delete breaks
		Lazarus::Toolbox::deleteChar(content,' ');//delete spaces
		r = new XMLConfigMarkIUInt(name,Lazarus::Toolbox::stringToUInt(content));
		break;

	case XMLConfigMarkIParameters::VALUE_TYPE_ULONG:
		removeEncasingBreaks(content);//delete breaks
		Lazarus::Toolbox::deleteChar(content,' ');//delete spaces
		r = new XMLConfigMarkIULong(name,Lazarus::Toolbox::stringToULong(content));
		break;

	case XMLConfigMarkIParameters::VALUE_TYPE_ULONGLONG:
		removeEncasingBreaks(content);//delete breaks
		Lazarus::Toolbox::deleteChar(content,' ');//delete spaces
		r = new XMLConfigMarkIULongLong(name,Lazarus::Toolbox::stringToULongLong(content));
		break;

	case XMLConfigMarkIParameters::VALUE_TYPE_STRING:
		removeEncasingBreaks(content);//delete breaks
		//Lazarus::Toolbox::deleteChar(content,' ');//delete spaces
		r = new XMLConfigMarkIString(name,content);
		break;

	//arrays
	case XMLConfigMarkIParameters::VALUE_TYPE_DOUBLE_ARRAY:
	{
		//split the string
		removeEncasingBreaks(content);//delete breaks
		Lazarus::Toolbox::deleteChar(content,' ');//delete spaces
		split_container = Lazarus::Toolbox::splitStringF(content,XMLConfigMarkIParameters::ARRAY_DELIMITER);
		double* array = new double[ split_container->getSize() ];

		for(Lazarus::FastNCSList<std::string>::Iterator it = split_container->getStart(); it != split_container->getEnd(); ++it)
		{
			array[counter] = Lazarus::Toolbox::stringToDouble(it.getElement()->getm_data());
			counter++;
		}

		r = new XMLConfigMarkIDoubleArray(name,array,split_container->getSize());
		break;
	}

	case XMLConfigMarkIParameters::VALUE_TYPE_FLOAT_ARRAY:
	{
		//split the string
		removeEncasingBreaks(content);//delete breaks
		Lazarus::Toolbox::deleteChar(content,' ');//delete spaces
		split_container = Lazarus::Toolbox::splitStringF(content,XMLConfigMarkIParameters::ARRAY_DELIMITER);
		float* array = new float[ split_container->getSize() ];

		for(Lazarus::FastNCSList<std::string>::Iterator it = split_container->getStart(); it != split_container->getEnd(); ++it)
		{
			array[counter] = Lazarus::Toolbox::stringToFloat(it.getElement()->getm_data());
			counter++;
		}

		r = new XMLConfigMarkIFloatArray(name,array,split_container->getSize());
		break;
	}

	case XMLConfigMarkIParameters::VALUE_TYPE_INT_ARRAY:
	{
		//split the string
		removeEncasingBreaks(content);//delete breaks
		Lazarus::Toolbox::deleteChar(content,' ');//delete spaces
		split_container = Lazarus::Toolbox::splitStringF(content,XMLConfigMarkIParameters::ARRAY_DELIMITER);
		int* array = new int[ split_container->getSize() ];

		for(Lazarus::FastNCSList<std::string>::Iterator it = split_container->getStart(); it != split_container->getEnd(); ++it)
		{
			array[counter] = Lazarus::Toolbox::stringToInt(it.getElement()->getm_data());
			counter++;
		}

		r = new XMLConfigMarkIIntArray(name,array,split_container->getSize());
		break;
	}

	case XMLConfigMarkIParameters::VALUE_TYPE_LONG_ARRAY:
	{
		//split the string
		removeEncasingBreaks(content);//delete breaks
		Lazarus::Toolbox::deleteChar(content,' ');//delete spaces
		split_container = Lazarus::Toolbox::splitStringF(content,XMLConfigMarkIParameters::ARRAY_DELIMITER);
		long* array = new long[ split_container->getSize() ];

		for(Lazarus::FastNCSList<std::string>::Iterator it = split_container->getStart(); it != split_container->getEnd(); ++it)
		{
			//printf("LONG ARRAY %d\n",Lazarus::Toolbox::stringToLong(it.getElement()->getm_data()));
			array[counter] = Lazarus::Toolbox::stringToLong(it.getElement()->getm_data());
			counter++;
		}

		r = new XMLConfigMarkILongArray(name,array,split_container->getSize());
		break;
	}

	case XMLConfigMarkIParameters::VALUE_TYPE_LONGLONG_ARRAY:
	{
		//split the string
		removeEncasingBreaks(content);//delete breaks
		Lazarus::Toolbox::deleteChar(content,' ');//delete spaces
		split_container = Lazarus::Toolbox::splitStringF(content,XMLConfigMarkIParameters::ARRAY_DELIMITER);
		long long* array = new long long[ split_container->getSize() ];

		for(Lazarus::FastNCSList<std::string>::Iterator it = split_container->getStart(); it != split_container->getEnd(); ++it)
		{
			//printf("LONG ARRAY %d\n",Lazarus::Toolbox::stringToLong(it.getElement()->getm_data()));
			array[counter] = Lazarus::Toolbox::stringToLongLong(it.getElement()->getm_data());
			counter++;
		}

		r = new XMLConfigMarkILongLongArray(name,array,split_container->getSize());
		break;
	}

	case XMLConfigMarkIParameters::VALUE_TYPE_UINT_ARRAY:
	{
		//split the string
		removeEncasingBreaks(content);//delete breaks
		Lazarus::Toolbox::deleteChar(content,' ');//delete spaces
		split_container = Lazarus::Toolbox::splitStringF(content,XMLConfigMarkIParameters::ARRAY_DELIMITER);
		unsigned int* array = new unsigned int[ split_container->getSize() ];

		for(Lazarus::FastNCSList<std::string>::Iterator it = split_container->getStart(); it != split_container->getEnd(); ++it)
		{
			array[counter] = Lazarus::Toolbox::stringToUInt(it.getElement()->getm_data());
			counter++;
		}

		r = new XMLConfigMarkIUIntArray(name,array,split_container->getSize());
		break;
	}

	case XMLConfigMarkIParameters::VALUE_TYPE_ULONG_ARRAY:
	{
		//split the string
		removeEncasingBreaks(content);//delete breaks
		Lazarus::Toolbox::deleteChar(content,' ');//delete spaces
		split_container = Lazarus::Toolbox::splitStringF(content,XMLConfigMarkIParameters::ARRAY_DELIMITER);
		unsigned long* array = new unsigned long[ split_container->getSize() ];

		for(Lazarus::FastNCSList<std::string>::Iterator it = split_container->getStart(); it != split_container->getEnd(); ++it)
		{
			array[counter] = Lazarus::Toolbox::stringToULong(it.getElement()->getm_data());
			counter++;
		}

		r = new XMLConfigMarkIULongArray(name,array,split_container->getSize());

		break;
	}

	case XMLConfigMarkIParameters::VALUE_TYPE_ULONGLONG_ARRAY:
	{
		//split the string
		removeEncasingBreaks(content);//delete breaks
		Lazarus::Toolbox::deleteChar(content,' ');//delete spaces
		split_container = Lazarus::Toolbox::splitStringF(content,XMLConfigMarkIParameters::ARRAY_DELIMITER);
		unsigned long long* array = new unsigned long long[ split_container->getSize() ];

		for(Lazarus::FastNCSList<std::string>::Iterator it = split_container->getStart(); it != split_container->getEnd(); ++it)
		{
			array[counter] = Lazarus::Toolbox::stringToULongLong(it.getElement()->getm_data());
			counter++;
		}

		r = new XMLConfigMarkIULongLongArray(name,array,split_container->getSize());

		break;
	}

	case XMLConfigMarkIParameters::VALUE_TYPE_STRING_ARRAY:
		printf("this should never happen!!\n");
		break;

	default:
		break;

	}

	DELETE_NULL_CHECKING(split_container);

	return r;
}

XMLConfigMarkISection* ConfigReaderMarkI::getSection(const std::string& name)
{
	XMLConfigMarkISection* r = NULL;

	try
	{
		r = m_section_map.at(name);
	}
	catch(std::out_of_range& e)
	{
		printf("ERROR: Section '%s' has not been found in section map\n",name.c_str());
		throw;
	}

	return r;

}

} /* namespace Lazarus */
