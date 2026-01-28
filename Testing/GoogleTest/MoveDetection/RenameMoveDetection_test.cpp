#include "pch.h"
#include <gtest/gtest.h>
#include "RenameMoveDetection.h"
#include "DiffContext.h"
#include "DiffItem.h"
#include "FilterEngine/FilterExpression.h"
#include "CompareStats.h"
#include "PathContext.h"

namespace
{
	/**
	 * @brief Test fixture for RenameMoveDetection
	 */
	class RenameMoveDetectionTest : public testing::Test
	{
	protected:
		/**
		 * @brief Create a mock DIFFITEM for testing
		 */
		DIFFITEM* CreateMockDiffItem2Way(CDiffContext& ctxt, DIFFITEM* parent,
			const String& filename, const String& path, int64_t size,
			bool existsLeft, bool existsRight, bool isDirectory = false)
		{
			DIFFITEM* pdi = ctxt.AddNewDiff(parent);
			pdi->renameMoveGroupId = -1;

			for (int i = 0; i < ctxt.GetCompareDirs(); ++i)
			{
				bool exists = (i == 0 && existsLeft) ||
					(i == 1 && existsRight);
				if (exists)
				{
					pdi->diffcode.setSideFlag(i);
					pdi->diffFileInfo[i].filename = filename;
					pdi->diffFileInfo[i].path = path;
					pdi->diffFileInfo[i].size = size;
				}
			}

			if (isDirectory)
				pdi->diffcode.diffcode |= DIFFCODE::DIR;
			else
				pdi->diffcode.diffcode |= DIFFCODE::FILE;

			return pdi;
		}

		DIFFITEM* CreateMockDiffItem3Way(CDiffContext& ctxt, DIFFITEM* parent,
			const String& filename, const String& path, int64_t size,
			bool existsLeft, bool existsMiddle, bool existsRight, bool isDirectory = false)
		{
			DIFFITEM* pdi = ctxt.AddNewDiff(parent);
			pdi->renameMoveGroupId = -1;

			for (int i = 0; i < ctxt.GetCompareDirs(); ++i)
			{
				bool exists = (i == 0 && existsLeft) ||
					(i == 1 && (ctxt.GetCompareDirs() == 3 ? existsMiddle : existsRight)) ||
					(i == 2 && existsRight);
				if (exists)
				{
					pdi->diffcode.setSideFlag(i);
					pdi->diffFileInfo[i].filename = filename;
					pdi->diffFileInfo[i].path = path;
					pdi->diffFileInfo[i].size = size;
				}
			}

			if (isDirectory)
				pdi->diffcode.diffcode |= DIFFCODE::DIR;
			else
				pdi->diffcode.diffcode |= DIFFCODE::FILE;

			return pdi;
		}
	};

	// ========================================================================
	// Basic Detection Tests
	// ========================================================================

	/**
	 * @brief Test that Detect() does nothing when no expression is set
	 */
	TEST_F(RenameMoveDetectionTest, NoExpressionSet)
	{
		PathContext paths(_T("C:\\Left"), _T("C:\\Right"));
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();
		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();

		CreateMockDiffItem2Way(ctxt, nullptr, _T("file.txt"), _T(""), 1000, true, false);
		CreateMockDiffItem2Way(ctxt, nullptr, _T("file.txt"), _T(""), 1000, false, true);

		// Should not crash and should do nothing
		EXPECT_NO_THROW(ctxt.m_pRenameMoveDetection->Detect(ctxt, true));

		DIFFITEM* diffpos = ctxt.GetFirstDiffPosition();
		while (diffpos)
		{
			DIFFITEM& di = ctxt.GetNextDiffRefPosition(diffpos);
			EXPECT_EQ(di.renameMoveGroupId, -1);
		}
	}

	/**
	 * @brief Test basic rename detection (same directory, different name)
	 */
	TEST_F(RenameMoveDetectionTest, BasicRenameDetection_2Way)
	{
		PathContext paths(_T("C:\\Left"), _T("C:\\Right"));
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();

		auto expr = std::make_unique<FilterExpression>();
		expr->Parse("Size");
		expr->SetDiffContext(&ctxt);

		// Same size, different names
		auto* pdi1 = CreateMockDiffItem2Way(ctxt, nullptr, _T("old.txt"), _T(""), 1000, true, false);
		auto* pdi2 = CreateMockDiffItem2Way(ctxt, nullptr, _T("new.txt"), _T(""), 1000, false, true);

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(expr.get());
		ctxt.m_pRenameMoveDetection->Detect(ctxt, false);

		// Should be grouped
		EXPECT_NE(pdi1->renameMoveGroupId, -1);
		EXPECT_EQ(pdi1->renameMoveGroupId, pdi2->renameMoveGroupId);
	}

	/**
	 * @brief Test basic rename detection (3-way)
	 */
	TEST_F(RenameMoveDetectionTest, BasicRenameDetection_3Way)
	{
		PathContext paths(_T("C:\\Left"), _T("C:\\Middle"), _T("C:\\Right"));
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();

		auto expr = std::make_unique<FilterExpression>();
		expr->Parse("Size");
		expr->SetDiffContext(&ctxt);

		// Same size, different names
		auto* pdi1 = CreateMockDiffItem3Way(ctxt, nullptr, _T("file1.txt"), _T(""), 2000, true, false, false);
		auto* pdi2 = CreateMockDiffItem3Way(ctxt, nullptr, _T("file2.txt"), _T(""), 2000, false, true, false);
		auto* pdi3 = CreateMockDiffItem3Way(ctxt, nullptr, _T("file3.txt"), _T(""), 2000, false, false, true);

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(expr.get());
		ctxt.m_pRenameMoveDetection->Detect(ctxt, false);

		// All should be grouped together
		EXPECT_NE(pdi1->renameMoveGroupId, -1);
		EXPECT_EQ(pdi1->renameMoveGroupId, pdi2->renameMoveGroupId);
		EXPECT_EQ(pdi2->renameMoveGroupId, pdi3->renameMoveGroupId);
	}

