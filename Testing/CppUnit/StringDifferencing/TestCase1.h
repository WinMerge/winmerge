/** 
 * @file  TestCase1.h
 *
 * @brief Declaration for first simple test case.
 */

#ifndef _TESTCASE1_H_
#define _TESTCASE1_H_

#include <cppunit/extensions/HelperMacros.h>


/**
 * @brief First simple testcase for StringDifferencing.
 * This is simple test case meant more of an example for writing testcases.
 * But of course this does some useful testing also. :)
 */
class TestCase1 : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TestCase1 );
  CPPUNIT_TEST( Identical1 );
  CPPUNIT_TEST( Difference1 );
  CPPUNIT_TEST( Difference2 );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

protected:
	void Identical1();

	void Difference1();
	void Difference2();
};

#endif // _TESTCASE1_H_
