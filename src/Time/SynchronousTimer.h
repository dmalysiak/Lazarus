/*
 * SynchronousTimer.h
 *
 *  Created on: 02.01.2015
 *      Author: Darius Malysiak
 */

#ifndef SYNCHRONOUSTIMER_H_
#define SYNCHRONOUSTIMER_H_

#include "Timer.h"

namespace Lazarus {

/**
 * An synchronous timer will always be blocking (as it will be executed in the current context, i.e. thread)!
 * Furthermore a synchronous timer will always be non-periodic!
 */
class SynchronousTimer: public Timer{
public:

	SynchronousTimer();
	virtual ~SynchronousTimer();

	/**
	 * This will start the timer.
	 */
	virtual void startTimer();

	/**
	 * This will stop the timer.
	 */
	virtual void stopTimer();

private:
	bool m_timer_run;

};

} /* namespace SHNetwork */

#endif /* SYNCHRONOUSTIMER_H_ */
