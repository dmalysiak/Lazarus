/*
 * ErosionFilter.h
 *
 *  Created on: 13.06.2016
 *      Author: Darius Malysiak
 */

#ifndef IMAGEPROCESSING_OPENINGFILTER_H_
#define IMAGEPROCESSING_OPENINGFILTER_H_

#include "../BaseObject.h"
#include "ErosionFilter.h"
#include "DilationFilter.h"

namespace Lazarus {

template<typename T>
class OpeningFilter: public Lazarus::BaseObject {
public:

	OpeningFilter()
	{
		mp_filter_mask1 = NULL;
		mp_filter_mask2 = NULL;
	}
	virtual ~OpeningFilter(){}

	void setKernels(const Lazarus::Matrix2<double>* filter1,const Lazarus::Matrix2<double>* filter2)
	{
		this->mp_filter_mask1 = filter1;
		this->mp_filter_mask1 = filter2;
	}

	/**
	 * We assume kernels with odd dimensions. The opening will be computed on an extended image with black borders
	 * such that the kernel can be positioned onto the first image pixel.
	 * Returns the filtered image in case of success otherwise NULL.
	 **/
	Lazarus::Image<T>* filterImage( Lazarus::Image<T>* image, double clamping_val=255.0 )
	{
		Lazarus::DilationFilter<T> filter1;
		Lazarus::ErosionFilter<T> filter2;

		filter1.setDilationKernel( mp_filter_mask2 );
		Lazarus::Image<T>* resultImage1 = filter2.filterImage(image);

		filter2.setErosionKernel( mp_filter_mask1 );
		Lazarus::Image<T>* resultImage2 = filter1.filterImage(resultImage1);

		delete resultImage1;

		return resultImage2;
	}

	/**
	 * We assume kernels with odd dimensions. The opening will be computed on an extended image with black borders
	 * such that the kernel can be positioned onto the first image pixel.
	 * Returns the filtered image in case of success otherwise NULL.
	 **/
	Lazarus::Image<T>* filterImageBW( Lazarus::Image<T>* image, double white=255.0 )
	{
		Lazarus::DilationFilter<T> filter1;
		Lazarus::ErosionFilter<T> filter2;

		filter1.setDilationKernel( mp_filter_mask2 );
		Lazarus::Image<T>* resultImage1 = filter2.filterImageBW(image);

		filter2.setErosionKernel( mp_filter_mask1 );
		Lazarus::Image<T>* resultImage2 = filter1.filterImageBW(resultImage1);

		delete resultImage1;

		return resultImage2;
	}

private:
	const Lazarus::Matrix2<double>* mp_filter_mask1;
	const Lazarus::Matrix2<double>* mp_filter_mask2;

};

}


#endif /* IMAGEPROCESSING_EROSIONFILTER_H_ */
