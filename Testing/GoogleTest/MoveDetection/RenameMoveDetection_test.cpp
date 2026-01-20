#include "pch.h"
#include <gtest/gtest.h>
#include "RenameMoveDetection.h"
#include "DiffContext.h"
#include "DiffItem.h"
#include "FilterEngine/FilterExpression.h"

namespace
{
	/**
	 * @brief Test fixture for RenameMoveDetection
	 */
	class RenameMoveDetectionTest : public testing::Test
	{
	protected:
		RenameMoveDetectionTest() {}
		virtual ~RenameMoveDetectionTest() {}

		/**
		 * @brief Create a mock DIFFITEM for testing
		 */
		void CreateMockDiffItem(DIFFITEM& di, const String& filename, bool existsLeft, bool existsRight, bool isDirectory = false)
		{
			di.renameMoveGroupId = -1;
			
			if (existsLeft)
			{
				di.diffcode.setSideFlag(0);
				di.diffFileInfo[0].filename = filename;
				di.diffFileInfo[0].path = _T("");
				di.diffFileInfo[0].size = 1000;
			}
			
			if (existsRight)
			{
				di.diffcode.setSideFlag(1);
				di.diffFileInfo[1].filename = filename;
				di.diffFileInfo[1].path = _T("");
				di.diffFileInfo[1].size = 1000;
			}
			
			if (isDirectory)
				di.diffcode.diffcode |= DIFFCODE::DIR;
			else
				di.diffcode.diffcode |= DIFFCODE::FILE;
		}
	};

	/**
	 * @brief Test that Detect() does nothing when no expression is set
	 */
	TEST_F(RenameMoveDetectionTest, NoExpressionSet)
	{
		PathContext paths;
		paths.SetLeft(_T("C:\\Left"));
		paths.SetRight(_T("C:\\Right"));
		
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();
		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(nullptr);
		
		// Should not crash and should return immediately
		EXPECT_NO_THROW(ctxt.m_pRenameMoveDetection->Detect(ctxt, true));
	}

	/**
	 * @brief Test basic move detection with simple expression
	 */
	TEST_F(RenameMoveDetectionTest, BasicMoveDetection)
	{
		PathContext paths;
		paths.SetLeft(_T("C:\\Left"));
		paths.SetRight(_T("C:\\Right"));
		
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();
		
		// Create a simple expression that matches files with same name
		auto pRenameMoveKeyExpression = std::make_unique<FilterExpression>();
		pRenameMoveKeyExpression->Parse("Name");
		pRenameMoveKeyExpression->SetDiffContext(&ctxt);
		
		// Add mock items
		DIFFITEM* pdi1 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem(*pdi1, _T("file1.txt"), true, false);
		
		DIFFITEM* pdi2 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem(*pdi2, _T("file1.txt"), false, true);
		
		// Run detection
		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(pRenameMoveKeyExpression.get());
		ctxt.m_pRenameMoveDetection->Detect(ctxt, true);
		
		// Verify that items were grouped
		EXPECT_NE(pdi1->renameMoveGroupId, -1);
		EXPECT_EQ(pdi1->renameMoveGroupId, pdi2->renameMoveGroupId);
		EXPECT_EQ(ctxt.m_pRenameMoveDetection->GetMovedItemGroups().size(), 1u);
	}

	/**
	 * @brief Test that files with different names are not grouped
	 */
	TEST_F(RenameMoveDetectionTest, DifferentNamesNotGrouped)
	{
		PathContext paths;
		paths.SetLeft(_T("C:\\Left"));
		paths.SetRight(_T("C:\\Right"));
		
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();
		
		auto pRenameMoveKeyExpression = std::make_unique<FilterExpression>();
		pRenameMoveKeyExpression->Parse("Name");
		pRenameMoveKeyExpression->SetDiffContext(&ctxt);
		
		DIFFITEM* pdi1 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem(*pdi1, _T("file1.txt"), true, false);
		
		DIFFITEM* pdi2 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem(*pdi2, _T("file2.txt"), false, true);
		
		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(pRenameMoveKeyExpression.get());
		ctxt.m_pRenameMoveDetection->Detect(ctxt, true);
		
		// Items should not be grouped
		EXPECT_EQ(pdi1->renameMoveGroupId, -1);
		EXPECT_EQ(pdi2->renameMoveGroupId, -1);
		EXPECT_EQ(ctxt.m_pRenameMoveDetection->GetMovedItemGroups().size(), 0u);
	}

