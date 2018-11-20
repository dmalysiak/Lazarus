/*
 * CipherCascade.h
 *
 *  Created on: 15.12.2014
 *      Author: Darius Malysiak
 */

#ifndef CIPHERCASCADE_H_
#define CIPHERCASCADE_H_

#include "../../BaseObject.h"
#include "../../Buffer.h"
#include "../../DataStructures/KTuple.h"
#include "CipherBox.h"

#include <string>

namespace Lazarus {

class CipherCascade: public BaseObject {
public:
	CipherCascade();
	virtual ~CipherCascade();

	/**
	 * Encrypts the data in 'input' via a cascade of ciphers to 'output'.
	 * Attention: The size of input and output buffers must be aligned according to the largest block size within the provided
	 * ciphers! Furthermore, this size must be divisible by the block size of each remaining cipher!!
	 * Returns -1 in case of errors, 0 otherwise.
	 */
	static int encrypt(KTuple<CipherBox*>* ciphers, Buffer* input, Buffer* output);

	/**
	 * Decrypts the data in 'input' via a cascade of ciphers to 'output'.
	 * Attention: The size of input and output buffers must be aligned according to the largest block size within the provided
	 * ciphers! Furthermore, this size must be divisible by the block size of each remaining cipher!!
	 * Returns -1 in case of errors, 0 otherwise.
	 */
	static int decrypt(KTuple<CipherBox*>* ciphers, Buffer* input, Buffer* output);

	/**
	 * Encrypts the specified file via a cascade of ciphers in-place.
	 * The employed method is not very efficient as the file will be encrypted on-the-disk for each cipher in the cascade.
	 * Returns -1 in case of errors, 0 otherwise.
	 */
	static int encryptFileInPlaceRaw(KTuple<CipherBox*>* ciphers, std::string file);

	/**
	 * Decrypts the specified file via a cascade of ciphers in-place.
	 * The employed method is not very efficient as the file will be decrypted on-the-disk for each cipher in the cascade.
	 * Returns -1 in case of errors, 0 otherwise.
	 */
	static int decryptFileInPlaceRaw(KTuple<CipherBox*>* ciphers, std::string file);
};

} /* namespace Lazarus */

#endif /* CIPHERCASCADE_H_ */
