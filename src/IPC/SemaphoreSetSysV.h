/*
 * Semaphore.h
 *
 *  Created on: Mar 21, 2014
 *      Author: Darius Malysiak
 */

#ifndef SEMAPHORESETSYSV_H_
#define SEMAPHORESETSYSV_H_

#include "../BaseObject.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>


namespace Lazarus {

class SemaphoreSetSysV: public Lazarus::BaseObject {
public:
	/**
	 * the system default permission (0600) will be used in case of 0
	 */
	SemaphoreSetSysV(int permission = 0);
	virtual ~SemaphoreSetSysV();

	/**
	 * 0 for success, -1 otherwise.
	 */
	int createSemaphoreSet(key_t key, int count = 1, unsigned short int init_value = 1);

	/**
	 * 0 for success, -1 otherwise.
	 */
	int openSemaphoreSet(key_t key);

	/**
	 * 0 for success, -1 otherwise.
	 */
	int closeSemaphoreSet();

	/**
	 * 0 for success, -1 otherwise.
	 */
	int changeSemaphoreValue( short amount, unsigned short semIndex = 0);

	/**
	 * this will destroy the semaphore set immediately. 0 for success, -1 otherwise.
	 */
	int destroySemaphoreSet();

private:
	int m_sem_count;
	int m_sem_id;
	key_t m_key;
	bool m_sem_open;
	int m_permission;
};

} /* namespace Lazarus */

#endif /* SEMAPHORESETSYSV_H_ */