	/**
	 * @brief Test move detection (different directory)
	 */
	TEST_F(RenameMoveDetectionTest, BasicMoveDetection)
	{
		PathContext paths(_T("C:\\Left"), _T("C:\\Right"));
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();

		auto expr = std::make_unique<FilterExpression>();
		expr->Parse("Name");
		expr->SetDiffContext(&ctxt);

		// Create parent directories
		auto* parent1 = CreateMockDiffItem2Way(ctxt, nullptr, _T("dir1"), _T(""), 0, true, false, true);
		auto* parent2 = CreateMockDiffItem2Way(ctxt, nullptr, _T("dir2"), _T(""), 0, false, true, true);

		// Files with same name in different directories
		auto* pdi1 = CreateMockDiffItem2Way(ctxt, parent1, _T("file.txt"), _T("dir1"), 1000, true, false);
		auto* pdi2 = CreateMockDiffItem2Way(ctxt, parent2, _T("file.txt"), _T("dir2"), 1000, false, true);

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(expr.get());
		ctxt.m_pRenameMoveDetection->Detect(ctxt, true);  // Enable move detection

		// Should be grouped
		EXPECT_NE(pdi1->renameMoveGroupId, -1);
		EXPECT_EQ(pdi1->renameMoveGroupId, pdi2->renameMoveGroupId);
	}

	/**
	 * @brief Test that move detection is disabled when doMoveDetection=false
	 */
	TEST_F(RenameMoveDetectionTest, MoveDetectionDisabled)
	{
		PathContext paths(_T("C:\\Left"), _T("C:\\Right"));
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();

		auto expr = std::make_unique<FilterExpression>();
		expr->Parse("Name");
		expr->SetDiffContext(&ctxt);

		// Create parent directories
		auto* parent1 = CreateMockDiffItem2Way(ctxt, nullptr, _T("dir1"), _T(""), 0, true, false, true);
		auto* parent2 = CreateMockDiffItem2Way(ctxt, nullptr, _T("dir2"), _T(""), 0, false, true, true);

		// Files in different directories
		auto* pdi1 = CreateMockDiffItem2Way(ctxt, parent1, _T("file.txt"), _T("dir1"), 1000, true, false);
		auto* pdi2 = CreateMockDiffItem2Way(ctxt, parent2, _T("file.txt"), _T("dir2"), 1000, false, true);

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(expr.get());
		ctxt.m_pRenameMoveDetection->Detect(ctxt, false);  // Disable move detection

		// Should NOT be grouped (move detection disabled)
		EXPECT_EQ(pdi1->renameMoveGroupId, -1);
		EXPECT_EQ(pdi2->renameMoveGroupId, -1);
	}

	// ========================================================================
	// Same Name Grouping Tests
	// ========================================================================

	/**
	 * @brief Test quick grouping by same name
	 */
	TEST_F(RenameMoveDetectionTest, SameNameGrouping)
	{
		PathContext paths(_T("C:\\Left"), _T("C:\\Right"));
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();

		auto expr = std::make_unique<FilterExpression>();
		expr->Parse("replace(Name, \"old\", \"new\")");
		expr->SetDiffContext(&ctxt);

		auto* parent1 = CreateMockDiffItem2Way(ctxt, nullptr, _T("dir_old"), _T(""), 0, true, false, true);
		auto* parent2 = CreateMockDiffItem2Way(ctxt, nullptr, _T("dir_new"), _T(""), 0, false, true, true);

		auto* pdi1 = CreateMockDiffItem2Way(ctxt, parent1, _T("file.txt"), _T("dir_old"), 1000, true, false);
		auto* pdi2 = CreateMockDiffItem2Way(ctxt, parent2, _T("file.txt"), _T("dir_new"), 1000, false, true);

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(expr.get());
		ctxt.m_pRenameMoveDetection->Detect(ctxt, false);

		// Should be grouped by name even though sizes differ
		EXPECT_NE(pdi1->renameMoveGroupId, -1);
		EXPECT_EQ(pdi1->renameMoveGroupId, pdi2->renameMoveGroupId);
	}

	// ========================================================================
	// Merge Tests
	// ========================================================================

	/**
	 * @brief Test merge of renamed items in same directory
	 */
	TEST_F(RenameMoveDetectionTest, MergeRenamedItems)
	{
		PathContext paths(_T("C:\\Left"), _T("C:\\Right"));
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();
		CompareStats compareStats(2);
		ctxt.m_pCompareStats = &compareStats;

		auto expr = std::make_unique<FilterExpression>();
		expr->Parse("Size");
		expr->SetDiffContext(&ctxt);

		CreateMockDiffItem2Way(ctxt, nullptr, _T("old.txt"), _T(""), 1000, true, false);
		CreateMockDiffItem2Way(ctxt, nullptr, _T("new.txt"), _T(""), 1000, false, true);

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(expr.get());
		ctxt.m_pRenameMoveDetection->Detect(ctxt, false);
		ctxt.m_pRenameMoveDetection->Merge(ctxt);

		// After merge, one item should exist on both sides
		int mergedItemCount = 0;
		DIFFITEM* diffpos = ctxt.GetFirstDiffPosition();
		while (diffpos)
		{
			DIFFITEM& di = ctxt.GetNextDiffRefPosition(diffpos);
			mergedItemCount++;
			// Merged item should exist on both sides
			EXPECT_TRUE(di.diffcode.exists(0));
			EXPECT_TRUE(di.diffcode.exists(1));
		}
		EXPECT_EQ(mergedItemCount, 1);
	}

