/*
 * DLLoader.cpp
 *
 *  Created on: 23.07.2014
 *      Author: Darius Malysiak
 */

#include "DLLoader.h"

#include <stdio.h>

namespace Lazarus {

DLLoader::DLLoader(const std::string& lib) {

	m_lib = lib;
	pthread_mutex_init(&m_mutex,NULL);
	mp_hDLL = NULL;
	m_ref_counter = 0;
}

DLLoader::~DLLoader()
{
	pthread_mutex_destroy(&m_mutex);
}

void DLLoader::load()
{
	pthread_mutex_lock(&m_mutex);

	mp_hDLL = dlopen(m_lib.c_str(), RTLD_LAZY|RTLD_GLOBAL);

	if(mp_hDLL == NULL)
	{
		printf("ERROR: library not found!\n");
	}

	m_ref_counter++;

	pthread_mutex_unlock(&m_mutex);
}

void DLLoader::unload()
{
	pthread_mutex_lock(&m_mutex);

	m_ref_counter--;

	if(mp_hDLL != NULL && m_ref_counter == 0)
	{
		dlclose(mp_hDLL);
	}

	pthread_mutex_unlock(&m_mutex);
}

void* DLLoader::getElementAddress(const std::string& name)
{
	return dlsym( mp_hDLL, name.c_str());
}

void* DLLoader::getProcAddress(void* pLibrary, const char* name)
{
	return dlsym( pLibrary, name);
}

void* DLLoader::getHandle()
{
	return mp_hDLL;
}

} /* namespace Lazarus */
