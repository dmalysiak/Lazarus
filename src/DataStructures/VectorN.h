/*
 * NVector.h
 *
 *  Created on: 08.03.2013
 *      Author: Darius Malysiak
 */

#ifndef VECTORN_H_
#define VECTORN_H_

#include "../BaseObject.h"

#include <pthread.h>

namespace Lazarus
{

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
class Vector8: public BaseObject {
public:
	Vector8()
	{
		m_data1 = T1();
		m_data2 = T2();
		m_data3 = T3();
		m_data4 = T4();
		m_data5 = T5();
		m_data6 = T6();
		m_data7 = T7();
		m_data8 = T8();

		pthread_rwlock_init(&m_rw_lock,NULL);
	}

	Vector8(const T1& a, const T2& b, const T3& c, const T4& d, const T5& e, const T6& f, const T7& g, const T8& h)
	{
		m_data1 = a;
		m_data2 = b;
		m_data3 = c;
		m_data4 = d;
		m_data5 = e;
		m_data6 = f;
		m_data7 = g;
		m_data8 = h;

		pthread_rwlock_init(&m_rw_lock,NULL);
	}

	/*
	 * copying a vector8 is not thread safe!
	 */
	Vector8(const Vector8<T1,T2,T3,T4,T5,T6,T7,T8>& v)
	{
		m_data1 = v.m_data1;
		m_data2 = v.m_data2;
		m_data3 = v.m_data3;
		m_data4 = v.m_data4;
		m_data5 = v.m_data5;
		m_data6 = v.m_data6;
		m_data7 = v.m_data7;
		m_data8 = v.m_data8;

		pthread_rwlock_init(&m_rw_lock,NULL);
	}

	virtual ~Vector8()
	{
		pthread_rwlock_destroy(&m_rw_lock);
	}


	inline void setElement1(const T1& data)
	{
		pthread_rwlock_wrlock(&m_rw_lock);
		m_data1 = data;
		pthread_rwlock_unlock(&m_rw_lock);
	}

	inline void setElement2(const T2& data)
	{
		pthread_rwlock_wrlock(&m_rw_lock);
		m_data2 = data;
		pthread_rwlock_unlock(&m_rw_lock);
	}

	inline void setElement3(const T3& data)
	{
		pthread_rwlock_wrlock(&m_rw_lock);
		m_data3 = data;
		pthread_rwlock_unlock(&m_rw_lock);
	}

	inline void setElement4(const T4& data)
	{
		pthread_rwlock_wrlock(&m_rw_lock);
		m_data4 = data;
		pthread_rwlock_unlock(&m_rw_lock);
	}

	inline void setElement5(const T5& data)
	{
		pthread_rwlock_wrlock(&m_rw_lock);
		m_data5 = data;
		pthread_rwlock_unlock(&m_rw_lock);
	}

	inline void setElement6(const T6& data)
	{
		pthread_rwlock_wrlock(&m_rw_lock);
		m_data6 = data;
		pthread_rwlock_unlock(&m_rw_lock);
	}

	inline void setElement7(const T7& data)
	{
		pthread_rwlock_wrlock(&m_rw_lock);
		m_data7 = data;
		pthread_rwlock_unlock(&m_rw_lock);
	}

	inline void setElement8(const T8& data)
	{
		pthread_rwlock_wrlock(&m_rw_lock);
		m_data8 = data;
		pthread_rwlock_unlock(&m_rw_lock);
	}


	inline T1 getElement1()
	{
		T1 data1;
		pthread_rwlock_rdlock(&m_rw_lock);
		data1 = m_data1;
		pthread_rwlock_unlock(&m_rw_lock);
		return data1;
	}

	inline T2 getElement2()
	{
		T2 data2;
		pthread_rwlock_rdlock(&m_rw_lock);
		data2 = m_data2;
		pthread_rwlock_unlock(&m_rw_lock);
		return data2;
	}

