/*
 * Image.h
 *
 *  Created on: 04.02.2013
 *      Author: Darius Malysiak
 */

#ifndef IMAGE_H_
#define IMAGE_H_

#include "../Serializable.h"
#include "KTuple.h"
#include "FastKTuple.h"
#include "Matrix.h"
#include "../PrimitiveBuffer.h"

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <cmath>
#include <typeinfo>

namespace Lazarus
{

/**
 * Helper class for packing 8bit unsigned int containers with pixel informations.
 * */
class ImageToolbox : public BaseObject
{
public:

	ImageToolbox(){}
	~ImageToolbox(){}

	static unsigned int packRGB(unsigned char r,unsigned char g,unsigned char b)
	{
		unsigned int rgb = 0;

		rgb = r;
		rgb = (rgb<<8) | g;
		rgb = (rgb<<8) | b;

		return rgb;
	}

	static unsigned int packRGBA(unsigned char r,unsigned char g,unsigned char b,unsigned char a)
	{
		unsigned int rgb = 0;

		rgb = r;
		rgb = (rgb<<8) | g;
		rgb = (rgb<<8) | b;
		rgb = (rgb<<8) | a;

		return rgb;
	}

	static void unpackRGB(unsigned char& r,unsigned char& g,unsigned char& b, const unsigned int& rgb)
	{
		r = rgb & 0xFF;
		g = (rgb>>8) & 0xFF;
		b = (rgb>>16) & 0xFF;
	}

	static void unpackRGBA(unsigned char& r,unsigned char& g,unsigned char& b,unsigned char& a, const unsigned int& rgba)
	{
		r = rgba & 0xFF;
		g = (rgba>>8) & 0xFF;
		b = (rgba>>16) & 0xFF;
		a = (rgba>>24) & 0xFF;
	}


};

template<typename W, typename U> class ImageConverter;

#define IDX2IMAGEELEMENT(x,y,channels,width) (  ((( (width) * (channels) ) * (y) ) + ( (channels) * (x) )) )

/**
 * All pixel information is stored in a row major interleaved format, i.e. a sequence
 * (c0 c1 ... cN)_0,..., (c0 c1 ... cN)_M
 * for each pixels and N+1 channels, all M+1 pixels in each image line are stored like that
 * in row-major format within mp_image_data.
 * The 4th or 2nd channel shall always contain the alpha value, furthermore RGB is the canonical alignment.
 * */

template<typename T>
class Image: public Serializable {
public:

	template<typename W, typename U>
	friend class Lazarus::ImageConverter;

	enum IMAGE_DATA_ALIGNMENT{IMAGE_DATA_ALIGNMENT_YPBPR=0,IMAGE_DATA_ALIGNMENT_YCBCR,IMAGE_DATA_ALIGNMENT_HSV,IMAGE_DATA_ALIGNMENT_HSI,IMAGE_DATA_ALIGNMENT_HSL,IMAGE_DATA_ALIGNMENT_RGB,IMAGE_DATA_ALIGNMENT_BGR,IMAGE_DATA_ALIGNMENT_RGBA,
		IMAGE_DATA_ALIGNMENT_GRAY,IMAGE_DATA_ALIGNMENT_BGRA,IMAGE_DATA_ALIGNMENT_GRAYA,IMAGE_DATA_ALIGNMENT_UNSPECIFIED};

	enum IMAGE_SCALING_TYPE{IMAGE_SCALING_TYPE_NEAREST, IMAGE_SCALING_TYPE_BILINEAR};

	/**
	 * Colors which are based on the min and max number of the corresponding type T.
	 * If an alpha channel exists then its value will be set to the maximal numerical value.
	 */
	static inline const FastKTuple<T>& getCOLOR_BLACK_RGB(){ static FastKTuple<T> c; if(c.getm_size()!=3){ c.push( std::numeric_limits<T>::min() );  c.push( std::numeric_limits<T>::min() ); c.push( std::numeric_limits<T>::min() );};return c; }
	static inline const FastKTuple<T>& getCOLOR_WHITE_RGB(){ static FastKTuple<T> c; if(c.getm_size()!=3){ c.push( std::numeric_limits<T>::max() );c.push( std::numeric_limits<T>::max() );c.push( std::numeric_limits<T>::max() ); };return c; }
	static inline const FastKTuple<T>& getCOLOR_RED_RGB(){ static FastKTuple<T> c; if(c.getm_size()!=3){ c.push( std::numeric_limits<T>::max() );c.push( std::numeric_limits<T>::max() );c.push( std::numeric_limits<T>::min() ); };return c; }
	static inline const FastKTuple<T>& getCOLOR_GREEN_RGB(){ static FastKTuple<T> c; if(c.getm_size()!=3){ c.push( std::numeric_limits<T>::min() );c.push( std::numeric_limits<T>::max() );c.push( std::numeric_limits<T>::min() ); };return c; }
	static inline const FastKTuple<T>& getCOLOR_YELLOW_RGB(){ static FastKTuple<T> c; if(c.getm_size()!=3){ c.push( std::numeric_limits<T>::min() );c.push( std::numeric_limits<T>::max() );c.push( std::numeric_limits<T>::max() ); };return c; }
	static inline const FastKTuple<T>& getCOLOR_BLUE_RGB(){ static FastKTuple<T> c; if(c.getm_size()!=3){ c.push( std::numeric_limits<T>::min() );c.push( std::numeric_limits<T>::min() );c.push( std::numeric_limits<T>::max() ); };return c; }

	static inline const FastKTuple<T>& getCOLOR_BLACK_RGBA(){ static FastKTuple<T> c; if(c.getm_size()!=4){ c.push( std::numeric_limits<T>::min() );c.push( std::numeric_limits<T>::min() );c.push( std::numeric_limits<T>::min() );c.push( std::numeric_limits<T>::max() ); };return c; }
	static inline const FastKTuple<T>& getCOLOR_WHITE_RGBA(){ static FastKTuple<T> c; if(c.getm_size()!=4){ c.push( std::numeric_limits<T>::max() );c.push( std::numeric_limits<T>::max() );c.push( std::numeric_limits<T>::max() );c.push( std::numeric_limits<T>::max() ); };return c; }
	static inline const FastKTuple<T>& getCOLOR_RED_RGBA(){ static FastKTuple<T> c; if(c.getm_size()!=4){ c.push( std::numeric_limits<T>::max() );c.push( std::numeric_limits<T>::min() );c.push( std::numeric_limits<T>::min() );c.push( std::numeric_limits<T>::max() ); };return c; }
	static inline const FastKTuple<T>& getCOLOR_GREEN_RGBA(){ static FastKTuple<T> c; if(c.getm_size()!=4){ c.push( std::numeric_limits<T>::min() );c.push( std::numeric_limits<T>::max() );c.push( std::numeric_limits<T>::min() );c.push( std::numeric_limits<T>::max() ); };return c; }
	static inline const FastKTuple<T>& getCOLOR_YELLOW_RGBA(){ static FastKTuple<T> c; if(c.getm_size()!=4){c.push( std::numeric_limits<T>::min() );c.push( std::numeric_limits<T>::max() );c.push( std::numeric_limits<T>::max() ); c.push( std::numeric_limits<T>::max() ); };return c; }
	static inline const FastKTuple<T>& getCOLOR_BLUE_RGBA(){ static FastKTuple<T> c; if(c.getm_size()!=4){c.push( std::numeric_limits<T>::min() );c.push( std::numeric_limits<T>::min() );c.push( std::numeric_limits<T>::max() ); c.push( std::numeric_limits<T>::max() ); };return c; }

	static inline const FastKTuple<T>& getCOLOR_BLACK_GRAY(){ static FastKTuple<T> c; if(c.getm_size()==0){ c.push( std::numeric_limits<T>::min() ); }; return c; }
	static inline const FastKTuple<T>& getCOLOR_WHITE_GRAY(){ static FastKTuple<T> c; if(c.getm_size()==0){ c.push( std::numeric_limits<T>::max() ); }; return c;}

	/**
	 * By default the channel limits will be constructed according to the data type.
	 * */
	Image() {

		m_width = 0;
		m_height = 0;
		m_channel_count = 0;
		mp_image_data = NULL;
		m_data_alignment = IMAGE_DATA_ALIGNMENT_RGB;
		pthread_rwlock_init(&m_rw_lock,NULL);
	}

	Image(const Image<T>& image)
	{
		m_width = image.getm_width();
		m_height = image.getm_height();
		m_channel_count = image.getm_channel_count();
		m_data_alignment = image.getm_data_alignment();

		//deep copy
		mp_image_data = new T[m_width*m_height*m_channel_count];
		memcpy(mp_image_data,image.getmp_image_data(),m_width*m_height*m_channel_count*sizeof(T));
		pthread_rwlock_init(&m_rw_lock,NULL);

	}

	Image(Image<T>* image)
	{
		m_width = image->getm_width();
		m_height = image->getm_height();
		m_channel_count = image->getm_channel_count();
		m_data_alignment = image->getm_data_alignment();

		//deep copy
		mp_image_data = new T[m_width*m_height*m_channel_count];
		memcpy(mp_image_data,image->getmp_image_data(),m_width*m_height*m_channel_count*sizeof(T));
		pthread_rwlock_init(&m_rw_lock,NULL);

	}

	Image(unsigned int width, unsigned int height, enum IMAGE_DATA_ALIGNMENT data_alignment=IMAGE_DATA_ALIGNMENT_RGB)
	{
		m_width = width;
		m_height = height;

		switch(data_alignment)
		{
			case IMAGE_DATA_ALIGNMENT_RGB:
				m_channel_count = 3;
				break;

			case IMAGE_DATA_ALIGNMENT_BGR:
				m_channel_count = 3;
				break;

			case IMAGE_DATA_ALIGNMENT_YCBCR:
				m_channel_count = 3;
				break;

			case IMAGE_DATA_ALIGNMENT_YPBPR:
				m_channel_count = 3;
				break;

			case IMAGE_DATA_ALIGNMENT_HSI:
				m_channel_count = 3;
				break;

			case IMAGE_DATA_ALIGNMENT_HSV:
				m_channel_count = 3;
				break;

			case IMAGE_DATA_ALIGNMENT_HSL:
				m_channel_count = 3;
				break;

			case IMAGE_DATA_ALIGNMENT_RGBA:
				m_channel_count = 4;
				break;

			case IMAGE_DATA_ALIGNMENT_BGRA:
				m_channel_count = 4;
				break;

			case IMAGE_DATA_ALIGNMENT_GRAY:
				m_channel_count = 1;
				break;

			case IMAGE_DATA_ALIGNMENT_GRAYA:
				m_channel_count = 2;
				break;

			default:
				break;
		}

		mp_image_data = new T[width*height*m_channel_count];
		m_data_alignment = data_alignment;
		pthread_rwlock_init(&m_rw_lock,NULL);
	}

	Image(T* image_data, unsigned int width, unsigned int height, enum IMAGE_DATA_ALIGNMENT data_alignment, bool deep_copy = true)
	{
		m_width = width;
		m_height = height;

		switch(data_alignment)
		{
			case IMAGE_DATA_ALIGNMENT_RGB:
				m_channel_count = 3;
				break;

			case IMAGE_DATA_ALIGNMENT_BGR:
				m_channel_count = 3;
				break;

			case IMAGE_DATA_ALIGNMENT_YCBCR:
				m_channel_count = 3;
				break;

			case IMAGE_DATA_ALIGNMENT_YPBPR:
				m_channel_count = 3;
				break;

			case IMAGE_DATA_ALIGNMENT_HSI:
				m_channel_count = 3;
				break;

			case IMAGE_DATA_ALIGNMENT_HSV:
				m_channel_count = 3;
				break;

			case IMAGE_DATA_ALIGNMENT_HSL:
				m_channel_count = 3;
				break;

			case IMAGE_DATA_ALIGNMENT_RGBA:
				m_channel_count = 4;
				break;

			case IMAGE_DATA_ALIGNMENT_BGRA:
				m_channel_count = 4;
				break;

			case IMAGE_DATA_ALIGNMENT_GRAY:
				m_channel_count = 1;
				break;

			case IMAGE_DATA_ALIGNMENT_GRAYA:
				m_channel_count = 2;
				break;

			default:
				break;
		}

		if(deep_copy == true)
		{
			mp_image_data = new T[width*height*m_channel_count];
			memcpy(mp_image_data, image_data, width*height*m_channel_count*sizeof(T));
		}
		else
		{
			mp_image_data = image_data;
		}

		m_data_alignment = data_alignment;
		pthread_rwlock_init(&m_rw_lock,NULL);

	}

	/**
	 * Will create a gray image, the double values will be casted to the image data type
	 * */
	Image(const Matrix2<double>& image_data)
	{
		m_width = image_data.getColumnCount();
		m_height = image_data.getRowCount();

		m_channel_count = 1;
		m_data_alignment = IMAGE_DATA_ALIGNMENT_GRAY;

		mp_image_data = new T[m_width*m_height*m_channel_count];

		for(unsigned int i=0;i<m_height;++i)
		{
			for(unsigned int j=0;j<m_width;++j)
			{
				mp_image_data[i*m_width + j] = image_data.getData(i,j);
			}
		}

		pthread_rwlock_init(&m_rw_lock,NULL);
	}

