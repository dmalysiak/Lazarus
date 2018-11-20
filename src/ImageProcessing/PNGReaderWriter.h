/*
 * JPEGReaderWriter.h
 *
 *  Created on: 07.02.2013
 *      Author: Darius Malysiak
 */

#ifndef PNGREADERWRITER_H_
#define PNGREADERWRITER_H_

#include "ImageReaderWriter.h"

#include <stdio.h>
#include <string.h>
#include <png.h>
#include <stdlib.h>
#include <setjmp.h>

namespace Lazarus
{

template<class T>
class PNGReaderWriter: public ImageReaderWriter<T> {
public:

	PNGReaderWriter()
	{
		m_bg_r = 0;
		m_bg_g = 0;
		m_bg_b = 0;

		m_has_bg = false;
		m_is_16_bit = false;
	}

	virtual ~PNGReaderWriter()
	{

	}

	/*
	 * This method will read an image in the specified alignment. Yet, not channel reduction will occur, i.e.
	 * if one tries to read images with an alpha channel and specifies an alignment without alpha channel, the method will
	 * abort, the same holds vice versa.
	 * The only partial exception is the conversion from read gray(a) images to rgb(a), yet also in this case we
	 * keep the channel count consistent e.g. it is not possible to demand rgba from a gray image. A conversion from
	 * RGB(a) to GRAY(A) is impossible.
	 * If the 'unspecified' alignment is given, the file existing alignment will be used.
	 * Regarding the data type we use following convention:
	 * if a 16-bit image is read and the datatype is smaller than 16 bits, the image channels will be reduced to 8 bits,
	 * otherwise the channels will be left intact.
	 * Additionally this method only supports interlaced (RGB RGB RGB ...) images. Furthermore an image will be saved either in 8 or
	 * in 16 bit depth.
	 */
	void readImage(const std::string &file, enum Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT alignment = Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_UNSPECIFIED)
	{

		int error = 0;
		png_uint_32  width, height;
		int  bit_depth, color_type, channels;
		unsigned char* image_data;

		FILE* infile = fopen(file.c_str(), "rb");

		if(infile == NULL)
		{
			printf("ERROR: could not open png file %s\n",file.c_str());
			return;
		}

		//initialize the reader
		unsigned char sig[8];


		/* first do a quick check that the file really is a PNG image; could
		 * have used slightly more general png_sig_cmp() function instead */

		fread(sig, 1, 8, infile);
		if(!png_check_sig(sig, 8))
		{
			error = 1;   /* bad signature */
		}

		/* could pass pointers to user-defined error handlers instead of NULLs: */
		png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if(!png_ptr)
		{
			error = 4;   /* out of memory */
		}

		png_infop info_ptr = png_create_info_struct(png_ptr);
		if(!info_ptr)
		{
			png_destroy_read_struct(&png_ptr, NULL, NULL);
			error = 4;   /* out of memory */
		}


		/* we could create a second info struct here (end_info), but it's only
		 * useful if we want to keep pre- and post-IDAT chunk info separated
		 * (mainly for PNG-aware image editors and converters) */


		/* setjmp() must be called in every function that calls a PNG-reading
		 * libpng function */

		if(setjmp(png_jmpbuf(png_ptr)))
		{
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			error = 2;
		}


		png_init_io(png_ptr, infile);
		png_set_sig_bytes(png_ptr, 8);  /* we already read the 8 signature bytes */

		png_read_info(png_ptr, info_ptr);  /* read all PNG info up to image data */


		/* alternatively, could make separate calls to png_get_image_width(),
		 * etc., but want bit_depth and color_type for later [don't care about
		 * compression_type and filter_type => NULLs] */

		png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
		  NULL, NULL, NULL);

		channels = (int)png_get_channels(png_ptr, info_ptr);

		//if everything went well so far

