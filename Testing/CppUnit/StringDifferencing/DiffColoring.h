/** 
* @file  DiffColoring.h
*
* @brief Declaration of DiffColoring test cases.
*
* This class doesn't check if the current number of differences found. It check
* the differences positions. Wrong difference positions cause bugs in the line
* coloring feature.
*
*/

#ifndef _DIFF_COLORING_H_INCLUDED_
#define _DIFF_COLORING_H_INCLUDED_

#include <cppunit/extensions/HelperMacros.h>

/**
* @brief Test cases for the line coloring feature.
*/
class DiffColoring : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( DiffColoring );
	CPPUNIT_TEST( OneWord );
	CPPUNIT_TEST( OneDiff );
	CPPUNIT_TEST( OneWordDiff );
	CPPUNIT_TEST( DifferentLength );
	CPPUNIT_TEST( DiffWithEmpty );
	CPPUNIT_TEST( PunctuationChar );
	CPPUNIT_TEST( PunctuationWord );
	CPPUNIT_TEST( Bug1491334 );
	CPPUNIT_TEST( Bug1639453 );
	CPPUNIT_TEST( Bug1683061 );
	CPPUNIT_TEST( Bug1714088 );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

protected:
	void OneWord();
	void OneDiff();
	void OneWordDiff();
	void DifferentLength();
	void DiffWithEmpty();
	void PunctuationChar();
	void PunctuationWord();
	void Bug1491334();
	void Bug1639453();
	void Bug1683061();
	void Bug1714088();
};

#endif // _DIFF_COLORING_H_INCLUDED_