	/**
	 * @brief Test merge does not happen for items in different directories
	 */
	TEST_F(RenameMoveDetectionTest, MergeNotInDifferentDirectories)
	{
		PathContext paths(_T("C:\\Left"), _T("C:\\Right"));
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();
		CompareStats compareStats(2);
		ctxt.m_pCompareStats = &compareStats;

		auto expr = std::make_unique<FilterExpression>();
		expr->Parse("Name");
		expr->SetDiffContext(&ctxt);

		auto* parent1 = CreateMockDiffItem2Way(ctxt, nullptr, _T("dir1"), _T(""), 0, true, false, true);
		auto* parent2 = CreateMockDiffItem2Way(ctxt, nullptr, _T("dir2"), _T(""), 0, false, true, true);

		auto* pdi1 = CreateMockDiffItem2Way(ctxt, parent1, _T("file.txt"), _T("dir1"), 1000, true, false);
		auto* pdi2 = CreateMockDiffItem2Way(ctxt, parent2, _T("file.txt"), _T("dir2"), 1000, false, true);

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(expr.get());
		ctxt.m_pRenameMoveDetection->Detect(ctxt, true);
		ctxt.m_pRenameMoveDetection->Merge(ctxt);

		// Items should be grouped but NOT merged (different directories)
		EXPECT_EQ(pdi1->renameMoveGroupId, pdi2->renameMoveGroupId);

		int itemCount = 0;
		DIFFITEM* diffpos = ctxt.GetFirstDiffPosition();
		while (diffpos)
		{
			ctxt.GetNextDiffRefPosition(diffpos);
			itemCount++;
		}
		EXPECT_EQ(itemCount, 4);  // 2 parents + 2 files
	}

	/**
	 * @brief Test merge with 3-way comparison
	 */
	TEST_F(RenameMoveDetectionTest, Merge_3Way)
	{
		PathContext paths(_T("C:\\Left"), _T("C:\\Middle"), _T("C:\\Right"));
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();
		CompareStats compareStats(3);
		ctxt.m_pCompareStats = &compareStats;

		auto expr = std::make_unique<FilterExpression>();
		expr->Parse("Size");
		expr->SetDiffContext(&ctxt);

		CreateMockDiffItem3Way(ctxt, nullptr, _T("file1.txt"), _T(""), 1000, true, false, false);
		CreateMockDiffItem3Way(ctxt, nullptr, _T("file2.txt"), _T(""), 1000, false, true, false);
		CreateMockDiffItem3Way(ctxt, nullptr, _T("file3.txt"), _T(""), 1000, false, false, true);

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(expr.get());
		ctxt.m_pRenameMoveDetection->Detect(ctxt, false);
		ctxt.m_pRenameMoveDetection->Merge(ctxt);

		// After merge, should have 1 item on all 3 sides
		int itemCount = 0;
		DIFFITEM* diffpos = ctxt.GetFirstDiffPosition();
		while (diffpos)
		{
			DIFFITEM& di = ctxt.GetNextDiffRefPosition(diffpos);
			itemCount++;
			EXPECT_TRUE(di.diffcode.exists(0));
			EXPECT_TRUE(di.diffcode.exists(1));
			EXPECT_TRUE(di.diffcode.exists(2));
		}
		EXPECT_EQ(itemCount, 1);
	}

	// ========================================================================
	// GetRenameMoveGroupItemsForSide Tests
	// ========================================================================

	/**
	 * @brief Test GetRenameMoveGroupItemsForSide
	 */
	TEST_F(RenameMoveDetectionTest, GetRenameMoveGroupItemsForSide)
	{
		PathContext paths(_T("C:\\Left"), _T("C:\\Middle"), _T("C:\\Right"));
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();

		auto expr = std::make_unique<FilterExpression>();
		expr->Parse("Name");
		expr->SetDiffContext(&ctxt);

		auto* pdi1 = CreateMockDiffItem3Way(ctxt, nullptr, _T("file.txt"), _T(""), 1000, true, false, false);
		auto* pdi2 = CreateMockDiffItem3Way(ctxt, nullptr, _T("file.txt"), _T(""), 1000, false, true, false);
		auto* pdi3 = CreateMockDiffItem3Way(ctxt, nullptr, _T("file.txt"), _T(""), 1000, false, false, true);

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(expr.get());
		ctxt.m_pRenameMoveDetection->Detect(ctxt, true);

		// Get items for each side
		auto leftItems = ctxt.m_pRenameMoveDetection->GetRenameMoveGroupItemsForSide(*pdi1, 0);
		auto middleItems = ctxt.m_pRenameMoveDetection->GetRenameMoveGroupItemsForSide(*pdi1, 1);
		auto rightItems = ctxt.m_pRenameMoveDetection->GetRenameMoveGroupItemsForSide(*pdi1, 2);

		EXPECT_EQ(leftItems.size(), 1);
		EXPECT_EQ(middleItems.size(), 1);
		EXPECT_EQ(rightItems.size(), 1);
		EXPECT_EQ(leftItems[0], pdi1);
		EXPECT_EQ(middleItems[0], pdi2);
		EXPECT_EQ(rightItems[0], pdi3);
	}

	/**
	 * @brief Test GetRenameMoveGroupItemsForSide with item not in group
	 */
	TEST_F(RenameMoveDetectionTest, GetRenameMoveGroupItemsForSide_NotInGroup)
	{
		PathContext paths(_T("C:\\Left"), _T("C:\\Right"));
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();

		auto* pdi = CreateMockDiffItem2Way(ctxt, nullptr, _T("file.txt"), _T(""), 1000, true, false);

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();

		auto items = ctxt.m_pRenameMoveDetection->GetRenameMoveGroupItemsForSide(*pdi, 0);
		EXPECT_TRUE(items.empty());
	}

	// ========================================================================
	// CheckMovedOrRenamed Tests
	// ========================================================================

	/**
	 * @brief Test CheckMovedOrRenamed detects rename
	 */
	TEST_F(RenameMoveDetectionTest, CheckMovedOrRenamed_Renamed)
	{
		PathContext paths(_T("C:\\Left"), _T("C:\\Right"));
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();

		auto expr = std::make_unique<FilterExpression>();
		expr->Parse("Size");
		expr->SetDiffContext(&ctxt);

		auto* pdi1 = CreateMockDiffItem2Way(ctxt, nullptr, _T("old.txt"), _T(""), 1000, true, false);
		CreateMockDiffItem2Way(ctxt, nullptr, _T("new.txt"), _T(""), 1000, false, true);

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(expr.get());
		ctxt.m_pRenameMoveDetection->Detect(ctxt, false);

		bool moved = false, renamed = false;
		ctxt.m_pRenameMoveDetection->CheckMovedOrRenamed(ctxt, *pdi1, moved, renamed);

		EXPECT_FALSE(moved);
		EXPECT_TRUE(renamed);
	}