	inline T3 getElement3()
	{
		T3 data3;
		pthread_rwlock_rdlock(&m_rw_lock);
		data3 = m_data3;
		pthread_rwlock_unlock(&m_rw_lock);
		return data3;
	}

	inline T4 getElement4()
	{
		T4 data4;
		pthread_rwlock_rdlock(&m_rw_lock);
		data4 = m_data4;
		pthread_rwlock_unlock(&m_rw_lock);
		return data4;
	}

	inline T5 getElement5()
	{
		T5 data5;
		pthread_rwlock_rdlock(&m_rw_lock);
		data5 = m_data5;
		pthread_rwlock_unlock(&m_rw_lock);
		return data5;
	}

	inline T6 getElement6()
	{
		T6 data6;
		pthread_rwlock_rdlock(&m_rw_lock);
		data6 = m_data6;
		pthread_rwlock_unlock(&m_rw_lock);
		return data6;
	}

	inline T7 getElement7()
	{
		T7 data7;
		pthread_rwlock_rdlock(&m_rw_lock);
		data7 = m_data7;
		pthread_rwlock_unlock(&m_rw_lock);
		return data7;
	}

	inline T8 getElement8()
	{
		T8 data8;
		pthread_rwlock_rdlock(&m_rw_lock);
		data8 = m_data8;
		pthread_rwlock_unlock(&m_rw_lock);
		return data8;
	}

	//------------------ faster, yet thread unsafe methods

	inline void setElement1NL(const T1& data)
	{
		m_data1 = data;
	}

	inline void setElement2NL(const T2& data)
	{
		m_data2 = data;
	}

	inline void setElement3NL(const T3& data)
	{
		m_data3 = data;
	}

	inline void setElement4NL(const T4& data)
	{
		m_data4 = data;
	}

	inline void setElement5NL(const T5& data)
	{
		m_data5 = data;
	}

	inline void setElement6NL(const T6& data)
	{
		m_data6 = data;
	}

	inline void setElement7NL(const T7& data)
	{
		m_data7 = data;
	}

	inline void setElement8NL(const T8& data)
	{
		m_data8 = data;
	}


	inline T1 getElement1NL() const
	{
		return m_data1;
	}

	inline T2 getElement2NL() const
	{
		return m_data2;
	}

	inline T3 getElement3NL() const
	{
		return m_data3;
	}

	inline T4 getElement4NL() const
	{
		return m_data4;
	}

	inline T5 getElement5NL() const
	{
		return m_data5;
	}

	inline T6 getElement6NL() const
	{
		return m_data6;
	}

	inline T7 getElement7NL() const
	{
		return m_data7;
	}

	inline T8 getElement8NL() const
	{
		return m_data8;
	}

private:
	T1 m_data1;
	T2 m_data2;
	T3 m_data3;
	T4 m_data4;
	T5 m_data5;
	T6 m_data6;
	T7 m_data7;
	T8 m_data8;

	pthread_rwlock_t m_rw_lock;
};


template<class T1, class T2, class T3, class T4, class T5, class T6, class T7>
class Vector7: public BaseObject {
public:
	Vector7()
	{
		m_data1 = T1();
		m_data2 = T2();
		m_data3 = T3();
		m_data4 = T4();
		m_data5 = T5();
		m_data6 = T6();
		m_data7 = T7();

		pthread_rwlock_init(&m_rw_lock,NULL);
	}

	virtual ~Vector7()
	{
		pthread_rwlock_destroy(&m_rw_lock);
	}

	Vector7(const T1& a, const T2& b, const T3& c, const T4& d, const T5& e, const T6& f, const T7& g)
	{
		m_data1 = a;
		m_data2 = b;
		m_data3 = c;
		m_data4 = d;
		m_data5 = e;
		m_data6 = f;
		m_data7 = g;

		pthread_rwlock_init(&m_rw_lock,NULL);
	}

