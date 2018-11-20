/*
 * ConvolutionFilter.h
 *
 *  Created on: 12.07.2013
 *      Author: darius
 */

#ifndef EDGEDETECTOR_H_
#define EDGEDETECTOR_H_

#include "../BaseObject.h"
#include "../DataStructures/Matrix.h"

namespace Lazarus {

template<typename T>
class EdgeDetector: public Lazarus::BaseObject {
public:

	EdgeDetector()
	{
		mp_filter_mask_x = NULL;
		mp_filter_mask_y = NULL;
	}

	/**
	 * The filter masks must be of identical size.
	 * */
	EdgeDetector(Lazarus::Matrix2<double>* filterX, Lazarus::Matrix2<double>* filterY)
	{
		mp_filter_mask_x = filterX;
		mp_filter_mask_y = filterY;
	}
	virtual ~EdgeDetector(){}

	/**
	 * The filter masks must be of identical size.
	 * */
	void setFilterMasks(Lazarus::Matrix2<double>* filterX, Lazarus::Matrix2<double>* filterY)
	{
		mp_filter_mask_x = filterX;
		mp_filter_mask_y = filterY;

		//print warning if filters are of different size
		if( (mp_filter_mask_x->rows() != mp_filter_mask_y->rows()) ||
			( mp_filter_mask_x->columns() != mp_filter_mask_x->columns() ) )
		{
			printf("ERROR: filter masks are of different size\n\n");
		}
	}

	/**
	 * We assume a filter mask with odd dimensions. The convolution will be computed on an extended image with black borders
	 * such that the kernel can be positioned onto the first image pixel.
	 * Returns the gradient matrix (each level represents a color channel) in case of success otherwise NULL.
	 * */
	Lazarus::Matrix3<std::complex<double>>* detectEdges( Lazarus::Image<T>* image)
	{
		unsigned int offset_x = (mp_filter_mask_x->getColumnCount()-1)/2;
		unsigned int offset_y = (mp_filter_mask_x->getRowCount()-1)/2;
		unsigned int image_width = image->getm_width();
		unsigned int image_heigth = image->getm_height();
		unsigned int channel_count = image->getm_channel_count();
		unsigned int filter_width = mp_filter_mask_x->getColumnCount();
		unsigned int filter_height = mp_filter_mask_x->getRowCount();

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

		//print warning if filters are of different size
		if( (mp_filter_mask_x->rows() != mp_filter_mask_y->rows()) ||
			( mp_filter_mask_x->columns() != mp_filter_mask_x->columns() ) )
		{
			printf("ERROR: filter masks are of different size\n\n");
			return NULL;
		}

		Lazarus::Matrix3<std::complex<double>>* output = new Lazarus::Matrix3<std::complex<double>>( image_heigth,image_width, image->channels() );
		Lazarus::Image<T>* temporary = new Lazarus::Image<T>( image_width + 2*offset_x,
				image_heigth + 2*offset_y, image->getm_data_alignment() );

		//fill the output and temp image with black
		Lazarus::FastKTuple<T> color(channel_count);

		for(unsigned int i=0; i< channel_count; i++)
		{
			color.setElement(i,0);
		}
		output->globalSetMatrixVal( std::complex<double>(0.0,0.0) );//init the output matrix to zero
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

		#pragma omp parallel for
		for(unsigned int i=offset_x; i<image_width+(offset_x); i++)
		{
			double temp_valueX = 0;
			double temp_valueY = 0;
			double filter_valueX = 0;
			double filter_valueY = 0;
			Lazarus::FastKTuple<T> new_color(channel_count);
			Lazarus::FastKTuple<T> color_(channel_count);
			unsigned int c_limit = std::max(channel_count,(unsigned int)3);

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
							filter_valueX = mp_filter_mask_x->getData((unsigned int)((int)offset_x+k),
									(unsigned int)((int)offset_y+l) );
							filter_valueY = mp_filter_mask_y->getData((unsigned int)((int)offset_x+k),
									(unsigned int)((int)offset_y+l) );
							temp_valueX += (double)(color_.getElement(c))*filter_valueX;
							temp_valueY += (double)(color_.getElement(c))*filter_valueY;
						}
					}

					output->setData( i-(offset_x),j-(offset_y),c,std::complex<double>( temp_valueX,temp_valueY) ) ;
					temp_valueX=0;
					temp_valueY=0;

				}

			}
		}

		//delete the temporary image
		delete temporary;

		return output;
	}



	/**
	 * We assume a filter mask with odd dimensions. The convolution will be computed on an extended image with black borders
	 * such that the kernel can be positioned onto the first image pixel.
	 * Returns the gradient matrix (each level represents a color channel) in case of success otherwise NULL.
	 * */
	Lazarus::Image<T>* detectEdgesAbs(Lazarus::Image<T>* image, const Lazarus::ChannelLimits<T>& limits = Lazarus::ChannelLimits<T>())
	{
		unsigned int offset_x = (mp_filter_mask_x->getColumnCount()-1)/2;
		unsigned int offset_y = (mp_filter_mask_x->getRowCount()-1)/2;
		unsigned int image_width = image->getm_width();
		unsigned int image_heigth = image->getm_height();
		unsigned int channel_count = image->getm_channel_count();
		unsigned int filter_width = mp_filter_mask_x->getColumnCount();
		unsigned int filter_height = mp_filter_mask_x->getRowCount();

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

		//print warning if filters are of different size
		if( (mp_filter_mask_x->rows() != mp_filter_mask_y->rows()) ||
			( mp_filter_mask_x->columns() != mp_filter_mask_x->columns() ) )
		{
			printf("ERROR: filter masks are of different size\n\n");
			return NULL;
		}

		Lazarus::Image<T>* output = new Lazarus::Image<T>( image_width, image_heigth, image->getm_data_alignment(),image->getChannelLimits()  );
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
		//cap the channel count
		if(channel_count > 3)
			c_limit = 3;
		else
			c_limit = channel_count;

		#pragma omp parallel for
		for(unsigned int i=offset_x; i<image_width+(offset_x); i++)
		{
			double temp_valueX = 0;
			double temp_valueY = 0;
			double filter_valueX = 0;
			double filter_valueY = 0;
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
							filter_valueX = mp_filter_mask_x->getData((unsigned int)((int)offset_x+k),
									(unsigned int)((int)offset_y+l) );
							filter_valueY = mp_filter_mask_y->getData((unsigned int)((int)offset_x+k),
									(unsigned int)((int)offset_y+l) );
							temp_valueX += (double)(color_.getElement(c))*filter_valueX;
							temp_valueY += (double)(color_.getElement(c))*filter_valueY;
						}
					}
					new_color.setElement(c,(T)std::min(std::max( std::sqrt(temp_valueX*temp_valueX+temp_valueY*temp_valueY) ,(double)std::numeric_limits<T>::min()),(double)image->getChannelLimits().m_max_values.getElement(c)));
					temp_valueX=0;
					temp_valueY=0;
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
	Lazarus::Matrix2<double>* mp_filter_mask_x;
	Lazarus::Matrix2<double>* mp_filter_mask_y;
};


} /* namespace Lazarus */
#endif /* CONVOLUTIONFILTER_H_ */
