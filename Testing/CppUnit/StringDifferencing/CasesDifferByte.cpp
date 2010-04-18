/** 
 * @file  CasesDifferByte.cpp
 *
 * @brief Implementation for Case difference tests..
 */

#include <cppunit/config/SourcePrefix.h>
#include "stdafx.h"

#include "CompareOptions.h"
#include "stringdiffs.h"

#include "CasesDifferByte.h"

CPPUNIT_TEST_SUITE_REGISTRATION( CasesDifferByte );

/** @brief Testcase initialization code. */
void CasesDifferByte::setUp()
{
	// Add possible initializations here
}

/** @brief Testcase cleanup code. */
void CasesDifferByte::tearDown()
{
	// Add possible cleanups here
}

/**
 * @brief Test we handle simple case difference correctly.
 * This function tests we handle one-char case difference correctly
 * with different compare options. In this test the difference is
 * the first char in word.
 */
void CasesDifferByte::CasesDifferByte1()
{
	wdiffarray diffs;
	CString string[2];
	int count = 0;

	string[0] = _T("Test");
	string[1] = _T("test");

	// Compare case, all whitespaces, whitespace break
	// We must find one difference
	sd_ComputeWordDiffs(2, string,
		true, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);

	// Ignore case, all whitespaces, whitespace break
	// No difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		false, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Compare case, whitespaces change, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		true, WHITESPACE_IGNORE_CHANGE, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);

	// Compare case, whitespaces ignore, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		true, WHITESPACE_IGNORE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);
}

/**
 * @brief Test we handle simple case difference correctly.
 * This function tests we handle one-char case difference correctly
 * with different compare options. In this test the difference is
 * in the middle of word.
 */
void CasesDifferByte::CasesDifferByte2()
{
	wdiffarray diffs;
	CString string[2];
	string[0] = _T("test");
	string[1] = _T("teSt");
	int count = 0;

	// Compare case, all whitespaces, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		true, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);

	// Ignore case, all whitespaces, whitespace break
	// No difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		false, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Compare case, whitespaces change, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		true, WHITESPACE_IGNORE_CHANGE, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);

	// Compare case, whitespaces ignore, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		true, WHITESPACE_IGNORE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);
}

/**
 * @brief Test we handle simple case difference correctly.
 * This function tests we handle one-chars case difference correctly
 * with different compare options.
 */
void CasesDifferByte::CasesDifferByte3()
{
	wdiffarray diffs;
	CString string[2];
	int count = 0;

	string[0] = _T("Test");
	string[1] = _T("test");

	// Compare case, all whitespaces, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		true, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);

	// Ignore case, all whitespaces, whitespace break
	// No difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		false, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Compare case, whitespaces change, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		true, WHITESPACE_IGNORE_CHANGE, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);

	// Compare case, whitespaces ignore, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		true, WHITESPACE_IGNORE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);
}

/**
 * @brief Test we handle simple case difference correctly.
 * This function tests we handle one-chars case difference correctly
 * with different compare options.
 */
void CasesDifferByte::CasesDifferByte4()
{
	wdiffarray diffs;
	CString string[2];
	string[0] = _T("test case");
	string[1] = _T("test cAse");
	int count = 0;

	// Compare case, all whitespaces, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		true, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);

	// Ignore case, all whitespaces, whitespace break
	// No difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		false, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Compare case, whitespaces change, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		true, WHITESPACE_IGNORE_CHANGE, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);

	// Compare case, whitespaces ignore, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		true, WHITESPACE_IGNORE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);
}

/**
 * @brief Test we handle simple case difference correctly.
 * This function tests we handle one-chars case difference correctly
 * with different compare options. This time with two words.
 */
void CasesDifferByte::CasesDifferByte5()
{
	wdiffarray diffs;
	CString string[2];
	string[0] = _T("test case");
	string[1] = _T("tEst cAse");
	int count = 0;

	// Compare case, all whitespaces, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		true, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 2);

	// Ignore case, all whitespaces, whitespace break
	// No difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		false, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Compare case, whitespaces change, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		true, WHITESPACE_IGNORE_CHANGE, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 2);

	// Compare case, whitespaces ignore, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		true, WHITESPACE_IGNORE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 2);
}

/**
 * @brief Test we handle simple case difference correctly.
 * This function tests we handle one-chars case difference correctly
 * with different compare options. This time with three words,
 * with one identical word between two different words.
 */
