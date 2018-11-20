/*
 * SharedMutexFactory.h
 *
 *  Created on: Jun 4, 2014
 *      Author: Darius Malysiak
 */

#ifndef SHAREDMUTEXFACTORY_H_
#define SHAREDMUTEXFACTORY_H_

#include "../BaseObject.h"
#include "SharedMutex.h"
#include "SharedMemoryPOSIX.h"
#include "SharedMemorySysV.h"

namespace Lazarus {

class SharedMutexFactory: public Lazarus::BaseObject {
public:
	SharedMutexFactory();
	virtual ~SharedMutexFactory();

	//----------------------------- POSIX ------------------------------------

	/*
	 * Will create a new shared mutex, i.e. it will create a new shared memory segment with the given name
	 * and allocate a mutex in it.
	 * ATTENTION: Do not call this multiple times for the same ID as this will result in undefined behavior!
	 */
	static SharedMutex createSharedMutexPOSIX(std::string sm_name);

	/*
	 * Will load a shared mutex from the specified shared memory
	 * ATTENTION: Do not call this multiple times for the same ID as this will result in undefined behavior!
	 */
	static SharedMutex loadSharedMutexPOSIX(std::string sm_name);

	/*
	 * Will delete the shared mutex, i.e. the shared memory containing it
	 * ATTENTION: Do not call this multiple times for the same ID as this will result in undefined behavior!
	 */
	static void freeSharedMutexPOSIX(SharedMutex* mutex);

	//----------------------------- SysV ------------------------------------

	/*
	 * Will create a new shared mutex, i.e. it will create a new shared memory segment with the given name
	 * and allocate a mutex in it.
	 * ATTENTION: Do not call this multiple times for the same ID as this will result in undefined behavior!
	 */
	static SharedMutex createSharedMutexSysV(key_t sm_name);

	/*
	 * Will load a shared mutex from the specified shared memory
	 * ATTENTION: Do not call this multiple times for the same ID as this will result in undefined behavior!
	 */
	static SharedMutex loadSharedMutexSysV(key_t sm_name);

	/*
	 * Will delete the shared mutex, i.e. the shared memory containing it
	 * ATTENTION: Do not call this multiple times for the same ID as this will result in undefined behavior!
	 */
	static void freeSharedMutexSysV(SharedMutex* mutex);
};

} /* namespace Lazarus */

#endif /* SHAREDMUTEXFACTORY_H_ */
