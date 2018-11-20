/*
 * SharedMutexFactory.cpp
 *
 *  Created on: Jun 4, 2014
 *      Author: Darius Malysiak
 */

#include "SharedMutexFactory.h"

#include <sys/mman.h>

namespace Lazarus {

SharedMutexFactory::SharedMutexFactory() {
	// TODO Auto-generated constructor stub

}

SharedMutexFactory::~SharedMutexFactory() {
	// TODO Auto-generated destructor stub
}

SharedMutex SharedMutexFactory::createSharedMutexPOSIX(std::string sm_name)
{
	Lazarus::SharedMemoryPOSIX shared_memory(0777);
	shared_memory.allocateMemory(sm_name,Lazarus::SharedMutex::getRequiredSMSize());

	Lazarus::SharedMutex s_mutex(&shared_memory);

	//create the mutex
	s_mutex.createMutex();

	return s_mutex;
}

SharedMutex SharedMutexFactory::loadSharedMutexPOSIX(std::string sm_name)
{
	Lazarus::SharedMemoryPOSIX shared_memory(0777);
	shared_memory.attachMemory(sm_name,Lazarus::SharedMutex::getRequiredSMSize());

	Lazarus::SharedMutex s_mutex(&shared_memory);

	//create the mutex
	s_mutex.loadMutex();

	return s_mutex;
}

void SharedMutexFactory::freeSharedMutexPOSIX(SharedMutex* mutex)
{
	mutex->getSM()->detachMemory();
	mutex->getSM()->markDisposable();
}



SharedMutex SharedMutexFactory::createSharedMutexSysV(key_t sm_name)
{
	Lazarus::SharedMemorySysV shared_memory(0777);
	shared_memory.allocateMemory(sm_name,Lazarus::SharedMutex::getRequiredSMSize());

	Lazarus::SharedMutex s_mutex(&shared_memory);

	//create the mutex
	s_mutex.createMutex();

	return s_mutex;
}

SharedMutex SharedMutexFactory::loadSharedMutexSysV(key_t sm_name)
{
	Lazarus::SharedMemorySysV shared_memory(0777);
	shared_memory.attachMemory(sm_name,Lazarus::SharedMutex::getRequiredSMSize());

	Lazarus::SharedMutex s_mutex(&shared_memory);

	//create the mutex
	s_mutex.loadMutex();

	return s_mutex;
}

void SharedMutexFactory::freeSharedMutexSysV(SharedMutex* mutex)
{
	mutex->getSM()->detachMemory();
	mutex->getSM()->markDisposable();
}

} /* namespace Lazarus */
