/** 
 * @file  PathsAndFilter.cpp
 *
 * @brief Implementation for PathsAndFilter testcase.
 */

#include <cppunit/config/SourcePrefix.h>
#include "stdafx.h"

#include "ProjectFile.h"
#include "PathsAndFilter.h"

/** @brief Projectfile to load. */
static const TCHAR FileName[] = _T("TestData\\LeftAndRight.WinMerge");
/** @brief Left path we should get from file. */
static const TCHAR LeftPath[] = _T("C:\\Temp\\Left");
/** @brief Right path we should get from file. */
static const TCHAR RightPath[] = _T("C:\\Temp\\Right");
/** @brief Filter we should get from file. */
static const TCHAR Filter[] = _T("Filter Name");

CPPUNIT_TEST_SUITE_REGISTRATION( PathsAndFilter );

/** @brief Testcase initialization code. */
void PathsAndFilter::setUp()
{
	// Add possible initializations here
	m_pProjectFile = new ProjectFile;
}

/** @brief Testcase cleanup code. */
void PathsAndFilter::tearDown()
{
	// Add possible cleanups here
	delete m_pProjectFile;
}

/**
 * @brief Load the projectfile.
 */
void PathsAndFilter::Load()
{
	CString sError;
	
	BOOL success = m_pProjectFile->Read(FileName, &sError);

	// Must succeed
	CPPUNIT_ASSERT(success == TRUE);
	CPPUNIT_ASSERT(sError.IsEmpty());
}

/**
 * @brief Read left path different ways.
 */
void PathsAndFilter::GetLeftPath()
{
	// Has left path
	BOOL bIsLeft = m_pProjectFile->HasLeft();
	CPPUNIT_ASSERT(bIsLeft == TRUE);
	
	// Get left path without read-only info
	CString left = m_pProjectFile->GetLeft();
	CPPUNIT_ASSERT(left.Compare(LeftPath) == 0);

	// Get left path with read-only info
	BOOL bReadOnly;
	left = m_pProjectFile->GetLeft(&bReadOnly);
	CPPUNIT_ASSERT(left.Compare(LeftPath) == 0);
	CPPUNIT_ASSERT(bReadOnly == FALSE);
}

/**
 * @brief Read right path different ways.
 */
void PathsAndFilter::GetRightPath()
{
	// Has right path
	BOOL bIsRight = m_pProjectFile->HasRight();
	CPPUNIT_ASSERT(bIsRight == TRUE);
	
	// Get right path without read-only info
	CString right = m_pProjectFile->GetRight();
	CPPUNIT_ASSERT(right.Compare(RightPath) == 0);

	// Get right path with read-only info
	BOOL bReadOnly;
	right = m_pProjectFile->GetRight(&bReadOnly);
	CPPUNIT_ASSERT(right.Compare(RightPath) == 0);
	CPPUNIT_ASSERT(bReadOnly == FALSE);
}

/**
 * @brief Make sure subfolder inclusion is not get.
 */
void PathsAndFilter::GetSubfolders()
{
	// We don't have a subfolders
	BOOL bHasSubfolders = m_pProjectFile->HasSubfolders();
	CPPUNIT_ASSERT(bHasSubfolders == FALSE);

	int subfolders = m_pProjectFile->GetSubfolders();
	CPPUNIT_ASSERT(subfolders == 0);
}

/**
 * @brief Read filter.
 */
void PathsAndFilter::GetFilter()
{
	// Now we have a filter
	BOOL bHasFilter = m_pProjectFile->HasFilter();
	CPPUNIT_ASSERT(bHasFilter == TRUE);

	CString filter = m_pProjectFile->GetFilter();
	CPPUNIT_ASSERT(filter.Compare(Filter) == 0);
}
