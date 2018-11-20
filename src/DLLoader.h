/*
 * DLLoader.h
 *
 *  Created on: 23.07.2014
 *      Author: Darius Malysiak
 */

#ifndef DLLOADER_H_
#define DLLOADER_H_

#include "BaseObject.h"

#include <dlfcn.h>
#include <string>
#include <pthread.h>

namespace Lazarus {

class DLLoader: public BaseObject {
public:
	DLLoader(const std::string& lib);
	virtual ~DLLoader();

	void load();
	void unload();

	void* getHandle();

	/**
	 * Returns the address of the specified 'name' element in the currently loaded library.
	 * It will return NULL if the element couldn't be found.
	 * */
	void* getElementAddress(const std::string& name);

	/**
	 * Returns the address of the specified 'name' element within the library identified by its handle
	 * 'pLibrary'.
	 * It will return NULL if the element couldn't be found.
	 * */
	static void* getProcAddress(void* pLibrary, const char* name);


private:
	void* mp_hDLL;
	std::string m_lib;
	pthread_mutex_t m_mutex;
	unsigned int m_ref_counter;
};

} /* namespace Lazarus */

#endif /* DLLOADER_H_ */
