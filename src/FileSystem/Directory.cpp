/*
 * Directory.cpp
 *
 *  Created on: 19.02.2013
 *      Author: Darius Malysiak
 */

#include "Directory.h"
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

Directory::Directory(const std::string& directory_path)
{

	mp_filelist = new FastNCSList<DirectoryEntry*>();
	current_iterator = NULL;
	m_directory = directory_path;
}

Directory::~Directory()
{
	//delete each directory entry
	GenNCSListElement<DirectoryEntry*>* list_element = mp_filelist->getFirst();

	//traverse the list
	while(list_element != NULL)
	{
		delete list_element->getm_data();
		list_element = list_element->getmp_neighbor();
	}

	DELETE_NULL_CHECKING(mp_filelist);

}

void Directory::reset()
{
	//delete each directory entry
	GenNCSListElement<DirectoryEntry*>* list_element = mp_filelist->getFirst();

	//traverse the list
	while(list_element != NULL)
	{
		delete list_element->getm_data();
		list_element = list_element->getmp_neighbor();
	}

	DELETE_NULL_CHECKING(mp_filelist);

	mp_filelist = new FastNCSList<DirectoryEntry*>();
	current_iterator = NULL;
}

int Directory::filter_fs_spec_entries_none(const struct dirent* dire)
{

	//filter all . and ..
    if( strncmp(dire->d_name, ".", 1) == 0 || strncmp(dire->d_name, "..", 2) == 0 )
    {
    	return 0;
    }


    return 1;
}

int Directory::filter_fs_spec_entries_files_and_folders(const struct dirent* dire)
{

	//filter all . and ..
    if( strncmp(dire->d_name, ".", 1) == 0 || strncmp(dire->d_name, "..", 2) == 0 )
    {
    	return 0;
    }

    //filter all unknown and directory types
    if( dire->d_type == DT_REG || dire->d_type == DT_DIR )
    {
    	return 1;
    }

    return 0;
}

int Directory::filter_fs_spec_entries_special(const struct dirent* dire)
{

	//filter all . and ..
    if( strncmp(dire->d_name, ".", 1) == 0 || strncmp(dire->d_name, "..", 2) == 0 )
    {
    	return 0;
    }

    //filter all unknown and directory types
    if( dire->d_type == DT_REG || dire->d_type == DT_DIR )
    {
    	return 0;
    }

    return 1;
}

int Directory::filter_fs_spec_entries_files(const struct dirent* dire)
{

	//filter all . and ..
    if( strncmp(dire->d_name, ".", 1) == 0 || strncmp(dire->d_name, "..", 2) == 0 )
    {
    	return 0;
    }

    //filter all unknown and directory types
    if( dire->d_type == DT_REG )
    {
    	return 1;
    }

    return 0;
}

int Directory::filter_fs_spec_entries_folders(const struct dirent* dire)
{
	//filter all . and ..
	if( strncmp(dire->d_name, ".", 1) == 0 || strncmp(dire->d_name, "..", 2) == 0 )
	{
		return 0;
	}

	//filter all unknown and directory types
	if( dire->d_type == DT_DIR )
    {
    	return 1;
    }

    return 0;

}


void Directory::readDirectoryFiles(const std::string& directory_path,const std::string& regex)
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
	DirectoryEntry* dir_entry = NULL;
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
	  dir_entry = new DirectoryEntry(filename,m_directory+"/"+filename);
	  mp_filelist->appendLast(dir_entry);
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

void Directory::readDirectoryFilesAndDirectories(const std::string& directory_path,const std::string& regex)
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
	DirectoryEntry* dir_entry = NULL;
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
	  dir_entry = new DirectoryEntry(filename,m_directory+"/"+filename);
	  mp_filelist->appendLast(dir_entry);
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

void Directory::readDirectoryDirectories(const std::string& directory_path,const std::string& regex)
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
	DirectoryEntry* dir_entry = NULL;
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
	  dir_entry = new DirectoryEntry(filename,m_directory+"/"+filename);
	  mp_filelist->appendLast(dir_entry);
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

void Directory::readDirectory(const std::string& directory_path,const std::string& regex)
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
	DirectoryEntry* dir_entry = NULL;
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
	  dir_entry = new DirectoryEntry(filename,m_directory+"/"+filename);
	  mp_filelist->appendLast(dir_entry);
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

void Directory::readDirectorySpecial(const std::string& directory_path,const std::string& regex)
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
	DirectoryEntry* dir_entry = NULL;
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
	  dir_entry = new DirectoryEntry(filename,m_directory+"/"+filename);
	  mp_filelist->appendLast(dir_entry);
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

FastNCSList<DirectoryEntry*>* Directory::getList()
{
	return mp_filelist;
}

const std::string* Directory::getm_directory() const
{
	return &m_directory;
}

DirectoryEntry* Directory::getNextFile()
{
	DirectoryEntry* output = NULL;

	if(current_iterator != NULL)
	{
		output = current_iterator->getm_data();
		current_iterator = current_iterator->getmp_neighbor();
		return output;
	}

	return output;

}


