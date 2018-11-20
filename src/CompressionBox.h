/*
 * CompressionBox.h
 *
 *  Created on: 11.07.2012
 *      Author: Darius Malysiak
 */

#ifndef COMPRESSIONBOX_H_
#define COMPRESSIONBOX_H_

#include "BaseObject.h"
#include "Buffer.h"

#ifdef USEZLIB
#include <zlib.h>
#endif

#include <cstring>
#include <stdio.h>
#include <iostream>
#include <fstream>

namespace Lazarus{

/**
 * This class does not support streams, so keep your buffers small enough ;-)
 */

class CompressionBox : BaseObject{
public:
	CompressionBox();
	virtual ~CompressionBox();

	Buffer* compressNoStream(Buffer* buffer);
	Buffer* decompressNoStream(Buffer* buffer, unsigned long length = 0);

	void setDecompressNoStreamSegmentSize(unsigned int size);
	void setFileReadChunkSize(unsigned int size);

	/**
	 * Must lie between 0 (no compression) and 9 (maximal compression).
	 * */
	void setCompressionLevel(unsigned char level);

	void compressFile(const std::string& input_file, const std::string& output_file);
	void decompressFile(const std::string& input_file, const std::string& output_file);


private:
#ifdef USEZLIB
	bool strmInitCompress(z_stream* strm);
	bool strmInitDecompress(z_stream* strm);
#endif

	bool checkZlibCompressError(int error);
	bool checkZlibDecompressError(int error);

	unsigned int decompress_no_stream_segment_size;
	unsigned char compression_level;//0-9
	unsigned int file_read_chunk_size;


};

}

#endif /* COMPRESSIONBOX_H_ */
