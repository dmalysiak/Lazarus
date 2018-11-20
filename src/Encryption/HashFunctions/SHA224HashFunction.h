/*
 * SHA224HashFunction.h
 *
 *  Created on: 15.12.2014
 *      Author: Darius Malysiak
 */

#ifndef SHA224HASH_H_
#define SHA224HASH_H_

#include "HashFunction.h"

namespace Lazarus {

class SHA224HashFunction: public HashFunction {
public:
	SHA224HashFunction();
	virtual ~SHA224HashFunction();

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
};

} /* namespace Lazarus */

#endif /* SHA224HASH_H_ */