	/*
	 * copying a vector7 is not thread safe!
	 */
	Vector7(const Vector7<T1,T2,T3,T4,T5,T6,T7>& v)
	{
		m_data1 = v.m_data1;
		m_data2 = v.m_data2;
		m_data3 = v.m_data3;
		m_data4 = v.m_data4;
		m_data5 = v.m_data5;
		m_data6 = v.m_data6;
		m_data7 = v.m_data7;

		pthread_rwlock_init(&m_rw_lock,NULL);
	}


	inline void setElement1(const T1& data)
	{
		pthread_rwlock_wrlock(&m_rw_lock);
		m_data1 = data;
		pthread_rwlock_unlock(&m_rw_lock);
	}

	inline void setElement2(const T2& data)
	{
		pthread_rwlock_wrlock(&m_rw_lock);
		m_data2 = data;
		pthread_rwlock_unlock(&m_rw_lock);
	}

	inline void setElement3(const T3& data)
	{
		pthread_rwlock_wrlock(&m_rw_lock);
		m_data3 = data;
		pthread_rwlock_unlock(&m_rw_lock);
	}

	inline void setElement4(const T4& data)
	{
		pthread_rwlock_wrlock(&m_rw_lock);
		m_data4 = data;
		pthread_rwlock_unlock(&m_rw_lock);
	}

	inline void setElement5(const T5& data)
	{
		pthread_rwlock_wrlock(&m_rw_lock);
		m_data5 = data;
		pthread_rwlock_unlock(&m_rw_lock);
	}

	inline void setElement6(const T6& data)
	{
		pthread_rwlock_wrlock(&m_rw_lock);
		m_data6 = data;
		pthread_rwlock_unlock(&m_rw_lock);
	}

	inline void setElement7(const T7& data)
	{
		pthread_rwlock_wrlock(&m_rw_lock);
		m_data7 = data;
		pthread_rwlock_unlock(&m_rw_lock);
	}



	inline T1 getElement1()
	{
		T1 data1;
		pthread_rwlock_rdlock(&m_rw_lock);
		data1 = m_data1;
		pthread_rwlock_unlock(&m_rw_lock);
		return data1;
	}

	inline T2 getElement2()
	{
		T2 data2;
		pthread_rwlock_rdlock(&m_rw_lock);
		data2 = m_data2;
		pthread_rwlock_unlock(&m_rw_lock);
		return data2;
	}

	inline T3 getElement3()
	{
		T3 data3;
		pthread_rwlock_rdlock(&m_rw_lock);
		data3 = m_data3;
		pthread_rwlock_unlock(&m_rw_lock);
		return data3;
	}

	inline T4 getElement4()
	{
		T4 data4;
		pthread_rwlock_rdlock(&m_rw_lock);
		data4 = m_data4;
		pthread_rwlock_unlock(&m_rw_lock);
		return data4;
	}

	inline T5 getElement5()
	{
		T5 data5;
		pthread_rwlock_rdlock(&m_rw_lock);
		data5 = m_data5;
		pthread_rwlock_unlock(&m_rw_lock);
		return data5;
	}

	inline T6 getElement6()
	{
		T6 data6;
		pthread_rwlock_rdlock(&m_rw_lock);
		data6 = m_data6;
		pthread_rwlock_unlock(&m_rw_lock);
		return data6;
	}

	inline T7 getElement7()
	{
		T7 data7;
		pthread_rwlock_rdlock(&m_rw_lock);
		data7 = m_data7;
		pthread_rwlock_unlock(&m_rw_lock);
		return data7;
	}

	//------------------ faster, yet thread unsafe methods

	inline void setElement1NL(const T1& data)
	{
		m_data1 = data;
	}

	inline void setElement2NL(const T2& data)
	{
		m_data2 = data;
	}

	inline void setElement3NL(const T3& data)
	{
		m_data3 = data;
	}

	inline void setElement4NL(const T4& data)
	{
		m_data4 = data;
	}

