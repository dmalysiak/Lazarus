/*
 * AESBox.h
 *
 *  Created on: Nov 30, 2014
 *      Author: Darius Malysiak
 */

#ifndef RC2BOX_H_
#define RC2BOX_H_

#include "TomCryptWrapper.h"

#ifdef USETOMCRYPT
#include <tomcrypt.h>
#endif

namespace Lazarus {

class RC2Box: public Lazarus::TomCryptWrapper {
public:
	/**
	 * The IV size must equal the ciphers block size (i.e. 8 bytes)!
	 * The key size should be at least 8-128 (at least 8, everything above 128 will be trimmed to 128),
	 * the round count must be 16.
	 */
	RC2Box(const unsigned char* key, const unsigned char* iv, int keysize, enum CHAINING_MODE chaining_mode = CHAINING_MODE_CTR, int cipher_rounds = 0);
	virtual ~RC2Box();


private:
	int setupChain(enum CHAINING_MODE chaining_mode);
};

} /* namespace Lazarus */

#endif /* RC2BOX_H_ */
