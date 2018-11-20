/*
 * PRNG.cpp
 *
 *  Created on: 16.12.2014
 *      Author: Darius Malysiak
 */

#include "PRNG.h"

namespace Lazarus {

PRNG::PRNG()
{
	m_entropy_sealed = false;
	m_prng_ready = false;
	m_prng_idx = -1;
}

PRNG::~PRNG() {


}
#ifdef USETOMCRYPT
prng_state* PRNG::getState()
{
	return &m_state;
}
#endif

int PRNG::getIdx()
{
	return m_prng_idx;
}

int PRNG::addEntropy(BasicBuffer* entropy_buffer)
{
	printf("ERROR: not implemented.\n");
	return -1;
}


int PRNG::sealEntropy()
{
	printf("ERROR: not implemented.\n");
	return -1;
}

int PRNG::resetPRNG()
{
	printf("ERROR: not implemented.\n");
	return -1;
}

BasicBuffer* PRNG::getRN(unsigned long length)
{
	printf("ERROR: not implemented.\n");
	return NULL;
}


int PRNG::getRN(BasicBuffer* output)
{
	printf("ERROR: not implemented.\n");
	return -1;
}


BasicBuffer* PRNG::retrieveEntropyState()
{
	printf("ERROR: not implemented.\n");
	return NULL;
}


int PRNG::setEntropyState(BasicBuffer* entropy)
{
	printf("ERROR: not implemented.\n");
	return -1;
}

} /* namespace Lazarus */
