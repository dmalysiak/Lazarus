/*
 * NVector.h
 *
 *  Created on: 08.03.2013
 *      Author: Darius Malysiak
 */

#ifndef FASTVECTORN_H_
#define FASTVECTORN_H_

#include "../BaseObject.h"

#include <pthread.h>

namespace Lazarus
{


template<class T1 = double, class T2 = double, class T3 = double, class T4 = double,
		class T5 = double, class T6 = double, class T7 = double, class T8 = double,
		class T9 = double>
class FastVector9: public BaseObject {
public:
	FastVector9()
	{
		m_data1 = T1();
		m_data2 = T2();
		m_data3 = T3();
		m_data4 = T4();
		m_data5 = T5();
		m_data6 = T6();
		m_data7 = T7();
		m_data8 = T8();
		m_data9 = T9();
	}

	FastVector9(const T1& a, const T2& b, const T3& c, const T4& d, const T5& e, const T6& f, const T7& g, const T8& h,
			 const T8& i)
	{
		m_data1 = a;
		m_data2 = b;
		m_data3 = c;
		m_data4 = d;
		m_data5 = e;
		m_data6 = f;
		m_data7 = g;
		m_data8 = h;
		m_data9 = i;
	}

	virtual ~FastVector9()
	{
	}


	T1 m_data1;
	T2 m_data2;
	T3 m_data3;
	T4 m_data4;
	T5 m_data5;
	T6 m_data6;
	T7 m_data7;
	T8 m_data8;
	T9 m_data9;

};


template<>
class FastVector9<double,double,double,double,double,double,double,double,double>: public BaseObject {
public:
	FastVector9()
	{
		m_data1 = 0;
		m_data2 = 0;
		m_data3 = 0;
		m_data4 = 0;
		m_data5 = 0;
		m_data6 = 0;
		m_data7 = 0;
		m_data8 = 0;
		m_data9 = 0;
	}

	FastVector9(double a, double b, double c, double d, double e, double f, double g, double h, double i)
	{
		m_data1 = a;
		m_data2 = b;
		m_data3 = c;
		m_data4 = d;
		m_data5 = e;
		m_data6 = f;
		m_data7 = g;
		m_data8 = h;
		m_data9 = i;
	}

	virtual ~FastVector9()
	{
	}


	FastVector9<double,double,double,double,double,double,double,double,double> operator +(FastVector9<double,double,double,double,double,double,double,double,double>& b)
	{
		FastVector9<double,double,double,double,double,double,double,double,double> sum;

		sum.m_data1 = m_data1 + b.m_data1;
		sum.m_data2 = m_data2 + b.m_data2;
		sum.m_data3 = m_data3 + b.m_data3;
		sum.m_data4 = m_data4 + b.m_data4;
		sum.m_data5 = m_data5 + b.m_data5;
		sum.m_data6 = m_data6 + b.m_data6;
		sum.m_data7 = m_data7 + b.m_data7;
		sum.m_data8 = m_data8 + b.m_data8;
		sum.m_data9 = m_data9 + b.m_data9;

		return sum;
	}

	FastVector9<double,double,double,double,double,double,double,double,double> operator +(double b)
	{
		FastVector9<double,double,double,double,double,double,double,double,double> sum;

		sum.m_data1 = m_data1 + b;
		sum.m_data2 = m_data2 + b;
		sum.m_data3 = m_data3 + b;
		sum.m_data4 = m_data4 + b;
		sum.m_data5 = m_data5 + b;
		sum.m_data6 = m_data6 + b;
		sum.m_data7 = m_data7 + b;
		sum.m_data8 = m_data8 + b;
		sum.m_data9 = m_data9 + b;

		return sum;
	}

	FastVector9<double,double,double,double,double,double,double,double,double> operator -(FastVector9<double,double,double,double,double,double,double,double,double>& b)
	{
		FastVector9<double,double,double,double,double,double,double,double,double> sum;

		sum.m_data1 = m_data1 - b.m_data1;
		sum.m_data2 = m_data2 - b.m_data2;
		sum.m_data3 = m_data3 - b.m_data3;
		sum.m_data4 = m_data4 - b.m_data4;
		sum.m_data5 = m_data5 - b.m_data5;
		sum.m_data6 = m_data6 - b.m_data6;
		sum.m_data7 = m_data7 - b.m_data7;
		sum.m_data8 = m_data8 - b.m_data8;
		sum.m_data9 = m_data9 - b.m_data9;

		return sum;
	}

	FastVector9<double,double,double,double,double,double,double,double,double> operator -(double b)
	{
		FastVector9<double,double,double,double,double,double,double,double,double> sum;

		sum.m_data1 = m_data1 - b;
		sum.m_data2 = m_data2 - b;
		sum.m_data3 = m_data3 - b;
		sum.m_data4 = m_data4 - b;
		sum.m_data5 = m_data5 - b;
		sum.m_data6 = m_data6 - b;
		sum.m_data7 = m_data7 - b;
		sum.m_data8 = m_data8 - b;
		sum.m_data9 = m_data9 - b;

		return sum;
	}

	double operator *(FastVector9<double,double,double,double,double,double,double,double,double>& b)
	{
		return m_data1 * b.m_data1 + m_data2 * b.m_data2 + m_data3 * b.m_data3 +
				m_data4 * b.m_data4 + m_data5 * b.m_data5 + m_data6 * b.m_data6 +
				m_data7 * b.m_data7 + m_data8 * b.m_data8 +m_data9 * b.m_data9;
	}

	FastVector9<double,double,double,double,double,double,double,double,double> operator *(double b)
	{
		FastVector9<double,double,double,double,double,double,double,double,double> sum;

		sum.m_data1 = m_data1 * b;
		sum.m_data2 = m_data2 * b;
		sum.m_data3 = m_data3 * b;
		sum.m_data4 = m_data4 * b;
		sum.m_data5 = m_data5 * b;
		sum.m_data6 = m_data6 * b;
		sum.m_data7 = m_data7 * b;
		sum.m_data8 = m_data8 * b;
		sum.m_data9 = m_data9 * b;

		return sum;
	}


	double m_data1;
	double m_data2;
	double m_data3;
	double m_data4;
	double m_data5;
	double m_data6;
	double m_data7;
	double m_data8;
	double m_data9;
};


template<>
class FastVector9<float,float,float,float,float,float,float,float,float>: public BaseObject {
public:
	FastVector9()
	{
		m_data1 = 0;
		m_data2 = 0;
		m_data3 = 0;
		m_data4 = 0;
		m_data5 = 0;
		m_data6 = 0;
		m_data7 = 0;
		m_data8 = 0;
		m_data9 = 0;
	}

	FastVector9(float a, float b, float c, float d, float e, float f, float g, float h, float i)
	{
		m_data1 = a;
		m_data2 = b;
		m_data3 = c;
		m_data4 = d;
		m_data5 = e;
		m_data6 = f;
		m_data7 = g;
		m_data8 = h;
		m_data9 = i;
	}

	virtual ~FastVector9()
	{
	}


	FastVector9<float,float,float,float,float,float,float,float,float> operator +(FastVector9<float,float,float,float,float,float,float,float,float>& b)
	{
		FastVector9<float,float,float,float,float,float,float,float,float> sum;

		sum.m_data1 = m_data1 + b.m_data1;
		sum.m_data2 = m_data2 + b.m_data2;
		sum.m_data3 = m_data3 + b.m_data3;
		sum.m_data4 = m_data4 + b.m_data4;
		sum.m_data5 = m_data5 + b.m_data5;
		sum.m_data6 = m_data6 + b.m_data6;
		sum.m_data7 = m_data7 + b.m_data7;
		sum.m_data8 = m_data8 + b.m_data8;
		sum.m_data9 = m_data9 + b.m_data9;

		return sum;
	}

	FastVector9<float,float,float,float,float,float,float,float,float> operator +(float b)
	{
		FastVector9<float,float,float,float,float,float,float,float,float> sum;

		sum.m_data1 = m_data1 + b;
		sum.m_data2 = m_data2 + b;
		sum.m_data3 = m_data3 + b;
		sum.m_data4 = m_data4 + b;
		sum.m_data5 = m_data5 + b;
		sum.m_data6 = m_data6 + b;
		sum.m_data7 = m_data7 + b;
		sum.m_data8 = m_data8 + b;
		sum.m_data9 = m_data9 + b;

		return sum;
	}

	FastVector9<float,float,float,float,float,float,float,float,float> operator -(FastVector9<float,float,float,float,float,float,float,float,float>& b)
	{
		FastVector9<float,float,float,float,float,float,float,float,float> sum;

		sum.m_data1 = m_data1 - b.m_data1;
		sum.m_data2 = m_data2 - b.m_data2;
		sum.m_data3 = m_data3 - b.m_data3;
		sum.m_data4 = m_data4 - b.m_data4;
		sum.m_data5 = m_data5 - b.m_data5;
		sum.m_data6 = m_data6 - b.m_data6;
		sum.m_data7 = m_data7 - b.m_data7;
		sum.m_data8 = m_data8 - b.m_data8;
		sum.m_data9 = m_data9 - b.m_data9;

		return sum;
	}

	FastVector9<float,float,float,float,float,float,float,float,float> operator -(float b)
	{
		FastVector9<float,float,float,float,float,float,float,float,float> sum;

		sum.m_data1 = m_data1 - b;
		sum.m_data2 = m_data2 - b;
		sum.m_data3 = m_data3 - b;
		sum.m_data4 = m_data4 - b;
		sum.m_data5 = m_data5 - b;
		sum.m_data6 = m_data6 - b;
		sum.m_data7 = m_data7 - b;
		sum.m_data8 = m_data8 - b;
		sum.m_data9 = m_data9 - b;

		return sum;
	}

	float operator *(FastVector9<float,float,float,float,float,float,float,float,float>& b)
	{
		return m_data1 * b.m_data1 + m_data2 * b.m_data2 + m_data3 * b.m_data3 +
				m_data4 * b.m_data4 + m_data5 * b.m_data5 + m_data6 * b.m_data6 +
				m_data7 * b.m_data7 + m_data8 * b.m_data8 + m_data9 * b.m_data9 ;
	}

	FastVector9<float,float,float,float,float,float,float,float,float> operator *(float b)
	{
		FastVector9<float,float,float,float,float,float,float,float,float> sum;

		sum.m_data1 = m_data1 * b;
		sum.m_data2 = m_data2 * b;
		sum.m_data3 = m_data3 * b;
		sum.m_data4 = m_data4 * b;
		sum.m_data5 = m_data5 * b;
		sum.m_data6 = m_data6 * b;
		sum.m_data7 = m_data7 * b;
		sum.m_data8 = m_data8 * b;
		sum.m_data9 = m_data9 * b;

		return sum;
	}