	/**
	 * @brief Test CheckMovedOrRenamed detects move
	 */
	TEST_F(RenameMoveDetectionTest, CheckMovedOrRenamed_Moved)
	{
		PathContext paths(_T("C:\\Left"), _T("C:\\Right"));
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();

		auto expr = std::make_unique<FilterExpression>();
		expr->Parse("Name");
		expr->SetDiffContext(&ctxt);

		auto* parent1 = CreateMockDiffItem2Way(ctxt, nullptr, _T("dir1"), _T(""), 0, true, false, true);
		auto* parent2 = CreateMockDiffItem2Way(ctxt, nullptr, _T("dir2"), _T(""), 0, false, true, true);

		auto* pdi1 = CreateMockDiffItem2Way(ctxt, parent1, _T("file.txt"), _T("dir1"), 1000, true, false);
		auto* pdi2 = CreateMockDiffItem2Way(ctxt, parent2, _T("file.txt"), _T("dir2"), 1000, false, true);

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(expr.get());
		ctxt.m_pRenameMoveDetection->Detect(ctxt, true);

		bool moved = false, renamed = false;
		ctxt.m_pRenameMoveDetection->CheckMovedOrRenamed(ctxt, *pdi1, moved, renamed);

		EXPECT_TRUE(moved);
		EXPECT_FALSE(renamed);
	}

	/**
	 * @brief Test CheckMovedOrRenamed detects both move and rename
	 */
	TEST_F(RenameMoveDetectionTest, CheckMovedOrRenamed_MovedAndRenamed)
	{
		PathContext paths(_T("C:\\Left"), _T("C:\\Right"));
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();

		auto expr = std::make_unique<FilterExpression>();
		expr->Parse("Size");
		expr->SetDiffContext(&ctxt);

		auto* parent1 = CreateMockDiffItem2Way(ctxt, nullptr, _T("dir1"), _T(""), 0, true, false, true);
		auto* parent2 = CreateMockDiffItem2Way(ctxt, nullptr, _T("dir2"), _T(""), 0, false, true, true);

		auto* pdi1 = CreateMockDiffItem2Way(ctxt, parent1, _T("old.txt"), _T("dir1"), 1000, true, false);
		auto* pdi2 = CreateMockDiffItem2Way(ctxt, parent2, _T("new.txt"), _T("dir2"), 1000, false, true);

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(expr.get());
		ctxt.m_pRenameMoveDetection->Detect(ctxt, true);

		bool moved = false, renamed = false;
		ctxt.m_pRenameMoveDetection->CheckMovedOrRenamed(ctxt, *pdi1, moved, renamed);

		EXPECT_TRUE(moved);
		EXPECT_TRUE(renamed);
	}

	/**
	 * @brief Test CheckMovedOrRenamed for item not in group
	 */
	TEST_F(RenameMoveDetectionTest, CheckMovedOrRenamed_NotInGroup)
	{
		PathContext paths(_T("C:\\Left"), _T("C:\\Right"));
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();

		auto* pdi = CreateMockDiffItem2Way(ctxt, nullptr, _T("file.txt"), _T(""), 1000, true, false);

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();

		bool moved = true, renamed = true;
		ctxt.m_pRenameMoveDetection->CheckMovedOrRenamed(ctxt, *pdi, moved, renamed);

		EXPECT_FALSE(moved);
		EXPECT_FALSE(renamed);
	}

	// ========================================================================
	// Edge Cases and Complex Scenarios
	// ========================================================================

	/**
	 * @brief Test multiple files with same key (ambiguous)
	 */
	TEST_F(RenameMoveDetectionTest, MultipleFilesWithSameKey)
	{
		PathContext paths(_T("C:\\Left"), _T("C:\\Right"));
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();

		auto expr = std::make_unique<FilterExpression>();
		expr->Parse("Size");
		expr->SetDiffContext(&ctxt);

		// Multiple files on left with same size
		auto* pdi1 = CreateMockDiffItem2Way(ctxt, nullptr, _T("file1.txt"), _T(""), 1000, true, false);
		auto* pdi2 = CreateMockDiffItem2Way(ctxt, nullptr, _T("file2.txt"), _T(""), 1000, true, false);

		// Single file on right with same size
		auto* pdi3 = CreateMockDiffItem2Way(ctxt, nullptr, _T("file3.txt"), _T(""), 1000, false, true);

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(expr.get());
		ctxt.m_pRenameMoveDetection->Detect(ctxt, false);

		// All should be grouped (ambiguous case)
		EXPECT_NE(pdi1->renameMoveGroupId, -1);
		EXPECT_EQ(pdi1->renameMoveGroupId, pdi2->renameMoveGroupId);
		EXPECT_EQ(pdi2->renameMoveGroupId, pdi3->renameMoveGroupId);

		// But merge should not happen (multiple items on one side)
		ctxt.m_pRenameMoveDetection->Merge(ctxt);

		int itemCount = 0;
		DIFFITEM* diffpos = ctxt.GetFirstDiffPosition();
		while (diffpos)
		{
			ctxt.GetNextDiffRefPosition(diffpos);
			itemCount++;
		}
		EXPECT_EQ(itemCount, 3);  // All 3 items remain separate
	}

	/**
	 * @brief Test items that exist on all sides are not grouped
	 */
	TEST_F(RenameMoveDetectionTest, ItemsExistingOnAllSidesNotGrouped)
	{
		PathContext paths(_T("C:\\Left"), _T("C:\\Right"));
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();

		auto expr = std::make_unique<FilterExpression>();
		expr->Parse("Name");
		expr->SetDiffContext(&ctxt);

		// File exists on both sides
		auto* pdi = CreateMockDiffItem2Way(ctxt, nullptr, _T("file.txt"), _T(""), 1000, true, true);

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(expr.get());
		ctxt.m_pRenameMoveDetection->Detect(ctxt, true);

		// Should not be grouped (already exists on all sides)
		EXPECT_EQ(pdi->renameMoveGroupId, -1);
	}

