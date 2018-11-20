/*
 * SharedMemoryToolbox.cpp
 *
 *  Created on: Mar 21, 2014
 *      Author: Darius Malysiak
 */

#include "SharedMemorySysV.h"

#include <stdio.h>

namespace Lazarus {

SharedMemorySysV::SharedMemorySysV(int permission) {

	m_permission = 0600 | permission;
	m_sh_id = -1;
	m_key = 0;
	m_size = 0;
	m_mapped = false;
	mp_attached_memory=NULL;

}

SharedMemorySysV::~SharedMemorySysV() {
	// TODO Auto-generated destructor stub
}

size_t SharedMemorySysV::getSize()
{
	return m_size;
}

int SharedMemorySysV::allocateMemory(key_t key, size_t size )
{
	if(m_mapped == true)
	{
		printf("ERROR: memory already mapped\n");
		return -1;
	}

	int res = shmget(key, size, IPC_CREAT|IPC_EXCL|m_permission );

	if(res == -1)
	{
		printf("ERROR: could not create shared memory\n");
		return -1;
	}

	m_size = size;
	m_key = key;
	m_sh_id = res;

	//attach the memory
	void* res_ = shmat(m_sh_id, 0, 0|m_permission);

	if( res_ == (void*)-1 )
	{
		printf("ERROR: could not attach memory\n");
		return -1;
	}

	mp_attached_memory = (unsigned char*)res_;
	m_mapped = true;

	return 0;
}

int SharedMemorySysV::markDisposable()
{
	if(m_mapped == false)
	{
		printf("ERROR: memory not mapped yet\n");
		return -1;
	}

	int res = shmctl(m_sh_id, IPC_RMID, NULL);

	if(res == -1)
	{
		printf("ERROR: could not create shared memory\n");
		return -1;
	}

	return 0;
}

int SharedMemorySysV::lockMemory()
{
	if(m_mapped == false)
	{
		printf("ERROR: memory not mapped yet\n");
		return -1;
	}

	int res = shmctl(m_sh_id, SHM_LOCK, NULL);

	if(res == -1)
	{
		printf("ERROR: could not lock shared memory\n");
		return -1;
	}

	return 0;
}

int SharedMemorySysV::unlockMemory()
{
	if(m_mapped == false)
	{
		printf("ERROR: memory not mapped yet\n");
		return -1;
	}

	int res = shmctl(m_sh_id, SHM_UNLOCK, NULL);

	if(res == -1)
	{
		printf("ERROR: could not unlock shared memory\n");
		return -1;
	}

	return 0;

}

int SharedMemorySysV::attachMemory(key_t key, size_t size)
{
	if(m_mapped == true)
	{
		printf("ERROR: memory already mapped\n");
		return -1;
	}

	//open the memory
	int res = shmget(key, 0, 0|m_permission  );

	if(res == -1)
	{
		printf("ERROR: could not open shared memory\n");
		return -1;
	}

	m_sh_id = res;
	m_size = size;
	m_key = key;
	m_mapped = true;

	//attach the memory
	void* res_ = shmat(m_sh_id, 0, 0|m_permission);

	if( res_ == (void*)-1 )
	{
		printf("ERROR: could not attach memory\n");
		return -1;
	}

	mp_attached_memory = (unsigned char*)res_;

	return 0;

}

int SharedMemorySysV::detachMemory()
{
	if(m_mapped == false)
	{
		printf("ERROR: memory not mapped yet\n");
		return -1;
	}

	int res = shmdt(mp_attached_memory);

	if(res == -1)
	{
		printf("ERROR: could not attach memory\n");
		return -1;
	}

	mp_attached_memory = NULL;

	return 0;
}

void* SharedMemorySysV::getAttachedMemory()
{
	return mp_attached_memory;
}

} /* namespace SHOCLImageProcessing */
