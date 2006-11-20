/** 
 * @file  SimpleLeft.h
 *
 * @brief Declaration for SimpleLeft testcase.
 */

#ifndef _SIMPLELEFT_H_
#define _SIMPLELEFT_H_

#include <cppunit/extensions/HelperMacros.h>


/**
 * @brief Test loading left-path-only projectfile.
 * This testcase loads projectfile that has only left path defined.
 * We test loading the file, getting left file path correctly and making
 * sure all other parameters are correctly non-defined when read.
 */
class SimpleLeft : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( SimpleLeft );
	CPPUNIT_TEST( Load );
	CPPUNIT_TEST( GetLeftPath );
	CPPUNIT_TEST( GetRightPath );
	CPPUNIT_TEST( GetSubfolders);
	CPPUNIT_TEST( GetFilter );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

protected:
	void Load();
	void GetLeftPath();
	void GetRightPath();
	void GetSubfolders();
	void GetFilter();

private:
	ProjectFile * m_pProjectFile;

};

#endif // _SIMPLELEFT_H_