	/**
	 * @brief Test directory rename detection
	 */
	TEST_F(RenameMoveDetectionTest, DirectoryRenameDetection)
	{
		PathContext paths(_T("C:\\Left"), _T("C:\\Right"));
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();

		auto expr = std::make_unique<FilterExpression>();
		expr->Parse("replace(Name, \"old\", \"new\")");
		expr->SetDiffContext(&ctxt);

		auto* pdi1 = CreateMockDiffItem2Way(ctxt, nullptr, _T("olddir"), _T(""), 0, true, false, true);
		auto* pdi2 = CreateMockDiffItem2Way(ctxt, nullptr, _T("newdir"), _T(""), 0, false, true, true);

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(expr.get());
		ctxt.m_pRenameMoveDetection->Detect(ctxt, false);

		// Directories should be grouped
		EXPECT_NE(pdi1->renameMoveGroupId, -1);
		EXPECT_EQ(pdi1->renameMoveGroupId, pdi2->renameMoveGroupId);
	}

	/**
	 * @brief Test recursive detection in subdirectories
	 */
	TEST_F(RenameMoveDetectionTest, RecursiveDetection)
	{
		PathContext paths(_T("C:\\Left"), _T("C:\\Right"));
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();

		auto expr = std::make_unique<FilterExpression>();
		expr->Parse("replace(Name, \"old\", \"new\")");
		expr->SetDiffContext(&ctxt);

		// Parent directories exist on both sides
		auto* parentLeft = CreateMockDiffItem2Way(ctxt, nullptr, _T("subdir"), _T(""), 0, true, true, true);

		// Files in subdirectories
		auto* pdi1 = CreateMockDiffItem2Way(ctxt, parentLeft, _T("file_old.txt"), _T("subdir"), 1000, true, false);
		auto* pdi2 = CreateMockDiffItem2Way(ctxt, parentLeft, _T("file_new.txt"), _T("subdir"), 1000, false, true);

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(expr.get());
		ctxt.m_pRenameMoveDetection->Detect(ctxt, false);

		// Files in subdirectory should be grouped
		EXPECT_NE(pdi1->renameMoveGroupId, -1);
		EXPECT_EQ(pdi1->renameMoveGroupId, pdi2->renameMoveGroupId);
	}

	/**
	 * @brief Test detection with renamed parent directories
	 */
	TEST_F(RenameMoveDetectionTest, RenamedParentDirectories)
	{
		PathContext paths(_T("C:\\Left"), _T("C:\\Right"));
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();

		auto expr = std::make_unique<FilterExpression>();
		expr->Parse("replace(Name, \"old\", \"new\")");
		expr->SetDiffContext(&ctxt);

		// Renamed directories
		auto* parentLeft = CreateMockDiffItem2Way(ctxt, nullptr, _T("olddir"), _T(""), 0, true, false, true);
		auto* parentRight = CreateMockDiffItem2Way(ctxt, nullptr, _T("newdir"), _T(""), 0, false, true, true);

		// Files with same name in renamed directories
		auto* pdi1 = CreateMockDiffItem2Way(ctxt, parentLeft, _T("file.txt"), _T("olddir"), 1000, true, false);
		auto* pdi2 = CreateMockDiffItem2Way(ctxt, parentRight, _T("file.txt"), _T("newdir"), 1000, false, true);

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(expr.get());
		ctxt.m_pRenameMoveDetection->Detect(ctxt, false);

		// Both directories and files should be grouped
		EXPECT_NE(parentLeft->renameMoveGroupId, -1);
		EXPECT_EQ(parentLeft->renameMoveGroupId, parentRight->renameMoveGroupId);
		EXPECT_NE(pdi1->renameMoveGroupId, -1);
		EXPECT_EQ(pdi1->renameMoveGroupId, pdi2->renameMoveGroupId);
	}

	/**
	 * @brief Test empty expression
	 */
	TEST_F(RenameMoveDetectionTest, EmptyExpression)
	{
		PathContext paths(_T("C:\\Left"), _T("C:\\Right"));
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();

		auto expr = std::make_unique<FilterExpression>();
		expr->Parse("");
		expr->SetDiffContext(&ctxt);

		auto* pdi1 = CreateMockDiffItem2Way(ctxt, nullptr, _T("file.txt"), _T(""), 1000, true, false);
		auto* pdi2 = CreateMockDiffItem2Way(ctxt, nullptr, _T("file.txt"), _T(""), 1000, false, true);

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(expr.get());
		ctxt.m_pRenameMoveDetection->Detect(ctxt, true);

		EXPECT_EQ(pdi1->renameMoveGroupId, -1);
		EXPECT_EQ(pdi1->renameMoveGroupId, pdi2->renameMoveGroupId);
	}

	/**
	 * @brief Test expression that returns array
	 */
	TEST_F(RenameMoveDetectionTest, ExpressionReturnsArray)
	{
		PathContext paths(_T("C:\\Left"), _T("C:\\Right"));
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();

		// Expression that concatenates name and size
		auto expr = std::make_unique<FilterExpression>();
		expr->Parse("BaseName + \"_\" + Size");
		expr->SetDiffContext(&ctxt);

		auto* pdi1 = CreateMockDiffItem2Way(ctxt, nullptr, _T("file.tx1"), _T(""), 1000, true, false);
		auto* pdi2 = CreateMockDiffItem2Way(ctxt, nullptr, _T("file.tx2"), _T(""), 1000, false, true);
		auto* pdi3 = CreateMockDiffItem2Way(ctxt, nullptr, _T("file.tx3"), _T(""), 2000, false, true);

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(expr.get());
		ctxt.m_pRenameMoveDetection->Detect(ctxt, true);

		// pdi1 and pdi2 should be grouped (same name and size)
		EXPECT_NE(pdi1->renameMoveGroupId, -1);
		EXPECT_EQ(pdi1->renameMoveGroupId, pdi2->renameMoveGroupId);

		// pdi3 should not be grouped (different size)
		EXPECT_NE(pdi3->renameMoveGroupId, pdi1->renameMoveGroupId);
	}

