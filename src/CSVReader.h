/*
 * CSVReader.h
 *
 *  Created on: 07.03.2016
 *      Author: Darius Malysiak
 */

#ifndef CORE_CSVREADER_H_
#define CORE_CSVREADER_H_

#include "BaseObject.h"

#include <string>

namespace Lazarus {

class CSVReader: public BaseObject {
public:
	/**
	 * This class reads CSV tokens (i.e. values) from the given file or string (read_file determines
	 * if filename_or_content represents a filename or some actual content, respectively).
	 * It is assumed that the file contains the following format:
	 * VAL<SEP>VAL<SEP>VAL<TER><LBREAK>
	 * linebreaks_after_terminator should be set to true if
	 * <LBREAK> occur after the <TER> symbol.
	 * */
	CSVReader(const std::string& filename_or_content,bool read_file = true, char sep=',',char ter=';', bool linebreaks_after_terminator=true);

	virtual ~CSVReader();

	/**
	 * The value is inserted into the given string.
	 *
	 * The function returns 0 in case of a complete value without hitting a TER symbol,
	 * 1 in case of hitting a TER symbol, 2 in case of hitting EOF, -1 else
	 * */
	int readCSVValue(std::string& s);

private:
	char m_sep;
	char m_ter;
	bool m_linebreaks_after_terminator;

	unsigned int m_current_pos;
	unsigned int m_content_length;

	std::string m_content;

	char getChar();
};

} /* namespace Lazarus */

#endif /* CORE_CSVREADER_H_ */
