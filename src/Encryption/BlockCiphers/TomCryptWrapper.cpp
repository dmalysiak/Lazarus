/*
 * TomCryptWrapper.cpp
 *
 *  Created on: 15.12.2014
 *      Author: Darius Malysiak
 */

#include "TomCryptWrapper.h"
#include "../../Toolbox.h"

#include <stdio.h>
#include <math.h>

namespace Lazarus {

#ifdef USETOMCRYPT
  
TomCryptWrapper::TomCryptWrapper() {

	m_ready = false;
	m_IV = NULL;
	m_key = NULL;
	m_cipher_rounds = 0;
	m_key_size = 0;
	m_block_size = 0;

}

TomCryptWrapper::~TomCryptWrapper() {
	// TODO Auto-generated destructor stub
}


int TomCryptWrapper::resetIV(enum CHAINING_MODE chaining_mode)
{
	//stop if system not ready
	if(m_ready == false)
	{
		printf("ERROR: IV reset requires a ready system\n");
		return -1;
	}

	int err = 0;

	switch(chaining_mode)
	{
	case CHAINING_MODE_CTR:

		if((err = ctr_setiv( m_IV, m_block_size, &m_ctr) ) != CRYPT_OK)
		{
			printf("ctr_setiv error: %s\n", error_to_string(err));
			return -1;
		}

		break;

	case CHAINING_MODE_CBC:

		if((err = cbc_setiv( m_IV, m_block_size, &m_cbc) ) != CRYPT_OK)
		{
			printf("cbc_setiv error: %s\n", error_to_string(err));
			return -1;
		}

		break;

	case CHAINING_MODE_ECB:

		printf("ecb_setiv does not exist :-)\n");
		return -1;

		break;

	case CHAINING_MODE_UNKNOWN:
		printf("Error resetting %s-XXX IV: unknown chaining mode\n",m_cipher_name.c_str());
		return -1;
		break;

	default:
		printf("Error resetting %s-XXX IV: unknown chaining mode\n",m_cipher_name.c_str());
		return -1;
		break;

	}

	return 0;
}


int TomCryptWrapper::terminateChain(enum CHAINING_MODE chaining_mode)
{
	int err = 0;

	switch(chaining_mode)
	{
	case CHAINING_MODE_CTR:

		/* terminate the stream */
		if ((err = ctr_done(&m_ctr)) != CRYPT_OK)
		{
			printf("ERROR closing ctr stream: %s\n", error_to_string(err));
			return -1;
		}

		break;

	case CHAINING_MODE_CBC:

		/* terminate the stream */
		if ((err = cbc_done(&m_cbc)) != CRYPT_OK)
		{
			printf("ERROR closing cbc stream: %s\n", error_to_string(err));
			return -1;
		}

		break;

	case CHAINING_MODE_ECB:

		/* terminate the stream */
		if ((err = ecb_done(&m_ecb)) != CRYPT_OK)
		{
			printf("ERROR closing ecb stream: %s\n", error_to_string(err));
			return -1;
		}

		break;

	case CHAINING_MODE_UNKNOWN:
		printf("Error closing %s-XXX mode: unknown chaining mode\n",m_cipher_name.c_str());
		m_ready = false;
		return -1;
		break;

	default:
		printf("Error closing %s-XXX mode: unknown chaining mode\n",m_cipher_name.c_str());
		m_ready = false;
		return -1;
		break;

	}

	return 0;
}

int TomCryptWrapper::encryptFile(std::string input_file, std::string output_file)
{

	//stop if system not ready
	if(m_ready == false)
	{
		printf("ERROR: encryption requires a ready system\n");
		return -1;
	}

	//how large should the read buffer be? (must be aligned with the block size, i.e. m_block_size bytes)
	unsigned int buffer_size = 131072;
	unsigned char* input_buffer = new unsigned char[buffer_size];
	unsigned char* output_buffer = new unsigned char[buffer_size];

	//wrapper objects on the stack, these will also free the buffers
	Buffer input_buffer_w(input_buffer,buffer_size,false);
	Buffer output_buffer_w(output_buffer,buffer_size,false);

	memset(input_buffer,0,buffer_size);

	//reset the IV before encrypting the stream
	if(m_chaining_mode != CHAINING_MODE_ECB)
	{
		resetIV(m_chaining_mode);
	}

	//open the files
	FILE* fp_input = fopen(input_file.c_str(), "rb");
	FILE* fp_output = fopen(output_file.c_str(), "wb+");

	if(fp_input == NULL)
	{
		printf("ERROR: could not open input file\n");
		return -1;
	}
	if(fp_output == NULL)
	{
		printf("ERROR: could not open output file\n");
		return -1;
	}

	//write the meta data to the file
	long int filesize = Toolbox::getFileSize(fp_input);
	fwrite(&filesize , sizeof(long int), 1, fp_output);

	//begin encryption
	int c = 0;
	unsigned int counter = 0;

	c = getc(fp_input);

	while(c != EOF)
	{
		if(counter == buffer_size)
		{
			//encrypt the buffer
			encryptBufferNoIVReset(&input_buffer_w,&output_buffer_w);

			//write the ciphertext to the output file
			fwrite(output_buffer_w.get_mp_data() , sizeof(unsigned char), buffer_size, fp_output);

			memset(input_buffer,0,buffer_size);//clean the buffer
			counter = 0;
		}

		input_buffer[counter] = c;
		c = getc(fp_input);
		++counter;
	}

	//check for a left over fragment, i.e. bytes left in the unfilled cache
	if(counter != 0)
	{
		//find the smallest block-aligned size for the remaining bytes
		unsigned int aligned_size = ceil((double)counter/(double)m_block_size)*m_block_size;

		Buffer input_buffer_w2(input_buffer,aligned_size,true);
		Buffer output_buffer_w2(aligned_size);

		//encrypt and write the last segment
		encryptBufferNoIVReset(&input_buffer_w2,&output_buffer_w2);

		//write the ciphertext to the output file
		fwrite (output_buffer_w2.get_mp_data() , sizeof(unsigned char), aligned_size, fp_output);

		counter = 0;
	}

	//clean up
	fclose(fp_input);
	fclose(fp_output);

	return 0;
}



int TomCryptWrapper::decryptFile(std::string input_file, std::string output_file)
{

	//stop if system not ready
	if(m_ready == false)
	{
		printf("ERROR: decryption requires a ready system\n");
		return -1;
	}

	//how large should the read buffer be? (must be aligned with the block size, i.e. m_block_size bytes)
	unsigned int buffer_size = 131072;
	unsigned char* input_buffer = new unsigned char[buffer_size];
	unsigned char* output_buffer = new unsigned char[buffer_size];

	//wrapper objects on the stack, these will also free the buffers
	Buffer input_buffer_w(input_buffer,buffer_size,false);
	Buffer output_buffer_w(output_buffer,buffer_size,false);

	memset(input_buffer,0,buffer_size);


	//reset the IV before decrypting the stream
	if(m_chaining_mode != CHAINING_MODE_ECB)
	{
		resetIV(m_chaining_mode);
	}

	//open the files
	FILE* fp_input = fopen(input_file.c_str(), "rb");
	FILE* fp_output = fopen(output_file.c_str(), "wb+");

	if(fp_input == NULL)
	{
		printf("ERROR: could not open input file\n");
		return -1;
	}
	if(fp_output == NULL)
	{
		printf("ERROR: could not open output file\n");
		return -1;
	}

	//read the meta data from the file
	long int filesize = 0;
	int read_elements = fread(&filesize , sizeof(long int), 1, fp_input);
	if(read_elements == 0)
	{
		printf("ERROR: could not read meta data from encrypted file\n");
		fclose(fp_input);
		fclose(fp_output);
		return -1;
	}

	//begin encryption
	int c = 0;
	unsigned int counter = 0;
	long int buffer_hits = 0;

	c = getc(fp_input);

	while(c != EOF)
	{
		if(counter == buffer_size)
		{
			//encrypt the buffer
			decryptBufferNoIVReset(&input_buffer_w,&output_buffer_w);

			//write the ciphertext to the output file
			fwrite(output_buffer_w.get_mp_data() , sizeof(unsigned char), buffer_size, fp_output);

			memset(input_buffer,0,buffer_size);//clean the buffer
			counter = 0;
			++buffer_hits;
		}

		input_buffer[counter] = c;
		c = getc(fp_input);
		++counter;
	}

	//check for a left over fragment, i.e. bytes left in the unfilled cache. The value of 'counter' will be a multiple of m_block_size!!
	if(counter != 0)
	{
		Buffer input_buffer_w2(input_buffer,counter,true);
		Buffer output_buffer_w2(counter);

		//encrypt and write the last segment
		decryptBufferNoIVReset(&input_buffer_w2,&output_buffer_w2);

		//write the ciphertext to the output file
		fwrite(output_buffer_w2.get_mp_data() , sizeof(unsigned char), filesize - buffer_hits*buffer_size  , fp_output);

		counter = 0;
	}

	//clean up
	fclose(fp_input);
	fclose(fp_output);

	return 0;
}


int TomCryptWrapper::encryptFileRaw(std::string input_file, std::string output_file)
{
	//stop if system not ready
	if(m_ready == false)
	{
		printf("ERROR: encryption requires a ready system\n");
		return -1;
	}

	//check if partial blocks can be handled (add other ciphers at this point)
	if(m_chaining_mode != CHAINING_MODE_CTR)
	{
		printf("ERROR: in-place encryption requires a cipher capable of partial blocks, e.g. CTR\n");
		return -1;
	}

	//how large should the read buffer be? (must be aligned with the block size, i.e. m_block_size bytes)
	unsigned int buffer_size = 131072;
	unsigned char* input_buffer = new unsigned char[buffer_size];
	unsigned char* output_buffer = new unsigned char[buffer_size];

	//wrapper objects on the stack, these will also free the buffers
	Buffer input_buffer_w(input_buffer,buffer_size,false);
	Buffer output_buffer_w(output_buffer,buffer_size,false);

	memset(input_buffer,0,buffer_size);


	//reset the IV before encrypting the stream
	if(m_chaining_mode != CHAINING_MODE_ECB)
	{
		resetIV(m_chaining_mode);
	}

	//open the files
	FILE* fp_input = fopen(input_file.c_str(), "rb");
	FILE* fp_output = fopen(output_file.c_str(), "wb+");

	if(fp_input == NULL)
	{
		printf("ERROR: could not open input file\n");
		return -1;
	}
	if(fp_output == NULL)
	{
		printf("ERROR: could not open output file\n");
		return -1;
	}

	//begin encryption
	int c = 0;
	unsigned int counter = 0;

	c = getc(fp_input);

	while(c != EOF)
	{
		if(counter == buffer_size)
		{
			//encrypt the buffer
			encryptBufferNoIVReset(&input_buffer_w,&output_buffer_w);

			//write the ciphertext to the output file
			fwrite(output_buffer_w.get_mp_data() , sizeof(unsigned char), buffer_size, fp_output);

			memset(input_buffer,0,buffer_size);//clean the buffer
			counter = 0;
		}

		input_buffer[counter] = c;
		c = getc(fp_input);
		++counter;
	}

	//check for a left over fragment, i.e. bytes left in the unfilled cache
	if(counter != 0)
	{
		//find the smallest block-aligned size for the remaining bytes
		unsigned int aligned_size = ceil((double)counter/(double)m_block_size)*m_block_size;

		Buffer input_buffer_w2(input_buffer,aligned_size,true);
		Buffer output_buffer_w2(aligned_size);

		//encrypt and write the last segment
		encryptBufferNoIVReset(&input_buffer_w2,&output_buffer_w2);

		//write the ciphertext to the output file
		fwrite (output_buffer_w2.get_mp_data() , sizeof(unsigned char), counter, fp_output);//aligned_size

		counter = 0;
	}

	//clean up
	fclose(fp_input);
	fclose(fp_output);

	return 0;
}



int TomCryptWrapper::decryptFileRaw(std::string input_file, std::string output_file)
{
	//stop if system not ready
	if(m_ready == false)
	{
		printf("ERROR: decryption requires a ready system\n");
		return -1;
	}

	//check if partial blocks can be handled (add other ciphers at this point)
	if(m_chaining_mode != CHAINING_MODE_CTR)
	{
		printf("ERROR: in-place decryption requires a cipher capable of partial blocks, e.g. CTR\n");
		return -1;
	}

	//how large should the read buffer be? (must be aligned with the block size, i.e. m_block_size bytes)
	unsigned int buffer_size = 131072;
	unsigned char* input_buffer = new unsigned char[buffer_size];
	unsigned char* output_buffer = new unsigned char[buffer_size];

	//wrapper objects on the stack, these will also free the buffers
	Buffer input_buffer_w(input_buffer,buffer_size,false);
	Buffer output_buffer_w(output_buffer,buffer_size,false);

	memset(input_buffer,0,buffer_size);

	//reset the IV before decrypting the stream
	if(m_chaining_mode != CHAINING_MODE_ECB)
	{
		resetIV(m_chaining_mode);
	}

	//open the files
	FILE* fp_input = fopen(input_file.c_str(), "rb");
	FILE* fp_output = fopen(output_file.c_str(), "wb+");

	if(fp_input == NULL)
	{
		printf("ERROR: could not open input file\n");
		return -1;
	}
	if(fp_output == NULL)
	{
		printf("ERROR: could not open output file\n");
		return -1;
	}

	//begin encryption
	int c = 0;
	unsigned int counter = 0;
	long int buffer_hits = 0;

	c = getc(fp_input);

	while(c != EOF)
	{
		if(counter == buffer_size)
		{
			//encrypt the buffer
			decryptBufferNoIVReset(&input_buffer_w,&output_buffer_w);

			//write the ciphertext to the output file
			fwrite(output_buffer_w.get_mp_data() , sizeof(unsigned char), buffer_size, fp_output);

			memset(input_buffer,0,buffer_size);//clean the buffer
			counter = 0;
			++buffer_hits;
		}

		input_buffer[counter] = c;
		c = getc(fp_input);
		++counter;
	}

	//check for a left over fragment, i.e. bytes left in the unfilled cache. The value of 'counter' >won't< be a multiple of m_block_size!!
	if(counter != 0)
	{
		//find the smallest block-aligned size for the remaining bytes
		unsigned int aligned_size = ceil((double)counter/(double)m_block_size)*m_block_size;

		Buffer input_buffer_w2(input_buffer,aligned_size,true);
		Buffer output_buffer_w2(aligned_size);

		//encrypt and write the last segment
		decryptBufferNoIVReset(&input_buffer_w2,&output_buffer_w2);

		//write the ciphertext to the output file
		fwrite(output_buffer_w2.get_mp_data() , sizeof(unsigned char), counter, fp_output);

		counter = 0;
	}

	//clean up
	fclose(fp_input);
	fclose(fp_output);

	return 0;
}


int TomCryptWrapper::encryptFileInPlaceRaw(std::string input_file)
{
	//stop if system not ready
	if(m_ready == false)
	{
		printf("ERROR: encryption requires a ready system\n");
		return -1;
	}

	//check if partial blocks can be handled (add other ciphers at this point)
	if(m_chaining_mode != CHAINING_MODE_CTR)
	{
		printf("ERROR: in-place encryption requires a cipher capable of partial blocks, e.g. CTR\n");
		return -1;
	}

	//how large should the read buffer be? (must be aligned with the block size, i.e. m_block_size bytes)
	unsigned int buffer_size = 131072;
	unsigned char* input_buffer = new unsigned char[buffer_size];
	unsigned char* output_buffer = new unsigned char[buffer_size];

	//wrapper objects on the stack, these will also free the buffers
	Buffer input_buffer_w(input_buffer,buffer_size,false);
	Buffer output_buffer_w(output_buffer,buffer_size,false);

	memset(input_buffer,0,buffer_size);


	//reset the IV before encrypting the stream
	if(m_chaining_mode != CHAINING_MODE_ECB)
	{
		resetIV(m_chaining_mode);
	}

	//open the files
	FILE* fp_input = fopen(input_file.c_str(), "rb+");

	if(fp_input == NULL)
	{
		printf("ERROR: could not open input file\n");
		return -1;
	}

	//begin encryption
	int c = 0;
	unsigned int counter = 0;

	c = getc(fp_input);

	while(c != EOF)
	{
		if(counter == buffer_size)
		{
			//encrypt the buffer
			encryptBufferNoIVReset(&input_buffer_w,&output_buffer_w);

			//step back one chunk
			fseek( fp_input, -((long int)buffer_size) ,SEEK_CUR);

			//write the ciphertext to the input file
			fwrite(output_buffer_w.get_mp_data() , sizeof(unsigned char), buffer_size, fp_input);

			//restore position
			fseek( fp_input, buffer_size ,SEEK_CUR);

			memset(input_buffer,0,buffer_size);//clean the buffer
			counter = 0;

		}

		input_buffer[counter] = c;
		c = getc(fp_input);
		++counter;
	}

	//check for a left over fragment, i.e. bytes left in the unfilled cache
	if(counter != 0)
	{
		//find the smallest block-aligned size for the remaining bytes
		unsigned int aligned_size = ceil((double)counter/(double)m_block_size)*m_block_size;

		Buffer input_buffer_w2(input_buffer,aligned_size,true);
		Buffer output_buffer_w2(aligned_size);

		//encrypt and write the last segment
		encryptBufferNoIVReset(&input_buffer_w2,&output_buffer_w2);

		//step back one chunk
		fseek( fp_input, -((long int)counter) ,SEEK_CUR);

		//write the ciphertext to the output file
		fwrite (output_buffer_w2.get_mp_data() , sizeof(unsigned char), counter, fp_input);//aligned_size

		//restore position
		fseek( fp_input, counter ,SEEK_CUR);

		counter = 0;
	}

	//clean up
	fclose(fp_input);

	return 0;
}



int TomCryptWrapper::decryptFileInPlaceRaw(std::string input_file)
{
	//stop if system not ready
	if(m_ready == false)
	{
		printf("ERROR: decryption requires a ready system\n");
		return -1;
	}

	//check if partial blocks can be handled (add other ciphers at this point)
	if(m_chaining_mode != CHAINING_MODE_CTR)
	{
		printf("ERROR: in-place decryption requires a cipher capable of partial blocks, e.g. CTR\n");
		return -1;
	}

	//how large should the read buffer be? (must be aligned with the block size, i.e. m_block_size bytes)
	unsigned int buffer_size = 131072;
	unsigned char* input_buffer = new unsigned char[buffer_size];
	unsigned char* output_buffer = new unsigned char[buffer_size];

	//wrapper objects on the stack, these will also free the buffers
	Buffer input_buffer_w(input_buffer,buffer_size,false);
	Buffer output_buffer_w(output_buffer,buffer_size,false);

	memset(input_buffer,0,buffer_size);

	//reset the IV before decrypting the stream
	if(m_chaining_mode != CHAINING_MODE_ECB)
	{
		resetIV(m_chaining_mode);
	}

	//open the files
	FILE* fp_input = fopen(input_file.c_str(), "rb+");

	if(fp_input == NULL)
	{
		printf("ERROR: could not open input file\n");
		return -1;
	}

	//begin encryption
	int c = 0;
	unsigned int counter = 0;
	long int buffer_hits = 0;

	c = getc(fp_input);

	while(c != EOF)
	{
		if(counter == buffer_size)
		{
			//encrypt the buffer
			decryptBufferNoIVReset(&input_buffer_w,&output_buffer_w);

			//step back one chunk
			fseek( fp_input, -((long int)buffer_size) ,SEEK_CUR);

			//write the ciphertext to the output file
			fwrite(output_buffer_w.get_mp_data() , sizeof(unsigned char), buffer_size, fp_input);

			//restore position
			fseek( fp_input, buffer_size ,SEEK_CUR);

			memset(input_buffer,0,buffer_size);//clean the buffer
			counter = 0;
			++buffer_hits;
		}

		input_buffer[counter] = c;
		c = getc(fp_input);
		++counter;
	}

	//check for a left over fragment, i.e. bytes left in the unfilled cache. The value of 'counter' >won't< be a multiple of m_block_size!!
	if(counter != 0)
	{
		//find the smallest block-aligned size for the remaining bytes
		unsigned int aligned_size = ceil((double)counter/(double)m_block_size)*m_block_size;

		Buffer input_buffer_w2(input_buffer,aligned_size,true);
		Buffer output_buffer_w2(aligned_size);

		//encrypt and write the last segment
		decryptBufferNoIVReset(&input_buffer_w2,&output_buffer_w2);

		//step back one chunk
		fseek( fp_input, -((long int)counter) ,SEEK_CUR);

		//write the ciphertext to the output file
		fwrite(output_buffer_w2.get_mp_data() , sizeof(unsigned char), counter, fp_input);

		//restore position
		fseek( fp_input, counter ,SEEK_CUR);

		counter = 0;
	}

	//clean up
	fclose(fp_input);

	return 0;
}


int TomCryptWrapper::encryptBuffer(BasicBuffer* input_buffer, BasicBuffer* output_buffer)
{
	//stop if system not ready
	if(m_ready == false)
	{
		printf("ERROR: encryption requires a ready system\n");
		return -1;
	}

	//first sanity check
	if(input_buffer->get_m_length() != output_buffer->get_m_length())
	{
		printf("ERROR: encryption requires buffers of the same size\n");
		return -1;
	}

	int err = 0;
	//the new size is a multiple of the block length, i.e. m_block_size bytes.
	unsigned int cipher_size = ceil((double)input_buffer->get_m_length()/(double)m_block_size)*m_block_size;

	//second sanity check (i.e. block-alignment)
	if(cipher_size != input_buffer->get_m_length())
	{
		printf("ERROR: encryption requires aligned buffers %u %u\n",cipher_size,input_buffer->get_m_length());
		return -1;
	}

	switch(m_chaining_mode)
	{
	case CHAINING_MODE_CTR:

		resetIV(m_chaining_mode);

		if( (err = ctr_encrypt(input_buffer->get_mp_data(), output_buffer->get_mp_data(), cipher_size, &m_ctr))  != CRYPT_OK )
		{
			printf("Error encrypting in %s-ctr mode: %s\n",m_cipher_name.c_str(), error_to_string(err));
			return -1;
		}

		break;

	case CHAINING_MODE_CBC:

		resetIV(m_chaining_mode);

		if( (err = cbc_encrypt(input_buffer->get_mp_data(), output_buffer->get_mp_data(), cipher_size, &m_cbc))  != CRYPT_OK )
		{
			printf("Error encrypting in %s-cbc mode: %s\n",m_cipher_name.c_str(), error_to_string(err));
			return -1;
		}

		break;

	case CHAINING_MODE_ECB:

		if( (err = ecb_encrypt(input_buffer->get_mp_data(), output_buffer->get_mp_data(), cipher_size, &m_ecb))  != CRYPT_OK )
		{
			printf("Error encrypting in %s-ecb mode: %s\n",m_cipher_name.c_str(), error_to_string(err));
			return -1;
		}

		break;

	case CHAINING_MODE_UNKNOWN:
		printf("Error encrypting in %s-XXX mode: unknown chaining mode\n",m_cipher_name.c_str());
		return -1;

	default:
		printf("Error encrypting in %s-XXX mode: unknown chaining mode\n",m_cipher_name.c_str());
		return -1;

	}

	return 0;
}

int TomCryptWrapper::decryptBuffer(BasicBuffer* input_buffer, BasicBuffer* output_buffer)
{
	//stop if system not ready
	if(m_ready == false)
	{
		printf("ERROR: decryption requires a ready system\n");
		return -1;
	}

	//first sanity check
	if(input_buffer->get_m_length() != output_buffer->get_m_length())
	{
		printf("ERROR: decryption requires buffers of the same size\n");
		return -1;
	}

	int err = 0;
	//the new size is a multiple of the block length, i.e. m_block_size bytes.
	unsigned int cipher_size = ceil((double)input_buffer->get_m_length()/(double)m_block_size)*m_block_size;

	//second sanity check (i.e. block-alignment)
	if(cipher_size != input_buffer->get_m_length())
	{
		printf("ERROR: decryption requires aligned buffers %u %u\n",cipher_size,input_buffer->get_m_length());
		return -1;
	}

	switch(m_chaining_mode)
	{
	case CHAINING_MODE_CTR:

		resetIV(m_chaining_mode);

		if( (err = ctr_decrypt(input_buffer->get_mp_data(), output_buffer->get_mp_data(), cipher_size, &m_ctr))  != CRYPT_OK )
		{
			printf("Error decrypting in %s-ctr mode: %s\n",m_cipher_name.c_str(), error_to_string(err));
			return -1;
		}

		break;

	case CHAINING_MODE_CBC:

		resetIV(m_chaining_mode);

		if( (err = cbc_decrypt(input_buffer->get_mp_data(), output_buffer->get_mp_data(), cipher_size, &m_cbc))  != CRYPT_OK )
		{
			printf("Error decrypting in %s-cbc mode: %s\n",m_cipher_name.c_str(), error_to_string(err));
			return -1;
		}

		break;

	case CHAINING_MODE_ECB:

		if( (err = ecb_decrypt(input_buffer->get_mp_data(), output_buffer->get_mp_data(), cipher_size, &m_ecb))  != CRYPT_OK )
		{
			printf("Error decrypting in %s-ecb mode: %s\n",m_cipher_name.c_str(), error_to_string(err));
			return -1;
		}

		break;

	case CHAINING_MODE_UNKNOWN:
		printf("Error decrypting in %s-XXX mode: unknown chaining mode\n",m_cipher_name.c_str());
		return -1;

	default:
		printf("Error decrypting in %s-XXX mode: unknown chaining mode\n",m_cipher_name.c_str());
		return -1;

	}

	return 0;
}


int TomCryptWrapper::encryptBufferRaw(BasicBuffer* input_buffer, BasicBuffer* output_buffer)
{
	//stop if system not ready
	if(m_ready == false)
	{
		printf("ERROR: encryption requires a ready system\n");
		return -1;
	}

	//first sanity check
	if(input_buffer->get_m_length() != output_buffer->get_m_length())
	{
		printf("ERROR: encryption requires buffers of the same size\n");
		return -1;
	}

	//check if partial blocks can be handled (add other ciphers at this point)
	if(m_chaining_mode != CHAINING_MODE_CTR)
	{
		printf("ERROR: raw encryption requires a cipher capable of partial blocks, e.g. CTR\n");
		return -1;
	}

	int err = 0;

	unsigned int full_blocks = floor((double)input_buffer->get_m_length()/(double)m_block_size);
	unsigned int full_block_fragment = full_blocks*m_block_size;

	unsigned int fraction = input_buffer->get_m_length() - full_block_fragment;
	unsigned char* input_data = input_buffer->get_mp_data();
	unsigned char* output_data = output_buffer->get_mp_data();

	switch(m_chaining_mode)
	{
	case CHAINING_MODE_CTR:

		resetIV(m_chaining_mode);

		//the largest part
		if(full_block_fragment != 0)
		if( (err = ctr_encrypt(input_data, output_data, full_block_fragment, &m_ctr))  != CRYPT_OK )
		{
			printf("Error encrypting in %s-ctr mode: %s\n",m_cipher_name.c_str(), error_to_string(err));
			return -1;
		}

		//the fraction, i.e. last block
		if(fraction != 0)
		{
			//copy data into block segment
			unsigned char temp_plain[m_block_size];
			unsigned char temp_cipher[m_block_size];
			//due to the small block sizes we avoid memcpys
			for(unsigned int i=0;i<fraction;++i)
			{
				temp_plain[i]= (input_data+full_block_fragment)[i];
			}
			//encrypt
			if( (err = ctr_encrypt(temp_plain, temp_cipher, m_block_size, &m_ctr))  != CRYPT_OK )
			{
				printf("Error encrypting fragment in %s-ctr mode: %s\n",m_cipher_name.c_str(), error_to_string(err));
				return -1;
			}
			//copy fragment into output
			for(unsigned int i=0;i<fraction;++i)
			{
				(output_data+full_block_fragment)[i] = temp_cipher[i];
			}
		}

		break;

	case CHAINING_MODE_UNKNOWN:
		printf("Error raw encrypting in %s-XXX mode: unknown chaining mode\n",m_cipher_name.c_str());
		return -1;

	default:
		printf("Error raw encrypting in %s-XXX mode: unknown chaining mode\n",m_cipher_name.c_str());
		return -1;

	}

	return 0;
}

int TomCryptWrapper::decryptBufferRaw(BasicBuffer* input_buffer, BasicBuffer* output_buffer)
{
	//stop if system not ready
	if(m_ready == false)
	{
		printf("ERROR: decryption requires a ready system\n");
		return -1;
	}

	//first sanity check
	if(input_buffer->get_m_length() != output_buffer->get_m_length())
	{
		printf("ERROR: decryption requires buffers of the same size\n");
		return -1;
	}

	//check if partial blocks can be handled (add other ciphers at this point)
	if(m_chaining_mode != CHAINING_MODE_CTR)
	{
		printf("ERROR: raw decryption requires a cipher capable of partial blocks, e.g. CTR\n");
		return -1;
	}

	int err = 0;

	unsigned int full_blocks = floor((double)input_buffer->get_m_length()/(double)m_block_size);
	unsigned int full_block_fragment = full_blocks*m_block_size;

	unsigned int fraction = input_buffer->get_m_length() - full_block_fragment;
	unsigned char* input_data = input_buffer->get_mp_data();
	unsigned char* output_data = output_buffer->get_mp_data();

	switch(m_chaining_mode)
	{
	case CHAINING_MODE_CTR:

		resetIV(m_chaining_mode);

		//the largest part
		if(full_block_fragment != 0)
		if( (err = ctr_decrypt(input_data, output_data, full_block_fragment, &m_ctr))  != CRYPT_OK )
		{
			printf("Error decrypting in %s-ctr mode: %s\n",m_cipher_name.c_str(), error_to_string(err));
			return -1;
		}

		//the fraction, i.e. last block
		if(fraction != 0)
		{
			//copy data into block segment
			unsigned char temp_plain[m_block_size];
			unsigned char temp_cipher[m_block_size];
			//due to the small block sizes we avoid memcpys
			for(unsigned int i=0;i<fraction;++i)
			{
				temp_cipher[i]= (input_data+full_block_fragment)[i];
			}
			//encrypt
			if( (err = ctr_decrypt(temp_cipher,temp_plain, m_block_size, &m_ctr))  != CRYPT_OK )
			{
				printf("Error encrypting fragment in %s-ctr mode: %s\n",m_cipher_name.c_str(), error_to_string(err));
				return -1;
			}
			//copy fragment into output
			for(unsigned int i=0;i<fraction;++i)
			{
				(output_data+full_block_fragment)[i] = temp_plain[i];
			}
		}

		break;

	case CHAINING_MODE_UNKNOWN:
		printf("Error raw encrypting in %s-XXX mode: unknown chaining mode\n",m_cipher_name.c_str());
		return -1;

	default:
		printf("Error raw encrypting in %s-XXX mode: unknown chaining mode\n",m_cipher_name.c_str());
		return -1;

	}

	return 0;
}


BasicBuffer* TomCryptWrapper::decryptBuffer(BasicBuffer* buffer)
{
	//stop if system not ready
	if(m_ready == false)
	{
		printf("ERROR: decryption requires a ready system\n");
		return NULL;
	}

	int err = 0;

	//the new size is a multiple of the block length, i.e. m_block_size bytes.
	unsigned int cipher_size = ceil((double)buffer->get_m_length()/(double)m_block_size)*m_block_size;

	//determine if padding is needed
	if(cipher_size != buffer->get_m_length())
	{
		printf("ERROR: ciphertext is not block-aligned\n");
		return NULL;
	}

	BasicBuffer* plaintext = new BasicBuffer(cipher_size);

	switch(m_chaining_mode)
	{
	case CHAINING_MODE_CTR:

		resetIV(m_chaining_mode);

		if( (err = ctr_decrypt(buffer->get_mp_data(), plaintext->get_mp_data(), cipher_size, &m_ctr))  != CRYPT_OK )
		{
			printf("Error decrypting in %s-ctr mode: %s\n",m_cipher_name.c_str(), error_to_string(err));
			delete plaintext;
			plaintext = NULL;
		}

		break;

	case CHAINING_MODE_CBC:

		resetIV(m_chaining_mode);

		if( (err = cbc_decrypt(buffer->get_mp_data(), plaintext->get_mp_data(), cipher_size, &m_cbc))  != CRYPT_OK )
		{
			printf("Error decrypting in %s-cbc mode: %s\n",m_cipher_name.c_str(), error_to_string(err));
			delete plaintext;
			plaintext = NULL;
		}

		break;

	case CHAINING_MODE_ECB:

		if( (err = ecb_decrypt(buffer->get_mp_data(), plaintext->get_mp_data(), cipher_size, &m_ecb))  != CRYPT_OK )
		{
			printf("Error decrypting in %s-ecb mode: %s\n",m_cipher_name.c_str(), error_to_string(err));
			delete plaintext;
			plaintext = NULL;
		}

		break;

	case CHAINING_MODE_UNKNOWN:
		printf("Error decrypting in %s-XXX mode: unknown chaining mode\n",m_cipher_name.c_str());
		delete plaintext;
		plaintext = NULL;
		break;

	default:
		printf("Error decrypting in %s-XXX mode: unknown chaining mode\n",m_cipher_name.c_str());
		delete plaintext;
		plaintext = NULL;
		break;

	}

	return plaintext;
}

BasicBuffer* TomCryptWrapper::encryptBuffer(BasicBuffer* buffer)
{
	//stop if system not ready
	if(m_ready == false)
	{
		printf("ERROR: encryption requires a ready system\n");
		return NULL;
	}

	int err = 0;
	bool padding_required = false;

	//the new size is a multiple of the block length, i.e. m_block_size bytes.
	unsigned int cipher_size = ceil((double)buffer->get_m_length()/(double)m_block_size)*m_block_size;

	BasicBuffer* cipher = new BasicBuffer(cipher_size);

	//determine if padding is needed
	if(cipher_size != buffer->get_m_length())
	{
		padding_required = true;
	}

	switch(m_chaining_mode)
	{
	case CHAINING_MODE_CTR:

		resetIV(m_chaining_mode);

		//no padding
		if(padding_required == false)
		{
			if( (err = ctr_encrypt(buffer->get_mp_data(), cipher->get_mp_data(), cipher_size, &m_ctr))  != CRYPT_OK )
			{
				printf("Error encrypting in %s-ctr mode: %s\n",m_cipher_name.c_str(), error_to_string(err));
				delete cipher;
				cipher = NULL;
			}
		}
		else //padding
		{
			unsigned long byte_fraction = m_block_size - (cipher_size - buffer->get_m_length());

			//first batch
			if( (err = ctr_encrypt(buffer->get_mp_data(), cipher->get_mp_data(), cipher_size-m_block_size, &m_ctr))  != CRYPT_OK )
			{
				printf("Error encrypting batch 1 in %s-ctr mode: %s\n",m_cipher_name.c_str(), error_to_string(err));
				delete cipher;
				cipher = NULL;
			}

			//second batch
			unsigned char last_block[m_block_size];
			memset(last_block,0,m_block_size);

			//memcpy(last_block,buffer->get_mp_data()+(cipher_size-m_block_size),byte_fraction);
			unsigned char* temp_data = buffer->get_mp_data()+(cipher_size-m_block_size);
			for(unsigned int i=0;i<byte_fraction;++i)
			{
				last_block[i] = temp_data[i];
			}

			if( (err = ctr_encrypt(last_block, cipher->get_mp_data()+(cipher_size-m_block_size), m_block_size, &m_ctr))  != CRYPT_OK )
			{
				printf("Error encrypting batch 2 in %s-ctr mode: %s\n",m_cipher_name.c_str(), error_to_string(err));
				delete cipher;
				cipher = NULL;
			}
		}

		break;

	case CHAINING_MODE_CBC:

		resetIV(m_chaining_mode);

		//no padding
		if(padding_required == false)
		{
			if( (err = cbc_encrypt(buffer->get_mp_data(), cipher->get_mp_data(), cipher_size, &m_cbc))  != CRYPT_OK )
			{
				printf("Error encrypting in %s-cbc mode: %s\n",m_cipher_name.c_str(), error_to_string(err));
				delete cipher;
				cipher = NULL;
			}
		}
		else //padding
		{
			unsigned long byte_fraction = m_block_size - (cipher_size - buffer->get_m_length());

			//first batch
			if( (err = cbc_encrypt(buffer->get_mp_data(), cipher->get_mp_data(), cipher_size-m_block_size, &m_cbc))  != CRYPT_OK )
			{
				printf("Error encrypting batch 1 in %s-cbc mode: %s\n",m_cipher_name.c_str(), error_to_string(err));
				delete cipher;
				cipher = NULL;
			}

			//second batch
			unsigned char last_block[m_block_size];
			memset(last_block,0,m_block_size);
			//memcpy(last_block,buffer->get_mp_data()+(cipher_size-m_block_size),byte_fraction);
			unsigned char* temp_data = buffer->get_mp_data()+(cipher_size-m_block_size);
			for(unsigned int i=0;i<byte_fraction;++i)
			{
				last_block[i] = temp_data[i];
			}

			if( (err = cbc_encrypt(last_block, cipher->get_mp_data()+(cipher_size-m_block_size), m_block_size, &m_cbc))  != CRYPT_OK )
			{
				printf("Error encrypting batch 2 in %s-cbc mode: %s\n",m_cipher_name.c_str(), error_to_string(err));
				delete cipher;
				cipher = NULL;
			}
		}

		break;

	case CHAINING_MODE_ECB:

		//no padding
		if(padding_required == false)
		{
			if( (err = ecb_encrypt(buffer->get_mp_data(), cipher->get_mp_data(), cipher_size, &m_ecb))  != CRYPT_OK )
			{
				printf("Error encrypting in %s-ecb mode: %s\n",m_cipher_name.c_str(), error_to_string(err));
				delete cipher;
				cipher = NULL;
			}
		}
		else //padding
		{
			unsigned long byte_fraction = m_block_size - (cipher_size - buffer->get_m_length());

			//first batch
			if( (err = ecb_encrypt(buffer->get_mp_data(), cipher->get_mp_data(), cipher_size-m_block_size, &m_ecb))  != CRYPT_OK )
			{
				printf("Error encrypting batch 1 in %s-ecb mode: %s\n",m_cipher_name.c_str(), error_to_string(err));
				delete cipher;
				cipher = NULL;
			}

			//second batch
			unsigned char last_block[m_block_size];
			memset(last_block,0,m_block_size);
			//memcpy(last_block,buffer->get_mp_data()+(cipher_size-m_block_size),byte_fraction);
			unsigned char* temp_data = buffer->get_mp_data()+(cipher_size-m_block_size);
			for(unsigned int i=0;i<byte_fraction;++i)
			{
				last_block[i] = temp_data[i];
			}

			if( (err = ecb_encrypt(last_block, cipher->get_mp_data()+(cipher_size-m_block_size), m_block_size, &m_ecb))  != CRYPT_OK )
			{
				printf("Error encrypting batch 2 in %s-ecb mode: %s\n",m_cipher_name.c_str(), error_to_string(err));
				delete cipher;
				cipher = NULL;
			}
		}

		break;

	case CHAINING_MODE_UNKNOWN:
		printf("Error encrypting in %s-XXX mode: unknown chaining mode\n",m_cipher_name.c_str());
		delete cipher;
		cipher = NULL;
		break;

	default:
		printf("Error encrypting in %s-XXX mode: unknown chaining mode\n",m_cipher_name.c_str());
		delete cipher;
		cipher = NULL;
		break;

	}

	return cipher;
}



int TomCryptWrapper::encryptBufferNoIVReset(Buffer* input_buffer, Buffer* output_buffer)
{
	//stop if system not ready
	if(m_ready == false)
	{
		printf("ERROR: encryption requires a ready system\n");
		return -1;
	}

	//first sanity check
	if(input_buffer->get_m_length() != output_buffer->get_m_length())
	{
		printf("ERROR: encryption requires buffers of the same size\n");
		return -1;
	}

	int err = 0;
	//the new size is a multiple of the block length, i.e. m_block_size bytes.
	unsigned int cipher_size = ceil((double)input_buffer->get_m_length()/(double)m_block_size)*m_block_size;

	//second sanity check (i.e. block-alignment)
	if(cipher_size != input_buffer->get_m_length())
	{
		printf("ERROR: encryption requires aligned buffers %u %u\n",cipher_size,input_buffer->get_m_length());
		return -1;
	}

	switch(m_chaining_mode)
	{
	case CHAINING_MODE_CTR:

		if( (err = ctr_encrypt(input_buffer->get_mp_data(), output_buffer->get_mp_data(), cipher_size, &m_ctr))  != CRYPT_OK )
		{
			printf("Error encrypting in %s-ctr mode: %s\n",m_cipher_name.c_str(), error_to_string(err));
			return -1;
		}

		break;

	case CHAINING_MODE_CBC:

		if( (err = cbc_encrypt(input_buffer->get_mp_data(), output_buffer->get_mp_data(), cipher_size, &m_cbc))  != CRYPT_OK )
		{
			printf("Error encrypting in %s-cbc mode: %s\n",m_cipher_name.c_str(), error_to_string(err));
			return -1;
		}

		break;

	case CHAINING_MODE_ECB:

		if( (err = ecb_encrypt(input_buffer->get_mp_data(), output_buffer->get_mp_data(), cipher_size, &m_ecb))  != CRYPT_OK )
		{
			printf("Error encrypting in %s-ecb mode: %s\n",m_cipher_name.c_str(), error_to_string(err));
			return -1;
		}

		break;

	case CHAINING_MODE_UNKNOWN:
		printf("Error encrypting in %s-XXX mode: unknown chaining mode\n",m_cipher_name.c_str());
		return -1;

	default:
		printf("Error encrypting in %s-XXX mode: unknown chaining mode\n",m_cipher_name.c_str());
		return -1;

	}

	return 0;
}

int TomCryptWrapper::decryptBufferNoIVReset(Buffer* input_buffer, Buffer* output_buffer)
{
	//stop if system not ready
	if(m_ready == false)
	{
		printf("ERROR: decryption requires a ready system\n");
		return -1;
	}

	//first sanity check
	if(input_buffer->get_m_length() != output_buffer->get_m_length())
	{
		printf("ERROR: decryption requires buffers of the same size\n");
		return -1;
	}

	int err = 0;
	//the new size is a multiple of the block length, i.e. m_block_size bytes.
	unsigned int cipher_size = ceil((double)input_buffer->get_m_length()/(double)m_block_size)*m_block_size;

	//second sanity check (i.e. block-alignment)
	if(cipher_size != input_buffer->get_m_length())
	{
		printf("ERROR: decryption requires aligned buffers %u %u\n",cipher_size,input_buffer->get_m_length());
		return -1;
	}

	switch(m_chaining_mode)
	{
	case CHAINING_MODE_CTR:

		if( (err = ctr_decrypt(input_buffer->get_mp_data(), output_buffer->get_mp_data(), cipher_size, &m_ctr))  != CRYPT_OK )
		{
			printf("Error decrypting in %s-ctr mode: %s\n",m_cipher_name.c_str(), error_to_string(err));
			return -1;
		}

		break;

	case CHAINING_MODE_CBC:

		if( (err = cbc_decrypt(input_buffer->get_mp_data(), output_buffer->get_mp_data(), cipher_size, &m_cbc))  != CRYPT_OK )
		{
			printf("Error decrypting in %s-cbc mode: %s\n",m_cipher_name.c_str(), error_to_string(err));
			return -1;
		}

		break;

	case CHAINING_MODE_ECB:

		if( (err = ecb_decrypt(input_buffer->get_mp_data(), output_buffer->get_mp_data(), cipher_size, &m_ecb))  != CRYPT_OK )
		{
			printf("Error decrypting in %s-ecb mode: %s\n",m_cipher_name.c_str(), error_to_string(err));
			return -1;
		}

		break;

	case CHAINING_MODE_UNKNOWN:
		printf("Error decrypting in %s-XXX mode: unknown chaining mode\n",m_cipher_name.c_str());
		return -1;

	default:
		printf("Error decrypting in %s-XXX mode: unknown chaining mode\n",m_cipher_name.c_str());
		return -1;

	}

	return 0;
}

#endif


#ifdef NOTOMCRYPT
  
TomCryptWrapper::TomCryptWrapper() {

	m_ready = false;
	m_IV = NULL;
	m_key = NULL;
	m_cipher_rounds = 0;
	m_key_size = 0;
	m_block_size = 0;

}

TomCryptWrapper::~TomCryptWrapper() {
	// TODO Auto-generated destructor stub
}


int TomCryptWrapper::resetIV(enum CHAINING_MODE chaining_mode)
{
	return 0;
}


int TomCryptWrapper::terminateChain(enum CHAINING_MODE chaining_mode)
{
	return 0;
}

int TomCryptWrapper::encryptFile(std::string input_file, std::string output_file)
{
	return 0;
}



int TomCryptWrapper::decryptFile(std::string input_file, std::string output_file)
{
	return 0;
}


int TomCryptWrapper::encryptFileRaw(std::string input_file, std::string output_file)
{
	return 0;
}



int TomCryptWrapper::decryptFileRaw(std::string input_file, std::string output_file)
{
	return 0;
}


int TomCryptWrapper::encryptFileInPlaceRaw(std::string input_file)
{
	return 0;
}



int TomCryptWrapper::decryptFileInPlaceRaw(std::string input_file)
{
	return 0;
}


int TomCryptWrapper::encryptBuffer(BasicBuffer* input_buffer, BasicBuffer* output_buffer)
{
	return 0;
}

int TomCryptWrapper::decryptBuffer(BasicBuffer* input_buffer, BasicBuffer* output_buffer)
{
	return 0;
}


int TomCryptWrapper::encryptBufferRaw(BasicBuffer* input_buffer, BasicBuffer* output_buffer)
{
	return 0;
}

int TomCryptWrapper::decryptBufferRaw(BasicBuffer* input_buffer, BasicBuffer* output_buffer)
{
	return 0;
}


BasicBuffer* TomCryptWrapper::decryptBuffer(BasicBuffer* buffer)
{
	return new Buffer();
}

BasicBuffer* TomCryptWrapper::encryptBuffer(BasicBuffer* buffer)
{
	return new Buffer();
}



int TomCryptWrapper::encryptBufferNoIVReset(Buffer* input_buffer, Buffer* output_buffer)
{
	return 0;
}

int TomCryptWrapper::decryptBufferNoIVReset(Buffer* input_buffer, Buffer* output_buffer)
{
	return 0;
}

#endif


} /* namespace Lazarus */
