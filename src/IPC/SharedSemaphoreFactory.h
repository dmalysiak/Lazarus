/*
 * SharedSemaphoreFactory.h
 *
 *  Created on: Jun 5, 2014
 *      Author: Darius Malysiak
 */

#ifndef SHAREDSEMAPHOREFACTORY_H_
#define SHAREDSEMAPHOREFACTORY_H_

#include "../BaseObject.h"
#include "SharedSemaphore.h"
#include "SharedMemoryPOSIX.h"
#include "SharedMemorySysV.h"

namespace Lazarus {

class SharedSemaphoreFactory: public Lazarus::BaseObject {
public:
	SharedSemaphoreFactory();
	virtual ~SharedSemaphoreFactory();

	//----------------------------- POSIX ------------------------------------

	/**
	 * Will create a new shared semaphore, i.e. it will create a new shared memory segment with the given name
	 * and allocate a semaphore in it.
	 * ATTENTION: Do not call this multiple times for the same ID as this will result in undefined behavior!
	 */
	static SharedSemaphore createSharedSemaphorePOSIX(std::string sm_name, unsigned int value);

	/**
	 * Will load a shared semaphore from the specified shared memory
	 * ATTENTION: Do not call this multiple times for the same ID as this will result in undefined behavior!
	 */
	static SharedSemaphore loadSharedSemaphorePOSIX(std::string sm_name);

	/**
	 * Will delete the shared semaphore, i.e. the shared memory containing it
	 * ATTENTION: Do not call this multiple times for the same ID as this will result in undefined behavior!
	 */
	static void freeSharedSemaphorePOSIX(SharedSemaphore* semaphore);

	//----------------------------- SysV ------------------------------------

	/**
	 * Will create a new shared semaphore, i.e. it will create a new shared memory segment with the given name
	 * and allocate a semaphore in it.
	 * ATTENTION: Do not call this multiple times for the same ID as this will result in undefined behavior!
	 */
	static SharedSemaphore createSharedSemaphoreSysV(key_t sm_name, unsigned int value);

	/**
	 * Will load a shared semaphore from the specified shared memory
	 * ATTENTION: Do not call this multiple times for the same ID as this will result in undefined behavior!
	 */
	static SharedSemaphore loadSharedSemaphoreSysV(key_t sm_name);

	/**
	 * Will delete the shared semaphore, i.e. the shared memory containing it
	 * ATTENTION: Do not call this multiple times for the same ID as this will result in undefined behavior!
	 */
	static void freeSharedSemaphoreSysV(SharedSemaphore* semaphore);
};

} /* namespace Lazarus */

#endif /* SHAREDSEMAPHOREFACTORY_H_ */
