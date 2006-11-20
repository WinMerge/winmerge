/** 
 * @file  LeftAndRightNonRecursive.h
 *
 * @brief Declaration for LeftAndRightNonRecursive testcase.
 */

#ifndef _LEFTANDRIGHTNONRECURSIVE_H_
#define _LEFTANDRIGHTNONRECURSIVE_H_

#include <cppunit/extensions/HelperMacros.h>


/**
 * @brief Test loading both (left, right) paths projectfile.
 * This testcase loads projectfile that has both paths defined.
 * This projecfile has subfolders defined but disabled.
 * We test loading the file, getting file paths correctly and making
 * sure all other parameters are correctly non-defined when read.
 */
class LeftAndRightNonRecursive : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( LeftAndRightNonRecursive );
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

#endif // _LEFTANDRIGHTNONRECURSIVE_H_