	/**
	 * @brief Test that directories and files are not mixed
	 */
	TEST_F(RenameMoveDetectionTest, DirectoriesAndFilesNotMixed)
	{
		PathContext paths;
		paths.SetLeft(_T("C:\\Left"));
		paths.SetRight(_T("C:\\Right"));
		
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();
		
		auto pRenameMoveKeyExpression = std::make_unique<FilterExpression>();
		pRenameMoveKeyExpression->Parse("Name");
		pRenameMoveKeyExpression->SetDiffContext(&ctxt);
		
		// File on left
		DIFFITEM* pdi1 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem(*pdi1, _T("item1"), true, false, false);
		
		// Directory on right with same name
		DIFFITEM* pdi2 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem(*pdi2, _T("item1"), false, true, true);
		
		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(pRenameMoveKeyExpression.get());
		ctxt.m_pRenameMoveDetection->Detect(ctxt, true);
		
		// Should not be grouped
		EXPECT_EQ(pdi1->renameMoveGroupId, -1);
		EXPECT_EQ(pdi2->renameMoveGroupId, -1);
	}

	/**
 * @brief Create a mock DIFFITEM for testing with 3-way comparison
 */
	void CreateMockDiffItem3Way(DIFFITEM& di, const String& filename, bool existsLeft, bool existsMiddle, bool existsRight, bool isDirectory = false)
	{
		di.renameMoveGroupId = -1;

		if (existsLeft)
		{
			di.diffcode.setSideFlag(0);
			di.diffFileInfo[0].filename = filename;
			di.diffFileInfo[0].path = _T("");
			di.diffFileInfo[0].size = 1000;
		}

		if (existsMiddle)
		{
			di.diffcode.setSideFlag(1);
			di.diffFileInfo[1].filename = filename;
			di.diffFileInfo[1].path = _T("");
			di.diffFileInfo[1].size = 1000;
		}

		if (existsRight)
		{
			di.diffcode.setSideFlag(2);
			di.diffFileInfo[2].filename = filename;
			di.diffFileInfo[2].path = _T("");
			di.diffFileInfo[2].size = 1000;
		}

		if (isDirectory)
			di.diffcode.diffcode |= DIFFCODE::DIR;
		else
			di.diffcode.diffcode |= DIFFCODE::FILE;
	}

	/**
	 * @brief Test basic 3-way move detection
	 */
	TEST_F(RenameMoveDetectionTest, ThreeWayBasicMoveDetection)
	{
		PathContext paths;
		paths.SetLeft(_T("C:\\Left"));
		paths.SetMiddle(_T("C:\\Middle"));
		paths.SetRight(_T("C:\\Right"));

		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();

		auto pRenameMoveKeyExpression = std::make_unique<FilterExpression>();
		pRenameMoveKeyExpression->Parse("Name");
		pRenameMoveKeyExpression->SetDiffContext(&ctxt);

		// Add mock items - same file in all three locations with same name
		DIFFITEM* pdi1 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem3Way(*pdi1, _T("file1.txt"), true, false, false);

		DIFFITEM* pdi2 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem3Way(*pdi2, _T("file1.txt"), false, true, false);

		DIFFITEM* pdi3 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem3Way(*pdi3, _T("file1.txt"), false, false, true);

		// Run detection
		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(pRenameMoveKeyExpression.get());
		ctxt.m_pRenameMoveDetection->Detect(ctxt, true);

		// Verify that all items were grouped together
		EXPECT_NE(pdi1->renameMoveGroupId, -1);
		EXPECT_EQ(pdi1->renameMoveGroupId, pdi2->renameMoveGroupId);
		EXPECT_EQ(pdi2->renameMoveGroupId, pdi3->renameMoveGroupId);
		EXPECT_EQ(ctxt.m_pRenameMoveDetection->GetMovedItemGroups().size(), 1u);
	}

