/*
 * SharedMemory.h
 *
 *  Created on: Jun 3, 2014
 *      Author: Darius Malysiak
 */

#ifndef SHAREDMEMORY_H_
#define SHAREDMEMORY_H_

#include "../BaseObject.h"

namespace Lazarus {

class SharedMemory: public Lazarus::BaseObject {
public:
	SharedMemory();
	virtual ~SharedMemory();

	virtual void* getAttachedMemory() = 0;
	virtual size_t getSize() = 0;
	virtual int markDisposable() = 0;
	virtual int detachMemory() = 0;
};

} /* namespace Lazarus */

#endif /* SHAREDMEMORY_H_ */
