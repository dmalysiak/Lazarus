/*
 * RSAKey.cpp
 *
 *  Created on: 19.12.2014
 *      Author: Darius Malysiak
 */

#include "RSAKey.h"

#ifdef USETOMCRYPT
#include <tfm.h>
#endif

namespace Lazarus {

#ifdef USETOMCRYPT  
RSAKey::RSAKey(rsa_key* key) {

	if(key->type == PK_PRIVATE)
	{
		m_private = true;

		getTFMint(key->N, &mp_N, &m_N_length);
		getTFMint(key->e, &mp_e, &m_e_length);
		getTFMint(key->d, &mp_d, &m_d_length);
		getTFMint(key->p, &mp_p, &m_p_length);
		getTFMint(key->q, &mp_q, &m_q_length);
		getTFMint(key->qP, &mp_qP, &m_qP_length);
		getTFMint(key->dP, &mp_dP, &m_dP_length);
		getTFMint(key->dQ, &mp_dQ, &m_dQ_length);

	}
	if(key->type == PK_PUBLIC)
	{
		m_private = false;

		getTFMint(key->N, &mp_N, &m_N_length);
		getTFMint(key->e, &mp_e, &m_e_length);

		mp_d = NULL;
		mp_p = NULL;
		mp_q = NULL;
		mp_qP = NULL;
		mp_dP = NULL;
		mp_dQ = NULL;

		m_d_length = -1;
		m_p_length = -1;
		m_q_length = -1;
		m_qP_length = -1;
		m_dP_length = -1;
		m_dQ_length = -1;
	}

}

RSAKey::~RSAKey()
{
	DELETE_ARRAY_NULL_CHECKING(mp_e);
	DELETE_ARRAY_NULL_CHECKING(mp_N);
	DELETE_ARRAY_NULL_CHECKING(mp_d);
	DELETE_ARRAY_NULL_CHECKING(mp_p);
	DELETE_ARRAY_NULL_CHECKING(mp_q);
	DELETE_ARRAY_NULL_CHECKING(mp_qP);
	DELETE_ARRAY_NULL_CHECKING(mp_dP);
	DELETE_ARRAY_NULL_CHECKING(mp_dQ);

}

void RSAKey::getTFMint(void* data, unsigned char** destination, long long* length)
{
	/*
	 * TODO: also track the sign of the integers, which should be +1 in the local cases.
	 * */
	fp_int* integer = (fp_int*)data;

	*destination = new unsigned char[sizeof(ulong64) * integer->used];
	*length = integer->used;

	for(int i=0;i<integer->used;++i)
	{
		ulong64 digit = integer->dp[i];
		//printf("%llu    ",digit);
		for(unsigned int j=0;j<sizeof(ulong64);++j)
		{
			(*destination)[i*sizeof(ulong64) + j] = (digit >> (7-j)*8) & 0xFF;
		}
	}
}

void RSAKey::printKey()
{

	if(m_private == true)
	{
		printf("RSA key is private\n");
		printf("e ");printfBigInt(mp_e,m_e_length);printf("\n");
		printf("d ");printfBigInt(mp_d,m_d_length);printf("\n");
		printf("N ");printfBigInt(mp_N,m_N_length);printf("\n");
		printf("p factor of N ");printfBigInt(mp_p,m_p_length);printf("\n");
		printf("q factor of N ");printfBigInt(mp_q,m_q_length);printf("\n");
		printf("1/q mod p CRT param ");printfBigInt(mp_qP,m_qP_length);printf("\n");
		printf("d mod (p - 1) CRT param ");printfBigInt(mp_dP,m_dP_length);printf("\n");
		printf("d mod (q - 1) CRT param ");printfBigInt(mp_dQ,m_dQ_length);
	}
	if(m_private == false)
	{
		printf("RSA key is public\n");
		printf("e ");printfBigInt(mp_e,m_e_length);printf("\n");
		printf("N ");printfBigInt(mp_N,m_N_length);printf("\n");
	}

}

void RSAKey::printfBigInt(unsigned char* data, long long length)
{
	for(long long i=0;i<length;++i)
	{
			printf("%02x ", data[i]);
	}
}

#endif




#ifdef NOTOMCRYPT
  
RSAKey::RSAKey() {
}

RSAKey::~RSAKey()
{

}

void RSAKey::getTFMint(void* data, unsigned char** destination, long long* length)
{
}

void RSAKey::printKey()
{
}

void RSAKey::printfBigInt(unsigned char* data, long long length)
{
}

#endif


} /* namespace Lazarus */
