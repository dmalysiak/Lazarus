/*
 * Frame.cpp
 *
 *  Created on: 19.03.2013
 *      Author: Darius Malysiak
 */

#include "Frame.h"

namespace Lazarus
{

Frame::Frame(GenericConnection* connection) {

	mp_connection = connection;

	m_total_timeout_val = 0;
}

Frame::~Frame() {


}

}