	float m_data1;
	float m_data2;
	float m_data3;
	float m_data4;
	float m_data5;
	float m_data6;
	float m_data7;
	float m_data8;
	float m_data9;

};


template<>
class FastVector9<int,int,int,int,int,int,int,int,int>: public BaseObject {
public:
	FastVector9()
	{
		m_data1 = 0;
		m_data2 = 0;
		m_data3 = 0;
		m_data4 = 0;
		m_data5 = 0;
		m_data6 = 0;
		m_data7 = 0;
		m_data8 = 0;
		m_data9 = 0;
	}

	FastVector9(int a, int b, int c, int d, int e, int f, int g, int h, int i)
	{
		m_data1 = a;
		m_data2 = b;
		m_data3 = c;
		m_data4 = d;
		m_data5 = e;
		m_data6 = f;
		m_data7 = g;
		m_data8 = h;
		m_data9 = i;
	}

	virtual ~FastVector9()
	{
	}


	FastVector9<int,int,int,int,int,int,int,int,int> operator +(FastVector9<int,int,int,int,int,int,int,int,int>& b)
	{
		FastVector9<int,int,int,int,int,int,int,int,int> sum;

		sum.m_data1 = m_data1 + b.m_data1;
		sum.m_data2 = m_data2 + b.m_data2;
		sum.m_data3 = m_data3 + b.m_data3;
		sum.m_data4 = m_data4 + b.m_data4;
		sum.m_data5 = m_data5 + b.m_data5;
		sum.m_data6 = m_data6 + b.m_data6;
		sum.m_data7 = m_data7 + b.m_data7;
		sum.m_data8 = m_data8 + b.m_data8;
		sum.m_data9 = m_data9 + b.m_data9;

		return sum;
	}

	FastVector9<int,int,int,int,int,int,int,int,int> operator +(int b)
	{
		FastVector9<int,int,int,int,int,int,int,int,int> sum;

		sum.m_data1 = m_data1 + b;
		sum.m_data2 = m_data2 + b;
		sum.m_data3 = m_data3 + b;
		sum.m_data4 = m_data4 + b;
		sum.m_data5 = m_data5 + b;
		sum.m_data6 = m_data6 + b;
		sum.m_data7 = m_data7 + b;
		sum.m_data8 = m_data8 + b;
		sum.m_data9 = m_data9 + b;

		return sum;
	}

	FastVector9<int,int,int,int,int,int,int,int,int> operator -(FastVector9<int,int,int,int,int,int,int,int,int>& b)
	{
		FastVector9<int,int,int,int,int,int,int,int,int> sum;

		sum.m_data1 = m_data1 - b.m_data1;
		sum.m_data2 = m_data2 - b.m_data2;
		sum.m_data3 = m_data3 - b.m_data3;
		sum.m_data4 = m_data4 - b.m_data4;
		sum.m_data5 = m_data5 - b.m_data5;
		sum.m_data6 = m_data6 - b.m_data6;
		sum.m_data7 = m_data7 - b.m_data7;
		sum.m_data8 = m_data8 - b.m_data8;
		sum.m_data9 = m_data9 - b.m_data9;

		return sum;
	}

	FastVector9<int,int,int,int,int,int,int,int,int> operator -(int b)
	{
		FastVector9<int,int,int,int,int,int,int,int,int> sum;

		sum.m_data1 = m_data1 - b;
		sum.m_data2 = m_data2 - b;
		sum.m_data3 = m_data3 - b;
		sum.m_data4 = m_data4 - b;
		sum.m_data5 = m_data5 - b;
		sum.m_data6 = m_data6 - b;
		sum.m_data7 = m_data7 - b;
		sum.m_data8 = m_data8 - b;
		sum.m_data9 = m_data9 - b;

		return sum;
	}

	int operator *(FastVector9<int,int,int,int,int,int,int,int,int>& b)
	{
		return m_data1 * b.m_data1 + m_data2 * b.m_data2 + m_data3 * b.m_data3 +
				m_data4 * b.m_data4 + m_data5 * b.m_data5 + m_data6 * b.m_data6 +
				m_data7 * b.m_data7 + m_data8 * b.m_data8 + m_data9 * b.m_data9;
	}

	FastVector9<int,int,int,int,int,int,int,int,int> operator *(int b)
	{
		FastVector9<int,int,int,int,int,int,int,int,int> sum;

		sum.m_data1 = m_data1 * b;
		sum.m_data2 = m_data2 * b;
		sum.m_data3 = m_data3 * b;
		sum.m_data4 = m_data4 * b;
		sum.m_data5 = m_data5 * b;
		sum.m_data6 = m_data6 * b;
		sum.m_data7 = m_data7 * b;
		sum.m_data8 = m_data8 * b;
		sum.m_data9 = m_data9 * b;

		return sum;
	}


	int m_data1;
	int m_data2;
	int m_data3;
	int m_data4;
	int m_data5;
	int m_data6;
	int m_data7;
	int m_data8;
	int m_data9;

};



template<class T1 = double, class T2 = double, class T3 = double, class T4 = double,
		class T5 = double, class T6 = double, class T7 = double, class T8 = double>
class FastVector8: public BaseObject {
public:
	FastVector8()
	{
		m_data1 = T1();
		m_data2 = T2();
		m_data3 = T3();
		m_data4 = T4();
		m_data5 = T5();
		m_data6 = T6();
		m_data7 = T7();
		m_data8 = T8();
	}

	FastVector8(const T1& a, const T2& b, const T3& c, const T4& d, const T5& e, const T6& f, const T7& g, const T8& h)
	{
		m_data1 = a;
		m_data2 = b;
		m_data3 = c;
		m_data4 = d;
		m_data5 = e;
		m_data6 = f;
		m_data7 = g;
		m_data8 = h;
	}

	virtual ~FastVector8()
	{
	}


	T1 m_data1;
	T2 m_data2;
	T3 m_data3;
	T4 m_data4;
	T5 m_data5;
	T6 m_data6;
	T7 m_data7;
	T8 m_data8;

};

template<>
class FastVector8<double,double,double,double,double,double,double,double>: public BaseObject {
public:
	FastVector8()
	{
		m_data1 = 0;
		m_data2 = 0;
		m_data3 = 0;
		m_data4 = 0;
		m_data5 = 0;
		m_data6 = 0;
		m_data7 = 0;
		m_data8 = 0;
	}

	FastVector8(double a, double b, double c, double d, double e, double f, double g, double h)
	{
		m_data1 = a;
		m_data2 = b;
		m_data3 = c;
		m_data4 = d;
		m_data5 = e;
		m_data6 = f;
		m_data7 = g;
		m_data8 = h;
	}

	virtual ~FastVector8()
	{
	}


	FastVector8<double,double,double,double,double,double,double,double> operator +(FastVector8<double,double,double,double,double,double,double,double>& b)
	{
		FastVector8<double,double,double,double,double,double,double,double> sum;

		sum.m_data1 = m_data1 + b.m_data1;
		sum.m_data2 = m_data2 + b.m_data2;
		sum.m_data3 = m_data3 + b.m_data3;
		sum.m_data4 = m_data4 + b.m_data4;
		sum.m_data5 = m_data5 + b.m_data5;
		sum.m_data6 = m_data6 + b.m_data6;
		sum.m_data7 = m_data7 + b.m_data7;
		sum.m_data8 = m_data8 + b.m_data8;

		return sum;
	}

	FastVector8<double,double,double,double,double,double,double,double> operator +(double b)
	{
		FastVector8<double,double,double,double,double,double,double,double> sum;

		sum.m_data1 = m_data1 + b;
		sum.m_data2 = m_data2 + b;
		sum.m_data3 = m_data3 + b;
		sum.m_data4 = m_data4 + b;
		sum.m_data5 = m_data5 + b;
		sum.m_data6 = m_data6 + b;
		sum.m_data7 = m_data7 + b;
		sum.m_data8 = m_data8 + b;

		return sum;
	}

	FastVector8<double,double,double,double,double,double,double,double> operator -(FastVector8<double,double,double,double,double,double,double,double>& b)
	{
		FastVector8<double,double,double,double,double,double,double,double> sum;

		sum.m_data1 = m_data1 - b.m_data1;
		sum.m_data2 = m_data2 - b.m_data2;
		sum.m_data3 = m_data3 - b.m_data3;
		sum.m_data4 = m_data4 - b.m_data4;
		sum.m_data5 = m_data5 - b.m_data5;
		sum.m_data6 = m_data6 - b.m_data6;
		sum.m_data7 = m_data7 - b.m_data7;
		sum.m_data8 = m_data8 - b.m_data8;

		return sum;
	}

	FastVector8<double,double,double,double,double,double,double,double> operator -(double b)
	{
		FastVector8<double,double,double,double,double,double,double,double> sum;

		sum.m_data1 = m_data1 - b;
		sum.m_data2 = m_data2 - b;
		sum.m_data3 = m_data3 - b;
		sum.m_data4 = m_data4 - b;
		sum.m_data5 = m_data5 - b;
		sum.m_data6 = m_data6 - b;
		sum.m_data7 = m_data7 - b;
		sum.m_data8 = m_data8 - b;

		return sum;
	}

	double operator *(FastVector8<double,double,double,double,double,double,double,double>& b)
	{
		return m_data1 * b.m_data1 + m_data2 * b.m_data2 + m_data3 * b.m_data3 +
				m_data4 * b.m_data4 + m_data5 * b.m_data5 + m_data6 * b.m_data6 +
				m_data7 * b.m_data7 + m_data8 * b.m_data8 ;
	}

	FastVector8<double,double,double,double,double,double,double,double> operator *(double b)
	{
		FastVector8<double,double,double,double,double,double,double,double> sum;

		sum.m_data1 = m_data1 * b;
		sum.m_data2 = m_data2 * b;
		sum.m_data3 = m_data3 * b;
		sum.m_data4 = m_data4 * b;
		sum.m_data5 = m_data5 * b;
		sum.m_data6 = m_data6 * b;
		sum.m_data7 = m_data7 * b;
		sum.m_data8 = m_data8 * b;

		return sum;
	}


