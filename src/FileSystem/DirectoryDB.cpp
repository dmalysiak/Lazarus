/*
 * Directory.cpp
 *
 *  Created on: 19.02.2013
 *      Author: Darius Malysiak
 */

#include "DirectoryDB.h"
#include "../Toolbox.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sendfile.h>
#include <pwd.h>
#include <grp.h>
#include <fcntl.h>
#include <regex.h>

#include <unistd.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>
#include <stdio.h>
#include <stdint.h>
#include <dirent.h>

namespace Lazarus {

static DirectoryEntry d____dummy;

DirectoryDB::DirectoryDB()
{
	mp_filelist = NULL;
	current_iterator = NULL;
}


DirectoryDB::DirectoryDB(const std::string& directory_path)
{
	mp_filelist = NULL;
	current_iterator = NULL;
	m_directory = directory_path;
}

DirectoryDB::~DirectoryDB()
{

}

void DirectoryDB::reset()
{
	m_fileDB.clear();
}


void DirectoryDB::readDirectoryFiles(const std::string& directory_path,const std::string& regex)
{
	//internal remark

	/**
	 * entries[i]->d_type might not be completely platform independent.
	 * The valid values are:
	 * DT_BLK      This is a block device.
     * DT_CHR      This is a character device.
     * DT_DIR      This is a directory.
     * DT_FIFO     This is a named pipe (FIFO).
     * DT_LNK      This is a symbolic link.
     * DT_REG      This is a regular file.
     * DT_SOCK     This is a UNIX domain socket.
     * DT_UNKNOWN  The file type is unknown.
	 * */

	//clean up for a new run
	reset();

	regex_t r;
	int res = regcomp(&r, regex.c_str(), REG_EXTENDED | REG_NOSUB);
	if( res != 0)
	{
		ERROR(getRegExError(res));
		return;
	}

	struct dirent** entries;
	std::string filename;

	//if a filename was given; update the internal one
	if(directory_path.compare("")!=0)
	{
		m_directory=directory_path;
	}

	int file_count = scandir(m_directory.c_str(), &entries, filter_fs_spec_entries_files, alphasort);

	if(file_count == -1)
	{
		printf("ERROR: could not open directory '%s'\n",m_directory.c_str());
		regfree(&r);
		return;
	}

	for(int i=0;i<file_count;i++)
	{
	  if(regexec(&r, entries[i]->d_name, 0, 0, 0) == REG_NOMATCH)
		  continue;

	  //get the attributes for each directory entry
	  filename = std::string(entries[i]->d_name);
	  m_fileDB.insert(std::make_pair(filename,DirectoryEntry(filename,m_directory+"/"+filename)));
	}

	/* once finished:
	 * update the classes directory path and reference to the first directory entry
	 */
    current_iterator = mp_filelist->getFirst();

    //free memory
    for(int i = 0; i < file_count; i++)
    {
    	free(entries[i]);
    }
    free(entries);
    regfree(&r);
}

void DirectoryDB::readDirectoryFilesAndDirectories(const std::string& directory_path,const std::string& regex)
{
	//clean up for a new run
	reset();

	//if a filename was given; update the internal one
	if(directory_path.compare("")!=0)
	{
		m_directory=directory_path;
	}

	regex_t r;
	int res = regcomp(&r, regex.c_str(), REG_EXTENDED | REG_NOSUB);
	if( res != 0)
	{
		ERROR(getRegExError(res));
		return;
	}

	struct dirent** entries;
	std::string filename;

	int file_count = scandir(m_directory.c_str(), &entries, filter_fs_spec_entries_files_and_folders, alphasort);

	if(file_count == -1)
	{
		printf("ERROR: could not open directory '%s'\n",m_directory.c_str());
		regfree(&r);
		return;
	}

	for(int i=0;i<file_count;i++)
	{
		if(regexec(&r, entries[i]->d_name, 0, 0, 0) == REG_NOMATCH)
		  continue;
	  //get the attributes for each directory entry
	  filename = std::string(entries[i]->d_name);
	  m_fileDB.insert(std::make_pair(filename,DirectoryEntry(filename,m_directory+"/"+filename)));
	}

	/* once finished:
	 * update the classes directory path and reference to the first directory entry
	 */
    current_iterator = mp_filelist->getFirst();

    //free memory
    for(int i = 0; i < file_count; i++)
    {
    	free(entries[i]);
    }
    free(entries);
    regfree(&r);
}

void DirectoryDB::readDirectoryDirectories(const std::string& directory_path,const std::string& regex)
{
	//clean up for a new run
	reset();

	//if a filename was given; update the internal one
	if(directory_path.compare("")!=0)
	{
		m_directory=directory_path;
	}

	regex_t r;
	int res = regcomp(&r, regex.c_str(), REG_EXTENDED | REG_NOSUB);
	if( res != 0)
	{
		ERROR(getRegExError(res));
		return;
	}

	struct dirent** entries;
	std::string filename;

	int file_count = scandir(m_directory.c_str(), &entries, filter_fs_spec_entries_folders, alphasort);

	if(file_count == -1)
	{
		printf("ERROR: could not open directory '%s'\n",m_directory.c_str());
		regfree(&r);
		return;
	}

	for(int i=0;i<file_count;i++)
	{
		if(regexec(&r, entries[i]->d_name, 0, 0, 0) == REG_NOMATCH)
		  continue;
	  //get the attributes for each directory entry
	  filename = std::string(entries[i]->d_name);
	  m_fileDB.insert(std::make_pair(filename,DirectoryEntry(filename,m_directory+"/"+filename)));
	}

	/* once finished:
	 * update the classes directory path and reference to the first directory entry
	 */
    current_iterator = mp_filelist->getFirst();

    //free memory
    for(int i = 0; i < file_count; i++)
    {
    	free(entries[i]);
    }
    free(entries);
    regfree(&r);
}

void DirectoryDB::readDirectory(const std::string& directory_path,const std::string& regex)
{
	//clean up for a new run
	reset();

	//if a filename was given; update the internal one
	if(directory_path.compare("")!=0)
	{
		m_directory=directory_path;
	}

	regex_t r;
	int res = regcomp(&r, regex.c_str(), REG_EXTENDED | REG_NOSUB);
	if( res != 0)
	{
		ERROR(getRegExError(res));
		return;
	}

	struct dirent** entries;
	std::string filename;

	int file_count = scandir(m_directory.c_str(), &entries, filter_fs_spec_entries_none, alphasort);

	if(file_count == -1)
	{
		printf("ERROR: could not open directory '%s'\n",m_directory.c_str());
		regfree(&r);
		return;
	}

	for(int i=0;i<file_count;i++)
	{
		if(regexec(&r, entries[i]->d_name, 0, 0, 0) == REG_NOMATCH)
		 continue;
	  //get the attributes for each directory entry
	  filename = std::string(entries[i]->d_name);
	  m_fileDB.insert(std::make_pair(filename,DirectoryEntry(filename,m_directory+"/"+filename)));
	}

	/* once finished:
	 * update the classes directory path and reference to the first directory entry
	 */
    current_iterator = mp_filelist->getFirst();

    //free memory
    for(int i = 0; i < file_count; i++)
    {
    	free(entries[i]);
    }
    free(entries);
    regfree(&r);
}

void DirectoryDB::readDirectorySpecial(const std::string& directory_path,const std::string& regex)
{
	//clean up for a new run
	reset();

	//if a filename was given; update the internal one
	if(directory_path.compare("")!=0)
	{
		m_directory=directory_path;
	}

	regex_t r;
	int res = regcomp(&r, regex.c_str(), REG_EXTENDED | REG_NOSUB);
	if( res != 0)
	{
		ERROR(getRegExError(res));
		return;
	}

	struct dirent** entries;
	std::string filename;

	int file_count = scandir(m_directory.c_str(), &entries, filter_fs_spec_entries_special, alphasort);

	if(file_count == -1)
	{
		printf("ERROR: could not open directory '%s'\n",m_directory.c_str());
		regfree(&r);
		return;
	}

	for(int i=0;i<file_count;i++)
	{
		if(regexec(&r, entries[i]->d_name, 0, 0, 0) == REG_NOMATCH)
			  continue;
	  //get the attributes for each directory entry
	  filename = std::string(entries[i]->d_name);
	  m_fileDB.insert(std::make_pair(filename,DirectoryEntry(filename,m_directory+"/"+filename)));
	}

	/* once finished:
	 * update the classes directory path and reference to the first directory entry
	 */
    current_iterator = mp_filelist->getFirst();

    //free memory
    for(int i = 0; i < file_count; i++)
    {
    	free(entries[i]);
    }
    free(entries);
    regfree(&r);
}

const std::unordered_multimap<std::string,DirectoryEntry>& DirectoryDB::getDB()
{
	return m_fileDB;
}

const DirectoryEntry& DirectoryDB::getFile(const std::string& filename, bool& found)
{

	//get an iterator to the results
	std::unordered_multimap<std::string,DirectoryEntry>::const_iterator entries = m_fileDB.find(filename);

	if(entries == m_fileDB.end() )
	{
		found = false;
		return d____dummy;
	}
	else
	{
		found = true;
		return entries->second;
	}

}

std::vector<DirectoryEntry> DirectoryDB::getFiles(const std::string& filename, bool& found)
{
	//get an iterator to the results
	std::unordered_multimap<std::string,DirectoryEntry>::const_iterator entries = m_fileDB.find(filename);
	std::vector<DirectoryEntry> out;

	if(entries == m_fileDB.end() )
	{
		found = false;
		return out;
	}
	else
	{
		found = true;

		for(; entries!= m_fileDB.end(); ++entries)
			out.push_back(entries->second);

		return out;
	}
}



} /* namespace MPCLib */
