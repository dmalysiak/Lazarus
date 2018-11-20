/*
 * ErosionFilter.h
 *
 *  Created on: 13.06.2016
 *      Author: Darius Malysiak
 */

#ifndef IMAGEPROCESSING_WEIGHTEDMEANFILTER_H_
#define IMAGEPROCESSING_WEIGHTEDMEANFILTER_H_

#include "../BaseObject.h"
#include "../DataStructures/Matrix.h"
#include "../DataStructures/Image.h"

namespace Lazarus {

/**
 * This filter will compute a weighted mean of a pixels region and assign this mean to the pixel.
 * */
template<typename T>
class WeightedMeanFilter: public Lazarus::BaseObject {
public:

	static const Lazarus::Matrix2<double>* get_HOM_MEAN_3x3_KERNEL(double val)
	{
		static Lazarus::Matrix2<double> _EROSION_KERNEL;
		_EROSION_KERNEL.initMatrix(3,3);

		_EROSION_KERNEL.setData(0,0,val);
		_EROSION_KERNEL.setData(0,1,val);
		_EROSION_KERNEL.setData(0,2,val);

		_EROSION_KERNEL.setData(1,0,val);
		_EROSION_KERNEL.setData(1,1,val);
		_EROSION_KERNEL.setData(1,2,val);

		_EROSION_KERNEL.setData(2,0,val);
		_EROSION_KERNEL.setData(2,1,val);
		_EROSION_KERNEL.setData(2,2,val);

		return &_EROSION_KERNEL;
	}

	static const Lazarus::Matrix2<double>* get_HOM_MEAN_KERNEL(double val, unsigned int size)
	{
		Lazarus::Matrix2<double>* _EROSION_KERNEL = new Lazarus::Matrix2<double>();
		_EROSION_KERNEL->initMatrix(size,size);
		_EROSION_KERNEL->globalSetMatrixVal(val);

		return _EROSION_KERNEL;
	}

	WeightedMeanFilter()
	{
		mp_filter_mask = NULL;
	}
	virtual ~WeightedMeanFilter(){}

	void setKernel(const Lazarus::Matrix2<double>* filter)
	{
		this->mp_filter_mask = filter;
	}

	/**
	 * We assume a kernel with odd dimensions. The erosion will be computed on an extended image with black borders
	 * such that the kernel can be positioned onto the first image pixel.
	 * Returns the filtered image in case of success otherwise NULL.
	 **/
	Lazarus::Image<T>* filterImage( Lazarus::Image<T>* image)
	{
		unsigned int offset_x = (mp_filter_mask->getColumnCount()-1)/2;
		unsigned int offset_y = (mp_filter_mask->getRowCount()-1)/2;
		unsigned int image_width = image->getm_width();
		unsigned int image_heigth = image->getm_height();
		unsigned int channel_count = image->getm_channel_count();
		unsigned int filter_width = mp_filter_mask->getColumnCount();
		unsigned int filter_height = mp_filter_mask->getRowCount();

		if(filter_width % 2 != 1)
		{
			printf("filter width %d is not odd\n",filter_width);
			return NULL;
		}
		if(filter_height % 2 != 1)
		{
			printf("filter height %d is not odd\n",filter_height);
			return NULL;
		}

		Lazarus::Image<T>* output = new Lazarus::Image<T>( image_width, image_heigth, image->getm_data_alignment() );
		Lazarus::Image<T>* temporary = new Lazarus::Image<T>( image_width + 2*offset_x,
				image_heigth + 2*offset_y, image->getm_data_alignment() );

		//fill the output and temp image with black
		Lazarus::FastKTuple<T> color(channel_count);

		for(unsigned int i=0; i< channel_count; i++)
		{
			color.setElement(i,0);
		}

		output->fillImageFast( &color );
		temporary->fillImageFast( &color );


		//copy the input image into the temp buffer;
		for(unsigned int i=0; i<image_width; i++)
		{
			for(unsigned int j=0; j<image_heigth; j++)
			{
				image->getPixelFast( &color,i,j );
				temporary->setPixelFast(&color,offset_x + i,offset_y + j);
			}
		}

		//start the convolution process

		//over every pixel
		//unsigned int c_limit = std::max(channel_count,(unsigned int)3);
		unsigned int c_limit = 0;
		if(channel_count > 3)
			c_limit = 3;
		else
			c_limit = channel_count;

		#pragma omp parallel for
		for(unsigned int i=offset_x; i<image_width+(offset_x); i++)
		{
			double filter_value = 0;
			double average = 0;

			Lazarus::FastKTuple<T> new_color(channel_count);
			Lazarus::FastKTuple<T> color_(channel_count);

			for(unsigned int j=offset_y; j<image_heigth+(offset_y); j++)
			{
				//over every color channel
				for(unsigned int c=0; c<c_limit; c++)
				{
					//erosion
					for(int k=-offset_x; k<=(int)offset_x; ++k)
					{
						for(int l=-offset_y; l<=(int)offset_y; ++l)
						{
							temporary->getPixelFast(&color_, (unsigned int)((int)i+k),
									(unsigned int)((int)j+l));
							filter_value = mp_filter_mask->getData((unsigned int)((int)offset_x+k),
									(unsigned int)((int)offset_y+l) );

							average += (color_.getElement(c) * filter_value);

						}

					}

					new_color.setElement(c,(T)(average/(filter_width*filter_height)));
					average = 0;
				}

				//preserve the alpha value
				if(channel_count>3)
				{
					new_color.setElement(3,color_.getElement(3));
				}

				output->setPixelFast(&new_color,i-(offset_x),j-(offset_y));

			}
		}

		//delete the temporary image
		delete temporary;

		return output;
	}

private:
	const Lazarus::Matrix2<double>* mp_filter_mask;
};

}


#endif /* IMAGEPROCESSING_WEIGHTEDMEANFILTER_H_ */