	double m_data1;
	double m_data2;
	double m_data3;
	double m_data4;
	double m_data5;
	double m_data6;
	double m_data7;
	double m_data8;

};


template<>
class FastVector8<float,float,float,float,float,float,float,float>: public BaseObject {
public:
	FastVector8()
	{
		m_data1 = 0;
		m_data2 = 0;
		m_data3 = 0;
		m_data4 = 0;
		m_data5 = 0;
		m_data6 = 0;
		m_data7 = 0;
		m_data8 = 0;
	}

	FastVector8(float a, float b, float c, float d, float e, float f, float g, float h)
	{
		m_data1 = a;
		m_data2 = b;
		m_data3 = c;
		m_data4 = d;
		m_data5 = e;
		m_data6 = f;
		m_data7 = g;
		m_data8 = h;
	}

	virtual ~FastVector8()
	{
	}


	FastVector8<float,float,float,float,float,float,float,float> operator +(FastVector8<float,float,float,float,float,float,float,float>& b)
	{
		FastVector8<float,float,float,float,float,float,float,float> sum;

		sum.m_data1 = m_data1 + b.m_data1;
		sum.m_data2 = m_data2 + b.m_data2;
		sum.m_data3 = m_data3 + b.m_data3;
		sum.m_data4 = m_data4 + b.m_data4;
		sum.m_data5 = m_data5 + b.m_data5;
		sum.m_data6 = m_data6 + b.m_data6;
		sum.m_data7 = m_data7 + b.m_data7;
		sum.m_data8 = m_data8 + b.m_data8;

		return sum;
	}

	FastVector8<float,float,float,float,float,float,float,float> operator +(float b)
	{
		FastVector8<float,float,float,float,float,float,float,float> sum;

		sum.m_data1 = m_data1 + b;
		sum.m_data2 = m_data2 + b;
		sum.m_data3 = m_data3 + b;
		sum.m_data4 = m_data4 + b;
		sum.m_data5 = m_data5 + b;
		sum.m_data6 = m_data6 + b;
		sum.m_data7 = m_data7 + b;
		sum.m_data8 = m_data8 + b;

		return sum;
	}

	FastVector8<float,float,float,float,float,float,float,float> operator -(FastVector8<float,float,float,float,float,float,float,float>& b)
	{
		FastVector8<float,float,float,float,float,float,float,float> sum;

		sum.m_data1 = m_data1 - b.m_data1;
		sum.m_data2 = m_data2 - b.m_data2;
		sum.m_data3 = m_data3 - b.m_data3;
		sum.m_data4 = m_data4 - b.m_data4;
		sum.m_data5 = m_data5 - b.m_data5;
		sum.m_data6 = m_data6 - b.m_data6;
		sum.m_data7 = m_data7 - b.m_data7;
		sum.m_data8 = m_data8 - b.m_data8;

		return sum;
	}

	FastVector8<float,float,float,float,float,float,float,float> operator -(float b)
	{
		FastVector8<float,float,float,float,float,float,float,float> sum;

		sum.m_data1 = m_data1 - b;
		sum.m_data2 = m_data2 - b;
		sum.m_data3 = m_data3 - b;
		sum.m_data4 = m_data4 - b;
		sum.m_data5 = m_data5 - b;
		sum.m_data6 = m_data6 - b;
		sum.m_data7 = m_data7 - b;
		sum.m_data8 = m_data8 - b;

		return sum;
	}

	float operator *(FastVector8<float,float,float,float,float,float,float,float>& b)
	{
		return m_data1 * b.m_data1 + m_data2 * b.m_data2 + m_data3 * b.m_data3 +
				m_data4 * b.m_data4 + m_data5 * b.m_data5 + m_data6 * b.m_data6 +
				m_data7 * b.m_data7 + m_data8 * b.m_data8 ;
	}

	FastVector8<float,float,float,float,float,float,float,float> operator *(float b)
	{
		FastVector8<float,float,float,float,float,float,float,float> sum;

		sum.m_data1 = m_data1 * b;
		sum.m_data2 = m_data2 * b;
		sum.m_data3 = m_data3 * b;
		sum.m_data4 = m_data4 * b;
		sum.m_data5 = m_data5 * b;
		sum.m_data6 = m_data6 * b;
		sum.m_data7 = m_data7 * b;
		sum.m_data8 = m_data8 * b;

		return sum;
	}


	float m_data1;
	float m_data2;
	float m_data3;
	float m_data4;
	float m_data5;
	float m_data6;
	float m_data7;
	float m_data8;

};


template<>
class FastVector8<int,int,int,int,int,int,int,int>: public BaseObject {
public:
	FastVector8()
	{
		m_data1 = 0;
		m_data2 = 0;
		m_data3 = 0;
		m_data4 = 0;
		m_data5 = 0;
		m_data6 = 0;
		m_data7 = 0;
		m_data8 = 0;
	}

	FastVector8(int a, int b, int c, int d, int e, int f, int g, int h)
	{
		m_data1 = a;
		m_data2 = b;
		m_data3 = c;
		m_data4 = d;
		m_data5 = e;
		m_data6 = f;
		m_data7 = g;
		m_data8 = h;
	}

	virtual ~FastVector8()
	{
	}


	FastVector8<int,int,int,int,int,int,int,int> operator +(FastVector8<int,int,int,int,int,int,int,int>& b)
	{
		FastVector8<int,int,int,int,int,int,int,int> sum;

		sum.m_data1 = m_data1 + b.m_data1;
		sum.m_data2 = m_data2 + b.m_data2;
		sum.m_data3 = m_data3 + b.m_data3;
		sum.m_data4 = m_data4 + b.m_data4;
		sum.m_data5 = m_data5 + b.m_data5;
		sum.m_data6 = m_data6 + b.m_data6;
		sum.m_data7 = m_data7 + b.m_data7;
		sum.m_data8 = m_data8 + b.m_data8;

		return sum;
	}

	FastVector8<int,int,int,int,int,int,int,int> operator +(int b)
	{
		FastVector8<int,int,int,int,int,int,int,int> sum;

		sum.m_data1 = m_data1 + b;
		sum.m_data2 = m_data2 + b;
		sum.m_data3 = m_data3 + b;
		sum.m_data4 = m_data4 + b;
		sum.m_data5 = m_data5 + b;
		sum.m_data6 = m_data6 + b;
		sum.m_data7 = m_data7 + b;
		sum.m_data8 = m_data8 + b;

		return sum;
	}

	FastVector8<int,int,int,int,int,int,int,int> operator -(FastVector8<int,int,int,int,int,int,int,int>& b)
	{
		FastVector8<int,int,int,int,int,int,int,int> sum;

		sum.m_data1 = m_data1 - b.m_data1;
		sum.m_data2 = m_data2 - b.m_data2;
		sum.m_data3 = m_data3 - b.m_data3;
		sum.m_data4 = m_data4 - b.m_data4;
		sum.m_data5 = m_data5 - b.m_data5;
		sum.m_data6 = m_data6 - b.m_data6;
		sum.m_data7 = m_data7 - b.m_data7;
		sum.m_data8 = m_data8 - b.m_data8;

		return sum;
	}

	FastVector8<int,int,int,int,int,int,int,int> operator -(int b)
	{
		FastVector8<int,int,int,int,int,int,int,int> sum;

		sum.m_data1 = m_data1 - b;
		sum.m_data2 = m_data2 - b;
		sum.m_data3 = m_data3 - b;
		sum.m_data4 = m_data4 - b;
		sum.m_data5 = m_data5 - b;
		sum.m_data6 = m_data6 - b;
		sum.m_data7 = m_data7 - b;
		sum.m_data8 = m_data8 - b;

		return sum;
	}

	int operator *(FastVector8<int,int,int,int,int,int,int,int>& b)
	{
		return m_data1 * b.m_data1 + m_data2 * b.m_data2 + m_data3 * b.m_data3 +
				m_data4 * b.m_data4 + m_data5 * b.m_data5 + m_data6 * b.m_data6 +
				m_data7 * b.m_data7 + m_data8 * b.m_data8 ;
	}

	FastVector8<int,int,int,int,int,int,int,int> operator *(int b)
	{
		FastVector8<int,int,int,int,int,int,int,int> sum;

		sum.m_data1 = m_data1 * b;
		sum.m_data2 = m_data2 * b;
		sum.m_data3 = m_data3 * b;
		sum.m_data4 = m_data4 * b;
		sum.m_data5 = m_data5 * b;
		sum.m_data6 = m_data6 * b;
		sum.m_data7 = m_data7 * b;
		sum.m_data8 = m_data8 * b;

		return sum;
	}


	int m_data1;
	int m_data2;
	int m_data3;
	int m_data4;
	int m_data5;
	int m_data6;
	int m_data7;
	int m_data8;

};


template<class T1 = double, class T2 = double, class T3 = double, class T4 = double,
		class T5 = double, class T6 = double, class T7 = double>
class FastVector7: public BaseObject {
public:
	FastVector7()
	{
		m_data1 = T1();
		m_data2 = T2();
		m_data3 = T3();
		m_data4 = T4();
		m_data5 = T5();
		m_data6 = T6();
		m_data7 = T7();
	}

	virtual ~FastVector7()
	{
	}

	FastVector7(const T1& a, const T2& b, const T3& c, const T4& d, const T5& e, const T6& f, const T7& g)
	{
		m_data1 = a;
		m_data2 = b;
		m_data3 = c;
		m_data4 = d;
		m_data5 = e;
		m_data6 = f;
		m_data7 = g;

	}

	T1 m_data1;
	T2 m_data2;
	T3 m_data3;
	T4 m_data4;
	T5 m_data5;
	T6 m_data6;
	T7 m_data7;

};


template<>
class FastVector7<double,double,double,double,double,double,double>: public BaseObject {
public:
	FastVector7()
	{
		m_data1 = 0;
		m_data2 = 0;
		m_data3 = 0;
		m_data4 = 0;
		m_data5 = 0;
		m_data6 = 0;
		m_data7 = 0;
	}

	FastVector7(double a, double b, double c, double d, double e, double f, double g)
	{
		m_data1 = a;
		m_data2 = b;
		m_data3 = c;
		m_data4 = d;
		m_data5 = e;
		m_data6 = f;
		m_data7 = g;
	}

	virtual ~FastVector7()
	{
	}


	FastVector7<double,double,double,double,double,double,double> operator +(FastVector7<double,double,double,double,double,double,double>& b)
	{
		FastVector7<double,double,double,double,double,double,double> sum;

		sum.m_data1 = m_data1 + b.m_data1;
		sum.m_data2 = m_data2 + b.m_data2;
		sum.m_data3 = m_data3 + b.m_data3;
		sum.m_data4 = m_data4 + b.m_data4;
		sum.m_data5 = m_data5 + b.m_data5;
		sum.m_data6 = m_data6 + b.m_data6;
		sum.m_data7 = m_data7 + b.m_data7;

		return sum;
	}