	/**
	 * @brief Test with CompareStats integration
	 */
	TEST_F(RenameMoveDetectionTest, CompareStatsIntegration)
	{
		PathContext paths(_T("C:\\Left"), _T("C:\\Right"));
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();
		CompareStats compareStats(2);
		ctxt.m_pCompareStats = &compareStats;
		ctxt.m_pCompareStats->IncreaseTotalItems(2);

		auto expr = std::make_unique<FilterExpression>();
		expr->Parse("BaseName");
		expr->SetDiffContext(&ctxt);

		auto* pdi1 = CreateMockDiffItem2Way(ctxt, nullptr, _T("file.txt"), _T(""), 1000, true, false);
		auto* pdi2 = CreateMockDiffItem2Way(ctxt, nullptr, _T("file.doc"), _T(""), 1000, false, true);

		int initialCount = ctxt.m_pCompareStats->GetTotalItems();

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(expr.get());
		ctxt.m_pRenameMoveDetection->Detect(ctxt, false);

		// Total items should be restored after detection
		EXPECT_EQ(ctxt.m_pCompareStats->GetTotalItems(), initialCount);

		ctxt.m_pRenameMoveDetection->Merge(ctxt);

		// After merge, total items should be reduced by 1
		EXPECT_EQ(ctxt.m_pCompareStats->GetTotalItems(), initialCount - 1);
	}

	/**
	 * @brief Test merge with child items (reparenting)
	 */
	TEST_F(RenameMoveDetectionTest, MergeWithChildItems)
	{
		PathContext paths(_T("C:\\Left"), _T("C:\\Right"));
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();
		CompareStats compareStats(2);
		ctxt.m_pCompareStats = &compareStats;

		auto expr = std::make_unique<FilterExpression>();
		expr->Parse("BaseName");
		expr->SetDiffContext(&ctxt);

		// Renamed directories
		auto* parent1 = CreateMockDiffItem2Way(ctxt, nullptr, _T("dir.ex1"), _T(""), 0, true, false, true);
		auto* parent2 = CreateMockDiffItem2Way(ctxt, nullptr, _T("dir.ex2"), _T(""), 0, false, true, true);

		// Child file in parent2
		auto* child = CreateMockDiffItem2Way(ctxt, parent2, _T("child.txt"), _T("dir.ex2"), 500, false, false, true);

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(expr.get());
		ctxt.m_pRenameMoveDetection->Detect(ctxt, false);
		ctxt.m_pRenameMoveDetection->Merge(ctxt);

		// After merge, parent1 should have the child
		EXPECT_TRUE(parent1->HasChildren());
		EXPECT_EQ(parent1->GetFirstChild(), child);
		EXPECT_EQ(child->GetParentLink(), parent1);
	}

	/**
	 * @brief Test that files and directories with same name are separate
	 */
	TEST_F(RenameMoveDetectionTest, FilesAndDirectoriesKeptSeparate)
	{
		PathContext paths(_T("C:\\Left"), _T("C:\\Right"));
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();

		auto expr = std::make_unique<FilterExpression>();
		expr->Parse("Name");
		expr->SetDiffContext(&ctxt);

		// File and directory with same name
		auto* file1 = CreateMockDiffItem2Way(ctxt, nullptr, _T("item"), _T(""), 1000, true, false, false);
		auto* file2 = CreateMockDiffItem2Way(ctxt, nullptr, _T("item"), _T(""), 1000, false, true, false);
		auto* dir1 = CreateMockDiffItem2Way(ctxt, nullptr, _T("item"), _T(""), 0, true, false, true);
		auto* dir2 = CreateMockDiffItem2Way(ctxt, nullptr, _T("item"), _T(""), 0, false, true, true);

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(expr.get());
		ctxt.m_pRenameMoveDetection->Detect(ctxt, true);

		// Files should be grouped together
		EXPECT_NE(file1->renameMoveGroupId, -1);
		EXPECT_EQ(file1->renameMoveGroupId, file2->renameMoveGroupId);

		// Directories should be grouped together
		EXPECT_NE(dir1->renameMoveGroupId, -1);
		EXPECT_EQ(dir1->renameMoveGroupId, dir2->renameMoveGroupId);

		// But files and directories should be in different groups
		EXPECT_NE(file1->renameMoveGroupId, dir1->renameMoveGroupId);

		EXPECT_EQ(ctxt.m_pRenameMoveDetection->GetRenameMoveItemGroups().size(), 2);
	}

	/**
	 * @brief Test partial match in 3-way (only 2 sides match)
	 */
	TEST_F(RenameMoveDetectionTest, ThreeWay_PartialMatch)
	{
		PathContext paths(_T("C:\\Left"), _T("C:\\Middle"), _T("C:\\Right"));
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();

		auto expr = std::make_unique<FilterExpression>();
		expr->Parse("BaseName");
		expr->SetDiffContext(&ctxt);

		// file1.tx* in Left and Middle
		auto* pdi1 = CreateMockDiffItem3Way(ctxt, nullptr, _T("file1.tx1"), _T(""), 1000, true, false, false);
		auto* pdi2 = CreateMockDiffItem3Way(ctxt, nullptr, _T("file1.tx2"), _T(""), 1000, false, true, false);

		// file2.txt only in Right
		auto* pdi3 = CreateMockDiffItem3Way(ctxt, nullptr, _T("file2.txt"), _T(""), 2000, false, false, true);

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(expr.get());
		ctxt.m_pRenameMoveDetection->Detect(ctxt, true);

		// file1.tx* should be grouped (2 sides)
		EXPECT_NE(pdi1->renameMoveGroupId, -1);
		EXPECT_EQ(pdi1->renameMoveGroupId, pdi2->renameMoveGroupId);

		// file2.txt should not be grouped (only 1 side)
		EXPECT_EQ(pdi3->renameMoveGroupId, -1);

		EXPECT_EQ(ctxt.m_pRenameMoveDetection->GetRenameMoveItemGroups().size(), 1);
	}