int Directory::createEmptyFile(const std::string& file, unsigned long long count, const std::string unit)
{
	std::string command;
	command = std::string("dd if=/dev/zero of=") + file + std::string("  bs=") + unit + std::string("  count=")+Toolbox::ulongLongToString(count);
	return system(command.c_str());
}

int Directory::createRandomFile(const std::string& file, unsigned long long count, const std::string unit)
{
	std::string command;
	command = std::string("dd if=/dev/urandom of=") + file + std::string("  bs=") + unit + std::string("  count=")+Toolbox::ulongLongToString(count);
	return system(command.c_str());
}

std::string Directory::getCurrentDirectory()
{
   char cwd[1024];
   if(getcwd(cwd, sizeof(cwd)) == NULL)
   {
	   printf("ERROR: could not get current working directory\n");
   }
   return std::string(cwd);
}


long long int Directory::getFileSize(const std::string& file)
{
	FILE* fp = fopen64(file.c_str(),"rb");

	if(fp == NULL)
	{
		return -1;
	}

	long long int size = 0;

	fseeko64(fp, 0L, SEEK_END);
	size = ftello64(fp);
	fseeko64(fp, 0L, SEEK_SET);

	fclose(fp);

	return size;
}


bool Directory::checkFileExistence(const std::string& file)
{
	if( access( file.c_str(), F_OK ) != -1 )
	{
	    return true;
	}

	return false;
}

int Directory::createFolder(const std::string& folder)
{
	return mkdir(folder.c_str() ,ACCESSPERMS);
}

/**
 * deletes a folder which must be empty, returns 0 if the command was executed successfully, otherwise -1.
 */
int Directory::deleteFolder(const std::string& folder)
{
	return rmdir(folder.c_str());
}

/**
 * Deletes the given file.
 * */
int Directory::deleteFile(const std::string& file)
{

	return unlink(file.c_str());
}

/**
 * Unlinks the given file.
 * */
int Directory::unlinkFile(const std::string& file)
{

	return unlink(file.c_str());
}

/**
 * Renames the given file.
 * */
int Directory::renameFile(const std::string& file, const std::string& new_name)
{

	return rename(file.c_str(),new_name.c_str());
}

/**
 * Creates a symbolic link to the given file.
 * */
int Directory::symLinkFile(const std::string& file, const std::string& link)
{

	return symlink(file.c_str(),link.c_str());
}

/**
 * Creates a symbolic hard link to the given file.
 * */
int Directory::hardLinkFile(const std::string& file, const std::string& link_)
{

	return link(file.c_str(),link_.c_str());
}


/**
 * Moves the given file.
 * */
int Directory::moveFile(const std::string& src, const std::string& dest)
{

	return rename(src.c_str(),dest.c_str());
}


/**
 * Copies the given file.
 * */
int Directory::copyFile(const std::string& src, const std::string& dest)
{

    int in = 0;
	int out = 0;

    if((in = open(src.c_str(), O_RDONLY)) == -1)
    {
        return -1;
    }

    if((out = open(dest.c_str(), O_RDWR | O_CREAT, 0600)) == -1)
    {
        close(in);
        return -1;
    }

    off_t bytesCopied = 0;
    struct stat fileinfo = {0};
    fstat(in, &fileinfo);

    int res = sendfile(out, in, &bytesCopied, fileinfo.st_size);

    close(in);
    close(out);

	return res;
}


std::string Directory::getRegExError(int val)
{
	std::string out;

	switch(val)
	{
	case REG_BADBR:
	    out = "Invalid use of back reference operator.";
	    break;
	case REG_BADPAT:
		out = "Invalid use of pattern operators such as group or list.";
	    break;
	case REG_BADRPT:
		out = "Invalid use of repetition operators such as using '*' as the first character.";
	    break;
	case REG_EBRACE:
		out = "Un-matched brace interval operators.";
	    break;
	case REG_EBRACK:
		out = "Un-matched bracket list operators.";
	    break;
	case REG_ECOLLATE:
		out = "Invalid collating element.";
	    break;
	case REG_ECTYPE:
		out = "Unknown character class name.";
	    break;
	case REG_EEND:
		out = "Nonspecific error. This is not defined by POSIX.2.";
	    break;
	case REG_EESCAPE:
		out = "Trailing backslash.";
	    break;
	case REG_EPAREN:
		out = "Un-matched parenthesis group operators.";
	    break;
	case REG_ERANGE:
		out = "Invalid use of the range operator, e.g., the ending point of the range occurs prior to the starting point.";
	    break;
	case REG_ESIZE:
		out = "Compiled regular expression requires a pattern buffer larger than 64Kb. This is not defined by POSIX.2.";
	    break;
	case REG_ESPACE:
		out = "The regex routines ran out of memory.";
	    break;
	case REG_ESUBREG:
		out = "Invalid back reference to a subexpression";
	    break;

	default:
		break;
	}

	return out;
}

} /* namespace MPCLib */
