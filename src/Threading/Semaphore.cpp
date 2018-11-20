/*
 * MutexLocker.cpp
 *
 *  Created on: Aug 18, 2013
 *      Author: Darius Malysiak
 */

#include "Semaphore.h"

namespace Lazarus
{

Semaphore::Semaphore(int value) {

	sem_init(&m_mutex, 0, value);

}

Semaphore::~Semaphore() {

	sem_post(&m_mutex);
	sem_destroy(&m_mutex);

}

void Semaphore::incSem() {

	sem_post(&m_mutex);

}

void Semaphore::decSem() {

	sem_wait(&m_mutex);

}

int Semaphore::getValue() {

	int value;
    sem_getvalue(&m_mutex, &value);
    return value;

}

}