	/**
	 * Will create an 3 channel image, the double values will be casted to the image data type.
	 * The data will be copied as follows image_data1(0)|image_data2(0)|image_data3(0)|image_data1(1)|....
	 * */
	Image(const Matrix2<double>& image_data1,const Matrix2<double>& image_data2,const Matrix2<double>& image_data3, enum IMAGE_DATA_ALIGNMENT data_alignment)
	{
		m_width = image_data1.getColumnCount();
		m_height = image_data1.getRowCount();
		m_data_alignment = data_alignment;
		m_channel_count = 3;


		mp_image_data = new T[m_width*m_height*m_channel_count];

		#pragma omp parallel for
		for(unsigned int i=0;i<m_height;++i)
		{
			for(unsigned int j=0;j<m_width;++j)
			{
				mp_image_data[3*i*m_width + 3*j] = image_data1.getData(i,j);
				mp_image_data[3*i*m_width + 3*j+1] = image_data2.getData(i,j);
				mp_image_data[3*i*m_width + 3*j+2] = image_data3.getData(i,j);
			}
		}

		pthread_rwlock_init(&m_rw_lock,NULL);
	}

	/**
	 * Will create an 3 channel image, the double values will be casted to the image data type.
	 * The data will be copied as follows image_data1(0)|image_data2(0)|image_data3(0)|image_data1(1)|....
	 * */
	Image(const Matrix2<unsigned char>& image_data1,const Matrix2<unsigned char>& image_data2,const Matrix2<unsigned char>& image_data3, enum IMAGE_DATA_ALIGNMENT data_alignment)
	{
		m_width = image_data1.getColumnCount();
		m_height = image_data1.getRowCount();
		m_data_alignment = data_alignment;
		m_channel_count = 3;


		mp_image_data = new T[m_width*m_height*m_channel_count];

		#pragma omp parallel for
		for(unsigned int i=0;i<m_height;++i)
		{
			for(unsigned int j=0;j<m_width;++j)
			{
				mp_image_data[3*i*m_width + 3*j] = image_data1.getData(i,j);
				mp_image_data[3*i*m_width + 3*j+1] = image_data2.getData(i,j);
				mp_image_data[3*i*m_width + 3*j+2] = image_data3.getData(i,j);
			}
		}

		pthread_rwlock_init(&m_rw_lock,NULL);
	}

	/**
	 * Will create an 3 channel image, the double values will be casted to the image data type.
	 * The data will be copied as follows image_data1(0)|image_data2(0)|image_data3(0)|image_data1(1)|....
	 * */
	Image(const Matrix2<unsigned int>& image_data1,const Matrix2<unsigned int>& image_data2,const Matrix2<unsigned int>& image_data3, enum IMAGE_DATA_ALIGNMENT data_alignment)
	{
		m_width = image_data1.getColumnCount();
		m_height = image_data1.getRowCount();
		m_data_alignment = data_alignment;
		m_channel_count = 3;


		mp_image_data = new T[m_width*m_height*m_channel_count];

		#pragma omp parallel for
		for(unsigned int i=0;i<m_height;++i)
		{
			for(unsigned int j=0;j<m_width;++j)
			{
				mp_image_data[3*i*m_width + 3*j] = image_data1.getData(i,j);
				mp_image_data[3*i*m_width + 3*j+1] = image_data2.getData(i,j);
				mp_image_data[3*i*m_width + 3*j+2] = image_data3.getData(i,j);
			}
		}

		pthread_rwlock_init(&m_rw_lock,NULL);

	}

	/**
	 * Will create an 3 channel image, the double values will be casted to the image data type.
	 * The data will be copied as follows image_data1(0)|image_data2(0)|image_data3(0)|image_data1(1)|....
	 * */
	Image(const Matrix2<int>& image_data1,const Matrix2<int>& image_data2,const Matrix2<int>& image_data3, enum IMAGE_DATA_ALIGNMENT data_alignment)
	{
		m_width = image_data1.getColumnCount();
		m_height = image_data1.getRowCount();
		m_data_alignment = data_alignment;
		m_channel_count = 3;


		mp_image_data = new T[m_width*m_height*m_channel_count];

		for(unsigned int i=0;i<m_height;++i)
		{
			for(unsigned int j=0;j<m_width;++j)
			{
				mp_image_data[3*i*m_width + 3*j] = image_data1.getData(i,j);
				mp_image_data[3*i*m_width + 3*j+1] = image_data2.getData(i,j);
				mp_image_data[3*i*m_width + 3*j+2] = image_data3.getData(i,j);
			}
		}

		pthread_rwlock_init(&m_rw_lock,NULL);
	}

	/**
	 * Will create an 3 channel image, the double values will be casted to the image data type.
	 * The data will be copied as follows image_data1(0)|image_data2(0)|image_data3(0)|image_data1(1)|....
	 * */
	Image(const Matrix2<char>& image_data1,const Matrix2<char>& image_data2,const Matrix2<char>& image_data3, enum IMAGE_DATA_ALIGNMENT data_alignment)
	{
		m_width = image_data1.getColumnCount();
		m_height = image_data1.getRowCount();
		m_data_alignment = data_alignment;
		m_channel_count = 3;


		mp_image_data = new T[m_width*m_height*m_channel_count];

		for(unsigned int i=0;i<m_height;++i)
		{
			for(unsigned int j=0;j<m_width;++j)
			{
				mp_image_data[3*i*m_width + 3*j] = image_data1.getData(i,j);
				mp_image_data[3*i*m_width + 3*j+1] = image_data2.getData(i,j);
				mp_image_data[3*i*m_width + 3*j+2] = image_data3.getData(i,j);
			}
		}

		pthread_rwlock_init(&m_rw_lock,NULL);
	}

	/**
	 * Will create a gray image, the double values will be casted to the image data type
	 * */
	Image(const Matrix2<unsigned char>& image_data)
	{
		m_width = image_data.getColumnCount();
		m_height = image_data.getRowCount();

		m_channel_count = 1;
		m_data_alignment = IMAGE_DATA_ALIGNMENT_GRAY;

		mp_image_data = new T[m_width*m_height*m_channel_count];

		for(unsigned int i=0;i<m_height;++i)
		{
			for(unsigned int j=0;j<m_width;++j)
			{
				mp_image_data[i*m_width + j] = image_data.getData(i,j);
			}
		}

		pthread_rwlock_init(&m_rw_lock,NULL);
	}


	/**
	 * Will create a gray image, the double values will be casted to the image data type
	 * */
	Image(const Matrix2<unsigned int>& image_data)
	{
		m_width = image_data.getColumnCount();
		m_height = image_data.getRowCount();

		m_channel_count = 1;
		m_data_alignment = IMAGE_DATA_ALIGNMENT_GRAY;

		mp_image_data = new T[m_width*m_height*m_channel_count];

		for(unsigned int i=0;i<m_height;++i)
		{
			for(unsigned int j=0;j<m_width;++j)
			{
				mp_image_data[i*m_width + j] = image_data.getData(i,j);
			}
		}

		pthread_rwlock_init(&m_rw_lock,NULL);
	}

	/**
	 * Will create a gray image, the double values will be casted to the image data type
	 * */
	Image(const Matrix2<int>& image_data)
	{
		m_width = image_data.getColumnCount();
		m_height = image_data.getRowCount();

		m_channel_count = 1;
		m_data_alignment = IMAGE_DATA_ALIGNMENT_GRAY;

		mp_image_data = new T[m_width*m_height*m_channel_count];

		for(unsigned int i=0;i<m_height;++i)
		{
			for(unsigned int j=0;j<m_width;++j)
			{
				mp_image_data[i*m_width + j] = image_data.getData(i,j);
			}
		}

		pthread_rwlock_init(&m_rw_lock,NULL);
	}

	/**
	 * Will create a gray image, the double values will be casted to the image data type
	 * */
	Image(const Matrix2<char>& image_data)
	{
		m_width = image_data.getColumnCount();
		m_height = image_data.getRowCount();

		m_channel_count = 1;
		m_data_alignment = IMAGE_DATA_ALIGNMENT_GRAY;

		mp_image_data = new T[m_width*m_height*m_channel_count];

		for(unsigned int i=0;i<m_height;++i)
		{
			for(unsigned int j=0;j<m_width;++j)
			{
				mp_image_data[i*m_width + j] = image_data.getData(i,j);
			}
		}

		pthread_rwlock_init(&m_rw_lock,NULL);
	}

	/**
	 * This method will convert the image to the given format. The only requirement: the provided image has at least the same channel count as data_alignment.
	 * Channels from the input image will be copied 1:1, e.g. BGR -> RGB which represents a swap.
	 * Should 'image' have more channels than 'data_alignment' a gray image will be created from the first channel.
	 * The only exception for the rule is the case in which the new image's last channel is an alpha channel with a total count of 4 output channels.
	 * In this case the new image's alpha channel will be set to 255.
	 * */
	Image(Image<T>* image, enum IMAGE_DATA_ALIGNMENT data_alignment)
	{
		m_width = image->getm_width();
		m_height = image->getm_height();
		m_data_alignment = data_alignment;

		pthread_rwlock_init(&m_rw_lock,NULL);

		//copy the data
		bool exception = false;
		switch(data_alignment)
		{
			case IMAGE_DATA_ALIGNMENT_RGB:
				m_channel_count = 3;
				break;

			case IMAGE_DATA_ALIGNMENT_BGR:
				m_channel_count = 3;
				break;

			case IMAGE_DATA_ALIGNMENT_YCBCR:
				m_channel_count = 3;
				break;

			case IMAGE_DATA_ALIGNMENT_YPBPR:
				m_channel_count = 3;
				break;

			case IMAGE_DATA_ALIGNMENT_HSI:
				m_channel_count = 3;
				break;

			case IMAGE_DATA_ALIGNMENT_HSV:
				m_channel_count = 3;
				break;

			case IMAGE_DATA_ALIGNMENT_HSL:
				m_channel_count = 3;
				break;

			case IMAGE_DATA_ALIGNMENT_RGBA:
				m_channel_count = 4;
				exception = true;
				break;

			case IMAGE_DATA_ALIGNMENT_BGRA:
				m_channel_count = 4;
				exception = true;
				break;

			case IMAGE_DATA_ALIGNMENT_GRAY:
				m_channel_count = 1;
				break;

			case IMAGE_DATA_ALIGNMENT_GRAYA:
				m_channel_count = 2;
				break;

			default:
				break;
		}


		//sanity check
		if(image->getm_channel_count() < m_channel_count && exception == false)//forced reduction
		{
			m_channel_count = 1;//this will only copy the first channel from the input image

			mp_image_data = new T[m_width*m_height*m_channel_count];

			FastKTuple<T> in_color(image->getm_channel_count());
			FastKTuple<T> out_color(m_channel_count);
			for(unsigned int i=0;i<m_width;++i)
			{
				for(unsigned int j=0;j<m_height;++j)
				{
					image->getPixelFast(&in_color,i,j);
					for(unsigned int k=0;k<m_channel_count;++k)
					{
						out_color[k] = in_color[k];
					}
					this->setPixelFast(&out_color,i,j);
				}
			}
		}
		else if(exception == false)//channel reduction
		{
			mp_image_data = new T[m_width*m_height*m_channel_count];

			FastKTuple<T> in_color(image->getm_channel_count());
			FastKTuple<T> out_color(m_channel_count);
			for(unsigned int i=0;i<m_width;++i)
			{
				for(unsigned int j=0;j<m_height;++j)
				{
					image->getPixelFast(&in_color,i,j);
					for(unsigned int k=0;k<m_channel_count;++k)
					{
						out_color[k] = in_color[k];
					}
					this->setPixelFast(&out_color,i,j);
				}
			}
		}
		else if(exception == true)//exception with 4 output channels and the last one being the alpha channel
		{
			mp_image_data = new T[m_width*m_height*m_channel_count];

			FastKTuple<T> in_color(image->getm_channel_count());
			FastKTuple<T> out_color(m_channel_count);
			for(unsigned int i=0;i<m_width;++i)
			{
				for(unsigned int j=0;j<m_height;++j)
				{
					image->getPixelFast(&in_color,i,j);
					for(unsigned int k=0;k<image->getm_channel_count();++k)//the input has less channels!!
					{
						out_color[k] = in_color[k];
					}
					out_color[3] = 255;
					this->setPixelFast(&out_color,i,j);
				}
			}
		}

	}

