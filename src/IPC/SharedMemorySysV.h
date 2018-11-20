/*
 * SharedMemory.h
 *
 *  Created on: Mar 21, 2014
 *      Author: Darius Malysiak
 */

#ifndef SHAREDMEMORYSYSV_H_
#define SHAREDMEMORYSYSV_H_

#include "SharedMemory.h"

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

namespace Lazarus {

/*
 * This class represents a shared memory segment (SysV) for read/write access with respect to the given permission mask.
 * The user is responsible for memory management, i.e. the destruction of any object belonging to this class won't detach
 * or mark any created or acquired shared memory as disposable!
 * Listing the shared memory segment can be done via 'ipcs -m'
 */

class SharedMemorySysV: public SharedMemory {
public:
	/*
	 * the system default UGO permission (octal 0600) will be used in case of 0
	 */
	SharedMemorySysV(int permission = 0);
	virtual ~SharedMemorySysV();

	/*
	 * Creates (allocates) new shared memory and attaches it to the process.
	 * Keep in mind that this method will fail if the specified key is already in use.
	 *  0 for success, -1 otherwise.
	 */
	int allocateMemory(key_t key, size_t size );

	/**
	 * will mark the shared memory as disposable, i.e. once the last process detached it will be freed.
	 * 0 for success, -1 otherwise.
	 */
	int markDisposable();

	/**
	 * locks the memory in local address space (as we use shared memory: this locks the memory for all involved processes).
	 * 0 for success, -1 otherwise.
	 */
	int lockMemory();

	/**
	 * unlocks the memory in local address space (as we use shared memory: this unlocks the memory for all involved processes).
	 * 0 for success, -1 otherwise.
	 */
	int unlockMemory();

	/**
	 * opens existing shared memory and map it into local address space.
	 * 0 for success, -1 otherwise.
	 */
	int attachMemory(key_t key, size_t size);

	/**
	 * unmap the memory from local address space.
	 * 0 for success, -1 otherwise.
	 */
	int detachMemory();

	virtual void* getAttachedMemory();

	virtual size_t getSize();

private:
	int m_sh_id;
	key_t m_key;
	size_t m_size;
	bool m_mapped;
	unsigned char* mp_attached_memory;
	int m_permission;
};

}

#endif /* SHAREDMEMORYSYSV_H_ */
