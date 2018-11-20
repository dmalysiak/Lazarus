/*
 * ConvolutionFilter.h
 *
 *  Created on: 12.07.2013
 *      Author: darius
 */

#ifndef CONVOLUTIONFILTER_H_
#define CONVOLUTIONFILTER_H_

#include "../BaseObject.h"
#include "../DataStructures/Matrix.h"

namespace Lazarus {

template<typename T>
class ConvolutionFilter: public Lazarus::BaseObject {
public:

	static Lazarus::Matrix2<double>* get_Sobel_X_KERNEL()
	{
		static Lazarus::Matrix2<double> _SOBEL_KERNEL;
		_SOBEL_KERNEL.initMatrix(3,3);

		_SOBEL_KERNEL.setData(0,0,-1.0);
		_SOBEL_KERNEL.setData(0,1,0.0);
		_SOBEL_KERNEL.setData(0,2,1.0);

		_SOBEL_KERNEL.setData(1,0,-2.0);
		_SOBEL_KERNEL.setData(1,1,0.0);
		_SOBEL_KERNEL.setData(1,2,+2.0);

		_SOBEL_KERNEL.setData(2,0,-1.0);
		_SOBEL_KERNEL.setData(2,1,0.0);
		_SOBEL_KERNEL.setData(2,2,1.0);

		return &_SOBEL_KERNEL;
	}

	static Lazarus::Matrix2<double>* get_Sobel_Y_KERNEL()
	{
		static Lazarus::Matrix2<double> _SOBEL_KERNEL;
		_SOBEL_KERNEL.initMatrix(3,3);

		_SOBEL_KERNEL.setData(0,0,1.0);
		_SOBEL_KERNEL.setData(0,1,2.0);
		_SOBEL_KERNEL.setData(0,2,1.0);

		_SOBEL_KERNEL.setData(1,0,0.0);
		_SOBEL_KERNEL.setData(1,1,0.0);
		_SOBEL_KERNEL.setData(1,2,0.0);

		_SOBEL_KERNEL.setData(2,0,-1.0);
		_SOBEL_KERNEL.setData(2,1,-2.0);
		_SOBEL_KERNEL.setData(2,2,-1.0);

		return &_SOBEL_KERNEL;
	}

	static Lazarus::Matrix2<double>* get_UNSHARP5x5_KERNEL()
	{
		static Lazarus::Matrix2<double> _SHARPEN_KERNEL;
		_SHARPEN_KERNEL.initMatrix(5,5);

		_SHARPEN_KERNEL.setData(0,0,-1.0/256.0);
		_SHARPEN_KERNEL.setData(0,1,-4.0/256.0);
		_SHARPEN_KERNEL.setData(0,2,-6.0/256.0);
		_SHARPEN_KERNEL.setData(0,3,-4.0/256.0);
		_SHARPEN_KERNEL.setData(0,4,-1.0/256.0);

		_SHARPEN_KERNEL.setData(1,0,-4.0/256.0);
		_SHARPEN_KERNEL.setData(1,1,-16.0/256.0);
		_SHARPEN_KERNEL.setData(1,2,-24.0/256.0);
		_SHARPEN_KERNEL.setData(1,3,-16.0/256.0);
		_SHARPEN_KERNEL.setData(1,4,-4.0/256.0);

		_SHARPEN_KERNEL.setData(2,0,-6.0/256.0);
		_SHARPEN_KERNEL.setData(2,1,-24.0/256.0);
		_SHARPEN_KERNEL.setData(2,2,476.0/256.0);
		_SHARPEN_KERNEL.setData(2,3,-24.0/256.0);
		_SHARPEN_KERNEL.setData(2,4,-6.0/256.0);

		_SHARPEN_KERNEL.setData(3,0,-4.0/256.0);
		_SHARPEN_KERNEL.setData(3,1,-16.0/256.0);
		_SHARPEN_KERNEL.setData(3,2,-24.0/256.0);
		_SHARPEN_KERNEL.setData(3,3,-16.0/256.0);
		_SHARPEN_KERNEL.setData(3,4,-4.0/256.0);

		_SHARPEN_KERNEL.setData(4,0,-1.0/256.0);
		_SHARPEN_KERNEL.setData(4,1,-4.0/256.0);
		_SHARPEN_KERNEL.setData(4,2,-6.0/256.0);
		_SHARPEN_KERNEL.setData(4,3,-4.0/256.0);
		_SHARPEN_KERNEL.setData(4,4,-1.0/256.0);

		return &_SHARPEN_KERNEL;
	}

