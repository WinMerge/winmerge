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
static const tchar_t FileName[] = _T("..\\TestData\\SimpleRight.WinMerge");
/** @brief Right path we should get from file. */
static const tchar_t RightPath[] = _T("C:\\Temp\\Right");

namespace
{

class SimpleRightTest : public testing::Test
{
protected:
	SimpleRightTest() : m_pProjectFile(nullptr),m_pProjectFileItem(nullptr)
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
		m_pProjectFileItem = &*m_pProjectFile->Items().begin();
		
	}

	virtual void TearDown()
	{
		// Code	here will be called	immediately	after each test	(right
		// before the destructor).
		delete m_pProjectFile;
	}

	// Objects declared here can be used by all tests in the test case for Foo.
	ProjectFile *m_pProjectFile;
	ProjectFileItem *m_pProjectFileItem;
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
	bool bIsRight = m_pProjectFileItem->HasRight();
	ASSERT_TRUE(bIsRight == true);
	
	// Get right path without read-only info
	String right = m_pProjectFileItem->GetRight();
	ASSERT_TRUE(right.compare(RightPath) == 0);

	// Get right path with read-only info
	bool bReadOnly;
	right = m_pProjectFileItem->GetRight(&bReadOnly);
	ASSERT_TRUE(right.compare(RightPath) == 0);
	ASSERT_TRUE(bReadOnly == false);
}

/**
 * @brief Make sure left path is not get.
 */
TEST_F(SimpleRightTest, GetLeftPath)
{
	// We don't have left path
	bool bIsLeft = m_pProjectFileItem->HasLeft();
	ASSERT_TRUE(bIsLeft == false);

	// Get left path without read-only info
	String left = m_pProjectFileItem->GetLeft();
	ASSERT_TRUE(left.empty());

	// Get left path with read-only info
	bool bReadOnly;
	left = m_pProjectFileItem->GetLeft(&bReadOnly);
	ASSERT_TRUE(left.empty());
	ASSERT_TRUE(bReadOnly == false);
}

/**
 * @brief Make sure subfolder inclusion is not get.
 */
TEST_F(SimpleRightTest, GetSubfolders)
{
	// We don't have a subfolders
	bool bHasSubfolders = m_pProjectFileItem->HasSubfolders();
	ASSERT_TRUE(bHasSubfolders == false);

	// Returns -1 if not set
	int subfolders = m_pProjectFileItem->GetSubfolders();
	ASSERT_TRUE(subfolders == -1);
}

/**
 * @brief Make sure filter is not get.
 */
TEST_F(SimpleRightTest, GetFilter)
{
	// We don't have a filter
	bool bHasFilter = m_pProjectFileItem->HasFilter();
	ASSERT_TRUE(bHasFilter == false);

	String filter = m_pProjectFileItem->GetFilter();
	ASSERT_TRUE(filter.empty());
}

/**
 * @brief Make sure prediffer is not get.
 */
TEST_F(SimpleRightTest, GetPrediffer)
{
	// We don't have a prediffer
	bool bHasPrediffer = m_pProjectFileItem->HasPrediffer();
	ASSERT_TRUE(bHasPrediffer == false);

	String prediffer = m_pProjectFileItem->GetPrediffer();
	ASSERT_TRUE(prediffer.empty());
}

/**
 * @brief Make sure unpacker is not get.
 */
TEST_F(SimpleRightTest, GetUnpacker)
{
	// We don't have a unpacker
	bool bHasUnpacker = m_pProjectFileItem->HasUnpacker();
	ASSERT_TRUE(bHasUnpacker == false);

	String unpacker = m_pProjectFileItem->GetUnpacker();
	ASSERT_TRUE(unpacker.empty());
}

}
