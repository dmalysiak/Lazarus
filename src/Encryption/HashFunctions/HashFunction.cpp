/*
 * TomCryptHashWrapper.cpp
 *
 *  Created on: 15.12.2014
 *      Author: Darius Malysiak
 */

#include "HashFunction.h"

namespace Lazarus {

HashFunction::HashFunction()
{

	m_ready = false;
	m_hashsize = 0;
	m_hash_blocksize = 0;
	m_hash_idx = -1;

}

HashFunction::~HashFunction() {
	// TODO Auto-generated destructor stub
}

#ifdef USETOMCRYPT
hash_state* HashFunction::getState()
{
	return &m_hash_state;
}
#endif

unsigned long HashFunction::getHashSize()
{
	return m_hashsize;
}

int HashFunction::getIdx()
{
	return m_hash_idx;
}

int HashFunction::getSingleFileHash(const std::string& input_file, BasicBuffer* output)
{
	printf("ERROR: not implemented\n");

	return -1;
}

BasicBuffer* HashFunction::getSingleHash(BasicBuffer* input)
{
	printf("ERROR: not implemented\n");

	return NULL;

}

int HashFunction::getSingleHash(BasicBuffer* input, BasicBuffer* output)
{
	printf("ERROR: not implemented\n");

	return -1;
}


BasicBuffer* HashFunction::getSingleFileHash(const std::string& input_file)
{
	printf("ERROR: not implemented\n");

	return NULL;
}


int HashFunction::process(BasicBuffer* input)
{
	printf("ERROR: not implemented\n");
	return -1;
}


BasicBuffer* HashFunction::getHash()
{
	printf("ERROR: not implemented\n");

	return NULL;
}


int HashFunction::getHash(BasicBuffer* output)
{
	printf("ERROR: not implemented\n");

	return -1;
}


void HashFunction::reset()
{
	printf("ERROR: not implemented\n");
}

} /* namespace Lazarus */
