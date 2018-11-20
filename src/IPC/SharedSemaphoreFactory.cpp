/*
 * SharedSemaphoreFactory.cpp
 *
 *  Created on: Jun 5, 2014
 *      Author: Darius Malysiak
 */

#include "SharedSemaphoreFactory.h"

namespace Lazarus {

SharedSemaphoreFactory::SharedSemaphoreFactory() {
	// TODO Auto-generated constructor stub

}

SharedSemaphoreFactory::~SharedSemaphoreFactory() {
	// TODO Auto-generated destructor stub
}


SharedSemaphore SharedSemaphoreFactory::createSharedSemaphorePOSIX(std::string sm_name, unsigned int value)
{
	Lazarus::SharedMemoryPOSIX shared_memory(0777);
	shared_memory.allocateMemory(sm_name,Lazarus::SharedSemaphore::getRequiredSMSize());

	Lazarus::SharedSemaphore s_mutex(&shared_memory);

	//create the mutex
	s_mutex.createSemaphore(value);

	return s_mutex;
}

SharedSemaphore SharedSemaphoreFactory::loadSharedSemaphorePOSIX(std::string sm_name)
{
	Lazarus::SharedMemoryPOSIX shared_memory(0777);
	shared_memory.attachMemory(sm_name,Lazarus::SharedSemaphore::getRequiredSMSize());

	Lazarus::SharedSemaphore s_mutex(&shared_memory);

	//create the mutex
	s_mutex.loadSemaphore();

	return s_mutex;
}

void SharedSemaphoreFactory::freeSharedSemaphorePOSIX(SharedSemaphore* semaphore)
{
	semaphore->getSM()->detachMemory();
	semaphore->getSM()->markDisposable();
}



SharedSemaphore SharedSemaphoreFactory::createSharedSemaphoreSysV(key_t sm_name, unsigned int value)
{
	Lazarus::SharedMemorySysV shared_memory(0777);
	shared_memory.allocateMemory(sm_name,Lazarus::SharedSemaphore::getRequiredSMSize());

	Lazarus::SharedSemaphore s_mutex(&shared_memory);

	//create the mutex
	s_mutex.createSemaphore(value);

	return s_mutex;
}

SharedSemaphore SharedSemaphoreFactory::loadSharedSemaphoreSysV(key_t sm_name)
{
	Lazarus::SharedMemorySysV shared_memory(0777);
	shared_memory.attachMemory(sm_name,Lazarus::SharedSemaphore::getRequiredSMSize());

	Lazarus::SharedSemaphore s_mutex(&shared_memory);

	//create the mutex
	s_mutex.loadSemaphore();

	return s_mutex;
}

void SharedSemaphoreFactory::freeSharedSemaphoreSysV(SharedSemaphore* semaphore)
{
	semaphore->getSM()->detachMemory();
	semaphore->getSM()->markDisposable();
}

} /* namespace Lazarus */
