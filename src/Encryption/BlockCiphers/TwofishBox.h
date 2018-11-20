/*
 * AESBox.h
 *
 *  Created on: Nov 30, 2014
 *      Author: Darius Malysiak
 */

#ifndef TWOFISHBOX_H_
#define TWOFISHBOX_H_

#include "TomCryptWrapper.h"

#ifdef USETOMCRYPT
#include <tomcrypt.h>
#endif

namespace Lazarus {

class TwofishBox: public Lazarus::TomCryptWrapper{
public:
	/**
	 * The IV size must equal the ciphers block size (i.e. 16 bytes)!
	 * The key size should be 16, 24 or 32 bytes (i.e. at least 16, everything above 32 will be trimmed to 32),
	 * the round count must be 16.
	 */
	TwofishBox(const unsigned char* key, const unsigned char* iv, int keysize, enum CHAINING_MODE chaining_mode = CHAINING_MODE_CTR, int cipher_rounds = 0);
	virtual ~TwofishBox();


private:
	int setupChain(enum CHAINING_MODE chaining_mode);
};

} /* namespace Lazarus */

#endif /* TWOFISHBOX_H_ */