void CasesDifferByte::CasesDifferByte6()
{
	wdiffarray diffs;
	CString string[2];
	string[0] = _T("test case string");
	string[1] = _T("tEst case String");
	int count = 0;

	// Compare case, all whitespaces, whitespace break
	// We must find two differences
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		true, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 2);

	// Ignore case, all whitespaces, whitespace break
	// No difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		false, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Compare case, whitespaces change, whitespace break
	// We must find two differences
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		true, WHITESPACE_IGNORE_CHANGE, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 2);

	// Compare case, whitespaces ignore, whitespace break
	// We must find two differences
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		true, WHITESPACE_IGNORE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 2);
}

/**
 * @brief Test we handle simple case difference correctly.
 * This function tests we handle one-chars case difference correctly
 * with different compare options. This time in one word.
 */
void CasesDifferByte::CasesDifferByte7()
{
	wdiffarray diffs;
	CString string[2];
	string[0] = _T("testcasestring");
	string[1] = _T("tEstcaseString");
	int count = 0;

	// Compare case, all whitespaces, whitespace break
	// We must find two differences
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		true, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);

	// Ignore case, all whitespaces, whitespace break
	// No difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		false, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Compare case, whitespaces change, whitespace break
	// We must find two differences
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		true, WHITESPACE_IGNORE_CHANGE, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);

	// Compare case, whitespaces ignore, whitespace break
	// We must find two differences
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		true, WHITESPACE_IGNORE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);
}

/**
 * @brief Test we handle simple case difference correctly.
 * This function tests we handle one-chars case difference correctly
 * with different compare options. This time in two words.
 */
void CasesDifferByte::CasesDifferByte8()
{
	wdiffarray diffs;
	CString string[2];
	string[0] = _T("testcasestring second");
	string[1] = _T("tEstcaseString second");
	int count = 0;

	// Compare case, all whitespaces, whitespace break
	// We must find two differences
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		true, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);

	// Ignore case, all whitespaces, whitespace break
	// No difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		false, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Compare case, whitespaces change, whitespace break
	// We must find two differences
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		true, WHITESPACE_IGNORE_CHANGE, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);

	// Compare case, whitespaces ignore, whitespace break
	// We must find two differences
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		true, WHITESPACE_IGNORE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);
}

/**
 * @brief Test we handle simple case difference correctly.
 * This function tests we handle one-chars case difference correctly
 * with different compare options. This time in two words.
 */
void CasesDifferByte::CasesDifferByte9()
{
	wdiffarray diffs;
	CString string[2];
	string[0] = _T("testcasestring secoNd");
	string[1] = _T("tEstcaseString second");
	int count = 0;

	// Compare case, all whitespaces, whitespace break
	// We must find two differences
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		true, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 2);

	// Ignore case, all whitespaces, whitespace break
	// No difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		false, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Compare case, whitespaces change, whitespace break
	// We must find two differences
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		true, WHITESPACE_IGNORE_CHANGE, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 2);

	// Compare case, whitespaces ignore, whitespace break
	// We must find two differences
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		true, WHITESPACE_IGNORE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 2);
}

/**
 * @brief Test we handle simple case difference correctly.
 * This function tests we handle one-chars case difference correctly
 * with different compare options. This time in three words.
 */
void CasesDifferByte::CasesDifferByte10()
{
	wdiffarray diffs;
	CString string[2];
	string[0] = _T("testcasestring second third");
	string[1] = _T("tEstcaseString second thIrd");
	int count = 0;

	// Compare case, all whitespaces, whitespace break
	// We must find two differences
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		true, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 2);

	// Ignore case, all whitespaces, whitespace break
	// No difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		false, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Compare case, whitespaces change, whitespace break
	// We must find two differences
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		true, WHITESPACE_IGNORE_CHANGE, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 2);

	// Compare case, whitespaces ignore, whitespace break
	// We must find two differences
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		true, WHITESPACE_IGNORE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 2);
}

/**
 * @brief Test we handle simple case difference correctly.
 * This function tests we handle one-chars case difference correctly
 * with different compare options. This time in four words.
 */
void CasesDifferByte::CasesDifferByte11()
{
	wdiffarray diffs;
	CString string[2];
	string[0] = _T("testcasestring second third fourth");
	string[1] = _T("tEstcaseString second thIrd fOurth");
	int count = 0;

	// Compare case, all whitespaces, whitespace break
	// We must find two differences
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		true, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 3);

	// Ignore case, all whitespaces, whitespace break
	// No difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		false, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Compare case, whitespaces change, whitespace break
	// We must find two differences
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		true, WHITESPACE_IGNORE_CHANGE, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 3);

	// Compare case, whitespaces ignore, whitespace break
	// We must find two differences
	diffs.RemoveAll();
	sd_ComputeWordDiffs(2, string,
		true, WHITESPACE_IGNORE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 3);
}
