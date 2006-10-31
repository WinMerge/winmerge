/** 
 * @file  IdenticalStrings1.h
 *
 * @brief Declaration for identical strings.
 */

#ifndef _IDENTICAL_STRINGS1_H_
#define _IDENTICAL_STRINGS1_H_

#include <cppunit/extensions/HelperMacros.h>


/**
 * @brief Identical string testing with different options.
 */
class IdenticalStrings1 : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( IdenticalStrings1 );
  CPPUNIT_TEST( IdenticalWord1 );
  CPPUNIT_TEST( IdenticalWord2 );
  CPPUNIT_TEST( IdenticalWord3 );
  CPPUNIT_TEST( IdenticalWord4 );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

protected:
	void IdenticalWord1();
	void IdenticalWord2();
	void IdenticalWord3();
	void IdenticalWord4();
};

#endif // IDENTICAL_STRINGS1
