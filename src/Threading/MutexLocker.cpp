/*
 * MutexLocker.cpp
 *
 *  Created on: Aug 18, 2013
 *      Author: Darius Malysiak
 */

#include "MutexLocker.h"

namespace Lazarus
{

MutexLocker::MutexLocker(pthread_mutex_t* mutex) {

	mp_mutex = mutex;

}

MutexLocker::MutexLocker(const MutexLocker& locker)
{
	mp_mutex = NULL;
}

MutexLocker::~MutexLocker() {

	pthread_mutex_unlock(mp_mutex);

}

void MutexLocker::lock() {

	pthread_mutex_lock(mp_mutex);

}

void MutexLocker::unlock() {

	pthread_mutex_unlock(mp_mutex);

}

}
