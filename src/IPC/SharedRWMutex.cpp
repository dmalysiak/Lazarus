/*
 * SharedMutex.cpp
 *
 *  Created on: Mar 21, 2014
 *      Author: Darius Malysiak
 */

#include "SharedRWMutex.h"

#include <string.h>

namespace Lazarus {

#ifdef _POSIX_THREAD_PROCESS_SHARED

SharedRWMutex::SharedRWMutex(SharedMemory* sm)
{
	mp_sm = sm;
	m_initialized = false;
	mp_mutex = NULL;
}

SharedRWMutex::~SharedRWMutex() {
	// TODO Auto-generated destructor stub
}

size_t SharedRWMutex::getRequiredSMSize()
{
	return sizeof(pthread_rwlock_t);
}

SharedMemory* SharedRWMutex::getSM()
{
	return mp_sm;
}

int SharedRWMutex::createMutex(unsigned char* absolute_address)
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
		mp_mutex = (pthread_rwlock_t*) absolute_address;
	}
	else
	{
		mp_mutex = (pthread_rwlock_t*) mp_sm->getAttachedMemory();
	}

	//make the mutex sharable
	pthread_rwlockattr_t mutex_attribute;
	int res = pthread_rwlockattr_init(&mutex_attribute);
	if(res == -1)
	{
		return -1;
	}
	res = pthread_rwlockattr_setpshared(&mutex_attribute,PTHREAD_PROCESS_SHARED);
	if(res == -1)
	{
		return -1;
	}
	res = pthread_rwlock_init(mp_mutex, &mutex_attribute);
	if(res == -1)
	{
		return -1;
	}

	m_initialized = true;

	res = pthread_rwlockattr_destroy(&mutex_attribute);
	if(res == -1)
	{
		printf("ERROR: could not destroy mutex attribute\n");
		return -1;
	}

	return 0;
}

int SharedRWMutex::loadMutex(unsigned char* absolute_address)
{
	if(m_initialized == true)
	{
		printf("ERROR: mutex already loaded\n");
		return -1;
	}

	//load the mutex from shared memory shared memory
	if(absolute_address != NULL)
	{
		mp_mutex = (pthread_rwlock_t*) absolute_address;
	}
	else
	{
		mp_mutex = (pthread_rwlock_t*) mp_sm->getAttachedMemory();
	}

	m_initialized = true;
	return 0;
}

int SharedRWMutex::destroyMutex()
{
	if(m_initialized == false)
	{
		printf("ERROR: mutex not loaded yet\n");
		return -1;
	}

	int res = pthread_rwlock_destroy(mp_mutex);

	m_initialized = false;
	return res;
}

int SharedRWMutex::lockMutexR()
{
	if(m_initialized == false)
	{
		printf("ERROR: mutex not loaded yet: lock failed\n");
		return -1;
	}

	int res = pthread_rwlock_rdlock(mp_mutex);
	return res;
}

int SharedRWMutex::lockMutexW()
{
	if(m_initialized == false)
	{
		printf("ERROR: mutex not loaded yet: lock failed\n");
		return -1;
	}

	int res = pthread_rwlock_rdlock(mp_mutex);
	return res;
}


int SharedRWMutex::unlockMutex()
{
	if(m_initialized == false)
	{
		printf("ERROR: mutex not loaded yet: unlock failed\n");
		return -1;
	}

	int res = pthread_rwlock_unlock(mp_mutex);
	return res;
}


#else
#warning This system does not support process shared mutex

SharedRWMutex::SharedRWMutex(SharedMemory* sm)
{
	mp_sm = sm;
	m_initialized = false;
	mp_mutex = NULL;
}

SharedRWMutex::~SharedRWMutex() {

}

size_t SharedRWMutex::getRequiredSMSize()
{
	return sizeof(pthread_mutex_t);
}

int SharedRWMutex::createMutex()
{
	return -1;
}

int SharedRWMutex::loadMutex()
{
	return -1;
}

int SharedRWMutex::destroyMutex()
{
	return -1;
}

int SharedRWMutex::lockMutexR()
{
	return -1;
}

int SharedRWMutex::lockMutexW()
{
	return -1;
}

int SharedRWMutex::unlockMutex()
{
	return -1;
}

#endif

} /* namespace Lazarus */
