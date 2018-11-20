/*
 * ProcessToolbox.h
 *
 *  Created on: Jul 5, 2014
 *      Author: Darius Malysiak
 */

#ifndef PROCESSTOOLBOX_H_
#define PROCESSTOOLBOX_H_

#include "../BaseObject.h"
#include "../DataStructures/FastKTuple.h"

#include "Process.h"

namespace Lazarus {

class ProcessToolbox: public Lazarus::BaseObject {
public:
	ProcessToolbox();
	virtual ~ProcessToolbox();

	/*
	 * spawns a process and replaces it with the context of exec_file.
	 * Returns a Process object in case of success, NULL otherwise.
	 * The spawned process is a child of the current one.
	 */
	static Process* spawnProcess(const std::string& exec_file, FastKTuple<std::string>* args = NULL);
};

} /* namespace Lazarus */

#endif /* PROCESSTOOLBOX_H_ */