	inline void setElement5NL(const T5& data)
	{
		m_data5 = data;
	}

	inline void setElement6NL(const T6& data)
	{
		m_data6 = data;
	}

	inline void setElement7NL(const T7& data)
	{
		m_data7 = data;
	}



	inline T1 getElement1NL() const
	{
		return m_data1;
	}

	inline T2 getElement2NL() const
	{
		return m_data2;
	}

	inline T3 getElement3NL() const
	{
		return m_data3;
	}

	inline T4 getElement4NL() const
	{
		return m_data4;
	}

	inline T5 getElement5NL() const
	{
		return m_data5;
	}

	inline T6 getElement6NL() const
	{
		return m_data6;
	}

	inline T7 getElement7NL() const
	{
		return m_data7;
	}

private:
	T1 m_data1;
	T2 m_data2;
	T3 m_data3;
	T4 m_data4;
	T5 m_data5;
	T6 m_data6;
	T7 m_data7;

	pthread_rwlock_t m_rw_lock;
};

template<class T1, class T2, class T3, class T4, class T5, class T6>
class Vector6: public BaseObject {
public:
	Vector6()
	{
		m_data1 = T1();
		m_data2 = T2();
		m_data3 = T3();
		m_data4 = T4();
		m_data5 = T5();
		m_data6 = T6();

		pthread_rwlock_init(&m_rw_lock,NULL);
	}

	Vector6(const T1& a, const T2& b, const T3& c, const T4& d, const T5& e, const T6& f)
	{
		m_data1 = a;
		m_data2 = b;
		m_data3 = c;
		m_data4 = d;
		m_data5 = e;
		m_data6 = f;

		pthread_rwlock_init(&m_rw_lock,NULL);
	}

	/*
	 * copying a vector6 is not thread safe!
	 */
	Vector6(const Vector6<T1,T2,T3,T4,T5,T6>& v)
	{
		m_data1 = v.m_data1;
		m_data2 = v.m_data2;
		m_data3 = v.m_data3;
		m_data4 = v.m_data4;
		m_data5 = v.m_data5;
		m_data6 = v.m_data6;

		pthread_rwlock_init(&m_rw_lock,NULL);
	}

	virtual ~Vector6()
	{
		pthread_rwlock_destroy(&m_rw_lock);
	}


	inline void setElement1(const T1& data)
	{
		pthread_rwlock_wrlock(&m_rw_lock);
		m_data1 = data;
		pthread_rwlock_unlock(&m_rw_lock);
	}

	inline void setElement2(const T2& data)
	{
		pthread_rwlock_wrlock(&m_rw_lock);
		m_data2 = data;
		pthread_rwlock_unlock(&m_rw_lock);
	}

	inline void setElement3(const T3& data)
	{
		pthread_rwlock_wrlock(&m_rw_lock);
		m_data3 = data;
		pthread_rwlock_unlock(&m_rw_lock);
	}

	inline void setElement4(const T4& data)
	{
		pthread_rwlock_wrlock(&m_rw_lock);
		m_data4 = data;
		pthread_rwlock_unlock(&m_rw_lock);
	}

	inline void setElement5(const T5& data)
	{
		pthread_rwlock_wrlock(&m_rw_lock);
		m_data5 = data;
		pthread_rwlock_unlock(&m_rw_lock);
	}

	inline void setElement6(const T6& data)
	{
		pthread_rwlock_wrlock(&m_rw_lock);
		m_data6 = data;
		pthread_rwlock_unlock(&m_rw_lock);
	}



	inline T1 getElement1()
	{
		T1 data1;
		pthread_rwlock_rdlock(&m_rw_lock);
		data1 = m_data1;
		pthread_rwlock_unlock(&m_rw_lock);
		return data1;
	}

	inline T2 getElement2()
	{
		T2 data2;
		pthread_rwlock_rdlock(&m_rw_lock);
		data2 = m_data2;
		pthread_rwlock_unlock(&m_rw_lock);
		return data2;
	}

