/*
 * ImageGrid.h
 *
 *  Created on: Oct 16, 2013
 *      Author: clustro
 */

#ifndef IMAGEGRID_H_
#define IMAGEGRID_H_

#include "../BaseObject.h"
#include "KTuple.h"

namespace Lazarus {

template<typename T>
class ImageGrid: public Lazarus::BaseObject {
public:
	ImageGrid(unsigned int x, unsigned int y, unsigned int block_width, unsigned int block_height)
	{
		m_image_count_x = x;
		m_image_count_y = y;

		m_block_width = block_width;
		m_block_height = block_height;

		mp_images = new KTuple< Image<T>* >( x * y );

		mp_rendered_image = NULL;
	}

	virtual ~ImageGrid()
	{
		DELETE_NULL_CHECKING(mp_images);
		DELETE_NULL_CHECKING(mp_rendered_image);
	}

	/*
	 * this only copies the pointer to an image, it doesn't create a deep copy.
	 */
	void setImage(unsigned int x, unsigned int y, Image<T>* image)
	{
		//check image dimensions
		if( image->getm_width() != m_block_width || image->getm_height() != m_block_height)
		{
			printf("ERROR: could not add image into image grid; size mismatch\n");
			return;
		}

		mp_images->setElement( y * m_image_count_x + x ,image);
	}

	Image<T>* getRenderedImage() const
	{
		return mp_rendered_image;
	}

	Image<T>* obtainRenderedImage()
	{
		Image<T>* output = mp_rendered_image;

		mp_rendered_image = NULL;

		return output;
	}

	Image<T>* renderImage()
	{
		//delete any existing rendered image
		DELETE_NULL_CHECKING(mp_rendered_image);

		unsigned int render_width = m_block_width * m_image_count_x;
		unsigned int render_height = m_block_height * m_image_count_y;

		Image<T>* current_image = NULL;
		T* current_image_data = NULL;

		mp_rendered_image = new Image<T>(render_width, render_height, Image<T>::IMAGE_DATA_ALIGNMENT_RGBA);
		T* image_data = mp_rendered_image->getmp_image_data();

		//reset the rendered image
		KTuple<T> color(4);
		color.setElement(0,0);
		color.setElement(1,0);
		color.setElement(2,0);
		color.setElement(3,255);
		mp_rendered_image->fillImage(&color);

		unsigned int row_size = m_block_width*4;
		unsigned int row_size_current_image = 0;

		unsigned int offset = 0;

		for(unsigned int y=0;y<m_image_count_y;y++)
		{
			for(unsigned int x=0;x<m_image_count_x;x++)
			{
				//get image (x,y)
				current_image = mp_images->getElement( y * m_image_count_x + x);

				//if no image has been set: skip iteration
				if(current_image == NULL)
				{
					continue;
				}

				//get pointer to the image data of the current image
				current_image_data = current_image->getmp_image_data();

				//determine row size
				row_size_current_image = m_block_width*current_image->getm_channel_count();

				if(row_size == row_size_current_image)
				{
					//copy image (x,y) into the final image: line by line
					for(unsigned int k=0;k<m_block_height;k++)
					{
						offset = (y+k) * row_size * m_image_count_x + row_size * x;

						memcpy(image_data + offset, current_image_data + k*row_size_current_image,row_size_current_image * sizeof(T));
					}
				}
				else
				{
					T* local_buff = NULL;
					unsigned int current_image_data_offset = 0;

					//copy image pixel by pixel
					for(unsigned int i=0;i<m_block_height;i++)
					{
						//set the offset to the appropriate line
						offset = (y+i) * row_size * m_image_count_x + row_size * x;

						for(unsigned int j=0;j<m_block_width;j++)
						{
							//add the x position within the current line and add it to the pointer
							local_buff = image_data + offset + j*4;

							//reset the values
							local_buff[0] = 0;//R
							local_buff[1] = 0;//G
							local_buff[2] = 0;//B
							local_buff[3] = 255;//A

							current_image_data_offset = i*m_block_width*current_image->getm_channel_count() + j*current_image->getm_channel_count();

							//copy the existing channels of the input image into the rgba destination
							switch(current_image->getm_channel_count())
							{
								//in case of gray image
								case 1:
									local_buff[ 0 ] = current_image_data[ current_image_data_offset ];
									local_buff[ 1 ] = current_image_data[ current_image_data_offset ];
									local_buff[ 2 ] = current_image_data[ current_image_data_offset ];
									break;

								//in case of 2 channel image
								case 2:
									local_buff[ 0 ] = current_image_data[ current_image_data_offset ];
									local_buff[ 1 ] = current_image_data[ current_image_data_offset +1 ];
									local_buff[ 2 ] = current_image_data[ current_image_data_offset +1 ];
									break;

								//in case of 3 channel image
								case 3:
									local_buff[ 0 ] = current_image_data[ current_image_data_offset ];
									local_buff[ 1 ] = current_image_data[ current_image_data_offset +1 ];
									local_buff[ 2 ] = current_image_data[ current_image_data_offset +2 ];
									break;

								/*note that the 4 channel case is handled in the case of
								'row_size == row_size_current_image'
								*/
								default:
									break;
							}
						}
					}
				}
			}
		}

		return mp_rendered_image;
	}


private:
	KTuple< Image<T>* >* mp_images;
	unsigned int m_image_count_x;
	unsigned int m_image_count_y;
	unsigned int m_block_width;
	unsigned int m_block_height;
	Image<T>* mp_rendered_image;//always in RGBA format
};

} /* namespace Lazarus */
#endif /* IMAGEGRID_H_ */
