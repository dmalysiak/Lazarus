/*
 * AESBox.cpp
 *
 *  Created on: Nov 30, 2014
 *      Author: Darius Malysiak
 */

#include "KhazadBox.h"

#include <stdio.h>
#include <math.h>

namespace Lazarus {

#ifdef USETOMCRYPT
  
KhazadBox::KhazadBox(const unsigned char* key, const unsigned char* iv, int keysize, enum CHAINING_MODE chaining_mode, int cipher_rounds)
{

	int err = 0;
	m_cipher_name = "Khazad";
	m_block_size = 8;
	m_chaining_mode = chaining_mode;

	/* register AES first */
	if( register_cipher(&khazad_desc) == -1)
	{
		printf("Error registering khazad cipher.\n");
		m_ready = false;
		return;
	}


	/*
	 * This will round down the given key size to a fitting value, if the initial key size was too small an error will be printed
	 * and the scheduling stops
    */
	if( (err = khazad_keysize(&keysize)) != CRYPT_OK )
	{
		printf("Error getting key size: %s\n", error_to_string(err));
		m_ready = false;
		return;
	}

	printf("khazad suggested a key size of %d\n", keysize);

	//copy key and IV
	m_key_size = keysize;
	m_key = new unsigned char[16];
	memset(m_key,0,16);
	memcpy(m_key,key,m_key_size);

	m_IV = new unsigned char[8];
	memcpy(m_IV,iv,8);

	m_cipher_rounds = cipher_rounds;

	/**
	 * set up the key for khazad
	 */
	if( (err = khazad_setup(key, keysize, cipher_rounds, &m_skey))  != CRYPT_OK )
	{
		printf("Error setting up khazad key: %s\n", error_to_string(err));
		m_ready = false;
		return;
	}

	//setup chaining mode
	setupChain(m_chaining_mode);

}

KhazadBox::~KhazadBox()
{
	//terminate the chain
	terminateChain(m_chaining_mode);

	//clean up
	khazad_done(&m_skey);

	DELETE_NULL_CHECKING(m_key);
	DELETE_NULL_CHECKING(m_IV);

}


int KhazadBox::setupChain(enum CHAINING_MODE chaining_mode)
{
	int err = 0;

	switch(chaining_mode)
	{
	case CHAINING_MODE_CTR:
		/**
		 * set up the counter mode
		 */
		if( (err = ctr_start(find_cipher("khazad"), m_IV, m_key, m_key_size, m_cipher_rounds, CTR_COUNTER_LITTLE_ENDIAN, &m_ctr))  != CRYPT_OK )
		{
			printf("Error setting up khazad-ctr mode: %s\n", error_to_string(err));
			m_ready = false;
			return -1;
		}
		else
		{
			m_ready = true;
		}
		break;

	case CHAINING_MODE_CBC:
		/**
		 * set up the cbc mode
		 */
		if( (err = cbc_start(find_cipher("khazad"), m_IV, m_key, m_key_size, m_cipher_rounds, &m_cbc))  != CRYPT_OK )
		{
			printf("Error setting up khazad-cbc mode: %s\n", error_to_string(err));
			m_ready = false;
			return -1;
		}
		else
		{
			m_ready = true;
		}
		break;

	case CHAINING_MODE_ECB:
		/**
		 * set up the ecb mode
		 */
		if( (err = ecb_start(find_cipher("khazad"), m_key, m_key_size, m_cipher_rounds, &m_ecb))  != CRYPT_OK )
		{
			printf("Error setting up khazad-ecb mode: %s\n", error_to_string(err));
			m_ready = false;
			return -1;
		}
		else
		{
			m_ready = true;
		}
		break;

	case CHAINING_MODE_UNKNOWN:
		printf("Error setting up khazad-XXX mode: unknown chaining mode\n");
		m_ready = false;
		return -1;

	default:
		printf("Error setting up khazad-XXX mode: unknown chaining mode\n");
		m_ready = false;
		return -1;

	}

	return 0;
}
#endif


#ifdef NOTOMCRYPT
  
KhazadBox::KhazadBox(const unsigned char* key, const unsigned char* iv, int keysize, enum CHAINING_MODE chaining_mode, int cipher_rounds)
{
}

KhazadBox::~KhazadBox()
{
}


int KhazadBox::setupChain(enum CHAINING_MODE chaining_mode)
{
	return 0;
}


#endif


} /* namespace Lazarus */
