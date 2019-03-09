/** 
 * @file  SimpleRightTest.cpp
 *
 * @brief Implementation for SimpleRightTest testcase.
 */

#include "pch.h"
#include <gtest/gtest.h>
#include "UnicodeString.h"
#include "ProjectFile.h"

/** @brief Projectfile to load. */
static const TCHAR FileName[] = _T("..\\TestData\\SimpleRight.WinMerge");
/** @brief Right path we should get from file. */
static const TCHAR RightPath[] = _T("C:\\Temp\\Right");

namespace
{

class SimpleRightTest : public testing::Test
{
protected:
	SimpleRightTest() : m_pProjectFile(nullptr)
	{
		// You can do set-up work for each test	here.
	}

	virtual ~SimpleRightTest()
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
TEST_F(SimpleRightTest, Load)
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
 * @brief Read right path different ways.
 */
TEST_F(SimpleRightTest, GetRightPath)
{
	// Has right path (only)
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
 * @brief Make sure left path is not get.
 */
TEST_F(SimpleRightTest, GetLeftPath)
{
	// We don't have left path
	bool bIsLeft = m_pProjectFile->HasLeft();
	ASSERT_TRUE(bIsLeft == false);

	// Get left path without read-only info
	String left = m_pProjectFile->GetLeft();
	ASSERT_TRUE(left.empty());

	// Get left path with read-only info
	bool bReadOnly;
	left = m_pProjectFile->GetLeft(&bReadOnly);
	ASSERT_TRUE(left.empty());
	ASSERT_TRUE(bReadOnly == false);
}

/**
 * @brief Make sure subfolder inclusion is not get.
 */
TEST_F(SimpleRightTest, GetSubfolders)
{
	// We don't have a subfolders
	bool bHasSubfolders = m_pProjectFile->HasSubfolders();
	ASSERT_TRUE(bHasSubfolders == false);

	// Returns -1 if not set
	int subfolders = m_pProjectFile->GetSubfolders();
	ASSERT_TRUE(subfolders == -1);
}

/**
 * @brief Make sure filter is not get.
 */
TEST_F(SimpleRightTest, GetFilter)
{
	// We don't have a filter
	bool bHasFilter = m_pProjectFile->HasFilter();
	ASSERT_TRUE(bHasFilter == false);

	String filter = m_pProjectFile->GetFilter();
	ASSERT_TRUE(filter.empty());
}

}
