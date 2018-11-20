/*
 * Sober128PRNG.cpp
 *
 *  Created on: 16.12.2014
 *      Author: Darius Malysiak
 */

#include "Sober128PRNG.h"
#include "../../Buffer.h"

namespace Lazarus {

Sober128PRNG::Sober128PRNG()
{
#ifdef USETOMCRYPT
	int err = 0;

	//register the prng
	if((err = register_prng(&sober128_desc)) == -1)
	{
		printf("register_prng error: %s\n", error_to_string(err));
		m_prng_ready = false;
		return;
	}

	/* start it */
	if((err = sober128_start(&m_state)) != CRYPT_OK)
	{
		printf("PRNG start error: %s\n", error_to_string(err));
		m_prng_ready = false;
		return;
	}

	m_entropy_sealed = false;
	m_prng_ready = true;
	m_prng_idx =  find_prng("sober128");
#endif
#ifdef NOTOMCRYPT
	printf("ERROR: no tomcrypt support\n");
#endif
}

Sober128PRNG::~Sober128PRNG() {
#ifdef USETOMCRYPT
	sober128_done(&m_state);
#endif
#ifdef NOTOMCRYPT
	printf("ERROR: no tomcrypt support\n");
#endif

}

int Sober128PRNG::addEntropy(BasicBuffer* entropy_buffer)
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

	//this is required as the length must be a multiple of 4 bytes
	if(entropy_buffer->get_m_length() % 4 != 0)
	{
		printf("ERROR: The length of the entropy buffer must be a multiple of 4 bytes.\n");
		return -1;
	}

	int err = 0;

	if ((err = sober128_add_entropy(entropy_buffer->get_mp_data(), entropy_buffer->get_m_length(), &m_state)) != CRYPT_OK)
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


int Sober128PRNG::sealEntropy()
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
	if ((err = sober128_ready(&m_state)) != CRYPT_OK)
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

int Sober128PRNG::resetPRNG()
{
#ifdef USETOMCRYPT
	int err = 0;

	//terminate the current state
	sober128_done(&m_state);

	/* start it */
	if((err = sober128_start(&m_state)) != CRYPT_OK)
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

BasicBuffer* Sober128PRNG::getRN(unsigned long length)
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

	unsigned long bytes = sober128_read(output->get_mp_data(), length, &m_state);

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


int Sober128PRNG::getRN(BasicBuffer* output)
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

	unsigned long bytes = sober128_read(output->get_mp_data(), output->get_m_length(), &m_state);

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


BasicBuffer* Sober128PRNG::retrieveEntropyState()
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

	if( (err = sober128_export(serialized_state, &size, &m_state)) == -1 )
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


int Sober128PRNG::setEntropyState(BasicBuffer* entropy)
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

	if( (err = sober128_import(entropy->get_mp_data(), entropy->get_m_length(), &m_state)) == -1 )
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
