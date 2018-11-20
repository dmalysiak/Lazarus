/*
 * MutexLocker.h
 *
 *  Created on: Aug 18, 2013
 *      Author: Darius Malysiak
 */

#ifndef RWLOCK_H_
#define RWLOCK_H_

#include "../BaseObject.h"

#include <pthread.h>

namespace Lazarus
{

class ReadWriteLock: public Lazarus::BaseObject {
public:
	ReadWriteLock();
	virtual ~ReadWriteLock();

	void rlock();
	void wlock();
	void unlock();

private:
	pthread_rwlock_t m_mutex;

};

}

#endif /* MUTEXLOCKER_H_ */
