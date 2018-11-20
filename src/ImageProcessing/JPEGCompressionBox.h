/*
 * JPEGReaderWriter.h
 *
 *  Created on: 07.02.2013
 *      Author: Darius Malysiak
 */

#ifndef JPEGCOMPRESSIONBOX_H_
#define JPEGCOMPRESSIONBOX_H_

#include "../BasicBuffer.h"
#include "../Buffer.h"

#include <stdio.h>
#include <string.h>
#include <jpeglib.h>
#include <stdlib.h>
#include <setjmp.h>

namespace Lazarus
{

/**
 * Sadly the libjpeg API does not support 8 and 12 bit at the same time. Thus this wrapper only supports 8 bit.
 * */
template<class T>
class JPEGCompressionBox : Lazarus::BaseObject {
public:

	JPEGCompressionBox()
	{

	}

	virtual ~JPEGCompressionBox()
	{

	}

	/**
	 * Expects a buffer of libjpeg compressed data.
	 * */
	static Lazarus::Image<unsigned char>* decompress(Lazarus::BasicBuffer* imagedata)
	{
		Lazarus::Image<unsigned char>* output = NULL;

	    struct jpeg_decompress_struct cinfo;
	    struct jpeg_error_mgr jerr;

	    cinfo.err = jpeg_std_error(&jerr);
	    jpeg_create_decompress(&cinfo);

	    jpeg_mem_src(&cinfo,imagedata->get_mp_data(),
	    		imagedata->get_m_length());

		jpeg_read_header(&cinfo, TRUE);

		//initialize the image
		switch(cinfo.output_components)
		{
			case 1:
			output = new Lazarus::Image<unsigned char>((unsigned int)(cinfo.image_width),
					(unsigned int)(cinfo.image_height),
					Lazarus::Image<unsigned char>::IMAGE_DATA_ALIGNMENT_GRAY);
			break;

			case 3:
			output = new Lazarus::Image<unsigned char>((unsigned int)(cinfo.image_width),
					(unsigned int)(cinfo.image_height),
					Lazarus::Image<unsigned char>::IMAGE_DATA_ALIGNMENT_RGB);
			break;
		}

		jpeg_start_decompress(&cinfo);//printf("channels in jpeg file %d\n",cinfo.output_components);
		//apparently the output_width etc fields only get filled after jpeg_start_decompress has been called

		//read the data
		JSAMPARRAY buffer;
		JDIMENSION row_stride = cinfo.output_width * cinfo.output_components;

		switch(output->alignment())
		{

			case Lazarus::Image<unsigned char>::IMAGE_DATA_ALIGNMENT_RGB:

			buffer = (*cinfo.mem->alloc_sarray)
				((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, cinfo.output_height);

			while (cinfo.output_scanline < cinfo.output_height)
			{

				jpeg_read_scanlines(&cinfo, buffer, 1);

				put_scanline_rgb(buffer[0], cinfo.output_scanline, cinfo.output_width,
							 cinfo.output_height, output->getmp_image_data());
			}

			break;

			case Lazarus::Image<unsigned char>::IMAGE_DATA_ALIGNMENT_GRAY:

			buffer = (*cinfo.mem->alloc_sarray)
				((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, cinfo.output_height);

			while (cinfo.output_scanline < cinfo.output_height)
			{

				jpeg_read_scanlines(&cinfo, buffer, 1);

				put_scanline_gray(buffer[0], cinfo.output_scanline, cinfo.output_width,
							 cinfo.output_height, output->getmp_image_data());
			}

			break;


			default:
				break;

		}

		jpeg_finish_decompress(&cinfo);
		jpeg_destroy_decompress(&cinfo);

		return output;
	}


	/**
	 * Expects a buffer of libjpeg compressed data.
	 * */
	static void decompress(Lazarus::BasicBuffer* imagedata, Lazarus::Image<unsigned char>* output)
	{
	    struct jpeg_decompress_struct cinfo;
	    struct jpeg_error_mgr jerr;

	    cinfo.err = jpeg_std_error(&jerr);
	    jpeg_create_decompress(&cinfo);

	    jpeg_mem_src(&cinfo,imagedata->get_mp_data(),
	    		imagedata->get_m_length());

		jpeg_read_header(&cinfo, TRUE);

		jpeg_start_decompress(&cinfo);//printf("channels in jpeg file %d\n",cinfo.output_components);
		//apparently the output_width etc fields only get filled after jpeg_start_decompress has been called

		//read the data
		JSAMPARRAY buffer;
		JDIMENSION row_stride = cinfo.output_width * cinfo.output_components;

		switch(output->alignment())
		{

			case Lazarus::Image<unsigned char>::IMAGE_DATA_ALIGNMENT_RGB:

			buffer = (*cinfo.mem->alloc_sarray)
				((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, cinfo.output_height);

			while (cinfo.output_scanline < cinfo.output_height)
			{

				jpeg_read_scanlines(&cinfo, buffer, 1);

				put_scanline_rgb(buffer[0], cinfo.output_scanline, cinfo.output_width,
							 cinfo.output_height, output->getmp_image_data());
			}

			break;

			case Lazarus::Image<unsigned char>::IMAGE_DATA_ALIGNMENT_GRAY:

			buffer = (*cinfo.mem->alloc_sarray)
				((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, cinfo.output_height);

			while (cinfo.output_scanline < cinfo.output_height)
			{

				jpeg_read_scanlines(&cinfo, buffer, 1);

				put_scanline_gray(buffer[0], cinfo.output_scanline, cinfo.output_width,
							 cinfo.output_height, output->getmp_image_data());
			}

			break;


			default:
				break;

		}

		jpeg_finish_decompress(&cinfo);
		jpeg_destroy_decompress(&cinfo);
	}

	/**
	 * Compresses an image into a buffer.
	 * */
	static Lazarus::BasicBuffer* compress(Lazarus::Image<T>* image, unsigned int quality = 100)
	{

		//create a temporary image with the correct type i.e. uchar
		Lazarus::Image<unsigned char>* temp_image = Lazarus::ImageConverter<T, unsigned char>::convertImageDataType(image);

	    struct jpeg_compress_struct cinfo;
	    struct jpeg_error_mgr jerr;

	    cinfo.err = jpeg_std_error(&jerr);
	    jpeg_create_compress(&cinfo);

	    //jpeg_mem_src(&cinfo,temp_image->getmp_image_data(),
	    //		temp_image->width()*temp_image->height()*temp_image->channels());

		unsigned char* data = NULL;
		unsigned long int size = 0;
	    jpeg_mem_dest(&cinfo, &data, &size);

		//initialize the fields
		cinfo.input_components = temp_image->channels(); //channel count
		cinfo.image_width = temp_image->getm_width(); 	/* image width and height, in pixels */
		cinfo.image_height = temp_image->getm_height();

		switch(image->getm_data_alignment())
		{
			case Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_BGRA:
				cinfo.in_color_space = JCS_EXT_BGRA; 	/* colorspace of input image */
				break;

			case Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_RGBA:
				cinfo.in_color_space = JCS_EXT_RGBA; 	/* colorspace of input image */
				break;

			case Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_RGB:
				cinfo.in_color_space = JCS_RGB; 	/* colorspace of input image */
				break;

			case Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_GRAY:
				cinfo.in_color_space = JCS_GRAYSCALE; 	/* colorspace of input image */
				break;

			default:
				break;
		}

		jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);

		jpeg_set_defaults(&cinfo);

		//printf("compress init\n");
		jpeg_start_compress(&cinfo, TRUE);
		//apparently the output_width etc fields only get filled after jpeg_start_decompress has been called

		JDIMENSION row_stride = temp_image->getm_width() * cinfo.input_components;	/* JSAMPLEs per row in image_buffer */
		JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
		unsigned char* image_buffer = temp_image->getmp_image_data();

		while (cinfo.next_scanline < cinfo.image_height) {
			/* jpeg_write_scanlines expects an array of pointers to scanlines.
			 * Here the array is only one element long, but you could pass
			 * more than one scanline at a time if that's more convenient.
			 */
			//printf("saved 1 row\n");
			row_pointer[0] = &image_buffer[cinfo.next_scanline * row_stride];
			jpeg_write_scanlines(&cinfo, row_pointer, 1);
		}

		jpeg_finish_compress(&cinfo);
		jpeg_destroy_compress(&cinfo);

		//delete the temporary image
		delete temp_image;

		return new Lazarus::Buffer(data,size);
	}


	/**
	 * Compresses an image into a buffer.
	 * */
	static void compress(Lazarus::Image<T>* image, Lazarus::Buffer* output,unsigned int quality = 100)
	{

		//create a temporary image with the correct type i.e. uchar
		Lazarus::Image<unsigned char>* temp_image = Lazarus::ImageConverter<T, unsigned char>::convertImageDataType(image);

	    struct jpeg_compress_struct cinfo;
	    struct jpeg_error_mgr jerr;

	    cinfo.err = jpeg_std_error(&jerr);
	    jpeg_create_compress(&cinfo);

	    //jpeg_mem_src(&cinfo,temp_image->getmp_image_data(),
	    //		temp_image->width()*temp_image->height()*temp_image->channels());

		unsigned char* data = NULL;
		unsigned long int size = 0;
	    jpeg_mem_dest(&cinfo, &data, &size);

		//initialize the fields
		cinfo.input_components = temp_image->channels(); //channel count
		cinfo.image_width = temp_image->getm_width(); 	/* image width and height, in pixels */
		cinfo.image_height = temp_image->getm_height();

		switch(image->getm_data_alignment())
		{
			case Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_BGRA:
				cinfo.in_color_space = JCS_EXT_BGRA; 	/* colorspace of input image */
				break;

			case Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_RGBA:
				cinfo.in_color_space = JCS_EXT_RGBA; 	/* colorspace of input image */
				break;

			case Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_RGB:
				cinfo.in_color_space = JCS_RGB; 	/* colorspace of input image */
				break;

			case Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_GRAY:
				cinfo.in_color_space = JCS_GRAYSCALE; 	/* colorspace of input image */
				break;

			default:
				break;
		}

		jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);

		jpeg_set_defaults(&cinfo);

		//printf("compress init\n");
		jpeg_start_compress(&cinfo, TRUE);
		//apparently the output_width etc fields only get filled after jpeg_start_decompress has been called

		JDIMENSION row_stride = temp_image->getm_width() * cinfo.input_components;	/* JSAMPLEs per row in image_buffer */
		JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
		unsigned char* image_buffer = temp_image->getmp_image_data();

		while (cinfo.next_scanline < cinfo.image_height) {
			/* jpeg_write_scanlines expects an array of pointers to scanlines.
			 * Here the array is only one element long, but you could pass
			 * more than one scanline at a time if that's more convenient.
			 */
			//printf("saved 1 row\n");
			row_pointer[0] = &image_buffer[cinfo.next_scanline * row_stride];
			jpeg_write_scanlines(&cinfo, row_pointer, 1);
		}

		jpeg_finish_compress(&cinfo);
		jpeg_destroy_compress(&cinfo);

		//delete the temporary image
		delete temp_image;

		output->set_mp_data(data,size);
	}

private:

	//RGB(A)
	static void put_scanline_rgb(unsigned char* buffer, int line, unsigned int width, unsigned int height, unsigned char* image_buffer)
	{
		unsigned int k = (line-1)*3*width;//(height-line)*3*width;
	    for(unsigned int i=0; i<3*width; i+=3)
	    {
	    	image_buffer[k+i]   = buffer[i];
	    	image_buffer[k+i+1] = buffer[i+1];
	    	image_buffer[k+i+2] = buffer[i+2];
	    }
	}


	//GRAY
	static void put_scanline_gray(unsigned char* buffer, int line, unsigned int width, unsigned int height, unsigned char* image_buffer)
	{
		unsigned int k = (line-1)*1*width;//(height-line)*3*width;
		for(unsigned int i=0; i<1*width; i+=1)
		{
			image_buffer[k+i]   = buffer[i];
		}
	}

};

}

#endif /* JPEGREADERWRITER_H_ */
