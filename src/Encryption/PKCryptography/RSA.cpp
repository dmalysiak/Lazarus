/*
 * RSA.cpp
 *
 *  Created on: 18.12.2014
 *      Author: Darius Malysiak
 */

#include "RSA.h"

#include "../../Buffer.h"

#ifdef USETOMCRYPT
#include <tfm.h>
#endif

namespace Lazarus {

#ifdef USETOMCRYPT 
  
RSA::RSA() {

	// register a math library (in this case TomsFastMath)
	ltc_mp = tfm_desc;

	m_key_ready = false;
	m_key_size = 0;

	m_key.N = NULL;
	m_key.e = NULL;
	m_key.d = NULL;
	m_key.dP = NULL;
	m_key.dQ = NULL;
	m_key.qP = NULL;
	m_key.p = NULL;
	m_key.q = NULL;
}

RSA::~RSA()
{
	//delete any existing key
	rsa_free(&m_key);
}

int RSA::makeKey(PRNG* prng, int bit_length, long e)
{
	int err = 0;

	//delete any existing key
	rsa_free(&m_key);
	m_key_ready = false;

	/* make an RSA-1024 key */
	if((err = rsa_make_key(prng->getState(), prng->getIdx(), bit_length/8, e, &m_key) ) != CRYPT_OK)
	{
		printf("ERROR: rsa_make_key %s\n", error_to_string(err));
		return -1;
	}

	m_key_ready = true;
	m_key_size = bit_length/8;

	return 0;
}

BasicBuffer* RSA::encryptKey(PRNG* prng, HashFunction* hf, std::string tag, BasicBuffer* key)
{
	//sanity check
	if(m_key_ready == false)
	{
		printf("ERROR: RSA key not ready\n");
		return NULL;
	}

	int err = 0;
	unsigned char* cipher = new unsigned char[8096];
	unsigned long cipher_length = 8096;

	if((err = rsa_encrypt_key_ex(key->get_mp_data(), key->get_m_length(),
								cipher, &cipher_length, (const unsigned char*)tag.c_str(), tag.size(),
								prng->getState(),  prng->getIdx(),
								hf->getIdx(), LTC_PKCS_1_OAEP, &m_key)  ) != CRYPT_OK)
	{
		printf("ERROR: rsa_encrypt_key %s\n", error_to_string(err));
		return NULL;
	}

	Buffer* output = new Buffer(cipher, cipher_length, true);
	delete[] cipher;

	return output;

}

BasicBuffer* RSA::decryptKey(HashFunction* hf, std::string tag, BasicBuffer* cipher_t)
{
	//sanity check
	if(m_key_ready == false)
	{
		printf("ERROR: RSA key not ready\n");
		return NULL;
	}

	int err = 0;
	unsigned char* plaintext = new unsigned char[8096];
	unsigned long plaintext_length = 8096;
	int stat = 0;

	if((err = rsa_decrypt_key_ex(cipher_t->get_mp_data(), cipher_t->get_m_length(),
								plaintext, &plaintext_length,
								(const unsigned char*)tag.c_str(), tag.size(),
								hf->getIdx(), LTC_PKCS_1_OAEP, &stat, &m_key)  ) != CRYPT_OK)
	{
		printf("ERROR: rsa_encrypt_key %s\n", error_to_string(err));
		return NULL;
	}

	if(stat == 0)
	{
		printf("ERROR: decryption invalid\n");
		return NULL;
	}

	Buffer* output = new Buffer(plaintext, plaintext_length, true);
	delete[] plaintext;

	return output;
}

BasicBuffer* RSA::getSignature(PRNG* prng, HashFunction* hf, BasicBuffer* message, unsigned long salt_size)
{
	//sanity check
	if(m_key_ready == false)
	{
		printf("ERROR: RSA key not ready\n");
		return NULL;
	}

	//check if salt value is sane
	if(salt_size >= m_key_size - hf->getHashSize()-2)
	{
		printf("ERROR: salt_size is too large.\n");
		return NULL;
	}

	int err = 0;
	unsigned char* signature = new unsigned char[16000];
	unsigned long signature_length = 16000;

	if((err = rsa_sign_hash_ex(message->get_mp_data(), message->get_m_length(),
								signature, &signature_length,
								LTC_PKCS_1_PSS, prng->getState(), prng->getIdx(),
								hf->getIdx(), salt_size,
								&m_key)  ) != CRYPT_OK)
	{
		printf("ERROR: rsa_sign_hash_ex %s\n", error_to_string(err));
		delete[] signature;
		return NULL;
	}

	Buffer* output = new Buffer(signature, signature_length, true);
	delete[] signature;

	return output;
}

int RSA::verifySignature(HashFunction* hf, BasicBuffer* signature, BasicBuffer* message, unsigned long salt_size)
{
	//sanity check
	if(m_key_ready == false)
	{
		printf("ERROR: RSA key not ready\n");
		return -1;
	}

	//check if salt value is sane
	if(salt_size >= m_key_size-hf->getHashSize()-2)
	{
		printf("ERROR: salt_size is too large.\n");
		return -1;
	}

	//hash the message
	BasicBuffer* hash_buffer = hf->getSingleHash(message);

	int err = 0;
	unsigned char* hash = hash_buffer->get_mp_data();
	unsigned long hash_length = hash_buffer->get_m_length();
	int stat = 0;

	if((err = rsa_verify_hash_ex(signature->get_mp_data(), signature->get_m_length(),
								hash, hash_length,
								LTC_PKCS_1_PSS,
								hf->getIdx(), salt_size, &stat,
								&m_key)  ) != CRYPT_OK)
	{
		printf("ERROR: rsa_verify_hash_ex %s\n", error_to_string(err));
		delete hash_buffer;
		delete[] hash;
		return -1;
	}

	if(stat == 0)
	{
		printf("ERROR: signature invalid\n");
		delete hash_buffer;
		delete[] hash;
		return -1;
	}


	return 0;
}

void RSA::printfTFMint(void* data)
{
	fp_int* integer = (fp_int*)data;

	for(int i=0;i<integer->used;++i)
	{
		ulong64 digit = integer->dp[i];
		//printf("%llu    ",digit);
		for(unsigned int j=0;j<sizeof(ulong64);++j)
		{
			printf("%02x ", (unsigned int) (digit >> (7-j)*8) & 0xFF );
		}
	}
}

RSAKey* RSA::getKey()
{
	//sanity check
	if(m_key_ready == false)
	{
		printf("ERROR: RSA key not ready\n");
		return NULL;
	}

	return new RSAKey(&m_key);
}

void RSA::printKey()
{
	//sanity check
	if(m_key_ready == false)
	{
		printf("ERROR: RSA key not ready\n");
		return;
	}

	if(m_key.type == PK_PRIVATE)
	{
		printf("RSA key is private\n");
		printf("e ");printfTFMint(m_key.e);printf("\n");
		printf("d ");printfTFMint(m_key.d);printf("\n");
		printf("N ");printfTFMint(m_key.N);printf("\n");
		printf("p factor of N ");printfTFMint(m_key.p);printf("\n");
		printf("q factor of N ");printfTFMint(m_key.q);printf("\n");
		printf("1/q mod p CRT param ");printfTFMint(m_key.qP);printf("\n");
		printf("d mod (p - 1) CRT param ");printfTFMint(m_key.dP);printf("\n");
		printf("d mod (q - 1) CRT param ");printfTFMint(m_key.dQ);
	}
	if(m_key.type == PK_PUBLIC)
	{
		printf("RSA key is public\n");
		printf("e ");printfTFMint(m_key.e);printf("\n");
		printf("N ");printfTFMint(m_key.N);printf("\n");
	}

}

int RSA::exportKey(BasicBuffer** buffer, bool export_as_private)
{
	//sanity check
	if(m_key_ready == false)
	{
		printf("ERROR: RSA key not ready\n");
		return -1;
	}

	unsigned char* buffer_ = new unsigned char[16000];
	unsigned long length = 16000;

	int err = 0;

	if(export_as_private == true)
	{
		if((err = rsa_export(buffer_, &length, PK_PRIVATE, &m_key) ) != CRYPT_OK)
		{
			printf("ERROR: rsa_export %s\n", error_to_string(err));
			delete[] buffer_;
			return -1;
		}
	}
	else
	{
		if((err = rsa_export(buffer_, &length, PK_PUBLIC, &m_key) ) != CRYPT_OK)
		{
			printf("ERROR: rsa_export %s\n", error_to_string(err));
			delete[] buffer_;
			return -1;
		}
	}
	//update the external ref to the new buffer
	Buffer* output = new Buffer(buffer_,length,true);
	(*buffer) = output;

	delete[] buffer_;

	return 0;
}

int RSA::importKey(BasicBuffer* buffer)
{
	//sanity check
	if(m_key_ready == false)
	{
		printf("ERROR: RSA key not ready\n");
		return -1;
	}

	int err = 0;

	if((err = rsa_import(buffer->get_mp_data(), buffer->get_m_length(), &m_key) ) != CRYPT_OK)
	{
		printf("ERROR: rsa_import %s\n", error_to_string(err));
		return -1;
	}

	return 0;
}

#endif


#ifdef NOTOMCRYPT 
  
RSA::RSA() {

}

RSA::~RSA()
{
}

int RSA::makeKey(PRNG* prng, int bit_length, long e)
{
	return 0;
}

BasicBuffer* RSA::encryptKey(PRNG* prng, HashFunction* hf, std::string tag, BasicBuffer* key)
{
	return NULL;
}

BasicBuffer* RSA::decryptKey(HashFunction* hf, std::string tag, BasicBuffer* cipher_t)
{
	return NULL;
}

BasicBuffer* RSA::getSignature(PRNG* prng, HashFunction* hf, BasicBuffer* message, unsigned long salt_size)
{
	return NULL;
}

int RSA::verifySignature(HashFunction* hf, BasicBuffer* signature, BasicBuffer* message, unsigned long salt_size)
{
	return 0;
}

void RSA::printfTFMint(void* data)
{
}

void RSA::printKey()
{
}

int RSA::exportKey(BasicBuffer** buffer, bool export_as_private)
{
}

int RSA::importKey(BasicBuffer* buffer)
{
	return 0;
}

#endif

} /* namespace Lazarus */