	inline T3 getElement3()
	{
		T3 data3;
		pthread_rwlock_rdlock(&m_rw_lock);
		data3 = m_data3;
		pthread_rwlock_unlock(&m_rw_lock);
		return data3;
	}

	inline T4 getElement4()
	{
		T4 data4;
		pthread_rwlock_rdlock(&m_rw_lock);
		data4 = m_data4;
		pthread_rwlock_unlock(&m_rw_lock);
		return data4;
	}

	inline T5 getElement5()
	{
		T5 data5;
		pthread_rwlock_rdlock(&m_rw_lock);
		data5 = m_data5;
		pthread_rwlock_unlock(&m_rw_lock);
		return data5;
	}

	inline T6 getElement6()
	{
		T6 data6;
		pthread_rwlock_rdlock(&m_rw_lock);
		data6 = m_data6;
		pthread_rwlock_unlock(&m_rw_lock);
		return data6;
	}

	//------------------ faster, yet thread unsafe methods

	inline void setElement1NL(const T1& data)
	{
		m_data1 = data;
	}

	inline void setElement2NL(const T2& data)
	{
		m_data2 = data;
	}

	inline void setElement3NL(const T3& data)
	{
		m_data3 = data;
	}

	inline void setElement4NL(const T4& data)
	{
		m_data4 = data;
	}

	inline void setElement5NL(const T5& data)
	{
		m_data5 = data;
	}

	inline void setElement6NL(const T6& data)
	{
		m_data6 = data;
	}



	inline T1 getElement1NL() const
	{
		return m_data1;
	}

	inline T2 getElement2NL() const
	{
		return m_data2;
	}

	inline T3 getElement3NL() const
	{
		return m_data3;
	}

	inline T4 getElement4NL() const
	{
		return m_data4;
	}

	inline T5 getElement5NL() const
	{
		return m_data5;
	}

	inline T6 getElement6NL() const
	{
		return m_data6;
	}


private:
	T1 m_data1;
	T2 m_data2;
	T3 m_data3;
	T4 m_data4;
	T5 m_data5;
	T6 m_data6;

	pthread_rwlock_t m_rw_lock;
};


template<class T1, class T2, class T3, class T4, class T5>
class Vector5: public BaseObject {
public:
	Vector5()
	{
		m_data1 = T1();
		m_data2 = T2();
		m_data3 = T3();
		m_data4 = T4();
		m_data5 = T5();

		pthread_rwlock_init(&m_rw_lock,NULL);
	}

	Vector5(const T1& a, const T2& b, const T3& c, const T4& d, const T5& e)
	{
		m_data1 = a;
		m_data2 = b;
		m_data3 = c;
		m_data4 = d;
		m_data5 = e;

		pthread_rwlock_init(&m_rw_lock,NULL);
	}

	/*
	 * copying a vector5 is not thread safe!
	 */
	Vector5(const Vector5<T1,T2,T3,T4,T5>& v)
	{
		m_data1 = v.m_data1;
		m_data2 = v.m_data2;
		m_data3 = v.m_data3;
		m_data4 = v.m_data4;
		m_data5 = v.m_data5;

		pthread_rwlock_init(&m_rw_lock,NULL);
	}

	virtual ~Vector5()
	{
		pthread_rwlock_destroy(&m_rw_lock);
	}


	inline void setElement1(const T1& data)
	{
		pthread_rwlock_wrlock(&m_rw_lock);
		m_data1 = data;
		pthread_rwlock_unlock(&m_rw_lock);
	}

	inline void setElement2(const T2& data)
	{
		pthread_rwlock_wrlock(&m_rw_lock);
		m_data2 = data;
		pthread_rwlock_unlock(&m_rw_lock);
	}

	inline void setElement3(const T3& data)
	{
		pthread_rwlock_wrlock(&m_rw_lock);
		m_data3 = data;
		pthread_rwlock_unlock(&m_rw_lock);
	}

