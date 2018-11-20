/*
 * Jiffies.cpp
 *
 *  Created on: Jul 4, 2014
 *      Author: Darius Malysiak
 */

#include "Jiffies.h"

#ifdef LINUX
#include <sys/sysinfo.h>
#endif

#include <unistd.h>

namespace Lazarus {

Jiffies::Jiffies() {
	// TODO Auto-generated constructor stub

}

Jiffies::~Jiffies() {
	// TODO Auto-generated destructor stub
}


unsigned long long Jiffies::getRealTime(unsigned long long jiffies)
{
	unsigned long long time = 0;
	long int user_hz = sysconf(_SC_CLK_TCK);

	time = (long double)jiffies * ((long double)1000000000.0/(long double)user_hz); // jiffies * s/jiffie * 1000000000 = jiffies * ns/jiffie

	return time;
}

} /* namespace Lazarus */
