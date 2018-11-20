/*
 * ConfigWriterMarkI.cpp
 *
 *  Created on: Feb 11, 2014
 *      Author: clustro
 */

#include "ConfigWriterMarkI.h"
#include "../Toolbox.h"

#include <libxml/encoding.h>

namespace Lazarus {

ConfigWriterMarkI::ConfigWriterMarkI() {

	/* Create a new XML buffer, to which the XML document will be
	 * written */
	m_buf = xmlBufferCreate();
	if(m_buf == NULL)
	{
		printf("testXmlwriterMemory: Error creating the xml buffer\n");
		return;
	}

	/* Create a new XmlWriter for memory, with no compression.
	 * Remark: there is no compression for this kind of xmlTextWriter */
	m_writer = xmlNewTextWriterMemory(m_buf, 0);
	if(m_writer == NULL)
	{
		printf("testXmlwriterMemory: Error creating the xml writer\n");
		return;
	}


}

ConfigWriterMarkI::~ConfigWriterMarkI()
{

	if(m_buf != NULL)
		xmlBufferFree(m_buf);

    if(m_writer != NULL)
    	xmlFreeTextWriter(m_writer);

	//delete the sections
    m_sections.destroy();
}

void ConfigWriterMarkI::reset()
{
	/**
	 * Seriously??? Why must the writer be freed BEFORE the buffer and WHY IS IT NOWHERE DOCUMENTED???
	 * */
	xmlFreeTextWriter(m_writer);
	xmlBufferFree(m_buf);

	m_buf = NULL;
	m_writer = NULL;

	m_buf = xmlBufferCreate();
	if(m_buf == NULL)
	{
		printf("testXmlwriterMemory: Error creating the xml buffer\n");
		return;
	}

	m_writer = xmlNewTextWriterMemory(m_buf, 0);
	if(m_writer == NULL)
	{
		printf("testXmlwriterMemory: Error creating the xml writer\n");
		return;
	}

	//delete the sections
	m_sections.destroy();
}

void ConfigWriterMarkI::addSection(XMLConfigMarkISection* section)
{
	m_sections.appendLast(section);
}

void ConfigWriterMarkI::writeSpace()
{
	char void_[2];
	void_[0]='\n';
	void_[1]='\0';

	int rc = xmlTextWriterWriteString(m_writer, BAD_CAST void_);
	if(rc < 0)
	{
		printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
		return;
	}
}

void ConfigWriterMarkI::writeConfig(const std::string& config_file)
{

//	    int rc;
//	    xmlTextWriterPtr writer;
//	    xmlBufferPtr buf;
//	    xmlChar *tmp;
//	    FILE *fp;

//	    /* Create a new XML buffer, to which the XML document will be
//	     * written */
//	    buf = xmlBufferCreate();
//	    if (buf == NULL) {
//	        printf("testXmlwriterMemory: Error creating the xml buffer\n");
//	        return;
//	    }
//
//	    /* Create a new XmlWriter for memory, with no compression.
//	     * Remark: there is no compression for this kind of xmlTextWriter */
//	    writer = xmlNewTextWriterMemory(buf, 0);
//	    if (writer == NULL) {
//	        printf("testXmlwriterMemory: Error creating the xml writer\n");
//	        return;
//	    }

//	    /* Start the document with the xml default for the version,
//	     * encoding ISO 8859-1 and the default for the standalone
//	     * declaration. */
//	    rc = xmlTextWriterStartDocument(m_writer, NULL, XML_ENCODING, NULL);
//	    if (rc < 0) {
//	        printf
//	            ("testXmlwriterMemory: Error at xmlTextWriterStartDocument\n");
//	        return;
//	    }
//
//	    /* Start an element named "EXAMPLE". Since thist is the first
//	     * element, this will be the root element of the document. */
//	    rc = xmlTextWriterStartElement(m_writer, BAD_CAST "EXAMPLE");
//	    if (rc < 0) {
//	        printf
//	            ("testXmlwriterMemory: Error at xmlTextWriterStartElement\n");
//	        return;
//	    }
//
//
//	    /* Here we could close the elements ORDER and EXAMPLE using the
//	     * function xmlTextWriterEndElement, but since we do not want to
//	     * write any other elements, we simply call xmlTextWriterEndDocument,
//	     * which will do all the work. */
//	    rc = xmlTextWriterEndDocument(m_writer);
//	    if (rc < 0) {
//	        printf("testXmlwriterMemory: Error at xmlTextWriterEndDocument\n");
//	        return;
//	    }
//
//	    xmlFreeTextWriter(m_writer);
//
//	    fp = fopen(config_file.c_str(), "w");
//	    if (fp == NULL) {
//	        printf("testXmlwriterMemory: Error at fopen\n");
//	        return;
//	    }
//
//	    fprintf(fp, "%s", (const char *) m_buf->content);
//
//	    fclose(fp);
//
//	    xmlBufferFree(m_buf);




	//------------------------- create the new xml file and write a basic config structure into it
	int rc = 0;
	FILE *fp = NULL;

	/* Start the document with the xml default for the version,
	 * encoding ISO 8859-1 and the default for the standalone
	 * declaration. */
	rc = xmlTextWriterStartDocument(m_writer, NULL, XML_ENCODING, NULL);
	if(rc < 0)
	{
		printf("testXmlwriterMemory: Error at xmlTextWriterStartDocument\n");
		return;
	}

	/* start with the root element */
	rc = xmlTextWriterStartElement(m_writer, BAD_CAST XMLConfigMarkIParameters::ROOT_TAG.c_str());
	if(rc < 0)
	{
		printf("testXmlwriterFilename: Error at xmlTextWriterStartElement\n");
		return;
	}

	writeSpace();

	//iterate over all sections
	XMLConfigMarkISection* section = NULL;
	for(Lazarus::FastNCSList<XMLConfigMarkISection*>::Iterator it = m_sections.getStart(); it != m_sections.getEnd();++it)
	{
		section = it.getElement()->getm_data();

		//start the section
		rc = xmlTextWriterStartElement(m_writer, BAD_CAST XMLConfigMarkIParameters::SECTION_TAG.c_str());
		if(rc < 0)
		{
			printf("testXmlwriterFilename: Error at xmlTextWriterStartElement\n");
			return;
		}

		//add the name attribute to the current section
		rc = xmlTextWriterWriteAttribute(m_writer, BAD_CAST XMLConfigMarkIParameters::SECTION_NAME_ATTRIBUTE.c_str(),
				BAD_CAST section->getName().c_str());
		if(rc < 0)
		{
			printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
			return;
		}

		//break after section tag
		writeSpace();

		//write all parameters to the current section
		writeParameters(section);

		//close the section
		rc = xmlTextWriterEndElement(m_writer);
		if(rc < 0)
		{
			printf("testXmlwriterMemory: Error at xmlTextWriterEndElement\n");
			return;
		}

		//break after section
		writeSpace();
		writeSpace();
	}


	/*close the root element*/
	rc = xmlTextWriterEndElement(m_writer);
	if(rc < 0)
	{
		printf("testXmlwriterMemory: Error at xmlTextWriterEndElement\n");
		return;
	}

    rc = xmlTextWriterEndDocument(m_writer);
    if (rc < 0) {
        printf("testXmlwriterMemory: Error at xmlTextWriterEndDocument\n");
        return;
    }

    //write the file
    fp = fopen(config_file.c_str(), "w");
    if(fp == NULL)
    {
        printf("testXmlwriterMemory: Error at fopen\n");
        return;
    }

    fprintf(fp, "%s", (const char *) m_buf->content);

    fclose(fp);

    reset();
}


void ConfigWriterMarkI::writeConfigToString(std::string& s)
{
	//------------------------- create the new xml file and write a basic config structure into it
	int rc = 0;

	/* Start the document with the xml default for the version,
	 * encoding ISO 8859-1 and the default for the standalone
	 * declaration. */
	rc = xmlTextWriterStartDocument(m_writer, NULL, XML_ENCODING, NULL);
	if(rc < 0)
	{
		printf("testXmlwriterMemory: Error at xmlTextWriterStartDocument\n");
		return;
	}

	/* start with the root element */
	rc = xmlTextWriterStartElement(m_writer, BAD_CAST XMLConfigMarkIParameters::ROOT_TAG.c_str());
	if(rc < 0)
	{
		printf("testXmlwriterFilename: Error at xmlTextWriterStartElement\n");
		return;
	}

	writeSpace();

	//iterate over all sections
	XMLConfigMarkISection* section = NULL;
	for(Lazarus::FastNCSList<XMLConfigMarkISection*>::Iterator it = m_sections.getStart(); it != m_sections.getEnd();++it)
	{
		section = it.getElement()->getm_data();

		//start the section
		rc = xmlTextWriterStartElement(m_writer, BAD_CAST XMLConfigMarkIParameters::SECTION_TAG.c_str());
		if(rc < 0)
		{
			printf("testXmlwriterFilename: Error at xmlTextWriterStartElement\n");
			return;
		}

		//add the name attribute to the current section
		rc = xmlTextWriterWriteAttribute(m_writer, BAD_CAST XMLConfigMarkIParameters::SECTION_NAME_ATTRIBUTE.c_str(),
				BAD_CAST section->getName().c_str());
		if(rc < 0)
		{
			printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
			return;
		}

		//break after section tag
		writeSpace();

		//write all parameters to the current section
		writeParameters(section);

		//close the section
		rc = xmlTextWriterEndElement(m_writer);
		if(rc < 0)
		{
			printf("testXmlwriterMemory: Error at xmlTextWriterEndElement\n");
			return;
		}

		//break after section
		writeSpace();
		writeSpace();
	}


	/*close the root element*/
	rc = xmlTextWriterEndElement(m_writer);
	if(rc < 0)
	{
		printf("testXmlwriterMemory: Error at xmlTextWriterEndElement\n");
		return;
	}

	rc = xmlTextWriterEndDocument(m_writer);
	if (rc < 0) {
		printf("testXmlwriterMemory: Error at xmlTextWriterEndDocument\n");
		return;
	}

    s = std::string((const char *) m_buf->content);

    //write the file
	reset();
}

void ConfigWriterMarkI::writeParameters(XMLConfigMarkISection* section)
{
	XMLConfigParameter* parameter = NULL;
	int rc = 0;

	//iterate over the parameters
	for(Lazarus::FastNCSList<XMLConfigParameter*>::Iterator it = section->getParameters()->getStart(); it != section->getParameters()->getEnd();++it)
	{
		parameter = it.getElement()->getm_data();

		//start the parameter
		rc = xmlTextWriterStartElement(m_writer, BAD_CAST XMLConfigMarkIParameters::PARAMETER_TAG.c_str());
		if(rc < 0)
		{
			printf("testXmlwriterFilename: Error at xmlTextWriterStartElement\n");
			return;
		}

		//add the name attribute to the current parameter
		rc = xmlTextWriterWriteAttribute(m_writer, BAD_CAST XMLConfigMarkIParameters::PARAMETER_NAME_ATTRIBUTE.c_str(),
				BAD_CAST parameter->getName().c_str());
		if(rc < 0)
		{
			printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
			return;
		}


		switch(parameter->getType())
		{
			case XMLConfigMarkIParameters::VALUE_TYPE_STRING:
			{
				//get the parameter
				XMLConfigMarkIString* parameter_ = (XMLConfigMarkIString*)it.getElement()->getm_data();

				//add the type attribute to the current parameter
				rc = xmlTextWriterWriteAttribute(m_writer, BAD_CAST XMLConfigMarkIParameters::PARAMETER_TYPE_ATTRIBUTE.c_str(),
						BAD_CAST XMLConfigMarkIParameters::XML_TAG_STRING.c_str());
				if(rc < 0)
				{
					printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
					return;
				}

				writeSpace();

				//add the actual content
				rc = xmlTextWriterWriteString(m_writer, BAD_CAST parameter_->getParam().c_str());
				if(rc < 0)
				{
					printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
					return;
				}

				writeSpace();

				break;
			}

			case XMLConfigMarkIParameters::VALUE_TYPE_DOUBLE:
			{
				//get the parameter
				XMLConfigMarkIDouble* parameter_ = (XMLConfigMarkIDouble*)it.getElement()->getm_data();

				//add the type attribute to the current parameter
				rc = xmlTextWriterWriteAttribute(m_writer, BAD_CAST XMLConfigMarkIParameters::PARAMETER_TYPE_ATTRIBUTE.c_str(),
						BAD_CAST XMLConfigMarkIParameters::XML_TAG_DOUBLE.c_str() );
				if(rc < 0)
				{
					printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
					return;
				}

				writeSpace();

				//add the actual content
				rc = xmlTextWriterWriteString(m_writer, BAD_CAST Lazarus::Toolbox::doubleToString( parameter_->getParam() ).c_str() );
				if(rc < 0)
				{
					printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
					return;
				}

				writeSpace();

				break;
			}

			case XMLConfigMarkIParameters::VALUE_TYPE_FLOAT:
			{
				//get the parameter
				XMLConfigMarkIFloat* parameter_ = (XMLConfigMarkIFloat*)it.getElement()->getm_data();

				//add the type attribute to the current parameter
				rc = xmlTextWriterWriteAttribute(m_writer, BAD_CAST XMLConfigMarkIParameters::PARAMETER_TYPE_ATTRIBUTE.c_str(),
						BAD_CAST XMLConfigMarkIParameters::XML_TAG_FLOAT.c_str() );
				if(rc < 0)
				{
					printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
					return;
				}

				writeSpace();

				//add the actual content
				rc = xmlTextWriterWriteString(m_writer, BAD_CAST Lazarus::Toolbox::floatToString( parameter_->getParam() ).c_str() );
				if(rc < 0)
				{
					printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
					return;
				}

				writeSpace();

				break;
			}

			case XMLConfigMarkIParameters::VALUE_TYPE_INT:
			{
				//get the parameter
				XMLConfigMarkIInt* parameter_ = (XMLConfigMarkIInt*)it.getElement()->getm_data();

				//add the type attribute to the current parameter
				rc = xmlTextWriterWriteAttribute(m_writer, BAD_CAST XMLConfigMarkIParameters::PARAMETER_TYPE_ATTRIBUTE.c_str(),
						BAD_CAST XMLConfigMarkIParameters::XML_TAG_INT.c_str());
				if(rc < 0)
				{
					printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
					return;
				}

				writeSpace();

				//add the actual content
				rc = xmlTextWriterWriteString(m_writer, BAD_CAST BAD_CAST Lazarus::Toolbox::intToString( parameter_->getParam() ).c_str() );
				if(rc < 0)
				{
					printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
					return;
				}

				writeSpace();

				break;
			}

			case XMLConfigMarkIParameters::VALUE_TYPE_UINT:
			{
				//get the parameter
				XMLConfigMarkIUInt* parameter_ = (XMLConfigMarkIUInt*)it.getElement()->getm_data();

				//add the type attribute to the current parameter
				rc = xmlTextWriterWriteAttribute(m_writer, BAD_CAST XMLConfigMarkIParameters::PARAMETER_TYPE_ATTRIBUTE.c_str(),
						BAD_CAST XMLConfigMarkIParameters::XML_TAG_UINT.c_str());
				if(rc < 0)
				{
					printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
					return;
				}

				writeSpace();

				//add the actual content
				rc = xmlTextWriterWriteString(m_writer, BAD_CAST BAD_CAST Lazarus::Toolbox::uintToString( parameter_->getParam() ).c_str() );
				if(rc < 0)
				{
					printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
					return;
				}

				writeSpace();

				break;
			}

			case XMLConfigMarkIParameters::VALUE_TYPE_LONG:
			{
				//get the parameter
				XMLConfigMarkILong* parameter_ = (XMLConfigMarkILong*)it.getElement()->getm_data();
				//printf("long %d \n",parameter_->getParam());
				//add the type attribute to the current parameter
				rc = xmlTextWriterWriteAttribute(m_writer, BAD_CAST XMLConfigMarkIParameters::PARAMETER_TYPE_ATTRIBUTE.c_str(),
						BAD_CAST XMLConfigMarkIParameters::XML_TAG_LONG.c_str());
				if(rc < 0)
				{
					printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
					return;
				}

				writeSpace();

				//add the actual content
				rc = xmlTextWriterWriteString(m_writer, BAD_CAST BAD_CAST Lazarus::Toolbox::longToString( parameter_->getParam() ).c_str() );
				if(rc < 0)
				{
					printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
					return;
				}

				writeSpace();

				break;
			}

			case XMLConfigMarkIParameters::VALUE_TYPE_LONGLONG:
			{
				//get the parameter
				XMLConfigMarkILongLong* parameter_ = (XMLConfigMarkILongLong*)it.getElement()->getm_data();
				//printf("long %d \n",parameter_->getParam());
				//add the type attribute to the current parameter
				rc = xmlTextWriterWriteAttribute(m_writer, BAD_CAST XMLConfigMarkIParameters::PARAMETER_TYPE_ATTRIBUTE.c_str(),
						BAD_CAST XMLConfigMarkIParameters::XML_TAG_LONGLONG.c_str());
				if(rc < 0)
				{
					printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
					return;
				}

				writeSpace();

				//add the actual content
				rc = xmlTextWriterWriteString(m_writer, BAD_CAST BAD_CAST Lazarus::Toolbox::longLongToString( parameter_->getParam() ).c_str() );
				if(rc < 0)
				{
					printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
					return;
				}

				writeSpace();

				break;
			}

			case XMLConfigMarkIParameters::VALUE_TYPE_ULONG:
			{
				//get the parameter
				XMLConfigMarkIULong* parameter_ = (XMLConfigMarkIULong*)it.getElement()->getm_data();

				//add the type attribute to the current parameter
				rc = xmlTextWriterWriteAttribute(m_writer, BAD_CAST XMLConfigMarkIParameters::PARAMETER_TYPE_ATTRIBUTE.c_str(),
						BAD_CAST XMLConfigMarkIParameters::XML_TAG_ULONG.c_str());
				if(rc < 0)
				{
					printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
					return;
				}

				writeSpace();

				//add the actual content
				rc = xmlTextWriterWriteString(m_writer, BAD_CAST BAD_CAST Lazarus::Toolbox::ulongToString( parameter_->getParam() ).c_str() );
				if(rc < 0)
				{
					printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
					return;
				}

				writeSpace();

				break;
			}

			case XMLConfigMarkIParameters::VALUE_TYPE_ULONGLONG:
			{
				//get the parameter
				XMLConfigMarkIULongLong* parameter_ = (XMLConfigMarkIULongLong*)it.getElement()->getm_data();

				//add the type attribute to the current parameter
				rc = xmlTextWriterWriteAttribute(m_writer, BAD_CAST XMLConfigMarkIParameters::PARAMETER_TYPE_ATTRIBUTE.c_str(),
						BAD_CAST XMLConfigMarkIParameters::XML_TAG_ULONGLONG.c_str());
				if(rc < 0)
				{
					printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
					return;
				}

				writeSpace();

				//add the actual content
				rc = xmlTextWriterWriteString(m_writer, BAD_CAST BAD_CAST Lazarus::Toolbox::ulongLongToString( parameter_->getParam() ).c_str() );
				if(rc < 0)
				{
					printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
					return;
				}

				writeSpace();

				break;
			}

			case XMLConfigMarkIParameters::VALUE_TYPE_STRING_ARRAY:
			{
				//get the parameter
				XMLConfigMarkIStringArray* parameter_ = (XMLConfigMarkIStringArray*)it.getElement()->getm_data();

				//add the type attribute to the current parameter
				rc = xmlTextWriterWriteAttribute(m_writer, BAD_CAST XMLConfigMarkIParameters::PARAMETER_TYPE_ATTRIBUTE.c_str(),
						BAD_CAST XMLConfigMarkIParameters::XML_TAG_STRING_ARRAY.c_str());
				if(rc < 0)
				{
					printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
					return;
				}

				writeSpace();

				//add the actual content, element by element
				Lazarus::FastNCSList<std::string>* list = parameter_->getParam();
				for(Lazarus::FastNCSList<std::string>::Iterator it_ = list->getStart(); it_ != list->getEnd();++it_)
				{
					rc = xmlTextWriterWriteElement(m_writer, BAD_CAST XMLConfigMarkIParameters::PARAMETER_STRING_TAG.c_str() ,
							BAD_CAST it_.getElement()->getm_data().c_str());
					if(rc < 0)
					{
						printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
						return;
					}

					writeSpace();
				}

				break;
			}

			case XMLConfigMarkIParameters::VALUE_TYPE_DOUBLE_ARRAY:
			{
				//get the parameter
				XMLConfigMarkIDoubleArray* parameter_ = (XMLConfigMarkIDoubleArray*)it.getElement()->getm_data();

				//add the type attribute to the current parameter
				rc = xmlTextWriterWriteAttribute(m_writer, BAD_CAST XMLConfigMarkIParameters::PARAMETER_TYPE_ATTRIBUTE.c_str(),
						BAD_CAST XMLConfigMarkIParameters::XML_TAG_DOUBLE_ARRAY.c_str());
				if(rc < 0)
				{
					printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
					return;
				}

				writeSpace();

				//add the actual content, element by element
				double* array = parameter_->getParam();

				//the first n-1 values
				for(unsigned int i=0;i<parameter_->getSize()-1;i++)
				{
					rc = xmlTextWriterWriteString(m_writer, BAD_CAST (Lazarus::Toolbox::doubleToString( array[i] )+std::string(",")).c_str() );
					if(rc < 0)
					{
						printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
						return;
					}
				}
				//the last value
				rc = xmlTextWriterWriteString(m_writer, BAD_CAST Lazarus::Toolbox::doubleToString( array[parameter_->getSize()-1] ).c_str() );
				if(rc < 0)
				{
					printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
					return;
				}

				writeSpace();

				break;
			}

			case XMLConfigMarkIParameters::VALUE_TYPE_INT_ARRAY:
			{
				//get the parameter
				XMLConfigMarkIIntArray* parameter_ = (XMLConfigMarkIIntArray*)it.getElement()->getm_data();

				//add the type attribute to the current parameter
				rc = xmlTextWriterWriteAttribute(m_writer, BAD_CAST XMLConfigMarkIParameters::PARAMETER_TYPE_ATTRIBUTE.c_str(),
						BAD_CAST XMLConfigMarkIParameters::XML_TAG_INT_ARRAY.c_str());
				if(rc < 0)
				{
					printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
					return;
				}

				writeSpace();

				//add the actual content, element by element
				int* array = parameter_->getParam();

				//the first n-1 values
				for(unsigned int i=0;i<parameter_->getSize()-1;i++)
				{
					rc = xmlTextWriterWriteString(m_writer, BAD_CAST (Lazarus::Toolbox::intToString( array[i] )+std::string(",")).c_str() );
					if(rc < 0)
					{
						printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
						return;
					}
				}
				//the last value
				rc = xmlTextWriterWriteString(m_writer, BAD_CAST Lazarus::Toolbox::intToString( array[parameter_->getSize()-1] ).c_str() );
				if(rc < 0)
				{
					printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
					return;
				}

				writeSpace();

				break;
			}

			case XMLConfigMarkIParameters::VALUE_TYPE_UINT_ARRAY:
			{
				//get the parameter
				XMLConfigMarkIUIntArray* parameter_ = (XMLConfigMarkIUIntArray*)it.getElement()->getm_data();

				//add the type attribute to the current parameter
				rc = xmlTextWriterWriteAttribute(m_writer, BAD_CAST XMLConfigMarkIParameters::PARAMETER_TYPE_ATTRIBUTE.c_str(),
						BAD_CAST XMLConfigMarkIParameters::XML_TAG_UINT_ARRAY.c_str());
				if(rc < 0)
				{
					printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
					return;
				}

				writeSpace();

				//add the actual content, element by element
				unsigned int* array = parameter_->getParam();

				//the first n-1 values
				for(unsigned int i=0;i<parameter_->getSize()-1;i++)
				{
					rc = xmlTextWriterWriteString(m_writer, BAD_CAST (Lazarus::Toolbox::uintToString( array[i] )+std::string(",")).c_str() );
					if(rc < 0)
					{
						printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
						return;
					}
				}
				//the last value
				rc = xmlTextWriterWriteString(m_writer, BAD_CAST Lazarus::Toolbox::uintToString( array[parameter_->getSize()-1] ).c_str() );
				if(rc < 0)
				{
					printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
					return;
				}

				writeSpace();

				break;
			}

			case XMLConfigMarkIParameters::VALUE_TYPE_LONG_ARRAY:
			{
				//get the parameter
				XMLConfigMarkILongArray* parameter_ = (XMLConfigMarkILongArray*)it.getElement()->getm_data();

				//add the type attribute to the current parameter
				rc = xmlTextWriterWriteAttribute(m_writer, BAD_CAST XMLConfigMarkIParameters::PARAMETER_TYPE_ATTRIBUTE.c_str(),
						BAD_CAST XMLConfigMarkIParameters::XML_TAG_LONG_ARRAY.c_str());
				if(rc < 0)
				{
					printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
					return;
				}

				writeSpace();

				//add the actual content, element by element
				long* array = parameter_->getParam();

				//the first n-1 values
				for(unsigned int i=0;i<parameter_->getSize()-1;i++)
				{
					rc = xmlTextWriterWriteString(m_writer, BAD_CAST (Lazarus::Toolbox::longToString( array[i] )+std::string(",")).c_str() );
					if(rc < 0)
					{
						printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
						return;
					}
				}
				//the last value
				rc = xmlTextWriterWriteString(m_writer, BAD_CAST Lazarus::Toolbox::longToString( array[parameter_->getSize()-1] ).c_str() );
				if(rc < 0)
				{
					printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
					return;
				}

				writeSpace();

				break;
			}

			case XMLConfigMarkIParameters::VALUE_TYPE_LONGLONG_ARRAY:
			{
				//get the parameter
				XMLConfigMarkILongLongArray* parameter_ = (XMLConfigMarkILongLongArray*)it.getElement()->getm_data();

				//add the type attribute to the current parameter
				rc = xmlTextWriterWriteAttribute(m_writer, BAD_CAST XMLConfigMarkIParameters::PARAMETER_TYPE_ATTRIBUTE.c_str(),
						BAD_CAST XMLConfigMarkIParameters::XML_TAG_LONGLONG_ARRAY.c_str());
				if(rc < 0)
				{
					printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
					return;
				}

				writeSpace();

				//add the actual content, element by element
				long long* array = parameter_->getParam();

				//the first n-1 values
				for(unsigned int i=0;i<parameter_->getSize()-1;i++)
				{
					rc = xmlTextWriterWriteString(m_writer, BAD_CAST (Lazarus::Toolbox::longLongToString( array[i] )+std::string(",")).c_str() );
					if(rc < 0)
					{
						printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
						return;
					}
				}
				//the last value
				rc = xmlTextWriterWriteString(m_writer, BAD_CAST Lazarus::Toolbox::longLongToString( array[parameter_->getSize()-1] ).c_str() );
				if(rc < 0)
				{
					printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
					return;
				}

				writeSpace();

				break;
			}

			case XMLConfigMarkIParameters::VALUE_TYPE_ULONG_ARRAY:
			{
				//get the parameter
				XMLConfigMarkIULongArray* parameter_ = (XMLConfigMarkIULongArray*)it.getElement()->getm_data();

				//add the type attribute to the current parameter
				rc = xmlTextWriterWriteAttribute(m_writer, BAD_CAST XMLConfigMarkIParameters::PARAMETER_TYPE_ATTRIBUTE.c_str(),
						BAD_CAST XMLConfigMarkIParameters::XML_TAG_ULONG_ARRAY.c_str());
				if(rc < 0)
				{
					printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
					return;
				}

				writeSpace();

				//add the actual content, element by element
				unsigned long* array = parameter_->getParam();

				//the first n-1 values
				for(unsigned int i=0;i<parameter_->getSize()-1;i++)
				{
					rc = xmlTextWriterWriteString(m_writer, BAD_CAST (Lazarus::Toolbox::ulongToString( array[i] )+std::string(",")).c_str() );
					if(rc < 0)
					{
						printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
						return;
					}
				}
				//the last value
				rc = xmlTextWriterWriteString(m_writer, BAD_CAST Lazarus::Toolbox::ulongToString( array[parameter_->getSize()-1] ).c_str() );
				if(rc < 0)
				{
					printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
					return;
				}

				writeSpace();

				break;
			}

			case XMLConfigMarkIParameters::VALUE_TYPE_ULONGLONG_ARRAY:
			{
				//get the parameter
				XMLConfigMarkIULongLongArray* parameter_ = (XMLConfigMarkIULongLongArray*)it.getElement()->getm_data();

				//add the type attribute to the current parameter
				rc = xmlTextWriterWriteAttribute(m_writer, BAD_CAST XMLConfigMarkIParameters::PARAMETER_TYPE_ATTRIBUTE.c_str(),
						BAD_CAST XMLConfigMarkIParameters::XML_TAG_ULONGLONG_ARRAY.c_str());
				if(rc < 0)
				{
					printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
					return;
				}

				writeSpace();

				//add the actual content, element by element
				unsigned long long* array = parameter_->getParam();

				//the first n-1 values
				for(unsigned int i=0;i<parameter_->getSize()-1;i++)
				{
					rc = xmlTextWriterWriteString(m_writer, BAD_CAST (Lazarus::Toolbox::ulongLongToString( array[i] )+std::string(",")).c_str() );
					if(rc < 0)
					{
						printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
						return;
					}
				}
				//the last value
				rc = xmlTextWriterWriteString(m_writer, BAD_CAST Lazarus::Toolbox::ulongLongToString( array[parameter_->getSize()-1] ).c_str() );
				if(rc < 0)
				{
					printf("testXmlwriterMemory: Error at xmlTextWriterWriteAttribute\n");
					return;
				}

				writeSpace();

				break;
			}

			default:
				break;
		}

		/*close the parameter element*/
		rc = xmlTextWriterEndElement(m_writer);
		if(rc < 0)
		{
			printf("testXmlwriterMemory: Error at xmlTextWriterEndElement\n");
			return;
		}

		writeSpace();

	}


}

} /* namespace Lazarus */
