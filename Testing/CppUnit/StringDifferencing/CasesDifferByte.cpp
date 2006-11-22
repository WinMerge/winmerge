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
	CString string1(_T("Test"));
	CString string2(_T("test"));
	int count = 0;

	// Compare case, all whitespaces, whitespace break
	// We must find one difference
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);

	// Ignore case, all whitespaces, whitespace break
	// No difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		false, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Compare case, whitespaces change, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_IGNORE_CHANGE, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);

	// Compare case, whitespaces ignore, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
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
	CString string1(_T("test"));
	CString string2(_T("teSt"));
	int count = 0;

	// Compare case, all whitespaces, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);

	// Ignore case, all whitespaces, whitespace break
	// No difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		false, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Compare case, whitespaces change, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_IGNORE_CHANGE, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);

	// Compare case, whitespaces ignore, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
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
	CString string1(_T("test case"));
	CString string2(_T("teSt case"));
	int count = 0;

	// Compare case, all whitespaces, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);

	// Ignore case, all whitespaces, whitespace break
	// No difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		false, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Compare case, whitespaces change, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_IGNORE_CHANGE, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);

	// Compare case, whitespaces ignore, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
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
	CString string1(_T("test case"));
	CString string2(_T("test cAse"));
	int count = 0;

	// Compare case, all whitespaces, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);

	// Ignore case, all whitespaces, whitespace break
	// No difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		false, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Compare case, whitespaces change, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_IGNORE_CHANGE, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);

	// Compare case, whitespaces ignore, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
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
	CString string1(_T("test case"));
	CString string2(_T("tEst cAse"));
	int count = 0;

	// Compare case, all whitespaces, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);

	// Ignore case, all whitespaces, whitespace break
	// No difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		false, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Compare case, whitespaces change, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_IGNORE_CHANGE, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);

	// Compare case, whitespaces ignore, whitespace break
	// We must find one difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_IGNORE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);
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
	CString string1(_T("test case string"));
	CString string2(_T("tEst case String"));
	int count = 0;

	// Compare case, all whitespaces, whitespace break
	// We must find two differences
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 2);

	// Ignore case, all whitespaces, whitespace break
	// No difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		false, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Compare case, whitespaces change, whitespace break
	// We must find two differences
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_IGNORE_CHANGE, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 2);

	// Compare case, whitespaces ignore, whitespace break
	// We must find two differences
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
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
	CString string1(_T("testcasestring"));
	CString string2(_T("tEstcaseString"));
	int count = 0;

	// Compare case, all whitespaces, whitespace break
	// We must find two differences
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);

	// Ignore case, all whitespaces, whitespace break
	// No difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		false, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Compare case, whitespaces change, whitespace break
	// We must find two differences
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_IGNORE_CHANGE, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);

	// Compare case, whitespaces ignore, whitespace break
	// We must find two differences
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
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
	CString string1(_T("testcasestring second"));
	CString string2(_T("tEstcaseString second"));
	int count = 0;

	// Compare case, all whitespaces, whitespace break
	// We must find two differences
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);

	// Ignore case, all whitespaces, whitespace break
	// No difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		false, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Compare case, whitespaces change, whitespace break
	// We must find two differences
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_IGNORE_CHANGE, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);

	// Compare case, whitespaces ignore, whitespace break
	// We must find two differences
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
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
	CString string1(_T("testcasestring secoNd"));
	CString string2(_T("tEstcaseString second"));
	int count = 0;

	// Compare case, all whitespaces, whitespace break
	// We must find two differences
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);

	// Ignore case, all whitespaces, whitespace break
	// No difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		false, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Compare case, whitespaces change, whitespace break
	// We must find two differences
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_IGNORE_CHANGE, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);

	// Compare case, whitespaces ignore, whitespace break
	// We must find two differences
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_IGNORE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);
}

/**
 * @brief Test we handle simple case difference correctly.
 * This function tests we handle one-chars case difference correctly
 * with different compare options. This time in three words.
 */
void CasesDifferByte::CasesDifferByte10()
{
	wdiffarray diffs;
	CString string1(_T("testcasestring second third"));
	CString string2(_T("tEstcaseString second thIrd"));
	int count = 0;

	// Compare case, all whitespaces, whitespace break
	// We must find two differences
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 2);

	// Ignore case, all whitespaces, whitespace break
	// No difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		false, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Compare case, whitespaces change, whitespace break
	// We must find two differences
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_IGNORE_CHANGE, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 2);

	// Compare case, whitespaces ignore, whitespace break
	// We must find two differences
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
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
	CString string1(_T("testcasestring second third fourth"));
	CString string2(_T("tEstcaseString second thIrd fOurth"));
	int count = 0;

	// Compare case, all whitespaces, whitespace break
	// We must find two differences
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 2);

	// Ignore case, all whitespaces, whitespace break
	// No difference
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		false, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Compare case, whitespaces change, whitespace break
	// We must find two differences
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_IGNORE_CHANGE, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 2);

	// Compare case, whitespaces ignore, whitespace break
	// We must find two differences
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_IGNORE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 2);
}
