/*
 * CommunicationCallback.cpp
 *
 *  Created on: 07.01.2015
 *      Author: Darius Malysiak
 */

#include "SynchronizationCallback.h"

namespace Lazarus {

SynchronizationCallback::SynchronizationCallback(bool use_semaphore) {

	m_use_semaphore = use_semaphore;

}

SynchronizationCallback::~SynchronizationCallback() {
	// TODO Auto-generated destructor stub
}

} /* namespace Lazarus */