	/**
	 * This method will convert the image to the given format. The only requirement: the provided image has at least the same channel count as data_alignment.
	 * Channels from the input image will be copied 1:1, e.g. BGR -> RGB which represents a swap.
	 * Should 'image' have more channels than 'data_alignment' a gray image will be created from the first channel.
	 * The only exception for the rule is the case in which the new image's last channel is an alpha channel with a total count of 4 output channels.
	 * In this case the new image's alpha channel will be set to 255.
	 * */
	Image(const Image<T>& image, enum IMAGE_DATA_ALIGNMENT data_alignment)
	{
		m_width = image.getm_width();
		m_height = image.getm_height();
		m_data_alignment = data_alignment;

		pthread_rwlock_init(&m_rw_lock,NULL);

		//copy the data
		bool exception = false;
		switch(data_alignment)
		{
			case IMAGE_DATA_ALIGNMENT_RGB:
				m_channel_count = 3;
				break;

			case IMAGE_DATA_ALIGNMENT_BGR:
				m_channel_count = 3;
				break;

			case IMAGE_DATA_ALIGNMENT_YCBCR:
				m_channel_count = 3;
				break;

			case IMAGE_DATA_ALIGNMENT_YPBPR:
				m_channel_count = 3;
				break;

			case IMAGE_DATA_ALIGNMENT_HSI:
				m_channel_count = 3;
				break;

			case IMAGE_DATA_ALIGNMENT_HSV:
				m_channel_count = 3;
				break;

			case IMAGE_DATA_ALIGNMENT_HSL:
				m_channel_count = 3;
				break;

			case IMAGE_DATA_ALIGNMENT_RGBA:
				m_channel_count = 4;
				exception = true;
				break;

			case IMAGE_DATA_ALIGNMENT_BGRA:
				m_channel_count = 4;
				exception = true;
				break;

			case IMAGE_DATA_ALIGNMENT_GRAY:
				m_channel_count = 1;
				break;

			case IMAGE_DATA_ALIGNMENT_GRAYA:
				m_channel_count = 2;
				break;

			default:
				break;
		}


		//sanity check
		if(image.getm_channel_count() < m_channel_count && exception == false)//forced reduction
		{
			m_channel_count = 1;//this will only copy the first channel from the input image

			mp_image_data = new T[m_width*m_height*m_channel_count];

			FastKTuple<T> in_color(image.getm_channel_count());
			FastKTuple<T> out_color(m_channel_count);
			for(unsigned int i=0;i<m_width;++i)
			{
				for(unsigned int j=0;j<m_height;++j)
				{
					image.getPixelFast(&in_color,i,j);
					for(unsigned int k=0;k<m_channel_count;++k)
					{
						out_color[k] = in_color[k];
					}
					this->setPixelFast(&out_color,i,j);
				}
			}
		}
		else if(exception == false)//channel reduction
		{
			mp_image_data = new T[m_width*m_height*m_channel_count];

			FastKTuple<T> in_color(image.getm_channel_count());
			FastKTuple<T> out_color(m_channel_count);
			for(unsigned int i=0;i<m_width;++i)
			{
				for(unsigned int j=0;j<m_height;++j)
				{
					image.getPixelFast(&in_color,i,j);
					for(unsigned int k=0;k<m_channel_count;++k)
					{
						out_color[k] = in_color[k];
					}
					this->setPixelFast(&out_color,i,j);
				}
			}
		}
		else if(exception == true)//exception with 4 output channels and the last one being the alpha channel
		{
			mp_image_data = new T[m_width*m_height*m_channel_count];

			FastKTuple<T> in_color(image.getm_channel_count());
			FastKTuple<T> out_color(m_channel_count);
			for(unsigned int i=0;i<m_width;++i)
			{
				for(unsigned int j=0;j<m_height;++j)
				{
					image.getPixelFast(&in_color,i,j);
					for(unsigned int k=0;k<image.getm_channel_count();++k)//the input has less channels!!
					{
						out_color[k] = in_color[k];
					}
					out_color[3] = 255;
					this->setPixelFast(&out_color,i,j);
				}
			}
		}

	}

	~Image()
	{

		DELETE_ARRAY_NULL_CHECKING(mp_image_data);
		pthread_rwlock_destroy(&m_rw_lock);

	}

	Image& operator=( const Image& image )
	{
		DELETE_ARRAY_NULL_CHECKING(mp_image_data);
		pthread_rwlock_destroy(&m_rw_lock);

		m_width = image->getm_width();
		m_height = image->getm_height();
		m_channel_count = image->getm_channel_count();
		m_data_alignment = image->getm_data_alignment();

		//deep copy
		mp_image_data = new T[m_width*m_height*m_channel_count];
		memcpy(mp_image_data,image.getmp_image_data(),m_width*m_height*m_channel_count*sizeof(T));
		pthread_rwlock_init(&m_rw_lock,NULL);

		return *this;
	}

	virtual void serialize()
	{
		Serializable::resetBuffer();

		registerElement<unsigned int>(3);
		registerUCharA(sizeof(T)*m_width*m_height*m_channel_count);
		registerUCharA(sizeof(IMAGE_DATA_ALIGNMENT));
		allocateBuffer();

		//serialize the attributes
		this->addUCharA((unsigned char*)mp_image_data,sizeof(T)*m_width*m_height*m_channel_count);
		this->addUCharA((unsigned char*)&m_data_alignment,sizeof(IMAGE_DATA_ALIGNMENT));
		this->addUInt(m_width);
		this->addUInt(m_height);
		this->addUInt(m_channel_count);

	}

	virtual void deserialize()
	{
		m_channel_count = this->getUInt();
		m_height = this->getUInt();
		m_width = this->getUInt();

		//data alignment
		unsigned long long size=0;
		unsigned char* temp = this->getUCharA(size);
		memcpy(&m_data_alignment,temp,size);
		delete temp;

		//image data
		temp = this->getUCharA(size);
		DELETE_NULL_CHECKING(mp_image_data);
		mp_image_data = new T[m_width*m_height*m_channel_count];
		memcpy(mp_image_data,temp,size);
		delete temp;

		resetBuffer();
	}

	/**
	 * Returns the channel count.
	 * */
	inline unsigned int getm_channel_count() const
	{
		return m_channel_count;
	}

	/**
	 * Returns the channel count.
	 * */
	inline unsigned int channels() const
	{
		return m_channel_count;
	}


	/**
	 * Returns an image channel within a matrix of size height * width.
	 * */
	Matrix2<T>* getChannelMatrix(unsigned int channel) const
	{
		Matrix2<T>* out = new Matrix2<T>();
		out->initMatrix(m_height,m_width);

		for(unsigned int i=0;i<m_height;++i)
		{
			for(unsigned int j=0;j<m_width;++j)
			{
				out->setData(i,j,mp_image_data[ m_width*m_channel_count*i + m_channel_count*j + channel ]);
			}
		}

		return out;
	}

	/**
	 * Returns an image channel within a matrix of size height * width.
	 * */
	Matrix2<double>* getChannelMatrixDouble(unsigned int channel) const
	{
		Matrix2<double>* out = new Matrix2<double>();
		out->initMatrix(m_height,m_width);

		for(unsigned int i=0;i<m_height;++i)
		{
			for(unsigned int j=0;j<m_width;++j)
			{
				out->setData(i,j,mp_image_data[ m_width*m_channel_count*i + m_channel_count*j + channel ]);
			}
		}

		return out;
	}

	/**
	 * Scales the current image, absolute measure/size.
	 * */
	void scaleImageAbs(unsigned int width, unsigned int height, IMAGE_SCALING_TYPE type = IMAGE_SCALING_TYPE_NEAREST)
	{
		/*bool x_upscaling = false;//indicates upscaling along the x-axis
		bool y_upscaling = false;//indicates upscaling along the y-axis

		if(width > m_width)
		{
			x_upscaling = true;
		}

		if(height > m_height)
		{
			y_upscaling = true;
		}*/

		pthread_rwlock_wrlock(&m_rw_lock);

		//scaling factors
		long double x_factor = (long double)m_width / (long double)width;// <1 for upscaling, >1 for downscaling
		long double y_factor = (long double)m_height / (long double)height;// <1 for upscaling, >1 for downscaling
		long double distance_x = std::ceil(x_factor);
		long double distance_y = std::ceil(y_factor);

		//container for rescaled image
		T* data = new T[width*height*m_channel_count];

		//interpolation switch
		switch(type)
		{
		case IMAGE_SCALING_TYPE_NEAREST:
			for(unsigned int i=0;i<height;++i)
			{
				for(unsigned int j=0;j<width;++j)
				{
					for(unsigned int k = 0;k<m_channel_count;k++)
					{
						data[ width*m_channel_count*i + m_channel_count*j + k ] = mp_image_data[(unsigned int)( m_width*m_channel_count*std::floor(y_factor*(long double)i) + m_channel_count*std::floor(x_factor*(long double)j) + k )  ];
					}
				}
			}
			break;

		case IMAGE_SCALING_TYPE_BILINEAR:
			for(unsigned int i=0;i<height;++i)
			{
				for(unsigned int j=0;j<width;++j)
				{
					for(unsigned int k = 0;k<m_channel_count;k++)
					{
						//x
						long double Q1 = (long double)mp_image_data[(unsigned int)( m_width*m_channel_count*std::floor(y_factor*(long double)i) + m_channel_count*std::floor(x_factor*(long double)j) + k ) ];
						long double Q2 = (long double)mp_image_data[(unsigned int)( m_width*m_channel_count*std::floor(y_factor*(long double)i) + m_channel_count*std::ceil(x_factor*(long double)j) + k )];
						//y
						long double Q3 = (long double)mp_image_data[(unsigned int)( m_width*m_channel_count*std::ceil(y_factor*(long double)i) + m_channel_count*std::floor(x_factor*(long double)j) + k )];
						long double Q4 = (long double)mp_image_data[(unsigned int)( m_width*m_channel_count*std::ceil(y_factor*(long double)i) + m_channel_count*std::ceil(x_factor*(long double)j) + k )];


						long double inter_x1 =  Q1 + (Q2 - Q1)*((x_factor*j)-std::floor(x_factor*j))/distance_x ;
						long double inter_x2 = Q3 + (Q4 - Q3)*((x_factor*j)-std::floor(x_factor*j))/distance_x ;

						T inter = (T)( inter_x1 + (inter_x2 - inter_x1)*((y_factor*i)-std::floor(y_factor*i))/distance_y  ) ;

						data[ width*m_channel_count*i + m_channel_count*j + k ] = inter;
					}
				}
			}
			break;
		}

		DELETE_NULL_CHECKING(mp_image_data);
		mp_image_data = data;
		m_width = width;
		m_height = height;

		pthread_rwlock_unlock(&m_rw_lock);

	}

	/**
	 * returns a scaled version of the current image, absolute measure/size.
	 * */
	Image<T>* getScaledImageAbs(unsigned int width, unsigned int height, IMAGE_SCALING_TYPE type = IMAGE_SCALING_TYPE_NEAREST)
	{
		/*bool x_upscaling = false;//indicates upscaling along the x-axis
		bool y_upscaling = false;//indicates upscaling along the y-axis

		if(width > m_width)
		{
			x_upscaling = true;
		}

		if(height > m_height)
		{
			y_upscaling = true;
		}*/

		pthread_rwlock_wrlock(&m_rw_lock);

		//scaling factors
		long double x_factor = (long double)m_width / (long double)width;// <1 for upscaling, >1 for downscaling
		long double y_factor = (long double)m_height / (long double)height;// <1 for upscaling, >1 for downscaling
		long double distance_x = std::ceil(x_factor);
		long double distance_y = std::ceil(y_factor);

		//container for rescaled image
		T* data = new T[width*height*m_channel_count];

		//interpolation switch
		switch(type)
		{
		case IMAGE_SCALING_TYPE_NEAREST:
			for(unsigned int i=0;i<height;++i)
			{
				for(unsigned int j=0;j<width;++j)
				{
					for(unsigned int k = 0;k<m_channel_count;k++)
					{
						data[ width*m_channel_count*i + m_channel_count*j + k ] = mp_image_data[(unsigned int)( m_width*m_channel_count*std::floor(y_factor*(long double)i) + m_channel_count*std::floor(x_factor*(long double)j) + k  ) ];
					}
				}
			}
			break;

		case IMAGE_SCALING_TYPE_BILINEAR:
			for(unsigned int i=0;i<height;++i)
			{
				for(unsigned int j=0;j<width;++j)
				{
					for(unsigned int k = 0;k<m_channel_count;k++)
					{
						//x
						long double Q1 = (long double)mp_image_data[(unsigned int)( m_width*m_channel_count*std::floor(y_factor*(long double)i) + m_channel_count*std::floor(x_factor*(long double)j) + k ) ];
						long double Q2 = (long double)mp_image_data[(unsigned int)( m_width*m_channel_count*std::floor(y_factor*(long double)i) + m_channel_count*std::ceil(x_factor*(long double)j) + k )];
						//y
						long double Q3 = (long double)mp_image_data[(unsigned int)( m_width*m_channel_count*std::ceil(y_factor*(long double)i) + m_channel_count*std::floor(x_factor*(long double)j) + k )];
						long double Q4 = (long double)mp_image_data[(unsigned int)( m_width*m_channel_count*std::ceil(y_factor*(long double)i) + m_channel_count*std::ceil(x_factor*(long double)j) + k )];


						long double inter_x1 =  Q1 + (Q2 - Q1)*((x_factor*j)-std::floor(x_factor*j))/distance_x ;
						long double inter_x2 = Q3 + (Q4 - Q3)*((x_factor*j)-std::floor(x_factor*j))/distance_x ;

						T inter = (T)( inter_x1 + (inter_x2 - inter_x1)*((y_factor*i)-std::floor(y_factor*i))/distance_y  ) ;

						data[ width*m_channel_count*i + m_channel_count*j + k ] = inter;
					}
				}
			}
			break;
		}

		pthread_rwlock_unlock(&m_rw_lock);

		return new Image<T>(data, width, height, m_data_alignment, false);

	}

