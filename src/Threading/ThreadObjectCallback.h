/*
 * ObjectCallback.h
 *
 *  Created on: 05.02.2013
 *      Author: Darius Malysiak
 */

#ifndef THREADOBJECTCALLBACK_H_
#define THREADOBJECTCALLBACK_H_

#include "Callback.h"
#include "Thread.h"

namespace Lazarus
{

/**
 * These ObjectCallbacks can be run asynchronously as a thread!
 */

/**
 * 'delete_co' determines if the callee object should be deleted (true=yes).
 */
template<class T, class U, class V>
class ThreadObjectCallback: public Callback, public Thread {
public:
	ThreadObjectCallback(bool delete_co = false)
	{
		mp_object_method = ( T (U::*)(Thread*,V) ) NULL;
		mp_callee_object = NULL;
		m_parameter = (V)0;
		m_delete_callee_object = delete_co;
	}

	virtual ~ThreadObjectCallback()
	{
		if(m_delete_callee_object == true)
		{
			DELETE_NULL_CHECKING(mp_callee_object);
		}
	}

	void run()
	{
		callbackMethod(this);
	}

	void callbackMethod(Thread* executing_thread = NULL)
	{
		(mp_callee_object->*mp_object_method)(executing_thread,m_parameter);
	}

	void set_mp_parameter(V parameter)
	{
		m_parameter = parameter;
	}

	void set_mp_object_method( T (U::*object_method)(Thread*,V) )
	{
		mp_object_method = object_method;
	}

	void set_mp_callee_object(U* object)
	{
		mp_callee_object = object;
	}

private:
	U* mp_callee_object;
	T (U::*mp_object_method)(Thread*,V);
	V m_parameter;
    bool m_delete_callee_object;
};

//------------------- callback container for functions without parameters, besides the thread pointer.

/**
 * 'delete_co' determines if the callee object should be deleted (true=yes).
 */
template<class T, class U>
class ThreadObjectCallback<T,U,void>: public Callback, public Thread {
public:
	ThreadObjectCallback(bool delete_co = false)
	{

		mp_object_method = ( T (U::*)(Thread*) ) NULL;
		mp_callee_object = NULL;
		m_delete_callee_object = delete_co;
	}

    virtual ~ThreadObjectCallback()
    {
    	if(m_delete_callee_object == true)
		{
			DELETE_NULL_CHECKING(mp_callee_object);
		}
    }

    void run()
	{
		callbackMethod(this);
	}

    void callbackMethod(Thread* executing_thread = NULL)
    {
        (mp_callee_object->*mp_object_method)(executing_thread);
    }

    void set_mp_object_method( T (U::*object_method)(Thread*) )
    {
        mp_object_method = object_method;
    }

    void set_mp_callee_object(U* object)
    {
        mp_callee_object = object;
    }

private:
    U* mp_callee_object;
    T (U::*mp_object_method)(Thread*);
    bool m_delete_callee_object;
};


//------------------- callback container for functions without any parameters, even without thread pointer.

/**
 * 'delete_co' determines if the callee object should be deleted (true=yes).
 */
template<class T, class U>
class ThreadObjectCallback<T,U,CALLBACK_NO_THREAD_REF____>: public Callback, public Thread {
public:
	ThreadObjectCallback(bool delete_co = false)
	{

		mp_object_method = ( T (U::*)() ) NULL;
		mp_callee_object = NULL;
		m_delete_callee_object = delete_co;

	}

    virtual ~ThreadObjectCallback()
    {
    	if(m_delete_callee_object == true)
		{
			DELETE_NULL_CHECKING(mp_callee_object);
		}
    }

    void run()
	{
		callbackMethod(this);
	}

    void callbackMethod(Thread* executing_thread = NULL)
    {
        (mp_callee_object->*mp_object_method)();
    }

    void set_mp_object_method( T (U::*object_method)() )
    {
        mp_object_method = object_method;
    }

    void set_mp_callee_object(U* object)
    {
        mp_callee_object = object;
    }

private:
    U* mp_callee_object;
    T (U::*mp_object_method)();
    bool m_delete_callee_object;
};

}
#endif /* THREADOBJECTCALLBACK_H_ */