	inline void setElement4(const T4& data)
	{
		pthread_rwlock_wrlock(&m_rw_lock);
		m_data4 = data;
		pthread_rwlock_unlock(&m_rw_lock);
	}

	inline void setElement5(const T5& data)
	{
		pthread_rwlock_wrlock(&m_rw_lock);
		m_data5 = data;
		pthread_rwlock_unlock(&m_rw_lock);
	}


	inline T1 getElement1()
	{
		T1 data1;
		pthread_rwlock_rdlock(&m_rw_lock);
		data1 = m_data1;
		pthread_rwlock_unlock(&m_rw_lock);
		return data1;
	}

	inline T2 getElement2()
	{
		T2 data2;
		pthread_rwlock_rdlock(&m_rw_lock);
		data2 = m_data2;
		pthread_rwlock_unlock(&m_rw_lock);
		return data2;
	}

	inline T3 getElement3()
	{
		T3 data3;
		pthread_rwlock_rdlock(&m_rw_lock);
		data3 = m_data3;
		pthread_rwlock_unlock(&m_rw_lock);
		return data3;
	}

	inline T4 getElement4()
	{
		T4 data4;
		pthread_rwlock_rdlock(&m_rw_lock);
		data4 = m_data4;
		pthread_rwlock_unlock(&m_rw_lock);
		return data4;
	}

	inline T5 getElement5()
	{
		T5 data5;
		pthread_rwlock_rdlock(&m_rw_lock);
		data5 = m_data5;
		pthread_rwlock_unlock(&m_rw_lock);
		return data5;
	}

	//------------------ faster, yet thread unsafe methods

	inline void setElement1NL(const T1& data)
	{
		m_data1 = data;
	}

	inline void setElement2NL(const T2& data)
	{
		m_data2 = data;
	}

	inline void setElement3NL(const T3& data)
	{
		m_data3 = data;
	}

	inline void setElement4NL(const T4& data)
	{
		m_data4 = data;
	}

	inline void setElement5NL(const T5& data)
	{
		m_data5 = data;
	}


	inline T1 getElement1NL() const
	{
		return m_data1;
	}

	inline T2 getElement2NL() const
	{
		return m_data2;
	}

	inline T3 getElement3NL() const
	{
		return m_data3;
	}

	inline T4 getElement4NL() const
	{
		return m_data4;
	}

	inline T5 getElement5NL() const
	{
		return m_data5;
	}

private:
	T1 m_data1;
	T2 m_data2;
	T3 m_data3;
	T4 m_data4;
	T5 m_data5;

	pthread_rwlock_t m_rw_lock;
};

template<class T1, class T2, class T3, class T4>
class Vector4: public BaseObject {
public:
	Vector4()
	{
		m_data1 = T1();
		m_data2 = T2();
		m_data3 = T3();
		m_data4 = T4();

		pthread_rwlock_init(&m_rw_lock,NULL);
	}

	Vector4(const T1& a, const T2& b, const T3& c, const T4& d)
	{
		m_data1 = a;
		m_data2 = b;
		m_data3 = c;
		m_data4 = d;

		pthread_rwlock_init(&m_rw_lock,NULL);
	}

	/*
	 * copying a vector4 is not thread safe!
	 */
	Vector4(const Vector4<T1,T2,T3,T4>& v)
	{
		m_data1 = v.m_data1;
		m_data2 = v.m_data2;
		m_data3 = v.m_data3;
		m_data4 = v.m_data4;

		pthread_rwlock_init(&m_rw_lock,NULL);
	}

	virtual ~Vector4()
	{
		pthread_rwlock_destroy(&m_rw_lock);
	}


	inline void setElement1(const T1& data)
	{
		pthread_rwlock_wrlock(&m_rw_lock);
		m_data1 = data;
		pthread_rwlock_unlock(&m_rw_lock);
	}

