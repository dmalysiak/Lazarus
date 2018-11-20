/*
 * FunctionTask.h
 *
 *  Created on: 30.01.2013
 *      Author: Darius Malysiak
 */

#ifndef FUNCTIONTASK_H_
#define FUNCTIONTASK_H_

#include "Task.h"

namespace Lazarus
{

/**
 * If a FunctionTasks static function member has at least one parameter, it must also prepend this parameter in
 * his signature with a 'Thread*' parameter. I.e. if one desires to wrap a static function as 'void f(X x)' then
 * it must be provided as 'void f(Thread* t, X x)'! The same holds for elements in the callback queue, yet individually
 * for each element in the queue!
 */

/**
 * This class represents an abstraction of executable functions. An instance of this class can be assigned a
 * pointer to an arbitrary static functions with return type T and parameter Type U, besides the thread pointer.
 * The 'run' method will execute that function once and afterwards process all elements from an assigned
 * callback queue. If no callback queue has been assigned nothing will happen.
 */

template<class T, class U>
class FunctionTask: public Task {
public:
	FunctionTask()
	{

		mp_execution_function = ( T (*)(Thread*,U) )NULL;
		m_function_parameter = U();

	}

	virtual ~FunctionTask()
	{

	}

	void runTask(Thread* executing_thread = NULL)
	{
	    //execute the function only if it exists
	    if(mp_execution_function != NULL)
	    {
	        //execute the desired function
	        mp_execution_function(executing_thread,m_function_parameter);
	    }

		//run the callback
		callback(executing_thread);
	}

	void setmp_function( T (*execution_function)(Thread*,U)  )
	{
		mp_execution_function = execution_function;
	}

	void setmp_function_parameter(U function_parameter)
	{
		m_function_parameter = function_parameter;
	}

	CLASS_RTTI(Lazarus::FunctionTask)

private:
	T (*mp_execution_function)(Thread*,U parameter);
	U m_function_parameter;
};

//-------------------- FunctionTask for function without parameters

/**
 * This class represents an abstraction of executable functions. An instance of this class can be assigned a
 * pointer to an arbitrary static functions with return type T and no parameters, i.e. U=void, besides the Thread pointer.
 * The 'run' method will execute that function once and afterwards process all elements from an assigned
 * callback queue. If no callback queue has been assigned nothing will happen.
 */
template<class T>
class FunctionTask<T,void>: public Task {
public:
    FunctionTask()
	{

	   mp_execution_function = ( T (*)(Thread*) )NULL;

	}

    virtual ~FunctionTask()
    {

    }

    void runTask(Thread* executing_thread = NULL)
    {
        //execute the function only if it exists
        if(mp_execution_function != NULL)
        {
            //execute the desired function
            mp_execution_function(executing_thread);
        }

        //run the callback
        callback();
    }

    void setmp_function( T (*execution_function)(Thread*)  )
    {
        mp_execution_function = execution_function;
    }

    CLASS_RTTI(Lazarus::FunctionTask)

private:
    T (*mp_execution_function)(Thread*);
};



//-------------------- FunctionTask for static function without any parameters at all

/**
 * This class represents an abstraction of executable functions. An instance of this class can be assigned a
 * pointer to an arbitrary static functions with return type T and no parameters i.e. U=void.
 * The 'run' method will execute that function once and afterwards process all elements from an assigned
 * callback queue. If no callback queue has been assigned nothing will happen.
 */
template<class T>
class FunctionTask<T, NO_STATIC_THREAD_REF___>: public Task {
public:
    FunctionTask()
	{

	   mp_execution_function = ( T (*)() )NULL;

	}

    virtual ~FunctionTask()
    {

    }

    void runTask(Thread* executing_thread = NULL)
    {
        //execute the function only if it exists
        if(mp_execution_function != NULL)
        {
            //execute the desired function
            mp_execution_function();
        }

        //run the callback
        callback();
    }

    void setmp_function( T (*execution_function)()  )
    {
        mp_execution_function = execution_function;
    }

    CLASS_RTTI(Lazarus::FunctionTask)

private:
    T (*mp_execution_function)();
};




}

#endif /* FUNCTIONTASK_H_ */
