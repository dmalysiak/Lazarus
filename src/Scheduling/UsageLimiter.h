/*
 * UsageLimiter.h
 *
 *  Created on: Jul 3, 2014
 *      Author: clustro
 */

#ifndef USAGELIMITER_H_
#define USAGELIMITER_H_

#include "../Threading/Thread.h"
#include "Process.h"

#include <unordered_map>

namespace Lazarus {

/*
 * this class enables one to limit available cpu time for a set of processes.
 * Keep in mind that the utilized approach requires one to have
 * kill-permissions for all registered processes.
 * Furthermore the limiter runs in user space, thus it is also limited by the underlying
 * schedule algorithms, i.e. it may exhibit a lack of accuracy.
 * Once the limiter thread has started no processes can be registered anymore!
 */

class UsageLimiter: public Lazarus::Thread {
public:

	/*
	 * The polling interval (unit us) determines how fast the limiter can react to changes of cpu time.
	 * The mode value determines how the cpu time is accumulated:
	 * SHARING_MODE_SET: the cpu time of all registered processes counts, i.e. all process share the same time slot
	 * SHARING_MODE_SINGLE: only a single process is watched by the limiter
	 * The mode can't be changed later!
	 * The value of 'perc' lies in [0,1] and represents the amount of time available in the time slot
	 */
	UsageLimiter(double perc);
	virtual ~UsageLimiter();

	/*
	 * allows the change of the default value for time slot size; the given value represents ns.
	 * Setting a too low value will result in serious computation overhead, since after each
	 * passed time slot the average consumption of the processes will be updated, i.e. the /proc
	 * entries will be read for each process.
	 */
	void setTimeSlotSize(unsigned long long int timeslot_size);

	/*
	 * allows the change of the default value for time slot percentage; the given value represents the percentage in [0,1]
	 */
	void setPerc(double perc);

	/*
	 * add the process to the limiter
	 */
	void registerProcess(pid_t pid);


	/*
	 * start the limiter
	 */
	void run();

	const double m_default_perc = 0.5;
	const unsigned long m_default_time_slot = 100000;//100us

private:
	unsigned long long m_time_slot;
	double m_perc;
	std::unordered_map<pid_t,Process*> m_p_map;
};

} /* namespace SHNetwork */
#endif /* USAGELIMITER_H_ */