	/**
	 * Scales the current image, absolute measure/size.
	 * */
	void scaleImageAbsFast(unsigned int width, unsigned int height, IMAGE_SCALING_TYPE type = IMAGE_SCALING_TYPE_NEAREST)
	{
		/*bool x_upscaling = false;//indicates upscaling along the x-axis
		bool y_upscaling = false;//indicates upscaling along the y-axis

		if(width > m_width)
		{
			x_upscaling = true;
		}

		if(height > m_height)
		{
			y_upscaling = true;
		}*/

		//scaling factors
		long double x_factor = (long double)m_width / (long double)width;// <1 for upscaling, >1 for downscaling
		long double y_factor = (long double)m_height / (long double)height;// <1 for upscaling, >1 for downscaling

		long double distance_x = std::ceil(x_factor);
		long double distance_y = std::ceil(y_factor);

		//container for rescaled image
		T* data = new T[width*height*m_channel_count];

		//interpolation switch
		switch(type)
		{
		case IMAGE_SCALING_TYPE_NEAREST:
			for(unsigned int i=0;i<height;++i)
			{
				for(unsigned int j=0;j<width;++j)
				{
					for(unsigned int k = 0;k<m_channel_count;k++)
					{
						data[ width*m_channel_count*i + m_channel_count*j + k ] = mp_image_data[(unsigned int)( m_width*m_channel_count*std::floor(y_factor*(long double)i) + m_channel_count*std::floor(x_factor*(long double)j) + k )  ];
					}
				}
			}
			break;

		case IMAGE_SCALING_TYPE_BILINEAR:
			for(unsigned int i=0;i<height;++i)
			{
				for(unsigned int j=0;j<width;++j)
				{
					for(unsigned int k = 0;k<m_channel_count;k++)
					{
						//x
						long double Q1 = (long double)mp_image_data[(unsigned int)( m_width*m_channel_count*std::floor(y_factor*(long double)i) + m_channel_count*std::floor(x_factor*(long double)j) + k ) ];
						long double Q2 = (long double)mp_image_data[(unsigned int)( m_width*m_channel_count*std::floor(y_factor*(long double)i) + m_channel_count*std::ceil(x_factor*(long double)j) + k )];
						//y
						long double Q3 = (long double)mp_image_data[(unsigned int)( m_width*m_channel_count*std::ceil(y_factor*(long double)i) + m_channel_count*std::floor(x_factor*(long double)j) + k )];
						long double Q4 = (long double)mp_image_data[(unsigned int)( m_width*m_channel_count*std::ceil(y_factor*(long double)i) + m_channel_count*std::ceil(x_factor*(long double)j) + k )];


						long double inter_x1 =  Q1 + (Q2 - Q1)*((x_factor*j)-std::floor(x_factor*j))/distance_x ;
						long double inter_x2 = Q3 + (Q4 - Q3)*((x_factor*j)-std::floor(x_factor*j))/distance_x ;

						T inter = (T)( inter_x1 + (inter_x2 - inter_x1)*((y_factor*i)-std::floor(y_factor*i))/distance_y  ) ;

						data[ width*m_channel_count*i + m_channel_count*j + k ] = inter;
					}
				}
			}
			break;
		}

		DELETE_NULL_CHECKING(mp_image_data);
		mp_image_data = data;
		m_width = width;
		m_height = height;

	}

	/**
	 * returns a scaled version of the current image, absolute measure/size.
	 * */
	Image<T>* getScaledImageAbsFast(unsigned int width, unsigned int height, IMAGE_SCALING_TYPE type = IMAGE_SCALING_TYPE_NEAREST)
	{
		/*bool x_upscaling = false;//indicates upscaling along the x-axis
		bool y_upscaling = false;//indicates upscaling along the y-axis

		if(width > m_width)
		{
			x_upscaling = true;
		}

		if(height > m_height)
		{
			y_upscaling = true;
		}*/

		//scaling factors
		long double x_factor = (long double)m_width / (long double)width;// <1 for upscaling, >1 for downscaling
		long double y_factor = (long double)m_height / (long double)height;// <1 for upscaling, >1 for downscaling

		long double distance_x = std::ceil(x_factor);
		long double distance_y = std::ceil(y_factor);

		//container for rescaled image
		T* data = new T[width*height*m_channel_count];

		//interpolation switch
		switch(type)
		{
		case IMAGE_SCALING_TYPE_NEAREST:
			for(unsigned int i=0;i<height;++i)
			{
				for(unsigned int j=0;j<width;++j)
				{
					for(unsigned int k = 0;k<m_channel_count;k++)
					{
						data[ width*m_channel_count*i + m_channel_count*j + k ] = mp_image_data[ (unsigned int)( m_width*m_channel_count*std::floor(y_factor*(long double)i) + m_channel_count*std::floor(x_factor*(long double)j) + k )];
					}
				}
			}
			break;

		case IMAGE_SCALING_TYPE_BILINEAR:
			for(unsigned int i=0;i<height;++i)
			{
				for(unsigned int j=0;j<width;++j)
				{
					for(unsigned int k = 0;k<m_channel_count;k++)
					{
						//x
						long double Q1 = (long double)mp_image_data[(unsigned int)( m_width*m_channel_count*std::floor(y_factor*(long double)i) + m_channel_count*std::floor(x_factor*(long double)j) + k ) ];
						long double Q2 = (long double)mp_image_data[(unsigned int)( m_width*m_channel_count*std::floor(y_factor*(long double)i) + m_channel_count*std::ceil(x_factor*(long double)j) + k )];
						//y
						long double Q3 = (long double)mp_image_data[(unsigned int)( m_width*m_channel_count*std::ceil(y_factor*(long double)i) + m_channel_count*std::floor(x_factor*(long double)j) + k )];
						long double Q4 = (long double)mp_image_data[(unsigned int)( m_width*m_channel_count*std::ceil(y_factor*(long double)i) + m_channel_count*std::ceil(x_factor*(long double)j) + k )];


						long double inter_x1 =  Q1 + (Q2 - Q1)*((x_factor*j)-std::floor(x_factor*j))/distance_x ;
						long double inter_x2 = Q3 + (Q4 - Q3)*((x_factor*j)-std::floor(x_factor*j))/distance_x ;

						//long double inter_x1 =  Q1 + (Q2 - Q1)*x_factor/distance_x ;
						//long double inter_x2 = Q3 + (Q4 - Q3)*x_factor/distance_x ;

						T inter = (T)( inter_x1 + (inter_x2 - inter_x1)*((y_factor*i)-std::floor(y_factor*i))/distance_y  ) ;

						data[ width*m_channel_count*i + m_channel_count*j + k ] = inter;
					}
				}
			}
			break;
		}

		return new Image<T>(data, width, height, m_data_alignment, false);

	}

	/**
	 * Reduces the image to the given proportions.
	 * */
	void trim(unsigned int startx, unsigned int starty, unsigned int width, unsigned int height)
	{
		//sanity check
		if( startx+width >  m_width || starty+height > m_height )
		{
			printf("ERROR: trimming failed\n");
			return;
		}

		pthread_rwlock_wrlock(&m_rw_lock);

		//get a subimage
		Image<T>* temp = this->getSubImage(startx,starty,width,height);

		//set the new image data
		DELETE_NULL_CHECKING(this->mp_image_data);

		m_width = width;
		m_height = height;

		this->mp_image_data = new T[width*height*m_channel_count];
		memcpy(mp_image_data,temp->getmp_image_data(),sizeof(T)*m_width*m_height*m_channel_count);

		//delete the initial subimage
		delete temp;

		pthread_rwlock_unlock(&m_rw_lock);
	}

	/**
	 * Reduces the image to the given proportions.
	 * */
	void trimFast(unsigned int startx, unsigned int starty, unsigned int width, unsigned int height)
	{
		//sanity check
		if( startx+width >  m_width || starty+height > m_height )
		{
			printf("ERROR: trimming failed\n");
			return;
		}

		//get a subimage
		Image<T>* temp = this->getSubImage(startx,starty,width,height);

		//set the new image data
		DELETE_NULL_CHECKING(this->mp_image_data);

		m_width = width;
		m_height = height;

		this->mp_image_data = new T[width*height*m_channel_count];
		memcpy(mp_image_data,temp->getmp_image_data(),sizeof(T)*m_width*m_height*m_channel_count);

		//delete the initial subimage
		delete temp;
	}

	/**
	 * Transforms the image to a gray image. The new pixel values will be calculated by taking the average number over the channels.
	 * Alpha channels will be ignored.
	 * */
	void createGrayImage()
	{
		pthread_rwlock_wrlock(&m_rw_lock);

		T* mp_image_data_new = new T[m_width*m_height];

		if(m_data_alignment != IMAGE_DATA_ALIGNMENT_GRAYA && m_data_alignment != IMAGE_DATA_ALIGNMENT_RGBA)
		{
			#pragma omp parallel
			{
				T val = T();
				#pragma omp for
				for(unsigned int i=0;i<m_height;++i)
				{
					for(unsigned int j=0;j<m_width;++j)
					{
						for(unsigned int k=0;k<m_channel_count;++k)
						{
							val += mp_image_data[m_channel_count*i*m_width + m_channel_count*j + k];
						}
						mp_image_data_new[i*m_width + j] = (T)( (double)val/(double)m_channel_count );
						val = T();
					}
				}
			}
		}
		else if(m_data_alignment == IMAGE_DATA_ALIGNMENT_GRAYA) //special cases for images with an alpha channel
		{
			#pragma omp parallel
			{
				T val = T();
				#pragma omp for
				for(unsigned int i=0;i<m_height;++i)
				{
					for(unsigned int j=0;j<m_width;++j)
					{
						for(unsigned int k=0;k<1;++k)
						{
							val += mp_image_data[m_channel_count*i*m_width + m_channel_count*j + k];
						}
						mp_image_data_new[i*m_width + j] = (T)( (double)val/(double)m_channel_count );
						val = T();
					}
				}
			}
		}
		else if(m_data_alignment == IMAGE_DATA_ALIGNMENT_RGBA)
		{
			#pragma omp parallel
			{
				T val = T();
				#pragma omp for
				for(unsigned int i=0;i<m_height;++i)
				{
					for(unsigned int j=0;j<m_width;++j)
					{
						for(unsigned int k=0;k<3;++k)
						{
							val += mp_image_data[m_channel_count*i*m_width + m_channel_count*j + k];
						}
						mp_image_data_new[i*m_width + j] = (T)( (double)val/(double)m_channel_count );
						val = T();
					}
				}
			}
		}

		m_channel_count = 1;
		m_data_alignment = IMAGE_DATA_ALIGNMENT_GRAY;

		DELETE_NULL_CHECKING(mp_image_data);
		mp_image_data = mp_image_data_new;

		pthread_rwlock_unlock(&m_rw_lock);
	}

	/**
	 * Transforms the image to a gray image. The new pixel values will be calculated by taking the average number over the channels.
	 * Alpha channels will be ignored.
	 * */
	void createGrayImageFast()
	{

		T* mp_image_data_new = new T[m_width*m_height];

		if(m_data_alignment != IMAGE_DATA_ALIGNMENT_GRAYA && m_data_alignment != IMAGE_DATA_ALIGNMENT_RGBA)
		{
			#pragma omp parallel
			{
				T val = T();
				#pragma omp for
				for(unsigned int i=0;i<m_height;++i)
				{
					for(unsigned int j=0;j<m_width;++j)
					{
						for(unsigned int k=0;k<m_channel_count;++k)
						{
							val += mp_image_data[m_channel_count*i*m_width + m_channel_count*j + k];
						}
						mp_image_data_new[i*m_width + j] = (T)( (double)val/(double)m_channel_count );
						val = T();
					}
				}
			}
		}
		else if(m_data_alignment == IMAGE_DATA_ALIGNMENT_GRAYA) //special cases for images with an alpha channel
		{
			#pragma omp parallel
			{
				T val = T();
				#pragma omp for
				for(unsigned int i=0;i<m_height;++i)
				{
					for(unsigned int j=0;j<m_width;++j)
					{
						for(unsigned int k=0;k<1;++k)
						{
							val += mp_image_data[m_channel_count*i*m_width + m_channel_count*j + k];
						}
						mp_image_data_new[i*m_width + j] = (T)( (double)val/(double)m_channel_count );
						val = T();
					}
				}
			}
		}
		else if(m_data_alignment == IMAGE_DATA_ALIGNMENT_RGBA)
		{
			#pragma omp parallel
			{
				T val = T();
				#pragma omp for
				for(unsigned int i=0;i<m_height;++i)
				{
					for(unsigned int j=0;j<m_width;++j)
					{
						for(unsigned int k=0;k<3;++k)
						{
							val += mp_image_data[m_channel_count*i*m_width + m_channel_count*j + k];
						}
						mp_image_data_new[i*m_width + j] = (T)( (double)val/(double)m_channel_count );
						val = T();
					}
				}
			}
		}

		m_channel_count = 1;
		m_data_alignment = IMAGE_DATA_ALIGNMENT_GRAY;

		DELETE_NULL_CHECKING(mp_image_data);
		mp_image_data = mp_image_data_new;
	}

