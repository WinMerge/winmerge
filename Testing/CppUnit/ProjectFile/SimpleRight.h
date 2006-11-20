/** 
 * @file  SimpleRight.h
 *
 * @brief Declaration for SimpleRight testcase.
 */

#ifndef _SIMPLERIGHT_H_
#define _SIMPLERIGHT_H_

#include <cppunit/extensions/HelperMacros.h>


/**
 * @brief Test loading right-path-only projectfile.
 * This testcase loads projectfile that has only right path defined.
 * We test loading the file, getting left file path correctly and making
 * sure all other parameters are correctly non-defined when read.
 */
class SimpleRight : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( SimpleRight );
	CPPUNIT_TEST( Load );
	CPPUNIT_TEST( GetRightPath );
	CPPUNIT_TEST( GetLeftPath );
	CPPUNIT_TEST( GetSubfolders );
	CPPUNIT_TEST( GetFilter );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

protected:
	void Load();
	void GetRightPath();
	void GetLeftPath();
	void GetSubfolders();
	void GetFilter();

private:
	ProjectFile * m_pProjectFile;

};

#endif // _SIMPLERIGHT_H_
