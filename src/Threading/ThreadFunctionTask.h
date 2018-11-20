/*
 * FunctionTask.h
 *
 *  Created on: 30.01.2013
 *      Author: Darius Malysiak
 */

#ifndef THREADFUNCTIONTASK_H_
#define THREADFUNCTIONTASK_H_

#include "Task.h"
#include "Thread.h"

namespace Lazarus
{

/**
 * If a ThreadFunctionTask's static function member has at least one parameter, it must also prepend this parameter in
 * his signature with a 'Thread*' parameter. I.e. if one desires to wrap a static function as 'void f(X x)' then
 * it must be provided as 'void f(Thread* t, X x)'! The same holds for elements in the callback queue, yet individually
 * for each element in the queue!
 */

/**
 * This class allows the asynchronous execution of tasks without any thread pool.
 */
template<class T, class U>
class ThreadFunctionTask: public Task, public Thread {
public:
	ThreadFunctionTask()
	{

		mp_execution_function = ( T (*)(Thread*,U) )NULL;
		m_function_parameter = U();

	}

	virtual ~ThreadFunctionTask()
	{

	}

	void run()
	{
		runTask(this);
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

	CLASS_RTTI(Lazarus::ThreadFunctionTask)

private:
	T (*mp_execution_function)(Thread* thread, U parameter);
	U m_function_parameter;
};

//-------------------- FunctionTask for static functions without additional parameters, i.e. only the thread pointer.
/**
 * This class allows the asynchronous execution of tasks without any thread pool.
 */
template<class T>
class ThreadFunctionTask<T,void>: public Task, public Thread  {
public:
	ThreadFunctionTask()
	{

		mp_execution_function = ( T (*)(Thread*) )NULL;

	}

    virtual ~ThreadFunctionTask()
    {

    }

    CLASS_RTTI(Lazarus::ThreadFunctionTask)

    void run()
	{
		runTask(this);
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
        callback(executing_thread);
    }

    void setmp_function( T (*execution_function)(Thread*)  )
    {
        mp_execution_function = execution_function;
    }

private:
    T (*mp_execution_function)(Thread*);
};


//-------------------- FunctionTask for static functions without any additional parameters, i.e. not even the thread pointer.
/**
 * This class allows the asynchronous execution of tasks without any thread pool.
 */
template<class T>
class ThreadFunctionTask<T,NO_STATIC_THREAD_REF___>: public Task, public Thread  {
public:
	ThreadFunctionTask()
	{

		mp_execution_function = ( T (*)() )NULL;

	}

    virtual ~ThreadFunctionTask()
    {

    }

    CLASS_RTTI(Lazarus::ThreadFunctionTask)

    void run()
	{
		runTask(this);
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
        callback(executing_thread);
    }

    void setmp_function( T (*execution_function)(Thread*)  )
    {
        mp_execution_function = execution_function;
    }

private:
    T (*mp_execution_function)();
};

}

#endif /* THREADFUNCTIONTASK_H_ */
