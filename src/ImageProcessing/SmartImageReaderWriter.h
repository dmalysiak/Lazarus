/*
 * SmartImageReaderWriter.h
 *
 *  Created on: Jun 29, 2014
 *      Author: Darius Malysiak
 */

#ifndef SMARTIMAGEREADERWRITER_H_
#define SMARTIMAGEREADERWRITER_H_

#include "ImageReaderWriter.h"
#include "JPEGReaderWriter.h"
#include "PNGReaderWriter.h"

namespace Lazarus {

template<class T>
class SmartImageReaderWriter: public Lazarus::ImageReaderWriter<T> {
public:
	SmartImageReaderWriter()
	{

	}

	virtual ~SmartImageReaderWriter()
	{

	}

	virtual void readImage(const std::string &file, enum Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT alignment = Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_RGB)
	{

		//in case of jpeg images
		if( file.substr(file.length()-3,3).compare("jpg") == 0
			|| file.substr(file.length()-4,4).compare("jpeg") == 0
			|| file.substr(file.length()-4,4).compare("JPEG") == 0
			|| file.substr(file.length()-3,3).compare("JPG") == 0)
		{
			Lazarus::JPEGReaderWriter<T> jpeg_reader;
			jpeg_reader.readImage(file,alignment);
			DELETE_NULL_CHECKING(this->mp_image);
			this->mp_image = jpeg_reader.obtain_mp_image();
		}

		//in case of png images
		if( file.substr(file.length()-3,3).compare("png") == 0
			|| file.substr(file.length()-3,3).compare("PNG") == 0)
		{
			Lazarus::PNGReaderWriter<T> png_reader;
			png_reader.readImage(file, alignment);
			DELETE_NULL_CHECKING(this->mp_image);
			this->mp_image = png_reader.obtain_mp_image();
		}

		//update the source filename
		DELETE_NULL_CHECKING(this->mp_filename);
		this->mp_filename = new std::string(file);

	}

	virtual void writeImage(const std::string &file, unsigned int quality)
	{
		//in case of jpeg images
		if( file.substr(file.length()-3,3).compare("jpg") == 0
			|| file.substr(file.length()-4,4).compare("jpeg") == 0
			|| file.substr(file.length()-4,4).compare("JPEG") == 0
			|| file.substr(file.length()-3,3).compare("JPG") == 0)
		{
			Lazarus::JPEGReaderWriter<T> jpeg_reader;
			jpeg_reader.setmp_imageCopy(this->mp_image);
			jpeg_reader.writeImage(file,quality);
		}

		//in case of png images
		if( file.substr(file.length()-3,3).compare("png") == 0
			|| file.substr(file.length()-3,3).compare("PNG") == 0)
		{
			Lazarus::PNGReaderWriter<T> png_reader;
			png_reader.setmp_imageCopy(this->mp_image);
			png_reader.writeImage(file,quality);
		}

		//update the source filename
		DELETE_NULL_CHECKING(this->mp_filename);
		this->mp_filename = new std::string(file);
	}

};

} /* namespace Lazarus */

#endif /* SMARTIMAGEREADERWRITER_H_ */