	/**
	 * @brief Test complex expression with multiple attributes
	 */
	TEST_F(RenameMoveDetectionTest, ComplexExpressionMultipleAttributes)
	{
		PathContext paths(_T("C:\\Left"), _T("C:\\Right"));
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();

		// Combine name and size
		auto expr = std::make_unique<FilterExpression>();
		expr->Parse("BaseName + \"_\" + Size");
		expr->SetDiffContext(&ctxt);

		auto* pdi1 = CreateMockDiffItem2Way(ctxt, nullptr, _T("file.tx1"), _T(""), 1000, true, false);
		auto* pdi2 = CreateMockDiffItem2Way(ctxt, nullptr, _T("file.tx2"), _T(""), 1000, false, true);
		auto* pdi3 = CreateMockDiffItem2Way(ctxt, nullptr, _T("file.tx3"), _T(""), 2000, false, true);

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(expr.get());
		ctxt.m_pRenameMoveDetection->Detect(ctxt, true);

		// pdi1 and pdi2 should be grouped (same name+size)
		EXPECT_NE(pdi1->renameMoveGroupId, -1);
		EXPECT_EQ(pdi1->renameMoveGroupId, pdi2->renameMoveGroupId);

		// pdi3 should be separate (different size)
		EXPECT_NE(pdi3->renameMoveGroupId, pdi1->renameMoveGroupId);
	}

	/**
	 * @brief Test GetRenameMoveItemGroups returns correct groups
	 */
	TEST_F(RenameMoveDetectionTest, GetRenameMoveItemGroups)
	{
		PathContext paths(_T("C:\\Left"), _T("C:\\Right"));
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();

		auto expr = std::make_unique<FilterExpression>();
		expr->Parse("BaseName");
		expr->SetDiffContext(&ctxt);

		// Group 1
		auto* pdi1 = CreateMockDiffItem2Way(ctxt, nullptr, _T("file1.tx1"), _T(""), 1000, true, false);
		CreateMockDiffItem2Way(ctxt, nullptr, _T("file1.tx2"), _T(""), 1000, false, true);

		// Group 2
		auto* pdi3 = CreateMockDiffItem2Way(ctxt, nullptr, _T("file2.tx1"), _T(""), 2000, true, false);
		CreateMockDiffItem2Way(ctxt, nullptr, _T("file2.tx2"), _T(""), 2000, false, true);

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(expr.get());
		ctxt.m_pRenameMoveDetection->Detect(ctxt, true);

		const auto& groups = ctxt.m_pRenameMoveDetection->GetRenameMoveItemGroups();
		EXPECT_EQ(groups.size(), 2);

		// Verify group contents
		EXPECT_EQ(groups[pdi1->renameMoveGroupId].size(), 2);
		EXPECT_EQ(groups[pdi3->renameMoveGroupId].size(), 2);
	}

	/**
	 * @brief Test 3-way with one side having no items
	 */
	TEST_F(RenameMoveDetectionTest, ThreeWay_OneSideEmpty)
	{
		PathContext paths(_T("C:\\Left"), _T("C:\\Middle"), _T("C:\\Right"));
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();

		auto expr = std::make_unique<FilterExpression>();
		expr->Parse("BaseName");
		expr->SetDiffContext(&ctxt);

		// File only in Left and Right (Middle empty)
		auto* pdi1 = CreateMockDiffItem3Way(ctxt, nullptr, _T("file.tx1"), _T(""), 1000, true, false, false);
		auto* pdi2 = CreateMockDiffItem3Way(ctxt, nullptr, _T("file.tx2"), _T(""), 1000, false, false, true);

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(expr.get());
		ctxt.m_pRenameMoveDetection->Detect(ctxt, true);

		// Should be grouped (2 sides is enough)
		EXPECT_NE(pdi1->renameMoveGroupId, -1);
		EXPECT_EQ(pdi1->renameMoveGroupId, pdi2->renameMoveGroupId);

		// GetRenameMoveGroupItemsForSide for middle should return empty
		auto middleItems = ctxt.m_pRenameMoveDetection->GetRenameMoveGroupItemsForSide(*pdi1, 1);
		EXPECT_TRUE(middleItems.empty());
	}

	/**
	 * @brief Test detection with no items
	 */
	TEST_F(RenameMoveDetectionTest, NoItems)
	{
		PathContext paths(_T("C:\\Left"), _T("C:\\Right"));
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();

		auto expr = std::make_unique<FilterExpression>();
		expr->Parse("Name");
		expr->SetDiffContext(&ctxt);

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(expr.get());

		EXPECT_NO_THROW(ctxt.m_pRenameMoveDetection->Detect(ctxt, true));
		EXPECT_NO_THROW(ctxt.m_pRenameMoveDetection->Merge(ctxt));

		EXPECT_EQ(ctxt.m_pRenameMoveDetection->GetRenameMoveItemGroups().size(), 0);
	}

	/**
	 * @brief Test merge does nothing when no groups exist
	 */
	TEST_F(RenameMoveDetectionTest, MergeWithNoGroups)
	{
		PathContext paths(_T("C:\\Left"), _T("C:\\Right"));
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();
		CompareStats compareStats(2);
		ctxt.m_pCompareStats = &compareStats;

		CreateMockDiffItem2Way(ctxt, nullptr, _T("file1.txt"), _T(""), 1000, true, false);
		CreateMockDiffItem2Way(ctxt, nullptr, _T("file2.txt"), _T(""), 2000, false, true);

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();

		EXPECT_NO_THROW(ctxt.m_pRenameMoveDetection->Merge(ctxt));
	}

