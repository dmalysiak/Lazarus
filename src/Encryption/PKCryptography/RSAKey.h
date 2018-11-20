/*
 * RSAKey.h
 *
 *  Created on: 19.12.2014
 *      Author: Darius Malysiak
 */

#ifndef RSAKEY_H_
#define RSAKEY_H_

#include "../../BaseObject.h"

#ifdef USETOMCRYPT
#include <tomcrypt.h>
#endif

namespace Lazarus {

class RSAKey: public BaseObject {
public:
#ifdef USETOMCRYPT
	RSAKey(rsa_key* key);
#endif
#ifdef NOTOMCRYPT
	RSAKey();
#endif
	virtual ~RSAKey();

	/*
	 * The RSA key attributes.
	 */
	bool m_private;

	unsigned char* mp_e;
	unsigned char* mp_d;
	unsigned char* mp_N;
	unsigned char* mp_p; //p factor of N
	unsigned char* mp_q; //q factor of N
	unsigned char* mp_qP; // 1/q mod p CRT param
	unsigned char* mp_dP; //d mod (p - 1) CRT param
	unsigned char* mp_dQ; //d mod (q - 1) CRT param

	//length in bytes
	long long m_e_length;
	long long m_d_length;
	long long m_N_length;
	long long m_p_length;
	long long m_q_length;
	long long m_qP_length;
	long long m_dP_length;
	long long m_dQ_length;

	/**
	 * outputs stats about the generated key
	 */
	void printKey();

private:
	void getTFMint(void* data, unsigned char** destination, long long* length);
	void printfBigInt(unsigned char* data, long long length);
};

} /* namespace Lazarus */

#endif /* RSAKEY_H_ */
