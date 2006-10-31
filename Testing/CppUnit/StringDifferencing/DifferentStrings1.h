/** 
 * @file  DifferentStrings1.h
 *
 * @brief Declaration for simple different strings tests..
 */

#ifndef _DIFFERENT_STRINGS1_H_
#define _DIFFERENT_STRINGS1_H_

#include <cppunit/extensions/HelperMacros.h>


/**
 * @brief Simple different strings testing.
 */
class DifferentStrings1 : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( DifferentStrings1 );
  CPPUNIT_TEST( CasesDiffer1 );
  CPPUNIT_TEST( CasesDiffer2 );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

protected:
	void CasesDiffer1();
	void CasesDiffer2();
};

#endif // DIFFERENT_STRINGS1
