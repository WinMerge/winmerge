#include "pch.h"
#include <gtest/gtest.h>
#include "MoveDetection.h"
#include "DiffContext.h"
#include "DiffItem.h"
#include "FilterEngine/FilterExpression.h"

namespace
{
	/**
	 * @brief Test fixture for MoveDetection
	 */
	class MoveDetectionTest : public testing::Test
	{
	protected:
		MoveDetectionTest() {}
		virtual ~MoveDetectionTest() {}

		/**
		 * @brief Create a mock DIFFITEM for testing
		 */
		void CreateMockDiffItem(DIFFITEM& di, const String& filename, bool existsLeft, bool existsRight, bool isDirectory = false)
		{
			di.movedGroupId = -1;
			
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
	TEST_F(MoveDetectionTest, NoExpressionSet)
	{
		PathContext paths;
		paths.SetLeft(_T("C:\\Left"));
		paths.SetRight(_T("C:\\Right"));
		
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();
		ctxt.m_pMoveDetection->SetMoveDetectionExpression(nullptr);
		
		// Should not crash and should return immediately
		EXPECT_NO_THROW(ctxt.m_pMoveDetection->Detect(ctxt));
	}

	/**
	 * @brief Test basic move detection with simple expression
	 */
	TEST_F(MoveDetectionTest, BasicMoveDetection)
	{
		PathContext paths;
		paths.SetLeft(_T("C:\\Left"));
		paths.SetRight(_T("C:\\Right"));
		
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();
		
		// Create a simple expression that matches files with same name
		auto pMoveDetectionExpression = std::make_unique<FilterExpression>();
		pMoveDetectionExpression->Parse("LeftName = RightName");
		
		// Add mock items
		DIFFITEM* pdi1 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem(*pdi1, _T("file1.txt"), true, false);
		
		DIFFITEM* pdi2 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem(*pdi2, _T("file1.txt"), false, true);
		
		// Run detection
		ctxt.m_pMoveDetection->SetMoveDetectionExpression(pMoveDetectionExpression.get());
		ctxt.m_pMoveDetection->Detect(ctxt);
		
		// Verify that items were grouped
		EXPECT_NE(pdi1->movedGroupId, -1);
		EXPECT_EQ(pdi1->movedGroupId, pdi2->movedGroupId);
		EXPECT_EQ(ctxt.m_pMoveDetection->GetMovedItems()->size(), 1u);
	}

	/**
	 * @brief Test that files with different names are not grouped
	 */
	TEST_F(MoveDetectionTest, DifferentNamesNotGrouped)
	{
		PathContext paths;
		paths.SetLeft(_T("C:\\Left"));
		paths.SetRight(_T("C:\\Right"));
		
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();
		
		auto pMoveDetectionExpression = std::make_unique<FilterExpression>();
		pMoveDetectionExpression->Parse("LeftName = RightName");
		
		DIFFITEM* pdi1 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem(*pdi1, _T("file1.txt"), true, false);
		
		DIFFITEM* pdi2 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem(*pdi2, _T("file2.txt"), false, true);
		
		ctxt.m_pMoveDetection->SetMoveDetectionExpression(pMoveDetectionExpression.get());
		ctxt.m_pMoveDetection->Detect(ctxt);
		
		// Items should not be grouped
		EXPECT_EQ(pdi1->movedGroupId, -1);
		EXPECT_EQ(pdi2->movedGroupId, -1);
		EXPECT_EQ(ctxt.m_pMoveDetection->GetMovedItems()->size(), 0u);
	}

	/**
	 * @brief Test that directories and files are not mixed
	 */
	TEST_F(MoveDetectionTest, DirectoriesAndFilesNotMixed)
	{
		PathContext paths;
		paths.SetLeft(_T("C:\\Left"));
		paths.SetRight(_T("C:\\Right"));
		
		CDiffContext ctxt(paths, 0);
		ctxt.InitDiffItemList();
		
		auto pMoveDetectionExpression = std::make_unique<FilterExpression>();
		pMoveDetectionExpression->Parse("LeftName = RightName");
		
		// File on left
		DIFFITEM* pdi1 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem(*pdi1, _T("item1"), true, false, false);
		
		// Directory on right with same name
		DIFFITEM* pdi2 = ctxt.AddNewDiff(nullptr);
		CreateMockDiffItem(*pdi2, _T("item1"), false, true, true);
		
		ctxt.m_pMoveDetection->SetMoveDetectionExpression(pMoveDetectionExpression.get());
		ctxt.m_pMoveDetection->Detect(ctxt);
		
		// Should not be grouped
		EXPECT_EQ(pdi1->movedGroupId, -1);
		EXPECT_EQ(pdi2->movedGroupId, -1);
	}

}  // namespace
