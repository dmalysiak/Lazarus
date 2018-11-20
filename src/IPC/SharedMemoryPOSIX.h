/*
 * SharedMemoryPOSIX.h
 *
 *  Created on: Jun 3, 2014
 *      Author: Darius Malysiak
 */

#ifndef SHAREDMEMORYPOSIX_H_
#define SHAREDMEMORYPOSIX_H_

#include "SharedMemory.h"
#include <string>
#include <sys/stat.h> /* Defines mode constants */

namespace Lazarus {

/**
 * This class represents a shared memory segment (POSIX) for read/write access with respect to the given permission mask.
 * The user is responsible for memory management, i.e. the destruction of any object belonging to this class won't detach
 * or mark any created or acquired shared memory as disposable! Furthermore no internal file descriptors to the SHM are being kept
 * open.
 * Shared memory segments will be kept under /dev/shm in a tmpfs filesystem.
 */

class SharedMemoryPOSIX: public Lazarus::SharedMemory {
public:

	/**
	 * The given RWX permission (default octal 0600) for UGO is ANDed against the process's umask.
	 * mode_t is usually an unsigned int.
	 */
	SharedMemoryPOSIX(mode_t permission = 0);
	virtual ~SharedMemoryPOSIX();

	/**
	 * Creates (allocates) shared memory. The specified size value might be rounded to a multiple of the systems pagesize.
	 * Yet this rounding is consistent throughout the objects lifetime. Furthermore the memory will be attached to this
	 * process. 0 for success, -1 otherwise.
	 */
	int allocateMemory(const std::string& name, size_t size);

	/**
	 * maps an existing object into process memory.  0 for success, -1 otherwise.
	 */
	int attachMemory(const std::string& name, size_t size);

	/**
	 * detaches memory from process memory.  0 for success, -1 otherwise.
	 */
	int detachMemory();

	/**
	 * Will mark the shared memory as disposable, i.e. once the last process detached (unmapped) the memory it will be freed.
	 * Keep in mind that once the memory has been marked as disposable, the content still may exist yet the name will
	 * be marked as free for the system. Thus all subsequent calls to allocateMemory with the same name will behave
	 * as if there is no memory with this ID, thus the contents will finally be freed (i.e. file will be overwritten in /dev/shm).
	 *  0 for success, -1 otherwise.
	 */
	int markDisposable();

	virtual void* getAttachedMemory();
	virtual size_t getSize();

	/**
	 * This is a special helper function. It will attempt to unlink the given memory, this is useful in the following case:
	 * Consider having mapped some memory with an elegant library, yet the program is terminated via SIGKILL, this will
	 * unmap the shared memory yet it won't call unlink! Thus the memory remains allocated.
	 * Thus a program should always call 'unlinkSHM' before allocating any shared memory, as this method will unlink the memory
	 * which (if really all involved process have been killed) will finally cause a release of it.
	 */
	static int unlinkSHM(const std::string& name);

	/**
	 * Checks if the shared memory object exists. Returns 0 if it exists, -1 otherwise.
	 */
	static int checkExistence(const std::string& name);

private:
	mode_t m_permission;
	unsigned char* mp_data;
	bool m_mapped;
	unsigned long m_size;
	std::string m_name;
};

} /* namespace Lazarus */

#endif /* SHAREDMEMORYPOSIX_H_ */
