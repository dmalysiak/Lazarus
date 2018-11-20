/*
 * ErosionFilter.h
 *
 *  Created on: 13.06.2016
 *      Author: Darius Malysiak
 */

#ifndef IMAGEPROCESSING_THRESHOLDFILTER_H_
#define IMAGEPROCESSING_THRESHOLDFILTER_H_

#include "../BaseObject.h"
#include "../DataStructures/Matrix.h"
#include "../DataStructures/Image.h"

namespace Lazarus {

template<typename T>
class ThresholdFilter: public Lazarus::BaseObject {
public:

	ThresholdFilter()
	{
	}
	virtual ~ThresholdFilter(){}

	/**
	 * Will cut off all values outside (>,<) the range of 'limits', all values outside the given range will
	 * be set to 'val'.
	 * Returns the filtered image in case of success otherwise NULL.
	 **/
	Lazarus::Image<T>* filterImage( Lazarus::Image<T>* image, const Lazarus::ChannelLimits<T>& limits, T val )
	{
		unsigned int image_width = image->getm_width();
		unsigned int image_heigth = image->getm_height();
		unsigned int channel_count = image->getm_channel_count();

		Lazarus::Image<T>* output = new Lazarus::Image<T>( image_width, image_heigth, image->getm_data_alignment() );

		#pragma omp parallel for
		for(unsigned int i=0; i<image_width; i++)
		{
			Lazarus::FastKTuple<T> new_color(channel_count);
			Lazarus::FastKTuple<T> color(channel_count);

			for(unsigned int j=0; j<image_heigth; j++)
			{
				image->getPixelFast(color,i,j);

				//over every color channel
				for(unsigned int c=0; c<channel_count; c++)
				{
					if( color[c] >= limits.m_min_values[c] && color[c] <= limits.m_max_values[c] )
					{
						new_color.setElement(c,color[c]);
					}
					else
					{
						new_color.setElement(c,val);
					}

				}

				output->setPixelFast(&new_color,i,j);

			}
		}

		return output;
	}

};

}


#endif /* IMAGEPROCESSING_EROSIONFILTER_H_ */
