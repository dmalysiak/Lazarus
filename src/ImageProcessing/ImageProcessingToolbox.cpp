/*
 * Toolbox.cpp
 *
 *  Created on: Feb 24, 2014
 *      Author: Darius Malysiak
 */

#include "ImageProcessingToolbox.h"

#include <cmath>
#include <omp.h>

namespace Lazarus {

ImageProcessingToolbox::ImageProcessingToolbox() {
	// TODO Auto-generated constructor stub

}

ImageProcessingToolbox::~ImageProcessingToolbox() {
	// TODO Auto-generated destructor stub
}

void ImageProcessingToolbox::convertYUV422ToRGB(char *pRawImage, unsigned char *pRGBImage,unsigned int width, unsigned int height)
{

    #pragma omp parallel
	{
		// y1 and y2 represent the luma component (the brightness)
		// u and v are the chrominance (color) components
		unsigned char y1, y2, u, v, c1, c2;
		char d, e;
		unsigned int j = std::floor((1600.0*1200.0*3.0)/(double)omp_get_num_threads())*omp_get_thread_num();// width = 720, height = 576;
		int r1, g1, b1, r2, g2, b2;

		#pragma omp for
		for (unsigned int i = 0; i < width * height * 2; i += 4)
		{

			// we get y1 u y2 v
			y1 = pRawImage[i];
			u = pRawImage[i+1];
			y2 = pRawImage[i+2];
			v = pRawImage[i+3];

			// check whether c1 would become negative
			if (y1 < 16)
			{
				c1 = 0;
			}
			else
			{
				c1 = y1 -16;
			}
			// check whether c2 would become negative
			if (y2 < 16)
			{
				c2 = 0;
			}
			else
			{
				c2 = y2 -16;
			}
			d = u - 128;
			e = v - 128;

			r1 = (298*c1 + 409*e) >> 8;
			g1 = (298*c1 - 100*d - 208*e +128) >> 8;
			b1 = (298*c1 + 516*d +128) >> 8;

			r2 = (298*c2 + 409*e) >> 8;
			g2 = (298*c2 - 100*d - 208*e +128) >> 8;
			b2 = (298*c2 + 516*d +128) >> 8;

			clip0_255(r1);
			clip0_255(g1);
			clip0_255(b1);

			clip0_255(r2);
			clip0_255(g2);
			clip0_255(b2);

			pRGBImage[j++] = (unsigned char) r1;
			pRGBImage[j++] = (unsigned char) g1;
			pRGBImage[j++] = (unsigned char) b1;

			pRGBImage[j++] = (unsigned char) r2;
			pRGBImage[j++] = (unsigned char) g2;
			pRGBImage[j++] = (unsigned char) b2;
		}
	}

	return;
}


void ImageProcessingToolbox::convertYUV422ToRGBSequential(char *pRawImage, unsigned char *pRGBImage,unsigned int width, unsigned int height)
{

	// y1 and y2 represent the luma component (the brightness)
	// u and v are the chrominance (color) components
	unsigned char y1, y2, u, v, c1, c2;
	char d, e;
	unsigned int j = 0;// width = 720, height = 576;
	int r1, g1, b1, r2, g2, b2;

	for (unsigned int i = 0; i < width * height * 2; i += 4)
	{

		// we get y1 u y2 v
		y1 = pRawImage[i];
		u = pRawImage[i+1];
		y2 = pRawImage[i+2];
		v = pRawImage[i+3];

		// check whether c1 would become negative
		if (y1 < 16)
		{
			c1 = 0;
		}
		else
		{
			c1 = y1 -16;
		}
		// check whether c2 would become negative
		if (y2 < 16)
		{
			c2 = 0;
		}
		else
		{
			c2 = y2 -16;
		}
		d = u - 128;
		e = v - 128;

		r1 = (298*c1 + 409*e) >> 8;
		g1 = (298*c1 - 100*d - 208*e +128) >> 8;
		b1 = (298*c1 + 516*d +128) >> 8;

		r2 = (298*c2 + 409*e) >> 8;
		g2 = (298*c2 - 100*d - 208*e +128) >> 8;
		b2 = (298*c2 + 516*d +128) >> 8;

		clip0_255(r1);
		clip0_255(g1);
		clip0_255(b1);

		clip0_255(r2);
		clip0_255(g2);
		clip0_255(b2);

		pRGBImage[j++] = (unsigned char) r1;
		pRGBImage[j++] = (unsigned char) g1;
		pRGBImage[j++] = (unsigned char) b1;

		pRGBImage[j++] = (unsigned char) r2;
		pRGBImage[j++] = (unsigned char) g2;
		pRGBImage[j++] = (unsigned char) b2;
	}
}


} /* namespace Lazarus */
