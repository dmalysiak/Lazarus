/*
 * CommandToolbox.h
 *
 *  Created on: Jul 5, 2014
 *      Author: Darius Malysiak
 */

#ifndef COMMANDTOOLBOX_H_
#define COMMANDTOOLBOX_H_

#include "../BaseObject.h"

#include <string>

namespace Lazarus {

class CommandToolbox: public Lazarus::BaseObject {
public:
	CommandToolbox();
	virtual ~CommandToolbox();

	/*
	 * executes a system command, discards the output. Returns the programs status code in case of success, -1 otherwise.
	 */
	static int executeSystemCommand(const std::string& com);

	/*
	 * executes a system command but waits until its end, returning its output
	 */
	static std::string executeSystemCommandFB(const std::string& com, unsigned int return_buffer_linesize);

	/**
	 * creates a new file, returns 0 if the command was executed (i.e. no indication for success)
	 * Existing file will be 'touched', i.e. their access and modification times will be changed to the current time.
	 */
	static int createFile(const std::string& file);

	/**
	 * Same as 'createFile'
	 */
	static int touchFile(const std::string& file);

	/**
	 * deletes a file, returns 0 if the command was executed (i.e. no indication for success)
	 */
	static int deleteFile(const std::string& file);

	/**
	 * creates a folder and all required parent folders, returns 0 if the command was executed (i.e. no indication for success)
	 * If the folder exists nothing will be done.
	 */
	static int createFolder(const std::string& folder);

	/**
	 * deletes a folder and all subfolders within it, returns 0 if the command was executed (i.e. no indication for success)
	 */
	static int deleteFolder(const std::string& folder);

	static int changeFileOwner(const std::string& file, const std::string& owner, const std::string& group);

	static int changeFileRights(const std::string& file, unsigned char rights);

	/*
	 * appends count zeros (bytes) to the specified file
	 */
	static int stuffFileWithZeros(const std::string& file, unsigned int count);
};

} /* namespace Lazarus */

#endif /* COMMANDTOOLBOX_H_ */