		//check for initialization error
		if( error != 0)
		{
			switch (error)
			{
				case 1:
					printf("ERROR:  [%s] is not a PNG file: incorrect signature\n", file.c_str());
					break;
				case 2:
					printf("ERROR:  [%s] has bad IHDR (libpng longjmp)\n", file.c_str());
					break;
				case 4:
					printf("ERROR:  insufficient memory\n");
					break;
				default:
					break;
			}

		}


		double  gamma;
		png_uint_32  i, rowbytes;
		png_bytepp  row_pointers = NULL;


		/* setjmp() must be called in every function that calls a PNG-reading
		 * libpng function */

		if (setjmp(png_jmpbuf(png_ptr)))
		{
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			return;
		}


		/* expand palette images to RGB, low-bit-depth grayscale images to 8 bits,
		 * transparency chunks to full alpha channel; strip 16-bit-per-sample
		 * images to 8 bits per sample; and convert grayscale to RGB[A] */

		//actual format of the image
		enum Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT alignment_ = Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_RGB;

		//determine the image type
		if( color_type == PNG_COLOR_TYPE_GRAY_ALPHA )
		{
			alignment_ = Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_GRAYA;
		}

		if( color_type == PNG_COLOR_TYPE_GRAY )
		{
			alignment_ = Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_GRAY;
		}

		if( color_type == PNG_COLOR_TYPE_RGB )
		{
			alignment_ = Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_RGB;
		}

		if( color_type == PNG_COLOR_TYPE_RGBA )
		{
			alignment_ = Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_RGBA;
		}

		//expand palette images or grayscale images with less than 8 bit into RGB(A) or GRAY(A)
		if (color_type == PNG_COLOR_TYPE_PALETTE || color_type == PNG_COLOR_TYPE_GRAY)
		{
			png_set_expand(png_ptr);


			if( color_type == PNG_COLOR_TYPE_PALETTE )
			{
				if(channels == 3)
				{
					alignment_ = Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_RGB;
				}
				if(channels == 4)
				{
					alignment_ = Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_RGBA;
				}
			}

		}

		//if simple transparency is used: expand to full alpha channel
		if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		{
			png_set_expand(png_ptr);
		}


		/*decrease 16 bit channels to 8 bit if required
		 */
		if(bit_depth == 16 && sizeof(T)*8<16)
		{
			printf("DEBUG: stripped 16 bit to 8 bit\n");
			png_set_strip_16(png_ptr);
			m_is_16_bit = false;
		}
		else if( bit_depth == 16 ) //in case of no reduction for 16bit data
		{
			m_is_16_bit = true;
		}


		/* unlike the example in the libpng documentation, we have *no* idea where
		 * this file may have come from--so if it doesn't have a file gamma, don't
		 * do any correction ("do no harm") */
		if(png_get_gAMA(png_ptr, info_ptr, &gamma))
		{
			double display_exponent = 1;
			png_set_gamma(png_ptr, display_exponent, gamma);
		}

		//if an alignment was specified
		if(alignment != Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_UNSPECIFIED)
		{


			//if the desired format is rgb(a)/bgr(a) and the actual image is gray: expand to rgb(a)/bgr(a)
			if( (alignment == Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_RGB ||
				alignment == Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_RGBA ||
				alignment == Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_BGR ||
				alignment == Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_BGRA ) &&
				( alignment_ == Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_GRAY ||
				alignment_ == Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_GRAYA ) )
			{
				png_set_gray_to_rgb(png_ptr);

				if( color_type == PNG_COLOR_TYPE_GRAY_ALPHA )
				{
					alignment_ = Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_RGBA;
				}

				if( color_type == PNG_COLOR_TYPE_GRAY )
				{
					alignment_ = Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_RGB;
				}
			}

			/*
			 * check if a swap is required (libpng swaps only between rgb(a)/bgr(a) formats) and realign the channels if required
			 */

			if( ( alignment == Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_RGB ||
				alignment == Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_RGBA ||
				alignment == Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_BGR ||
				alignment == Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_BGRA ) &&
				( alignment_ == Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_RGB ||
				alignment_ == Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_RGBA ||
				alignment_ == Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_BGR ||
				alignment_ == Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_BGRA ) )
			{printf("DEBUG: swap applicable\n");
				switch(alignment)
				{
					//bgr
					case Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_BGR:
						png_set_bgr(png_ptr);
						printf("DEBUG: swap done\n");
						break;

					//bgra
					case Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_BGRA:
						png_set_bgr(png_ptr);
						printf("DEBUG: swap done\n");
						break;

					//unchanged rgb(a) (default of libpng)
					default:
						printf("DEBUG: swap not done\n");
						break;
				}
			}


			//two final sanity checks for possible channel reductions
			if( (alignment == Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_RGB ||
				alignment == Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_BGR ||
				alignment == Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_GRAY) &&
				(alignment_ == Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_RGBA ||
				alignment_ == Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_BGRA ||
				alignment_ == Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_GRAYA) )
			{
				png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
				fclose(infile);
				printf("ERROR: channel reduction detected\n");
				return;
			}

			if( (alignment == Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_RGBA ||
				alignment == Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_BGRA ||
				alignment == Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_GRAYA) &&
				(alignment_ == Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_RGB ||
				alignment_ == Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_BGR ||
				alignment_ == Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_GRAY) )
			{
				png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
				fclose(infile);
				printf("ERROR: channel reduction detected\n");
				return;
			}

		}


