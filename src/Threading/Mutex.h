/*
 * MutexLocker.h
 *
 *  Created on: Aug 18, 2013
 *      Author: Darius Malysiak
 */

#ifndef MUTEX_H_
#define MUTEX_H_

#include "../BaseObject.h"

#include <pthread.h>

namespace Lazarus
{

class Mutex: public Lazarus::BaseObject {
public:
	Mutex();
	virtual ~Mutex();

	void lock();
	void unlock();

private:
	pthread_mutex_t m_mutex;

};

}

#endif /* MUTEXLOCKER_H_ */
