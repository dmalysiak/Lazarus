/*
 * MutexLocker.cpp
 *
 *  Created on: Aug 18, 2013
 *      Author: Darius Malysiak
 */

#include "ReadWriteLock.h"

namespace Lazarus
{

ReadWriteLock::ReadWriteLock() {

	pthread_rwlock_init(&m_mutex,NULL);

}

ReadWriteLock::~ReadWriteLock() {

	pthread_rwlock_unlock(&m_mutex);
	pthread_rwlock_destroy(&m_mutex);

}

void ReadWriteLock::rlock() {

	pthread_rwlock_rdlock(&m_mutex);

}

void ReadWriteLock::wlock() {

	pthread_rwlock_wrlock(&m_mutex);

}

void ReadWriteLock::unlock() {

	pthread_rwlock_unlock(&m_mutex);

}

}