		/* all transformations have been registered; now update info_ptr data,
		 * get rowbytes and channels, and allocate image memory */

		png_read_update_info(png_ptr, info_ptr);

		rowbytes = png_get_rowbytes(png_ptr, info_ptr);
		channels = (int)png_get_channels(png_ptr, info_ptr);

		if((image_data = (unsigned char*)malloc(rowbytes*height)) == NULL)
		{
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			return;
		}
		if((row_pointers = (png_bytepp)malloc(height*sizeof(png_bytep))) == NULL)
		{
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			free(image_data);
			image_data = NULL;
			return;
		}

		printf("readpng_get_image:  channels = %d, rowbytes = %d, height = %d\n", channels, rowbytes, height);


		/* set the individual row_pointers to point at the correct offsets */
		for(i = 0;  i < height;  ++i)
		{
			row_pointers[i] = image_data + i*rowbytes;
		}

		/* now we can go ahead and just read the whole image */
		png_read_image(png_ptr, row_pointers);


		/* and we're done!  (png_read_end() can be omitted if no processing of
		 * post-IDAT text/time/etc. is desired) */

		free(row_pointers);
		row_pointers = NULL;

		png_read_end(png_ptr, NULL);

		//---------------------- insert the read image data into an image object

		//delete the old image
		DELETE_NULL_CHECKING(this->mp_image);

		//if required, use a 16-bit temp image in order to handle actual 16-bit channels
		if( (bit_depth == 16 && sizeof(T)*8<16) == false && bit_depth == 16)//if no bit reduction occurred for 16-bit image: 16-bit problem
		{
			Lazarus::Image<unsigned short int>* temp_image = new Lazarus::Image<unsigned short int>(width,height,
					(enum Lazarus::Image<unsigned short int>::IMAGE_DATA_ALIGNMENT)alignment_);

			memcpy( temp_image->getmp_image_data(), image_data, rowbytes*height );

			//convert the temporary image into the desired format
			this->mp_image = Lazarus::ImageConverter<unsigned short int, T>::convertImageDataType(temp_image);

			delete(temp_image);
		}
		else //8-bit problem (i.e. reduction occurred or image actually has only 8 bits)
		{
			Lazarus::Image<unsigned char>* temp_image = new Lazarus::Image<unsigned char>(width,height,
					(enum Lazarus::Image<unsigned char>::IMAGE_DATA_ALIGNMENT)alignment_);

			memcpy( temp_image->getmp_image_data(), image_data, rowbytes*height );

			//convert the temporary image into the desired format
			this->mp_image = Lazarus::ImageConverter<unsigned char, T>::convertImageDataType(temp_image);

			delete(temp_image);
		}

