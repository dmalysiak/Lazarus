/*
 * ImageReaderWriter.h
 *
 *  Created on: 07.02.2013
 *      Author: Darius Malysiak
 */

#ifndef IMAGEREADERWRITER_H_
#define IMAGEREADERWRITER_H_

#include "../BaseObject.h"
#include "../DataStructures/Image.h"

#include <stdio.h>
#include <string>

namespace Lazarus
{

template<class T>
class ImageReaderWriter: public Lazarus::BaseObject {
public:
	ImageReaderWriter()
	{
		mp_filename = NULL;
		mp_image = NULL;
	}

	virtual ~ImageReaderWriter()
	{
		DELETE_NULL_CHECKING(mp_filename);
		DELETE_NULL_CHECKING(mp_image);
	}

	virtual void readImage(const std::string &file, enum Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT alignment = Lazarus::Image<T>::IMAGE_DATA_ALIGNMENT_RGB)
	{
		printf("not implemented yet\n");
	}

	virtual void writeImage(const std::string &file)
	{
		printf("not implemented yet\n");
	}

	virtual void writeImage(const std::string &file, unsigned int quality)
	{
		printf("not implemented yet\n");
	}

	Lazarus::Image<T>* getmp_image()
	{
		return this->mp_image;
	}

	//the caller of this method basically takes complete ownership of the image object
	//I.e. the ImageReaderWriter object looses the image (mp_image = NULL)!
	Lazarus::Image<T>* obtain_mp_image()
	{
		Lazarus::Image<T>* image = this->mp_image; //copy the pointer
		this->mp_image = NULL; //obtain the image
		return image;
	}

	//takes ownership of the image
	void setmp_image(Lazarus::Image<T>* image)
	{
		//delete the old image
		DELETE_NULL_CHECKING(mp_image);

		//update the image
		this->mp_image = image;

		//reset the source file
		DELETE_NULL_CHECKING(mp_filename);
	}

	//creates a copy of the image
	void setmp_imageCopy(Lazarus::Image<T>* image)
	{
		//delete the old image
		DELETE_NULL_CHECKING(mp_image);

		//update the image
		this->mp_image = new Lazarus::Image<T>(image);

		//reset the source file
		DELETE_NULL_CHECKING(mp_filename);
	}

protected:
	Lazarus::Image<T>* mp_image;
	std::string* mp_filename;
};

}

#endif /* IMAGEREADERWRITER_H_ */
