/*
 * Toolbox.h
 *
 *  Created on: Feb 24, 2014
 *      Author: Darius Malysiak
 */

#ifndef IMGTOOLBOX_H_
#define IMGTOOLBOX_H_

#include "../BaseObject.h"

namespace Lazarus {

class ImageProcessingToolbox: public Lazarus::BaseObject {
public:
	ImageProcessingToolbox();
	virtual ~ImageProcessingToolbox();

	// clips value to the range {0,...,255}
	static void inline clip0_255(int &value)
	{
		if (value < 0)
		{
			value = 0;
		}
		else if (value > 255)
		{
			value = 255;
		}

		return;
	}

	static void convertYUV422ToRGB(char *pRawImage, unsigned char *pRGBImage,unsigned int width, unsigned int height);
	static void convertYUV422ToRGBSequential(char *pRawImage, unsigned char *pRGBImage,unsigned int width, unsigned int height);
};

} /* namespace Lazarus */

#endif /* IMGTOOLBOX_H_ */
