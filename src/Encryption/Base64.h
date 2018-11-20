/*
 * Base64.h
 *
 *  Created on: 23.05.2016
 *      Author: Darius Malysiak
 */

#ifndef CORE_ENCRYPTION_BASE64_H_
#define CORE_ENCRYPTION_BASE64_H_

#include "../BaseObject.h"
#include "../Buffer.h"

#include <string>


namespace Lazarus {

class Base64: public BaseObject {
public:
	Base64();
	virtual ~Base64();

	/**
	* Encodes a given string in Base64
	* @param input The input string to Base64-encode
	* @param inputSize The size of the input to decode
	* @return A Base64-encoded version of the encoded string
	*/
	static std::string encodeBase64(const char* input, const unsigned long inputSize);

	/**
	* Encodes a given string in Base64
	* @param input The input string to Base64-encode
	* @return A Base64-encoded version of the encoded string
	*/
	static std::string encodeBase64(const std::string& input);

	/**
	* Encodes a given string in Base64
	* @param input The binary input to Base64-encode
	* @return A Base64-encoded version of the encoded string
	*/
	static std::string encodeBase64(const Buffer& input);

	/**
	* Decodes a Base64-encoded string.
	* @param input The input string to decode
	* @return A buffer that represents the binary Base64-decoded data of the input
	*/
	static Buffer* decodeBase64(const std::string& input);

};

} /* namespace Lazarus */

#endif /* CORE_ENCRYPTION_BASE64_H_ */
