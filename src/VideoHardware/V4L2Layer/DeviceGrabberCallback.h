/*
 * DeviceGrabberCallback.h
 *
 *  Created on: Feb 25, 2014
 *      Author: Darius Malysiak
 */

#ifndef DEVICEGRABBERCALLBACK_H_
#define DEVICEGRABBERCALLBACK_H_

#include "../../Threading/Callback.h"
#include "../../Threading/Thread.h"
#include "../../PrimitiveBuffer.h"

namespace Lazarus {

class DeviceGrabberCallback: public Lazarus::Callback {
public:
	DeviceGrabberCallback();
	virtual ~DeviceGrabberCallback();

	void setImageBuffer(Lazarus::PrimitiveBuffer* current_image_buffer);

	virtual void callbackMethod(Lazarus::Thread* executing_thread = NULL) = 0;

protected:
	Lazarus::PrimitiveBuffer* mp_current_image_buffer;
};

} /* namespace SHImageProcessing */

#endif /* DEVICEGRABBERCALLBACK_H_ */