	/**
	 * @brief Test 3-way move detection with partial matches
	 */
	TEST_F(RenameMoveDetectionTest, ThreeWayPartialMatches)
	{
		PathContext paths;
		paths.SetLeft(_T("C:\\Left"));
		paths.SetMiddle(_T("C:\\Middle"));
		paths.SetRight(_T("C:\\Right"));

		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();

		auto pRenameMoveKeyExpression = std::make_unique<FilterExpression>();
		pRenameMoveKeyExpression->Parse("Name");
		pRenameMoveKeyExpression->SetDiffContext(&ctxt);

		// File exists in Left and Right only
		DIFFITEM* pdi1 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem3Way(*pdi1, _T("file1.txt"), true, false, false);

		DIFFITEM* pdi2 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem3Way(*pdi2, _T("file1.txt"), false, false, true);

		// File exists in Middle only (different file)
		DIFFITEM* pdi3 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem3Way(*pdi3, _T("file2.txt"), false, true, false);

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(pRenameMoveKeyExpression.get());
		ctxt.m_pRenameMoveDetection->Detect(ctxt, true);

		// file1.txt should be grouped (Left and Right)
		EXPECT_NE(pdi1->renameMoveGroupId, -1);
		EXPECT_EQ(pdi1->renameMoveGroupId, pdi2->renameMoveGroupId);

		// file2.txt should not be grouped (only in Middle)
		EXPECT_EQ(pdi3->renameMoveGroupId, -1);

		EXPECT_EQ(ctxt.m_pRenameMoveDetection->GetMovedItemGroups().size(), 1u);
	}

	/**
	 * @brief Test 3-way move detection with multiple groups
	 */
	TEST_F(RenameMoveDetectionTest, ThreeWayMultipleGroups)
	{
		PathContext paths;
		paths.SetLeft(_T("C:\\Left"));
		paths.SetMiddle(_T("C:\\Middle"));
		paths.SetRight(_T("C:\\Right"));

		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();

		auto pRenameMoveKeyExpression = std::make_unique<FilterExpression>();
		pRenameMoveKeyExpression->Parse("Name");
		pRenameMoveKeyExpression->SetDiffContext(&ctxt);

		// Group 1: file1.txt in Left and Middle
		DIFFITEM* pdi1 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem3Way(*pdi1, _T("file1.txt"), true, false, false);

		DIFFITEM* pdi2 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem3Way(*pdi2, _T("file1.txt"), false, true, false);

		// Group 2: file2.txt in Middle and Right
		DIFFITEM* pdi3 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem3Way(*pdi3, _T("file2.txt"), false, true, false);

		DIFFITEM* pdi4 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem3Way(*pdi4, _T("file2.txt"), false, false, true);

		// Group 3: file3.txt in all three locations
		DIFFITEM* pdi5 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem3Way(*pdi5, _T("file3.txt"), true, false, false);

		DIFFITEM* pdi6 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem3Way(*pdi6, _T("file3.txt"), false, true, false);

		DIFFITEM* pdi7 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem3Way(*pdi7, _T("file3.txt"), false, false, true);

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(pRenameMoveKeyExpression.get());
		ctxt.m_pRenameMoveDetection->Detect(ctxt, true);

		// Verify Group 1
		EXPECT_NE(pdi1->renameMoveGroupId, -1);
		EXPECT_EQ(pdi1->renameMoveGroupId, pdi2->renameMoveGroupId);

		// Verify Group 2
		EXPECT_NE(pdi3->renameMoveGroupId, -1);
		EXPECT_EQ(pdi3->renameMoveGroupId, pdi4->renameMoveGroupId);

		// Verify Group 3
		EXPECT_NE(pdi5->renameMoveGroupId, -1);
		EXPECT_EQ(pdi5->renameMoveGroupId, pdi6->renameMoveGroupId);
		EXPECT_EQ(pdi6->renameMoveGroupId, pdi7->renameMoveGroupId);

		// Verify groups are distinct
		EXPECT_NE(pdi1->renameMoveGroupId, pdi3->renameMoveGroupId);
		EXPECT_NE(pdi1->renameMoveGroupId, pdi5->renameMoveGroupId);
		EXPECT_NE(pdi3->renameMoveGroupId, pdi5->renameMoveGroupId);

		EXPECT_EQ(ctxt.m_pRenameMoveDetection->GetMovedItemGroups().size(), 3u);
	}

