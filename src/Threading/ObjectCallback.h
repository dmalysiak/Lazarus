/*
 * ObjectCallback.h
 *
 *  Created on: 05.02.2013
 *      Author: Darius Malysiak
 */

#ifndef OBJECTCALLBACK_H_
#define OBJECTCALLBACK_H_

#include "Callback.h"

namespace Lazarus
{

/**
 * 'delete_co' determines if the callee object should be deleted (true=yes).
 * The callback takes callback functions 'FNAME' of type 'T U::FNAME(Thread*, V a)'.
 * Instatiate this with 'ObjectCallback<T,U,V> c'.
 */
template<class T, class U, class V>
class ObjectCallback: public Callback {
public:
	ObjectCallback(bool delete_co = false)
	{

		mp_object_method = ( T (U::*)(Thread*, V) ) NULL;
		mp_callee_object = NULL;
		m_parameter = V();
		m_delete_callee_object = delete_co;
	}

	virtual ~ObjectCallback()
	{
		if(m_delete_callee_object == true)
		{
			DELETE_NULL_CHECKING(mp_callee_object);
		}
	}

	void callbackMethod(Thread* executing_thread = NULL)
	{
		(mp_callee_object->*mp_object_method)(executing_thread,m_parameter);
	}

	void set_mp_parameter(V parameter)
	{
		m_parameter = parameter;
	}

	void set_mp_object_method( T (U::*object_method)(Thread*, V) )
	{
		mp_object_method = object_method;
	}

	void set_mp_callee_object(U* object)
	{
		mp_callee_object = object;
	}

	/**
	 * 'var' determines if the callee object should be deleted (true=yes).
	 */
	void setDeletable(bool var)
	{
		m_delete_callee_object = var;
	}

private:
	U* mp_callee_object;
	T (U::*mp_object_method)(Thread*, V);
	V m_parameter;
    bool m_delete_callee_object;
};

//------------------- callback container for functions without parameters

/**
 * 'delete_co' determines if the callee object should be deleted (true=yes).
 * The callback takes callback functions 'FNAME' of type 'T U::FNAME(Thread*)'.
 * Instatiate this with 'ObjectCallback<T,U> c'.
 */
template<class T, class U>
class ObjectCallback<T,U,void>: public Callback {
public:
    ObjectCallback(bool delete_co = false)
	{

		mp_object_method = ( T (U::*)(Thread*) ) NULL;
		mp_callee_object = NULL;
		m_delete_callee_object = delete_co;
	}

    virtual ~ObjectCallback()
    {
    	if(m_delete_callee_object == true)
		{
			DELETE_NULL_CHECKING(mp_callee_object);
		}
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

    /**
	 * 'var' determines if the callee object should be deleted (true=yes).
	 */
	void setDeletable(bool var)
	{
		m_delete_callee_object = var;
	}

private:
    U* mp_callee_object;
    T (U::*mp_object_method)(Thread*);
    bool m_delete_callee_object;
};

//------------------- callback container for functions without parameters

/**
 * 'delete_co' determines if the callee object should be deleted (true=yes).
 * The callback takes callback functions 'FNAME' of type 'T U::FNAME()'.
 * Instatiate this with 'ObjectCallback<T,U,CALLBACK_NO_THREAD_REF____> c'.
 */
template<class T, class U>
class ObjectCallback<T,U,CALLBACK_NO_THREAD_REF____>: public Callback {
public:
    ObjectCallback(bool delete_co = false)
	{

		mp_object_method = ( T (U::*)() ) NULL;
		mp_callee_object = NULL;
		m_delete_callee_object = delete_co;
	}

    virtual ~ObjectCallback()
    {
    	if(m_delete_callee_object == true)
		{
			DELETE_NULL_CHECKING(mp_callee_object);
		}
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

    /**
	 * 'var' determines if the callee object should be deleted (true=yes).
	 */
	void setDeletable(bool var)
	{
		m_delete_callee_object = var;
	}

private:
    U* mp_callee_object;
    T (U::*mp_object_method)();
    bool m_delete_callee_object;
};

}
#endif /* OBJECTCALLBACK_H_ */
