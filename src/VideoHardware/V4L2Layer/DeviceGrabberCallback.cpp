/*
 * DeviceGrabberCallback.cpp
 *
 *  Created on: Feb 25, 2014
 *      Author: Darius Malysiak
 */

#include "DeviceGrabberCallback.h"

namespace Lazarus {

DeviceGrabberCallback::DeviceGrabberCallback()
{
	mp_current_image_buffer = NULL;
}

void DeviceGrabberCallback::setImageBuffer(Lazarus::PrimitiveBuffer* current_image_buffer)
{

	mp_current_image_buffer = current_image_buffer;

}

DeviceGrabberCallback::~DeviceGrabberCallback()
{
	// TODO Auto-generated destructor stub
}

} /* namespace SHImageProcessing */
