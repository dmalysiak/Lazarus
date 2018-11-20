/*
 * PRNG.h
 *
 *  Created on: 16.12.2014
 *      Author: Darius Malysiak
 */

#ifndef PRNG_H_
#define PRNG_H_

#include "../../BaseObject.h"
#include "../../BasicBuffer.h"

#ifdef USETOMCRYPT
#include <tomcrypt.h>
#endif

#include <string>

namespace Lazarus {

class PRNG: public BaseObject {
public:
	PRNG();
	virtual ~PRNG();

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

	/**
	 * Returns a pointer to the PRNGs internal state
	 */
#ifdef USETOMCRYPT
	virtual prng_state* getState();
#endif

	/**
	 * Returns the index of the PRNG
	 */
	int getIdx();


protected:
	int m_prng_idx;
#ifdef USETOMCRYPT
	prng_state m_state;
#endif
	bool m_entropy_sealed;//indicates a sealed entropy!
	bool m_prng_ready;//indicates a ready PRNG, i.e. the state variable has been initialized!
};

} /* namespace Lazarus */

#endif /* PRNG_H_ */
