/*
 * Semaphore.cpp
 *
 *  Created on: Mar 21, 2014
 *      Author: Darius Malysiak
 */

#include "SemaphoreSetSysV.h"

namespace Lazarus {

SemaphoreSetSysV::SemaphoreSetSysV(int permission) {

	m_sem_id = -1;
	m_sem_count = 0;
	m_key = 0;
	m_sem_open = false;
	m_permission = permission;

}

SemaphoreSetSysV::~SemaphoreSetSysV() {

}

int SemaphoreSetSysV::createSemaphoreSet(key_t key, int count, unsigned short int init_value)
{
	if(m_sem_open == true)
	{
		printf("ERROR: semaphore set already open\n");
		return -1;
	}

	int res = semget(key, count, IPC_CREAT|IPC_EXCL|m_permission);

	if(res == -1)
	{
		printf("ERROR: could not create semaphore set\n");
		return -1;
	}

	//init the semaphores with default values

	unsigned short int values[count];

	for(int i=0;i<count;++i)
	{
		values[i] = init_value;
	}

	res = semctl(res, 0, SETALL, values);

	if(res == -1)
	{
		printf("ERROR: could not init semaphore set\n");
		return -1;
	}

	m_key = key;
	m_sem_id = res;
	m_sem_open = true;

	return 0;
}

int SemaphoreSetSysV::openSemaphoreSet(key_t key)
{
	if(m_sem_open == true)
	{
		printf("ERROR: semaphore set already open\n");
		return -1;
	}

	int res = semget(key, 0, 0|m_permission );

	if(res == -1)
	{
		printf("ERROR: could not create semaphore set\n");
		return -1;
	}

	m_key = key;
	m_sem_id = res;

	m_sem_open = true;
	return 0;
}

int SemaphoreSetSysV::changeSemaphoreValue( short amount, unsigned short semIndex)
{
	if(m_sem_open == false)
	{
		printf("ERROR: semaphore not opened\n");
		return -1;
	}

	struct sembuf sem;

	sem.sem_num = semIndex;
	sem.sem_op = amount;
	sem.sem_flg = 0;

	int res = semop(m_sem_id, &sem, 1);

	if(res == -1)
	{
		printf("ERROR: could not alter semaphore\n");
		return -1;
	}

	return 0;
}

int SemaphoreSetSysV::destroySemaphoreSet()
{
	if(m_sem_open == false)
	{
		printf("ERROR: semaphore not opened\n");
		return -1;
	}

	int res = shmctl(m_sem_id, IPC_RMID, NULL);

	if(res == -1)
	{
		printf("ERROR: could not destroy semaphore set\n");
		return -1;
	}

	m_sem_open = false;
	return 0;
}

int SemaphoreSetSysV::closeSemaphoreSet()
{
	if(m_sem_open == false)
	{
		printf("ERROR: semaphore not opened\n");
		return -1;
	}

	m_sem_id = -1;
	m_sem_open = false;

	return 0;
}

} /* namespace Lazarus */
