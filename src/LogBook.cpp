/*
 * LogBook.cpp
 *
 *  Created on: Nov 6, 2013
 *      Author: Darius Malysiak
 */

#include "LogBook.h"
#include "Toolbox.h"
#include "System/CommandToolbox.h"

#include <stdio.h>

namespace Lazarus {

LogBook::LogBook(const std::string& log_book_title, long long max_size) {

	mp_log_entries = new Fifo<std::string>(max_size);
	m_log_book_title = log_book_title;
	m_max_size = max_size;
	m_rotate_counter = 0;
	m_rotate_mode = false;
}

LogBook::LogBook()
{
	mp_log_entries = new Fifo<std::string>(20);
	m_log_book_title = std::string("- untitled logbook -");
	m_max_size = -1;
	m_rotate_mode = false;
	m_rotate_counter = 0;
}

LogBook::~LogBook() {

	delete mp_log_entries;

}

void LogBook::setRotateMode(bool rotate, const std::string& file)
{
	m_rotate_mode = rotate;
	m_rotate_file = file;
}

void LogBook::addEntry(const std::string& entry)
{
	//just keep pushing into the log
	if(m_rotate_mode == false)
	{
		mp_log_entries->push(entry);
	}
	else //save the history and push afterwards if necessary.
	{
		std::string s = m_rotate_file+Toolbox::ulongLongToString(m_rotate_counter);

		//if cap was reached
		if(mp_log_entries->getSize() + 1 > (unsigned long long)m_max_size)
		{
			saveLogBook(s);

			//compress if possible
			int res = CommandToolbox::executeSystemCommand("tar -zcvf "+s+".tar.gz "+s);

			//delete the file if the compression succeeded.
			if(res == 0)
			{
				CommandToolbox::deleteFile(s);
			}

			m_rotate_counter++;
			mp_log_entries->clear();
			mp_log_entries->push(entry);
		}
		else
			mp_log_entries->push(entry);
	}
}

void LogBook::printLogs() const
{
	printf("Logbook: %s",m_log_book_title.c_str());

	//iterate over all log entries
	for(auto it = mp_log_entries->getStart();it != mp_log_entries->getEnd();++it)
	{
		printf("%s\n", it.getElement()->getm_data().c_str());
	}

}

void LogBook::saveLogBook(const std::string& file) const
{
	Toolbox::writeStringVectorToFile(*mp_log_entries,(char*)file.c_str());
}

void LogBook::saveLogBookCompressed(const std::string& file) const
{
	Toolbox::writeStringVectorToFile(*mp_log_entries,(char*)file.c_str());

	//compress if possible
	int res = CommandToolbox::executeSystemCommand("tar -zcvf "+file+".tar.gz "+file);

	//delete the file if the compression succeeded.
	if(res == 0)
	{
		CommandToolbox::deleteFile(file);
	}
}


//-------------------------------------------------------------


FileLogBook::FileLogBook(const std::string& log_book_title, const std::string& rotatePath, long long max_size) {

	m_log_book_title = log_book_title;
	m_max_size = max_size;
	m_rotate_counter = 0;
	m_entry_counter = 0;
	m_rotate_path = Toolbox::ensureValidPath(rotatePath);
}

FileLogBook::FileLogBook()
{
	m_log_book_title = std::string("- untitled logbook -");
	m_max_size = -1;
	m_rotate_counter = 0;
	m_entry_counter = 0;
	m_rotate_path = "./";
}

FileLogBook::~FileLogBook() {

}

void FileLogBook::addEntry(const std::string& entry)
{
	std::string s = m_rotate_path+m_log_book_title+Toolbox::ulongLongToString(m_rotate_counter);

	//if cap was reached
	if(m_entry_counter + 1 > (unsigned long long)m_max_size)
	{
		//compress if possible
		std::string temp = "tar -zcvf \""+s+".tar.gz\" "+"\""+s+"\"";
		printf("%s\n",m_rotate_path.c_str());
		int res = CommandToolbox::executeSystemCommand("tar -zcvf \""+s+".tar.gz\" "+"\""+s+"\"");

		//delete the file if the compression succeeded.
		if(res == 0)
		{
			CommandToolbox::deleteFile("\""+s+"\"");
		}

		m_rotate_counter++;

		Lazarus::Toolbox::appendStringToFile(entry,  m_rotate_path+m_log_book_title+Toolbox::ulongLongToString(m_rotate_counter));

		m_entry_counter = 1;
	}
	else
	{
		Lazarus::Toolbox::appendStringToFile(entry, s);
		m_entry_counter++;
	}

}


} /* namespace Lazarus */

