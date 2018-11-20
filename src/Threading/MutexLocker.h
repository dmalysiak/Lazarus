/*
 * MutexLocker.h
 *
 *  Created on: Aug 18, 2013
 *      Author: Darius Malysiak
 */

#ifndef MUTEXLOCKER_H_
#define MUTEXLOCKER_H_

#include "../BaseObject.h"

#include <pthread.h>

namespace Lazarus
{

class MutexLocker: public Lazarus::BaseObject {
public:
	MutexLocker(pthread_mutex_t* mp_mutex);
	MutexLocker(const MutexLocker& locker);
	virtual ~MutexLocker();

	void lock();
	void unlock();

private:
	pthread_mutex_t* mp_mutex;

};

}

#endif /* MUTEXLOCKER_H_ */
