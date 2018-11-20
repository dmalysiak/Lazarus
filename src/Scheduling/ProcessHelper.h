/*
 * ProcessHelper.h
 *
 *  Created on: Jul 3, 2014
 *      Author: Darius Malysiak
 */

#ifndef PROCESSHELPER_H_
#define PROCESSHELPER_H_

#include "../BaseObject.h"
#include "../DataStructures/FastNCSList.h"
#include "Process.h"

namespace Lazarus {

class ProcessHelper: public Lazarus::BaseObject {
public:
	ProcessHelper();
	virtual ~ProcessHelper();

	static FastNCSList<Process*>* getAllProcesses();
};

} /* namespace Lazarus */

#endif /* PROCESSHELPER_H_ */
