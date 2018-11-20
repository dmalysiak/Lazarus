/*
 * RSA.h
 *
 *  Created on: 18.12.2014
 *      Author: Darius Malysiak
 */

#ifndef RSA_H_
#define RSA_H_

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

/**
 * This class is mainly used for key encryption or signature handling via RSA.
 * All padding is done according to the OAEP standard or PKCS v2.1.
 * All signatures will use PSS, i.e. v2.1. If tags should be used, keep in mind that
 * a successful decoding can only be achieved if the tags for encoding and decoding match!
 * Furthermore this class uses Tom's fast math library.
 */

class RSA: public BaseObject {
public:
	RSA();
	virtual ~RSA();

	/**
	 * Generates a random key via the provided PRNG and exponent 'e'.
	 * The prng must be ready for byte drawing! 'bit_length' should
	 * range within [1024,4096] with size steps of 8 bits.
	 *  Returns -1 in case of errors, 0 otherwise.
	 */
	int makeKey(PRNG* prng, int bit_length = 1024, long e = 65537);

	/**
	 * Encrypts the given key and returns the cipher. The prng must be ready for byte drawing!
	 * 'tag' is a string which can be attached to the cipher.
	 * Returns NULL in case of errors.
	 */
	BasicBuffer* encryptKey(PRNG* prng, HashFunction* hf, std::string tag, BasicBuffer* key);

	/**
	 * Decrypts the given key ('cipher_t') and returns the cipher.
	 * 'tag' is a string which can be attached to the cipher (must be identical with the tag used for encryption!).
	 * Returns NULL in case of errors.
	 */
	BasicBuffer* decryptKey(HashFunction* hf, std::string tag, BasicBuffer* cipher_t);

	/**
	 * Signs the given message and returns the signature. The salt size should be between 8 and
	 * 16 bytes, yet more is possible.
	 * Returns NULL in case of errors.
	 */
	BasicBuffer* getSignature(PRNG* prng, HashFunction* hf, BasicBuffer* message, unsigned long salt_size = 8);

	/**
	 * Verifies the signature for the given message. The salt size should be between 8 and 16 bytes,
	 * yet more is possible.
	 * Returns -1 in case of errors/failed verification or 0 in case of success.
	 */
	int verifySignature( HashFunction* hf, BasicBuffer* signature, BasicBuffer* message, unsigned long salt_size=8);

	/**
	 * Exports the key into the given buffer, 'export_as_private' determines if the data should be exported as a
	 * private key or a public one!
	 * The output format is RSAPublicKey and RSAPrivateKey (as defined in ASN.1).
	 */
	int exportKey(BasicBuffer** buffer, bool export_as_private = true);

	/**
	 * Imports the key into the internal variable. Also supports OpenSSL DER formatted public RSA keys.
	 */
	int importKey(BasicBuffer* buffer);

	/**
	 * Returns a wrapped RSA key!
	 * Returns NULL in case of errors.
	 */
#ifdef USETOMCRYPT
	RSAKey* getKey();
#endif
	/**
	 * outputs stats about the generated key
	 */
	void printKey();

private:
#ifdef USETOMCRYPT
	rsa_key	 m_key;
#endif
	long m_key_size;
	bool m_key_ready;

	void printfTFMint(void* data);

};

} /* namespace Lazarus */

#endif /* RSA_H_ */
