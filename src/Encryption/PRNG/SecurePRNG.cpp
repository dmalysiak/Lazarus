/*
 * SecurePRNG.cpp
 *
 *  Created on: 16.12.2014
 *      Author: Darius Malysiak
 */

#include "SecurePRNG.h"
#include "../../Buffer.h"

namespace Lazarus {

SecurePRNG::SecurePRNG()
{
#ifdef USETOMCRYPT
	/* register SPRNG */
	if(register_prng(&sprng_desc) == -1)
	{
		printf("Error registering SPRNG\n");
		return;
	}

	m_entropy_sealed = true;
	m_prng_ready = true;
	m_prng_idx =  find_prng("sprng");
#endif
#ifdef NOTOMCRYPT
	printf("ERROR: no tomcrypt support\n");
#endif
}

SecurePRNG::~SecurePRNG() {
#ifdef USETOMCRYPT
	sprng_done(&m_state);
#endif
#ifdef NOTOMCRYPT
	printf("ERROR: no tomcrypt support\n");
#endif

}
#ifdef USETOMCRYPT
prng_state* SecurePRNG::getState()
{
	return NULL;
}
#endif

BasicBuffer* SecurePRNG::getRN(unsigned long length)
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

	unsigned long bytes = sprng_read(output->get_mp_data(), length, &m_state);

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


int SecurePRNG::getRN(BasicBuffer* output)
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
		printf("ERROR: SPRNG not initialized.\n");
		return -1;
	}

	unsigned long bytes = sprng_read(output->get_mp_data(), output->get_m_length(), &m_state);

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

int SecurePRNG::kickstartPRNG(PRNG* prng, int entropy_bits)
{
#ifdef USETOMCRYPT
	int err = 0;

	if( (err = rng_make_prng( entropy_bits, prng->getIdx(), prng->getState(), NULL)) != CRYPT_OK )
	{
		printf("SPRNG make PRNG error: %s\n", error_to_string(err));
		return -1;
	}

	//seal the entropy pool
	prng->sealEntropy();

	return 0;
#endif
#ifdef NOTOMCRYPT
	printf("ERROR: no tomcrypt support\n");
	return 0;
#endif
}



} /* namespace Lazarus */
