/*
 * CipherBox.h
 *
 *  Created on: 13.12.2014
 *      Author: Darius Malysiak
 */

#ifndef CIPHERBOX_H_
#define CIPHERBOX_H_

#include "../../BaseObject.h"
#include "../../BasicBuffer.h"

#ifdef USETOMCRYPT
#include "../TomCrypt/src/headers/tomcrypt.h"
#endif

namespace Lazarus {

/**
 * ATTENTION: The cipherbox classes are not fully thread-safe:
 * All encryption and decryption ops are thread safe, yet the initialization of a cipher within a constructor is not
 * thread safe! Thus one should instantiate all objects before any multithreaded application!
 */

class CipherBox: public BaseObject {
public:

	enum CHAINING_MODE{CHAINING_MODE_CTR,CHAINING_MODE_CBC,CHAINING_MODE_ECB,CHAINING_MODE_UNKNOWN};

	CipherBox();
	virtual ~CipherBox();

	/**
	 * The encrypted files may contain any kind of header.
	 */
	virtual int encryptFile(std::string input_file, std::string output_file);
	virtual int decryptFile(std::string input_file, std::string output_file);

	/*
	 * Encrypts the content of the given buffer object and returns a new buffer object which contains the block-aligned ciphertext.
	 * For performance reasons one should keep the buffer input already block-aligned (esp. for frequent encryptions of small buffers).
	 * Returns NULL in case of errors.
	 */
	virtual BasicBuffer* encryptBuffer(BasicBuffer* buffer);

	/*
	 * Decrypts the content of the given buffer object and returns a new buffer object which contains the block-aligned plaintext.
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

	//----------------------- The following methods allow daisy-chaining of cipher boxes.

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
	 * The encrypted files will contain no header with meta information.
	 * Yet the cipher must support partial blocks, e.g. any cipher in CTR-mode;
	 */
	virtual int encryptFileRaw(std::string input_file, std::string output_file);
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
	enum CHAINING_MODE m_chaining_mode;
};

} /* namespace Lazarus */

#endif /* CIPHERBOX_H_ */