	/**
	 * Transforms the image to a gray image. The new pixel values will be calculated by taking the maximal number over the channels.
	 * Alpha channels will be ignored.
	 * */
	void createGrayImageMaxFast()
	{

		T* mp_image_data_new = new T[m_width*m_height];
		T val = T();

		if(m_data_alignment != IMAGE_DATA_ALIGNMENT_GRAYA && m_data_alignment != IMAGE_DATA_ALIGNMENT_RGBA)//if not 2 or 4 channels
		{
			for(unsigned int i=0;i<m_height;++i)
			{
				for(unsigned int j=0;j<m_width;++j)
				{
					val = T();
					for(unsigned int k=0;k<m_channel_count;++k)
					{
						if(val < mp_image_data[m_channel_count*i*m_width + m_channel_count*j + k])
							val = mp_image_data[m_channel_count*i*m_width + m_channel_count*j + k];
					}
					mp_image_data_new[i*m_width + j] = val ;

				}
			}
		}
		else if(m_data_alignment == IMAGE_DATA_ALIGNMENT_GRAYA) //special cases for images with an alpha channel  //2 channels
		{
			for(unsigned int i=0;i<m_height;++i)
			{
				for(unsigned int j=0;j<m_width;++j)
				{
					val = T();
					for(unsigned int k=0;k<1;++k)
					{
						if(val < mp_image_data[m_channel_count*i*m_width + m_channel_count*j + k])
							val = mp_image_data[m_channel_count*i*m_width + m_channel_count*j + k];
					}
					mp_image_data_new[i*m_width + j] = val ;
				}
			}
		}
		else if(m_data_alignment == IMAGE_DATA_ALIGNMENT_RGBA)//4 channels
		{
			for(unsigned int i=0;i<m_height;++i)
			{
				for(unsigned int j=0;j<m_width;++j)
				{
					val = T();
					for(unsigned int k=0;k<3;++k)
					{
						if(val < mp_image_data[m_channel_count*i*m_width + m_channel_count*j + k])
							val = mp_image_data[m_channel_count*i*m_width + m_channel_count*j + k];
					}
					mp_image_data_new[i*m_width + j] = val ;
				}
			}
		}

		m_channel_count = 1;
		m_data_alignment = IMAGE_DATA_ALIGNMENT_GRAY;

		DELETE_NULL_CHECKING(mp_image_data);
		mp_image_data = mp_image_data_new;
	}

	/**
	 * Computes the image energy for the given region.
	 * */
	unsigned long long getEnergy(unsigned int start_x, unsigned int start_y, unsigned int width, unsigned int height)
	{
		//check for sanity
		if(start_y+height > m_height || start_x+width > m_width)
		{
			printf("ERROR: region too large\n");
			return 0;
		}

		unsigned long long out = 0;

		for(unsigned int i=start_x;i<width; ++i)
		{
			for(unsigned int j=start_y;j<height; ++j)
			{
				for(unsigned int c = 0; c < m_channel_count ; c++)
				{
					out += ((long long)mp_image_data[ m_width*m_channel_count*j + m_channel_count*i +c ]*
							(long long)mp_image_data[ m_width*m_channel_count*j + m_channel_count*i +c ]);
				}
			}
		}

		return out;
	}

	/**
	 * Computes the delta-image energy for the given region.
	 * */
	unsigned long long getDeltaEnergy(Image<T>* image, unsigned int start_x, unsigned int start_y, unsigned int width, unsigned int height)
	{
		//check for sanity
		if(start_y+height > m_height || start_x+width > m_width)
		{
			printf("ERROR: region too large for base image\n");
			return 0;
		}

		if(start_y+height > image->height() || start_x+width > image->width())
		{
			printf("ERROR: region too large for external image\n");
			return 0;
		}

		unsigned long long out = 0;
		long long delta = 0;

		unsigned int im_channels = image->channels();
		unsigned int im_width = image->width();
		T* im_data = image->getmp_image_data();

		for(unsigned int i=start_x;i<width; ++i)
		{
			for(unsigned int j=start_y;j<height; ++j)
			{
				for(unsigned int c = 0; c < m_channel_count ; c++)
				{
					delta = mp_image_data[ m_width*m_channel_count*j + m_channel_count*i +c ]
						   - im_data[ im_width*im_channels*j + im_channels*i +c ];

					out += delta * delta;
				}
			}
		}

		return out;
	}

	/**
	 * This method works for at most 4 channels.
	 * */
	void setPixel(KTuple<T>* pixel, unsigned int x, unsigned int y)
	{
		pthread_rwlock_wrlock(&m_rw_lock);

		//check for sanity
		if(y >= m_height || x >= m_width|| y<0 || x<0)
		{
			pthread_rwlock_unlock(&m_rw_lock);
			return;
		}

		/*for(unsigned int i = 0;i<m_channel_count;i++)
		{
			mp_image_data[ m_width*m_channel_count*y + m_channel_count*x + i ] = pixel->getElement(i);
		}*/

		long long int offset = m_width*m_channel_count*y + m_channel_count*x;

		switch(m_channel_count)
		{
		case 1:
			mp_image_data[ offset ] = pixel->getElement(0);
			break;

		case 2:
			mp_image_data[ offset ] = pixel->getElement(0);
			mp_image_data[ offset +1] = pixel->getElement(1);
			break;

		case 3:
			mp_image_data[ offset ] = pixel->getElement(0);
			mp_image_data[ offset +1] = pixel->getElement(1);
			mp_image_data[ offset +2] = pixel->getElement(2);
			break;

		case 4:
			mp_image_data[ offset ] = pixel->getElement(0);
			mp_image_data[ offset +1] = pixel->getElement(1);
			mp_image_data[ offset +2] = pixel->getElement(2);
			mp_image_data[ offset +3] = pixel->getElement(3);
			break;
		}

		pthread_rwlock_unlock(&m_rw_lock);

	}

	/**
	 * This method works for at most 4 channels.
	 * */
	inline void setPixelFast(FastKTuple<T>* pixel, unsigned int x, unsigned int y)
	{
		//check for sanity
		if(y >= m_height || x >= m_width || y<0 || x<0)
		{
			return;
		}

		/*for(unsigned int i = 0;i<m_channel_count;i++)
		{
			mp_image_data[ m_width*m_channel_count*y + m_channel_count*x + i ] = pixel->getElement(i);
		}*/

		long long int offset = m_width*m_channel_count*y + m_channel_count*x;

		switch(m_channel_count)
		{
		case 1:
			mp_image_data[ offset ] = pixel->getElement(0);
			break;

		case 2:
			mp_image_data[ offset ] = pixel->getElement(0);
			mp_image_data[ offset +1] = pixel->getElement(1);
			break;

		case 3:
			mp_image_data[ offset ] = pixel->getElement(0);
			mp_image_data[ offset +1] = pixel->getElement(1);
			mp_image_data[ offset +2] = pixel->getElement(2);
			break;

		case 4:
			mp_image_data[ offset ] = pixel->getElement(0);
			mp_image_data[ offset +1] = pixel->getElement(1);
			mp_image_data[ offset +2] = pixel->getElement(2);
			mp_image_data[ offset +3] = pixel->getElement(3);
			break;
		}
	}

