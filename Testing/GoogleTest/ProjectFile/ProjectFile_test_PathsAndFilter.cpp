/** 
 * @file  PathsAndFilter.cpp
 *
 * @brief Implementation for PathsAndFilter testcase.
 */

#include "pch.h"
#include <gtest/gtest.h>
#include "UnicodeString.h"
#include "ProjectFile.h"

/** @brief Projectfile to load. */
static const tchar_t FileName[] = _T("..\\TestData\\PathsAndFilter.WinMerge");
/** @brief Left path we should get from file. */
static const tchar_t LeftPath[] = _T("C:\\Temp\\Left");
/** @brief Right path we should get from file. */
static const tchar_t RightPath[] = _T("C:\\Temp\\Right");
/** @brief Filter we should get from file. */
static const tchar_t Filter[] = _T("Filter Name");
/** @brief Prediffer we should get from file. */
static const tchar_t Prediffer[] = _T("IgnoreColumns.dll");
/** @brief Unpacker we should get from file. */
static const tchar_t Unpacker[] = _T("CompareMSExcelFiles.sct");
/** @brief Left desc we should get from file. */
static const tchar_t LeftDesc[] = _T("left description");
/** @brief Right desc we should get from file. */
static const tchar_t RightDesc[] = _T("right description");

namespace
{

class PathAndFilterTest : public testing::Test
{
protected:
	PathAndFilterTest() : m_pProjectFile(nullptr), m_pProjectFileItem(nullptr)
	{
		// You can do set-up work for each test	here.
	}

	virtual ~PathAndFilterTest()
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
TEST_F(PathAndFilterTest, Load)
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
TEST_F(PathAndFilterTest, GetLeftPath)
{
	// Has left path
	bool bIsLeft = m_pProjectFileItem->HasLeft();
	ASSERT_TRUE(bIsLeft == true);
	
	// Get left path without read-only info
	String left = m_pProjectFileItem->GetLeft();
	ASSERT_TRUE(left.compare(LeftPath) == 0);

	// Get left path with read-only info
	bool bReadOnly;
	left = m_pProjectFileItem->GetLeft(&bReadOnly);
	ASSERT_TRUE(left.compare(LeftPath) == 0);
	ASSERT_TRUE(bReadOnly == false);
}

/**
 * @brief Read right path different ways.
 */
TEST_F(PathAndFilterTest, GetRightPath)
{
	// Has right path
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
 * @brief Make sure subfolder inclusion is not get.
 */
TEST_F(PathAndFilterTest, GetSubfolders)
{
	// We don't have a subfolders
	bool bHasSubfolders = m_pProjectFileItem->HasSubfolders();
	ASSERT_TRUE(bHasSubfolders == false);

	// Returns -1 if not set
	int subfolders = m_pProjectFileItem->GetSubfolders();
	ASSERT_TRUE(subfolders == -1);
}

/**
 * @brief Read filter.
 */
TEST_F(PathAndFilterTest, GetFilter)
{
	// Now we have a filter
	bool bHasFilter = m_pProjectFileItem->HasFilter();
	ASSERT_TRUE(bHasFilter == true);

	String filter = m_pProjectFileItem->GetFilter();
	ASSERT_TRUE(filter.compare(Filter) == 0);
}

/**
 * @brief Read prediffer.
 */
TEST_F(PathAndFilterTest, GetPrediffer)
{
	// Now we have a prediffer
	bool bHasPrediffer = m_pProjectFileItem->HasPrediffer();
	ASSERT_TRUE(bHasPrediffer == true);

	String prediffer = m_pProjectFileItem->GetPrediffer();
	ASSERT_TRUE(prediffer.compare(Prediffer) == 0);
}

/**
 * @brief Read unpacker.
 */
TEST_F(PathAndFilterTest, GetUnpacker)
{
	// Now we have a unpacker
	bool bHasUnpacker = m_pProjectFileItem->HasUnpacker();
	ASSERT_TRUE(bHasUnpacker == true);

	String unpacker = m_pProjectFileItem->GetUnpacker();
	ASSERT_TRUE(unpacker.compare(Unpacker) == 0);
}

/**
 * @brief Read left/right description.
 */
TEST_F(PathAndFilterTest, GetDesc)
{
	// Now we have a left description
	bool bHasLeftDesc = m_pProjectFileItem->HasLeftDesc();
	ASSERT_TRUE(bHasLeftDesc == true);

	String leftDesc = m_pProjectFileItem->GetLeftDesc();
	ASSERT_TRUE(leftDesc.compare(LeftDesc) == 0);

	// Now we have a right description
	bool bHasRightDesc = m_pProjectFileItem->HasRightDesc();
	ASSERT_TRUE(bHasRightDesc == true);

	String rightDesc = m_pProjectFileItem->GetRightDesc();
	ASSERT_TRUE(rightDesc.compare(RightDesc) == 0);
}

/**
 * @brief Read table properties
 */
TEST_F(PathAndFilterTest, GetTableProperties)
{
	bool bHasTableDelimiter = m_pProjectFileItem->HasTableDelimiter();
	ASSERT_TRUE(bHasTableDelimiter == true);

	tchar_t tableDelimiter = m_pProjectFileItem->GetTableDelimiter();
	ASSERT_TRUE(tableDelimiter == '\t');

	bool bHasTableQuote = m_pProjectFileItem->HasTableQuote();
	ASSERT_TRUE(bHasTableQuote == true);

	tchar_t tableQuote = m_pProjectFileItem->GetTableQuote();
	ASSERT_TRUE(tableQuote == '\"');

	bool bHasTableAllowNewLinesInQuotes = m_pProjectFileItem->HasTableAllowNewLinesInQuotes();
	ASSERT_TRUE(bHasTableAllowNewLinesInQuotes == true);

	bool tableAllowNewLinesInQuotes = m_pProjectFileItem->GetTableAllowNewLinesInQuotes();
	ASSERT_TRUE(tableAllowNewLinesInQuotes == true);
}

}
