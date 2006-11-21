/** 
 * @file  IdenticalStrings1.h
 *
 * @brief Implementation for Identical strings testing.
 */

#include <cppunit/config/SourcePrefix.h>
#include "stdafx.h"

#include "CompareOptions.h"
#include "stringdiffs.h"

#include "IdenticalStrings1.h"

CPPUNIT_TEST_SUITE_REGISTRATION( IdenticalStrings1 );

static const CString string1(_T("Test"));
static const CString string2(_T("Test"));

/** @brief Testcase initialization code. */
void IdenticalStrings1::setUp()
{
	// Add possible initializations here
}

/** @brief Testcase cleanup code. */
void IdenticalStrings1::tearDown()
{
	// Add possible cleanups here
}

/**
 * @brief Test identical words are detected as such.
 * This function tests that two identical words are detected
 * as identical with different word-compare settings. This function
 * tests whitespace-break, word-level compare.
 */
void IdenticalStrings1::IdenticalWord1()
{
	wdiffarray diffs;
	int count = 0;

	// Break type is whitespace or punctuation

	// Compare case, all whitespaces, whitespace break
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_COMPARE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Ignore case, all whitespaces, whitespace break
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		false, WHITESPACE_COMPARE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Compare case, whitespaces change, whitespace break
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_IGNORE_CHANGE, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Compare case, whitespaces ignore, whitespace break
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_IGNORE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);
}

/**
 * @brief Test identical words are detected as such.
 * This function tests that two identical words are detected
 * as identical with different word-compare settings. This function
 * tests punctuation-break, word-level compare.
 */
void IdenticalStrings1::IdenticalWord2()
{
	wdiffarray diffs;
	int count = 0;

	// Break type is whitespace or punctuation

	// Compare case, all whitespaces, whitespace break
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_COMPARE_ALL, 1, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Ignore case, all whitespaces, whitespace break
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		false, WHITESPACE_COMPARE_ALL, 1, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Compare case, whitespaces change, whitespace break
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_IGNORE_CHANGE, 1, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Compare case, whitespaces ignore, whitespace break
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_IGNORE_ALL, 1, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);
}

/**
 * @brief Test identical words are detected as such.
 * This function tests that two identical words are detected
 * as identical with different word-compare settings. This function
 * tests whitespace-break, byte-level compare.
 */
void IdenticalStrings1::IdenticalWord3()
{
	wdiffarray diffs;
	int count = 0;

	// Break type is whitespace or punctuation

	// Compare case, all whitespaces, whitespace break
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Ignore case, all whitespaces, whitespace break
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		false, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Compare case, whitespaces change, whitespace break
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_IGNORE_CHANGE, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Compare case, whitespaces ignore, whitespace break
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_IGNORE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);
}

/**
 * @brief Test identical words are detected as such.
 * This function tests that two identical words are detected
 * as identical with different word-compare settings. This function
 * tests punctuation-break, byte-level compare.
 */
void IdenticalStrings1::IdenticalWord4()
{
	wdiffarray diffs;
	int count = 0;

	// Break type is whitespace or punctuation

	// Compare case, all whitespaces, whitespace break
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_COMPARE_ALL, 1, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Ignore case, all whitespaces, whitespace break
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		false, WHITESPACE_COMPARE_ALL, 1, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Compare case, whitespaces change, whitespace break
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_IGNORE_CHANGE, 1, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	// Compare case, whitespaces ignore, whitespace break
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_IGNORE_ALL, 1, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);
}
