/*
 * CipherCascade.cpp
 *
 *  Created on: 15.12.2014
 *      Author: Darius Malysiak
 */

#include "CipherCascade.h"

namespace Lazarus {

CipherCascade::CipherCascade() {
	// TODO Auto-generated constructor stub

}

CipherCascade::~CipherCascade() {
	// TODO Auto-generated destructor stub
}

int CipherCascade::encrypt(KTuple<CipherBox*>* ciphers, Buffer* input, Buffer* output)
{
	Buffer temp(output->get_m_length());

	if(ciphers->getm_size() == 0)
	{
		printf("ERROR: no ciphers in list\n");
		return -1;
	}

	long long int i=1;

	//first encryption
	ciphers->getElement(0)->encryptBuffer(input,&temp);

	for(;i<ciphers->getm_size();++i)
	{
		//if index odd
		if(i % 2 != 0)
		{
			ciphers->getElement(i)->encryptBuffer(&temp,output);
		}
		else //if even
		{
			ciphers->getElement(i)->encryptBuffer(output,&temp);
		}
	}

	//check if ended with an even index; if so then swap the data
	if(i % 2 == 0)
	{
		memcpy(output->get_mp_data(),temp.get_mp_data(),output->get_m_length());
	}

	return 0;
}

int CipherCascade::decrypt(KTuple<CipherBox*>* ciphers, Buffer* input, Buffer* output)
{
	Buffer temp(output->get_m_length());

	if(ciphers->getm_size() == 0)
	{
		printf("ERROR: no ciphers in list\n");
		return -1;
	}

	long long int i=1;

	//first encryption
	ciphers->getElement(0)->encryptBuffer(input,&temp);

	for(;i<ciphers->getm_size();++i)
	{
		//if index odd
		if(i % 2 != 0)
		{
			ciphers->getElement(i)->decryptBuffer(&temp,output);
		}
		else //if even
		{
			ciphers->getElement(i)->decryptBuffer(output,&temp);
		}
	}

	//check if ended with an even index; if so then swap the data
	if(i % 2 == 0)
	{
		memcpy(output->get_mp_data(),temp.get_mp_data(),output->get_m_length());
	}

	return 0;
}

int CipherCascade::encryptFileInPlaceRaw(KTuple<CipherBox*>* ciphers, std::string file)
{
	if(ciphers->getm_size() == 0)
	{
		printf("ERROR: no ciphers in list\n");
		return -1;
	}

	for(long long int i=0;i<ciphers->getm_size();++i)
	{
			ciphers->getElement(i)->encryptFileInPlaceRaw(file);
	}

	return 0;
}


int CipherCascade::decryptFileInPlaceRaw(KTuple<CipherBox*>* ciphers, std::string file)
{
	if(ciphers->getm_size() == 0)
	{
		printf("ERROR: no ciphers in list\n");
		return -1;
	}

	for(long long int i=0;i<ciphers->getm_size();++i)
	{
			ciphers->getElement(i)->decryptFileInPlaceRaw(file);
	}

	return 0;
}

} /* namespace Lazarus */