		//extract background color
		if(png_get_valid(png_ptr, info_ptr, PNG_INFO_bKGD))
		{
			m_has_bg = true;
			png_color_16p pBackground;

			/* it is not obvious from the libpng documentation, but this function
			 * takes a pointer to a pointer, and it always returns valid red, green
			 * and blue values, regardless of color_type: */

			png_get_bKGD(png_ptr, info_ptr, &pBackground);


			/* however, it always returns the raw bKGD data, regardless of any
			 * bit-depth transformations, so check depth and adjust if necessary */

			if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
			{
				if(bit_depth == 1)
					m_bg_r = m_bg_g = m_bg_b = pBackground->gray ? 255 : 0;
				else if(bit_depth == 2)
					m_bg_r = m_bg_g = m_bg_b = (255/3) * pBackground->gray;
				else /* bit_depth == 4 */
					m_bg_r = m_bg_g = m_bg_b = (255/15) * pBackground->gray;
			}
			else
			{
				m_bg_r   = pBackground->red;
				m_bg_g = pBackground->green;
				m_bg_b  = pBackground->blue;
			}
		}
		else
		{
			m_has_bg = false;
		}

		//clean up
		free(image_data);
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(infile);

		//update the source filename
		DELETE_NULL_CHECKING(this->mp_filename);
		this->mp_filename = new std::string(file);

	}


