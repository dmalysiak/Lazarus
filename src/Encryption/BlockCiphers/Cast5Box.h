/*
 * AESBox.h
 *
 *  Created on: Nov 30, 2014
 *      Author: Darius Malysiak
 */

#ifndef CAST5BOX_H_
#define CAST5BOX_H_

#include "TomCryptWrapper.h"

#ifdef USETOMCRYPT
#include <tomcrypt.h>
#endif

namespace Lazarus {

class Cast5Box: public Lazarus::TomCryptWrapper {
public:
	/**
	 * The IV size must equal the ciphers block size (i.e. 8 bytes)!
	 * The key size should be at least 5-16 (at least 5, everything above 16 will be trimmed to 16),
	 * the round count must be 12 or 16.
	 */
	Cast5Box(const unsigned char* key, const unsigned char* iv, int keysize, enum CHAINING_MODE chaining_mode = CHAINING_MODE_CTR, int cipher_rounds = 0);
	virtual ~Cast5Box();


private:
	int setupChain(enum CHAINING_MODE chaining_mode);
};

} /* namespace Lazarus */

#endif /* CAST5BOX_H_ */
