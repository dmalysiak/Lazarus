/*
 * TomCryptWrapper.h
 *
 *  Created on: 15.12.2014
 *      Author: Darius Malysiak
 */

#ifndef TOMCRYPTWRAPPER_H_
#define TOMCRYPTWRAPPER_H_

#include "CipherBox.h"
#include "../../Buffer.h"

#ifdef USETOMCRYPT
#include <tomcrypt.h>
#endif

#include <string>

namespace Lazarus {

/*
 * Any instance of this class will always be "not ready"!
 */

class TomCryptWrapper: public CipherBox {
public:
	TomCryptWrapper();
	virtual ~TomCryptWrapper();


	/**
	 * Encrypts the given file block by block, uses a cache for performance improvements.
	 * An 8 byte (long int) header containing the original size is prepended to the cipher.
	 * Attention: 'output_file' will be overwritten! Returns -1 in case of errors, 0 otherwise.
	 */
	virtual int encryptFile(std::string input_file, std::string output_file);

	/**
	 * Decrypts the given file block by block, uses a cache for performance improvements.
	 * An 8 byte (long int) header containing the original size is prepended to the cipher.
	 * Attention: 'output_file' will be overwritten! Returns -1 in case of errors, 0 otherwise.
	 */
	virtual int decryptFile(std::string input_file, std::string output_file);

	/*
	 * Encrypts the content of the given buffer object and returns a new buffer object which contains the block-aligned ciphertext.
	 * For performance reasons one should keep the buffer input already block-aligned (esp. for frequent encryptions of small buffers).
	 * Returns NULL in case of errors.
	 */
	virtual BasicBuffer* encryptBuffer(BasicBuffer* buffer);

	/*
	 * Decrypts the content of the given buffer object and returns a new buffer object which contains the block-aligned plaintext.
	 * It is assumed that the input buffer is block-aligned!
	 * Returns NULL in case of errors.
	 */
	virtual BasicBuffer* decryptBuffer(BasicBuffer* buffer);

	/*
	 * Encrypts the content of the given buffer object and writes the ciphertext into the given output buffer.
	 * The buffers must NOT be block-aligned! Yet they must be of the same size.
	 * Furthermore the cipher must support partial blocks, e.g. any cipher in CTR-mode;
	 * Returns -1 in case of errors, 0 otherwise.
	 */
	virtual int encryptBufferRaw(BasicBuffer* input_buffer, BasicBuffer* output_buffer);

	/*
	 * Encrypts the content of the given buffer object and writes the ciphertext into the given output buffer.
	 * The buffers must NOT be block-aligned! Yet they must be of the same size.
	 * Furthermore the cipher must support partial blocks, e.g. any cipher in CTR-mode;
	 * Returns -1 in case of errors, 0 otherwise.
	 */
	virtual int decryptBufferRaw(BasicBuffer* input_buffer, BasicBuffer* output_buffer);


	//--------------------------- The following methods allow daisy-chaining of cipher boxes.

	/*
	 * Encrypts the content of the given buffer object and writes the ciphertext into the given output buffer.
	 * All buffers must be block-aligned! Returns -1 in case of errors, 0 otherwise.
	 */
	virtual int encryptBuffer(BasicBuffer* input_buffer, BasicBuffer* output_buffer);

	/*
	 * Decrypts the content of the given buffer object and writes the ciphertext into the given output buffer.
	 * All buffers must be block-aligned! Returns -1 in case of errors, 0 otherwise.
	 */
	virtual int decryptBuffer(BasicBuffer* input_buffer, BasicBuffer* output_buffer);

	/**
	 * Encrypts the given file block by block, uses a cache for performance improvements.
	 * The output filesize is identical to the source file.
	 * Furthermore the cipher must support partial blocks, e.g. any cipher in CTR-mode;
	 * Attention: 'output_file' will be overwritten! Returns -1 in case of errors, 0 otherwise.
	 */
	virtual int encryptFileRaw(std::string input_file, std::string output_file);

	/**
	 * Decrypts the given file block by block, uses a cache for performance improvements.
	 * The output filesize is identical to the source file.
	 * Furthermore the cipher must support partial blocks, e.g. any cipher in CTR-mode;
	 * Attention: 'output_file' will be overwritten! Returns -1 in case of errors, 0 otherwise.
	 */
	virtual int decryptFileRaw(std::string input_file, std::string output_file);

	/**
	 * Encrypts the given file block by block, uses a cache for performance improvements.
	 * The output filesize is identical to the source file.
	 * Furthermore the cipher must support partial blocks, e.g. any cipher in CTR-mode;
	 * Returns -1 in case of errors, 0 otherwise.
	 */
	virtual int encryptFileInPlaceRaw(std::string input_file);

	/**
	 * Decrypts the given file block by block, uses a cache for performance improvements.
	 * The output filesize is identical to the source file.
	 * Furthermore the cipher must support partial blocks, e.g. any cipher in CTR-mode;
	 *  Returns -1 in case of errors, 0 otherwise.
	 */
	virtual int decryptFileInPlaceRaw(std::string input_file);



protected:
#ifdef USETOMCRYPT
	symmetric_key m_skey;
	symmetric_CTR m_ctr;
	symmetric_CBC m_cbc;
	symmetric_ECB m_ecb;
#endif
        
	std::string m_cipher_name;
	unsigned int m_block_size;
	bool m_ready;
	int m_key_size;
	unsigned char* m_IV;//16 bytes
	unsigned char* m_key;
	int m_cipher_rounds;

	int terminateChain(enum CHAINING_MODE chaining_mode);
	int resetIV(enum CHAINING_MODE chaining_mode);

	/*
	 * Encrypts the content of the given buffer object and writes the ciphertext into the given output buffer.
	 * All buffers must be block-aligned! Returns -1 in case of errors, 0 otherwise. The IV won't be reseted in each call!!
	 */
	virtual int encryptBufferNoIVReset(Buffer* input_buffer, Buffer* output_buffer);

	/*
	 * Decrypts the content of the given buffer object and writes the ciphertext into the given output buffer.
	 * All buffers must be block-aligned! Returns -1 in case of errors, 0 otherwise. The IV won't be reseted in each call!!
	 */
	virtual int decryptBufferNoIVReset(Buffer* input_buffer, Buffer* output_buffer);
};

} /* namespace Lazarus */

#endif /* TOMCRYPTWRAPPER_H_ */
