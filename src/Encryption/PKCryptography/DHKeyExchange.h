/*
 * DHKeyExchange.h
 *
 *  Created on: 19.12.2014
 *      Author: Darius Malysiak
 */

#ifndef DHKEYEXCHANGE_H_
#define DHKEYEXCHANGE_H_

#include "../../BaseObject.h"
#include "../../BasicBuffer.h"

#include "../PRNG/PRNG.h"
#include "RSAKey.h"
#include "../HashFunctions/HashFunction.h"

#ifdef USETOMCRYPT
#include <tomcrypt.h>
#endif

#include <string>


namespace Lazarus {

class DHKeyExchange: public BaseObject {
public:
	DHKeyExchange();
	virtual ~DHKeyExchange();

	/*
	 * This will create the private key. 'size' specifies the length of the modulus in bytes.
	 * The key size must lie in the range [96,512], everything above 512 will cause an error!
	 * If the given size (<=512) can not be accepted, the system will round to the nearest
	 * acceptable size (e.g. for 32 it will yield 96)!
	 * Returns -1 in case of errors, 0 otherwise.
	 *
	 * ATTENTION: the prng should ('...must...' read on!) be an uninitialized PRNG but not the SecurePRNG!!!
	 * The reason lies in the internal behavior of libTomCrypt, which will instantiate a PRNG
	 * via the securePRNG. The 'createKey' method will reset the PRNG before it starts (thereby
	 * allowing you to use initialized PRNGs).
	 */
	int createKey(PRNG* prng, int size = 128);

	/*
	 * This will encrypt the given key 'key' with the public params of the current private key.
	 * In a nutshell: A private key is generated, a shared secret is generated with the public
	 * params of the current key. The generated key is exported as a public key and appended to the
	 * plaintext, the shared secret is hashed and the >plaintext< is XORed with it.
	 * Returns NULL in case of errors. The used hash function is also appended to the ciphertext;
	 * (PUBLIC_KEY_OF_RANDOM_PRIVATE_KEY,HASHID,CIPHER)
	 *
	 * ATTENTION: the prng should ('...must...' read on!) be an uninitialized PRNG but not the SecurePRNG!!!
	 * The reason lies in the internal behavior of libTomCrypt, which will instantiate a PRNG
	 * via the securePRNG. The 'createKey' method will reset the PRNG before it starts (thereby
	 * allowing you to use initialized PRNGs).
	 */
	BasicBuffer* encryptKey(PRNG* prng, HashFunction* hf, BasicBuffer* key);

	/*
	 * This will decrypt the given key 'c_key' with the current private key.
	 * In order to successfully decrypt the key it previously used hash function must be
	 * registered before calling this method (i.e. one instance of the desired hash function had to be
	 * created before, yet it does not have to exist anymore)!
	 * Returns NULL in case of errors.
	 *
	 * ATTENTION: the prng should ('...must...' read on!) be an uninitialized PRNG but not the SecurePRNG!!!
	 * The reason lies in the internal behavior of libTomCrypt, which will instantiate a PRNG
	 * via the securePRNG. The 'createKey' method will reset the PRNG before it starts (thereby
	 * allowing you to use initialized PRNGs).
	 */
	BasicBuffer* decryptKey(BasicBuffer* c_key);

	/**
	 * This will essentially compute public_y ^ private x mod p and return the serialized bigInt.
	 * The input must be a public key, otherwise the method will fail.
	 * The output can be used as e.g. input for a hash function whose output in turn can be used as
	 * a password for symmetric ciphers.
	 * Returns NULL in case of errors.
	 */
	BasicBuffer* createSharedSecret(BasicBuffer* public_key);

	/**
	 * Exports the current (i.e. private) key as a serialized buffer.
	 * Returns NULL in case of errors. 'export_as_private' determines if the key should be exported
	 * with x and y!
	 */
	BasicBuffer* exportKey(bool export_as_private = false);

	/**
	 * Imports a new (i.e. private) key as a serialized buffer.
	 * Returns -1 in case of errors, 0 otherwise.
	 */
	int importKey(BasicBuffer* key);

private:
#ifdef USETOMCRYPT
	dh_key m_key;
#endif
	bool m_private_key_ready;
};

} /* namespace Lazarus */

#endif /* DHKEYEXCHANGE_H_ */