	/**
	 * @brief Test that 3-way directories and files are not mixed
	 */
	TEST_F(RenameMoveDetectionTest, ThreeWayDirectoriesAndFilesNotMixed)
	{
		PathContext paths;
		paths.SetLeft(_T("C:\\Left"));
		paths.SetMiddle(_T("C:\\Middle"));
		paths.SetRight(_T("C:\\Right"));

		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();

		auto pRenameMoveKeyExpression = std::make_unique<FilterExpression>();
		pRenameMoveKeyExpression->Parse("Name");
		pRenameMoveKeyExpression->SetDiffContext(&ctxt);

		// File in Left
		DIFFITEM* pdi1 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem3Way(*pdi1, _T("item1"), true, false, false, false);

		// Directory in Middle with same name
		DIFFITEM* pdi2 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem3Way(*pdi2, _T("item1"), false, true, false, true);

		// File in Right with same name
		DIFFITEM* pdi3 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem3Way(*pdi3, _T("item1"), false, false, true, false);

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(pRenameMoveKeyExpression.get());
		ctxt.m_pRenameMoveDetection->Detect(ctxt, true);

		// Left and Right files should be grouped
		EXPECT_NE(pdi1->renameMoveGroupId, -1);
		EXPECT_EQ(pdi1->renameMoveGroupId, pdi3->renameMoveGroupId);

		// Middle directory should not be grouped with files
		EXPECT_EQ(pdi2->renameMoveGroupId, -1);

		EXPECT_EQ(ctxt.m_pRenameMoveDetection->GetMovedItemGroups().size(), 1u);
	}

	/**
	 * @brief Test 3-way with complex expression
	 */
	TEST_F(RenameMoveDetectionTest, ThreeWayComplexExpression)
	{
		PathContext paths;
		paths.SetLeft(_T("C:\\Left"));
		paths.SetMiddle(_T("C:\\Middle"));
		paths.SetRight(_T("C:\\Right"));

		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();

		// Use size-based expression
		auto pRenameMoveKeyExpression = std::make_unique<FilterExpression>();
		pRenameMoveKeyExpression->Parse("Size");
		pRenameMoveKeyExpression->SetDiffContext(&ctxt);

		// Files with same size but different names
		DIFFITEM* pdi1 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem3Way(*pdi1, _T("fileA.txt"), true, false, false);
		pdi1->diffFileInfo[0].size = 2000;

		DIFFITEM* pdi2 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem3Way(*pdi2, _T("fileB.txt"), false, true, false);
		pdi2->diffFileInfo[1].size = 2000;

		DIFFITEM* pdi3 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem3Way(*pdi3, _T("fileC.txt"), false, false, true);
		pdi3->diffFileInfo[2].size = 2000;

		// File with different size
		DIFFITEM* pdi4 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem3Way(*pdi4, _T("fileD.txt"), true, false, false);
		pdi4->diffFileInfo[0].size = 3000;

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(pRenameMoveKeyExpression.get());
		ctxt.m_pRenameMoveDetection->Detect(ctxt, true);

		// Files with size 2000 should be grouped
		EXPECT_NE(pdi1->renameMoveGroupId, -1);
		EXPECT_EQ(pdi1->renameMoveGroupId, pdi2->renameMoveGroupId);
		EXPECT_EQ(pdi2->renameMoveGroupId, pdi3->renameMoveGroupId);

		// File with size 3000 should not be grouped
		EXPECT_EQ(pdi4->renameMoveGroupId, -1);

		EXPECT_EQ(ctxt.m_pRenameMoveDetection->GetMovedItemGroups().size(), 1u);
	}

