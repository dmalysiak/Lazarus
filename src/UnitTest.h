/*
 * UnitTest.h
 *
 *  Created on: 05.10.2016
 *      Author: Darius Malysiak
 */

#ifndef CORE_UNITTEST_H_
#define CORE_UNITTEST_H_

#include "BaseObject.h"
#include "DataStructures/FastKTuple.h"

namespace Lazarus {

class UnitTestCase: public BaseObject{
public:

	/**
	 * Implement the test.
	 * */
	virtual bool test() = 0;

};

class UnitTest: public BaseObject {
public:
	UnitTest();
	virtual ~UnitTest();

	/**
	 * The UnitTest instance takes ownership over the test cases.
	 * */
	void addTest(UnitTestCase* test);

	/**
	 * Invokes all registered tests and returns their result.
	 * */
	FastKTuple<bool> runTests();

private:
	FastKTuple<UnitTestCase*> m_tests;
};

} /* namespace Lazarus */

#endif /* CORE_UNITTEST_H_ */