	FastVector7<double,double,double,double,double,double,double> operator +(double b)
	{
		FastVector7<double,double,double,double,double,double,double> sum;

		sum.m_data1 = m_data1 + b;
		sum.m_data2 = m_data2 + b;
		sum.m_data3 = m_data3 + b;
		sum.m_data4 = m_data4 + b;
		sum.m_data5 = m_data5 + b;
		sum.m_data6 = m_data6 + b;
		sum.m_data7 = m_data7 + b;

		return sum;
	}

	FastVector7<double,double,double,double,double,double,double> operator -(FastVector7<double,double,double,double,double,double,double>& b)
	{
		FastVector7<double,double,double,double,double,double,double> sum;

		sum.m_data1 = m_data1 - b.m_data1;
		sum.m_data2 = m_data2 - b.m_data2;
		sum.m_data3 = m_data3 - b.m_data3;
		sum.m_data4 = m_data4 - b.m_data4;
		sum.m_data5 = m_data5 - b.m_data5;
		sum.m_data6 = m_data6 - b.m_data6;
		sum.m_data7 = m_data7 - b.m_data7;

		return sum;
	}

	FastVector7<double,double,double,double,double,double,double> operator -(double b)
	{
		FastVector7<double,double,double,double,double,double,double> sum;

		sum.m_data1 = m_data1 - b;
		sum.m_data2 = m_data2 - b;
		sum.m_data3 = m_data3 - b;
		sum.m_data4 = m_data4 - b;
		sum.m_data5 = m_data5 - b;
		sum.m_data6 = m_data6 - b;
		sum.m_data7 = m_data7 - b;

		return sum;
	}

	double operator *(FastVector7<double,double,double,double,double,double,double>& b)
	{
		return m_data1 * b.m_data1 + m_data2 * b.m_data2 + m_data3 * b.m_data3 +
				m_data4 * b.m_data4 + m_data5 * b.m_data5 + m_data6 * b.m_data6 +
				m_data7 * b.m_data7 ;
	}

	FastVector7<double,double,double,double,double,double,double> operator *(double b)
	{
		FastVector7<double,double,double,double,double,double,double> sum;

		sum.m_data1 = m_data1 * b;
		sum.m_data2 = m_data2 * b;
		sum.m_data3 = m_data3 * b;
		sum.m_data4 = m_data4 * b;
		sum.m_data5 = m_data5 * b;
		sum.m_data6 = m_data6 * b;
		sum.m_data7 = m_data7 * b;

		return sum;
	}


	double m_data1;
	double m_data2;
	double m_data3;
	double m_data4;
	double m_data5;
	double m_data6;
	double m_data7;

};


template<>
class FastVector7<float,float,float,float,float,float,float>: public BaseObject {
public:
	FastVector7()
	{
		m_data1 = 0;
		m_data2 = 0;
		m_data3 = 0;
		m_data4 = 0;
		m_data5 = 0;
		m_data6 = 0;
		m_data7 = 0;
	}

	FastVector7(float a, float b, float c, float d, float e, float f, float g)
	{
		m_data1 = a;
		m_data2 = b;
		m_data3 = c;
		m_data4 = d;
		m_data5 = e;
		m_data6 = f;
		m_data7 = g;
	}

	virtual ~FastVector7()
	{
	}


	FastVector7<float,float,float,float,float,float,float> operator +(FastVector7<float,float,float,float,float,float,float>& b)
	{
		FastVector7<float,float,float,float,float,float,float> sum;

		sum.m_data1 = m_data1 + b.m_data1;
		sum.m_data2 = m_data2 + b.m_data2;
		sum.m_data3 = m_data3 + b.m_data3;
		sum.m_data4 = m_data4 + b.m_data4;
		sum.m_data5 = m_data5 + b.m_data5;
		sum.m_data6 = m_data6 + b.m_data6;
		sum.m_data7 = m_data7 + b.m_data7;

		return sum;
	}

	FastVector7<float,float,float,float,float,float,float> operator +(float b)
	{
		FastVector7<float,float,float,float,float,float,float> sum;

		sum.m_data1 = m_data1 + b;
		sum.m_data2 = m_data2 + b;
		sum.m_data3 = m_data3 + b;
		sum.m_data4 = m_data4 + b;
		sum.m_data5 = m_data5 + b;
		sum.m_data6 = m_data6 + b;
		sum.m_data7 = m_data7 + b;

		return sum;
	}

	FastVector7<float,float,float,float,float,float,float> operator -(FastVector7<float,float,float,float,float,float,float>& b)
	{
		FastVector7<float,float,float,float,float,float,float> sum;

		sum.m_data1 = m_data1 - b.m_data1;
		sum.m_data2 = m_data2 - b.m_data2;
		sum.m_data3 = m_data3 - b.m_data3;
		sum.m_data4 = m_data4 - b.m_data4;
		sum.m_data5 = m_data5 - b.m_data5;
		sum.m_data6 = m_data6 - b.m_data6;
		sum.m_data7 = m_data7 - b.m_data7;

		return sum;
	}

	FastVector7<float,float,float,float,float,float,float> operator -(float b)
	{
		FastVector7<float,float,float,float,float,float,float> sum;

		sum.m_data1 = m_data1 - b;
		sum.m_data2 = m_data2 - b;
		sum.m_data3 = m_data3 - b;
		sum.m_data4 = m_data4 - b;
		sum.m_data5 = m_data5 - b;
		sum.m_data6 = m_data6 - b;
		sum.m_data7 = m_data7 - b;

		return sum;
	}

	float operator *(FastVector7<float,float,float,float,float,float,float>& b)
	{
		return m_data1 * b.m_data1 + m_data2 * b.m_data2 + m_data3 * b.m_data3 +
				m_data4 * b.m_data4 + m_data5 * b.m_data5 + m_data6 * b.m_data6 +
				m_data7 * b.m_data7 ;
	}

	FastVector7<float,float,float,float,float,float,float> operator *(float b)
	{
		FastVector7<float,float,float,float,float,float,float> sum;

		sum.m_data1 = m_data1 * b;
		sum.m_data2 = m_data2 * b;
		sum.m_data3 = m_data3 * b;
		sum.m_data4 = m_data4 * b;
		sum.m_data5 = m_data5 * b;
		sum.m_data6 = m_data6 * b;
		sum.m_data7 = m_data7 * b;

		return sum;
	}


	float m_data1;
	float m_data2;
	float m_data3;
	float m_data4;
	float m_data5;
	float m_data6;
	float m_data7;

};


template<>
class FastVector7<int,int,int,int,int,int,int>: public BaseObject {
public:
	FastVector7()
	{
		m_data1 = 0;
		m_data2 = 0;
		m_data3 = 0;
		m_data4 = 0;
		m_data5 = 0;
		m_data6 = 0;
		m_data7 = 0;
	}

	FastVector7(int a, int b, int c, int d, int e, int f, int g)
	{
		m_data1 = a;
		m_data2 = b;
		m_data3 = c;
		m_data4 = d;
		m_data5 = e;
		m_data6 = f;
		m_data7 = g;
	}

	virtual ~FastVector7()
	{
	}


	FastVector7<int,int,int,int,int,int,int> operator +(FastVector7<int,int,int,int,int,int,int>& b)
	{
		FastVector7<int,int,int,int,int,int,int> sum;

		sum.m_data1 = m_data1 + b.m_data1;
		sum.m_data2 = m_data2 + b.m_data2;
		sum.m_data3 = m_data3 + b.m_data3;
		sum.m_data4 = m_data4 + b.m_data4;
		sum.m_data5 = m_data5 + b.m_data5;
		sum.m_data6 = m_data6 + b.m_data6;
		sum.m_data7 = m_data7 + b.m_data7;

		return sum;
	}

	FastVector7<int,int,int,int,int,int,int> operator +(int b)
	{
		FastVector7<int,int,int,int,int,int,int> sum;

		sum.m_data1 = m_data1 + b;
		sum.m_data2 = m_data2 + b;
		sum.m_data3 = m_data3 + b;
		sum.m_data4 = m_data4 + b;
		sum.m_data5 = m_data5 + b;
		sum.m_data6 = m_data6 + b;
		sum.m_data7 = m_data7 + b;

		return sum;
	}

	FastVector7<int,int,int,int,int,int,int> operator -(FastVector7<int,int,int,int,int,int,int>& b)
	{
		FastVector7<int,int,int,int,int,int,int> sum;

		sum.m_data1 = m_data1 - b.m_data1;
		sum.m_data2 = m_data2 - b.m_data2;
		sum.m_data3 = m_data3 - b.m_data3;
		sum.m_data4 = m_data4 - b.m_data4;
		sum.m_data5 = m_data5 - b.m_data5;
		sum.m_data6 = m_data6 - b.m_data6;
		sum.m_data7 = m_data7 - b.m_data7;

		return sum;
	}

	FastVector7<int,int,int,int,int,int,int> operator -(int b)
	{
		FastVector7<int,int,int,int,int,int,int> sum;

		sum.m_data1 = m_data1 - b;
		sum.m_data2 = m_data2 - b;
		sum.m_data3 = m_data3 - b;
		sum.m_data4 = m_data4 - b;
		sum.m_data5 = m_data5 - b;
		sum.m_data6 = m_data6 - b;
		sum.m_data7 = m_data7 - b;

		return sum;
	}

	int operator *(FastVector7<int,int,int,int,int,int,int>& b)
	{
		return m_data1 * b.m_data1 + m_data2 * b.m_data2 + m_data3 * b.m_data3 +
				m_data4 * b.m_data4 + m_data5 * b.m_data5 + m_data6 * b.m_data6 +
				m_data7 * b.m_data7 ;
	}

	FastVector7<int,int,int,int,int,int,int> operator *(int b)
	{
		FastVector7<int,int,int,int,int,int,int> sum;

		sum.m_data1 = m_data1 * b;
		sum.m_data2 = m_data2 * b;
		sum.m_data3 = m_data3 * b;
		sum.m_data4 = m_data4 * b;
		sum.m_data5 = m_data5 * b;
		sum.m_data6 = m_data6 * b;
		sum.m_data7 = m_data7 * b;

		return sum;
	}


	int m_data1;
	int m_data2;
	int m_data3;
	int m_data4;
	int m_data5;
	int m_data6;
	int m_data7;

};

template<class T1 = double, class T2 = double, class T3 = double, class T4 = double,
		class T5 = double, class T6 = double>
class FastVector6: public BaseObject {
public:
	FastVector6()
	{
		m_data1 = T1();
		m_data2 = T2();
		m_data3 = T3();
		m_data4 = T4();
		m_data5 = T5();
		m_data6 = T6();

	}

