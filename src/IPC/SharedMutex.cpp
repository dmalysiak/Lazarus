/*
 * SharedMutex.cpp
 *
 *  Created on: Mar 21, 2014
 *      Author: Darius Malysiak
 */

#include "SharedMutex.h"

#include <string.h>

namespace Lazarus {

#ifdef _POSIX_THREAD_PROCESS_SHARED

SharedMutex::SharedMutex(SharedMemory* sm)
{
	mp_sm = sm;
	m_initialized = false;
	mp_mutex = NULL;
}

SharedMutex::SharedMutex(const SharedMutex& m)
{
	mp_sm = m.mp_sm;
	m_initialized = m.m_initialized.load();
	mp_mutex = m.mp_mutex;
}

SharedMutex::~SharedMutex() {
	// TODO Auto-generated destructor stub
}

size_t SharedMutex::getRequiredSMSize()
{
	return sizeof(pthread_mutex_t);
}

SharedMemory* SharedMutex::getSM()
{
	return mp_sm;
}

int SharedMutex::createMutex(unsigned char* absolute_address)
{
	if(m_initialized == true)
	{
		printf("ERROR: mutex already loaded\n");
		return -1;
	}

	//allocate the mutex

	//copy it into shared memory
	if(absolute_address != NULL)
	{
		mp_mutex = (pthread_mutex_t*) absolute_address;
	}
	else
	{
		mp_mutex = (pthread_mutex_t*) mp_sm->getAttachedMemory();
	}

	//make the mutex sharable
	pthread_mutexattr_t mutex_attribute;
	int res = pthread_mutexattr_init(&mutex_attribute);
	if(res == -1)
	{
		return -1;
	}
	res = pthread_mutexattr_setpshared(&mutex_attribute,PTHREAD_PROCESS_SHARED);
	if(res == -1)
	{
		return -1;
	}
	res = pthread_mutex_init(mp_mutex, &mutex_attribute);
	if(res == -1)
	{
		return -1;
	}

	m_initialized = true;

	res = pthread_mutexattr_destroy(&mutex_attribute);
	if(res == -1)
	{
		printf("ERROR: could not destroy mutex attribute\n");
		return -1;
	}

	return 0;
}

int SharedMutex::loadMutex(unsigned char* absolute_address)
{
	if(m_initialized == true)
	{
		printf("ERROR: mutex already loaded\n");
		return -1;
	}

	//load the mutex from shared memory shared memory
	if(absolute_address != NULL)
	{
		mp_mutex = (pthread_mutex_t*) absolute_address;
	}
	else
	{
		mp_mutex = (pthread_mutex_t*) mp_sm->getAttachedMemory();
	}

	m_initialized = true;
	return 0;
}

int SharedMutex::destroyMutex()
{
	if(m_initialized == false)
	{
		printf("ERROR: mutex not loaded yet\n");
		return -1;
	}

	int res = pthread_mutex_destroy(mp_mutex);

	m_initialized = false;
	return res;
}

int SharedMutex::lockMutex()
{
	if(m_initialized == false)
	{
		printf("ERROR: mutex not loaded yet: lock failed\n");
		return -1;
	}

	int res = pthread_mutex_lock(mp_mutex);
	return res;
}


int SharedMutex::unlockMutex()
{
	if(m_initialized == false)
	{
		printf("ERROR: mutex not loaded yet: unlock failed\n");
		return -1;
	}

	int res = pthread_mutex_unlock(mp_mutex);
	return res;
}


#else
#warning This system does not support process shared mutex

SharedMutex::SharedMutex(SharedMemory* sm)
{
	mp_sm = sm;
	m_initialized = false;
	mp_mutex = NULL;
}

SharedMutex::~SharedMutex() {

}

size_t SharedMutex::getRequiredSMSize()
{
	return sizeof(pthread_mutex_t);
}

int SharedMutex::createMutex()
{
	return -1;
}

int SharedMutex::loadMutex()
{
	return -1;
}

int SharedMutex::destroyMutex()
{
	return -1;
}

int SharedMutex::lockMutex()
{
	return -1;
}

int SharedMutex::unlockMutex()
{
	return -1;
}

#endif

} /* namespace Lazarus */
