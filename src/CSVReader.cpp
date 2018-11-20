/*
 * CSVReader.cpp
 *
 *  Created on: 07.03.2016
 *      Author: Darius Malysiak
 */

#include "CSVReader.h"

#include "Toolbox.h"

namespace Lazarus {

CSVReader::CSVReader(const std::string& filename_or_content,bool read_file,char sep,char ter, bool linebreaks_after_terminator) {

	m_sep = sep;
	m_ter = ter;
	m_linebreaks_after_terminator = linebreaks_after_terminator;
	m_current_pos = 0;

	if(read_file == true)
	{
		m_content = Lazarus::Toolbox::readFileToString(filename_or_content);
	}
	else
	{
		m_content = filename_or_content;
	}

	m_content_length = m_content.size();
}

CSVReader::~CSVReader() {
	// TODO Auto-generated destructor stub
}

char CSVReader::getChar()
{
	char c = m_content.c_str()[m_current_pos];
	++m_current_pos;
	return c;
}

int CSVReader::readCSVValue(std::string& s)
{
	char c[2];
	c[1] = '\0';
	//read chars until ,  ; or EOF found
	while(1){
		c[0] = getChar();
		if(c[0]==m_sep) return 0;
		if(c[0]==m_ter)
		{
			if(m_linebreaks_after_terminator == true)
			{
				getChar(); //read ahead one char, i.e. skip line break. Uncomment this if linebreaks are not being used after ;
			}

			return 1;
		}
		if(m_current_pos==m_content_length)
		{
			return 2;
		}
		s.append(c);//printf("debug %c\n",c);
	}

	return -1;
}

} /* namespace Lazarus */
