/*
 * UnitTest.cpp
 *
 *  Created on: 05.10.2016
 *      Author: Darius Malysiak
 */

#include "UnitTest.h"

namespace Lazarus {

UnitTest::UnitTest() {
	// TODO Auto-generated constructor stub

}

UnitTest::~UnitTest()
{
	m_tests.eraseWithDestructor();
}

/**
 * The UnitTest instance takes ownership over the test cases.
 **/
void UnitTest::addTest(UnitTestCase* test)
{
	m_tests.push(test);
}

FastKTuple<bool> UnitTest::runTests()
{
	FastKTuple<bool> res(m_tests.getm_size());

	for(unsigned int i=0;i<m_tests.getm_size();++i)
	{
		res[i] = m_tests[i]->test();
	}

	return res;
}

} /* namespace Lazarus */
