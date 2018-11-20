/*
 * DHKeyExchange.cpp
 *
 *  Created on: 19.12.2014
 *      Author: Darius Malysiak
 */

#include "DHKeyExchange.h"

#include "../../Buffer.h"

namespace Lazarus {

#ifdef USETOMCRYPT
  
DHKeyExchange::DHKeyExchange() {

	m_private_key_ready = false;

	m_key.x = NULL;
	m_key.y = NULL;

	// register a math library (in this case TomsFastMath)
	ltc_mp = tfm_desc;

}

DHKeyExchange::~DHKeyExchange() {
	dh_free(&m_key);
}

BasicBuffer* DHKeyExchange::encryptKey(PRNG* prng, HashFunction* hf, BasicBuffer* key)
{
	//sanity check
	if(m_private_key_ready == false)
	{
		printf("ERROR: private key is not ready.\n");
		return NULL;
	}

	if(hf->getHashSize() < key->get_m_length())
	{
		printf("ERROR: the hash functions output size is too small; it must be at least as large as the plaintext.\n");
		return NULL;
	}

	//reset the PRNG
	prng->resetPRNG();

	int err = 0;
	unsigned char* buff = new unsigned char[16000];
	unsigned long buff_l = 16000;

	if((err = dh_encrypt_key(key->get_mp_data(), key->get_m_length(), buff, &buff_l,
			prng->getState(),prng->getIdx(),hf->getIdx(),&m_key)) != CRYPT_OK)
	{
		printf("ERROR: dh_encrypt_key failed\n");
		delete[] buff;
		return NULL;
	}

	Buffer* output = new Buffer(buff,buff_l, true);
	delete[] buff;

	return output;

	return 0;
}

BasicBuffer* DHKeyExchange::decryptKey(BasicBuffer* c_key)
{
	//sanity check
	if(m_private_key_ready == false)
	{
		printf("ERROR: private key is not ready.\n");
		return NULL;
	}

	int err = 0;
	unsigned char* buff = new unsigned char[16000];
	unsigned long buff_l = 16000;

	if((err = dh_decrypt_key(c_key->get_mp_data(), c_key->get_m_length(), buff, &buff_l, &m_key)) != CRYPT_OK)
	{
		printf("ERROR: dh_encrypt_key failed\n");
		delete[] buff;
		return NULL;
	}

	Buffer* output = new Buffer(buff,buff_l, true);
	delete[] buff;

	return output;

	return 0;
}

BasicBuffer* DHKeyExchange::createSharedSecret(BasicBuffer* public_key)
{
	//sanity check
	if(m_private_key_ready == false)
	{
		printf("ERROR: private key is not ready.\n");
		return NULL;
	}

	//import the public key
	dh_key dh_public_key;
	int err = 0;
	unsigned char* buff = new unsigned char[8096];
	unsigned long buff_l = 8096;

	if((err = dh_import(public_key->get_mp_data(), public_key->get_m_length(), &dh_public_key)) != CRYPT_OK)
	{
		printf("ERROR: dh_import of public key failed\n");
		delete[] buff;
		return NULL;
	}

	if((err = dh_shared_secret(&m_key, &dh_public_key, buff, &buff_l)) != CRYPT_OK)
	{
		printf("ERROR: dh_shared_secret failed\n");
		delete[] buff;
		return NULL;
	}

	dh_free(&dh_public_key);

	Buffer* output = new Buffer(buff,buff_l, true);
	delete[] buff;

	return output;
}

BasicBuffer* DHKeyExchange::exportKey(bool export_as_private)
{

	//sanity check
	if(m_private_key_ready == false)
	{
		printf("ERROR: private key is not ready.\n");
		return NULL;
	}

	int err = 0;
	unsigned char* buff = new unsigned char[8096];
	unsigned long buff_l = 8096;

	if(export_as_private == true)
	{
		if((err = dh_export(buff, &buff_l, PK_PRIVATE, &m_key)) != CRYPT_OK)
		{
			printf("ERROR: dh_export failed\n");
			delete[] buff;
			return NULL;
		}
	}
	else
	{
		if((err = dh_export(buff, &buff_l, PK_PUBLIC, &m_key)) != CRYPT_OK)
		{
			printf("ERROR: dh_export failed\n");
			delete[] buff;
			return NULL;
		}
	}

	Buffer* output = new Buffer(buff,buff_l, true);
	delete[] buff;

	return output;

}

int DHKeyExchange::importKey(BasicBuffer* key)
{
	//sanity check
	if(m_private_key_ready == true)
	{
		//free the old key
		dh_free(&m_key);

		m_private_key_ready = false;
	}

	int err = 0;

	if((err = dh_import(key->get_mp_data(), key->get_m_length(), &m_key)) != CRYPT_OK)
	{
		printf("ERROR: dh_import failed\n");
		return -1;
	}

	return 0;
}

int DHKeyExchange::createKey(PRNG* prng, int size)
{
	//sanity check
	if(m_private_key_ready == true)
	{
		//free the old key
		dh_free(&m_key);

		m_private_key_ready = false;
	}

	//reset the PRNG
	prng->resetPRNG();

	int err = 0;
	//create the key
	if((err = dh_make_key(prng->getState(), prng->getIdx(), size, &m_key)) != CRYPT_OK)
	{
		printf("ERROR: could not create DH key.: %s\n", error_to_string(err));
		return -1;
	}

	m_private_key_ready = true;

	return 0;
}

#endif


#ifdef NOTOMCRYPT
  
DHKeyExchange::DHKeyExchange() {
}

DHKeyExchange::~DHKeyExchange() {
}

BasicBuffer* DHKeyExchange::encryptKey(PRNG* prng, HashFunction* hf, BasicBuffer* key)
{
	return NULL;
}

BasicBuffer* DHKeyExchange::decryptKey(BasicBuffer* c_key)
{
	return NULL;
}

BasicBuffer* DHKeyExchange::createSharedSecret(BasicBuffer* public_key)
{
	return NULL;
}

BasicBuffer* DHKeyExchange::exportKey(bool export_as_private)
{
	return NULL;

}

int DHKeyExchange::importKey(BasicBuffer* key)
{
	return NULL;
}

int DHKeyExchange::createKey(PRNG* prng, int size)
{
	return 0;
}

#endif

} /* namespace Lazarus */