	static Lazarus::Matrix2<double>* get_SHARPEN_KERNEL()
	{
		static Lazarus::Matrix2<double> _SHARPEN_KERNEL;
		_SHARPEN_KERNEL.initMatrix(3,3);

		_SHARPEN_KERNEL.setData(0,0,1.0/16.0);
		_SHARPEN_KERNEL.setData(0,1,2.0/16.0);
		_SHARPEN_KERNEL.setData(0,2,1.0/16.0);

		_SHARPEN_KERNEL.setData(1,0,2.0/16.0);
		_SHARPEN_KERNEL.setData(1,1,4.0/16.0);
		_SHARPEN_KERNEL.setData(1,2,2.0/16.0);

		_SHARPEN_KERNEL.setData(2,0,1.0/16.0);
		_SHARPEN_KERNEL.setData(2,1,2.0/16.0);
		_SHARPEN_KERNEL.setData(2,2,1.0/16.0);

		return &_SHARPEN_KERNEL;
	}

	static Lazarus::Matrix2<double>* get_GAUSSIAN_BLUR_KERNEL()
	{
		static Lazarus::Matrix2<double> _GBLUR_KERNEL;
		_GBLUR_KERNEL.initMatrix(3,3);

		_GBLUR_KERNEL.setData(0,0,1.0/16.0);
		_GBLUR_KERNEL.setData(0,1,2.0/16.0);
		_GBLUR_KERNEL.setData(0,2,1.0/16.0);

		_GBLUR_KERNEL.setData(1,0,2.0/16.0);
		_GBLUR_KERNEL.setData(1,1,4.0/16.0);
		_GBLUR_KERNEL.setData(1,2,2.0/16.0);

		_GBLUR_KERNEL.setData(2,0,1.0/16.0);
		_GBLUR_KERNEL.setData(2,1,2.0/16.0);
		_GBLUR_KERNEL.setData(2,2,1.0/16.0);

		return &_GBLUR_KERNEL;
	}

	static Lazarus::Matrix2<double>* get_BOX_BLUR_KERNEL()
	{
		static Lazarus::Matrix2<double> _BBLUR_KERNEL;
		_BBLUR_KERNEL.initMatrix(3,3);

		_BBLUR_KERNEL.setData(0,0,1.0/9.0);
		_BBLUR_KERNEL.setData(0,1,1.0/9.0);
		_BBLUR_KERNEL.setData(0,2,1.0/9.0);

		_BBLUR_KERNEL.setData(1,0,1.0/9.0);
		_BBLUR_KERNEL.setData(1,1,1.0/9.0);
		_BBLUR_KERNEL.setData(1,2,1.0/9.0);

		_BBLUR_KERNEL.setData(2,0,1.0/9.0);
		_BBLUR_KERNEL.setData(2,1,1.0/9.0);
		_BBLUR_KERNEL.setData(2,2,1.0/9.0);

		return &_BBLUR_KERNEL;
	}

	ConvolutionFilter()
	{
		mp_filter_mask = NULL;
	}
	ConvolutionFilter(Lazarus::Matrix2<double>* filter)
	{
		mp_filter_mask = filter;
	}
	virtual ~ConvolutionFilter(){}

	void setFilterMask(Lazarus::Matrix2<double>* filter)
	{
		this->mp_filter_mask = filter;
	}

	/**
	 * We assume a filter mask with odd dimensions. The convolution will be computed on an extended image with black borders
	 * such that the kernel can be positioned onto the first image pixel.
	 * Returns the filtered image in case of success otherwise NULL.
	 * */
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

		Lazarus::Image<T>* output = new Lazarus::Image<T>( image_width, image_heigth, image->getm_data_alignment(),image->getChannelLimits() );
		Lazarus::Image<T>* temporary = new Lazarus::Image<T>( image_width + 2*offset_x,
				image_heigth + 2*offset_y, image->getm_data_alignment(),image->getChannelLimits() );

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
		unsigned int c_limit = 0;
		if(channel_count > 3)
			c_limit = 3;
		else
			c_limit = channel_count;

		#pragma omp parallel for
		for(unsigned int i=offset_x; i<image_width+(offset_x); i++)
		{

			double temp_value = 0;
			double filter_value = 0;
			Lazarus::FastKTuple<T> new_color(channel_count);
			Lazarus::FastKTuple<T> color_(channel_count);

			for(unsigned int j=offset_y; j<image_heigth+(offset_y); j++)
			{
				//over every color channel
				for(unsigned int c=0; c<c_limit; c++)
				{
					//convolution
					for(int k=-offset_x; k<=(int)offset_x; ++k)
					{
						for(int l=-offset_y; l<=(int)offset_y; ++l)
						{
							temporary->getPixelFast(&color_, (unsigned int)((int)i+k),
									(unsigned int)((int)j+l));
							filter_value = mp_filter_mask->getData((unsigned int)((int)offset_x+k),
									(unsigned int)((int)offset_y+l) );
							temp_value += (double)(color_.getElement(c))*filter_value;

						}
					}
					new_color.setElement(c,(T)std::min(std::max(temp_value,(double)std::numeric_limits<T>::min()),(double)image->getChannelLimits().m_max_values.getElement(c)));
					temp_value=0;
				}

				//set the alpha value to the image value
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
	Lazarus::Matrix2<double>* mp_filter_mask;
};


} /* namespace Lazarus */
#endif /* CONVOLUTIONFILTER_H_ */