	Image<T>* getSubImage(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
	{
		pthread_rwlock_rdlock(&m_rw_lock);

		//check for sanity
		if(y+height > m_height || x+width > m_width)
		{
			pthread_rwlock_unlock(&m_rw_lock);
			printf("ERROR: sub-image too large\n");
			return NULL;
		}

		Image<T>* sub_image = new Image<T>(width,height,m_data_alignment);

		T* sub_data = sub_image->getmp_image_data();

		unsigned int x_padding = x*m_channel_count*sizeof(T);
		unsigned int original_row_size = m_width*m_channel_count*sizeof(T);

		unsigned int rowsize = width*m_channel_count*sizeof(T);
		//unsigned int padding = width*m_channel_count;

		//copy the region from the image
		#pragma omp parallel for
		for(unsigned int i=0;i<height;i++)
		{
			memcpy( sub_data + rowsize*i , mp_image_data + x_padding + original_row_size*(i+y), rowsize );
		}

		/*#pragma omp parallel
		{

			FastKTuple<T> color(this->m_channel_count);

			for(unsigned int i=0;i<width; ++i)
			{
				for(unsigned int j=0;j<height; ++j)
				{
					this->getPixelFast(&color,x+i,y+j);
					sub_image->setPixelFast(&color,i,j);
				}
			}
		}*/

		pthread_rwlock_unlock(&m_rw_lock);

		return sub_image;

	}

	/**
	 * Does not use parallel memcpy calls.
	 * */
	Image<T>* getSubImageFastSeq(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
	{
		//check for sanity
		if(y+height > m_height || x+width > m_width)
		{
			printf("ERROR: sub-image too large\n");
			return NULL;
		}

		Image<T>* sub_image = new Image<T>(width,height,m_data_alignment);

		T* sub_data = sub_image->getmp_image_data();

		unsigned int x_padding = x*m_channel_count*sizeof(T);
		unsigned int original_row_size = m_width*m_channel_count*sizeof(T);

		unsigned int rowsize = width*m_channel_count*sizeof(T);

		//copy the region from the image
		for(unsigned int i=0;i<height;i++)
		{
			memcpy( sub_data + rowsize*i , mp_image_data + x_padding + original_row_size*(i+y), rowsize );
		}

		return sub_image;

	}

	Image<T>* getSubImageFast(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
	{
		//check for sanity
		if(y+height > m_height || x+width > m_width)
		{
			printf("ERROR: sub-image too large\n");
			return NULL;
		}

		Image<T>* sub_image = new Image<T>(width,height,m_data_alignment);

		T* sub_data = sub_image->getmp_image_data();

		unsigned int x_padding = x*m_channel_count*sizeof(T);
		unsigned int original_row_size = m_width*m_channel_count*sizeof(T);

		unsigned int rowsize = width*m_channel_count*sizeof(T);

		//printf("x  %u y %u\n",x,y);
		//printf("maxsource %llu maxtarget %llu \n",m_width*m_channel_count*sizeof(T)*m_height,
		//		height*width*m_channel_count*sizeof(T));

		//copy the region from the image
		#pragma omp parallel for
		for(unsigned int i=0;i<height;i++)
		{
			memcpy( sub_data + rowsize*i , mp_image_data + x_padding + original_row_size*(i+y), rowsize );
		}

		/*#pragma omp parallel
		{
			FastKTuple<T> color(this->m_channel_count);

			for(unsigned int i=0;i<width; ++i)
			{
				for(unsigned int j=0;j<height; ++j)
				{
					this->getPixelFast(&color,x+i,y+j);
					sub_image->setPixelFast(&color,i,j);
				}
			}
		}*/

		return sub_image;

	}

	void insertSubImage(Image<T>* image, unsigned int x, unsigned int y)
	{
		pthread_rwlock_wrlock(&m_rw_lock);

		//check for sanity
		if(y+image->getm_height() >= m_height || x+image->getm_width() >= m_width)
		{
			pthread_rwlock_unlock(&m_rw_lock);
			return;
		}
		if(image->channels() != this->m_channel_count)
		{
			pthread_rwlock_unlock(&m_rw_lock);
			return;
		}

		T* sub_data = image->getmp_image_data();

		unsigned int x_padding = x*m_channel_count*sizeof(T);
		unsigned int y_padding = m_width*m_channel_count*sizeof(T);
		unsigned int image_height = image->getm_height();

		unsigned int memsize = image->getm_width()*m_channel_count*sizeof(T);

		//copy the region to the image
		#pragma omp parallel for
		for(unsigned int i=0;i<image_height;i++)
		{
			memcpy(mp_image_data + x_padding + y_padding*(y+i), sub_data + memsize*i , memsize );
		}

		pthread_rwlock_unlock(&m_rw_lock);

	}

	void insertSubImageFast(Image<T>* image, unsigned int x, unsigned int y)
	{
		//check for sanity
		if(y+image->getm_height() >= m_height || x+image->getm_width() >= m_width)
		{
			return;
		}
		if(image->channels() != this->m_channel_count)
		{
			return;
		}


		/*unsigned int channel_count = image->getm_channel_count();
		unsigned int image_height = image->getm_height();
		unsigned int image_width = image->getm_width();

		//copy the region to the image
		#pragma omp parallel
		{
			Lazarus::FastKTuple<T> color(channel_count);
			for(unsigned int i=0;i<image_height;++i)
			{
				for(unsigned int j=0;j<image_width;++j)
				{
					image->getPixelFast(&color,j,i);
					setPixelFast(&color,x+j,y+i);
				}
			}
		}*/


		T* sub_data = image->getmp_image_data();

		unsigned int x_padding = x*m_channel_count*sizeof(T);
		unsigned int y_padding = y*m_width*m_channel_count*sizeof(T);
		unsigned int row_size = m_width*m_channel_count*sizeof(T);
		unsigned int image_height = image->getm_height();

		unsigned int row_size2 = image->getm_width()*m_channel_count*sizeof(T);

		#pragma omp parallel for
		for(unsigned int i=0;i<image_height;i++)
		{
			memcpy(mp_image_data + x_padding + y_padding + row_size*i, sub_data + row_size2*i , row_size2 );
		}


	}

	/*
	 * converts the image into a grayscale image by copying one channel into all others.
	 */
	void grayifyFast(unsigned int channel=0)
	{
		unsigned int row_size = m_width*m_channel_count;

		for(unsigned int i=0;i<m_height;++i)
		{
			for(unsigned int j=0;j<m_width;++j)
			{
				T value = mp_image_data[i*row_size + j*m_channel_count+channel];

				if(m_channel_count <= 3)
				{
					for(unsigned k=0;k<m_channel_count;++k)
					{
						mp_image_data[i*row_size + j*m_channel_count + k] = value;
					}
				}
				else
				{
					for(unsigned k=0;k<3;++k)
					{
						mp_image_data[i*row_size + j*m_channel_count + k] = value;
					}
				}

			}
		}
	}

	/*
	 * converts the image into a binary image (per channel). Everything below the threshold will become black
	 */
	void binarizeFast(T threshold, T white, T black)
	{
		unsigned int pixelcount = m_height*m_width;

		#pragma omp parallel for
		for(unsigned int i=0;i<pixelcount*m_channel_count;++i)
		{
			if(mp_image_data[i] < threshold)
			{
				mp_image_data[i] = black;
			}
			else
			{
				mp_image_data[i] = white;
			}
		}
	}

	/*
	 * converts the image into a binary image (per channel). Everything below the threshold will become black.
	 */
	Image<T>* getBinaryImageFast(T threshold, T white, T black)
	{
		unsigned int pixelcount = m_height*m_width;
		T* data = new T[m_channel_count*pixelcount];
		memcpy(data,mp_image_data,sizeof(T)*m_channel_count*pixelcount);

		#pragma omp parallel for
		for(unsigned int i=0;i<pixelcount*m_channel_count;++i)
		{
			if(data[i] < threshold)
			{
				data[i] = black;
			}
			else
			{
				data[i] = white;
			}
		}

		Image<T>* out = new Image<T>(data,m_width,m_height,m_data_alignment,false);
		return out;
	}

	/*
	 * pulls the square root of each pixel value (not including an existing alpha channel)
	 */
	void sqrtValueFast()
	{
		unsigned int row_size = m_width*m_channel_count;

		for(unsigned int i=0;i<m_height;++i)
		{
			for(unsigned int j=0;j<m_width;++j)
			{
				if(m_channel_count <= 3)
				{
					for(unsigned k=0;k<m_channel_count;++k)
					{
						mp_image_data[i*row_size + j*m_channel_count + k] = sqrt( mp_image_data[i*row_size + j*m_channel_count + k] );
					}
				}
				else
				{
					for(unsigned k=0;k<3;++k)
					{
						mp_image_data[i*row_size + j*m_channel_count + k] = sqrt( mp_image_data[i*row_size + j*m_channel_count + k] );
					}
				}
			}
		}
	}

	/*
	 * flips the image along the central vertical axis
	 */
	void flipImageHorizontalFast()
	{

		T* new_data = new T[m_height*m_width*m_channel_count];
		unsigned int row_size = m_width*m_channel_count;

		for(unsigned int i=0;i<m_height;++i)
		{
			for(unsigned int j=0, j_=m_width;j<m_width;++j,--j_)
			{
				for(unsigned k=0;k<m_channel_count;++k)
				{
					new_data[i*row_size + j*m_channel_count + k] = mp_image_data[i*row_size + j_*m_channel_count + k];
				}
			}
		}

		DELETE_NULL_CHECKING(mp_image_data);

		mp_image_data = new_data;

	}

	/*
	 * flips the image along the central horizontal axis
	 */
	void flipImageVerticalFast()
	{

		T* new_data = new T[m_height*m_width*m_channel_count];
		unsigned int row_size = m_width*m_channel_count;

		for(unsigned int i=0,i_=m_height;i<m_height;++i,--i_)
		{
			for(unsigned int j=0;j<m_width;++j)
			{
				for(unsigned k=0;k<m_channel_count;++k)
				{
					new_data[i*row_size + j*m_channel_count + k] = mp_image_data[i_*row_size + j*m_channel_count + k];
				}
			}
		}

		DELETE_NULL_CHECKING(mp_image_data);

		mp_image_data = new_data;

	}

	/*
	 * flips the image along the central vertical axis
	 */
	void flipImageHorizontal()
	{
		pthread_rwlock_wrlock(&m_rw_lock);

		T* new_data = new T[m_height*m_width*m_channel_count];
		unsigned int row_size = m_width*m_channel_count;

		for(unsigned int i=0;i<m_height;++i)
		{
			for(unsigned int j=0, j_=m_width;j<m_width;++j,--j_)
			{
				for(unsigned k=0;k<m_channel_count;++k)
				{
					new_data[i*row_size + j*m_channel_count + k] = mp_image_data[i*row_size + j_*m_channel_count + k];
				}
			}
		}

		DELETE_NULL_CHECKING(mp_image_data);

		mp_image_data = new_data;

		pthread_rwlock_unlock(&m_rw_lock);
	}

	/*
	 * flips the image along the central horizontal axis
	 */
	void flipImageVertical()
	{
		pthread_rwlock_wrlock(&m_rw_lock);

		T* new_data = new T[m_height*m_width*m_channel_count];
		unsigned int row_size = m_width*m_channel_count;

		for(unsigned int i=0,i_=m_height;i<m_height;++i,--i_)
		{
			for(unsigned int j=0;j<m_width;++j)
			{
				for(unsigned k=0;k<m_channel_count;++k)
				{
					new_data[i*row_size + j*m_channel_count + k] = mp_image_data[i_*row_size + j*m_channel_count + k];
				}
			}
		}

		DELETE_NULL_CHECKING(mp_image_data);

		mp_image_data = new_data;

		pthread_rwlock_unlock(&m_rw_lock);
	}

	void insertSubImageKeyColor(Image<T>* image, unsigned int x, unsigned int y,T r,T g,T b)
	{
		pthread_rwlock_wrlock(&m_rw_lock);

		//check for sanity
		if(y+image->getm_height() > m_height || x+image->getm_width() > m_width)
		{
			pthread_rwlock_unlock(&m_rw_lock);
			return;
		}

		unsigned int image_height = image->getm_height();
		unsigned int image_width = image->getm_width();

		//copy the region to the image
		Lazarus::FastKTuple<T> color(4);
		T* color_data = color.getmp_data();
		for(unsigned int i=0;i<image_height;i++)
		{
			for(unsigned int j=0;j<image_width;j++)
			{
				image->getPixelFast(&color,j,i);

				//skip key color
				if(color_data[0] == r && color_data[1] == g && color_data[2] == b)
				{
					continue;
				}

				this->setPixelFast(&color,x+j,y+i);
			}
		}

		pthread_rwlock_unlock(&m_rw_lock);

	}

	void printImageInfo()
	{
		printf("Image has size of %u x %u\n",m_width,m_height);

		switch(m_data_alignment)
		{
			case IMAGE_DATA_ALIGNMENT_RGB:
				printf("Data alignment is RGB and thus has %u channels\n",m_channel_count);
				break;

			case IMAGE_DATA_ALIGNMENT_BGR:
				printf("Data alignment is BGR and thus has %u channels\n",m_channel_count);
				break;

			case IMAGE_DATA_ALIGNMENT_YCBCR:
				printf("Data alignment is YCBCR and thus has %u channels\n",m_channel_count);
				break;

			case IMAGE_DATA_ALIGNMENT_YPBPR:
				printf("Data alignment is YPBPR and thus has %u channels\n",m_channel_count);
				break;

			case IMAGE_DATA_ALIGNMENT_HSI:
				printf("Data alignment is HSI and thus has %u channels\n",m_channel_count);
				break;

			case IMAGE_DATA_ALIGNMENT_HSV:
				printf("Data alignment is HSV and thus has %u channels\n",m_channel_count);
				break;

			case IMAGE_DATA_ALIGNMENT_HSL:
				printf("Data alignment is HSL and thus has %u channels\n",m_channel_count);
				break;

			case IMAGE_DATA_ALIGNMENT_RGBA:
				printf("Data alignment is RGBA and thus has %u channels\n",m_channel_count);
				break;

			case IMAGE_DATA_ALIGNMENT_BGRA:
				printf("Data alignment is BGRA and thus has %u channels\n",m_channel_count);
				break;

			case IMAGE_DATA_ALIGNMENT_GRAY:
				printf("Data alignment is GRAY and thus has %u channels\n",m_channel_count);
				break;

			case IMAGE_DATA_ALIGNMENT_GRAYA:
				printf("Data alignment is GRAY and thus has %u channels\n",m_channel_count);
				break;

			default:
				break;
		}

	}

	Image<T>* getMirrorExtensionImage(unsigned int x_ext, unsigned int y_ext)
	{
		pthread_rwlock_rdlock(&m_rw_lock);

		//check for sanity
		if( y_ext > m_height || x_ext > m_width)
		{
			pthread_rwlock_unlock(&m_rw_lock);
			return NULL;
		}

		Image<T>* torus_image = new Image<T>(m_width + (x_ext * 2),m_height + (y_ext * 2),m_data_alignment);

		T* torus_data = torus_image->getmp_image_data();

		unsigned int image_memsize = m_width*m_channel_count*sizeof(T);
		unsigned int image_row_size = m_width*m_channel_count;
		unsigned int x_ext_padding = x_ext*m_channel_count;
		unsigned int torus_row_size = torus_image->getm_width()*m_channel_count;
		unsigned int y_ext_padding = y_ext*torus_row_size;
		unsigned int torus_width = torus_image->getm_width();
		unsigned int torus_channels = torus_image->getm_channel_count();

		//copy the whole picture into the center
		for(unsigned int i=0;i<m_height;i++)
		{
			memcpy( torus_data + y_ext_padding + x_ext_padding + torus_row_size*i , mp_image_data + image_row_size*i, image_memsize );
		}

		//the upper y-extension
		for(unsigned int i=0;i < y_ext;i++)
		{
			memcpy( torus_data + x_ext_padding + torus_row_size*i , mp_image_data + image_row_size*(y_ext-1-i), image_memsize );
		}
		//the lower y-extension
		for(unsigned int i=0;i < y_ext;i++)
		{
			memcpy( torus_data + x_ext_padding+ y_ext_padding + torus_row_size*(i+m_height) ,
					mp_image_data + image_row_size*( (m_height-1) - i),
					image_memsize );
		}

		//upper left corner
		for(unsigned int i=0;i<y_ext;i++)
		{
			for(unsigned int j=0;j<x_ext;j++)
			{
				for(unsigned int k=0;k<torus_channels;k++)
				{
					//printf("x %d y %d k %d || x %d y %d || IDX(%d,%d)=%d\n",j,i,k,x_ext-1 -j,y_ext-1 - i,x_ext-1 -j,y_ext-1 - i,IDX2IMAGEELEMENT( x_ext-1 -j,y_ext-1 - i, m_channel_count,m_width )+k);
					torus_data[ IDX2IMAGEELEMENT( j,i, torus_channels,torus_width )+k ] =
							mp_image_data[ IDX2IMAGEELEMENT( x_ext-1 -j,y_ext-1 - i, m_channel_count,m_width )+k ];
				}
			}
		}

		//upper right corner
		for(unsigned int i=0;i<y_ext;i++)
		{
			for(unsigned int j=0;j<x_ext;j++)
			{
				for(unsigned int k=0;k<torus_channels;k++)
				{
					torus_data[ IDX2IMAGEELEMENT(m_width + x_ext + j,i,
							torus_channels,torus_width )+k
							] =
					mp_image_data[ IDX2IMAGEELEMENT( (m_width-1) - j, y_ext-1 - i,
							m_channel_count,m_width )+k
						   ];
				}
			}
		}

		//lower left corner
		for(unsigned int i=0;i<y_ext;i++)
		{
			for(unsigned int j=0;j<x_ext;j++)
			{
				for(unsigned int k=0;k<torus_channels;k++)
				{
					torus_data[ IDX2IMAGEELEMENT( j,m_height + y_ext +i, torus_channels,torus_width )+k ] =
							mp_image_data[ IDX2IMAGEELEMENT( x_ext-1 -j,(m_height-1) - i, m_channel_count,m_width )+k ];
				}
			}
		}

		//lower right corner
		for(unsigned int i=0;i<y_ext;i++)
		{
			for(unsigned int j=0;j<x_ext;j++)
			{
				for(unsigned int k=0;k<torus_channels;k++)
				{
					torus_data[ IDX2IMAGEELEMENT( m_width + x_ext +j, m_height+ y_ext + i,
							torus_channels,torus_width )+k
							] =
					mp_image_data[ IDX2IMAGEELEMENT( (m_width-1) - j, (m_height-1) - i,
							m_channel_count,m_width )+k
						   ];
				}
			}
		}

		//left side of the image
		for(unsigned int i=0; i < m_height ;i++)
		{
			for(unsigned int j=0;j<x_ext;j++)
			{
				for(unsigned int k=0;k<torus_channels;k++)
				{
					torus_data[ IDX2IMAGEELEMENT( j, y_ext + i,
							torus_channels,torus_width )+k
							] =
					mp_image_data[ IDX2IMAGEELEMENT( (x_ext -1)  - j, i,
							m_channel_count,m_width )+k
						   ];
				}
			}
		}

		//right side of the image
		for(unsigned int i=0; i < m_height ;i++)
		{
			for(unsigned int j=0;j<x_ext;j++)
			{
				for(unsigned int k=0;k<torus_channels;k++)
				{
					torus_data[ IDX2IMAGEELEMENT( (torus_width - 1) - j, y_ext + i,
							torus_channels,torus_width )+k
							] =
					mp_image_data[ IDX2IMAGEELEMENT( (m_width-1) - (x_ext -1)  + j, i,
							m_channel_count,m_width )+k
						   ];
				}
			}
		}


		pthread_rwlock_unlock(&m_rw_lock);

		return torus_image;

	}

	/**
	 * This method works for at most 4 channels.
	 * */
	void getPixel(KTuple<T>* pixel, unsigned int x, unsigned int y)
	{
		pthread_rwlock_rdlock(&m_rw_lock);

		//check for sanity
		if(y >= m_height || x >= m_width)
		{
			ERROR("OUT OF BOUNDS")
			pthread_rwlock_unlock(&m_rw_lock);
			return;
		}

		/*for(unsigned int i = 0;i<m_channel_count;i++)
		{
			pixel->setElement(i, mp_image_data[ m_width*m_channel_count*y + m_channel_count*x + i ] );
		}*/

		long long int offset = m_width*m_channel_count*y + m_channel_count*x;

		switch(m_channel_count)
		{
		case 1:
			pixel->setElement(0, mp_image_data[ offset ] );
			break;

		case 2:
			pixel->setElement(0, mp_image_data[ offset] );
			pixel->setElement(1, mp_image_data[ offset + 1 ] );
			break;

		case 3:
			pixel->setElement(0, mp_image_data[ offset ] );
			pixel->setElement(1, mp_image_data[ offset + 1 ] );
			pixel->setElement(2, mp_image_data[ offset + 2 ] );
			break;

		case 4:
			pixel->setElement(0, mp_image_data[ offset ] );
			pixel->setElement(1, mp_image_data[ offset + 1 ] );
			pixel->setElement(2, mp_image_data[ offset + 2 ] );
			pixel->setElement(3, mp_image_data[ offset + 3 ] );
			break;
		}

		pthread_rwlock_unlock(&m_rw_lock);
	}

	/**
	 * Returns the pixel value at position (x,y) for channel c, returns T() in case of errors.
	 * */
	inline T getPixel(unsigned int x, unsigned int y, unsigned int c)
	{
		pthread_rwlock_rdlock(&m_rw_lock);

		//check for sanity
		if(c >= m_channel_count || y >= m_height || x >= m_width)
		{
			ERROR("OUT OF BOUNDS")
			pthread_rwlock_unlock(&m_rw_lock);
			return T();
		}

		T res = mp_image_data[ m_width*m_channel_count*y + m_channel_count*x +c ];

		pthread_rwlock_unlock(&m_rw_lock);

		return res;
	}

	/**
	 * This method works for at most 4 channels.
	 * */
	void getPixel(const KTuple<T>& pixel, unsigned int x, unsigned int y)
	{
		pthread_rwlock_rdlock(&m_rw_lock);

		//check for sanity
		if(y >= m_height || x >= m_width)
		{
			ERROR("OUT OF BOUNDS")
			pthread_rwlock_unlock(&m_rw_lock);
			return;
		}

		/*for(unsigned int i = 0;i<m_channel_count;i++)
		{
			pixel->setElement(i, mp_image_data[ m_width*m_channel_count*y + m_channel_count*x + i ] );
		}*/

		long long int offset = m_width*m_channel_count*y + m_channel_count*x;

		switch(m_channel_count)
		{
		case 1:
			pixel->setElement(0, mp_image_data[ offset ] );
			break;

		case 2:
			pixel.setElement(0, mp_image_data[ offset] );
			pixel.setElement(1, mp_image_data[ offset + 1 ] );
			break;

		case 3:
			pixel.setElement(0, mp_image_data[ offset ] );
			pixel.setElement(1, mp_image_data[ offset + 1 ] );
			pixel.setElement(2, mp_image_data[ offset + 2 ] );
			break;

		case 4:
			pixel.setElement(0, mp_image_data[ offset ] );
			pixel.setElement(1, mp_image_data[ offset + 1 ] );
			pixel.setElement(2, mp_image_data[ offset + 2 ] );
			pixel.setElement(3, mp_image_data[ offset + 3 ] );
			break;
		}

		pthread_rwlock_unlock(&m_rw_lock);
	}

	/**
	 * Returns the pixel value at position (x,y) for channel c, returns T() in case of errors.
	 * */
	inline T getPixelFast(unsigned int x, unsigned int y, unsigned int c)
	{
		//check for sanity
		if(c >= m_channel_count || y >= m_height || x >= m_width)
		{
			ERROR("OUT OF BOUNDS")
			return T();
		}

		return mp_image_data[ m_width*m_channel_count*y + m_channel_count*x +c ];
	}

	/**
	 * This method works for at most 4 channels.
	 * */
	inline void getPixelFast(FastKTuple<T>* pixel, unsigned int x, unsigned int y)
	{
		//check for sanity
		if(y >= m_height || x >= m_width|| y<0 || x<0)
		{
			ERROR("OUT OF BOUNDS")
			return;
		}

		/*for(unsigned int i = 0;i<m_channel_count;i++)
		{
			pixel->setElement(i, mp_image_data[ m_width*m_channel_count*y + m_channel_count*x + i ] );
		}*/

		long long int offset = m_width*m_channel_count*y + m_channel_count*x;

		switch(m_channel_count)
		{
		case 1:
			pixel->setElement(0, mp_image_data[ offset ] );
			break;

		case 2:
			pixel->setElement(0, mp_image_data[ offset] );
			pixel->setElement(1, mp_image_data[ offset + 1 ] );
			break;

		case 3:
			pixel->setElement(0, mp_image_data[ offset ] );
			pixel->setElement(1, mp_image_data[ offset + 1 ] );
			pixel->setElement(2, mp_image_data[ offset + 2 ] );
			break;

		case 4:
			pixel->setElement(0, mp_image_data[ offset ] );
			pixel->setElement(1, mp_image_data[ offset + 1 ] );
			pixel->setElement(2, mp_image_data[ offset + 2 ] );
			pixel->setElement(3, mp_image_data[ offset + 3 ] );
			break;
		}

	}

	/**
	 * This method works for at most 4 channels.
	 * */
	inline void getPixelFast(const FastKTuple<T>& pixel, unsigned int x, unsigned int y)
	{
		//check for sanity
		if(y >= m_height || x >= m_width|| y<0 || x<0)
		{
			ERROR("OUT OF BOUNDS")
			return;
		}

		/*for(unsigned int i = 0;i<m_channel_count;i++)
		{
			pixel->setElement(i, mp_image_data[ m_width*m_channel_count*y + m_channel_count*x + i ] );
		}*/

		long long int offset = m_width*m_channel_count*y + m_channel_count*x;

		switch(m_channel_count)
		{
		case 1:
			pixel.setElement(0, mp_image_data[ offset ] );
			break;

		case 2:
			pixel.setElement(0, mp_image_data[ offset] );
			pixel.setElement(1, mp_image_data[ offset + 1 ] );
			break;

		case 3:
			pixel.setElement(0, mp_image_data[ offset ] );
			pixel.setElement(1, mp_image_data[ offset + 1 ] );
			pixel.setElement(2, mp_image_data[ offset + 2 ] );
			break;

		case 4:
			pixel.setElement(0, mp_image_data[ offset ] );
			pixel.setElement(1, mp_image_data[ offset + 1 ] );
			pixel.setElement(2, mp_image_data[ offset + 2 ] );
			pixel.setElement(3, mp_image_data[ offset + 3 ] );
			break;
		}

	}

	void fillImage(KTuple<T>* color)
	{
		pthread_rwlock_wrlock(&m_rw_lock);

		unsigned int row_elements = m_channel_count*m_width;
		T row_pattern[row_elements];
		unsigned int row_byte_size = m_channel_count*sizeof(T)*m_width;
		T* color_data = color->getmp_data();

		//create a row pattern
		for(unsigned int i = 0;i<m_width;i++)
		{
			for(unsigned int k=0;k<m_channel_count;k++)
				row_pattern[ i*m_channel_count + k ] = color_data[k];
		}

		#pragma omp parallel
		{
			T* data = mp_image_data;
			#pragma omp for
			for(unsigned int j = 0;j<m_height;j++)
			{
				memcpy( data + j*row_elements , row_pattern, row_byte_size );
			}
		}

		//delete[] pattern;
		pthread_rwlock_unlock(&m_rw_lock);

	}

	void fillImageFast(FastKTuple<T>* color)
	{
		unsigned int row_elements = m_channel_count*m_width;
		T row_pattern[row_elements];
		unsigned int row_byte_size = m_channel_count*sizeof(T)*m_width;
		T* color_data = color->getmp_data();

		//create a row pattern
		for(unsigned int i = 0;i<m_width;i++)
		{
			for(unsigned int k=0;k<m_channel_count;k++)
				row_pattern[ i*m_channel_count + k ] = color_data[k];
		}

		#pragma omp parallel
		{
			T* data = mp_image_data;
			#pragma omp for
			for(unsigned int j = 0;j<m_height;j++)
			{
				memcpy( data + j*row_elements , row_pattern, row_byte_size );
			}
		}
	}

	void fillImage(const KTuple<T>& color)
	{
		pthread_rwlock_wrlock(&m_rw_lock);

		unsigned int row_elements = m_channel_count*m_width;
		T row_pattern[row_elements];
		unsigned int row_byte_size = m_channel_count*sizeof(T)*m_width;
		FastKTuple<T> color_ = color;//constness mania :-)
		T* color_data = color_.getmp_data();

		//create a row pattern
		for(unsigned int i = 0;i<m_width;i++)
		{
			for(unsigned int k=0;k<m_channel_count;k++)
				row_pattern[ i*m_channel_count + k ] = color_data[k];
		}

		#pragma omp parallel
		{
			T* data = mp_image_data;
			#pragma omp for
			for(unsigned int j = 0;j<m_height;j++)
			{
				memcpy( data + j*row_elements , row_pattern, row_byte_size );
			}
		}

		pthread_rwlock_unlock(&m_rw_lock);

	}

	void fillImageFast(const FastKTuple<T>& color)
	{
		unsigned int row_elements = m_channel_count*m_width;
		T row_pattern[row_elements];
		unsigned int row_byte_size = m_channel_count*sizeof(T)*m_width;
		FastKTuple<T> color_ = color;//constness mania :-)
		T* color_data = color_.getmp_data();

		//create a row pattern
		for(unsigned int i = 0;i<m_width;i++)
		{
			for(unsigned int k=0;k<m_channel_count;k++)
				row_pattern[ i*m_channel_count + k ] = color_data[k];
		}

		#pragma omp parallel
		{
			T* data = mp_image_data;
			#pragma omp for
			for(unsigned int j = 0;j<m_height;j++)
			{
				memcpy( data + j*row_elements , row_pattern, row_byte_size );
			}
		}
	}

	/**
	 * Will save the raw image buffer in the given file.
	 * No meta information will be provided, the format is a sequence of <RGBA>...<RGBA>(depending on the current alignment) in row-major fashion, where each
	 * pixel value is saved as multiple bytes depending on T.
	 * */
	void saveRaw(const std::string& file)
	{
		pthread_rwlock_rdlock(&m_rw_lock);

		PrimitiveBuffer buffer((unsigned char*)(this->mp_image_data), this->m_height*this->m_width*this->m_channel_count*sizeof(T));

		buffer.writeBufferToBinaryFile(file);

		pthread_rwlock_unlock(&m_rw_lock);
	}

	/**
	 * Will read a raw image from the given file. It is expected that the image data was saved in the format
	 * <RGBA>...<RGBA>(depending on the original alignment) in row-major fashion, with each channel (R,G,B,A) being sizeof(T) bytes large.
	 * In other words one needs the meta information of the original image.
	 * */
	void readFromRawFile(const std::string& file,unsigned int width, unsigned int height, enum IMAGE_DATA_ALIGNMENT data_alignment=IMAGE_DATA_ALIGNMENT_RGB)
	{
		pthread_rwlock_wrlock(&m_rw_lock);

		PrimitiveBuffer buffer;
		buffer.readBufferFromBinaryFile(file);

		DELETE_NULL_CHECKING(this->mp_image_data);
		this->mp_image_data = (T*)buffer.get_mp_data();

		m_width = width;
		m_height = height;

		switch(data_alignment)
		{
			case IMAGE_DATA_ALIGNMENT_RGB:
				m_channel_count = 3;
				break;

			case IMAGE_DATA_ALIGNMENT_BGR:
				m_channel_count = 3;
				break;

			case IMAGE_DATA_ALIGNMENT_RGBA:
				m_channel_count = 4;
				break;

			case IMAGE_DATA_ALIGNMENT_BGRA:
				m_channel_count = 4;
				break;

			case IMAGE_DATA_ALIGNMENT_GRAY:
				m_channel_count = 1;
				break;

			case IMAGE_DATA_ALIGNMENT_GRAYA:
				m_channel_count = 2;
				break;

			default:
				break;
		}

		m_data_alignment = data_alignment;

		pthread_rwlock_unlock(&m_rw_lock);
	}

	/**
	 * Returns the width.
	 * */
	inline unsigned int getm_width() const
	{
	    return this->m_width;
	}

	/**
	 * Returns the height.
	 * */
	inline unsigned int getm_height() const
	{
	    return this->m_height;
	}

	/**
	 * Returns the width.
	 * */
	inline unsigned int width() const
	{
	    return this->m_width;
	}

	/**
	 * Returns the height.
	 * */
	inline unsigned int height() const
	{
	    return this->m_height;
	}


	inline IMAGE_DATA_ALIGNMENT getm_data_alignment() const
	{
	    return this->m_data_alignment;
	}

	inline IMAGE_DATA_ALIGNMENT alignment() const
	{
		return this->m_data_alignment;
	}

	T* getmp_image_data() const
	{
		return this->mp_image_data;
	}

	const long double m_kr = 0.299;
	const long double m_kg = 0.587;
	const long double m_kb = 0.114;

protected:
	unsigned int m_width;
	unsigned int m_height;
	unsigned int m_channel_count;
	T* mp_image_data;
	IMAGE_DATA_ALIGNMENT m_data_alignment;

	pthread_rwlock_t m_rw_lock;
};

template<typename SOURCETYPE, typename TARGETTYPE> class ImageConverter
{
public:
	ImageConverter()
	{}

	~ImageConverter()
	{}

	/**
	 * Will return the RGB image converted from YPBPR. Channel limits will default to those of type T.
	 * */
	template<typename T>
	static Image<T>* transformYPBPRToRGB(Image<long double>* in)
	{
		if(in->getm_data_alignment() != Image<long double>::IMAGE_DATA_ALIGNMENT_YPBPR)
		{
			printf("ERROR: only RGB to YPBPR conversion supported\n");
			return NULL;
		}

		unsigned long long width = in->width();
		unsigned long long height = in->height();

		Image<long double>* out = new Image<long double>(width,height,
				Image<long double>::IMAGE_DATA_ALIGNMENT_YPBPR);

		T* out_data = out->getmp_image_data();
		long double* in_data = in->getmp_image_data();

		long double y = 0;
		long double pb = 0;
		long double pr = 0;

		T r = T();
		T g = T();
		T b = T();


		for(unsigned int i=0;i<height;++i)
		{
			for(unsigned int j=0;j<width;++j)
			{
				//normalized values
				y = in_data[ width*3*i + 3*j  ];
				pb = in_data[ width*3*i + 3*j + 1 ];
				pr = in_data[ width*3*i + 3*j + 2 ];

				b = (pb*2.0*(1.0-out->m_kb)+y)*(long double)255;
				r = (pr*2.0*(1.0-out->m_kr)+y)*(long double)255;
				g = (y-out->m_kr*r-out->m_kb*b)/(1.0-out->m_kr-out->m_kb)*(long double)255;

				out_data[ width*3*i + 3*j ] = r;
				out_data[ width*3*i + 3*j + 1 ] = g;
				out_data[ width*3*i + 3*j + 2 ] = b;
			}
		}
		return out;
	}

	/**
	 * Will return the RGB image converted from YCBCR. Channel limits will default to those of type T.
	 * */
	template<typename T>
	static Image<T>* transformYCBCRToRGB(Image<long double>* in)
	{
		if(in->getm_data_alignment() != Image<long double>::IMAGE_DATA_ALIGNMENT_YCBCR)
		{
			printf("ERROR: only RGB to YCBCR conversion supported\n");
			return NULL;
		}

		unsigned long long width = in->width();
		unsigned long long height = in->height();

		Image<long double>* out = new Image<long double>(width,height,
				Image<long double>::IMAGE_DATA_ALIGNMENT_YCBCR);

		T* out_data = out->getmp_image_data();
		long double* in_data = in->getmp_image_data();

		long double y = 0;
		long double pb = 0;
		long double pr = 0;

		T r = T();
		T g = T();
		T b = T();


		for(unsigned int i=0;i<height;++i)
		{
			for(unsigned int j=0;j<width;++j)
			{
				//normalized values
				y = (in_data[ width*3*i + 3*j  ]-16.0)/219.0;
				pb = (in_data[ width*3*i + 3*j + 1 ]-128.0)/224.0;
				pr = (in_data[ width*3*i + 3*j + 2 ]-128.0)/224.0;

				b = (pb*2.0*(1.0-out->m_kb)+y)*(long double)255;
				r = (pr*2.0*(1.0-out->m_kr)+y)*(long double)255;
				g = (y-out->m_kr*r-out->m_kb*b)/(1.0-out->m_kr-out->m_kb)*(long double)255;

				out_data[ width*3*i + 3*j ] = r;
				out_data[ width*3*i + 3*j + 1 ] = g;
				out_data[ width*3*i + 3*j + 2 ] = b;
			}
		}
		return out;
	}

	//convert the input image into an rgb image
	static Lazarus::Image<TARGETTYPE>* convertImageRGB2BGRA(Image<SOURCETYPE>* source_image)
	{
		//create the new image
		Lazarus::Image<TARGETTYPE>* target_image = new Lazarus::Image<TARGETTYPE>( source_image->m_width, source_image->m_height,
				Lazarus::Image<TARGETTYPE>::IMAGE_DATA_ALIGNMENT_BGRA );

		pthread_rwlock_rdlock(&source_image->m_rw_lock);

		//copy the data into the new image
		for(unsigned int i = 0;i<source_image->m_width;i++)
		{
			for(unsigned int j = 0;j<source_image->m_height;j++)
			{
				target_image->mp_image_data[ j*source_image->m_width*4 + i*4 + 0] =
						(TARGETTYPE)source_image->mp_image_data[ j*source_image->m_width*3 + i*3 + 2 ];

				target_image->mp_image_data[ j*source_image->m_width*4 + i*4 + 1] =
						(TARGETTYPE)source_image->mp_image_data[ j*source_image->m_width*3 + i*3 + 1 ];

				target_image->mp_image_data[ j*source_image->m_width*4 + i*4 + 2] =
						(TARGETTYPE)source_image->mp_image_data[ j*source_image->m_width*3 + i*3 + 0 ];

				target_image->mp_image_data[ j*source_image->m_width*4 + i*4 + 3] = (TARGETTYPE)255;
			}
		}


		pthread_rwlock_unlock(&source_image->m_rw_lock);

		return target_image;

	}

	//convert the input image into an rgb image
	static Lazarus::Image<TARGETTYPE>* convertImageRGB2RGBA(Image<SOURCETYPE>* source_image)
	{
		//create the new image
		Lazarus::Image<TARGETTYPE>* target_image = new Lazarus::Image<TARGETTYPE>( source_image->m_width, source_image->m_height,
				Lazarus::Image<TARGETTYPE>::IMAGE_DATA_ALIGNMENT_RGBA );

		pthread_rwlock_rdlock(&source_image->m_rw_lock);

		//copy the data into the new image
		for(unsigned int i = 0;i<source_image->m_width;i++)
		{
			for(unsigned int j = 0;j<source_image->m_height;j++)
			{
				target_image->mp_image_data[ j*source_image->m_width*4 + i*4 + 0] =
						(TARGETTYPE)source_image->mp_image_data[ j*source_image->m_width*3 + i*3 + 0 ];

				target_image->mp_image_data[ j*source_image->m_width*4 + i*4 + 1] =
						(TARGETTYPE)source_image->mp_image_data[ j*source_image->m_width*3 + i*3 + 1 ];

				target_image->mp_image_data[ j*source_image->m_width*4 + i*4 + 2] =
						(TARGETTYPE)source_image->mp_image_data[ j*source_image->m_width*3 + i*3 + 2 ];

				target_image->mp_image_data[ j*source_image->m_width*4 + i*4 + 3] = (TARGETTYPE)255;
			}
		}


		pthread_rwlock_unlock(&source_image->m_rw_lock);

		return target_image;

	}

	static Lazarus::Image<TARGETTYPE>* convertImageBGRA2RGB(Image<SOURCETYPE>* source_image)
	{
		//create the new image
		Lazarus::Image<TARGETTYPE>* target_image = new Lazarus::Image<TARGETTYPE>( source_image->m_width, source_image->m_height,
				Lazarus::Image<TARGETTYPE>::IMAGE_DATA_ALIGNMENT_RGB );

		pthread_rwlock_rdlock(&source_image->m_rw_lock);

		//copy the data into the new image
		for(unsigned int i = 0;i<source_image->m_width;i++)
		{
			for(unsigned int j = 0;j<source_image->m_height;j++)
			{
				target_image->mp_image_data[ j*source_image->m_width*3 + i*3 + 0] =
						(TARGETTYPE)source_image->mp_image_data[ j*source_image->m_width*4 + i*4 + 2 ];

				target_image->mp_image_data[ j*source_image->m_width*3 + i*3 + 1] =
						(TARGETTYPE)source_image->mp_image_data[ j*source_image->m_width*4 + i*4 + 1 ];

				target_image->mp_image_data[ j*source_image->m_width*3 + i*3 + 2] =
						(TARGETTYPE)source_image->mp_image_data[ j*source_image->m_width*4 + i*4 + 0 ];

			}
		}


		pthread_rwlock_unlock(&source_image->m_rw_lock);

		return target_image;

	}

	static Lazarus::Image<TARGETTYPE>* convertImageRGBA2RGB(Image<SOURCETYPE>* source_image)
	{
		//create the new image
		Lazarus::Image<TARGETTYPE>* target_image = new Lazarus::Image<TARGETTYPE>( source_image->m_width, source_image->m_height,
				Lazarus::Image<TARGETTYPE>::IMAGE_DATA_ALIGNMENT_RGB );

		pthread_rwlock_rdlock(&source_image->m_rw_lock);

		//copy the data into the new image
		for(unsigned int i = 0;i<source_image->m_width;i++)
		{
			for(unsigned int j = 0;j<source_image->m_height;j++)
			{
				target_image->mp_image_data[ j*source_image->m_width*3 + i*3 + 0] =
						(TARGETTYPE)source_image->mp_image_data[ j*source_image->m_width*4 + i*4 + 0 ];

				target_image->mp_image_data[ j*source_image->m_width*3 + i*3 + 1] =
						(TARGETTYPE)source_image->mp_image_data[ j*source_image->m_width*4 + i*4 + 1 ];

				target_image->mp_image_data[ j*source_image->m_width*3 + i*3 + 2] =
						(TARGETTYPE)source_image->mp_image_data[ j*source_image->m_width*4 + i*4 + 2 ];

			}
		}


		pthread_rwlock_unlock(&source_image->m_rw_lock);

		return target_image;

	}

	static Lazarus::Image<TARGETTYPE>* convertImageBGRA2RGBA(Image<SOURCETYPE>* source_image)
	{
		//create the new image
		Lazarus::Image<TARGETTYPE>* target_image = new Lazarus::Image<TARGETTYPE>( source_image->m_width, source_image->m_height,
				Lazarus::Image<TARGETTYPE>::IMAGE_DATA_ALIGNMENT_RGBA );

		pthread_rwlock_rdlock(&source_image->m_rw_lock);

		//copy the data into the new image
		for(unsigned int i = 0;i<source_image->m_width;i++)
		{
			for(unsigned int j = 0;j<source_image->m_height;j++)
			{
				target_image->mp_image_data[ j*source_image->m_width*4 + i*4 + 0] =
						(TARGETTYPE)source_image->mp_image_data[ j*source_image->m_width*4 + i*4 + 2 ];

				target_image->mp_image_data[ j*source_image->m_width*4 + i*4 + 1] =
						(TARGETTYPE)source_image->mp_image_data[ j*source_image->m_width*4 + i*4 + 1 ];

				target_image->mp_image_data[ j*source_image->m_width*4 + i*4 + 2] =
						(TARGETTYPE)source_image->mp_image_data[ j*source_image->m_width*4 + i*4 + 0 ];

				target_image->mp_image_data[ j*source_image->m_width*4 + i*4 + 3] =
						(TARGETTYPE)source_image->mp_image_data[ j*source_image->m_width*4 + i*4 + 3 ];

			}
		}


		pthread_rwlock_unlock(&source_image->m_rw_lock);

		return target_image;

	}

	//convert the input image to another data type
	static Lazarus::Image<TARGETTYPE>* convertImageDataType(Image<SOURCETYPE>* source_image)
	{
		//create the new image
		Lazarus::Image<TARGETTYPE>* target_image = new Lazarus::Image<TARGETTYPE>( source_image->m_width, source_image->m_height,
				(enum Lazarus::Image<TARGETTYPE>::IMAGE_DATA_ALIGNMENT)source_image->getm_data_alignment() );

		pthread_rwlock_rdlock(&source_image->m_rw_lock);

		//copy the data into the new image
		for(unsigned int i = 0;i<source_image->m_width;i++)
		{
			for(unsigned int j = 0;j<source_image->m_height;j++)
			{
				for(unsigned int k = 0;k<source_image->m_channel_count;k++)
				{
					target_image->mp_image_data[ j*source_image->m_width*source_image->m_channel_count + i*source_image->m_channel_count + k] =
								(TARGETTYPE)source_image->mp_image_data[ j*source_image->m_width*source_image->m_channel_count + i*source_image->m_channel_count + k ];
				}
			}
		}


		pthread_rwlock_unlock(&source_image->m_rw_lock);

		return target_image;
	}
};

}

#endif /* IMAGE_H_ */
