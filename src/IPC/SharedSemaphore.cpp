/*
 * SharedSemaphore.cpp
 *
 *  Created on: Jun 5, 2014
 *      Author: Darius Malysiak
 */

#include "SharedSemaphore.h"

#include <string.h>

namespace Lazarus {

SharedSemaphore::SharedSemaphore(SharedMemory* sm) {

	mp_sm = sm;
	m_initialized = false;
	mp_sem = NULL;

}

SharedSemaphore::~SharedSemaphore() {
	// TODO Auto-generated destructor stub
}


size_t SharedSemaphore::getRequiredSMSize()
{
	return sizeof(sem_t);
}

SharedMemory* SharedSemaphore::getSM()
{
	return mp_sm;
}

int SharedSemaphore::getSemVal()
{
	if(m_initialized == false)
	{
		printf("ERROR: semaphore not loaded\n");
		return 0;
	}

	int val = 0;

	int res = sem_getvalue(mp_sem, &val);

	if(res == -1)
	{
		printf("ERROR: could not get semaphore value\n");
	}

	return val;
}

int SharedSemaphore::createSemaphore(unsigned int sem_value, unsigned char* absolute_address)
{
	if(m_initialized == true)
	{
		printf("ERROR: semaphore already loaded\n");
		return -1;
	}

	//allocate the semaphore

	//copy it into shared memory
	if(absolute_address != NULL)
	{
		mp_sem = (sem_t*) absolute_address;
	}
	else
	{
		mp_sem = (sem_t*) mp_sm->getAttachedMemory();
	}

	//make the semaphore sharable
	int res = sem_init(mp_sem, 1, sem_value);

	if(res == -1)
	{
		printf("ERROR: could not create shared semaphore\n");
		return -1;
	}

	m_initialized = true;

	//printf("sem should be %u and is %d\n",sem_value,getSemVal());

	return 0;
}

int SharedSemaphore::loadSemaphore(unsigned char* absolute_address)
{
	if(m_initialized == true)
	{
		printf("ERROR: semaphore already loaded\n");
		return -1;
	}

	//load the mutex from shared memory shared memory
	if(absolute_address != NULL)
	{
		mp_sem = (sem_t*) absolute_address;
	}
	else
	{
		mp_sem = (sem_t*) mp_sm->getAttachedMemory();
	}

	m_initialized = true;

	return 0;
}

int SharedSemaphore::destroySemaphore()
{
	if(m_initialized == false)
	{
		printf("ERROR: semaphore not loaded yet\n");
		return -1;
	}

	int res = sem_destroy(mp_sem);

	m_initialized = false;

	return res;
}

int SharedSemaphore::incSemaphore()
{
	if(m_initialized == false)
	{
		printf("ERROR: semaphore not loaded yet: lock failed\n");
		return -1;
	}

	int res = sem_post(mp_sem);

	return res;
}

int SharedSemaphore::decSemaphore()
{
	if(m_initialized == false)
	{
		printf("ERROR: semaphore not loaded yet: unlock failed\n");
		return -1;
	}

	int res = sem_wait(mp_sem);

	return res;
}

int SharedSemaphore::destroySemInSHM(SharedMemory* sm, unsigned char* absolute_address)
{
	//get the semaphore from shared memory
	sem_t* sem = NULL;

	if(absolute_address != NULL)
	{
			sem = (sem_t*) absolute_address;
	}
	else
	{
			sem = (sem_t*) sm->getAttachedMemory();
	}

	//attempt to destroy it
	int res = sem_destroy(sem);

	return res;
}

} /* namespace Lazarus */
