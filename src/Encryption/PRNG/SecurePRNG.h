/*
 * SecurePRNG.h
 *
 *  Created on: 16.12.2014
 *      Author: Darius Malysiak
 */

#ifndef SECUREPRNG_H_
#define SECUREPRNG_H_

#include "PRNG.h"

#ifdef USETOMCRYPT
#include <tomcrypt.h>
#endif

#include <string>

namespace Lazarus {

/**
 * This class wraps the secure PRNG, which is essentially nothing more than a /dev/random drainer!
 * Thus this PRNG does not need to be seeded with any kind of entropy.
 */

class SecurePRNG: public PRNG {
public:
	SecurePRNG();
	virtual ~SecurePRNG();

	/**
	 * Returns a buffer with 'length' random entries. Returns NULL in case of errors (e.g. if no bytes could be read).
	 */
	virtual BasicBuffer* getRN(unsigned long length);

	/**
	 * Fill the given buffer completely with 'buffer-size' random entries.
	 * Returns -1 in case of errors (e.g. if no bytes could be read), 0 otherwise.
	 */
	virtual int getRN(BasicBuffer* output);

	/**
	 * This method fills the given PRNG with entropy from /dev/random and seals the entropy pool.
	 * Thus in turn kickstarts the given PRNG! Keep in mind that this method requires the entropy
	 * to be specified in bits rather than bytes!
	 * Returns -1 in case of errors (e.g. if no bytes could be read), 0 otherwise.
	 */
	int kickstartPRNG(PRNG* prng, int entropy_bits);

	/**
	 * Returns a pointer to the PRNGs internal state; in this case always NULL!
	 */
#ifdef USETOMCRYPT
	virtual prng_state* getState();
#endif

};

} /* namespace Lazarus */

#endif /* SECUREPRNG_H_ */
