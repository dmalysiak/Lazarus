/*
 * SharedMemoryPOSIX.cpp
 *
 *  Created on: Jun 3, 2014
 *      Author: Darius Malysiak
 */

#include "SharedMemoryPOSIX.h"

#include <fcntl.h> /* Defines O_* constants */

#include <sys/mman.h> //required for mapping memory segments
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

namespace Lazarus {

SharedMemoryPOSIX::SharedMemoryPOSIX(mode_t permission) {

	m_permission = 0600 | permission;
	mp_data = NULL;
	m_size = 0;
	m_mapped = false;

}

SharedMemoryPOSIX::~SharedMemoryPOSIX() {
	// TODO Auto-generated destructor stub
}

void* SharedMemoryPOSIX::getAttachedMemory()
{
	return mp_data;
}

size_t SharedMemoryPOSIX::getSize()
{
	return m_size;
}

int SharedMemoryPOSIX::allocateMemory(const std::string& name, size_t size)
{

	if(m_mapped == true)
	{
		printf("ERROR: memory already mapped\n");
		return -1;
	}

	//O_EXCL ensures a fail in case of an existing descriptor
	int res = shm_open(name.c_str() , O_CREAT | O_EXCL | O_RDWR, m_permission);

	if(res == -1)
	{
		printf("ERROR: could not create POSIX shared memory\n");
		return -1;
	}

	//truncate the created memory to the correct size
	int res_ = ftruncate(res, size);

	if(res_ == -1)
	{
		printf("ERROR: could not truncate POSIX shared memory\n");

		//unlink the memory
		res_ = shm_unlink(name.c_str());

		if(res_ == -1)
		{
			printf("ERROR: FATAL -> SOMETHING WENT HORRIBLY WRONG DURING ALLOCATION OF SHARED MEMORY\n");
		}

		close(res);
		return -1;
	}

	//map the shared memory into process memory
	mp_data = (unsigned char*)mmap(NULL, size , PROT_READ | PROT_WRITE , MAP_SHARED , res, 0);

	if( (void*)mp_data == MAP_FAILED)
	{
		printf("ERROR: could not map POSIX shared memory \n");

		//unlink the memory
		res_ = shm_unlink(name.c_str());

		if(res_ == -1)
		{
			printf("ERROR: FATAL -> SOMETHING WENT HORRIBLY WRONG DURING ALLOCATION OF SHARED MEMORY\n");
		}

		close(res);
		return -1;
	}

	m_mapped = true;
	m_name = name;
	m_size = size;
	close(res);

	return 0;
}

int SharedMemoryPOSIX::attachMemory(const std::string& name, size_t size)
{
	if(m_mapped == true)
	{
		printf("ERROR: memory already mapped\n");
		return -1;
	}

	//open shared memory
	int res = shm_open(name.c_str(), O_RDWR, 0);

	if(res == -1)
	{
		printf("ERROR: could not open POSIX shared memory\n");
		return -1;
	}

	//truncate the created memory to the correct size
	int res_ = ftruncate(res, size);

	if(res_ == -1)
	{
		printf("ERROR: could not truncate POSIX shared memory\n");
		close(res);
		return -1;
	}

	//map the shared memory into process memory
	mp_data = (unsigned char*)mmap(NULL, size , PROT_READ | PROT_WRITE , MAP_SHARED , res, 0);

	if( (void*)mp_data == MAP_FAILED)
	{
		printf("ERROR: could not map POSIX shared memory \n");
		close(res);
		return -1;
	}

	m_mapped = true;
	m_name = name;
	m_size = size;
	close(res);

	return 0;
}

int SharedMemoryPOSIX::checkExistence(const std::string& name)
{
	int res = shm_open(name.c_str(), O_RDONLY, 0);

	if(res == -1)
	{
		return -1;
	}

	close(res);//in case of success, close the file descriptor again.

	return 0;
}

int SharedMemoryPOSIX::detachMemory()
{
	if(m_mapped == false)
	{
		printf("ERROR: shared memory has not been mapped yet\n");
	}

	int res = munmap(mp_data , m_size);

	if(res == -1)
	{
		printf("ERROR: could not unmap POSIX shared memory\n");
		return -1;
	}

	m_mapped = false;

	return 0;
}

int SharedMemoryPOSIX::unlinkSHM(const std::string& name)
{
	//unlink the memory
	int res_ = shm_unlink(name.c_str());

	if(res_ == -1)
	{
		printf("ERROR: could not unlink shared posix memory\n");
		return -1;
	}

	return 0;
}

int SharedMemoryPOSIX::markDisposable()
{
	//unlink the memory
	int res_ = shm_unlink(m_name.c_str());

	if(res_ == -1)
	{
		printf("ERROR: could not unlink shared posix memory\n");
		return -1;
	}

	return 0;
}

} /* namespace Lazarus */
