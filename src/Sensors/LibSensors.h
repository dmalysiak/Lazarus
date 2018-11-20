/*
 * LibSensors.h
 *
 *  Created on: 06.09.2014
 *      Author: Darius Malysiak
 */

#ifndef LIBSENSORS_H_
#define LIBSENSORS_H_

#include "../BaseObject.h"

namespace Lazarus {

class LibSensors: public Lazarus::BaseObject {
public:
	static LibSensors* getInstance();

private:
	static LibSensors* mp_instance;
	bool m_initialized;
	LibSensors();
	LibSensors(const LibSensors& ls);
	virtual ~LibSensors();

	class MemGuard: public Lazarus::BaseObject
	{
	public:
		~MemGuard();
	};


	friend class MemGuard;
};

} /* namespace SHHW */

#endif /* LIBSENSORS_H_ */
