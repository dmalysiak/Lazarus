/*
 * MD5Hash.cpp
 *
 *  Created on: 15.12.2014
 *      Author: Darius Malysiak
 */

#include "MD5HashFunction.h"

namespace Lazarus {

MD5HashFunction::MD5HashFunction() {
#ifdef USETOMCRYPT
	m_hashsize = 16;

	/* register hash function*/
	if (register_hash(&md5_desc) == -1)
	{
		printf("Error registering MD5.\n");
		return;
	}

	m_ready = true;
	m_hash_idx = find_hash("md5");

	//reset/init the hash state
	reset();
#endif
#ifdef NOTOMCRYPT
	printf("ERROR: no tomcrypt support\n");
#endif
}

MD5HashFunction::~MD5HashFunction() {
	// TODO Auto-generated destructor stub
}


BasicBuffer* MD5HashFunction::getSingleHash(BasicBuffer* input)
{
#ifdef USETOMCRYPT
	//sanity check
	if(m_ready == false)
	{
		printf("ERROR: hash system not ready\n");
		return NULL;
	}

	//reset/init the hash state
	reset();

	BasicBuffer* output = new BasicBuffer(m_hashsize);

	//process the input
	md5_process(&m_hash_state, input->get_mp_data(), input->get_m_length() );

	//get the hash
	md5_done(&m_hash_state, output->get_mp_data());


	return output;
#endif
#ifdef NOTOMCRYPT
	printf("ERROR: no tomcrypt support\n");
	return NULL;
#endif
}

int MD5HashFunction::getSingleHash(BasicBuffer* input, BasicBuffer* output)
{
#ifdef USETOMCRYPT
	//sanity check
	if(m_ready == false)
	{
		printf("ERROR: hash system not ready\n");
		return -1;
	}

	//reset/init the hash state
	reset();

	//second check
	if(output->get_m_length()<m_hashsize)
	{
		printf("ERROR: output buffer must have a size of at least the digest length\n");
		return -1;
	}

	//process the input
	md5_process(&m_hash_state, input->get_mp_data(), input->get_m_length() );

	//get the hash
	md5_done(&m_hash_state, output->get_mp_data());

	return 0;
#endif
#ifdef NOTOMCRYPT
	printf("ERROR: no tomcrypt support\n");
	return 0;
#endif
}


BasicBuffer* MD5HashFunction::getSingleFileHash(const std::string& input_file)
{
#ifdef USETOMCRYPT
	//sanity check
	if(m_ready == false)
	{
		printf("ERROR: hash system not ready\n");
		return NULL;
	}

	//reset/init the hash state
	reset();

	BasicBuffer* output = new BasicBuffer(m_hashsize);

	//hash the file
	unsigned long int size = output->get_m_length();
	hash_file( find_hash("md5"), input_file.c_str(), output->get_mp_data(), &size);

	return output;
#endif
#ifdef NOTOMCRYPT
	printf("ERROR: no tomcrypt support\n");
	return NULL;
#endif
}

int MD5HashFunction::getSingleFileHash(const std::string& input_file, BasicBuffer* output)
{
#ifdef USETOMCRYPT
	//sanity check
	if(m_ready == false)
	{
		printf("ERROR: hash system not ready\n");
		return -1;
	}

	//reset/init the hash state
	reset();

	//second check
	if(output->get_m_length()<m_hashsize)
	{
		printf("ERROR: output buffer must have a size of at least the digest length\n");
		return -1;
	}

	//hash the file
	unsigned long int size = output->get_m_length();
	hash_file( find_hash("md5"), input_file.c_str(), output->get_mp_data(), &size);

	return 0;
#endif
#ifdef NOTOMCRYPT
	printf("ERROR: no tomcrypt support\n");
	return 0;
#endif
}


int MD5HashFunction::process(BasicBuffer* input)
{
#ifdef USETOMCRYPT
	//sanity check
	if(m_ready == false)
	{
		printf("ERROR: hash system not ready\n");
		return -1;
	}

	//process the input
	md5_process(&m_hash_state, input->get_mp_data(), input->get_m_length() );

	return 0;
#endif
#ifdef NOTOMCRYPT
	printf("ERROR: no tomcrypt support\n");
	return 0;
#endif

}


BasicBuffer* MD5HashFunction::getHash()
{
#ifdef USETOMCRYPT
	//sanity check
	if(m_ready == false)
	{
		printf("ERROR: hash system not ready\n");
		return NULL;
	}

	BasicBuffer* output = new BasicBuffer(m_hashsize);

	//get the hash
	md5_done(&m_hash_state, output->get_mp_data());

	reset();

	return output;
#endif
#ifdef NOTOMCRYPT
	printf("ERROR: no tomcrypt support\n");
	return NULL;
#endif
}


int MD5HashFunction::getHash(BasicBuffer* output)
{
#ifdef USETOMCRYPT
	//sanity check
	if(m_ready == false)
	{
		printf("ERROR: hash system not ready\n");
		return -1;
	}

	//second check
	if(output->get_m_length()<m_hashsize)
	{
		printf("ERROR: output buffer must have a size of at least the digest length\n");
		return -1;
	}

	//get the hash
	md5_done(&m_hash_state, output->get_mp_data());

	reset();

	return 0;
#endif
#ifdef NOTOMCRYPT
	printf("ERROR: no tomcrypt support\n");
	return 0;
#endif
}


void MD5HashFunction::reset()
{
#ifdef USETOMCRYPT
	//sanity check
	if(m_ready == false)
	{
		printf("ERROR: hash system not ready\n");
	}

	/* setup the hash */
	md5_init(&m_hash_state);
#endif
#ifdef NOTOMCRYPT
	printf("ERROR: no tomcrypt support\n");
#endif
}

} /* namespace Lazarus */
