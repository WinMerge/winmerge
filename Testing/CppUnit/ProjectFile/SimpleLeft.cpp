/** 
 * @file  SimpleLeft.cpp
 *
 * @brief Implementation for SimpleLeft testcase.
 */

#include <cppunit/config/SourcePrefix.h>
#include "stdafx.h"

#include "ProjectFile.h"
#include "SimpleLeft.h"

/** @brief Projectfile to load. */
static const TCHAR FileName[] = _T("TestData\\SimpleLeft.WinMerge");
/** @brief Left path we should get from file. */
static const TCHAR LeftPath[] = _T("C:\\Temp\\Left");

CPPUNIT_TEST_SUITE_REGISTRATION( SimpleLeft );

/** @brief Testcase initialization code. */
void SimpleLeft::setUp()
{
	// Add possible initializations here
	m_pProjectFile = new ProjectFile;

	CString sError;
	BOOL success = m_pProjectFile->Read(FileName, &sError);
}

/** @brief Testcase cleanup code. */
void SimpleLeft::tearDown()
{
	// Add possible cleanups here
	delete m_pProjectFile;
}

/**
 * @brief Load the projectfile.
 */
void SimpleLeft::Load()
{
	CString sError;
	
	// setUp already created the project file for us, but this
	// test is for testing creation and loading..
	if (m_pProjectFile)
		delete m_pProjectFile;
	m_pProjectFile = new ProjectFile;
	CPPUNIT_ASSERT(m_pProjectFile);

	BOOL success = m_pProjectFile->Read(FileName, &sError);

	// Must succeed
	CPPUNIT_ASSERT(success == TRUE);
	CPPUNIT_ASSERT(sError.IsEmpty());
}

/**
 * @brief Read left path different ways.
 */
void SimpleLeft::GetLeftPath()
{
	// Has left path (only)
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
 * @brief Make sure right path is not get.
 */
void SimpleLeft::GetRightPath()
{
	// We don't have right path
	BOOL bIsRight = m_pProjectFile->HasRight();
	CPPUNIT_ASSERT(bIsRight == FALSE);

	// Get right path without read-only info
	CString right = m_pProjectFile->GetRight();
	CPPUNIT_ASSERT(right.IsEmpty());

	// Get right path with read-only info
	BOOL bReadOnly;
	right = m_pProjectFile->GetRight(&bReadOnly);
	CPPUNIT_ASSERT(right.IsEmpty());
	CPPUNIT_ASSERT(bReadOnly == FALSE);
}

/**
 * @brief Make sure filter is not get.
 */
void SimpleLeft::GetSubfolders()
{
	// We don't have a subfolders
	BOOL bHasSubfolders = m_pProjectFile->HasSubfolders();
	CPPUNIT_ASSERT(bHasSubfolders == FALSE);

	// Returns -1 if not set
	int subfolders = m_pProjectFile->GetSubfolders();
	CPPUNIT_ASSERT(subfolders == -1);
}

/**
 * @brief Make sure subfolder inclusion is not get.
 */
void SimpleLeft::GetFilter()
{
	// We don't have a filter
	BOOL bHasFilter = m_pProjectFile->HasFilter();
	CPPUNIT_ASSERT(bHasFilter == FALSE);

	CString filter = m_pProjectFile->GetFilter();
	CPPUNIT_ASSERT(filter.IsEmpty());
}
