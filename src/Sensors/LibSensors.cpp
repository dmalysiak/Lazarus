/*
 * LibSensors.cpp
 *
 *  Created on: 06.09.2014
 *      Author: Darius Malysiak
 */

#include "LibSensors.h"

#ifdef USELMSENSORS
#include <sensors/sensors.h>
#endif

namespace Lazarus {

#ifdef USELMSENSORS

LibSensors::MemGuard::~MemGuard()
{
  if( LibSensors::mp_instance != NULL )
  {
	 delete LibSensors::mp_instance;
	 LibSensors::mp_instance = NULL;
  }
}

LibSensors::LibSensors() {

	int res = sensors_init(NULL);

	if(res!=0)
	{
		printf("ERROR: could not init libsensors\n");
		m_initialized = false;
	}
	else
	{
		m_initialized = true;
	}

}

LibSensors::~LibSensors() {

	sensors_cleanup();

}

LibSensors* LibSensors::getInstance()
{
	/*
	 * once the program terminates it will destroy this static object, i.e. call its destructor
	 */
	static MemGuard g;

	if(mp_instance == NULL)
	{
	  mp_instance = new LibSensors();
	}

	return mp_instance;
}

LibSensors* LibSensors::mp_instance = NULL;
#endif

#ifdef NOLMSENSORS


LibSensors::MemGuard::~MemGuard()
{
  if( LibSensors::mp_instance != NULL )
  {
	 delete LibSensors::mp_instance;
	 LibSensors::mp_instance = NULL;
  }
}

LibSensors::LibSensors() {

	m_initialized = false;

}

LibSensors::~LibSensors() {


}

LibSensors* LibSensors::getInstance()
{
	/*
	 * once the program terminates it will destroy this static object, i.e. call its destructor
	 */
	static MemGuard g;

	if(mp_instance == NULL)
	{
	  mp_instance = new LibSensors();
	}

	return mp_instance;
}

LibSensors* LibSensors::mp_instance = NULL;

#endif

} /* namespace SHHW */