	/*
	 * compression level lies in the range of [0,9] with 9=Z_BEST_COMPRESSION
	 */
	void writeImage(const std::string &file, unsigned int compression)
	{

		if(this->mp_image == NULL)
		{
			printf("ERROR: no image has been set\n");
			return;
		}

		if(compression > 9 )
		{
			printf("ERROR: invalid value for compression\n");
			return;
		}

		//create the meta data
		_mainprog_info wpng_info;
		wpng_info.height = this->mp_image->getm_height();
		wpng_info.width = this->mp_image->getm_width();
		wpng_info.have_text = false;
		wpng_info.have_time = false;
		wpng_info.have_bg = m_has_bg;
		wpng_info.bg_red = m_bg_r;
		wpng_info.bg_green = m_bg_g;
		wpng_info.bg_blue = m_bg_b;
		wpng_info.gamma = 0;
		if(sizeof(T)*8 > 8)
		{
			wpng_info.sample_depth = 16;
		}
		else
		{
			wpng_info.sample_depth = 8;
		}
		//we write the image in interlaced mode
		unsigned long rowbytes = this->mp_image->getm_channel_count() * wpng_info.width * sizeof(T);

		wpng_info.row_pointers = (unsigned char**)malloc(wpng_info.height * sizeof(unsigned char*));
		if(wpng_info.row_pointers == NULL)
		{
			printf("ERROR:  insufficient memory for image data\n");
			return;
		}
		for(long i = 0;  i < wpng_info.height;  ++i)
		{
			wpng_info.row_pointers[i] = (unsigned char*)this->mp_image->getmp_image_data() + i*rowbytes;
		}

		//initialize the library
		int error = 0;

		png_structp  png_ptr;       /* note:  temporary variables! */
		png_infop  info_ptr;
		int color_type, interlace_type;


		/* could also replace libpng warning-handler (final NULL), but no need: */

		png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if(!png_ptr)
			error = 4;   /* out of memory */

		info_ptr = png_create_info_struct(png_ptr);
		if(!info_ptr)
		{
			png_destroy_write_struct(&png_ptr, NULL);
			error = 4;   /* out of memory */
		}


		/* setjmp() must be called in every function that calls a PNG-writing
		 * libpng function, unless an alternate error handler was installed--
		 * but compatible error handlers must either use longjmp() themselves
		 * (as in this program) or exit immediately, so here we go: */

		if(setjmp(wpng_info.jmpbuf))
		{
			png_destroy_write_struct(&png_ptr, &info_ptr);
			error = 2;
		}


		/* make sure outfile is (re)opened in BINARY mode */
		if((wpng_info.outfile = fopen(file.c_str(), "wb")) == NULL)
		{
			printf("ERROR: could not open output file [%s]\n",file.c_str());
			png_destroy_write_struct(&png_ptr, &info_ptr);
			return;
		}
		png_init_io(png_ptr, wpng_info.outfile);


		/* set the compression levels--in general, always want to leave filtering
		 * turned on (except for palette images) and allow all of the filters,
		 * which is the default; want 32K zlib window, unless entire image buffer
		 * is 16K or smaller (unknown here)--also the default; usually want max
		 * compression (NOT the default); and remaining compression flags should
		 * be left alone */

		png_set_compression_level(png_ptr, compression);
	/*
		>> this is default for no filtering; Z_FILTERED is default otherwise:
		png_set_compression_strategy(png_ptr, Z_DEFAULT_STRATEGY);
		>> these are all defaults:
		png_set_compression_mem_level(png_ptr, 8);
		png_set_compression_window_bits(png_ptr, 15);
		png_set_compression_method(png_ptr, 8);
	 */


		/* set the image parameters appropriately */

		if(this->mp_image->getm_data_alignment() == Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_GRAY)
			color_type = PNG_COLOR_TYPE_GRAY;
		else if(this->mp_image->getm_data_alignment() == Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_RGB)
			color_type = PNG_COLOR_TYPE_RGB;
		else if(this->mp_image->getm_data_alignment() == Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_RGBA)
			color_type = PNG_COLOR_TYPE_RGB_ALPHA;
		else if(this->mp_image->getm_data_alignment() == Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_GRAYA)
			color_type = PNG_COLOR_TYPE_GRAY_ALPHA;
		else
		{
			printf("ERROR: unknown channel format\n");
			png_destroy_write_struct(&png_ptr, &info_ptr);
			return;
		}

		interlace_type = PNG_INTERLACE_ADAM7;

		png_set_IHDR(png_ptr, info_ptr, wpng_info.width, wpng_info.height,
				wpng_info.sample_depth, color_type, interlace_type,
				PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

		if(wpng_info.gamma > 0.0)
		{
			png_set_gAMA(png_ptr, info_ptr, wpng_info.gamma);
		}

		if(wpng_info.have_bg)
		{
			png_color_16  background;

			background.red = wpng_info.bg_red;
			background.green = wpng_info.bg_green;
			background.blue = wpng_info.bg_blue;
			background.gray = wpng_info.bg_red;

			png_set_bKGD(png_ptr, info_ptr, &background);
		}

		if(wpng_info.have_time)
		{
			png_time  modtime;

			png_convert_from_time_t(&modtime, wpng_info.modtime);
			png_set_tIME(png_ptr, info_ptr, &modtime);
		}

		if(wpng_info.have_text)
		{
			png_text  text[6];
			int  num_text = 0;

			if (wpng_info.have_text & TEXT_TITLE) {
				text[num_text].compression = PNG_TEXT_COMPRESSION_NONE;
				text[num_text].key = (char*)"Title";
				text[num_text].text = wpng_info.title;
				++num_text;
			}
			if (wpng_info.have_text & TEXT_AUTHOR) {
				text[num_text].compression = PNG_TEXT_COMPRESSION_NONE;
				text[num_text].key = (char*)"Author";
				text[num_text].text = wpng_info.author;
				++num_text;
			}
			if (wpng_info.have_text & TEXT_DESC) {
				text[num_text].compression = PNG_TEXT_COMPRESSION_NONE;
				text[num_text].key = (char*)"Description";
				text[num_text].text = wpng_info.desc;
				++num_text;
			}
			if (wpng_info.have_text & TEXT_COPY) {
				text[num_text].compression = PNG_TEXT_COMPRESSION_NONE;
				text[num_text].key = (char*)"Copyright";
				text[num_text].text = wpng_info.copyright;
				++num_text;
			}
			if (wpng_info.have_text & TEXT_EMAIL) {
				text[num_text].compression = PNG_TEXT_COMPRESSION_NONE;
				text[num_text].key = (char*)"E-mail";
				text[num_text].text = wpng_info.email;
				++num_text;
			}
			if (wpng_info.have_text & TEXT_URL) {
				text[num_text].compression = PNG_TEXT_COMPRESSION_NONE;
				text[num_text].key = (char*)"URL";
				text[num_text].text = wpng_info.url;
				++num_text;
			}
			png_set_text(png_ptr, info_ptr, text, num_text);
		}


		/* write all chunks up to (but not including) first IDAT */

		png_write_info(png_ptr, info_ptr);


		/* if we wanted to write any more text info *after* the image data, we
		 * would set up text struct(s) here and call png_set_text() again, with
		 * just the new data; png_set_tIME() could also go here, but it would
		 * have no effect since we already called it above (only one tIME chunk
		 * allowed) */


		/* set up the transformations:  for now, just pack low-bit-depth pixels
		 * into bytes (one, two or four pixels per byte) */

		png_set_packing(png_ptr);
	/*  png_set_shift(png_ptr, &sig_bit);  to scale low-bit-depth values */


		//the error value should now be unchanged at 0

		//check for errors
		switch(error)
		{
			case 2:
				printf("ERROR:  libpng initialization problem (longjmp)\n");
				break;
			case 4:
				printf("ERROR:  insufficient memory\n");
				break;
			case 11:
				printf("ERROR:  internal logic error (unexpected PNM type)\n");
				break;
			default:
				break;
		}

		//write the image

		/* and now we just write the whole image; libpng takes care of interlacing
		 * for us */

		png_write_image(png_ptr, wpng_info.row_pointers);


		/* since that's it, we also close out the end of the PNG file now--if we
		 * had any text or time info to write after the IDATs, second argument
		 * would be info_ptr, but we optimize slightly by sending NULL pointer: */

		png_write_end(png_ptr, NULL);


		//free memory
		png_destroy_write_struct(&png_ptr, &info_ptr);

		if (wpng_info.outfile) {
			fclose(wpng_info.outfile);
		}

		if (wpng_info.row_pointers) {
			free(wpng_info.row_pointers);
		}
	}

	//fills the bg color into the tuple
	void getBGColor(Lazarus::FastKTuple<unsigned int>* bgcolor)
	{
		bgcolor->mp_data[0] = m_bg_r;
		bgcolor->mp_data[1] = m_bg_g;
		bgcolor->mp_data[2] = m_bg_b;
	}

	void setBGColor(Lazarus::FastKTuple<unsigned int>* bgcolor)
	{
		m_bg_r = bgcolor->mp_data[0];
		m_bg_g = bgcolor->mp_data[1];
		m_bg_b = bgcolor->mp_data[2];
	}

private:

	unsigned int m_bg_r;
	unsigned int m_bg_g;
	unsigned int m_bg_b;

	bool m_has_bg;
	bool m_is_16_bit;

	const int TEXT_TITLE  =  0x01;
	const int TEXT_AUTHOR =  0x02;
	const int TEXT_DESC   =  0x04;
	const int TEXT_COPY   =  0x08;
	const int TEXT_EMAIL  =  0x10;
	const int TEXT_URL    =  0x20;

	const int TEXT_TITLE_OFFSET   =     0;
	const int TEXT_AUTHOR_OFFSET  =    72;
	const int TEXT_COPY_OFFSET    = (2*72);
	const int TEXT_EMAIL_OFFSET   = (3*72);
	const int TEXT_URL_OFFSET     = (4*72);
	const int TEXT_DESC_OFFSET    = (5*72);

	struct _mainprog_info
	{
		double gamma;
		long width;
		long height;
		time_t modtime;
		FILE* outfile;
		unsigned char** row_pointers;
		char* title;
		char* author;
		char* desc;
		char* copyright;
		char* email;
		char* url;
		int filter;    /* command-line-filter flag, not PNG row filter! */
		int sample_depth;
		bool have_bg;
		bool have_time;
		bool have_text;
		jmp_buf jmpbuf;
		unsigned char bg_red;
		unsigned char bg_green;
		unsigned char bg_blue;
	};


};

}

#endif /* JPEGREADERWRITER_H_ */