	/**
	 * @brief Test 3-way with only single file in each location
	 */
	TEST_F(RenameMoveDetectionTest, ThreeWaySingleFilePerLocation)
	{
		PathContext paths;
		paths.SetLeft(_T("C:\\Left"));
		paths.SetMiddle(_T("C:\\Middle"));
		paths.SetRight(_T("C:\\Right"));

		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();

		auto pRenameMoveKeyExpression = std::make_unique<FilterExpression>();
		pRenameMoveKeyExpression->Parse("Name");
		pRenameMoveKeyExpression->SetDiffContext(&ctxt);

		// Only in Left
		DIFFITEM* pdi1 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem3Way(*pdi1, _T("leftonly.txt"), true, false, false);

		// Only in Middle
		DIFFITEM* pdi2 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem3Way(*pdi2, _T("middleonly.txt"), false, true, false);

		// Only in Right
		DIFFITEM* pdi3 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem3Way(*pdi3, _T("rightonly.txt"), false, false, true);

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(pRenameMoveKeyExpression.get());
		ctxt.m_pRenameMoveDetection->Detect(ctxt, true);

		// No items should be grouped (all have unique names)
		EXPECT_EQ(pdi1->renameMoveGroupId, -1);
		EXPECT_EQ(pdi2->renameMoveGroupId, -1);
		EXPECT_EQ(pdi3->renameMoveGroupId, -1);
		EXPECT_EQ(ctxt.m_pRenameMoveDetection->GetMovedItemGroups().size(), 0u);
	}

	/**
	 * @brief Test 3-way with mixed 2-way and 3-way matches
	 */
	TEST_F(RenameMoveDetectionTest, ThreeWayMixed2WayAnd3WayMatches)
	{
		PathContext paths;
		paths.SetLeft(_T("C:\\Left"));
		paths.SetMiddle(_T("C:\\Middle"));
		paths.SetRight(_T("C:\\Right"));

		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();

		auto pRenameMoveKeyExpression = std::make_unique<FilterExpression>();
		pRenameMoveKeyExpression->Parse("Name");
		pRenameMoveKeyExpression->SetDiffContext(&ctxt);

		// 3-way match: file1.txt
		DIFFITEM* pdi1 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem3Way(*pdi1, _T("file1.txt"), true, false, false);

		DIFFITEM* pdi2 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem3Way(*pdi2, _T("file1.txt"), false, true, false);

		DIFFITEM* pdi3 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem3Way(*pdi3, _T("file1.txt"), false, false, true);

		// 2-way match: file2.txt (Left-Right only)
		DIFFITEM* pdi4 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem3Way(*pdi4, _T("file2.txt"), true, false, false);

		DIFFITEM* pdi5 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem3Way(*pdi5, _T("file2.txt"), false, false, true);

		// 2-way match: file3.txt (Left-Middle only)
		DIFFITEM* pdi6 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem3Way(*pdi6, _T("file3.txt"), true, false, false);

		DIFFITEM* pdi7 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem3Way(*pdi7, _T("file3.txt"), false, true, false);

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(pRenameMoveKeyExpression.get());
		ctxt.m_pRenameMoveDetection->Detect(ctxt, true);

		// Verify 3-way match group
		EXPECT_NE(pdi1->renameMoveGroupId, -1);
		EXPECT_EQ(pdi1->renameMoveGroupId, pdi2->renameMoveGroupId);
		EXPECT_EQ(pdi2->renameMoveGroupId, pdi3->renameMoveGroupId);

		// Verify 2-way match group (Left-Right)
		EXPECT_NE(pdi4->renameMoveGroupId, -1);
		EXPECT_EQ(pdi4->renameMoveGroupId, pdi5->renameMoveGroupId);

		// Verify 2-way match group (Left-Middle)
		EXPECT_NE(pdi6->renameMoveGroupId, -1);
		EXPECT_EQ(pdi6->renameMoveGroupId, pdi7->renameMoveGroupId);

		// Verify all groups are distinct
		EXPECT_NE(pdi1->renameMoveGroupId, pdi4->renameMoveGroupId);
		EXPECT_NE(pdi1->renameMoveGroupId, pdi6->renameMoveGroupId);
		EXPECT_NE(pdi4->renameMoveGroupId, pdi6->renameMoveGroupId);

		EXPECT_EQ(ctxt.m_pRenameMoveDetection->GetMovedItemGroups().size(), 3u);
	}

}  // namespace
