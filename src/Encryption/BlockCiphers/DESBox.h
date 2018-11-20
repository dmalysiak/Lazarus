/*
 * AESBox.h
 *
 *  Created on: Nov 30, 2014
 *      Author: Darius Malysiak
 */

#ifndef DESBOX_H_
#define DESBOX_H_

#include "TomCryptWrapper.h"

#ifdef USETOMCRYPT
#include <tomcrypt.h>
#endif

namespace Lazarus {

class DESBox: public Lazarus::TomCryptWrapper {
public:
	/**
	 * The IV size must equal the ciphers block size (i.e. 8 bytes)!
	 * The key size should be at least 8 (everything above 8 will be trimmed to 8),
	 * the round count must be 16.
	 */
	DESBox(const unsigned char* key, const unsigned char* iv, int keysize, enum CHAINING_MODE chaining_mode = CHAINING_MODE_CTR, int cipher_rounds = 0);
	virtual ~DESBox();


private:
	int setupChain(enum CHAINING_MODE chaining_mode);
};

} /* namespace Lazarus */

#endif /* DESBOX_H_ */
