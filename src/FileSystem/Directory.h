/*
 * Directory.h
 *
 *  Created on: 19.02.2013
 *      Author: Darius Malysiak
 */

#ifndef DIRECTORY_H_
#define DIRECTORY_H_

#include "../BaseObject.h"
#include "../DataStructures/FastNCSList.h"
#include "DirectoryEntry.h"

#include <string>
#include <dirent.h>

namespace Lazarus {

class Directory: public BaseObject {
public:
	Directory(const std::string& directory_path="");
	virtual ~Directory();

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
	 * returns a pointer to the next directory entry, NULL at the end.
	 * Do not free the returned object!
	 */
	DirectoryEntry* getNextFile();

	/**
	 * Returns a pointer to the list of read entries.
	 * */
	FastNCSList<DirectoryEntry*>* getList();

	const std::string* getm_directory() const;


	//-------------------------------------------------

	/**
	 * Deletes the given file. Returns 0 on success, otherwise -1, check errno for more details in case of errors.
	 * */
	static int deleteFile(const std::string& file);

	/**
	 * Unlinks the given file. Returns 0 on success, otherwise -1, check errno for more details in case of errors.
	 * */
	static int unlinkFile(const std::string& file);

	/**
	 * Renames the given file. Returns 0 on success, otherwise -1, check errno for more details in case of errors.
	 * Source and destination must be on the same filesystem.
	 * */
	static int renameFile(const std::string& file, const std::string& new_name);

	/**
	 * Moves the given file. Returns 0 on success, otherwise -1, check errno for more details in case of errors.
	 * Source and destination must be on the same filesystem, use copy and delete in case of different filesystems.
	 * */
	static int moveFile(const std::string& src, const std::string& dest);

	/**
	 * Copies the given file. Returns 0 on success, otherwise -1, check errno for more details in case of errors.
	 * Source and destination must be on the same filesystem.
	 * */
	static int copyFile(const std::string& src, const std::string& dest);

	/**
	 * Creates a new symbolic soft link to the given file. Returns 0 on success, otherwise -1, check errno for more details in case of errors.
	 * */
	static int symLinkFile(const std::string& src, const std::string& dest);

	/**
	 * Creates a new symbolic hard link to the given file. Returns 0 on success, otherwise -1, check errno for more details in case of errors.
	 * */
	static int hardLinkFile(const std::string& src, const std::string& dest);

	/**
	 * Creates an empty file (i.e. filled with NULL). Returns 0 in case of success, -1 otherwise.
	 * Unit may be the count in bytes or e.g. 1M, 1G etc.
	 * */
	static int createEmptyFile(const std::string& file, unsigned long long count, const std::string unit = std::string("1024"));

	/**
	 * Creates a file filled with random data. Returns 0 in case of success, -1 otherwise.
	 * Unit may be the count in bytes or e.g. 1M, 1G etc.
	 * */
	static int createRandomFile(const std::string& file, unsigned long long count, const std::string unit = std::string("1024"));

	/**
	 * Returns the directory of the executable (i.e. not including the executable and no trailing slash).
	 * */
	static std::string getCurrentDirectory();

	/**
	 * Returns true if the specified file exists, otherwise false
	 * */
	static bool checkFileExistence(const std::string& file);

	/**
	 * -1 in case of errors, filesize otherwise.
	 */
	static long long int getFileSize(const std::string& filename);

	/**
	 * creates a folder and all required parent folders, returns 0 if the command was executed successfully, otherwise -1.
	 * If the folder exists nothing will be done. One can check the detailed error be querying errno.
	 * Permissions for the new folder will be 0777.
	 */
	static int createFolder(const std::string& folder);

	/**
	 * deletes a folder which must be empty, returns 0 if the command was executed successfully, otherwise -1.
	 * One can check the detailed error be querying errno.
	 */
	static int deleteFolder(const std::string& folder);

	//---------------------------------------------------------

	static int filter_fs_spec_entries_none(const struct dirent* dire);
	static int filter_fs_spec_entries_files_and_folders(const struct dirent* dire);
	static int filter_fs_spec_entries_files(const struct dirent* dire);
	static int filter_fs_spec_entries_folders(const struct dirent* dire);
	static int filter_fs_spec_entries_special(const struct dirent* dire);


protected:
	std::string m_directory;
	FastNCSList<DirectoryEntry*>* mp_filelist;
	GenNCSListElement<DirectoryEntry*>* current_iterator;


	std::string getRegExError(int val);


	virtual void reset();
};

} /* namespace MPCLib */
#endif /* DIRECTORY_H_ */
