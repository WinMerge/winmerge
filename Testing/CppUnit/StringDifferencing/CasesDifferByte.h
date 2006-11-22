/** 
 * @file  CasesDifferByte.h
 *
 * @brief Declaration for Case difference tests.
 */

#ifndef _CASESDIFFERBYTE_H_
#define _CASESDIFFERBYTE_H_

#include <cppunit/extensions/HelperMacros.h>


/**
 * @brief Compares case differences with byte compare method.
 */
class CasesDifferByte : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( CasesDifferByte );
	CPPUNIT_TEST( CasesDifferByte1 );
	CPPUNIT_TEST( CasesDifferByte2 );
	CPPUNIT_TEST( CasesDifferByte3 );
	CPPUNIT_TEST( CasesDifferByte4 );
	CPPUNIT_TEST( CasesDifferByte5 );
	CPPUNIT_TEST( CasesDifferByte6 );
	CPPUNIT_TEST( CasesDifferByte7 );
	CPPUNIT_TEST( CasesDifferByte8 );
	CPPUNIT_TEST( CasesDifferByte9 );
	CPPUNIT_TEST( CasesDifferByte10 );
	CPPUNIT_TEST( CasesDifferByte11 );
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

protected:
	void CasesDifferByte1();
	void CasesDifferByte2();
	void CasesDifferByte3();
	void CasesDifferByte4();
	void CasesDifferByte5();
	void CasesDifferByte6();
	void CasesDifferByte7();
	void CasesDifferByte8();
	void CasesDifferByte9();
	void CasesDifferByte10();
	void CasesDifferByte11();
};

#endif // _CASESDIFFERBYTE_
