/*
 * Base64.cpp
 *
 *  Created on: 23.05.2016
 *      Author: Darius Malysiak
 */

#include "Base64.h"
#ifdef USETOMCRYPT
#include <tomcrypt.h>
#endif
namespace Lazarus {

Base64::Base64() {
	// TODO Auto-generated constructor stub

}

Base64::~Base64() {
	// TODO Auto-generated destructor stub
}


/**
* Encodes a given string in Base64
* @param input The input string to Base64-encode
* @param inputSize The size of the input to decode
* @return A Base64-encoded version of the encoded string
*/
std::string Base64::encodeBase64(const char* input, const unsigned long inputSize)
{
#ifdef USETOMCRYPT
    unsigned long outlen = inputSize + (inputSize / 3.0) + 16;
    unsigned char* outbuf = new unsigned char[outlen]; //Reserve output memory
    base64_encode((unsigned char*) input, inputSize, outbuf, &outlen);
    std::string ret((char*) outbuf, outlen);
    delete[] outbuf;
    return ret;
#endif
#ifdef NOTOMCRYPT
    return "";
#endif

}

/**
* Encodes a given string in Base64
* @param input The input string to Base64-encode
* @return A Base64-encoded version of the encoded string
*/
std::string Base64::encodeBase64(const std::string& input)
{
#ifdef USETOMCRYPT
    return encodeBase64(input.c_str(), input.size());
#endif    
#ifdef NOTOMCRYPT
    return "";
#endif
}

/**
* Encodes a given string in Base64
* @param input The input (binary) to Base64-encode
* @return A Base64-encoded version of the encoded string
*/
std::string Base64::encodeBase64(const Buffer& input)
{
#ifdef USETOMCRYPT
    return encodeBase64((const char*)input.get_mp_data(), input.get_m_length());
#endif
#ifdef NOTOMCRYPT
    return "";
#endif
}

/**
* Decodes a Base64-encoded string.
* @param input The input string to decode
* @return A buffer that represents the binary Base64-decoded data of the input
*/
Buffer* Base64::decodeBase64(const std::string& input)
{
#ifdef USETOMCRYPT
    unsigned char* out = new unsigned char[input.size()];
    unsigned long outlen = input.size();
    base64_decode((unsigned char*) input.c_str(), input.size(), out, &outlen);

    return new Buffer(out,outlen);
#endif
#ifdef USETOMCRYPT
    return new Buffer(1);
#endif
}

} /* namespace Lazarus */
