/*
 * ErosionFilter.h
 *
 *  Created on: 13.06.2016
 *      Author: Darius Malysiak
 */

#ifndef IMAGEPROCESSING_EMBEDDINGFILTER_H_
#define IMAGEPROCESSING_EMBEDDINGFILTER_H_

#include "../BaseObject.h"
#include "../DataStructures/Image.h"

namespace Lazarus {

template<typename T>
class EmbeddingFilter: public Lazarus::BaseObject {
public:

	EmbeddingFilter(unsigned long long out_width, unsigned long long out_height)
	{
		m_out_height = out_height;
		m_out_width = out_width;
		mp_output_image = NULL;

	}
	virtual ~EmbeddingFilter()
	{
		DELETE_NULL_CHECKING(mp_output_image);
	}

	/**
	 * This will embed the subimage of a given image in a larger image at the given position (output_x,output_y).
	 * The background will be filled with back_color. Do not delete the returned image!
	 * This method will be slightly more efficient since it dows not allocate and initialize a new output image with each
	 * call!
	 **/
	Lazarus::Image<T>* filterImageFlow( Lazarus::Image<T>* image, unsigned long long input_start_x, unsigned long long input_start_y,
			unsigned long long input_width, unsigned long long input_height, unsigned long long output_x, unsigned long long output_y,
			Lazarus::FastKTuple<T>& back_color )
	{

		if(mp_output_image == NULL)
		{
			mp_output_image = new Lazarus::Image<T>( m_out_width, m_out_height, image->getm_data_alignment() );

			//fill the output image
			mp_output_image->fillImageFast( &back_color );
		}

		Lazarus::FastKTuple<T> pixel(image->channels());

		//copy the input image into the temp buffer;
		for(unsigned int i=0; i<input_width; i++)
		{
			for(unsigned int j=0; j<input_height; j++)
			{
				image->getPixelFast( &pixel,input_start_x+i,input_start_y+j );
				mp_output_image->setPixelFast(&pixel,output_x + i,output_y + j);
			}
		}

		return mp_output_image;
	}

	/**
	 * This will embed the given image in a larger image at the given position (output_x,output_y).
	 * The background will be filled with back_color. Do not delete the returned image!
	 * This method will be slightly more efficient since it dows not allocate and initialize a new output image with each
	 **/
	Lazarus::Image<T>* filterImageFlow( Lazarus::Image<T>* image, unsigned long long output_x, unsigned long long output_y,
			Lazarus::FastKTuple<T>& back_color )
	{

		if(mp_output_image == NULL)
		{
			mp_output_image = new Lazarus::Image<T>( m_out_width, m_out_height, image->getm_data_alignment() );

			//fill the output image
			mp_output_image->fillImageFast( &back_color );
		}

		Lazarus::FastKTuple<T> pixel(image->channels());

		//copy the input image into the temp buffer;
		for(unsigned int i=0; i<image->width(); i++)
		{
			for(unsigned int j=0; j<image->height(); j++)
			{
				image->getPixelFast( &pixel,i,j );
				mp_output_image->setPixelFast(&pixel,output_x + i,output_y + j);
			}
		}

		return mp_output_image;
	}

	/**
	 * This will embed the subimage of a given image in a larger image at the given position (output_x,output_y). The returned image may be deleted afterwards.
	 * The background will be filled with back_color.
	 **/
	Lazarus::Image<T>* filterImage( Lazarus::Image<T>* image, unsigned long long input_start_x, unsigned long long input_start_y,
			unsigned long long input_width, unsigned long long input_height, unsigned long long output_x, unsigned long long output_y,
			Lazarus::FastKTuple<T>& back_color )
	{

		Lazarus::Image<T>* output = new Lazarus::Image<T>( m_out_width, m_out_height, image->getm_data_alignment() );

		//fill the output image
		output->fillImageFast( &back_color );

		Lazarus::FastKTuple<T> pixel(image->channels());

		//copy the input image into the temp buffer;
		for(unsigned int i=0; i<input_width; i++)
		{
			for(unsigned int j=0; j<input_height; j++)
			{
				image->getPixelFast( &pixel,input_start_x+i,input_start_y+j );
				output->setPixelFast(&pixel,output_x + i,output_y + j);
			}
		}

		return output;
	}

	/**
	 * This will embed the given image in a larger image at the given position (output_x,output_y). The returned image may be deleted afterwards.
	 * The background will be filled with back_color.
	 **/
	Lazarus::Image<T>* filterImage( Lazarus::Image<T>* image, unsigned long long output_x, unsigned long long output_y,
			Lazarus::FastKTuple<T>& back_color )
	{

		Lazarus::Image<T>* output = new Lazarus::Image<T>( m_out_width, m_out_height, image->getm_data_alignment() );

		//fill the output image
		output->fillImageFast( &back_color );

		Lazarus::FastKTuple<T> pixel(image->channels());

		//copy the input image into the temp buffer;
		for(unsigned int i=0; i<image->width(); i++)
		{
			for(unsigned int j=0; j<image->height(); j++)
			{
				image->getPixelFast( &pixel,i,j );
				output->setPixelFast(&pixel,output_x + i,output_y + j);
			}
		}

		return output;
	}


private:
	unsigned long long m_out_height;
	unsigned long long m_out_width;

	Lazarus::Image<T>* mp_output_image;
};

}


#endif /* IMAGEPROCESSING_EROSIONFILTER_H_ */
