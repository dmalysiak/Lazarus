/*
 * ThreadGridStructure.h
 *
 *  Created on: 23.01.2013
 *      Author: Darius Malysiak
 */

#ifndef THREADBLOCKGRIDSTRUCTURE_H_
#define THREADBLOCKGRIDSTRUCTURE_H_

#include "../GridStructure.h"

namespace Lazarus
{

class ThreadBlockGridStructure: public GridStructure {
public:
	ThreadBlockGridStructure();
	virtual ~ThreadBlockGridStructure();

	CLASS_RTTI(Lazarus::ThreadBlockGridStructure)

private:
	unsigned int m_thread_count;
};

}

#endif /* THREADBLOCKGRIDSTRUCTURE_H_ */
