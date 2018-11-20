/*
 * ImageDecorator.h
 *
 *  Created on: 13.02.2013
 *      Author: Darius Malysiak
 */

#ifndef IMAGEDECORATOR_H_
#define IMAGEDECORATOR_H_

#include "../BaseObject.h"
#include "../DataStructures/Image.h"

#include <string>


namespace Lazarus
{

template<class T>
class ImageDecorator: public Lazarus::BaseObject {
public:
	ImageDecorator();
	virtual ~ImageDecorator();

	static void drawLine(Lazarus::Image<T>* image,
			long long int positionX1,
			long long int positionY1,
			long long int positionX2,
			long long int positionY2,
	              T r,
	              T g,
	              T b, T a, double rot_angle = 0);

	static void drawRectangle(Lazarus::Image<T>* image,
			long long int positionX,
			long long int positionY,
	                   unsigned int width,
	                   unsigned int height,
	                   T r,
	                   T g,
	                   T b, T a, double rot_angle = 0);

	static void drawRectangleL(Lazarus::Image<T>* image,
			long long int positionX,
			long long int positionY,
	                   unsigned int width,
	                   unsigned int height,
	                   T r,
	                   T g,
	                   T b, T a,
	                   unsigned int line_width, double rot_angle = 0);

	static void drawCircle(Lazarus::Image<T>* image,
			long long int positionX,
			long long int positionY,
	                unsigned int radius,
	                T r,
	                T g,
	                T b, T a);

	static void drawCircleL(Lazarus::Image<T>* image,
			long long int positionX,
			long long int positionY,
	                unsigned int radius,
	                T r,
	                T g,
	                T b, T a,
	                unsigned int line_width);

	static void drawFilledCircle(Lazarus::Image<T>* image,
			long long int positionX,
			long long int positionY,
	                      unsigned int radius,
	                      T r,
	                      T g,
	                      T b, T a);

	static void drawEllipse(Lazarus::Image<T>* image,
			long long int positionX,
			long long int positionY,
            unsigned int a_axis,
            unsigned int b_axis,
            T r,
            T g,
            T b, T a, double rot_angle = 0);

	static void drawFilledEllipse(Lazarus::Image<T>* image,
			long long int positionX,
			long long int positionY,
            unsigned int a_axis,
            unsigned int b_axis,
            T r,
            T g,
            T b, T a, double rot_angle = 0);

	static void drawEllipseL(Lazarus::Image<T>* image,
			long long int positionX,
			long long int positionY,
			unsigned int a_axis,
			unsigned int b_axis,
			T r,
			T g,
			T b, T a,
			unsigned int line_width, double rot_angle = 0);

	static void drawFilledRectangle(Lazarus::Image<T>* image,
			long long int positionX,
			long long int positionY,
	                         unsigned int width,
	                         unsigned int height,
	                         T r,
	                         T g,
	                         T b, T a, double rot_angle = 0);

	/**
	 * If a rotation is specified this method will rotate the rectangle around the center.
	 * */
	static void drawFilledRectangleCenter(Lazarus::Image<T>* image,
			long long int positionX,
			long long int positionY,
							 unsigned int width,
							 unsigned int height,
							 T r,
							 T g,
							 T b, T a, double rot_angle = 0, int center_x=0, int center_y=0);

	static void renderText(Lazarus::Image<T>* image, const std::string& text,
			long long int positionX,
			long long int positionY,unsigned int size,
            T r,
            T g,
            T b, T a, double rot_angle = 0, unsigned int face_index=0);

	static void renderTextGray(Lazarus::Image<T>* image, const std::string& text,
			long long int positionX,
			long long int positionY,unsigned int size, T a, double rot_angle = 0, unsigned int face_index=0);
};

}

#endif /* IMAGEDECORATOR_H_ */
