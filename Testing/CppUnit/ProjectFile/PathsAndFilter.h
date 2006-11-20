/** 
 * @file  PathsAndFilter.h
 *
 * @brief Declaration for PathsAndFilter testcase.
 */

#ifndef _PATHSANDFILTER_H_
#define _PATHSANDFILTER_H_

#include <cppunit/extensions/HelperMacros.h>


/**
 * @brief Test loading both (left, right) paths and filter projectfile.
 * This testcase loads projectfile that has both paths and filter defined.
 * We test loading the file, getting both paths and filter correctly.
 */
class PathsAndFilter : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( PathsAndFilter );
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

#endif // _PATHSANDFILTER_H_
