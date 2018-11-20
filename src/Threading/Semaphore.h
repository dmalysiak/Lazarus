/*
 * MutexLocker.h
 *
 *  Created on: Aug 18, 2013
 *      Author: Darius Malysiak
 */

#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

#include "../BaseObject.h"

#include <semaphore.h>

namespace Lazarus
{

class Semaphore: public Lazarus::BaseObject {
public:
	Semaphore(int value = 0);
	virtual ~Semaphore();

	void incSem();
	void decSem();
	int getValue();

private:
	sem_t m_mutex;

};

}

#endif /* MUTEXLOCKER_H_ */
