/** 
 * @file  LeftAndRightRecursive.cpp
 *
 * @brief Implementation for LeftAndRightRecursive testcase.
 */

#include "pch.h"
#include <gtest/gtest.h>
#include "UnicodeString.h"
#include "ProjectFile.h"

/** @brief Projectfile to load. */
static const TCHAR FileName[] = _T("..\\TestData\\LeftAndRightRecursive.WinMerge");
/** @brief Left path we should get from file. */
static const TCHAR LeftPath[] = _T("C:\\Temp\\Left");
/** @brief Right path we should get from file. */
static const TCHAR RightPath[] = _T("C:\\Temp\\Right");

namespace
{

class LeftAndRightRecursiveTest : public testing::Test
{
protected:
	LeftAndRightRecursiveTest() : m_pProjectFile(nullptr)
	{
		// You can do set-up work for each test	here.
	}

	virtual ~LeftAndRightRecursiveTest()
	{
		// You can do clean-up work	that doesn't throw exceptions here.
	}

	// If	the	constructor	and	destructor are not enough for setting up
	// and cleaning up each test, you can define the following methods:

	virtual void SetUp()
	{
		// Code	here will be called	immediately	after the constructor (right
		// before each test).

		m_pProjectFile = new ProjectFile;

		bool success = m_pProjectFile->Read(FileName);
	}

	virtual void TearDown()
	{
		// Code	here will be called	immediately	after each test	(right
		// before the destructor).
		delete m_pProjectFile;
	}

	// Objects declared here can be used by all tests in the test case for Foo.
	ProjectFile *m_pProjectFile;
};

/**
 * @brief Load the projectfile.
 */
TEST_F(LeftAndRightRecursiveTest, Load)
{
	// setUp already created the project file for us, but this
	// test is for testing creation and loading..
	if (m_pProjectFile)
		delete m_pProjectFile;
	m_pProjectFile = new ProjectFile;
	ASSERT_TRUE(m_pProjectFile != NULL);

	bool success = m_pProjectFile->Read(FileName);

	// Must succeed
	ASSERT_TRUE(success == true);
}

/**
 * @brief Read left path different ways.
 */
TEST_F(LeftAndRightRecursiveTest, GetLeftPath)
{
	// Has left path
	bool bIsLeft = m_pProjectFile->HasLeft();
	ASSERT_TRUE(bIsLeft == true);
	
	// Get left path without read-only info
	String left = m_pProjectFile->GetLeft();
	ASSERT_TRUE(left.compare(LeftPath) == 0);

	// Get left path with read-only info
	bool bReadOnly;
	left = m_pProjectFile->GetLeft(&bReadOnly);
	ASSERT_TRUE(left.compare(LeftPath) == 0);
	ASSERT_TRUE(bReadOnly == false);
}

/**
 * @brief Read right path different ways.
 */
TEST_F(LeftAndRightRecursiveTest, GetRightPath)
{
	// Has right path
	bool bIsRight = m_pProjectFile->HasRight();
	ASSERT_TRUE(bIsRight == true);
	
	// Get right path without read-only info
	String right = m_pProjectFile->GetRight();
	ASSERT_TRUE(right.compare(RightPath) == 0);

	// Get right path with read-only info
	bool bReadOnly;
	right = m_pProjectFile->GetRight(&bReadOnly);
	ASSERT_TRUE(right.compare(RightPath) == 0);
	ASSERT_TRUE(bReadOnly == false);
}

/**
 * @brief Make sure subfolder inclusion is get and enabled.
 */
TEST_F(LeftAndRightRecursiveTest, GetSubfolders)
{
	// Now we have subfolders
	bool bHasSubfolders = m_pProjectFile->HasSubfolders();
	ASSERT_TRUE(bHasSubfolders == true);

	int subfolders = m_pProjectFile->GetSubfolders();
	ASSERT_TRUE(subfolders == 1);
}

/**
 * @brief Make sure filter is not get.
 */
TEST_F(LeftAndRightRecursiveTest, GetFilter)
{
	// We don't have a filter
	bool bHasFilter = m_pProjectFile->HasFilter();
	ASSERT_TRUE(bHasFilter == false);

	String filter = m_pProjectFile->GetFilter();
	ASSERT_TRUE(filter.empty());
}

}
