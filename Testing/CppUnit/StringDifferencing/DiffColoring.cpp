/** 
* @file  DiffColoring.cpp
*
* @brief Implementation of DiffColoring test cases.
*/

#include <cppunit/config/SourcePrefix.h>
#include "StdAfx.h"

#include "CompareOptions.h"
#include "stringdiffs.h"

#include "DiffColoring.h"

CPPUNIT_TEST_SUITE_REGISTRATION( DiffColoring );

bool operator==(const wdiff& rdf, const wdiff& ldf)
{
	return ((rdf.start[0] == ldf.start[0]) &&
		    (rdf.end[0] == ldf.end[0]) &&
			(rdf.start[1] == ldf.start[1]) &&
			(rdf.start[1] == ldf.start[1]));
}

/** @brief Testcase initialization code. */
void DiffColoring::setUp()
{
	// Add possible initializations here
}

/** @brief Testcase cleanup code. */
void DiffColoring::tearDown()
{
	// Add possible cleanups here
}

/**
* @brief Test a single word difference.
*/
void DiffColoring::OneWord()
{
	wdiffarray diffs;
	CString string1(_T("Left"));
	CString string2(_T("Right"));
	int count = 0;

	// Compare case, all white spaces, whitespace break
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_COMPARE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);
	CPPUNIT_ASSERT(diffs[0] == wdiff(0, 3, 0, 3));
}

/**
* @brief Test a multi words difference.
*/
void DiffColoring::OneDiff()
{
	wdiffarray diffs;
	CString string1(_T("This is the first line"));
	CString string2(_T("This is not a cow"));
	int count = 0;

	// Compare case, all white spaces, whitespace break
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_COMPARE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);
	CPPUNIT_ASSERT(diffs[0] == wdiff(8, 21, 8, 16));
}

/**
* @brief Test a different string length difference.
*/
void DiffColoring::DifferentLength()
{
	wdiffarray diffs;
	CString string1(_T("First line to check"));
	CString string2(_T("Second line"));
	int count = 0;

	// Compare case, all white spaces, whitespace break
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_COMPARE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 2);
	CPPUNIT_ASSERT(diffs[0] == wdiff(0, 4, 0, 5));
	CPPUNIT_ASSERT(diffs[1] == wdiff(11, 18, 11, 10));
}

/**
* @brief Test a string against an empty string difference.
*/
void DiffColoring::DiffWithEmpty()
{
	wdiffarray diffs;
	CString string1(_T("First line to check"));
	CString string2(_T(""));
	int count = 0;

	// Compare case, all white spaces, whitespace break
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_COMPARE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);
	CPPUNIT_ASSERT(diffs[0] == wdiff(0, 18, 0, -1));
}

/**
* @brief Bug #1491334: Thinly highlighted position is wrong.
*/
void DiffColoring::Bug1491334()
{
	wdiffarray diffs;
	CString string1(_T("00 52 C8 52"));
	CString string2(_T("00 00 00 52"));
	int count = 0;

	// Compare case, all white spaces, whitespace break
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_COMPARE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);
	CPPUNIT_ASSERT(diffs[0] == wdiff(3, 7, 3, 7));
}

/**
* @brief Bug #1639453: Imaginary difference highlighted in character level mode.
*/
void DiffColoring::Bug1639453()
{
	wdiffarray diffs;
	CString string1(_T("[12, 34]"));
	CString string2(_T("[12, 34, 56]"));
	int count = 0;

	// Compare case, all white spaces, whitespace break + punctuation, char level
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_COMPARE_ALL, 1, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);
	CPPUNIT_ASSERT(diffs[0] == wdiff(7, 7, 7, 11));
}

/**
* @brief Bug #1683061: Bug in highlighting.
*/
void DiffColoring::Bug1683061()
{
	wdiffarray diffs;
	CString string1(_T("ABC"));
	CString string2(_T("ABCD"));
	int count = 0;

	// Compare case, all white spaces, whitespace break
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_COMPARE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);
	CPPUNIT_ASSERT(diffs[0] == wdiff(0, 2, 0, 3));

	// Compare case, all white spaces, whitespace break + punctuation
	diffs.RemoveAll();
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 1);
	CPPUNIT_ASSERT(diffs[0] == wdiff(3, 2, 3, 3));
}