	inline void setElement2(const T2& data)
	{
		pthread_rwlock_wrlock(&m_rw_lock);
		m_data2 = data;
		pthread_rwlock_unlock(&m_rw_lock);
	}

	inline void setElement3(const T3& data)
	{
		pthread_rwlock_wrlock(&m_rw_lock);
		m_data3 = data;
		pthread_rwlock_unlock(&m_rw_lock);
	}

	inline void setElement4(const T4& data)
	{
		pthread_rwlock_wrlock(&m_rw_lock);
		m_data4 = data;
		pthread_rwlock_unlock(&m_rw_lock);
	}


	inline T1 getElement1()
	{
		T1 data1;
		pthread_rwlock_rdlock(&m_rw_lock);
		data1 = m_data1;
		pthread_rwlock_unlock(&m_rw_lock);
		return data1;
	}

	inline T2 getElement2()
	{
		T2 data2;
		pthread_rwlock_rdlock(&m_rw_lock);
		data2 = m_data2;
		pthread_rwlock_unlock(&m_rw_lock);
		return data2;
	}

	inline T3 getElement3()
	{
		T3 data3;
		pthread_rwlock_rdlock(&m_rw_lock);
		data3 = m_data3;
		pthread_rwlock_unlock(&m_rw_lock);
		return data3;
	}

	inline T4 getElement4()
	{
		T4 data4;
		pthread_rwlock_rdlock(&m_rw_lock);
		data4 = m_data4;
		pthread_rwlock_unlock(&m_rw_lock);
		return data4;
	}

	//------------------ faster, yet thread unsafe methods


	inline void setElement1NL(const T1& data)
	{
		m_data1 = data;
	}

	inline void setElement2NL(const T2& data)
	{
		m_data2 = data;
	}

	inline void setElement3NL(const T3& data)
	{
		m_data3 = data;
	}

	inline void setElement4NL(const T4& data)
	{
		m_data4 = data;
	}


	inline T1 getElement1NL() const
	{
		return m_data1;
	}

	inline T2 getElement2NL() const
	{
		return m_data2;
	}

	inline T3 getElement3NL() const
	{
		return m_data3;
	}

	inline T4 getElement4NL() const
	{
		return m_data4;
	}

private:
	T1 m_data1;
	T2 m_data2;
	T3 m_data3;
	T4 m_data4;

	pthread_rwlock_t m_rw_lock;
};

template<class T1, class T2, class T3>
class Vector3: public BaseObject {
public:
	Vector3()
	{
		m_data1 = T1();
		m_data2 = T2();
		m_data3 = T3();

		pthread_rwlock_init(&m_rw_lock,NULL);
	}

	Vector3(const T1& a, const T2& b, const T3& c)
	{
		m_data1 = a;
		m_data2 = b;
		m_data3 = c;

		pthread_rwlock_init(&m_rw_lock,NULL);
	}

	/*
	 * copying a vector3 is not thread safe!
	 */
	Vector3(const Vector3<T1,T2,T3>& v)
	{
		m_data1 = v.m_data1;
		m_data2 = v.m_data2;
		m_data3 = v.m_data3;

		pthread_rwlock_init(&m_rw_lock,NULL);
	}

	virtual ~Vector3()
	{
		pthread_rwlock_destroy(&m_rw_lock);
	}


	inline void setElement1(const T1& data)
	{
		pthread_rwlock_wrlock(&m_rw_lock);
		m_data1 = data;
		pthread_rwlock_unlock(&m_rw_lock);
	}

	inline void setElement2(const T2& data)
	{
		pthread_rwlock_wrlock(&m_rw_lock);
		m_data2 = data;
		pthread_rwlock_unlock(&m_rw_lock);
	}

	inline void setElement3(const T3& data)
	{
		pthread_rwlock_wrlock(&m_rw_lock);
		m_data3 = data;
		pthread_rwlock_unlock(&m_rw_lock);
	}


