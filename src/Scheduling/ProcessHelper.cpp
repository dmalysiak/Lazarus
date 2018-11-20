/*
 * ProcessHelper.cpp
 *
 *  Created on: Jul 3, 2014
 *      Author: Darius Malysiak
 */

#include "ProcessHelper.h"
#include "../FileSystem/Directory.h"
#include "../FileSystem/DirectoryEntry.h"
#include "../Toolbox.h"

namespace Lazarus {

ProcessHelper::ProcessHelper() {
	// TODO Auto-generated constructor stub

}

ProcessHelper::~ProcessHelper() {
	// TODO Auto-generated destructor stub
}

FastNCSList<Process*>* ProcessHelper::getAllProcesses()
{
	FastNCSList<Process*>* plist = new FastNCSList<Process*>();

	//read all the directories under /proc
	Directory d;

	std::string path("/proc/");
	d.readDirectoryDirectories(path);

	//and iterate over them, saving only those which correspond to a pid
	DirectoryEntry* dir = d.getNextFile();
	const std::string* dir_name;
	Process* p = NULL;

	while(dir != NULL)
	{
		dir_name = &(dir->m_filename);

		//check if the name begins with a number (this is not fool proof)
		if(dir_name->c_str()[0] == '0' || dir_name->c_str()[0] == '1' ||
				dir_name->c_str()[0] == '2' || dir_name->c_str()[0] == '3' ||
				dir_name->c_str()[0] == '4' || dir_name->c_str()[0] == '5' ||
				dir_name->c_str()[0] == '6' || dir_name->c_str()[0] == '7' ||
				dir_name->c_str()[0] == '8' || dir_name->c_str()[0] == '9')
		{
			p = new Process(Toolbox::stringToULong( *dir_name ));

			plist->appendLast(p);
		}

		dir = d.getNextFile();
	}


	return plist;

}

} /* namespace Lazarus */