	FastVector6(const T1& a, const T2& b, const T3& c, const T4& d, const T5& e, const T6& f)
	{
		m_data1 = a;
		m_data2 = b;
		m_data3 = c;
		m_data4 = d;
		m_data5 = e;
		m_data6 = f;
	}

	virtual ~FastVector6()
	{
	}


	T1 m_data1;
	T2 m_data2;
	T3 m_data3;
	T4 m_data4;
	T5 m_data5;
	T6 m_data6;

};


template<>
class FastVector6<double,double,double,double,double,double>: public BaseObject {
public:
	FastVector6()
	{
		m_data1 = 0;
		m_data2 = 0;
		m_data3 = 0;
		m_data4 = 0;
		m_data5 = 0;
		m_data6 = 0;
	}

	FastVector6(double a, double b, double c, double d, double e, double f)
	{
		m_data1 = a;
		m_data2 = b;
		m_data3 = c;
		m_data4 = d;
		m_data5 = e;
		m_data6 = f;
	}

	virtual ~FastVector6()
	{
	}


	FastVector6<double,double,double,double,double,double> operator +(FastVector6<double,double,double,double,double,double>& b)
	{
		FastVector6<double,double,double,double,double,double> sum;

		sum.m_data1 = m_data1 + b.m_data1;
		sum.m_data2 = m_data2 + b.m_data2;
		sum.m_data3 = m_data3 + b.m_data3;
		sum.m_data4 = m_data4 + b.m_data4;
		sum.m_data5 = m_data5 + b.m_data5;
		sum.m_data6 = m_data6 + b.m_data6;

		return sum;
	}

	FastVector6<double,double,double,double,double,double> operator +(double b)
	{
		FastVector6<double,double,double,double,double,double> sum;

		sum.m_data1 = m_data1 + b;
		sum.m_data2 = m_data2 + b;
		sum.m_data3 = m_data3 + b;
		sum.m_data4 = m_data4 + b;
		sum.m_data5 = m_data5 + b;
		sum.m_data6 = m_data6 + b;

		return sum;
	}

	FastVector6<double,double,double,double,double,double> operator -(FastVector6<double,double,double,double,double,double>& b)
	{
		FastVector6<double,double,double,double,double,double> sum;

		sum.m_data1 = m_data1 - b.m_data1;
		sum.m_data2 = m_data2 - b.m_data2;
		sum.m_data3 = m_data3 - b.m_data3;
		sum.m_data4 = m_data4 - b.m_data4;
		sum.m_data5 = m_data5 - b.m_data5;
		sum.m_data6 = m_data6 - b.m_data6;

		return sum;
	}

	FastVector6<double,double,double,double,double,double> operator -(double b)
	{
		FastVector6<double,double,double,double,double,double> sum;

		sum.m_data1 = m_data1 - b;
		sum.m_data2 = m_data2 - b;
		sum.m_data3 = m_data3 - b;
		sum.m_data4 = m_data4 - b;
		sum.m_data5 = m_data5 - b;
		sum.m_data6 = m_data6 - b;

		return sum;
	}

	double operator *(FastVector6<double,double,double,double,double,double>& b)
	{
		return m_data1 * b.m_data1 + m_data2 * b.m_data2 + m_data3 * b.m_data3 +
				m_data4 * b.m_data4 + m_data5 * b.m_data5 + m_data6 * b.m_data6 ;
	}

	FastVector6<double,double,double,double,double,double> operator *(double b)
	{
		FastVector6<double,double,double,double,double,double> sum;

		sum.m_data1 = m_data1 * b;
		sum.m_data2 = m_data2 * b;
		sum.m_data3 = m_data3 * b;
		sum.m_data4 = m_data4 * b;
		sum.m_data5 = m_data5 * b;
		sum.m_data6 = m_data6 * b;

		return sum;
	}


	double m_data1;
	double m_data2;
	double m_data3;
	double m_data4;
	double m_data5;
	double m_data6;

};


template<>
class FastVector6<float,float,float,float,float,float>: public BaseObject {
public:
	FastVector6()
	{
		m_data1 = 0;
		m_data2 = 0;
		m_data3 = 0;
		m_data4 = 0;
		m_data5 = 0;
		m_data6 = 0;
	}

	FastVector6(float a, float b, float c, float d, float e, float f)
	{
		m_data1 = a;
		m_data2 = b;
		m_data3 = c;
		m_data4 = d;
		m_data5 = e;
		m_data6 = f;
	}

	virtual ~FastVector6()
	{
	}


	FastVector6<float,float,float,float,float,float> operator +(FastVector6<float,float,float,float,float,float>& b)
	{
		FastVector6<float,float,float,float,float,float> sum;

		sum.m_data1 = m_data1 + b.m_data1;
		sum.m_data2 = m_data2 + b.m_data2;
		sum.m_data3 = m_data3 + b.m_data3;
		sum.m_data4 = m_data4 + b.m_data4;
		sum.m_data5 = m_data5 + b.m_data5;
		sum.m_data6 = m_data6 + b.m_data6;

		return sum;
	}

	FastVector6<float,float,float,float,float,float> operator +(float b)
	{
		FastVector6<float,float,float,float,float,float> sum;

		sum.m_data1 = m_data1 + b;
		sum.m_data2 = m_data2 + b;
		sum.m_data3 = m_data3 + b;
		sum.m_data4 = m_data4 + b;
		sum.m_data5 = m_data5 + b;
		sum.m_data6 = m_data6 + b;

		return sum;
	}

	FastVector6<float,float,float,float,float,float> operator -(FastVector6<float,float,float,float,float,float>& b)
	{
		FastVector6<float,float,float,float,float,float> sum;

		sum.m_data1 = m_data1 - b.m_data1;
		sum.m_data2 = m_data2 - b.m_data2;
		sum.m_data3 = m_data3 - b.m_data3;
		sum.m_data4 = m_data4 - b.m_data4;
		sum.m_data5 = m_data5 - b.m_data5;
		sum.m_data6 = m_data6 - b.m_data6;

		return sum;
	}

	FastVector6<float,float,float,float,float,float> operator -(float b)
	{
		FastVector6<float,float,float,float,float,float> sum;

		sum.m_data1 = m_data1 - b;
		sum.m_data2 = m_data2 - b;
		sum.m_data3 = m_data3 - b;
		sum.m_data4 = m_data4 - b;
		sum.m_data5 = m_data5 - b;
		sum.m_data6 = m_data6 - b;

		return sum;
	}

	float operator *(FastVector6<float,float,float,float,float,float>& b)
	{
		return m_data1 * b.m_data1 + m_data2 * b.m_data2 + m_data3 * b.m_data3 +
				m_data4 * b.m_data4 + m_data5 * b.m_data5 + m_data6 * b.m_data6 ;
	}

	FastVector6<float,float,float,float,float,float> operator *(float b)
	{
		FastVector6<float,float,float,float,float,float> sum;

		sum.m_data1 = m_data1 * b;
		sum.m_data2 = m_data2 * b;
		sum.m_data3 = m_data3 * b;
		sum.m_data4 = m_data4 * b;
		sum.m_data5 = m_data5 * b;
		sum.m_data6 = m_data6 * b;

		return sum;
	}


	float m_data1;
	float m_data2;
	float m_data3;
	float m_data4;
	float m_data5;
	float m_data6;

};


template<>
class FastVector6<int,int,int,int,int,int>: public BaseObject {
public:
	FastVector6()
	{
		m_data1 = 0;
		m_data2 = 0;
		m_data3 = 0;
		m_data4 = 0;
		m_data5 = 0;
		m_data6 = 0;
	}

	FastVector6(int a, int b, int c, int d, int e, int f)
	{
		m_data1 = a;
		m_data2 = b;
		m_data3 = c;
		m_data4 = d;
		m_data5 = e;
		m_data6 = f;
	}

	virtual ~FastVector6()
	{
	}


	FastVector6<int,int,int,int,int,int> operator +(FastVector6<int,int,int,int,int,int>& b)
	{
		FastVector6<int,int,int,int,int,int> sum;

		sum.m_data1 = m_data1 + b.m_data1;
		sum.m_data2 = m_data2 + b.m_data2;
		sum.m_data3 = m_data3 + b.m_data3;
		sum.m_data4 = m_data4 + b.m_data4;
		sum.m_data5 = m_data5 + b.m_data5;
		sum.m_data6 = m_data6 + b.m_data6;

		return sum;
	}

	FastVector6<int,int,int,int,int,int> operator +(int b)
	{
		FastVector6<int,int,int,int,int,int> sum;

		sum.m_data1 = m_data1 + b;
		sum.m_data2 = m_data2 + b;
		sum.m_data3 = m_data3 + b;
		sum.m_data4 = m_data4 + b;
		sum.m_data5 = m_data5 + b;
		sum.m_data6 = m_data6 + b;

		return sum;
	}

	FastVector6<int,int,int,int,int,int> operator -(FastVector6<int,int,int,int,int,int>& b)
	{
		FastVector6<int,int,int,int,int,int> sum;

		sum.m_data1 = m_data1 - b.m_data1;
		sum.m_data2 = m_data2 - b.m_data2;
		sum.m_data3 = m_data3 - b.m_data3;
		sum.m_data4 = m_data4 - b.m_data4;
		sum.m_data5 = m_data5 - b.m_data5;
		sum.m_data6 = m_data6 - b.m_data6;

		return sum;
	}

	FastVector6<int,int,int,int,int,int> operator -(int b)
	{
		FastVector6<int,int,int,int,int,int> sum;

		sum.m_data1 = m_data1 - b;
		sum.m_data2 = m_data2 - b;
		sum.m_data3 = m_data3 - b;
		sum.m_data4 = m_data4 - b;
		sum.m_data5 = m_data5 - b;
		sum.m_data6 = m_data6 - b;

		return sum;
	}

	int operator *(FastVector6<int,int,int,int,int,int>& b)
	{
		return m_data1 * b.m_data1 + m_data2 * b.m_data2 + m_data3 * b.m_data3 +
				m_data4 * b.m_data4 + m_data5 * b.m_data5 + m_data6 * b.m_data6 ;
	}