	inline T1 getElement1()
	{
		T1 data1;
		pthread_rwlock_rdlock(&m_rw_lock);
		data1 = m_data1;
		pthread_rwlock_unlock(&m_rw_lock);
		return data1;
	}

	inline T2 getElement2()
	{
		T2 data2;
		pthread_rwlock_rdlock(&m_rw_lock);
		data2 = m_data2;
		pthread_rwlock_unlock(&m_rw_lock);
		return data2;
	}

	inline T3 getElement3()
	{
		T3 data3;
		pthread_rwlock_rdlock(&m_rw_lock);
		data3 = m_data3;
		pthread_rwlock_unlock(&m_rw_lock);
		return data3;
	}

	//------------------ faster, yet thread unsafe methods


	inline void setElement1NL(const T1& data)
	{
		m_data1 = data;
	}

	inline void setElement2NL(const T2& data)
	{
		m_data2 = data;
	}

	inline void setElement3NL(const T3& data)
	{
		m_data3 = data;
	}


	inline T1 getElement1NL() const
	{
		return m_data1;
	}

	inline T2 getElement2NL() const
	{
		return m_data2;
	}

	inline T3 getElement3NL() const
	{
		return m_data3;
	}

private:
	T1 m_data1;
	T2 m_data2;
	T3 m_data3;

	pthread_rwlock_t m_rw_lock;
};


template<class T1, class T2>
class Vector2: public BaseObject {
public:
	Vector2()
	{
		m_data1 = T1();
		m_data2 = T2();
		pthread_rwlock_init(&m_rw_lock,NULL);
	}

	virtual ~Vector2()
	{
		pthread_rwlock_destroy(&m_rw_lock);
	}

	Vector2(const T1& a, const T2& b)
	{
		m_data1 = a;
		m_data2 = b;

		pthread_rwlock_init(&m_rw_lock,NULL);
	}

	/*
	 * copying a vector2 is not thread safe!
	 */
	Vector2(const Vector2<T1,T2>& v)
	{
		m_data1 = v.m_data1;
		m_data2 = v.m_data2;

		pthread_rwlock_init(&m_rw_lock,NULL);
	}


	inline void setElement1(const T1& data)
	{
		pthread_rwlock_wrlock(&m_rw_lock);
		m_data1 = data;
		pthread_rwlock_unlock(&m_rw_lock);
	}

	inline void setElement2(const T2& data)
	{
		pthread_rwlock_wrlock(&m_rw_lock);
		m_data2 = data;
		pthread_rwlock_unlock(&m_rw_lock);
	}

	inline T1 getElement1()
	{
		T1 data1;
		pthread_rwlock_rdlock(&m_rw_lock);
		data1 = m_data1;
		pthread_rwlock_unlock(&m_rw_lock);
		return data1;
	}

	inline T2 getElement2()
	{
		T2 data2;
		pthread_rwlock_rdlock(&m_rw_lock);
		data2 = m_data2;
		pthread_rwlock_unlock(&m_rw_lock);
		return data2;
	}

	//------------------ faster, yet thread unsafe methods

	inline void setElement1NL(const T1& data)
	{
		m_data1 = data;
	}

	inline void setElement2NL(const T2& data)
	{
		m_data2 = data;
	}

	inline T1 getElement1NL() const
	{
		return m_data1;
	}

	inline T2 getElement2NL() const
	{
		return m_data2;
	}


private:
	T1 m_data1;
	T2 m_data2;

	pthread_rwlock_t m_rw_lock;
};

//typedefs for our convenience
typedef Vector2<double,double> RealPoint2D;
typedef Vector3<double,double,double> RealPoint3D;
typedef Vector4<double,double,double,double> RealPoint4D;
typedef Vector2<int,int> IntegerPoint2D;
typedef Vector3<int,int,int> IntegerPoint3D;
typedef Vector4<int,int,int,int> IntegerPoint4D;

}


#endif /* VECTORN_H_ */
