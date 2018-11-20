/*
 * ImageDecorator.cpp
 *
 *  Created on: 13.02.2013
 *      Author: Darius Malysiak
 */

#include "ImageDecorator.h"

#include <stdio.h>
#include <cmath>

namespace Lazarus
{

template<class T>
ImageDecorator<T>::ImageDecorator() {
	// TODO Auto-generated constructor stub

}

template<class T>
ImageDecorator<T>::~ImageDecorator() {
	// TODO Auto-generated destructor stub
}

void rotate(long long int &x, long long int &y, double rotA, double rotB, double rotC, double rotD)
{
	  long long int temp_x = std::round(rotA * (double)x + rotB * (double)y);
	  long long int temp_y = std::round(rotC * (double)x + rotD * (double)y);

	  x=temp_x;
	  y=temp_y;
}


template<class T>
void ImageDecorator<T>::drawLine(Lazarus::Image<T>* image,
				long long int positionX1,
				long long int positionY1,
				long long int positionX2,
				long long int positionY2,
				T r,
				T g,
				T b, T a, double rot_angle)
{

	Lazarus::FastKTuple<T> color(4);
	color.setElement(0,r);
	color.setElement(1,g);
	color.setElement(2,b);
	color.setElement(3,a);

	double rotA = cos(-rot_angle);
	double rotB = -sin(-rot_angle);
	double rotC = sin(-rot_angle);
	double rotD = cos(-rot_angle);

	//calculate the center of the bounding rect
	int center_x = abs(positionX2-positionX1)/2;
	int center_y = abs(positionY2-positionY1)/2;

	//from left to right
	if( positionX2>positionX1 )
	{
		center_x = positionX1 + center_x;

		//from top to bottom
		if(positionY2 > positionY1)
		{
			center_y = positionY1 + center_y;
		}
		else //from bottom to top
		{
			center_y = positionY1 - center_y;
		}
	}
	else //from right to left
	{
		center_x = positionX1 - center_x;

		//from top to bottom
		if(positionY2 > positionY1)
		{
			center_y = positionY1 + center_y;
		}
		else //from bottom to top
		{
			center_y = positionY1 - center_y;
		}
	}

	//shift the points into the rotation origin
	positionX2 -= center_x;
	positionX1 -= center_x;
	positionY2 -= center_y;
	positionY1 -= center_y;

	//rotate the endpoints
	rotate(positionX1,positionY1,rotA,rotB,rotC,rotD);
	rotate(positionX2,positionY2,rotA,rotB,rotC,rotD);

	//shift the points back
	positionX2 += center_x;
	positionX1 += center_x;
	positionY2 += center_y;
	positionY1 += center_y;

	int dx =  abs(positionX2-positionX1), sx = positionX1<positionX2 ? 1 : -1;
	int dy = -abs(positionY2-positionY1), sy = positionY1<positionY2 ? 1 : -1;
	int err = dx+dy, e2; /* error value e_xy */

	for(;;)
	{
		image->setPixelFast(&color,positionX1,positionY1);

		if (positionY1==positionY2 && positionX1==positionX2) break;
		e2 = 2*err;
		if (e2 > dy) { err += dy; positionX1 += sx; } /* e_xy+e_x > 0 */
		if (e2 < dx) { err += dx; positionY1 += sy; } /* e_xy+e_y < 0 */
	}


}

template<class T>
void ImageDecorator<T>::drawRectangleL(Lazarus::Image<T>* image,
		long long int positionX,
		long long int positionY,
                   unsigned int width,
                   unsigned int height,
                   T r,
                   T g,
                   T b, T a,
                   unsigned int line_width, double rot_angle)
{

	for(unsigned int i=0;i<line_width;i++)
	{
		drawRectangle(image,positionX-i,positionY-i,width+2*i,height+2*i,r,g,b,a,rot_angle);
	}

}


template<class T>
void ImageDecorator<T>::drawRectangle(Lazarus::Image<T>* image,
		long long int positionX1,
		long long int positionY1,
					unsigned int width,
					unsigned int height,
					T r,
					T g,
					T b, T a, double rot_angle)
{

	//this is impossible!
    if(positionX1 >= (long long int)image->getm_width() || positionY1 >= (long long int)image->getm_height())
    {
        return;
    }

    double rotA = cos(-rot_angle);
	double rotB = -sin(-rot_angle);
	double rotC = sin(-rot_angle);
	double rotD = cos(-rot_angle);

    //calculate the center of the bounding rect (1234 clockwise, starting at upper left corner),
	//as well as the four corners
	long long int positionX2 = positionX1 + width;
	long long int positionY2 = positionY1;
	long long int positionX3 = positionX1 + width;
	long long int positionY3 = positionY1 + height;
	long long int positionX4 = positionX1;
	long long int positionY4 = positionY1 + height;
	int center_x = positionX1+(positionX3-positionX1)/2;
	int center_y = positionY1+(positionY3-positionY1)/2;

	//shift the points into the rotation origin
	positionX1 -= center_x;
	positionX2 -= center_x;
	positionX3 -= center_x;
	positionX4 -= center_x;
	positionY1 -= center_y;
	positionY2 -= center_y;
	positionY3 -= center_y;
	positionY4 -= center_y;

	/*printf("center %d %d \n",center_x,center_y);
	printf("%d %d \n",positionX1,positionY1);
	printf("%d %d \n",positionX2,positionY2);
	printf("%d %d \n",positionX3,positionY3);
	printf("%d %d \n",positionX4,positionY4);

	printf("after rot\n");*/

	//rotate the endpoints
	rotate(positionX1,positionY1,rotA,rotB,rotC,rotD);
	rotate(positionX2,positionY2,rotA,rotB,rotC,rotD);
	rotate(positionX3,positionY3,rotA,rotB,rotC,rotD);
	rotate(positionX4,positionY4,rotA,rotB,rotC,rotD);

	/*printf("center %d %d \n",center_x,center_y);
	printf("%d %d \n",positionX1,positionY1);
	printf("%d %d \n",positionX2,positionY2);
	printf("%d %d \n",positionX3,positionY3);
	printf("%d %d \n",positionX4,positionY4);
	printf("\n\n");*/

	//shift the points back
	positionX1 += center_x;
	positionX2 += center_x;
	positionX3 += center_x;
	positionX4 += center_x;
	positionY1 += center_y;
	positionY2 += center_y;
	positionY3 += center_y;
	positionY4 += center_y;

	drawLine(image,positionX1,positionY1,positionX2,positionY2,r,g,b,a,0);
	drawLine(image,positionX2,positionY2,positionX3,positionY3,r,g,b,a,0);
	drawLine(image,positionX3,positionY3,positionX4,positionY4,r,g,b,a,0);
	drawLine(image,positionX4,positionY4,positionX1,positionY1,r,g,b,a,0);



}

template<class T>
void ImageDecorator<T>::drawCircle(Lazarus::Image<T>* image,
		long long int positionX,
		long long int positionY,
                unsigned int radius,
                T r,
				T g,
				T b, T a)
{
	Lazarus::FastKTuple<T> color(4);
	color.setElement(0,r);
	color.setElement(1,g);
	color.setElement(2,b);
	color.setElement(3,a);

    //not bresenham's circle algorithm
    unsigned int x = 0;
    unsigned int y = radius;
    int d = 1 - radius;

    image->setPixelFast(&color,positionX+x, positionY - y );

    while(y > x)
    {
        //we choose the upper pixel (i.e. the right neighbor of the current pixel)
        if(d < 0)
        {
            //update d
            d = d + 2*x + 3;
            x++;
        }
        else //we choose the next lower pixel
        {
            //update d
            d = d + 2*(x-y) + 5;
            x++;
            y--;
        }

        //resulting pixel
        image->setPixelFast(&color,positionX+ x, positionY- y);

        //and the remaining symmetric 7 others
        image->setPixelFast(&color,positionX+ y, positionY- x );
        image->setPixelFast(&color,positionX+ y, positionY + x );
        image->setPixelFast(&color,positionX+ x, positionY+ y );
        image->setPixelFast(&color,positionX- x, positionY+ y );
        image->setPixelFast(&color,positionX- y, positionY- x );
        image->setPixelFast(&color,positionX- y, positionY+ x );
        image->setPixelFast(&color,positionX- x, positionY- y );

    }


}

template<class T>
void ImageDecorator<T>::drawCircleL(Lazarus::Image<T>* image,
		long long int positionX,
		long long int positionY,
                unsigned int radius,
                T r,
				T g,
				T b, T a,
				unsigned int line_width)
{
	  Lazarus::FastKTuple<T> color(4);
	  color.setElement(0,r);
	  color.setElement(1,g);
	  color.setElement(2,b);
	  color.setElement(3,a);

		for(unsigned int i=0;i<line_width;i++)
		{
		   drawCircle(image,positionX,positionY,radius+i,r,g,b,a);
		}
}

template<class T>
void ImageDecorator<T>::drawFilledCircle(Lazarus::Image<T>* image,
		long long int positionX,
		long long int positionY,
						unsigned int radius,
						T r,
						T g,
						T b, T a)
{

  Lazarus::FastKTuple<T> color(4);
  color.setElement(0,r);
  color.setElement(1,g);
  color.setElement(2,b);
  color.setElement(3,a);

  double r_2 = pow(radius,2);

  //draw first quadrant
  for(unsigned int i=0;i <= radius ;i++)
  {
      for(unsigned int j= 0 ; j <= sqrt( r_2 - pow(i,2) )  ;j++)
      {
          image->setPixelFast(&color,positionX+i, positionY-j);
      }
  }
  //draw second quadrant
  for(unsigned int i=0;i <= radius ;i++)
  {
      for(unsigned int j= 0 ; j <= sqrt( r_2 - pow(i,2) )  ;j++)
      {
          image->setPixelFast(&color,positionX-i, positionY-j);
      }

  }
  //draw third quadrant
  for(unsigned int i=0;i <= radius ;i++)
  {
      for(unsigned int j= 0 ; j <= sqrt( r_2 - pow(i,2) )  ;j++)
      {
          image->setPixelFast(&color,positionX-i, positionY+j);
      }

  }
  //draw fourth quadrant
  for(unsigned int i=0;i <= radius ;i++)
  {
      for(unsigned int j= 0 ; j <= sqrt( r_2 - pow(i,2) )  ;j++)
      {
          image->setPixelFast(&color,positionX+i, positionY+j);
      }

  }

}

template<class T>
void ImageDecorator<T>::drawEllipse(Lazarus::Image<T>* image,
		long long int positionX,
		long long int positionY,
        unsigned int a_axis,
        unsigned int b_axis,
        T r,
        T g,
        T b, T a, double rot_angle)
{

   Lazarus::FastKTuple<T> color(4);
   color.setElement(0,r);
   color.setElement(1,g);
   color.setElement(2,b);
   color.setElement(3,a);

   long long int temp_x = 0;
   long long int temp_y = 0;

   double rotA = cos(rot_angle);
   double rotB = -sin(rot_angle);
   double rotC = sin(rot_angle);
   double rotD = cos(rot_angle);

   double rotA2 = cos(-rot_angle);
   double rotB2 = -sin(-rot_angle);
   double rotC2 = sin(-rot_angle);
   double rotD2 = cos(-rot_angle);

   //transform the inital vector (0,b_axis) into the new system
   int dx = 0;
   int dy = b_axis; /* im I. Quadranten von links oben nach rechts unten */
   //rotate(dx,dy,3.0*rot_angle);
   long a2 = a_axis*a_axis;
   long b2 = b_axis*b_axis;
   long err = b2-(2*b_axis -1)*a2;
   long e2; /* Fehler im 1. Schritt */

   //we distinguish with respect to efficiency
   if( rot_angle == 0.0 )//if not very close to zero
   {
       do {
		   temp_x = dx;
		   temp_y = dy;
		   rotate(temp_x,temp_y,rotA2,rotB2,rotC2,rotD2);

		   image->setPixelFast(&color,positionX+temp_x, positionY+temp_y); /* I. Quadrant */

		   image->setPixelFast(&color,positionX-temp_x, positionY-temp_y); /* III. Quadrant */

		   temp_x = dx;
		   temp_y = dy;
		   rotate(temp_x,temp_y,rotA,rotB,rotC,rotD);

		   image->setPixelFast(&color,positionX-temp_x, positionY+temp_y); /* II. Quadrant */

		   image->setPixelFast(&color,positionX+temp_x, positionY-temp_y); /* IV. Quadrant */

		   e2 = 2*err;
		   if (e2 <  (2*dx+1)*b2) { dx++; err += (2*dx+1)*b2; }
		   if (e2 > -(2*dy-1)*a2) { dy--; err -= (2*dy-1)*a2; }
       } while (dy >= 0);


       while (dx++ < (int)a_axis)
       { /* fehlerhafter Abbruch bei flachen Ellipsen (b=1) */

    	   temp_x = dx;
    	   temp_y = 0;

    	   rotate(temp_x,temp_y,rotA,rotB,rotC,rotD);

    	   image->setPixelFast(&color,positionX+temp_x, positionY); /* -> Spitze der Ellipse vollenden */
    	   image->setPixelFast(&color,positionX-temp_x, positionY);
       }
   }
   else
   {
	  do {
		  temp_x = dx;
		  temp_y = dy;

		  image->setPixelFast(&color,positionX+temp_x, positionY+temp_y); /* I. Quadrant */

		  image->setPixelFast(&color,positionX-temp_x, positionY+temp_y); /* II. Quadrant */

		  image->setPixelFast(&color,positionX-temp_x, positionY-temp_y); /* III. Quadrant */

		  image->setPixelFast(&color,positionX+temp_x, positionY-temp_y); /* IV. Quadrant */

		  e2 = 2*err;
		  if (e2 <  (2*dx+1)*b2) { dx++; err += (2*dx+1)*b2; }
		  if (e2 > -(2*dy-1)*a2) { dy--; err -= (2*dy-1)*a2; }
	  } while (dy >= 0);


	   while (dx++ < (int)a_axis)
	   { /* fehlerhafter Abbruch bei flachen Ellipsen (b=1) */

		   temp_x = dx;
		   temp_y = 0;

		   image->setPixelFast(&color,positionX+temp_x, positionY); /* -> Spitze der Ellipse vollenden */
		   image->setPixelFast(&color,positionX-temp_x, positionY);
	   }
   }

}

template<class T>
void ImageDecorator<T>::drawFilledEllipse(Lazarus::Image<T>* image,
		long long int positionX,
		long long int positionY,
        unsigned int a_axis,
        unsigned int b_axis,
        T r,
        T g,
        T b, T a, double rot_angle)
{
   int dx = 0;
   int dy = b_axis; /* im I. Quadranten von links oben nach rechts unten */
   long a2 = a_axis*a_axis;
   long b2 = b_axis*b_axis;
   long err = b2-(2*b_axis -1)*a2;
   long e2; /* Fehler im 1. Schritt */

   Lazarus::FastKTuple<T> color(4);
   color.setElement(0,r);
   color.setElement(1,g);
   color.setElement(2,b);
   color.setElement(3,255);
   Lazarus::FastKTuple<T> temp_color(4);
   temp_color.setElement(3,255);

   long long int temp_x = 0;
   long long int temp_y = 0;

   double rotA = cos(rot_angle);
   double rotB = -sin(rot_angle);
   double rotC = sin(rot_angle);
   double rotD = cos(rot_angle);

   double rotA2 = cos(-rot_angle);
   double rotB2 = -sin(-rot_angle);
   double rotC2 = sin(-rot_angle);
   double rotD2 = cos(-rot_angle);

	//we distinguish with respect to efficiency
	if( rot_angle == 0 )//if not very close to zero
	{
       do
       {
		   //printf("%d\n",dy);
		   for(int i=dy; i >= 0; i--)
		   {

			   temp_x = dx;
			   temp_y = i;
			   rotate(temp_x,temp_y,rotA2,rotB2,rotC2,rotD2);

			   image->setPixelFast(&color,positionX+temp_x, positionY+temp_y); /* I. Quadrant */

			   image->setPixelFast(&color,positionX-temp_x, positionY-temp_y); /* III. Quadrant */

			   //filling gaps with a kind of structuring element
				  image->getPixelFast(&temp_color,positionX+temp_x-1, positionY+temp_y-1);
				  if(temp_color.getElement(0) != color.getElement(0) ||
						  temp_color.getElement(1) != color.getElement(1)||
						  temp_color.getElement(2) != color.getElement(2) ||
						  temp_color.getElement(3) != color.getElement(3))
				  {
					  image->setPixelFast(&color,positionX+temp_x-1, positionY+temp_y-1);
				  }

				  image->getPixelFast(&temp_color,positionX+temp_x+1, positionY+temp_y+1);
				  if(temp_color.getElement(0) != color.getElement(0) ||
						  temp_color.getElement(1) != color.getElement(1)||
						  temp_color.getElement(2) != color.getElement(2) ||
						  temp_color.getElement(3) != color.getElement(3))
				  {
					  image->setPixelFast(&color,positionX+temp_x+1, positionY+temp_y+1);
				  }

				  image->getPixelFast(&temp_color,positionX+temp_x-1, positionY+temp_y+1);
				  if(temp_color.getElement(0) != color.getElement(0) ||
						  temp_color.getElement(1) != color.getElement(1)||
						  temp_color.getElement(2) != color.getElement(2) ||
						  temp_color.getElement(3) != color.getElement(3))
				  {
					  image->setPixelFast(&color,positionX+temp_x-1, positionY+temp_y+1);
				  }

				  image->getPixelFast(&temp_color,positionX+temp_x+1, positionY+temp_y-1);
				  if(temp_color.getElement(0) != color.getElement(0) ||
						  temp_color.getElement(1) != color.getElement(1)||
						  temp_color.getElement(2) != color.getElement(2) ||
						  temp_color.getElement(3) != color.getElement(3))
				  {
					  image->setPixelFast(&color,positionX+temp_x+1, positionY+temp_y-1);
				  }

				  //---------------------------------------------------------------

				  image->getPixelFast(&temp_color,positionX-temp_x-1, positionY-temp_y-1);
				  if(temp_color.getElement(0) != color.getElement(0) ||
						  temp_color.getElement(1) != color.getElement(1)||
						  temp_color.getElement(2) != color.getElement(2) ||
						  temp_color.getElement(3) != color.getElement(3))
				  {
					  image->setPixelFast(&color,positionX-temp_x-1, positionY-temp_y-1);
				  }

				  image->getPixelFast(&temp_color,positionX-temp_x+1, positionY-temp_y+1);
				  if(temp_color.getElement(0) != color.getElement(0) ||
						  temp_color.getElement(1) != color.getElement(1)||
						  temp_color.getElement(2) != color.getElement(2) ||
						  temp_color.getElement(3) != color.getElement(3))
				  {
					  image->setPixelFast(&color,positionX-temp_x+1, positionY-temp_y+1);
				  }

				  image->getPixelFast(&temp_color,positionX-temp_x-1, positionY-temp_y+1);
				  if(temp_color.getElement(0) != color.getElement(0) ||
						  temp_color.getElement(1) != color.getElement(1)||
						  temp_color.getElement(2) != color.getElement(2) ||
						  temp_color.getElement(3) != color.getElement(3))
				  {
					  image->setPixelFast(&color,positionX-temp_x-1, positionY-temp_y+1);
				  }

				  image->getPixelFast(&temp_color,positionX-temp_x+1, positionY-temp_y-1);
				  if(temp_color.getElement(0) != color.getElement(0) ||
						  temp_color.getElement(1) != color.getElement(1)||
						  temp_color.getElement(2) != color.getElement(2) ||
						  temp_color.getElement(3) != color.getElement(3))
				  {
					  image->setPixelFast(&color,positionX-temp_x+1, positionY-temp_y-1);
				  }


			   temp_x = dx;
			   temp_y = i;
			   rotate(temp_x,temp_y,rotA,rotB,rotC,rotD);

			   image->setPixelFast(&color,positionX-temp_x, positionY+temp_y); /* II. Quadrant */

			   image->setPixelFast(&color,positionX+temp_x, positionY-temp_y); /* IV. Quadrant */



				  image->getPixelFast(&temp_color,positionX-temp_x-1, positionY+temp_y-1);
				  if(temp_color.getElement(0) != color.getElement(0) ||
						  temp_color.getElement(1) != color.getElement(1)||
						  temp_color.getElement(2) != color.getElement(2) ||
						  temp_color.getElement(3) != color.getElement(3))
				  {
					  image->setPixelFast(&color,positionX-temp_x-1, positionY+temp_y-1);
				  }

				  image->getPixelFast(&temp_color,positionX-temp_x+1, positionY+temp_y+1);
				  if(temp_color.getElement(0) != color.getElement(0) ||
						  temp_color.getElement(1) != color.getElement(1)||
						  temp_color.getElement(2) != color.getElement(2) ||
						  temp_color.getElement(3) != color.getElement(3))
				  {
					  image->setPixelFast(&color,positionX-temp_x+1, positionY+temp_y+1);
				  }

				  image->getPixelFast(&temp_color,positionX-temp_x-1, positionY+temp_y+1);
				  if(temp_color.getElement(0) != color.getElement(0) ||
						  temp_color.getElement(1) != color.getElement(1)||
						  temp_color.getElement(2) != color.getElement(2) ||
						  temp_color.getElement(3) != color.getElement(3))
				  {
					  image->setPixelFast(&color,positionX-temp_x-1, positionY+temp_y+1);
				  }

				  image->getPixelFast(&temp_color,positionX-temp_x+1, positionY+temp_y-1);
				  if(temp_color.getElement(0) != color.getElement(0) ||
						  temp_color.getElement(1) != color.getElement(1)||
						  temp_color.getElement(2) != color.getElement(2) ||
						  temp_color.getElement(3) != color.getElement(3))
				  {
					  image->setPixelFast(&color,positionX-temp_x+1, positionY+temp_y-1);
				  }

				  //---------------------------------------------------------------

				  image->getPixelFast(&temp_color,positionX+temp_x-1, positionY-temp_y-1);
				  if(temp_color.getElement(0) != color.getElement(0) ||
						  temp_color.getElement(1) != color.getElement(1)||
						  temp_color.getElement(2) != color.getElement(2) ||
						  temp_color.getElement(3) != color.getElement(3))
				  {
					  image->setPixelFast(&color,positionX+temp_x-1, positionY-temp_y-1);
				  }

				  image->getPixelFast(&temp_color,positionX+temp_x+1, positionY-temp_y+1);
				  if(temp_color.getElement(0) != color.getElement(0) ||
						  temp_color.getElement(1) != color.getElement(1)||
						  temp_color.getElement(2) != color.getElement(2) ||
						  temp_color.getElement(3) != color.getElement(3))
				  {
					  image->setPixelFast(&color,positionX+temp_x+1, positionY-temp_y+1);
				  }

				  image->getPixelFast(&temp_color,positionX+temp_x-1, positionY-temp_y+1);
				  if(temp_color.getElement(0) != color.getElement(0) ||
						  temp_color.getElement(1) != color.getElement(1)||
						  temp_color.getElement(2) != color.getElement(2) ||
						  temp_color.getElement(3) != color.getElement(3))
				  {
					  image->setPixelFast(&color,positionX+temp_x-1, positionY-temp_y+1);
				  }

				  image->getPixelFast(&temp_color,positionX+temp_x+1, positionY-temp_y-1);
				  if(temp_color.getElement(0) != color.getElement(0) ||
						  temp_color.getElement(1) != color.getElement(1)||
						  temp_color.getElement(2) != color.getElement(2) ||
						  temp_color.getElement(3) != color.getElement(3))
				  {
					  image->setPixelFast(&color,positionX+temp_x+1, positionY-temp_y-1);
				  }


		   }

		   e2 = 2*err;
		   if (e2 <  (2*dx+1)*b2) { dx++; err += (2*dx+1)*b2; }
		   if (e2 > -(2*dy-1)*a2) { dy--; err -= (2*dy-1)*a2; }
	   } while (dy >= 0);


       while (dx++ < (int)a_axis)
       { /* fehlerhafter Abbruch bei flachen Ellipsen (b=1) */

    	   temp_x = dx;
    	   temp_y = 0;

    	   rotate(temp_x,temp_y,rotA,rotB,rotC,rotD);

    	   image->setPixelFast(&color,positionX+temp_x, positionY); /* -> Spitze der Ellipse vollenden */
    	   image->setPixelFast(&color,positionX-temp_x, positionY);
       }

	}
	else //in case of zero rotation
	{
	   do
	   {
		   //printf("%d\n",dy);
		   for(int i=dy; i >= 0; i--)
		   {

			   temp_x = dx;
			   temp_y = i;

			   image->setPixelFast(&color,positionX+temp_x, positionY+temp_y); /* I. Quadrant */

			   image->setPixelFast(&color,positionX-temp_x, positionY-temp_y); /* III. Quadrant */

			   image->setPixelFast(&color,positionX-temp_x, positionY+temp_y); /* II. Quadrant */

			   image->setPixelFast(&color,positionX+temp_x, positionY-temp_y); /* IV. Quadrant */
		   }

		   e2 = 2*err;
		   if (e2 <  (2*dx+1)*b2) { dx++; err += (2*dx+1)*b2; }
		   if (e2 > -(2*dy-1)*a2) { dy--; err -= (2*dy-1)*a2; }
	   } while (dy >= 0);


	   while (dx++ < (int)a_axis)
	   { /* fehlerhafter Abbruch bei flachen Ellipsen (b=1) */

		   temp_x = dx;
		   temp_y = 0;

		   image->setPixelFast(&color,positionX+temp_x, positionY); /* -> Spitze der Ellipse vollenden */
		   image->setPixelFast(&color,positionX-temp_x, positionY);
	   }
	}

}

template<class T>
void ImageDecorator<T>::drawEllipseL(Lazarus::Image<T>* image,
		long long int positionX,
		long long int positionY,
        unsigned int a_axis,
        unsigned int b_axis,
        T r,
        T g,
        T b, T a,
        unsigned int line_width, double rot_angle)
{

	Lazarus::FastKTuple<T> color(4);
	color.setElement(0,r);
	color.setElement(1,g);
	color.setElement(2,b);
	color.setElement(3,a);

   for(unsigned int i=0;i<line_width;i++)
   {
	   drawEllipse(image,positionX,positionY,a_axis+i,b_axis+i,r,g,b,a,rot_angle);
   }

}

template<class T>
void ImageDecorator<T>::drawFilledRectangle(Lazarus::Image<T>* image,
		long long int positionX,
		long long int positionY,
                         unsigned int width,
                         unsigned int height,
                         T r,
                         T g,
                         T b, T a, double rot_angle)
{

  Lazarus::FastKTuple<T> color(4);
  color.setElement(0,r);
  color.setElement(1,g);
  color.setElement(2,b);
  color.setElement(3,a);

  Lazarus::FastKTuple<T> temp_color(4);

  long long int temp_x=0;
  long long int temp_y=0;

  long long int center_x = positionX + width/2.0;
  long long int center_y = positionY + height/2.0;

  double rotA = cos(rot_angle);
  double rotB = -sin(rot_angle);
  double rotC = sin(rot_angle);
  double rotD = cos(rot_angle);

  //we distinguish with respect to efficiency
  if( rot_angle == 0 )//if not very close to zero
  {
	  for(int i=0;i < (int)width;++i)
	  {
		  for(int j=0;j > -((int)height);--j)
		  {
			  temp_x = i - (int)width/2;
			  temp_y = j + (int)height/2;

			  rotate(temp_x,temp_y,rotA,rotB,rotC,rotD);

			  if((i!=0 || j!=0))
			  {
				  //filling gaps with a kind of structuring element
				  image->getPixelFast(&temp_color,center_x-temp_x-1,center_y+temp_y-1);
				  if(temp_color.getElement(0) != color.getElement(0) ||
						  temp_color.getElement(1) != color.getElement(1)||
						  temp_color.getElement(2) != color.getElement(2) ||
						  temp_color.getElement(3) != color.getElement(3))
				  {
					  image->setPixelFast(&color,center_x-temp_x-1,center_y+temp_y-1);
				  }
				  image->getPixelFast(&temp_color,center_x+temp_x-1,center_y+temp_y);
				  if(temp_color.getElement(0) != color.getElement(0) ||
						  temp_color.getElement(1) != color.getElement(1)||
						  temp_color.getElement(2) != color.getElement(2) ||
						  temp_color.getElement(3) != color.getElement(3))
				  {
					  image->setPixelFast(&color,center_x-temp_x-1,center_y+temp_y);
				  }
			  }

			  image->setPixelFast(&color,center_x-temp_x,center_y+temp_y);
		  }
	  }
  }
  else //zero rotation
  {
	  for(int i=0;i < (int)width;i++)
	  {
		  for(int j=0;j > -((int)height);--j)
		  {
			  temp_x = i - (int)width/2;
			  temp_y = j + (int)height/2;

			  image->setPixelFast(&color,center_x-temp_x,center_y+temp_y);
		  }
	  }
  }

}


template<class T>
void ImageDecorator<T>::drawFilledRectangleCenter(Lazarus::Image<T>* image,
		long long int positionX,
		long long int positionY,
                         unsigned int width,
                         unsigned int height,
                         T r,
                         T g,
                         T b, T a, double rot_angle, int center_x, int center_y)
{

  Lazarus::FastKTuple<T> color(4);
  color.setElement(0,r);
  color.setElement(1,g);
  color.setElement(2,b);
  color.setElement(3,a);

  Lazarus::FastKTuple<T> temp_color(4);

  long long int temp_x=0;
  long long int temp_y=0;

  double rotA = cos(rot_angle);
  double rotB = -sin(rot_angle);
  double rotC = sin(rot_angle);
  double rotD = cos(rot_angle);

  //we distinguish with respect to efficiency
  if( rot_angle == 0.0 )//if not very close to zero
  {
	  for(unsigned int i=0;i < width;++i)
	  {
		  for(unsigned int j=0;j < height;++j)
		  {
			  temp_x = (long long int)center_x-( (long long int)i + positionX);
			  temp_y = (long long int)center_y-( (long long int)j + positionY);

			  rotate(temp_x,temp_y,rotA,rotB,rotC,rotD);

			  //filling gaps
			  if(temp_x+( (long long int)i + positionX) > 1)
			  image->setPixelFast(&color,temp_x+( (long long int)i + positionX)-1,temp_y+( (long long int)j + positionY));
			  if(temp_x+( (long long int)i + positionX) > 1 && temp_y+( (long long int)j + positionY) > 1)
			  image->setPixelFast(&color,temp_x+( (long long int)i + positionX)-1,temp_y+( (long long int)j + positionY)-1);


			  image->setPixelFast(&color,temp_x+( (long long int)i + positionX),temp_y+( (long long int)j + positionY));
		  }
	  }
  }
  else //zero rotation
  {
	  for(unsigned int i=0;i < width;++i)
	  {
		  for(unsigned int j=0;j < height;++j)
		  {
			  temp_x = positionX+i;
			  temp_y = positionY+j;

			  image->setPixelFast(&color,temp_x,temp_y);
		  }
	  }
  }

}

template class ImageDecorator<unsigned char>;
template class ImageDecorator<char>;
template class ImageDecorator<unsigned short int>;
template class ImageDecorator<short int>;

}
