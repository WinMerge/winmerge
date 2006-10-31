/** 
 * @file  TestCase1.h
 *
 * @brief Implementatino for first simple test case.
 */

#include <cppunit/config/SourcePrefix.h>
#include "stdafx.h"

#include "CompareOptions.h"
#include "stringdiffs.h"

#include "TestCase1.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TestCase1 );

/** @brief Testcase initialization code. */
void TestCase1::setUp()
{
	// Add possible initializations here
}

/** @brief Testcase cleanup code. */
void TestCase1::tearDown()
{
	// Add possible cleanups here
}

/**
 * @brief Test identical words are detected as such.
 * This function tests that two identical words are detected
 * as identical with different word-compare settings.
 */
void TestCase1::Identical1()
{
	wdiffarray diffs;
	CString string1(_T("Test"));
	CString string2(_T("Test"));
	int count = 0;

	// Compare case, all whitespaces, whitespace break
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_COMPARE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Ignore case, all whitespaces, whitespace break
	sd_ComputeWordDiffs(string1, string2,
		false, WHITESPACE_COMPARE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Compare case, whitespaces change, whitespace break
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_IGNORE_CHANGE, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Compare case, whitespaces ignore, whitespace break
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_IGNORE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);
}

/**
 * @brief Test different words are detected as such.
 * This function tests that two different words are detected
 * as different with different word-compare settings.
 */
void TestCase1::Difference1()
{
	wdiffarray diffs;
	CString string1(_T("Test"));
	CString string2(_T("test"));
	int count = 0;

	// Break type is whitespace or punctuation

	// Check strings with different settings
	sd_ComputeWordDiffs(string1, string2,
		false, WHITESPACE_COMPARE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);
	diffs.RemoveAll();

	// Check strings with different settings
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_COMPARE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);
	diffs.RemoveAll();

	string1 = _T("tesT");
	string2 = _T("test");
	// Check strings with different settings
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_COMPARE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT_MESSAGE(_T("testT & test difference not found!"), count == 1);
	diffs.RemoveAll();
}

/**
 * @brief Test different two-words are detected as such.
 * This function tests that two different two-words are detected
 * as different with different word-compare settings.
 */
void TestCase1::Difference2()
{
	wdiffarray diffs;
	CString string1(_T("Test string"));
	CString string2(_T("test strIng"));
	int count = 0;

	// Break type is whitespace or punctuation

	// Check strings with different settings
	sd_ComputeWordDiffs(string1, string2,
		false, WHITESPACE_COMPARE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);
	diffs.RemoveAll();

	// This fails? Why?
	// Shouldn't it it find two differences? But it founds only one?
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_COMPARE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 2);
	diffs.RemoveAll();
}
