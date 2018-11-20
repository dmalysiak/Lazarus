/*
 * FortunaPRNG.cpp
 *
 *  Created on: 16.12.2014
 *      Author: Darius Malysiak
 */

#include "YarrowPRNG.h"
#include "../../Buffer.h"

namespace Lazarus {

YarrowPRNG::YarrowPRNG()
{
#ifdef USETOMCRYPT
	int err = 0;

	//register the prng
	if((err = register_prng(&yarrow_desc)) == -1)
	{
		printf("register_prng error: %s\n", error_to_string(err));
		m_prng_ready = false;
		return;
	}

	/* start it */
	if((err = yarrow_start(&m_state)) != CRYPT_OK)
	{
		printf("PRNG start error: %s\n", error_to_string(err));
		m_prng_ready = false;
		return;
	}

	m_entropy_sealed = false;
	m_prng_ready = true;
	m_prng_idx =  find_prng("yarrow");
#endif
#ifdef NOTOMCRYPT
	printf("ERROR: no tomcrypt support\n");
#endif
}

YarrowPRNG::~YarrowPRNG() {
#ifdef USETOMCRYPT
	yarrow_done(&m_state);
#endif
#ifdef NOTOMCRYPT
	printf("ERROR: no tomcrypt support\n");
#endif

}

int YarrowPRNG::addEntropy(BasicBuffer* entropy_buffer)
{
#ifdef USETOMCRYPT
	//sanity check
	if(m_entropy_sealed == true)
	{
		printf("ERROR: entropy has been sealed.\n");
		return -1;
	}

	if(m_prng_ready != true)
	{
		printf("ERROR: PRNG not initialized.\n");
		return -1;
	}

	int err = 0;

	if ((err = yarrow_add_entropy(entropy_buffer->get_mp_data(), entropy_buffer->get_m_length(), &m_state)) != CRYPT_OK)
	{
		printf("add_entropy error: %s\n", error_to_string(err));
		return -1;
	}


	return 0;
#endif
#ifdef NOTOMCRYPT
	printf("ERROR: no tomcrypt support\n");
	return 0;
#endif
}


int YarrowPRNG::sealEntropy()
{
#ifdef USETOMCRYPT
	//sanity check
	if(m_entropy_sealed == true)
	{
		printf("ERROR: entropy has already been sealed.\n");
		return -1;
	}

	if(m_prng_ready != true)
	{
		printf("ERROR: PRNG not initialized.\n");
		return -1;
	}

	int err = 0;

	/* ready and read */
	if ((err = yarrow_ready(&m_state)) != CRYPT_OK)
	{
		printf("Ready error: %s\n", error_to_string(err));
		return -1;
	}

	m_entropy_sealed = true;

	return 0;
#endif
#ifdef NOTOMCRYPT
	printf("ERROR: no tomcrypt support\n");
	return 0;
#endif
}

int YarrowPRNG::resetPRNG()
{
#ifdef USETOMCRYPT
	int err = 0;

	//terminate the current state
	yarrow_done(&m_state);

	/* start it */
	if((err = yarrow_start(&m_state)) != CRYPT_OK)
	{
		printf("PRNG start error: %s\n", error_to_string(err));
		m_prng_ready = false;
		return -1;
	}

	m_entropy_sealed = false;
	m_prng_ready = true;

	return 0;
#endif
#ifdef NOTOMCRYPT
	printf("ERROR: no tomcrypt support\n");
	return 0;
#endif
}

BasicBuffer* YarrowPRNG::getRN(unsigned long length)
{
#ifdef USETOMCRYPT
	//sanity check
	if(m_entropy_sealed != true)
	{
		printf("ERROR: entropy has not been sealed.\n");
		return NULL;
	}

	if(m_prng_ready != true)
	{
		printf("ERROR: PRNG not initialized.\n");
		return NULL;
	}

	BasicBuffer* output = new BasicBuffer(length);

	unsigned long bytes = yarrow_read(output->get_mp_data(), length, &m_state);

	if(bytes == 0 || bytes != length)
	{
		delete output;
		output = NULL;
	}

	return output;
#endif
#ifdef NOTOMCRYPT
	printf("ERROR: no tomcrypt support\n");
	return NULL;
#endif

}


int YarrowPRNG::getRN(BasicBuffer* output)
{
#ifdef USETOMCRYPT
	//sanity check
	if(m_entropy_sealed != true)
	{
		printf("ERROR: entropy has not been sealed.\n");
		return -1;
	}

	if(m_prng_ready != true)
	{
		printf("ERROR: PRNG not initialized.\n");
		return -1;
	}

	unsigned long bytes = yarrow_read(output->get_mp_data(), output->get_m_length(), &m_state);

	if(bytes == 0 || bytes != output->get_m_length())
	{
		return -1;
	}

	return 0;
#endif
#ifdef NOTOMCRYPT
	printf("ERROR: no tomcrypt support\n");
	return 0;
#endif
}


BasicBuffer* YarrowPRNG::retrieveEntropyState()
{
#ifdef USETOMCRYPT
	//sanity check
	if(m_entropy_sealed != true)
	{
		printf("ERROR: entropy has not been sealed.\n");
		return NULL;
	}

	if(m_prng_ready != true)
	{
		printf("ERROR: PRNG not initialized.\n");
		return NULL;
	}

	int err = 0;

	unsigned long size = 16000;
	unsigned char* serialized_state = new unsigned char[16000];

	if( (err = yarrow_export(serialized_state, &size, &m_state)) == -1 )
	{
		printf("PRNG export error: %s\n", error_to_string(err));
		delete[] serialized_state;
		return NULL;
	}

	//the buffer now owns the state data
	Buffer* output = new Buffer(serialized_state,size,true);
	delete[] serialized_state;

	return output;
#endif
#ifdef NOTOMCRYPT
	printf("ERROR: no tomcrypt support\n");
	return NULL;
#endif
}


int YarrowPRNG::setEntropyState(BasicBuffer* entropy)
{
#ifdef USETOMCRYPT
	//sanity check
	if(m_prng_ready != true)
	{
		printf("ERROR: PRNG not initialized.\n");
		return -1;
	}

	int err = 0;

	//reset the PRNG
	err = resetPRNG();

	if(err == -1)
	{
		printf("ERROR: could not set entropy state due to a failed PRNG reset\n");
		return -1;
	}

	if( (err = yarrow_import(entropy->get_mp_data(), entropy->get_m_length(), &m_state)) == -1 )
	{
		printf("PRNG import error: %s\n", error_to_string(err));
		return -1;
	}

	return 0;
#endif
#ifdef NOTOMCRYPT
	printf("ERROR: no tomcrypt support\n");
	return 0;
#endif
}

} /* namespace Lazarus */
