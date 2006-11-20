/** 
 * @file  LeftAndRight.h
 *
 * @brief Declaration for LeftAndRight testcase.
 */

#ifndef _LEFTANDRIGHT_H_
#define _LEFTANDRIGHT_H_

#include <cppunit/extensions/HelperMacros.h>


/**
 * @brief Test loading both (left, right) paths projectfile.
 * This testcase loads projectfile that has both paths defined.
 * We test loading the file, getting file paths correctly and making
 * sure all other parameters are correctly non-defined when read.
 */
class LeftAndRight : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( LeftAndRight );
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

#endif // _LEFTANDRIGHT_H_
