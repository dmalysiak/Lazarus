/*
 * TaskHelper.h
 *
 *  Created on: 05.03.2013
 *      Author: Darius Malysiak
 */

#ifndef TASKHELPER_H_
#define TASKHELPER_H_

#include "FunctionTask.h"
#include "ObjectCallback.h"

#include "ThreadFunctionTask.h"
#include "ThreadObjectCallback.h"

#include "CallbackQueue.h"

namespace Lazarus
{


/**
 * SRef = static function has a reference to the thread
 * DRef = callback method has a reference to the thread
 *
 * If one of these suffixes is missing then the corresponding element has no parameters at all.
 */

/*
 * In order to execute object methods one has to use the callback queue of a function task and assign an ObjectCallback instance to it.
 */

//******************* non thread tasks *********************************************

/**
 * This method returns a function task which can be deployed in a thread pool.
 * The function tasks assigned function is the given static function, yet its callback queue will be filled with a new queue
 * which contains a single element, namely an ObjectCallback instance which will execute the given method of the
 * given object.
 */

//void specials
template<typename FUNCTION_RETURN_TYPE,
		typename CALLBACK_RETURN_TYPE, class CALLBACK_CLASS_TYPE,
		typename CALLBACK_PARAMETER_TYPE >
FunctionTask<FUNCTION_RETURN_TYPE,void>* quickDeployCompleteTaskVSRefDRef(
		FUNCTION_RETURN_TYPE (*static_function)(Thread*),
		CALLBACK_CLASS_TYPE* class_instance,
		CALLBACK_RETURN_TYPE (CALLBACK_CLASS_TYPE::*class_method)(Thread*,CALLBACK_PARAMETER_TYPE),
		CALLBACK_PARAMETER_TYPE callback_parameter)
{
	//create a task
	FunctionTask<FUNCTION_RETURN_TYPE,void>* ft = new FunctionTask<FUNCTION_RETURN_TYPE,void>();

	//static function setup
	ft->setmp_function(static_function);

	//# ----------------- prepare the notifier callback
	//create the callback container
	ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,CALLBACK_PARAMETER_TYPE>* o1 =
			new ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,CALLBACK_PARAMETER_TYPE>();

	//assign the callee objects to the containers
	o1->set_mp_callee_object(class_instance);

	//assign the functions to the containers
	o1->set_mp_object_method(class_method);

	//assign the parameters for the function
	o1->set_mp_parameter(callback_parameter);

	//create the queue and enqueue the objects
	CallbackQueue* c_queue = new CallbackQueue();
	c_queue->enqueueCallback(o1);

	//register the queue at the function task
	ft->registerCallbackQueue(c_queue);

	return ft;
}

template<typename FUNCTION_RETURN_TYPE, typename FUNCTION_PARAMETER_TYPE,
		typename CALLBACK_RETURN_TYPE, class CALLBACK_CLASS_TYPE >
FunctionTask<FUNCTION_RETURN_TYPE,FUNCTION_PARAMETER_TYPE>* quickDeployCompleteTaskSRefVDRef(
		FUNCTION_RETURN_TYPE (*static_function)(Thread*,FUNCTION_PARAMETER_TYPE),
		FUNCTION_PARAMETER_TYPE static_function_parameter,
		CALLBACK_CLASS_TYPE* class_instance,
		CALLBACK_RETURN_TYPE (CALLBACK_CLASS_TYPE::*class_method)(Thread*) )
{
	//create a task
	FunctionTask<FUNCTION_RETURN_TYPE,FUNCTION_PARAMETER_TYPE>* ft = new FunctionTask<FUNCTION_RETURN_TYPE,FUNCTION_PARAMETER_TYPE>();

	//static function setup
	ft->setmp_function(static_function);
	ft->setmp_function_parameter(static_function_parameter);

	//# ----------------- prepare the notifier callback
	//create the callback container
	ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,void>* o1 =
			new ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,void>();

	//assign the callee objects to the containers
	o1->set_mp_callee_object(class_instance);

	//assign the functions to the containers
	o1->set_mp_object_method(class_method);

	//create the queue and enqueue the objects
	CallbackQueue* c_queue = new CallbackQueue();
	c_queue->enqueueCallback(o1);

	//register the queue at the function task
	ft->registerCallbackQueue(c_queue);

	return ft;
}

template<typename FUNCTION_RETURN_TYPE,
		typename CALLBACK_RETURN_TYPE, class CALLBACK_CLASS_TYPE >
FunctionTask<FUNCTION_RETURN_TYPE,void>* quickDeployCompleteTaskVSRefVDRef(
		FUNCTION_RETURN_TYPE (*static_function)(Thread*),
		CALLBACK_CLASS_TYPE* class_instance,
		CALLBACK_RETURN_TYPE (CALLBACK_CLASS_TYPE::*class_method)(Thread*) )
{
	//create a task
	FunctionTask<FUNCTION_RETURN_TYPE,void>* ft = new FunctionTask<FUNCTION_RETURN_TYPE,void>();

	//static function setup
	ft->setmp_function(static_function);

	//# ----------------- prepare the notifier callback
	//create the callback container
	ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,void>* o1 =
			new ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,void>();

	//assign the callee objects to the containers
	o1->set_mp_callee_object(class_instance);

	//assign the functions to the containers
	o1->set_mp_object_method(class_method);

	//create the queue and enqueue the objects
	CallbackQueue* c_queue = new CallbackQueue();
	c_queue->enqueueCallback(o1);

	//register the queue at the function task
	ft->registerCallbackQueue(c_queue);

	return ft;
}

//generic for non-void params
template<typename FUNCTION_RETURN_TYPE, typename FUNCTION_PARAMETER_TYPE,
		typename CALLBACK_RETURN_TYPE, class CALLBACK_CLASS_TYPE,
		typename CALLBACK_PARAMETER_TYPE >
FunctionTask<FUNCTION_RETURN_TYPE,FUNCTION_PARAMETER_TYPE>* quickDeployCompleteTaskSRefDRef(
		FUNCTION_RETURN_TYPE (*static_function)(Thread*,FUNCTION_PARAMETER_TYPE),
		FUNCTION_PARAMETER_TYPE static_function_parameter,
		CALLBACK_CLASS_TYPE* class_instance,
		CALLBACK_RETURN_TYPE (CALLBACK_CLASS_TYPE::*class_method)(Thread*,CALLBACK_PARAMETER_TYPE),
		CALLBACK_PARAMETER_TYPE callback_parameter)
{
	//create a task
	FunctionTask<FUNCTION_RETURN_TYPE,FUNCTION_PARAMETER_TYPE>* ft = new FunctionTask<FUNCTION_RETURN_TYPE,FUNCTION_PARAMETER_TYPE>();

	//static function setup
	ft->setmp_function(static_function);
	ft->setmp_function_parameter(static_function_parameter);

	//# ----------------- prepare the notifier callback
	//create the callback container
	ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,CALLBACK_PARAMETER_TYPE>* o1 =
			new ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,CALLBACK_PARAMETER_TYPE>();

	//assign the callee objects to the containers
	o1->set_mp_callee_object(class_instance);

	//assign the functions to the containers
	o1->set_mp_object_method(class_method);

	//assign the parameters for the function
	o1->set_mp_parameter(callback_parameter);

	//create the queue and enqueue the objects
	CallbackQueue* c_queue = new CallbackQueue();
	c_queue->enqueueCallback(o1);

	//register the queue at the function task
	ft->registerCallbackQueue(c_queue);

	return ft;
}


//special for void
template<typename FUNCTION_RETURN_TYPE,
		typename CALLBACK_RETURN_TYPE, class CALLBACK_CLASS_TYPE>
FunctionTask<FUNCTION_RETURN_TYPE,void>* quickDeployCompleteTaskVSRef(
		FUNCTION_RETURN_TYPE (*static_function)(Thread*),
		CALLBACK_CLASS_TYPE* class_instance,
		CALLBACK_RETURN_TYPE (CALLBACK_CLASS_TYPE::*class_method)() )
{
	//create a task
	FunctionTask<FUNCTION_RETURN_TYPE,void>* ft = new FunctionTask<FUNCTION_RETURN_TYPE,void>();

	//static function setup
	ft->setmp_function(static_function);

	//# ----------------- prepare the notifier callback
	//create the callback container
	ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,CALLBACK_NO_THREAD_REF____>* o1 =
			new ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,CALLBACK_NO_THREAD_REF____>();

	//assign the callee objects to the containers
	o1->set_mp_callee_object(class_instance);

	//assign the functions to the containers
	o1->set_mp_object_method(class_method);

	//create the queue and enqueue the objects
	CallbackQueue* c_queue = new CallbackQueue();
	c_queue->enqueueCallback(o1);

	//register the queue at the function task
	ft->registerCallbackQueue(c_queue);

	return ft;
}


template<typename FUNCTION_RETURN_TYPE, typename FUNCTION_PARAMETER_TYPE,
		typename CALLBACK_RETURN_TYPE, class CALLBACK_CLASS_TYPE>
FunctionTask<FUNCTION_RETURN_TYPE,FUNCTION_PARAMETER_TYPE>* quickDeployCompleteTaskSRef(
		FUNCTION_RETURN_TYPE (*static_function)(Thread*,FUNCTION_PARAMETER_TYPE),
		FUNCTION_PARAMETER_TYPE static_function_parameter,
		CALLBACK_CLASS_TYPE* class_instance,
		CALLBACK_RETURN_TYPE (CALLBACK_CLASS_TYPE::*class_method)() )
{
	//create a task
	FunctionTask<FUNCTION_RETURN_TYPE,FUNCTION_PARAMETER_TYPE>* ft = new FunctionTask<FUNCTION_RETURN_TYPE,FUNCTION_PARAMETER_TYPE>();

	//static function setup
	ft->setmp_function(static_function);
	ft->setmp_function_parameter(static_function_parameter);

	//# ----------------- prepare the notifier callback
	//create the callback container
	ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,CALLBACK_NO_THREAD_REF____>* o1 =
			new ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,CALLBACK_NO_THREAD_REF____>();

	//assign the callee objects to the containers
	o1->set_mp_callee_object(class_instance);

	//assign the functions to the containers
	o1->set_mp_object_method(class_method);

	//create the queue and enqueue the objects
	CallbackQueue* c_queue = new CallbackQueue();
	c_queue->enqueueCallback(o1);

	//register the queue at the function task
	ft->registerCallbackQueue(c_queue);

	return ft;
}

//special for void parameter
template<typename FUNCTION_RETURN_TYPE,
		typename CALLBACK_RETURN_TYPE, class CALLBACK_CLASS_TYPE >
FunctionTask<FUNCTION_RETURN_TYPE,NO_STATIC_THREAD_REF___>* quickDeployCompleteTaskVDRef(
		FUNCTION_RETURN_TYPE (*static_function)(),
		CALLBACK_CLASS_TYPE* class_instance,
		CALLBACK_RETURN_TYPE (CALLBACK_CLASS_TYPE::*class_method)(Thread*))
{
	//create a task
	FunctionTask<FUNCTION_RETURN_TYPE,NO_STATIC_THREAD_REF___>* ft = new FunctionTask<FUNCTION_RETURN_TYPE,NO_STATIC_THREAD_REF___>();

	//static function setup
	ft->setmp_function(static_function);

	//# ----------------- prepare the notifier callback
	//create the callback container
	ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,void>* o1 =
			new ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,void>();

	//assign the callee objects to the containers
	o1->set_mp_callee_object(class_instance);

	//assign the functions to the containers
	o1->set_mp_object_method(class_method);

	//create the queue and enqueue the objects
	CallbackQueue* c_queue = new CallbackQueue();
	c_queue->enqueueCallback(o1);

	//register the queue at the function task
	ft->registerCallbackQueue(c_queue);

	return ft;
}

template<typename FUNCTION_RETURN_TYPE,
		typename CALLBACK_RETURN_TYPE, class CALLBACK_CLASS_TYPE,
		typename CALLBACK_PARAMETER_TYPE >
FunctionTask<FUNCTION_RETURN_TYPE,NO_STATIC_THREAD_REF___>* quickDeployCompleteTaskDRef(
		FUNCTION_RETURN_TYPE (*static_function)(),
		CALLBACK_CLASS_TYPE* class_instance,
		CALLBACK_RETURN_TYPE (CALLBACK_CLASS_TYPE::*class_method)(Thread*,CALLBACK_PARAMETER_TYPE),
		CALLBACK_PARAMETER_TYPE callback_parameter)
{
	//create a task
	FunctionTask<FUNCTION_RETURN_TYPE,NO_STATIC_THREAD_REF___>* ft = new FunctionTask<FUNCTION_RETURN_TYPE,NO_STATIC_THREAD_REF___>();

	//static function setup
	ft->setmp_function(static_function);

	//# ----------------- prepare the notifier callback
	//create the callback container
	ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,CALLBACK_PARAMETER_TYPE>* o1 =
			new ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,CALLBACK_PARAMETER_TYPE>();

	//assign the callee objects to the containers
	o1->set_mp_callee_object(class_instance);

	//assign the functions to the containers
	o1->set_mp_object_method(class_method);

	//assign the parameters for the function
	o1->set_mp_parameter(callback_parameter);

	//create the queue and enqueue the objects
	CallbackQueue* c_queue = new CallbackQueue();
	c_queue->enqueueCallback(o1);

	//register the queue at the function task
	ft->registerCallbackQueue(c_queue);

	return ft;
}


template<typename FUNCTION_RETURN_TYPE,
		typename CALLBACK_RETURN_TYPE, class CALLBACK_CLASS_TYPE >
FunctionTask<FUNCTION_RETURN_TYPE,NO_STATIC_THREAD_REF___>* quickDeployCompleteTask(
		FUNCTION_RETURN_TYPE (*static_function)(),
		CALLBACK_CLASS_TYPE* class_instance,
		CALLBACK_RETURN_TYPE (CALLBACK_CLASS_TYPE::*class_method)() )
{
	//create a task
	FunctionTask<FUNCTION_RETURN_TYPE,NO_STATIC_THREAD_REF___>* ft = new FunctionTask<FUNCTION_RETURN_TYPE,NO_STATIC_THREAD_REF___>();

	//static function setup
	ft->setmp_function(static_function);

	//# ----------------- prepare the notifier callback
	//create the callback container
	ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,CALLBACK_NO_THREAD_REF____>* o1 =
			new ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,CALLBACK_NO_THREAD_REF____>();

	//assign the callee objects to the containers
	o1->set_mp_callee_object(class_instance);

	//assign the functions to the containers
	o1->set_mp_object_method(class_method);

	//create the queue and enqueue the objects
	CallbackQueue* c_queue = new CallbackQueue();
	c_queue->enqueueCallback(o1);

	//register the queue at the function task
	ft->registerCallbackQueue(c_queue);

	return ft;
}



/**
 * This method returns a function task which can be deployed in a thread pool.
 * The function tasks assigned function is this methods parameter, yet the Tasks callback queue will be left empty.
 */

//special for void
template<typename FUNCTION_RETURN_TYPE>
FunctionTask<FUNCTION_RETURN_TYPE,void>* quickDeployStaticFunctionTaskVSRef(
		FUNCTION_RETURN_TYPE (*static_function)(Thread*) )
{
	//create a task
	FunctionTask<FUNCTION_RETURN_TYPE,void>* ft = new FunctionTask<FUNCTION_RETURN_TYPE,void>();

	//static function setup
	ft->setmp_function(static_function);

	return ft;
}

template<typename FUNCTION_RETURN_TYPE, typename FUNCTION_PARAMETER_TYPE>
FunctionTask<FUNCTION_RETURN_TYPE,FUNCTION_PARAMETER_TYPE>* quickDeployStaticFunctionTaskSRef(
		FUNCTION_RETURN_TYPE (*static_function)(Thread*,FUNCTION_PARAMETER_TYPE),
		FUNCTION_PARAMETER_TYPE static_function_parameter)
{
	//create a task
	FunctionTask<FUNCTION_RETURN_TYPE,FUNCTION_PARAMETER_TYPE>* ft = new FunctionTask<FUNCTION_RETURN_TYPE,FUNCTION_PARAMETER_TYPE>();

	//static function setup
	ft->setmp_function(static_function);
	ft->setmp_function_parameter(static_function_parameter);

	return ft;
}

template<typename FUNCTION_RETURN_TYPE>
FunctionTask<FUNCTION_RETURN_TYPE,NO_STATIC_THREAD_REF___>* quickDeployStaticFunctionTask(
		FUNCTION_RETURN_TYPE (*static_function)() )
{
	//create a task
	FunctionTask<FUNCTION_RETURN_TYPE,NO_STATIC_THREAD_REF___>* ft = new FunctionTask<FUNCTION_RETURN_TYPE,NO_STATIC_THREAD_REF___>();

	//static function setup
	ft->setmp_function(static_function);

	return ft;
}

/**
 * This method returns a function task which can be deployed in a thread pool.
 * The function tasks assigned function is empty, yet its callback queue will be filled with a new queue
 * which contains a single element, namely an ObjectCallback instance which will execute the given method of the
 * given object.
 */

//special for void
template<typename CALLBACK_RETURN_TYPE, class CALLBACK_CLASS_TYPE >
FunctionTask<CALLBACK_RETURN_TYPE,NO_STATIC_THREAD_REF___>* quickDeployCallbackTaskVDRef(
		CALLBACK_CLASS_TYPE* class_instance,
		CALLBACK_RETURN_TYPE (CALLBACK_CLASS_TYPE::*class_method)(Thread*) )
{
	//create a task
	FunctionTask<CALLBACK_RETURN_TYPE,NO_STATIC_THREAD_REF___>* ft = new FunctionTask<CALLBACK_RETURN_TYPE,NO_STATIC_THREAD_REF___>();

	//# ----------------- prepare the notifier callback
	//create the callback container
	ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,void>* o1 =
			new ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,void>();

	//assign the callee objects to the containers
	o1->set_mp_callee_object(class_instance);

	//assign the functions to the containers
	o1->set_mp_object_method(class_method);

	//create the queue and enqueue the objects
	CallbackQueue* c_queue = new CallbackQueue();
	c_queue->enqueueCallback(o1);

	//register the queue at the function task
	ft->registerCallbackQueue(c_queue);

	return ft;
}

template<typename CALLBACK_RETURN_TYPE, class CALLBACK_CLASS_TYPE,
		typename CALLBACK_PARAMETER_TYPE >
FunctionTask<CALLBACK_RETURN_TYPE,NO_STATIC_THREAD_REF___>* quickDeployCallbackTaskDRef(
		CALLBACK_CLASS_TYPE* class_instance,
		CALLBACK_RETURN_TYPE (CALLBACK_CLASS_TYPE::*class_method)(Thread*, CALLBACK_PARAMETER_TYPE),
		CALLBACK_PARAMETER_TYPE callback_parameter)
{
	//create a task
	FunctionTask<CALLBACK_RETURN_TYPE,NO_STATIC_THREAD_REF___>* ft = new FunctionTask<CALLBACK_RETURN_TYPE,NO_STATIC_THREAD_REF___>();

	//# ----------------- prepare the notifier callback
	//create the callback container
	ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,CALLBACK_PARAMETER_TYPE>* o1 =
			new ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,CALLBACK_PARAMETER_TYPE>();

	//assign the callee objects to the containers
	o1->set_mp_callee_object(class_instance);

	//assign the functions to the containers
	o1->set_mp_object_method(class_method);

	//assign the parameters for the function
	o1->set_mp_parameter(callback_parameter);

	//create the queue and enqueue the objects
	CallbackQueue* c_queue = new CallbackQueue();
	c_queue->enqueueCallback(o1);

	//register the queue at the function task
	ft->registerCallbackQueue(c_queue);

	return ft;
}

template<typename CALLBACK_RETURN_TYPE, class CALLBACK_CLASS_TYPE>
FunctionTask<CALLBACK_RETURN_TYPE,NO_STATIC_THREAD_REF___>* quickDeployCallbackTask(
		CALLBACK_CLASS_TYPE* class_instance,
		CALLBACK_RETURN_TYPE (CALLBACK_CLASS_TYPE::*class_method)())
{
	//create a task
	FunctionTask<CALLBACK_RETURN_TYPE,NO_STATIC_THREAD_REF___>* ft = new FunctionTask<CALLBACK_RETURN_TYPE,NO_STATIC_THREAD_REF___>();

	//# ----------------- prepare the notifier callback
	//create the callback container
	ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,CALLBACK_NO_THREAD_REF____>* o1 =
			new ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,CALLBACK_NO_THREAD_REF____>();

	//assign the callee objects to the containers
	o1->set_mp_callee_object(class_instance);

	//assign the functions to the containers
	o1->set_mp_object_method(class_method);

	//create the queue and enqueue the objects
	CallbackQueue* c_queue = new CallbackQueue();
	c_queue->enqueueCallback(o1);

	//register the queue at the function task
	ft->registerCallbackQueue(c_queue);

	return ft;
}



//**************** thread tasks ********************************************************************

/**
 * These methods return function tasks which don't require any thread pool in order to be executed.
 */

//void specials
template<typename FUNCTION_RETURN_TYPE,
		typename CALLBACK_RETURN_TYPE, class CALLBACK_CLASS_TYPE,
		typename CALLBACK_PARAMETER_TYPE >
ThreadFunctionTask<FUNCTION_RETURN_TYPE,void>* quickDeployCompleteThreadTaskVSRefDRef(
		FUNCTION_RETURN_TYPE (*static_function)(Thread*),
		CALLBACK_CLASS_TYPE* class_instance,
		CALLBACK_RETURN_TYPE (CALLBACK_CLASS_TYPE::*class_method)(Thread*,CALLBACK_PARAMETER_TYPE),
		CALLBACK_PARAMETER_TYPE callback_parameter)
{
	//create a task
	ThreadFunctionTask<FUNCTION_RETURN_TYPE,void>* ft = new ThreadFunctionTask<FUNCTION_RETURN_TYPE,void>();

	//static function setup
	ft->setmp_function(static_function);

	//# ----------------- prepare the notifier callback
	//create the callback container
	ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,CALLBACK_PARAMETER_TYPE>* o1 =
			new ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,CALLBACK_PARAMETER_TYPE>();

	//assign the callee objects to the containers
	o1->set_mp_callee_object(class_instance);

	//assign the functions to the containers
	o1->set_mp_object_method(class_method);

	//assign the parameters for the function
	o1->set_mp_parameter(callback_parameter);

	//create the queue and enqueue the objects
	CallbackQueue* c_queue = new CallbackQueue();
	c_queue->enqueueCallback(o1);

	//register the queue at the function task
	ft->registerCallbackQueue(c_queue);

	return ft;
}

template<typename FUNCTION_RETURN_TYPE, typename FUNCTION_PARAMETER_TYPE,
		typename CALLBACK_RETURN_TYPE, class CALLBACK_CLASS_TYPE >
ThreadFunctionTask<FUNCTION_RETURN_TYPE,FUNCTION_PARAMETER_TYPE>* quickDeployCompleteThreadTaskSRefVDRef(
		FUNCTION_RETURN_TYPE (*static_function)(Thread*,FUNCTION_PARAMETER_TYPE),
		FUNCTION_PARAMETER_TYPE static_function_parameter,
		CALLBACK_CLASS_TYPE* class_instance,
		CALLBACK_RETURN_TYPE (CALLBACK_CLASS_TYPE::*class_method)(Thread*) )
{
	//create a task
	ThreadFunctionTask<FUNCTION_RETURN_TYPE,FUNCTION_PARAMETER_TYPE>* ft = new ThreadFunctionTask<FUNCTION_RETURN_TYPE,FUNCTION_PARAMETER_TYPE>();

	//static function setup
	ft->setmp_function(static_function);
	ft->setmp_function_parameter(static_function_parameter);

	//# ----------------- prepare the notifier callback
	//create the callback container
	ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,void>* o1 =
			new ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,void>();

	//assign the callee objects to the containers
	o1->set_mp_callee_object(class_instance);

	//assign the functions to the containers
	o1->set_mp_object_method(class_method);

	//create the queue and enqueue the objects
	CallbackQueue* c_queue = new CallbackQueue();
	c_queue->enqueueCallback(o1);

	//register the queue at the function task
	ft->registerCallbackQueue(c_queue);

	return ft;
}

template<typename FUNCTION_RETURN_TYPE,
		typename CALLBACK_RETURN_TYPE, class CALLBACK_CLASS_TYPE >
ThreadFunctionTask<FUNCTION_RETURN_TYPE,void>* quickDeployCompleteThreadTaskVSRefVDRef(
		FUNCTION_RETURN_TYPE (*static_function)(Thread*),
		CALLBACK_CLASS_TYPE* class_instance,
		CALLBACK_RETURN_TYPE (CALLBACK_CLASS_TYPE::*class_method)(Thread*) )
{
	//create a task
	ThreadFunctionTask<FUNCTION_RETURN_TYPE,void>* ft = new ThreadFunctionTask<FUNCTION_RETURN_TYPE,void>();

	//static function setup
	ft->setmp_function(static_function);

	//# ----------------- prepare the notifier callback
	//create the callback container
	ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,void>* o1 =
			new ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,void>();

	//assign the callee objects to the containers
	o1->set_mp_callee_object(class_instance);

	//assign the functions to the containers
	o1->set_mp_object_method(class_method);

	//create the queue and enqueue the objects
	CallbackQueue* c_queue = new CallbackQueue();
	c_queue->enqueueCallback(o1);

	//register the queue at the function task
	ft->registerCallbackQueue(c_queue);

	return ft;
}

//generic for non-void params
template<typename FUNCTION_RETURN_TYPE, typename FUNCTION_PARAMETER_TYPE,
		typename CALLBACK_RETURN_TYPE, class CALLBACK_CLASS_TYPE,
		typename CALLBACK_PARAMETER_TYPE >
ThreadFunctionTask<FUNCTION_RETURN_TYPE,FUNCTION_PARAMETER_TYPE>* quickDeployCompleteThreadTaskSRefDRef(
		FUNCTION_RETURN_TYPE (*static_function)(Thread*,FUNCTION_PARAMETER_TYPE),
		FUNCTION_PARAMETER_TYPE static_function_parameter,
		CALLBACK_CLASS_TYPE* class_instance,
		CALLBACK_RETURN_TYPE (CALLBACK_CLASS_TYPE::*class_method)(Thread*,CALLBACK_PARAMETER_TYPE),
		CALLBACK_PARAMETER_TYPE callback_parameter)
{
	//create a task
	ThreadFunctionTask<FUNCTION_RETURN_TYPE,FUNCTION_PARAMETER_TYPE>* ft = new ThreadFunctionTask<FUNCTION_RETURN_TYPE,FUNCTION_PARAMETER_TYPE>();

	//static function setup
	ft->setmp_function(static_function);
	ft->setmp_function_parameter(static_function_parameter);

	//# ----------------- prepare the notifier callback
	//create the callback container
	ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,CALLBACK_PARAMETER_TYPE>* o1 =
			new ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,CALLBACK_PARAMETER_TYPE>();

	//assign the callee objects to the containers
	o1->set_mp_callee_object(class_instance);

	//assign the functions to the containers
	o1->set_mp_object_method(class_method);

	//assign the parameters for the function
	o1->set_mp_parameter(callback_parameter);

	//create the queue and enqueue the objects
	CallbackQueue* c_queue = new CallbackQueue();
	c_queue->enqueueCallback(o1);

	//register the queue at the function task
	ft->registerCallbackQueue(c_queue);

	return ft;
}


//special for void
template<typename FUNCTION_RETURN_TYPE,
		typename CALLBACK_RETURN_TYPE, class CALLBACK_CLASS_TYPE>
ThreadFunctionTask<FUNCTION_RETURN_TYPE,void>* quickDeployCompleteThreadTaskVSRef(
		FUNCTION_RETURN_TYPE (*static_function)(Thread*),
		CALLBACK_CLASS_TYPE* class_instance,
		CALLBACK_RETURN_TYPE (CALLBACK_CLASS_TYPE::*class_method)() )
{
	//create a task
	ThreadFunctionTask<FUNCTION_RETURN_TYPE,void>* ft = new ThreadFunctionTask<FUNCTION_RETURN_TYPE,void>();

	//static function setup
	ft->setmp_function(static_function);

	//# ----------------- prepare the notifier callback
	//create the callback container
	ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,CALLBACK_NO_THREAD_REF____>* o1 =
			new ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,CALLBACK_NO_THREAD_REF____>();

	//assign the callee objects to the containers
	o1->set_mp_callee_object(class_instance);

	//assign the functions to the containers
	o1->set_mp_object_method(class_method);

	//create the queue and enqueue the objects
	CallbackQueue* c_queue = new CallbackQueue();
	c_queue->enqueueCallback(o1);

	//register the queue at the function task
	ft->registerCallbackQueue(c_queue);

	return ft;
}


template<typename FUNCTION_RETURN_TYPE, typename FUNCTION_PARAMETER_TYPE,
		typename CALLBACK_RETURN_TYPE, class CALLBACK_CLASS_TYPE>
ThreadFunctionTask<FUNCTION_RETURN_TYPE,FUNCTION_PARAMETER_TYPE>* quickDeployCompleteThreadTaskSRef(
		FUNCTION_RETURN_TYPE (*static_function)(Thread*,FUNCTION_PARAMETER_TYPE),
		FUNCTION_PARAMETER_TYPE static_function_parameter,
		CALLBACK_CLASS_TYPE* class_instance,
		CALLBACK_RETURN_TYPE (CALLBACK_CLASS_TYPE::*class_method)() )
{
	//create a task
	ThreadFunctionTask<FUNCTION_RETURN_TYPE,FUNCTION_PARAMETER_TYPE>* ft = new ThreadFunctionTask<FUNCTION_RETURN_TYPE,FUNCTION_PARAMETER_TYPE>();

	//static function setup
	ft->setmp_function(static_function);
	ft->setmp_function_parameter(static_function_parameter);

	//# ----------------- prepare the notifier callback
	//create the callback container
	ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,CALLBACK_NO_THREAD_REF____>* o1 =
			new ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,CALLBACK_NO_THREAD_REF____>();

	//assign the callee objects to the containers
	o1->set_mp_callee_object(class_instance);

	//assign the functions to the containers
	o1->set_mp_object_method(class_method);

	//create the queue and enqueue the objects
	CallbackQueue* c_queue = new CallbackQueue();
	c_queue->enqueueCallback(o1);

	//register the queue at the function task
	ft->registerCallbackQueue(c_queue);

	return ft;
}

//special for void parameter
template<typename FUNCTION_RETURN_TYPE,
		typename CALLBACK_RETURN_TYPE, class CALLBACK_CLASS_TYPE >
ThreadFunctionTask<FUNCTION_RETURN_TYPE,NO_STATIC_THREAD_REF___>* quickDeployCompleteThreadTaskVDRef(
		FUNCTION_RETURN_TYPE (*static_function)(),
		CALLBACK_CLASS_TYPE* class_instance,
		CALLBACK_RETURN_TYPE (CALLBACK_CLASS_TYPE::*class_method)(Thread*))
{
	//create a task
	ThreadFunctionTask<FUNCTION_RETURN_TYPE,NO_STATIC_THREAD_REF___>* ft = new ThreadFunctionTask<FUNCTION_RETURN_TYPE,NO_STATIC_THREAD_REF___>();

	//static function setup
	ft->setmp_function(static_function);

	//# ----------------- prepare the notifier callback
	//create the callback container
	ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,void>* o1 =
			new ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,void>();

	//assign the callee objects to the containers
	o1->set_mp_callee_object(class_instance);

	//assign the functions to the containers
	o1->set_mp_object_method(class_method);

	//create the queue and enqueue the objects
	CallbackQueue* c_queue = new CallbackQueue();
	c_queue->enqueueCallback(o1);

	//register the queue at the function task
	ft->registerCallbackQueue(c_queue);

	return ft;
}

template<typename FUNCTION_RETURN_TYPE,
		typename CALLBACK_RETURN_TYPE, class CALLBACK_CLASS_TYPE,
		typename CALLBACK_PARAMETER_TYPE >
ThreadFunctionTask<FUNCTION_RETURN_TYPE,NO_STATIC_THREAD_REF___>* quickDeployCompleteThreadTaskDRef(
		FUNCTION_RETURN_TYPE (*static_function)(),
		CALLBACK_CLASS_TYPE* class_instance,
		CALLBACK_RETURN_TYPE (CALLBACK_CLASS_TYPE::*class_method)(Thread*,CALLBACK_PARAMETER_TYPE),
		CALLBACK_PARAMETER_TYPE callback_parameter)
{
	//create a task
	ThreadFunctionTask<FUNCTION_RETURN_TYPE,NO_STATIC_THREAD_REF___>* ft = new ThreadFunctionTask<FUNCTION_RETURN_TYPE,NO_STATIC_THREAD_REF___>();

	//static function setup
	ft->setmp_function(static_function);

	//# ----------------- prepare the notifier callback
	//create the callback container
	ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,CALLBACK_PARAMETER_TYPE>* o1 =
			new ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,CALLBACK_PARAMETER_TYPE>();

	//assign the callee objects to the containers
	o1->set_mp_callee_object(class_instance);

	//assign the functions to the containers
	o1->set_mp_object_method(class_method);

	//assign the parameters for the function
	o1->set_mp_parameter(callback_parameter);

	//create the queue and enqueue the objects
	CallbackQueue* c_queue = new CallbackQueue();
	c_queue->enqueueCallback(o1);

	//register the queue at the function task
	ft->registerCallbackQueue(c_queue);

	return ft;
}


template<typename FUNCTION_RETURN_TYPE,
		typename CALLBACK_RETURN_TYPE, class CALLBACK_CLASS_TYPE >
ThreadFunctionTask<FUNCTION_RETURN_TYPE,NO_STATIC_THREAD_REF___>* quickDeployCompleteThreadTask(
		FUNCTION_RETURN_TYPE (*static_function)(),
		CALLBACK_CLASS_TYPE* class_instance,
		CALLBACK_RETURN_TYPE (CALLBACK_CLASS_TYPE::*class_method)() )
{
	//create a task
	ThreadFunctionTask<FUNCTION_RETURN_TYPE,NO_STATIC_THREAD_REF___>* ft = new ThreadFunctionTask<FUNCTION_RETURN_TYPE,NO_STATIC_THREAD_REF___>();

	//static function setup
	ft->setmp_function(static_function);

	//# ----------------- prepare the notifier callback
	//create the callback container
	ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,CALLBACK_NO_THREAD_REF____>* o1 =
			new ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,CALLBACK_NO_THREAD_REF____>();

	//assign the callee objects to the containers
	o1->set_mp_callee_object(class_instance);

	//assign the functions to the containers
	o1->set_mp_object_method(class_method);

	//create the queue and enqueue the objects
	CallbackQueue* c_queue = new CallbackQueue();
	c_queue->enqueueCallback(o1);

	//register the queue at the function task
	ft->registerCallbackQueue(c_queue);

	return ft;
}



/**
 * This method returns a function task which can be deployed in a thread pool.
 * The function tasks assigned function is this methods parameter, yet the Tasks callback queue will be left empty.
 */

//special for void
template<typename FUNCTION_RETURN_TYPE>
ThreadFunctionTask<FUNCTION_RETURN_TYPE,void>* quickDeployStaticFunctionThreadTaskVSRef(
		FUNCTION_RETURN_TYPE (*static_function)(Thread*) )
{
	//create a task
	ThreadFunctionTask<FUNCTION_RETURN_TYPE,void>* ft = new ThreadFunctionTask<FUNCTION_RETURN_TYPE,void>();

	//static function setup
	ft->setmp_function(static_function);

	return ft;
}

template<typename FUNCTION_RETURN_TYPE, typename FUNCTION_PARAMETER_TYPE>
ThreadFunctionTask<FUNCTION_RETURN_TYPE,FUNCTION_PARAMETER_TYPE>* quickDeployStaticFunctionThreadTaskSRef(
		FUNCTION_RETURN_TYPE (*static_function)(Thread*,FUNCTION_PARAMETER_TYPE),
		FUNCTION_PARAMETER_TYPE static_function_parameter)
{
	//create a task
	ThreadFunctionTask<FUNCTION_RETURN_TYPE,FUNCTION_PARAMETER_TYPE>* ft = new ThreadFunctionTask<FUNCTION_RETURN_TYPE,FUNCTION_PARAMETER_TYPE>();

	//static function setup
	ft->setmp_function(static_function);
	ft->setmp_function_parameter(static_function_parameter);

	return ft;
}

template<typename FUNCTION_RETURN_TYPE>
ThreadFunctionTask<FUNCTION_RETURN_TYPE,NO_STATIC_THREAD_REF___>* quickDeployStaticFunctionThreadTask(
		FUNCTION_RETURN_TYPE (*static_function)() )
{
	//create a task
	ThreadFunctionTask<FUNCTION_RETURN_TYPE,NO_STATIC_THREAD_REF___>* ft = new ThreadFunctionTask<FUNCTION_RETURN_TYPE,NO_STATIC_THREAD_REF___>();

	//static function setup
	ft->setmp_function(static_function);

	return ft;
}

/**
 * This method returns a function task which can be deployed in a thread pool.
 * The function tasks assigned function is empty, yet its callback queue will be filled with a new queue
 * which contains a single element, namely an ObjectCallback instance which will execute the given method of the
 * given object.
 */

//special for void
template<typename CALLBACK_RETURN_TYPE, class CALLBACK_CLASS_TYPE >
ThreadFunctionTask<CALLBACK_RETURN_TYPE,NO_STATIC_THREAD_REF___>* quickDeployCallbackThreadTaskVDRef(
		CALLBACK_CLASS_TYPE* class_instance,
		CALLBACK_RETURN_TYPE (CALLBACK_CLASS_TYPE::*class_method)(Thread*) )
{
	//create a task
	ThreadFunctionTask<CALLBACK_RETURN_TYPE,NO_STATIC_THREAD_REF___>* ft = new ThreadFunctionTask<CALLBACK_RETURN_TYPE,NO_STATIC_THREAD_REF___>();

	//# ----------------- prepare the notifier callback
	//create the callback container
	ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,void>* o1 =
			new ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,void>();

	//assign the callee objects to the containers
	o1->set_mp_callee_object(class_instance);

	//assign the functions to the containers
	o1->set_mp_object_method(class_method);

	//create the queue and enqueue the objects
	CallbackQueue* c_queue = new CallbackQueue();
	c_queue->enqueueCallback(o1);

	//register the queue at the function task
	ft->registerCallbackQueue(c_queue);

	return ft;
}

template<typename CALLBACK_RETURN_TYPE, class CALLBACK_CLASS_TYPE,
		typename CALLBACK_PARAMETER_TYPE >
ThreadFunctionTask<CALLBACK_RETURN_TYPE,NO_STATIC_THREAD_REF___>* quickDeployCallbackThreadTaskDRef(
		CALLBACK_CLASS_TYPE* class_instance,
		CALLBACK_RETURN_TYPE (CALLBACK_CLASS_TYPE::*class_method)(Thread*, CALLBACK_PARAMETER_TYPE),
		CALLBACK_PARAMETER_TYPE callback_parameter)
{
	//create a task
	ThreadFunctionTask<CALLBACK_RETURN_TYPE,NO_STATIC_THREAD_REF___>* ft = new ThreadFunctionTask<CALLBACK_RETURN_TYPE,NO_STATIC_THREAD_REF___>();

	//# ----------------- prepare the notifier callback
	//create the callback container
	ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,CALLBACK_PARAMETER_TYPE>* o1 =
			new ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,CALLBACK_PARAMETER_TYPE>();

	//assign the callee objects to the containers
	o1->set_mp_callee_object(class_instance);

	//assign the functions to the containers
	o1->set_mp_object_method(class_method);

	//assign the parameters for the function
	o1->set_mp_parameter(callback_parameter);

	//create the queue and enqueue the objects
	CallbackQueue* c_queue = new CallbackQueue();
	c_queue->enqueueCallback(o1);

	//register the queue at the function task
	ft->registerCallbackQueue(c_queue);

	return ft;
}

template<typename CALLBACK_RETURN_TYPE, class CALLBACK_CLASS_TYPE>
ThreadFunctionTask<CALLBACK_RETURN_TYPE,NO_STATIC_THREAD_REF___>* quickDeployCallbackThreadTask(
		CALLBACK_CLASS_TYPE* class_instance,
		CALLBACK_RETURN_TYPE (CALLBACK_CLASS_TYPE::*class_method)())
{
	//create a task
	ThreadFunctionTask<CALLBACK_RETURN_TYPE,NO_STATIC_THREAD_REF___>* ft = new ThreadFunctionTask<CALLBACK_RETURN_TYPE,NO_STATIC_THREAD_REF___>();

	//# ----------------- prepare the notifier callback
	//create the callback container
	ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,CALLBACK_NO_THREAD_REF____>* o1 =
			new ThreadObjectCallback<CALLBACK_RETURN_TYPE,CALLBACK_CLASS_TYPE,CALLBACK_NO_THREAD_REF____>();

	//assign the callee objects to the containers
	o1->set_mp_callee_object(class_instance);

	//assign the functions to the containers
	o1->set_mp_object_method(class_method);

	//create the queue and enqueue the objects
	CallbackQueue* c_queue = new CallbackQueue();
	c_queue->enqueueCallback(o1);

	//register the queue at the function task
	ft->registerCallbackQueue(c_queue);

	return ft;
}



}
#endif /* TASKHELPER_H_ */