	FastVector6<int,int,int,int,int,int> operator *(int b)
	{
		FastVector6<int,int,int,int,int,int> sum;

		sum.m_data1 = m_data1 * b;
		sum.m_data2 = m_data2 * b;
		sum.m_data3 = m_data3 * b;
		sum.m_data4 = m_data4 * b;
		sum.m_data5 = m_data5 * b;
		sum.m_data6 = m_data6 * b;

		return sum;
	}


	int m_data1;
	int m_data2;
	int m_data3;
	int m_data4;
	int m_data5;
	int m_data6;

};

template<class T1 = double, class T2 = double, class T3 = double, class T4 = double, class T5 = double>
class FastVector5: public BaseObject {
public:
	FastVector5()
	{
		m_data1 = T1();
		m_data2 = T2();
		m_data3 = T3();
		m_data4 = T4();
		m_data5 = T5();

	}

	FastVector5(const T1& a, const T2& b, const T3& c, const T4& d, const T5& e)
	{
		m_data1 = a;
		m_data2 = b;
		m_data3 = c;
		m_data4 = d;
		m_data5 = e;
	}

	virtual ~FastVector5()
	{
	}

	T1 m_data1;
	T2 m_data2;
	T3 m_data3;
	T4 m_data4;
	T5 m_data5;

};


template<>
class FastVector5<double,double,double,double,double>: public BaseObject {
public:
	FastVector5()
	{
		m_data1 = 0;
		m_data2 = 0;
		m_data3 = 0;
		m_data4 = 0;
		m_data5 = 0;
	}

	FastVector5(double a, double b, double c, double d, double e)
	{
		m_data1 = a;
		m_data2 = b;
		m_data3 = c;
		m_data4 = d;
		m_data5 = e;
	}

	virtual ~FastVector5()
	{
	}


	FastVector5<double,double,double,double,double> operator +(FastVector5<double,double,double,double,double>& b)
	{
		FastVector5<double,double,double,double,double> sum;

		sum.m_data1 = m_data1 + b.m_data1;
		sum.m_data2 = m_data2 + b.m_data2;
		sum.m_data3 = m_data3 + b.m_data3;
		sum.m_data4 = m_data4 + b.m_data4;
		sum.m_data5 = m_data5 + b.m_data5;

		return sum;
	}

	FastVector5<double,double,double,double,double> operator +(double b)
	{
		FastVector5<double,double,double,double,double> sum;

		sum.m_data1 = m_data1 + b;
		sum.m_data2 = m_data2 + b;
		sum.m_data3 = m_data3 + b;
		sum.m_data4 = m_data4 + b;
		sum.m_data5 = m_data5 + b;

		return sum;
	}

	FastVector5<double,double,double,double,double> operator -(FastVector5<double,double,double,double,double>& b)
	{
		FastVector5<double,double,double,double,double> sum;

		sum.m_data1 = m_data1 - b.m_data1;
		sum.m_data2 = m_data2 - b.m_data2;
		sum.m_data3 = m_data3 - b.m_data3;
		sum.m_data4 = m_data4 - b.m_data4;
		sum.m_data5 = m_data5 - b.m_data5;

		return sum;
	}

	FastVector5<double,double,double,double,double> operator -(double b)
	{
		FastVector5<double,double,double,double,double> sum;

		sum.m_data1 = m_data1 - b;
		sum.m_data2 = m_data2 - b;
		sum.m_data3 = m_data3 - b;
		sum.m_data4 = m_data4 - b;
		sum.m_data5 = m_data5 - b;

		return sum;
	}

	double operator *(FastVector5<double,double,double,double,double>& b)
	{
		return m_data1 * b.m_data1 + m_data2 * b.m_data2 + m_data3 * b.m_data3 +
				m_data4 * b.m_data4 + m_data5 * b.m_data5;
	}

	FastVector5<double,double,double,double,double> operator *(double b)
	{
		FastVector5<double,double,double,double,double> sum;

		sum.m_data1 = m_data1 * b;
		sum.m_data2 = m_data2 * b;
		sum.m_data3 = m_data3 * b;
		sum.m_data4 = m_data4 * b;
		sum.m_data5 = m_data5 * b;

		return sum;
	}


	double m_data1;
	double m_data2;
	double m_data3;
	double m_data4;
	double m_data5;

};


template<>
class FastVector5<float,float,float,float,float>: public BaseObject {
public:
	FastVector5()
	{
		m_data1 = 0;
		m_data2 = 0;
		m_data3 = 0;
		m_data4 = 0;
		m_data5 = 0;
	}

	FastVector5(float a, float b, float c, float d, float e)
	{
		m_data1 = a;
		m_data2 = b;
		m_data3 = c;
		m_data4 = d;
		m_data5 = e;
	}

	virtual ~FastVector5()
	{
	}


	FastVector5<float,float,float,float,float> operator +(FastVector5<float,float,float,float,float>& b)
	{
		FastVector5<float,float,float,float,float> sum;

		sum.m_data1 = m_data1 + b.m_data1;
		sum.m_data2 = m_data2 + b.m_data2;
		sum.m_data3 = m_data3 + b.m_data3;
		sum.m_data4 = m_data4 + b.m_data4;
		sum.m_data5 = m_data5 + b.m_data5;

		return sum;
	}

	FastVector5<float,float,float,float,float> operator +(float b)
	{
		FastVector5<float,float,float,float,float> sum;

		sum.m_data1 = m_data1 + b;
		sum.m_data2 = m_data2 + b;
		sum.m_data3 = m_data3 + b;
		sum.m_data4 = m_data4 + b;
		sum.m_data5 = m_data5 + b;

		return sum;
	}

	FastVector5<float,float,float,float,float> operator -(FastVector5<float,float,float,float,float>& b)
	{
		FastVector5<float,float,float,float,float> sum;

		sum.m_data1 = m_data1 - b.m_data1;
		sum.m_data2 = m_data2 - b.m_data2;
		sum.m_data3 = m_data3 - b.m_data3;
		sum.m_data4 = m_data4 - b.m_data4;
		sum.m_data5 = m_data5 - b.m_data5;

		return sum;
	}

	FastVector5<float,float,float,float,float> operator -(float b)
	{
		FastVector5<float,float,float,float,float> sum;

		sum.m_data1 = m_data1 - b;
		sum.m_data2 = m_data2 - b;
		sum.m_data3 = m_data3 - b;
		sum.m_data4 = m_data4 - b;
		sum.m_data5 = m_data5 - b;

		return sum;
	}

	float operator *(FastVector5<float,float,float,float,float>& b)
	{
		return m_data1 * b.m_data1 + m_data2 * b.m_data2 + m_data3 * b.m_data3 +
				m_data4 * b.m_data4 + m_data5 * b.m_data5;
	}

	FastVector5<float,float,float,float,float> operator *(float b)
	{
		FastVector5<float,float,float,float,float> sum;

		sum.m_data1 = m_data1 * b;
		sum.m_data2 = m_data2 * b;
		sum.m_data3 = m_data3 * b;
		sum.m_data4 = m_data4 * b;
		sum.m_data5 = m_data5 * b;

		return sum;
	}


	float m_data1;
	float m_data2;
	float m_data3;
	float m_data4;
	float m_data5;

};


template<>
class FastVector5<int,int,int,int,int>: public BaseObject {
public:
	FastVector5()
	{
		m_data1 = 0;
		m_data2 = 0;
		m_data3 = 0;
		m_data4 = 0;
		m_data5 = 0;
	}

	FastVector5(int a, int b, int c, int d, int e)
	{
		m_data1 = a;
		m_data2 = b;
		m_data3 = c;
		m_data4 = d;
		m_data5 = e;
	}

	virtual ~FastVector5()
	{
	}


	FastVector5<int,int,int,int,int> operator +(FastVector5<int,int,int,int,int>& b)
	{
		FastVector5<int,int,int,int,int> sum;

		sum.m_data1 = m_data1 + b.m_data1;
		sum.m_data2 = m_data2 + b.m_data2;
		sum.m_data3 = m_data3 + b.m_data3;
		sum.m_data4 = m_data4 + b.m_data4;
		sum.m_data5 = m_data5 + b.m_data5;

		return sum;
	}

	FastVector5<int,int,int,int,int> operator +(int b)
	{
		FastVector5<int,int,int,int,int> sum;

		sum.m_data1 = m_data1 + b;
		sum.m_data2 = m_data2 + b;
		sum.m_data3 = m_data3 + b;
		sum.m_data4 = m_data4 + b;
		sum.m_data5 = m_data5 + b;

		return sum;
	}

	FastVector5<int,int,int,int,int> operator -(FastVector5<int,int,int,int,int>& b)
	{
		FastVector5<int,int,int,int,int> sum;

		sum.m_data1 = m_data1 - b.m_data1;
		sum.m_data2 = m_data2 - b.m_data2;
		sum.m_data3 = m_data3 - b.m_data3;
		sum.m_data4 = m_data4 - b.m_data4;
		sum.m_data5 = m_data5 - b.m_data5;

		return sum;
	}

	FastVector5<int,int,int,int,int> operator -(int b)
	{
		FastVector5<int,int,int,int,int> sum;

		sum.m_data1 = m_data1 - b;
		sum.m_data2 = m_data2 - b;
		sum.m_data3 = m_data3 - b;
		sum.m_data4 = m_data4 - b;
		sum.m_data5 = m_data5 - b;

		return sum;
	}

	int operator *(FastVector5<int,int,int,int,int>& b)
	{
		return m_data1 * b.m_data1 + m_data2 * b.m_data2 + m_data3 * b.m_data3 +
				m_data4 * b.m_data4 + m_data5 * b.m_data5;
	}

	FastVector5<int,int,int,int,int> operator *(int b)
	{
		FastVector5<int,int,int,int,int> sum;

		sum.m_data1 = m_data1 * b;
		sum.m_data2 = m_data2 * b;
		sum.m_data3 = m_data3 * b;
		sum.m_data4 = m_data4 * b;
		sum.m_data5 = m_data5 * b;

		return sum;
	}


	int m_data1;
	int m_data2;
	int m_data3;
	int m_data4;
	int m_data5;

};


template<class T1 = double, class T2 = double, class T3 = double, class T4 = double>
class FastVector4: public BaseObject {
public:
	FastVector4()
	{
		m_data1 = T1();
		m_data2 = T2();
		m_data3 = T3();
		m_data4 = T4();
	}

	FastVector4(const T1& a, const T2& b, const T3& c, const T4& d)
	{
		m_data1 = a;
		m_data2 = b;
		m_data3 = c;
		m_data4 = d;
	}

	virtual ~FastVector4()
	{
	}

