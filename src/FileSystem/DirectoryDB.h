/*
 * Directory.h
 *
 *  Created on: 19.02.2013
 *      Author: Darius Malysiak
 */

#ifndef DIRECTORYDB_H_
#define DIRECTORYDB_H_

#include "Directory.h"
#include "DirectoryEntry.h"

#include <unordered_map>
#include <vector>
#include <string>
#include <dirent.h>

namespace Lazarus {

/**
 * This class creates a file database which can be queried efficiently.
 * */

class DirectoryDB: public Directory {
public:
	DirectoryDB();
	DirectoryDB(const std::string& directory_path="");
	virtual ~DirectoryDB();

	/**
	 * Will read every entry within the given directory.
	 */
	virtual void readDirectory(const std::string& directory_path="",const std::string& regex=".");

	/**
	 * Will read every non-file and non-directory entry within the given directory.
	 */
	virtual void readDirectorySpecial(const std::string& directory_path="",const std::string& regex=".");

	/**
	 * Will read every regular file within the given directory.
	 */
	virtual void readDirectoryFiles(const std::string& directory_path="",const std::string& regex=".");

	/**
	 * Will read every subdirectory within the given directory.
	 */
	virtual void readDirectoryDirectories(const std::string& directory_path="",const std::string& regex=".");

	/**
	 * Will read every file and subdirectory within the given directory.
	 */
	virtual void readDirectoryFilesAndDirectories(const std::string& directory_path="",const std::string& regex=".");

	/*
	 * Returns the entry associated with the given name, if multiple entries exist e.g. files located in subfolders
	 * only the first found entry will be returned ('found' will be set true). Should no entry be found a default
	 * constructed DirectoryEntry will be returned and the found flag will be set false.
	 */
	const DirectoryEntry& getFile(const std::string& filename, bool& found);

	/*
	 * Returns the entries associated with the given name, if multiple entries exist e.g. files located in subfolders
	 * will be added to an unordered_map ('found' will be set true). Should no entry be found an empty
	 * vector will be returned and the found flag will be set false.
	 */
	std::vector<DirectoryEntry> getFiles(const std::string& filename, bool& found);

	/**
	 * Returns a reference to the list of read entries.
	 * */
	const std::unordered_multimap<std::string,DirectoryEntry>& getDB();


private:
	std::string m_directory;
	std::unordered_multimap<std::string,DirectoryEntry> m_fileDB;

	virtual void reset();
};

} /* namespace MPCLib */
#endif /* DIRECTORY_H_ */
