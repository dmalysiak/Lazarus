/*
 * Sober128PRNG.h
 *
 *  Created on: 16.12.2014
 *      Author: Darius Malysiak
 */

#ifndef SOBER128PRNG_H_
#define SOBER128PRNG_H_

#include "PRNG.h"

#ifdef USETOMCRYPT
#include <tomcrypt.h>
#endif

#include <string>

namespace Lazarus {

class Sober128PRNG: public PRNG {
public:
	Sober128PRNG();
	virtual ~Sober128PRNG();

	/**
	 * Adds more entropy to the entropy pool. Returns -1 in case of errors, 0 otherwise.
	 */
	virtual int addEntropy(BasicBuffer* entropy_buffer);

	/**
	 * Seals the entropy pool, this prevents adding any new elements to the pool. Returns -1 in case of errors, 0 otherwise.
	 */
	virtual int sealEntropy();

	/**
	 * This resets the PRNG, i.e. removes all added entropy, sets the PRNG in 'not ready', unseals the entropy pool.
	 * Returns -1 in case of errors, 0 otherwise.
	 */
	virtual int resetPRNG();

	/**
	 * This retrieves the current entropy state. Returns NULL in case of errors.
	 */
	virtual BasicBuffer* retrieveEntropyState();

	/**
	 * This sets the current entropy state. The entropy isn't sealed yet! Returns -1 in case of errors, 0 otherwise.
	 */
	virtual int setEntropyState(BasicBuffer* entropy);


	/**
	 * Returns a buffer with 'length' random entries. Returns NULL in case of errors (e.g. if no bytes could be read).
	 */
	virtual BasicBuffer* getRN(unsigned long length);

	/**
	 * Fill the given buffer completely with 'buffer-size' random entries.
	 * Returns -1 in case of errors (e.g. if no bytes could be read), 0 otherwise.
	 */
	virtual int getRN(BasicBuffer* output);

};

} /* namespace Lazarus */

#endif /* SOBER128PRNG_H_ */
