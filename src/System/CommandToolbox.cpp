/*
 * CommandToolbox.cpp
 *
 *  Created on: Jul 5, 2014
 *      Author: Darius Malysiak
 */

#include "CommandToolbox.h"
#include "../Toolbox.h"

#include <stdio.h>
#include <stdlib.h>

namespace Lazarus {

CommandToolbox::CommandToolbox() {
	// TODO Auto-generated constructor stub

}

CommandToolbox::~CommandToolbox() {
	// TODO Auto-generated destructor stub
}

int CommandToolbox::executeSystemCommand(const std::string& com)
{
	return system(com.c_str());
}


std::string CommandToolbox::executeSystemCommandFB(const std::string& com, unsigned int return_buffer_linesize)
{
	char out[return_buffer_linesize];
	std::string output;

	/* Open the command for reading. */
	FILE* fp = popen(com.c_str(), "r");
	if(fp == NULL)
	{
		printf("Failed to run command\n" );
		return std::string("ERROR");
	}

	/* Read the output a line at a time - output it. */
	while(fgets(out, return_buffer_linesize, fp) != NULL)
	{
		output += std::string(out);
	}

	/* close */
	pclose(fp);

	return output;
}

int CommandToolbox::createFile(const std::string& file)
{
	std::string command = std::string("touch ")+file;

	return executeSystemCommand(command);
}

int CommandToolbox::touchFile(const std::string& file)
{
	return createFile(file);
}

int CommandToolbox::deleteFile(const std::string& file)
{
	std::string command = std::string("rm -f ")+file;

	return executeSystemCommand(command);
}

int CommandToolbox::createFolder(const std::string& folder)
{
	std::string command = std::string("mkdir -p ")+folder;

	return executeSystemCommand(command);
}

int CommandToolbox::deleteFolder(const std::string& folder)
{
	std::string command = std::string("rmdir -rf ")+folder;

	return executeSystemCommand(command);
}

int CommandToolbox::changeFileOwner(const std::string& file, const std::string& owner, const std::string& group)
{
	std::string command = std::string("chown ")+file+std::string(" ");

	if(owner.size() == 0)
	{
		printf("ERROR: owner unspecified\n");
		return -1;
	}

	command += owner;

	if(group.size() != 0)
	{
		command += std::string(":")+group;
	}

	return executeSystemCommand(command);
}

int CommandToolbox::changeFileRights(const std::string& file, unsigned char rights)
{
	std::string command = std::string("chmod ") + Toolbox::uintToString(rights)+ std::string(" ")+file;

	return executeSystemCommand(command);
}

int CommandToolbox::stuffFileWithZeros(const std::string& file, unsigned int count)
{
	std::string command = std::string("dd if=/dev/zero bs=1 count=") + Toolbox::uintToString(count)+ std::string(" >> ")+file;

	return executeSystemCommand(command);
}

} /* namespace Lazarus */
