/*
 * SharedSemaphore.h
 *
 *  Created on: Jun 5, 2014
 *      Author: Darius Malysiak
 */

#ifndef SHAREDSEMAPHORE_H_
#define SHAREDSEMAPHORE_H_

#include "SharedMemory.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <unistd.h>

namespace Lazarus {

/**
 * Represents a shared semaphore, the given shared memory object must be already primed (i.e. have the shared memory mapped).
 * Furthermore it is expected by default that only a single semaphore resides in the shared memory, namely in the beginning of it.
 * Yet this can be adjusted by 'absolute_address' in 'createSemaphore(.)'.
 * Furthermore this class can be primed with a semaphore only once, i.e. it can acquire a semaphore only once via either
 * 'createSemaphore(.)' or 'loadSemaphore(.)'. In order to acquire a new semaphore with the same object one has to call
 * 'destroySemaphore()' before.
 * Also keep in mind that the destruction of any object of this class won't induce a destruction of the associated semaphore.
 */

class SharedSemaphore: public Lazarus::BaseObject {
public:
	SharedSemaphore(SharedMemory* sm);
	virtual ~SharedSemaphore();


	/**
	 * creates a new semaphore in the given shared memory with the specified initial value.
	 * If NULL is given as an absolute address then the beginning of the shared memory is used.
	 * 0 for success, -1 otherwise.
	 */
	int createSemaphore(unsigned int sem_value, unsigned char* absolute_address = NULL);

	/**
	 * loads a semaphore from the given shared memory with a given absolute address from the shared
	 * memory, if NULL is given then the beginning of the shared memory is used.
	 * 0 for success, -1 otherwise.
	 */
	int loadSemaphore(unsigned char* absolute_address = NULL);

	/**
	 * destroys the semaphore in shared memory. 0 for success, -1 otherwise.
	 */
	int destroySemaphore();

	/**
	 * increments (unlocks) the semaphores value. 0 for success, -1 otherwise.
	 */
	int incSemaphore();

	/**
	 * decrements (locks) the semaphores value, if the value is already 0 the semaphore will wait until it rises again.
	 * 0 for success, -1 otherwise.
	 */
	int decSemaphore();

	/**
	 * static method which returns the required size of shared memory for holding a shared mutex.
	 */
	static size_t getRequiredSMSize();

	/**
	 * This method will attempt to destroy the shared semaphore located in the given shared memory at the
	 * specified 'absolute_address', if NULL is given then the beginning of the shared memory is used.
	 * 0 for success, -1 otherwise.
	 */
	static int destroySemInSHM(SharedMemory* sm, unsigned char* absolute_address = NULL);

	/**
	 * returns a reference to the shared memory where the mutex resides
	 */
	SharedMemory* getSM();

	/**
	 * Returns the current semaphore value. If the semaphore is locked, then the value returned is either
	 * zero or a negative number whose absolute value represents the number of processes waiting for the semaphore
	 * at some unspecified time during the call.
	 */
	int getSemVal();

private:
	bool m_initialized;
	sem_t* mp_sem;
	SharedMemory* mp_sm;
};

} /* namespace Lazarus */

#endif /* SHAREDSEMAPHORE_H_ */