	/**
	 * @brief Test single item per side (no match possible)
	 */
	TEST_F(RenameMoveDetectionTest, SingleItemPerSide_NoMatch)
	{
		PathContext paths(_T("C:\\Left"), _T("C:\\Right"));
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();

		auto expr = std::make_unique<FilterExpression>();
		expr->Parse("Name");
		expr->SetDiffContext(&ctxt);

		// Different names
		auto* pdi1 = CreateMockDiffItem2Way(ctxt, nullptr, _T("file1.txt"), _T(""), 1000, true, false);
		auto* pdi2 = CreateMockDiffItem2Way(ctxt, nullptr, _T("file2.txt"), _T(""), 1000, false, true);

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(expr.get());
		ctxt.m_pRenameMoveDetection->Detect(ctxt, true);

		EXPECT_EQ(pdi1->renameMoveGroupId, -1);
		EXPECT_EQ(pdi2->renameMoveGroupId, -1);
		EXPECT_EQ(ctxt.m_pRenameMoveDetection->GetRenameMoveItemGroups().size(), 0);
	}

	/**
	 * @brief Test 3-way CheckMovedOrRenamed
	 */
	TEST_F(RenameMoveDetectionTest, CheckMovedOrRenamed_3Way_Renamed)
	{
		PathContext paths(_T("C:\\Left"), _T("C:\\Middle"), _T("C:\\Right"));
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();

		auto expr = std::make_unique<FilterExpression>();
		expr->Parse("Size");
		expr->SetDiffContext(&ctxt);

		auto* pdi1 = CreateMockDiffItem3Way(ctxt, nullptr, _T("file1.txt"), _T(""), 1000, true, false, false);
		CreateMockDiffItem3Way(ctxt, nullptr, _T("file2.txt"), _T(""), 1000, false, true, false);
		CreateMockDiffItem3Way(ctxt, nullptr, _T("file3.txt"), _T(""), 1000, false, false, true);

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(expr.get());
		ctxt.m_pRenameMoveDetection->Detect(ctxt, false);

		bool moved = false, renamed = false;
		ctxt.m_pRenameMoveDetection->CheckMovedOrRenamed(ctxt, *pdi1, moved, renamed);

		EXPECT_FALSE(moved);
		EXPECT_TRUE(renamed);  // All have different names
	}

	/**
	 * @brief Test 3-way with mixed moves and renames
	 */
	TEST_F(RenameMoveDetectionTest, ThreeWay_MixedMovesAndRenames)
	{
		PathContext paths(_T("C:\\Left"), _T("C:\\Middle"), _T("C:\\Right"));
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();

		auto expr = std::make_unique<FilterExpression>();
		expr->Parse("Size");
		expr->SetDiffContext(&ctxt);

		auto* parent1 = CreateMockDiffItem3Way(ctxt, nullptr, _T("dir1"), _T(""), 0, true, false, false, true);
		auto* parent2 = CreateMockDiffItem3Way(ctxt, nullptr, _T("dir2"), _T(""), 0, false, true, false, true);
		auto* parent3 = CreateMockDiffItem3Way(ctxt, nullptr, _T("dir3"), _T(""), 0, false, false, true, true);

		auto* pdi1 = CreateMockDiffItem3Way(ctxt, parent1, _T("file1.txt"), _T("dir1"), 1000, true, false, false);
		CreateMockDiffItem3Way(ctxt, parent2, _T("file2.txt"), _T("dir2"), 1000, false, true, false);
		CreateMockDiffItem3Way(ctxt, parent3, _T("file3.txt"), _T("dir3"), 1000, false, false, true);

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(expr.get());
		ctxt.m_pRenameMoveDetection->Detect(ctxt, true);

		bool moved = false, renamed = false;
		ctxt.m_pRenameMoveDetection->CheckMovedOrRenamed(ctxt, *pdi1, moved, renamed);

		EXPECT_TRUE(moved);    // Different directories
		EXPECT_TRUE(renamed);  // Different names
	}

	/**
	 * @brief Test that already grouped items are skipped
	 */
	TEST_F(RenameMoveDetectionTest, AlreadyGroupedItemsSkipped)
	{
		PathContext paths(_T("C:\\Left"), _T("C:\\Right"));
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();

		auto expr = std::make_unique<FilterExpression>();
		expr->Parse("BaseName");
		expr->SetDiffContext(&ctxt);

		auto* pdi1 = CreateMockDiffItem2Way(ctxt, nullptr, _T("file.tx1"), _T(""), 1000, true, false);
		auto* pdi2 = CreateMockDiffItem2Way(ctxt, nullptr, _T("file.tx2"), _T(""), 1000, false, true);

		// Pre-assign to group
		pdi1->renameMoveGroupId = 0;
		pdi2->renameMoveGroupId = 0;

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(expr.get());
		ctxt.m_pRenameMoveDetection->Detect(ctxt, true);

		// Should remain in original group
		EXPECT_EQ(pdi1->renameMoveGroupId, 0);
		EXPECT_EQ(pdi2->renameMoveGroupId, 0);
	}

	/**
	 * @brief Test large number of items
	 */
	TEST_F(RenameMoveDetectionTest, LargeNumberOfItems)
	{
		PathContext paths(_T("C:\\Left"), _T("C:\\Right"));
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();

		auto expr = std::make_unique<FilterExpression>();
		expr->Parse("BaseName");
		expr->SetDiffContext(&ctxt);

		// Create 100 pairs of files
		for (int i = 0; i < 100; ++i)
		{
			String filenameL = strutils::format(_T("file%d.tx1"), i);
			String filenameR = strutils::format(_T("file%d.tx2"), i);
			CreateMockDiffItem2Way(ctxt, nullptr, filenameL, _T(""), 1000, true, false);
			CreateMockDiffItem2Way(ctxt, nullptr, filenameR, _T(""), 1000, false, true);
		}

		ctxt.m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();
		ctxt.m_pRenameMoveDetection->SetRenameMoveKeyExpression(expr.get());

		EXPECT_NO_THROW(ctxt.m_pRenameMoveDetection->Detect(ctxt, true));
		EXPECT_EQ(ctxt.m_pRenameMoveDetection->GetRenameMoveItemGroups().size(), 100);
	}

}  // namespace