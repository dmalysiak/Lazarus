/*
 * MutexLocker.cpp
 *
 *  Created on: Aug 18, 2013
 *      Author: Darius Malysiak
 */

#include "Mutex.h"

namespace Lazarus
{

Mutex::Mutex() {

	pthread_mutex_init(&m_mutex,NULL);

}

Mutex::~Mutex() {

	pthread_mutex_unlock(&m_mutex);
	pthread_mutex_destroy(&m_mutex);

}

void Mutex::lock() {

	pthread_mutex_lock(&m_mutex);

}

void Mutex::unlock() {

	pthread_mutex_unlock(&m_mutex);

}

}
