/*
 * CipherBox.cpp
 *
 *  Created on: 13.12.2014
 *      Author: Darius Malysiak
 */

#include "CipherBox.h"

#include <stdio.h>

namespace Lazarus {

CipherBox::CipherBox()
{
	m_chaining_mode = CHAINING_MODE_UNKNOWN;
}

CipherBox::~CipherBox() {
	// TODO Auto-generated destructor stub
}

int CipherBox::encryptFile(std::string input, std::string output)
{
	printf("ERROR: file encryption not implemented\n");
	return -1;
}

int CipherBox::decryptFile(std::string input, std::string output)
{
	printf("ERROR: file decryption not implemented\n");
	return -1;
}

int CipherBox::encryptFileInPlaceRaw(std::string input_file)
{
	printf("ERROR: file encryption not implemented\n");
	return -1;
}

int CipherBox::decryptFileInPlaceRaw(std::string input_file)
{
	printf("ERROR: file decryption not implemented\n");
	return -1;
}

int CipherBox::encryptFileRaw(std::string input, std::string output)
{
	printf("ERROR: file encryption not implemented\n");
	return -1;
}

int CipherBox::decryptFileRaw(std::string input, std::string output)
{
	printf("ERROR: file decryption not implemented\n");
	return -1;
}

BasicBuffer* CipherBox::encryptBuffer(BasicBuffer* buffer)
{
	printf("ERROR: buffer encryption not implemented\n");
	return NULL;
}

BasicBuffer* CipherBox::decryptBuffer(BasicBuffer* buffer)
{
	printf("ERROR: buffer decryption not implemented\n");
	return NULL;
}


int CipherBox::encryptBufferRaw(BasicBuffer* input_buffer, BasicBuffer* output_buffer)
{
	printf("ERROR: buffer encryption not implemented\n");
	return -1;
}

int CipherBox::decryptBufferRaw(BasicBuffer* input_buffer, BasicBuffer* output_buffer)
{
	printf("ERROR: buffer decryption not implemented\n");
	return -1;
}

int CipherBox::encryptBuffer(BasicBuffer* input_buffer, BasicBuffer* output_buffer)
{
	printf("ERROR: buffer encryption not implemented\n");
	return -1;
}

int CipherBox::decryptBuffer(BasicBuffer* input_buffer, BasicBuffer* output_buffer)
{
	printf("ERROR: buffer decryption not implemented\n");
	return -1;
}

} /* namespace Lazarus */
