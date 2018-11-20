/*
 * JPEGReaderWriter.h
 *
 *  Created on: 07.02.2013
 *      Author: Darius Malysiak
 */

#ifndef JPEGREADERWRITER_H_
#define JPEGREADERWRITER_H_

#include "ImageReaderWriter.h"

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
class JPEGReaderWriter: public ImageReaderWriter<T> {
public:

	JPEGReaderWriter()
	{

	}

	virtual ~JPEGReaderWriter()
	{

	}

	void readImage(const std::string &file, enum Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT alignment = Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_RGB)
	{

	    struct jpeg_decompress_struct cinfo;
	    struct jpeg_error_mgr jerr;

	    cinfo.err = jpeg_std_error(&jerr);
	    jpeg_create_decompress(&cinfo);

	    FILE * infile;
		if ((infile = fopen(file.c_str(), "rb")) == NULL) {
			printf("can't open %s\n", file.c_str());
			exit(1);
		}

		jpeg_stdio_src(&cinfo, infile);

		jpeg_read_header(&cinfo, TRUE);

		//initialize the image
		DELETE_NULL_CHECKING(this->mp_image);
		//printf("RGBA image %d %d\n",cinfo.image_width, cinfo.image_height);
		/*this->mp_image = new Lazarus::Image<unsigned char>((unsigned int)(cinfo.image_width),
				(unsigned int)(cinfo.image_height), Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_RGB);*/

		Lazarus::Image<unsigned char>* temp_image = new Lazarus::Image<unsigned char>((unsigned int)(cinfo.image_width),
				(unsigned int)(cinfo.image_height),
				(enum Lazarus::Image<unsigned char>::IMAGE_DATA_ALIGNMENT)alignment);

		//printf("decompress init\n");
		jpeg_start_decompress(&cinfo);//printf("channels in jpeg file %d\n",cinfo.output_components);
		//apparently the output_width etc fields only get filled after jpeg_start_decompress has been called

		//read the data
		JSAMPARRAY buffer;
		JDIMENSION row_stride = cinfo.output_width * cinfo.output_components;
	//	JDIMENSION width =  cinfo.output_width;
	//	JDIMENSION height = cinfo.output_height;
		//JDIMENSION read_lines = 0;

		switch(alignment)
		{

			case Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_RGB:

				buffer = (*cinfo.mem->alloc_sarray)
					((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, cinfo.output_height);

				while (cinfo.output_scanline < cinfo.output_height)
				{

					jpeg_read_scanlines(&cinfo, buffer, 1);

					put_scanline_rgb(buffer[0], cinfo.output_scanline, cinfo.output_width,
							 	 cinfo.output_height, temp_image->getmp_image_data());
				}

				break;

			case Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_RGBA:

				buffer = (*cinfo.mem->alloc_sarray)
					((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

				while (cinfo.output_scanline < cinfo.output_height)
				{

					jpeg_read_scanlines(&cinfo, buffer, 1);

					put_scanline_rgba(buffer[0], cinfo.output_scanline, cinfo.output_width,
								 cinfo.output_height, temp_image->getmp_image_data());
				}

				break;

			case Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_BGR:

				/* Make a one-row-high sample array that will go away when done with image */
				buffer = (*cinfo.mem->alloc_sarray)
					((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

				while (cinfo.output_scanline < cinfo.output_height)
				{

					jpeg_read_scanlines(&cinfo, buffer, 1);

					put_scanline_bgr(buffer[0], cinfo.output_scanline, cinfo.output_width,
								 cinfo.output_height, temp_image->getmp_image_data());

				}

				break;

			case Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_BGRA:

				/* Make a one-row-high sample array that will go away when done with image */
				buffer = (*cinfo.mem->alloc_sarray)
					((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

				while (cinfo.output_scanline < cinfo.output_height)
				{

					jpeg_read_scanlines(&cinfo, buffer, 1);

					put_scanline_bgra(buffer[0], cinfo.output_scanline, cinfo.output_width,
								 cinfo.output_height, temp_image->getmp_image_data());

				}

				break;

			default:
				break;

		}


	//	/* Make a one-row-high sample array that will go away when done with image */
	////	buffer = (*cinfo.mem->alloc_sarray)
	////		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, cinfo.output_height);
	//	buffer = (*cinfo.mem->alloc_sarray)
	//		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, cinfo.output_height);
	//
	//	printf("iterating\n");
	//	JDIMENSION read_lines = 0;
	//	while (cinfo.output_scanline < cinfo.output_height) {
	//
	////		read_lines = jpeg_read_scanlines(&cinfo, buffer, 1);
	//
	//		read_lines = jpeg_read_scanlines(&cinfo, buffer, cinfo.output_height);
	//
	////		put_scanline(buffer[0], cinfo.output_scanline, cinfo.output_width,
	////				 	 cinfo.output_height, this->mp_image->getmp_image_data());
	//
	//		put_multi_scanline(buffer[0], cinfo.output_scanline, cinfo.output_width,
	//				 	 cinfo.output_height, this->mp_image->getmp_image_data(),read_lines);
	//	}

		jpeg_finish_decompress(&cinfo);
		jpeg_destroy_decompress(&cinfo);
		fclose(infile);

		//update the source filename
		DELETE_NULL_CHECKING(this->mp_filename);
		this->mp_filename = new std::string(file);

		//convert the temporary image into the desired format
		this->mp_image = Lazarus::ImageConverter<unsigned char, T>::convertImageDataType(temp_image);

		delete temp_image;

	}

	void writeImage(const std::string &file, unsigned int quality = 100)
	{

		if(this->mp_image == NULL)
		{
			printf("ERROR: no image has been set\n");
			return;
		}

		//create a temporary image with the correct type i.e. uchar
		Lazarus::Image<unsigned char>* temp_image = Lazarus::ImageConverter<T, unsigned char>::convertImageDataType(this->mp_image);


	    struct jpeg_compress_struct cinfo;
	    struct jpeg_error_mgr jerr;


	    cinfo.err = jpeg_std_error(&jerr);
	    jpeg_create_compress(&cinfo);

	    FILE * outfile;
		if ((outfile = fopen(file.c_str(), "wb")) == NULL) {
	    //if ((infile = fopen("/root/test_out.jpg", "wb")) == NULL) {
			printf("can't open %s\n", file.c_str());
			jpeg_finish_compress(&cinfo);
			jpeg_destroy_compress(&cinfo);
			exit(1);
		}
	    jpeg_stdio_dest(&cinfo, outfile);

		//initialize the fields
		cinfo.input_components = temp_image->channels(); //channel count
		cinfo.image_width = temp_image->getm_width(); 	/* image width and height, in pixels */
		cinfo.image_height = temp_image->getm_height();

		switch(this->mp_image->getm_data_alignment())
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
		fclose(outfile);

		//update the source filename
		DELETE_NULL_CHECKING(this->mp_filename);
		this->mp_filename = new std::string(file);

		//delete the temporary image
		delete temp_image;
	}


private:

	//RGB(A)
	void put_scanline_rgb(unsigned char* buffer, int line, unsigned int width, unsigned int height, unsigned char* image_buffer)
	{
		unsigned int k = (line-1)*3*width;//(height-line)*3*width;
	    for(unsigned int i=0; i<3*width; i+=3)
	    {
	    	image_buffer[k+i]   = buffer[i];
	    	image_buffer[k+i+1] = buffer[i+1];
	    	image_buffer[k+i+2] = buffer[i+2];
	    }
	}

	void put_multi_scanline_rgb(unsigned char* buffer, int line, unsigned int width, unsigned int height, unsigned char* image_buffer, unsigned int amount)
	{
		unsigned int k = (line-amount)*3*width;//(height-line)*3*width;
		//printf("read %d lines, copying: %d-%d, new data: %d \n",amount,k,k+sizeof(T)*amount*3*width,sizeof(T)*amount*3*width);
		memcpy(image_buffer+k,buffer,sizeof(T)*amount*3*width);
	}

	void put_scanline_rgba(unsigned char* buffer, int line, unsigned int width, unsigned int height, unsigned char* image_buffer)
	{
		unsigned int k = (line-1)*4*width;//(height-line)*4*width;
		unsigned int j = 0;
		for(unsigned int i=0; i<4*width; i+=4)
		{
			image_buffer[k+i]   = buffer[j];
			image_buffer[k+i+1] = buffer[j+1];
			image_buffer[k+i+2] = buffer[j+2];
			image_buffer[k+i+3] = 255;

			j+=3;
		}
	}

	//BGR(A)
	void put_scanline_bgr(unsigned char* buffer, int line, unsigned int width, unsigned int height, unsigned char* image_buffer)
	{
		unsigned int k = (line-1)*3*width;//(height-line)*3*width;
	    for(unsigned int i=0; i<3*width; i+=3)
	    {
	    	image_buffer[k+i]   = buffer[i+2];//b
	    	image_buffer[k+i+1] = buffer[i+1];//g
	    	image_buffer[k+i+2] = buffer[i];//r
	    }
	}

	void put_scanline_bgra(unsigned char* buffer, int line, unsigned int width, unsigned int height, unsigned char* image_buffer)
	{
		unsigned int k = (line-1)*4*width;//(height-line)*4*width;
		unsigned int j = 0;
		for(unsigned int i=0; i<4*width; i+=4)
		{
	    	image_buffer[k+i]   = buffer[j+2];//b
	    	image_buffer[k+i+1] = buffer[j+1];//g
	    	image_buffer[k+i+2] = buffer[j];//r
			image_buffer[k+i+3] = 255; //a
			j+=3;
		}
	}

};

}

#endif /* JPEGREADERWRITER_H_ */
