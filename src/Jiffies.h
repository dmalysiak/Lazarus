/*
 * Jiffies.h
 *
 *  Created on: Jul 4, 2014
 *      Author: Darius Malysiak
 */

#ifndef JIFFIES_H_
#define JIFFIES_H_

#include "BaseObject.h"

namespace Lazarus {

class Jiffies: public Lazarus::BaseObject {
public:
	Jiffies();
	virtual ~Jiffies();

	/*
	 * translates the given jiffies into nanoseconds
	 */
	static unsigned long long getRealTime(unsigned long long jiffies);
};

} /* namespace Lazarus */

#endif /* JIFFIES_H_ */
