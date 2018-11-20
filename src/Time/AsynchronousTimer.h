/*
 * AsynchronousTimer.h
 *
 *  Created on: 02.01.2015
 *      Author: Darius Malysiak
 */

#ifndef ASYNCHRONOUSTIMER_H_
#define ASYNCHRONOUSTIMER_H_

#include "Timer.h"
#include "../Threading/Thread.h"

namespace Lazarus {

/**
 * An asynchronous timer will always be nonblocking (as it will be executed in a new thread)!
 * In case of a periodic execution one has to keep in mind that the total execution time for one
 * period consists of 'time for task' + 'period length', as the timer will stop the underlying system timer,
 * execute the task and restart the timer.
 * This approach ensures that no overruns are happening in case the tasks takes for time than one period length.
 * This can be circumvented by deploying the task in a new thread, but think that over at least twice ...
 */
class AsynchronousTimer: public Timer, public Thread {
public:

	AsynchronousTimer(enum TIMER_TYPE timer_type);
	virtual ~AsynchronousTimer();

	/**
	 * This will start the timer, i.e. the associated thread will start and the timer will be activated
	 */
	virtual void startTimer();

	/**
	 * This will stop the timer, i.e. the associated thread will stop and the timer will be deactivated.
	 * This method will block until the thread has stopped.
	 */
	virtual void stopTimer();

	virtual void preFlag();

	virtual void run();

};

} /* namespace SHNetwork */

#endif /* ASYNCHRONOUSTIMER_H_ */
