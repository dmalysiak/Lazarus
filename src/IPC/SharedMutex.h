/*
 * SharedMutex.h
 *
 *  Created on: Mar 21, 2014
 *      Author: Darius Malysiak
 */

#ifndef SHAREDMUTEX_H_
#define SHAREDMUTEX_H_

#include "SharedMemory.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <pthread.h>
#include <unistd.h>

#include <atomic>

namespace Lazarus {

/*
 * Represents a shared mutex, the given shared memory object must be already primed (i.e. have the shared memory mapped).
 * Furthermore it is expected that only a single mutex resides in the shared memory, namely in the beginning of it.
 * Yet this can be adjusted by 'absolute_address' in 'createMutex(.)'.
 * Furthermore this class can be primed with a semaphore only once, i.e. it can acquire a semaphore only once via either
 * 'createMutex(.)' or 'loadMutex(.)'. In order to acquire a new mutex with the same object one has to call 'destroySemaphore()'
 * before.
 * Also keep in mind that the destruction of any object of this class won't induce a destruction of the associated semaphore.
 */

class SharedMutex: public Lazarus::BaseObject {
public:
	SharedMutex(SharedMemory* sm);

	/**
	 * Keep in mind that this copy constructor will not copy (i.e. duplicate) any underlying
	 * shared memory.
	 * */
	SharedMutex(const SharedMutex& m);

	virtual ~SharedMutex();

	/**
	 * creates a new mutex in the given shared memory. 0 for success, -1 otherwise.
	 * If NULL is given as an absolute address then the beginning of the shared memory is used.
	 */
	int createMutex(unsigned char* absolute_address = NULL);

	/**
	 * loads a mutex from the given shared memory. 0 for success, -1 otherwise.
	 * If NULL is given as an absolute address then the beginning of the shared memory is used.
	 */
	int loadMutex(unsigned char* absolute_address = NULL);

	/**
	 * destroys the mutex in shared memory. 0 for success, -1 otherwise.
	 */
	int destroyMutex();

	/**
	 * locks the mutex. 0 for success, -1 otherwise.
	 */
	int lockMutex();

	/**
	 * unlocks the mutex. 0 for success, -1 otherwise.
	 */
	int unlockMutex();

	/**
	 * static method which returns the required size of shared memory for holding a shared mutex.
	 */
	static size_t getRequiredSMSize();

	/**
	 * returns a reference to the shared memory where the mutex resides.
	 */
	SharedMemory* getSM();

private:
	std::atomic_bool m_initialized;
	pthread_mutex_t* mp_mutex;
	SharedMemory* mp_sm;
};

} /* namespace Lazarus */

#endif /* SHAREDMUTEX_H_ */
