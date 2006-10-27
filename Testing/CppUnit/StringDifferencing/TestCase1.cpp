
#include <cppunit/config/SourcePrefix.h>
#include "stdafx.h"

#include "CompareOptions.h"
#include "stringdiffs.h"

#include "TestCase1.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TestCase1 );

void TestCase1::setUp()
{
	// Add possible initializations here
}

void TestCase1::tearDown()
{
	// Add possible cleanups here
}

void TestCase1::Identical1()
{
	wdiffarray diffs;
	CString string1(_T("Test"));
	CString string2(_T("Test"));
	int count = 0;

	// Break type is whitespace or punctuation

	// Check strings with different settings
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_COMPARE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	sd_ComputeWordDiffs(string1, string2,
		false, WHITESPACE_COMPARE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_IGNORE_CHANGE, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_IGNORE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_COMPARE_ALL, 1, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);
}

void TestCase1::Identical2()
{
	wdiffarray diffs;
	CString string1(_T("Test words"));
	CString string2(_T("Test words"));
	int count = 0;

	// Break type is whitespace or punctuation

	// Check strings with different settings
	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_COMPARE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	sd_ComputeWordDiffs(string1, string2,
		false, WHITESPACE_COMPARE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_IGNORE_CHANGE, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_IGNORE_ALL, 0, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_COMPARE_ALL, 1, false,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);

	sd_ComputeWordDiffs(string1, string2,
		true, WHITESPACE_COMPARE_ALL, 0, true,
		&diffs);
	count = diffs.GetSize();
	CPPUNIT_ASSERT(count == 0);
}

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
