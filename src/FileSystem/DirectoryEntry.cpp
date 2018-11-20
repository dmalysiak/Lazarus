/*
 * DirectoryEntry.cpp
 *
 *  Created on: 19.02.2013
 *      Author: Darius Malysiak
 */

#include "DirectoryEntry.h"
#include "../Toolbox.h"

#include "../Encryption/HashFunctions/SHA1HashFunction.h"
#include "../Encryption/HashFunctions/SHA256HashFunction.h"
#include "../Encryption/HashFunctions/SHA512HashFunction.h"
#include "../Encryption/HashFunctions/MD5HashFunction.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>

#include <time.h>
#include <locale.h>
#include <langinfo.h>
#include <stdio.h>
#include <stdint.h>
#include <dirent.h>

namespace Lazarus {

DirectoryEntry::DirectoryEntry()
{
	m_filenumber = 0;

	m_entry_type = ENTRY_TYPE_UNKNOWN;

	m_user_id = 0;
	m_group_id = 0;
	m_link_count = 0;

	m_access_time_ns = 0;
	m_modification_time_ns = 0;
	m_change_time_ns = 0;

	m_allocated_blocks = 0;

	m_filesize = 0;
	m_preferred_blocksize = 0;
}

DirectoryEntry::DirectoryEntry(const std::string &filename, const std::string &full_path) {

	m_filename = filename;
	m_fullpath = full_path;
	m_filenumber = 0;

	m_entry_type = ENTRY_TYPE_UNKNOWN;

	m_user_id = 0;
	m_group_id = 0;
	m_link_count = 0;

	m_access_time_ns = 0;
	m_modification_time_ns = 0;
	m_change_time_ns = 0;

	m_allocated_blocks = 0;

	m_filesize = 0;
	m_preferred_blocksize = 0;

	updateInfo();
}

DirectoryEntry::~DirectoryEntry() {
	// TODO Auto-generated destructor stub
}

void DirectoryEntry::printInfo()
{
	printf("File system information:\n");
	printf("%s | %s :\n",m_fullpath.c_str(),m_filename.c_str());

	if(m_entry_type == ENTRY_TYPE_DIR)
		printf("Type: Directory\n");

	if(m_entry_type == ENTRY_TYPE_FILE)
		printf("Type: File\n");

	if(m_entry_type == ENTRY_TYPE_LINK)
		printf("Type: Link\n");

	if(m_entry_type == ENTRY_TYPE_UNKNOWN)
		printf("Type: ???\n");

	if(m_entry_type == ENTRY_TYPE_CHAR_DEVICE)
		printf("Type: Char device\n");

	if(m_entry_type == ENTRY_TYPE_BLOCK_DEVICE)
		printf("Type: Block device\n");

	if(m_entry_type == ENTRY_TYPE_SOCKET)
		printf("Type: Socket\n");

	if(m_entry_type == ENTRY_TYPE_FIFO)
		printf("Type: FIFO\n");

	printf("User:%s/%u Group:%s/%u \n",m_user_name.c_str(),m_user_id,m_group_name.c_str(),m_group_id);
	printf("Permissions: %s\n",m_permissions.c_str());
	printf("Access time: %d:%d:%d.%llu , %d.%d.%d\n",m_access_time.tm_hour,
			m_access_time.tm_min,
			m_access_time.tm_sec,m_access_time_ns,
			m_access_time.tm_mday,
			m_access_time.tm_mon,
			m_access_time.tm_year);
	printf("Change time: %d:%d:%d.%llu , %d.%d.%d\n",m_change_time.tm_hour,
			m_change_time.tm_min,
			m_change_time.tm_sec,m_change_time_ns,
			m_change_time.tm_mday,
			m_change_time.tm_mon,
			m_change_time.tm_year);
	printf("Modification time: %d:%d:%d.%llu , %d.%d.%d\n",m_modification_time.tm_hour,
			m_modification_time.tm_min,
			m_modification_time.tm_sec,m_modification_time_ns,
			m_modification_time.tm_mday,
			m_modification_time.tm_mon,
			m_modification_time.tm_year);
	printf("Device ID: %lu Link count: %lu Serialnr %lu Preferred blocksize: %lu Filesize: %lu Allocated blocks(512 bytes): %lu\n",
			m_dev,m_link_count,m_filenumber,m_preferred_blocksize,m_filesize,m_allocated_blocks);

	//in case of links also print the file to which the link points
	if(m_entry_type == ENTRY_TYPE_LINK)
		printf("Linked file %s\n", m_linked_file.c_str());

}

DirectoryEntry::ENTRY_TYPE DirectoryEntry::translateType(__mode_t mode)
{

	if( (mode & S_IFBLK) == S_IFBLK)
		return ENTRY_TYPE_BLOCK_DEVICE;

	if( (mode & S_IFCHR) == S_IFCHR)
		return ENTRY_TYPE_CHAR_DEVICE;

	if( (mode & S_IFIFO) == S_IFIFO)
		return ENTRY_TYPE_FIFO;

	if( (mode & S_IFREG) == S_IFREG)
		return ENTRY_TYPE_FILE;

	if( (mode & S_IFDIR) == S_IFDIR)
		return ENTRY_TYPE_DIR;

	if( (mode & S_IFLNK) == S_IFLNK)
		return ENTRY_TYPE_LINK;

	if( (mode & S_IFSOCK) == S_IFSOCK)
		return ENTRY_TYPE_SOCKET;

	return ENTRY_TYPE_UNKNOWN;
}

int DirectoryEntry::updateInfo()
{
	m_linked_file = "";

	struct stat statbuf;
	struct passwd* pwd = NULL;
	struct group* grp = NULL;

	/* Get entry's information. */
	if(lstat(m_fullpath.c_str(), &statbuf) == -1)
	{
		printf("ERROR: could not get file system info for %s / %s\n",m_fullpath.c_str(),m_filename.c_str());
		return -1;
	}

	//get the file permissions
	m_permissions = sperm(statbuf.st_mode);

	//get the group
	pwd = getpwuid(statbuf.st_uid);
	if(pwd != NULL)
	{
		m_user_id = pwd->pw_uid;
		m_user_name = std::string(pwd->pw_name);
	}
	else
	{
		m_user_id = 0;
		m_user_name = std::string("");
	}

	grp = getgrgid(statbuf.st_gid);
	if(grp != NULL)
	{
		m_group_id = grp->gr_gid;
		m_group_name = std::string(grp->gr_name);
	}
	else
	{
		m_group_id = 0;
		m_group_name = std::string("");
	}

	//modification time
	m_modification_time = *localtime(&statbuf.st_mtime);
	m_modification_time_ns = statbuf.st_mtim.tv_nsec;

	//last access time
	m_access_time = *localtime(&statbuf.st_atime);
	m_access_time_ns = statbuf.st_atim.tv_nsec;

	//time of last status change
	m_change_time = *localtime(&statbuf.st_ctime);
	m_change_time_ns = statbuf.st_ctim.tv_nsec;

	//add the offset
	m_modification_time.tm_year += 1900;
	m_access_time.tm_year += 1900;
	m_change_time.tm_year += 1900;

	//link count
	m_link_count = statbuf.st_nlink;

	//type of object
	m_entry_type = translateType(statbuf.st_mode);

	//serial number of file
	m_filenumber = statbuf.st_ino;

	//file size
	m_filesize = statbuf.st_size;

	//preferred blocksize
	m_preferred_blocksize = statbuf.st_blksize;

	//allocated blocks
	m_allocated_blocks = statbuf.st_blocks;

	//device id
	m_dev = statbuf.st_dev;

	//in case of links read the path of the file to which the link points
	if(m_entry_type == ENTRY_TYPE_LINK)
	{
		m_linked_file = Toolbox::readFileToString(m_fullpath);
	}

	return 0;
}



std::string DirectoryEntry::sperm(__mode_t mode)
{
    char local_buff[13];
    local_buff[12]='\0';
    int i = 0;

    // user permissions
    if((mode & S_IRUSR) == S_IRUSR)
	{
    	local_buff[i] = 'r';
	}
    else
    {
    	local_buff[i] = '-';
    }
    ++i;

    if((mode & S_IWUSR) == S_IWUSR)
    {
    	local_buff[i] = 'w';
    }
    else
    {
    	local_buff[i] = '-';
    }
    ++i;

    if((mode & S_IXUSR) == S_IXUSR)
    {
    	local_buff[i] = 'x';
    }
    else
    {
    	local_buff[i] = '-';
    }
    ++i;

    // group permissions
    if((mode & S_IRGRP) == S_IRGRP)
    {
    	local_buff[i] = 'r';
    }
    else
	{
    	local_buff[i] = '-';
	}
    ++i;

    if((mode & S_IWGRP) == S_IWGRP)
    {
    	local_buff[i] = 'w';
    }
    else
    {
    	local_buff[i] = '-';
    }
    ++i;

    if((mode & S_IXGRP) == S_IXGRP)
    {
    	local_buff[i] = 'x';
    }
    else
    {
    	local_buff[i] = '-';
    }
    ++i;

    // other permissions
    if((mode & S_IROTH) == S_IROTH)
	{
    	local_buff[i] = 'r';
	}
    else
    {
    	local_buff[i] = '-';
    }
    ++i;

    if((mode & S_IWOTH) == S_IWOTH)
    {
    	local_buff[i] = 'w';
    }
    else
    {
    	local_buff[i] = '-';
    }
    ++i;

    if((mode & S_IXOTH) == S_IXOTH)
	{
    	local_buff[i] = 'x';
	}
    else
    {
    	local_buff[i] = '-';
    }
    ++i;

    //special
    if((mode & S_ISUID) == S_ISUID)
	{
		local_buff[i] = 'X';
	}
	else
	{
		local_buff[i] = '-';
	}
    ++i;

    if((mode & S_ISGID) == S_ISGID)
   	{
   		local_buff[i] = 'X';
   	}
   	else
   	{
   		local_buff[i] = '-';
   	}
    ++i;

    if((mode & S_ISVTX) == S_ISVTX)
   	{
   		local_buff[i] = 'X';
   	}
   	else
   	{
   		local_buff[i] = '-';
   	}
    ++i;

    return std::string(local_buff);
}

BasicBuffer* DirectoryEntry::getFileMD5() const
{
	MD5HashFunction hf;

	return hf.getSingleFileHash(m_fullpath);
}

BasicBuffer* DirectoryEntry::getFileSHA1() const
{
	SHA1HashFunction hf;

	return hf.getSingleFileHash(m_fullpath);
}

BasicBuffer* DirectoryEntry::getFileSHA256() const
{
	SHA256HashFunction hf;

	return hf.getSingleFileHash(m_fullpath);
}

BasicBuffer* DirectoryEntry::getFileSHA512() const
{
	SHA512HashFunction hf;

	return hf.getSingleFileHash(m_fullpath);
}


} /* namespace MPCLib */
