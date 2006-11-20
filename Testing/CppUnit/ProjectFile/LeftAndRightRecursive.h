/** 
 * @file  LeftAndRightRecursive.h
 *
 * @brief Declaration for LeftAndRightRecursive testcase.
 */

#ifndef _LEFTANDRIGHTRECURSIVE_H_
#define _LEFTANDRIGHTRECURSIVE_H_

#include <cppunit/extensions/HelperMacros.h>


/**
 * @brief Test loading both (left, right) paths projectfile.
 * This testcase loads projectfile that has both paths defined.
 * This projecfile has subfolders defined and enabled, so it is recursive.
 * We test loading the file, getting file paths correctly and making
 * sure all other parameters are correctly non-defined when read.
 */
class LeftAndRightRecursive : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( LeftAndRightRecursive );
	CPPUNIT_TEST( Load );
	CPPUNIT_TEST( GetLeftPath );
	CPPUNIT_TEST( GetRightPath );
	CPPUNIT_TEST( GetSubfolders );
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

#endif // _LEFTANDRIGHTRECURSIVE_H_
