/*
 * CompressionBox.cpp
 *
 *  Created on: 11.07.2012
 *      Author: Darius Malysiak
 */

#include "CompressionBox.h"

#include <assert.h>

namespace Lazarus{

CompressionBox::CompressionBox() {

	decompress_no_stream_segment_size = 1000;
	file_read_chunk_size = 100;
	compression_level = 6;
}

CompressionBox::~CompressionBox() {
}

#ifdef USEZLIB
bool CompressionBox::strmInitCompress(z_stream* strm)
{
    strm->zalloc = Z_NULL;
    strm->zfree  = Z_NULL;
    strm->opaque = Z_NULL;

    if(checkZlibCompressError(deflateInit(strm,compression_level)) ==true )
    {
    	printf("ERROR: error while deflate initialization\n");
    	return true;
    }

    return false;
}
#endif

#ifdef USEZLIB
bool CompressionBox::strmInitDecompress(z_stream* strm)
{
    strm->zalloc = Z_NULL;
    strm->zfree = Z_NULL;
    strm->opaque = Z_NULL;
    strm->avail_in = 0;
    strm->next_in = Z_NULL;

    if(checkZlibCompressError(inflateInit(strm)) ==true )
    {
    	printf("ERROR: error while inflate initialization\n");
    	return true;
    }

    return false;
}
#endif

Buffer* CompressionBox::compressNoStream(Buffer* buffer)
{
#ifdef USEZLIB
	z_stream strm;
	//estimate the output buffer length (it should not be larger than the input length)
	unsigned int estimated_compress_buffer_size = (unsigned int)deflateBound(&strm,buffer->get_m_length());
	unsigned char* compress_buffer  = new unsigned char[estimated_compress_buffer_size];


	int state = 0;
	unsigned char* out = new unsigned char[estimated_compress_buffer_size];

	strmInitCompress(&strm);
	strm.next_in = buffer->get_mp_data();
	strm.avail_in = buffer->get_m_length();

	strm.avail_out = estimated_compress_buffer_size;
	strm.next_out = out;

	state =	deflate(&strm, Z_FINISH);

	//check if errors occurred, if so, do a clean finish
	if(checkZlibCompressError(state) == true)
	{
		printf("ERROR: zlib deflate error\n");
		deflateEnd (&strm);
		delete[](out);
		return NULL;
	}


	unsigned int available_bytes = estimated_compress_buffer_size - strm.avail_out;

	Buffer* compress_buffer_ = new Buffer(available_bytes);

	memcpy(compress_buffer_->get_mp_data(), out, available_bytes * sizeof(unsigned char));

	deflateEnd(&strm);
	delete[] out;
	delete[] compress_buffer;

	return compress_buffer_;
#endif

#ifndef USEZLIB
	printf("ERROR: no ZLIB support available.\n");
	return NULL;
#endif
}

Buffer* CompressionBox::decompressNoStream(Buffer* buffer, unsigned long length)
{
#ifdef USEZLIB
	//if output size is known this method can commence an efficient decompression
	if(length != 0)
	{
		Buffer* output = new Buffer(length);
		int result = uncompress(output->get_mp_data(),&length,(const unsigned char*)buffer->get_mp_data(),buffer->get_m_length());

		if(checkZlibDecompressError(result) == true)
		{
			delete output;
			return NULL;
		}

		return output;
	}
	//otherwise we need a slow process with lots of memory allocations / copies
	else
	{
		z_stream strm;
		unsigned char* decompress_buffer  = new unsigned char[decompress_no_stream_segment_size]();
		unsigned char* swap_buffer = NULL;

		int state = 0;
		unsigned int offset=0;
		unsigned int size_factor = 1;
		unsigned char* out = new unsigned char[decompress_no_stream_segment_size]();
		unsigned int decompressed_bytes=0;
		unsigned int total_bytes = 0;

		strmInitDecompress(&strm);
		strm.next_in = buffer->get_mp_data();
		strm.avail_in = buffer->get_m_length();


		do
		{
			strm.avail_out = decompress_no_stream_segment_size;
			strm.next_out = out;

			state =	inflate(&strm, Z_NO_FLUSH);

			//check if errors occurred, if so, do a clean finish
			if(checkZlibDecompressError(state) == true)
			{
				printf("zlib inflate error\n");
				inflateEnd(&strm);
				delete[] swap_buffer;
				delete[] out;
				delete[] decompress_buffer;
				return NULL;
			}

			decompressed_bytes = decompress_no_stream_segment_size - strm.avail_out;
			//save the current decompressed data-chunk in the decompress buffer(offset=0 in the first iteration)
			memcpy(decompress_buffer+offset, out, decompressed_bytes *sizeof(unsigned char));

			//if the current output buffer was filled and the decompression hasn't finished yet
			//we have to increase the decompression buffer (UGLY METHOD; but no way around)
			if(strm.avail_out == 0 && state != Z_STREAM_END)
			{
				//clear the swap buffer
				DELETE_ARRAY_NULL_CHECKING(swap_buffer);
				//allocate new swap buffer
				swap_buffer = new unsigned char[size_factor*decompress_no_stream_segment_size]();
				//save the so far decompressed data
				memcpy(swap_buffer, decompress_buffer, size_factor*decompress_no_stream_segment_size *sizeof(unsigned char));
				//delete the decompress buffer
				DELETE_ARRAY_NULL_CHECKING(decompress_buffer);

				//increase the size factor
				size_factor++;

				//create a bigger decompress buffer
				decompress_buffer = new unsigned char[size_factor*decompress_no_stream_segment_size]();
				//restore the decompressed data
				memcpy(decompress_buffer, swap_buffer, (size_factor-1)*decompress_no_stream_segment_size *sizeof(unsigned char));
				offset = (size_factor-1)*decompress_no_stream_segment_size;

			}

			total_bytes += decompressed_bytes;

		}while(state != Z_STREAM_END);


		inflateEnd (& strm);

		Buffer* output_buffer = new Buffer(total_bytes);
		memcpy(output_buffer->get_mp_data(), decompress_buffer, total_bytes * sizeof(unsigned char));

		delete[] swap_buffer;
		delete[] out;
		delete[] decompress_buffer;

		return output_buffer;
	}

	//dummy
	return NULL;

#endif

#ifndef USEZLIB
	printf("ERROR: no ZLIB support available.\n");
	return NULL;
#endif
}

void CompressionBox::setDecompressNoStreamSegmentSize(unsigned int size)
{
#ifdef USEZLIB
	decompress_no_stream_segment_size = size;
#endif

#ifndef USEZLIB
	printf("ERROR: no ZLIB support available.\n");
#endif
}

void CompressionBox::compressFile(const std::string& input_file, const std::string& output_file)
{
#ifdef USEZLIB
	int ret, flush;
	unsigned have;
	z_stream strm;
	unsigned char* in = new unsigned char[file_read_chunk_size];
	unsigned char* out = new unsigned char[file_read_chunk_size];

	//open the files
	FILE* source = NULL;
	FILE* dest = NULL;

	source = fopen(input_file.c_str(),"r");
	if(source == NULL)
	{
		printf("ERROR: could not open input file\n");
		delete[] in;
		delete[] out;
		return;
	}

	dest = fopen(output_file.c_str(),"w");
	if(dest == NULL)
	{
		printf("ERROR: could not open output file\n");
		delete[] in;
		delete[] out;
		fclose(source);
		return;
	}

	/* allocate deflate state */
	if(strmInitCompress(&strm) == true)
	{
		delete[] in;
		delete[] out;
		fclose(dest);
		fclose(source);
		return;
	}

	/* compress until end of file */
	do {
		strm.avail_in = fread(in, 1, file_read_chunk_size, source);
		if(ferror(source))
		{
			deflateEnd(&strm);
			delete[] in;
			delete[] out;
			fclose(dest);
			fclose(source);
			return;
		}
		flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
		strm.next_in = in;

		/* run deflate() on input until output buffer not full, finish
		   compression if all of source has been read in */
		do{
			strm.avail_out = file_read_chunk_size;
			strm.next_out = out;
			ret = deflate(&strm, flush);    /* no bad return value */
			assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
			have = file_read_chunk_size - strm.avail_out;
			if (fwrite(out, 1, have, dest) != have || ferror(dest))
			{
				deflateEnd(&strm);
				delete[] in;
				delete[] out;
				fclose(dest);
				fclose(source);
				return;
			}
		}while(strm.avail_out == 0);
		assert(strm.avail_in == 0);     /* all input will be used */

		/* done when last data in file processed */
	}while(flush != Z_FINISH);
	assert(ret == Z_STREAM_END);        /* stream will be complete */

	/* clean up and return */
	deflateEnd(&strm);
	delete[] in;
	delete[] out;
	fclose(dest);
	fclose(source);
#endif

#ifndef USEZLIB
	printf("ERROR: no ZLIB support available.\n");
#endif
}

void CompressionBox::decompressFile(const std::string& input_file, const std::string& output_file)
{
#ifdef USEZLIB
	int ret;
	unsigned have;
	z_stream strm;
	unsigned char* in = new unsigned char[file_read_chunk_size];
	unsigned char* out = new unsigned char[file_read_chunk_size];

	//open the files
	FILE* source = NULL;
	FILE* dest = NULL;

	source = fopen(input_file.c_str(),"r");
	if(source == NULL)
	{
		printf("ERROR: could not open input file\n");
		delete[] in;
		delete[] out;
		return;
	}

	dest = fopen(output_file.c_str(),"w");
	if(dest == NULL)
	{
		printf("ERROR: could not open output file\n");
		delete[] in;
		delete[] out;
		fclose(source);
		return;
	}

	/* allocate inflate state */
	if(strmInitDecompress(&strm) == true)
	{
		printf("ERROR: could not initialize inflate\n");
		delete[] in;
		delete[] out;
		fclose(dest);
		fclose(source);
		return;
	}

	/* decompress until deflate stream ends or end of file */
	do{
		strm.avail_in = fread(in, 1, file_read_chunk_size, source);
		if(ferror(source))
		{
			inflateEnd(&strm);
			delete[] in;
			delete[] out;
			fclose(dest);
			fclose(source);
			return;
		}
		if (strm.avail_in == 0)
		{
			break;
		}
		strm.next_in = in;

		/* run inflate() on input until output buffer not full */
		do {
			strm.avail_out = file_read_chunk_size;
			strm.next_out = out;

			ret = inflate(&strm, Z_NO_FLUSH);

			assert(ret != Z_STREAM_ERROR);  /* state not clobbered */

			switch(ret)
			{
				case Z_NEED_DICT:
					ret = Z_DATA_ERROR;     /* and fall through */

				case Z_DATA_ERROR:

				case Z_MEM_ERROR:
					(void)inflateEnd(&strm);
					delete[] in;
					delete[] out;
					fclose(dest);
					fclose(source);
					return;
			}
			have = file_read_chunk_size - strm.avail_out;
			if(fwrite(out, 1, have, dest) != have || ferror(dest))
			{
				(void)inflateEnd(&strm);
				delete[] in;
				delete[] out;
				fclose(dest);
				fclose(source);
				return;
			}
		}while(strm.avail_out == 0);

		/* done when inflate() says it's done */
	}while(ret != Z_STREAM_END);

	/* clean up and return */
	inflateEnd(&strm);
	delete[] in;
	delete[] out;
	fclose(dest);
	fclose(source);
#endif

#ifndef USEZLIB
	printf("ERROR: no ZLIB support available.\n");
#endif
}

void CompressionBox::setCompressionLevel(unsigned char level)
{
#ifdef USEZLIB
	if(level <= 9 && level >= 0)
	{
		compression_level = level;
	}
	else
	{
		printf("ERROR: valid compression levels range from 0 to 9\n");
	}
#endif

#ifndef USEZLIB
	printf("ERROR: no ZLIB support available.\n");
#endif
}

void CompressionBox::setFileReadChunkSize(unsigned int size)
{
#ifdef USEZLIB
	file_read_chunk_size = size;
#endif

#ifndef USEZLIB
	printf("ERROR: no ZLIB support available.\n");
#endif
}

bool CompressionBox::checkZlibCompressError(int error)
{
#ifdef USEZLIB
	bool errorIndicator = false;

	switch(error)
	{
	case Z_STREAM_ERROR:
		errorIndicator = true;
		printf("STREAM ERROR\n");
		break;

	case Z_DATA_ERROR:
		errorIndicator = true;
		printf("DATA ERROR\n");
		break;

	case Z_MEM_ERROR:
		errorIndicator = true;
		printf("MEMORY ERROR\n");
		break;

	case Z_VERSION_ERROR:
		errorIndicator = true;
		printf("VERSION ERROR\n");
		break;

	default:
		break;

	}

	return errorIndicator;
#endif

#ifndef USEZLIB
	printf("ERROR: no ZLIB support available.\n");
#endif
}

bool CompressionBox::checkZlibDecompressError(int error)
{
#ifdef USEZLIB
	bool errorIndicator = false;

	switch(error)
	{
	case Z_STREAM_ERROR:
		errorIndicator = true;
		printf("STREAM ERROR\n");
		break;

	case Z_DATA_ERROR:
		errorIndicator = true;
		printf("DATA ERROR\n");
		break;

	case Z_MEM_ERROR:
		errorIndicator = true;
		printf("MEMORY ERROR\n");
		break;

	case Z_VERSION_ERROR:
		errorIndicator = true;
		printf("VERSION ERROR\n");
		break;

	default:
		break;

	}

	return errorIndicator;
#endif

#ifndef USEZLIB
	printf("ERROR: no ZLIB support available.\n");
#endif

}


}
