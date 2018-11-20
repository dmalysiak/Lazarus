/*
 * LogBook.h
 *
 *  Created on: Nov 6, 2013
 *      Author: Darius Malysiak
 */

#ifndef LOGBOOK_H_
#define LOGBOOK_H_

#include "BaseObject.h"
#include "DataStructures/Fifo.h"

#include <string>

namespace Lazarus {

/**
 * This logbook will keep its entries in memory:
 * - once the capacity limit was reached the oldest entry will be dropped
 * - a call to saveLogBook will save the current entries
 * - if setRotateMode was called in order to enable the rotation mode; once the
 *   capacity limit was reached the entries will be written to disk and the log book
 *   will be cleared afterwards.
 *
 *   Lazarus::LogBook logbook("Tracker logs",50000);
 *   logbook.setRotate(true,"./logfile");
 *
 *	 for(int i=0;i<700000;i++)
 *		logbook.addEntry(Lazarus::Toolbox::intToString(i)+"\n");
 *
 * */
class LogBook: public Lazarus::BaseObject {
public:
	LogBook();
	LogBook(const std::string& log_book_title, long long max_size = 200);
	virtual ~LogBook();

	void addEntry(const std::string& entry);
	void printLogs() const;
	void saveLogBook(const std::string& file) const;
	void saveLogBookCompressed(const std::string& file) const;

	/**
	 * This can activate the rotate mode in which the logbooks
	 * content will be written to disk once the capacity was reached.
	 * Afterwards all entries will be purged and the new entry will be added.
	 * */
	void setRotateMode(bool rotate, const std::string& file = "./log");

private:
	std::string m_log_book_title;
	long long m_max_size;
	Fifo<std::string>* mp_log_entries;
	bool m_rotate_mode;
	std::string m_rotate_file;
	unsigned long long m_rotate_counter;
};

/**
 * This is a straight-to-file logbook, all entries will be saved immediately to the given file.
 * Once the cap was reached, the file will be renamed in rotation-fashion and compressed if possible.
 * The output will be 'rotatePath+"/"+log_book_title+rotationIndex'
 *
 * Lazarus::FileLogBook logbook("Tracker logs","./",50000);
 *
 * for(int i=0;i<700000;i++)
 *		logbook.addEntry(Lazarus::Toolbox::intToString(i)+"\n");
 *
 * */
class FileLogBook: public Lazarus::BaseObject {
public:
	FileLogBook();
	FileLogBook(const std::string& log_book_title, const std::string& rotatePath = "./", long long max_size = 200);
	virtual ~FileLogBook();

	void addEntry(const std::string& entry);

private:
	std::string m_log_book_title;
	long long m_max_size;
	std::string m_rotate_path;
	unsigned long long m_rotate_counter;
	unsigned long long m_entry_counter;
};

} /* namespace Lazarus */
#endif /* LOGBOOK_H_ */
