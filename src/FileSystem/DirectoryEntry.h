/*
 * DirectoryEntry.h
 *
 *  Created on: 19.02.2013
 *      Author: Darius Malysiak
 */

#ifndef DIRECTORYENTRY_H_
#define DIRECTORYENTRY_H_

#include "../BaseObject.h"
#include "../BasicBuffer.h"

#include <sys/stat.h>
#include <string>

namespace Lazarus {

class DirectoryEntry: public BaseObject {
public:

	enum ENTRY_TYPE{ENTRY_TYPE_UNKNOWN, ENTRY_TYPE_BLOCK_DEVICE, ENTRY_TYPE_FILE, ENTRY_TYPE_DIR,
		ENTRY_TYPE_LINK, ENTRY_TYPE_SOCKET, ENTRY_TYPE_FIFO, ENTRY_TYPE_CHAR_DEVICE};

	DirectoryEntry();
	DirectoryEntry(const std::string &filename, const std::string &full_path);
	virtual ~DirectoryEntry();

	/**
	 * Updates the file system information for this entry. Returns -1 in case of errors, 0 otherwise.
	 * An existing instance of this class can be primed for a different file by setting
	 * m_fullpath and m_filename to new values and calling updateInfo() afterwards.
	 * */
	int updateInfo();

	/**
	 * Returns a string which represents the canonical file permissions in octal coding
	 * i.e. ugoUGS with U indicating 'set user ID', G indicating 'set group ID' and
	 * S indicating 'set sticky bit'. The first 9 values may be r,w,x while the last
	 * 3 symbols are either X (true) or - (false).
	 * The returned string has always a length of 12.
	 * */
	static std::string sperm(__mode_t mode);

	/**
	 * Returns the type for the given entry.
	 * */
	static enum ENTRY_TYPE translateType(__mode_t mode);

	/**
	 * Prints the entry's information to the console.
	 * */
	void printInfo();

	/**
	 * Will compute and return the file's MD5 hash. The returned buffer must be freed.
	 * */
	BasicBuffer* getFileMD5() const;

	/**
	 * Will compute and return the file's SHA1 hash. The returned buffer must be freed.
	 * */
	BasicBuffer* getFileSHA1() const;

	/**
	 * Will compute and return the file's SHA256 hash. The returned buffer must be freed.
	 * */
	BasicBuffer* getFileSHA256() const;

	/**
	 * Will compute and return the file's SHA512 hash. The returned buffer must be freed.
	 * */
	BasicBuffer* getFileSHA512() const;

	//the file attributes which can be updated via 'updateInfo'
	std::string m_permissions;
	ENTRY_TYPE m_entry_type;
	struct tm m_modification_time;
	struct tm m_change_time;
	struct tm m_access_time;
	__gid_t m_group_id;
	std::string m_group_name;
	__uid_t m_user_id;
	std::string m_user_name;
	unsigned long long m_modification_time_ns;
	unsigned long long m_access_time_ns;
	unsigned long long m_change_time_ns;
	__nlink_t m_link_count;
	__blkcnt_t m_allocated_blocks;//amount of allocated 512 byte blocks.
	std::string m_fullpath;
	std::string m_filename;
	ino_t m_filenumber;
	//in case of a link this will contain the length of the path-string (which is saved in the link) to the actual file
	__off_t m_filesize;
	//the preferred blocksize for the file, writing smaller fragments to the file may be inefficient
	blksize_t m_preferred_blocksize;
	//in case of links this string will contain the path to the linked file
	std::string m_linked_file;
	//the ID of the device which actually contains the file
	dev_t m_dev;
};

} /* namespace MPCLib */
#endif /* DIRECTORYENTRY_H_ */