	T1 m_data1;
	T2 m_data2;
	T3 m_data3;
	T4 m_data4;

};


template<>
class FastVector4<double,double,double,double>: public BaseObject {
public:
	FastVector4()
	{
		m_data1 = 0;
		m_data2 = 0;
		m_data3 = 0;
		m_data4 = 0;
	}

	FastVector4(double a, double b, double c, double d)
	{
		m_data1 = a;
		m_data2 = b;
		m_data3 = c;
		m_data4 = d;
	}

	virtual ~FastVector4()
	{
	}


	FastVector4<double,double,double,double> operator +(FastVector4<double,double,double,double>& b)
	{
		FastVector4<double,double,double,double> sum;

		sum.m_data1 = m_data1 + b.m_data1;
		sum.m_data2 = m_data2 + b.m_data2;
		sum.m_data3 = m_data3 + b.m_data3;
		sum.m_data4 = m_data4 + b.m_data4;

		return sum;
	}

	FastVector4<double,double,double,double> operator +(double b)
	{
		FastVector4<double,double,double,double> sum;

		sum.m_data1 = m_data1 + b;
		sum.m_data2 = m_data2 + b;
		sum.m_data3 = m_data3 + b;
		sum.m_data4 = m_data4 + b;

		return sum;
	}

	FastVector4<double,double,double,double> operator -(FastVector4<double,double,double,double>& b)
	{
		FastVector4<double,double,double,double> sum;

		sum.m_data1 = m_data1 - b.m_data1;
		sum.m_data2 = m_data2 - b.m_data2;
		sum.m_data3 = m_data3 - b.m_data3;
		sum.m_data4 = m_data4 - b.m_data4;

		return sum;
	}

	FastVector4<double,double,double,double> operator -(double b)
	{
		FastVector4<double,double,double,double> sum;

		sum.m_data1 = m_data1 - b;
		sum.m_data2 = m_data2 - b;
		sum.m_data3 = m_data3 - b;
		sum.m_data4 = m_data4 - b;

		return sum;
	}

	double operator *(FastVector4<double,double,double,double>& b)
	{
		return m_data1 * b.m_data1 + m_data2 * b.m_data2 + m_data3 * b.m_data3 +
				m_data4 * b.m_data4;
	}

	FastVector4<double,double,double,double> operator *(double b)
	{
		FastVector4<double,double,double,double> sum;

		sum.m_data1 = m_data1 * b;
		sum.m_data2 = m_data2 * b;
		sum.m_data3 = m_data3 * b;
		sum.m_data4 = m_data4 * b;

		return sum;
	}


	double m_data1;
	double m_data2;
	double m_data3;
	double m_data4;

};


template<>
class FastVector4<float,float,float,float>: public BaseObject {
public:
	FastVector4()
	{
		m_data1 = 0;
		m_data2 = 0;
		m_data3 = 0;
		m_data4 = 0;
	}

	FastVector4(float a, float b, float c, float d)
	{
		m_data1 = a;
		m_data2 = b;
		m_data3 = c;
		m_data4 = d;
	}

	virtual ~FastVector4()
	{
	}


	FastVector4<float,float,float,float> operator +(FastVector4<float,float,float,float>& b)
	{
		FastVector4<float,float,float,float> sum;

		sum.m_data1 = m_data1 + b.m_data1;
		sum.m_data2 = m_data2 + b.m_data2;
		sum.m_data3 = m_data3 + b.m_data3;
		sum.m_data4 = m_data4 + b.m_data4;

		return sum;
	}

	FastVector4<float,float,float,float> operator +(float b)
	{
		FastVector4<float,float,float,float> sum;

		sum.m_data1 = m_data1 + b;
		sum.m_data2 = m_data2 + b;
		sum.m_data3 = m_data3 + b;
		sum.m_data4 = m_data4 + b;

		return sum;
	}

	FastVector4<float,float,float,float> operator -(FastVector4<float,float,float,float>& b)
	{
		FastVector4<float,float,float,float> sum;

		sum.m_data1 = m_data1 - b.m_data1;
		sum.m_data2 = m_data2 - b.m_data2;
		sum.m_data3 = m_data3 - b.m_data3;
		sum.m_data4 = m_data4 - b.m_data4;

		return sum;
	}

	FastVector4<float,float,float,float> operator -(float b)
	{
		FastVector4<float,float,float,float> sum;

		sum.m_data1 = m_data1 - b;
		sum.m_data2 = m_data2 - b;
		sum.m_data3 = m_data3 - b;
		sum.m_data4 = m_data4 - b;

		return sum;
	}

	float operator *(FastVector4<float,float,float,float>& b)
	{
		return m_data1 * b.m_data1 + m_data2 * b.m_data2 + m_data3 * b.m_data3 +
				m_data4 * b.m_data4;
	}

	FastVector4<float,float,float,float> operator *(float b)
	{
		FastVector4<float,float,float,float> sum;

		sum.m_data1 = m_data1 * b;
		sum.m_data2 = m_data2 * b;
		sum.m_data3 = m_data3 * b;
		sum.m_data4 = m_data4 * b;

		return sum;
	}


	float m_data1;
	float m_data2;
	float m_data3;
	float m_data4;

};


template<>
class FastVector4<int,int,int,int>: public BaseObject {
public:
	FastVector4()
	{
		m_data1 = 0;
		m_data2 = 0;
		m_data3 = 0;
		m_data4 = 0;
	}

	FastVector4(int a, int b, int c, int d)
	{
		m_data1 = a;
		m_data2 = b;
		m_data3 = c;
		m_data4 = d;
	}

	virtual ~FastVector4()
	{
	}


	FastVector4<int,int,int,int> operator +(FastVector4<int,int,int,int>& b)
	{
		FastVector4<int,int,int,int> sum;

		sum.m_data1 = m_data1 + b.m_data1;
		sum.m_data2 = m_data2 + b.m_data2;
		sum.m_data3 = m_data3 + b.m_data3;
		sum.m_data4 = m_data4 + b.m_data4;

		return sum;
	}

	FastVector4<int,int,int,int> operator +(int b)
	{
		FastVector4<int,int,int,int> sum;

		sum.m_data1 = m_data1 + b;
		sum.m_data2 = m_data2 + b;
		sum.m_data3 = m_data3 + b;
		sum.m_data4 = m_data4 + b;

		return sum;
	}

	FastVector4<int,int,int,int> operator -(FastVector4<int,int,int,int>& b)
	{
		FastVector4<int,int,int,int> sum;

		sum.m_data1 = m_data1 - b.m_data1;
		sum.m_data2 = m_data2 - b.m_data2;
		sum.m_data3 = m_data3 - b.m_data3;
		sum.m_data4 = m_data4 - b.m_data4;

		return sum;
	}

	FastVector4<int,int,int,int> operator -(int b)
	{
		FastVector4<int,int,int,int> sum;

		sum.m_data1 = m_data1 - b;
		sum.m_data2 = m_data2 - b;
		sum.m_data3 = m_data3 - b;
		sum.m_data4 = m_data4 - b;

		return sum;
	}

	int operator *(FastVector4<int,int,int,int>& b)
	{
		return m_data1 * b.m_data1 + m_data2 * b.m_data2 + m_data3 * b.m_data3 +
				m_data4 * b.m_data4;
	}

	FastVector4<int,int,int,int> operator *(int b)
	{
		FastVector4<int,int,int,int> sum;

		sum.m_data1 = m_data1 * b;
		sum.m_data2 = m_data2 * b;
		sum.m_data3 = m_data3 * b;
		sum.m_data4 = m_data4 * b;

		return sum;
	}


	int m_data1;
	int m_data2;
	int m_data3;
	int m_data4;

};

template<class T1 = double, class T2 = double, class T3 = double>
class FastVector3: public BaseObject {
public:
	FastVector3()
	{
		m_data1 = T1();
		m_data2 = T2();
		m_data3 = T3();
	}

	FastVector3(const T1& a, const T2& b, const T3& c)
	{
		m_data1 = a;
		m_data2 = b;
		m_data3 = c;
	}

	virtual ~FastVector3()
	{
	}

	T1 m_data1;
	T2 m_data2;
	T3 m_data3;

};

template<>
class FastVector3<double,double,double>: public BaseObject {
public:
	FastVector3()
	{
		m_data1 = 0;
		m_data2 = 0;
		m_data3 = 0;
	}

	FastVector3(double a, double b, double c)
	{
		m_data1 = a;
		m_data2 = b;
		m_data3 = c;
	}

	virtual ~FastVector3()
	{
	}

	FastVector3<double,double,double> operator +(FastVector3<double,double,double>& b)
	{
		FastVector3<double,double,double> sum;

		sum.m_data1 = m_data1 + b.m_data1;
		sum.m_data2 = m_data2 + b.m_data2;
		sum.m_data3 = m_data3 + b.m_data3;

		return sum;
	}

	FastVector3<double,double,double> operator +(double b)
	{
		FastVector3<double,double,double> sum;

		sum.m_data1 = m_data1 + b;
		sum.m_data2 = m_data2 + b;
		sum.m_data3 = m_data3 + b;

		return sum;
	}

	FastVector3<double,double,double> operator -(FastVector3<double,double,double>& b)
	{
		FastVector3<double,double,double> sum;

		sum.m_data1 = m_data1 - b.m_data1;
		sum.m_data2 = m_data2 - b.m_data2;
		sum.m_data3 = m_data3 - b.m_data3;

		return sum;
	}

	FastVector3<double,double,double> operator -(double b)
	{
		FastVector3<double,double,double> sum;

		sum.m_data1 = m_data1 - b;
		sum.m_data2 = m_data2 - b;
		sum.m_data3 = m_data3 - b;

		return sum;
	}

	double operator *(FastVector3<double,double,double>& b)
	{
		return m_data1 * b.m_data1 + m_data2 * b.m_data2 + m_data3 * b.m_data3;
	}

	FastVector3<double,double,double> operator *(double b)
	{
		FastVector3<double,double,double> sum;

		sum.m_data1 = m_data1 * b;
		sum.m_data2 = m_data2 * b;
		sum.m_data3 = m_data3 * b;

		return sum;
	}

	double m_data1;
	double m_data2;
	double m_data3;

};

template<>
class FastVector3<float,float,float>: public BaseObject {
public:
	FastVector3()
	{
		m_data1 = 0;
		m_data2 = 0;
		m_data3 = 0;
	}

	FastVector3(float a, float b, float c)
	{
		m_data1 = a;
		m_data2 = b;
		m_data3 = c;
	}

	virtual ~FastVector3()
	{
	}

