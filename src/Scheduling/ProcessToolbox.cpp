/*
 * ProcessToolbox.cpp
 *
 *  Created on: Jul 5, 2014
 *      Author: Darius Malysiak
 */

#include "ProcessToolbox.h"

#include <unistd.h>
#include <signal.h>

#ifdef OSX
#include <stdlib.h>
#endif

namespace Lazarus {

ProcessToolbox::ProcessToolbox() {
	// TODO Auto-generated constructor stub

}

ProcessToolbox::~ProcessToolbox() {
	// TODO Auto-generated destructor stub
}

Process* ProcessToolbox::spawnProcess(const std::string& exec_file, FastKTuple<std::string>* args)
{
	Process* proc = NULL;

	if(args != NULL)
	{
		//prepare the argument list (number of elements in tuple + 1 for the NULL terminator and 1 for the file path)
		char** args_ = (char**)malloc(( args->getm_size() + 2) * sizeof(char*));

		for(unsigned int i=0;i<args->getm_size();++i)
		{
			args_[i+1] = (char*)malloc( ( args->getElementRef(i).size() +1) * sizeof(char) ); //allocate memory
			memcpy(args_[i+1], args->getElementRef(i).c_str(), args->getElementRef(i).size()); //copy the original string
			args_[i+1][args->getElementRef(i).size()] = '\0'; //terminate the string
		}

		//the first argument is the actual file path
		args_[0] = (char*)malloc( (exec_file.size()+1) * sizeof(char) );
		memcpy(args_[0],exec_file.c_str(),exec_file.size());
		args_[0][exec_file.size()] = '\0';

		//the required NULL termination for the array
		args_[args->getm_size()+1] = NULL;

		//setting the signal will prevent the child process to enter a defunct/zombie state after its termination
		signal(SIGCHLD, SIG_IGN);

		//spawn the child process
		pid_t child = fork();

		if(child < 0)
		{
			printf("ERROR: could not fork\n");
		}
		else if(child == 0) //child process
		{
			int ret = execvp(exec_file.c_str(), args_);
			//if we are here there was an error, show it
			perror("ERROR: could not replace spawned context");
			printf("could not execute %s\n",exec_file.c_str());
			exit(ret);
		}
		else //parent process
		{
			//free memory
			for(unsigned int i=0;i<args->getm_size()+1;++i)
			{
				free(args_[i]);
			}
			free(args_);

			//prepare the new process object (child contains the PID of the child process)
			proc = new Process(child);
		}

	}
	else // no args
	{
		//prepare the argument list (number of elements in tuple + 1 for the NULL terminator and 1 for the file path)
		char** args_ = (char**)malloc(2 * sizeof(char*));

		//the first argument is the actual file path
		args_[0] = (char*)malloc( (exec_file.size()+1) * sizeof(char) );
		memcpy(args_[0],exec_file.c_str(),exec_file.size());
		args_[0][exec_file.size()] = '\0';

		//the required NULL termination for the array
		args_[1] = NULL;

		//spawn the child process
		pid_t child = fork();

		if(child < 0)
		{
			printf("ERROR: could not fork\n");
		}
		else if(child == 0) //child process
		{
			int ret = execvp(exec_file.c_str(), args_);
			//if we are here there was an error, show it
			perror("ERROR: could not replace spawned context with");
			printf("could not execute %s\n",exec_file.c_str());
			exit(ret);
		}
		else //parent process
		{
			//free memory
			free(args_[0]);//args_[0] is NULL and does not have to be freed

			free(args_);

			//prepare the new process object (child contains the PID of the child process)
			proc = new Process(child);
		}
	}

	return proc;
}

} /* namespace Lazarus */
