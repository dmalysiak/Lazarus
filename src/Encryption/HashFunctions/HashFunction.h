/*
 * TomCryptHashWrapper.h
 *
 *  Created on: 15.12.2014
 *      Author: Darius Malysiak
 */

#ifndef TOMCRYPTHASHWRAPPER_H_
#define TOMCRYPTHASHWRAPPER_H_

#include "../../BaseObject.h"
#include "../../BasicBuffer.h"

#ifdef USETOMCRYPT
#include <tomcrypt.h>
#endif

#include <string>

namespace Lazarus {

/**
 * ATTENTION: The hash classes are not fully thread-safe:
 * All hash ops are thread safe, yet the initialization of a hash within a constructor is not
 * thread safe! Thus one should instantiate all objects before any multithreaded application!
 */

class HashFunction: public BaseObject {
public:
	HashFunction();
	virtual ~HashFunction();

	//------------------------------- Single element hashing

	/**
	 * Returns the hash for the given input. In case of errors NULL is returned.
	 */
	virtual BasicBuffer* getSingleHash(BasicBuffer* input);

	/**
	 * Writes the hash for the given input into the output buffer.
	 * The output buffer must be large enough to contain the hash.
	 * In case of errors -1 is returned, 0 otherwise.
	 */
	virtual int getSingleHash(BasicBuffer* input, BasicBuffer* output);

	/**
	 * returns the hash value for a given file, uses a cache for performance reasons. In case of errors NULL is returned.
	 */
	virtual BasicBuffer* getSingleFileHash(const std::string& input_file);

	/**
	 * returns the hash value for a given file, uses a cache for performance reasons.
	 * The output buffer must be large enough to contain the hash.
	 * In case of errors -1 is returned, 0 otherwise.
	 */
	virtual int getSingleFileHash(const std::string& input_file, BasicBuffer* output);

	//------------------------------- Continuous hashing

	/**
	 * Returns the hash for the given input. In case of errors -1 is returned, otherwise 0.
	 */
	virtual int process(BasicBuffer* input);

	/**
	 * Returns the hash for the current state. In case of errors NULL is returned.
	 */
	virtual BasicBuffer* getHash();

	/**
	 * Writes the hash for the current state into the output buffer.
	 * The output buffer must be large enough to contain the hash.
	 * In case of errors -1 is returned, 0 otherwise.
	 */
	virtual int getHash(BasicBuffer* output);

	/**
	 * Resets the hash state.
	 */
	virtual void reset();

	/**
	 * Returns the idx for the hash function.
	 */
	int getIdx();

	/**
	 * Returns a pointer to the internal state.
	 */
#ifdef USETOMCRYPT
	hash_state* getState();
#endif

	/**
	 * Returns the size of this functions output
	 */
	unsigned long getHashSize();

protected:
	int m_hash_idx;
#ifdef USETOMCRYPT
	hash_state m_hash_state;
#endif
	unsigned long m_hashsize;
	unsigned long m_hash_blocksize;
	bool m_ready;

};

} /* namespace Lazarus */

#endif /* TOMCRYPTHASHWRAPPER_H_ */