	FastVector3<float,float,float> operator +(FastVector3<float,float,float>& b)
	{
		FastVector3<float,float,float> sum;

		sum.m_data1 = m_data1 + b.m_data1;
		sum.m_data2 = m_data2 + b.m_data2;
		sum.m_data3 = m_data3 + b.m_data3;

		return sum;
	}

	FastVector3<float,float,float> operator +(float b)
	{
		FastVector3<float,float,float> sum;

		sum.m_data1 = m_data1 + b;
		sum.m_data2 = m_data2 + b;
		sum.m_data3 = m_data3 + b;

		return sum;
	}

	FastVector3<float,float,float> operator -(FastVector3<float,float,float>& b)
	{
		FastVector3<float,float,float> sum;

		sum.m_data1 = m_data1 - b.m_data1;
		sum.m_data2 = m_data2 - b.m_data2;
		sum.m_data3 = m_data3 - b.m_data3;

		return sum;
	}

	FastVector3<float,float,float> operator -(float b)
	{
		FastVector3<float,float,float> sum;

		sum.m_data1 = m_data1 - b;
		sum.m_data2 = m_data2 - b;
		sum.m_data3 = m_data3 - b;

		return sum;
	}

	float operator *(FastVector3<float,float,float>& b)
	{
		return m_data1 * b.m_data1 + m_data2 * b.m_data2 + m_data3 * b.m_data3;
	}

	FastVector3<float,float,float> operator *(float b)
	{
		FastVector3<float,float,float> sum;

		sum.m_data1 = m_data1 * b;
		sum.m_data2 = m_data2 * b;
		sum.m_data3 = m_data3 * b;

		return sum;
	}

	float m_data1;
	float m_data2;
	float m_data3;

};

template<>
class FastVector3<int,int,int>: public BaseObject {
public:
	FastVector3()
	{
		m_data1 = 0;
		m_data2 = 0;
		m_data3 = 0;
	}

	FastVector3(int a, int b, int c)
	{
		m_data1 = a;
		m_data2 = b;
		m_data3 = c;
	}

	virtual ~FastVector3()
	{
	}

	FastVector3<int,int,int> operator +(FastVector3<int,int,int>& b)
	{
		FastVector3<int,int,int> sum;

		sum.m_data1 = m_data1 + b.m_data1;
		sum.m_data2 = m_data2 + b.m_data2;
		sum.m_data3 = m_data3 + b.m_data3;

		return sum;
	}

	FastVector3<int,int,int> operator +(int b)
	{
		FastVector3<int,int,int> sum;

		sum.m_data1 = m_data1 + b;
		sum.m_data2 = m_data2 + b;
		sum.m_data3 = m_data3 + b;

		return sum;
	}

	FastVector3<int,int,int> operator -(FastVector3<int,int,int>& b)
	{
		FastVector3<int,int,int> sum;

		sum.m_data1 = m_data1 - b.m_data1;
		sum.m_data2 = m_data2 - b.m_data2;
		sum.m_data3 = m_data3 - b.m_data3;

		return sum;
	}

	FastVector3<int,int,int> operator -(int b)
	{
		FastVector3<int,int,int> sum;

		sum.m_data1 = m_data1 - b;
		sum.m_data2 = m_data2 - b;
		sum.m_data3 = m_data3 - b;

		return sum;
	}

	int operator *(FastVector3<int,int,int>& b)
	{
		return m_data1 * b.m_data1 + m_data2 * b.m_data2 + m_data3 * b.m_data3;
	}

	FastVector3<int,int,int> operator *(int b)
	{
		FastVector3<int,int,int> sum;

		sum.m_data1 = m_data1 * b;
		sum.m_data2 = m_data2 * b;
		sum.m_data3 = m_data3 * b;

		return sum;
	}

	int m_data1;
	int m_data2;
	int m_data3;

};

template<class T1 = double, class T2 = double>
class FastVector2: public BaseObject {
public:
	FastVector2()
	{
		m_data1 = T1();
		m_data2 = T2();
	}

	virtual ~FastVector2()
	{
	}

	FastVector2(const T1& a, const T2& b)
	{
		m_data1 = a;
		m_data2 = b;
	}

	T1 m_data1;
	T2 m_data2;

};


template<>
class FastVector2<double,double>: public BaseObject {
public:
	FastVector2()
	{
		m_data1 = 0;
		m_data2 = 0;
	}

	FastVector2(double a, double b)
	{
		m_data1 = a;
		m_data2 = b;
	}

	virtual ~FastVector2()
	{
	}

	FastVector2<double,double> operator +(FastVector2<double,double>& b)
	{
		FastVector2<double,double> sum;

		sum.m_data1 = m_data1 + b.m_data1;
		sum.m_data2 = m_data2 + b.m_data2;

		return sum;
	}

	FastVector2<double,double> operator +(double b)
	{
		FastVector2<double,double> sum;

		sum.m_data1 = m_data1 + b;
		sum.m_data2 = m_data2 + b;

		return sum;
	}

	FastVector2<double,double> operator -(FastVector2<double,double>& b)
	{
		FastVector2<double,double> sum;

		sum.m_data1 = m_data1 - b.m_data1;
		sum.m_data2 = m_data2 - b.m_data2;

		return sum;
	}

	FastVector2<double,double> operator -(double b)
	{
		FastVector2<double,double> sum;

		sum.m_data1 = m_data1 - b;
		sum.m_data2 = m_data2 - b;

		return sum;
	}

	double operator *(FastVector2<double,double>& b)
	{
		return m_data1 * b.m_data1 + m_data2 * b.m_data2 ;
	}

	FastVector2<double,double> operator *(double b)
	{
		FastVector2<double,double> sum;

		sum.m_data1 = m_data1 * b;
		sum.m_data2 = m_data2 * b;

		return sum;
	}

	double m_data1;
	double m_data2;

};


template<>
class FastVector2<float,float>: public BaseObject {
public:
	FastVector2()
	{
		m_data1 = 0;
		m_data2 = 0;
	}

	FastVector2(float a, float b)
	{
		m_data1 = a;
		m_data2 = b;
	}

	virtual ~FastVector2()
	{
	}

	FastVector2<float,float> operator +(FastVector2<float,float>& b)
	{
		FastVector2<float,float> sum;

		sum.m_data1 = m_data1 + b.m_data1;
		sum.m_data2 = m_data2 + b.m_data2;

		return sum;
	}

	FastVector2<float,float> operator +(float b)
	{
		FastVector2<float,float> sum;

		sum.m_data1 = m_data1 + b;
		sum.m_data2 = m_data2 + b;

		return sum;
	}

	FastVector2<float,float> operator -(FastVector2<float,float>& b)
	{
		FastVector2<float,float> sum;

		sum.m_data1 = m_data1 - b.m_data1;
		sum.m_data2 = m_data2 - b.m_data2;

		return sum;
	}

	FastVector2<float,float> operator -(float b)
	{
		FastVector2<float,float> sum;

		sum.m_data1 = m_data1 - b;
		sum.m_data2 = m_data2 - b;

		return sum;
	}

	float operator *(FastVector2<float,float>& b)
	{
		return m_data1 * b.m_data1 + m_data2 * b.m_data2 ;
	}

	FastVector2<float,float> operator *(float b)
	{
		FastVector2<float,float> sum;

		sum.m_data1 = m_data1 * b;
		sum.m_data2 = m_data2 * b;

		return sum;
	}

	float m_data1;
	float m_data2;

};


template<>
class FastVector2<int,int>: public BaseObject {
public:
	FastVector2()
	{
		m_data1 = 0;
		m_data2 = 0;
	}

	FastVector2(int a, int b)
	{
		m_data1 = a;
		m_data2 = b;
	}

	virtual ~FastVector2()
	{
	}

	FastVector2<int,int> operator +(FastVector2<int,int>& b)
	{
		FastVector2<int,int> sum;

		sum.m_data1 = m_data1 + b.m_data1;
		sum.m_data2 = m_data2 + b.m_data2;

		return sum;
	}

	FastVector2<int,int> operator +(int b)
	{
		FastVector2<int,int> sum;

		sum.m_data1 = m_data1 + b;
		sum.m_data2 = m_data2 + b;

		return sum;
	}

	FastVector2<int,int> operator -(FastVector2<int,int>& b)
	{
		FastVector2<int,int> sum;

		sum.m_data1 = m_data1 - b.m_data1;
		sum.m_data2 = m_data2 - b.m_data2;

		return sum;
	}

	FastVector2<int,int> operator -(int b)
	{
		FastVector2<int,int> sum;

		sum.m_data1 = m_data1 - b;
		sum.m_data2 = m_data2 - b;

		return sum;
	}

	int operator *(FastVector2<int,int>& b)
	{
		return m_data1 * b.m_data1 + m_data2 * b.m_data2 ;
	}

	FastVector2<int,int> operator *(int b)
	{
		FastVector2<int,int> sum;

		sum.m_data1 = m_data1 * b;
		sum.m_data2 = m_data2 * b;

		return sum;
	}

	int m_data1;
	int m_data2;

};


//typedefs for our convenience
typedef FastVector2<double,double> RealPoint2DFast;
typedef FastVector3<double,double,double> RealPoint3DFast;
typedef FastVector4<double,double,double,double> RealPoint4DFast;
typedef FastVector5<double,double,double,double,double> RealPoint5DFast;
typedef FastVector6<double,double,double,double,double,double> RealPoint6DFast;
typedef FastVector7<double,double,double,double,double,double,double> RealPoint7DFast;
typedef FastVector2<int,int> IntegerPoint2DFast;
typedef FastVector3<int,int,int> IntegerPoint3DFast;
typedef FastVector4<int,int,int,int> IntegerPoint4DFast;
typedef FastVector5<int,int,int,int,int> IntegerPoint5DFast;
typedef FastVector6<int,int,int,int,int,int> IntegerPoint6DFast;
typedef FastVector7<int,int,int,int,int,int,int> IntegerPoint7DFast;
typedef FastVector2<long long int,long long int> LargeIntegerPoint2DFast;
typedef FastVector3<long long int,long long int,long long int> LargeIntegerPoint3DFast;
typedef FastVector4<long long int,long long int,long long int,long long int> LargeIntegerPoint4DFast;
typedef FastVector5<long long int,long long int,long long int,long long int,long long int> LargeIntegerPoint5DFast;
typedef FastVector6<long long int,long long int,long long int,long long int,long long int,long long int> LargeIntegerPoint6DFast;
typedef FastVector7<long long int,long long int,long long int,long long int,long long int,long long int,long long int> LargeIntegerPoint7DFast;

}


#endif /* FASTVECTORN_H_ */
