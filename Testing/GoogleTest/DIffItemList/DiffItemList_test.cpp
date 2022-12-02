#include "pch.h"
#include <gtest/gtest.h>
#include "DiffItemList.h"
#include "paths.h"

namespace
{
	// The fixture for testing paths functions.
	class DiffItemListTest : public testing::Test
	{
	protected:
		// You can remove any or all of the following functions if its body
		// is	empty.

		DiffItemListTest()
		{
			// You can do set-up work for each test	here.
		}

		virtual ~DiffItemListTest()
		{
			// You can do clean-up work	that doesn't throw exceptions here.
		}

		// If	the	constructor	and	destructor are not enough for setting up
		// and cleaning up each test, you can define the following methods:

		virtual void SetUp()
		{
			// Code	here will be called	immediately	after the constructor (right
			// before each test).
		}

		virtual void TearDown()
		{
			// Code	here will be called	immediately	after each test	(right
			// before the destructor).
		}

		// Objects declared here can be used by all tests in the test case for Foo.
	};

	static void SetFile(DIFFITEM& di, const String& file)
	{
		for (int i = 0; i < 2; i++)
		{
			di.diffcode.setSideFlag(i);
			di.diffFileInfo[i].path = paths::GetPathOnly(file);
			di.diffFileInfo[i].filename = paths::FindFileName(file);
		}
	}

	TEST_F(DiffItemListTest, AddNewDiff)
	{
		DiffItemList list;
		list.InitDiffItemList();
		DIFFITEM *pDir1 = list.AddNewDiff(nullptr);
		DIFFITEM *pDir2 = list.AddNewDiff(nullptr);
		DIFFITEM *pFile1 = list.AddNewDiff(pDir1);
		DIFFITEM *pFile2 = list.AddNewDiff(pDir1);
		DIFFITEM *pFile3 = list.AddNewDiff(pDir2);
		DIFFITEM *pFile4 = list.AddNewDiff(pDir2);
		SetFile(*pDir1, _T("Dir1"));
		SetFile(*pDir2, _T("Dir2"));
		SetFile(*pFile1, _T("Dir1\\File1"));
		SetFile(*pFile2, _T("Dir1\\File2"));
		SetFile(*pFile3, _T("Dir2\\File3"));
		SetFile(*pFile4, _T("Dir2\\File4"));

		DIFFITEM *pdi = list.GetFirstDiffPosition();
		EXPECT_EQ(String(_T("Dir1")), pdi->diffFileInfo[0].GetFile());
		list.GetNextSiblingDiffPosition(pdi);
		EXPECT_EQ(String(_T("Dir2")), pdi->diffFileInfo[0].GetFile());
		pdi = list.GetFirstChildDiffPosition(pdi);
		EXPECT_EQ(String(_T("Dir2\\File3")), pdi->diffFileInfo[0].GetFile());
		list.GetNextSiblingDiffPosition(pdi);
		EXPECT_EQ(String(_T("Dir2\\File4")), pdi->diffFileInfo[0].GetFile());

		pdi = list.GetFirstChildDiffPosition(list.GetFirstDiffPosition());
		EXPECT_EQ(String(_T("Dir1\\File1")), pdi->diffFileInfo[0].GetFile());
		list.GetNextSiblingDiffPosition(pdi);
		EXPECT_EQ(String(_T("Dir1\\File2")), pdi->diffFileInfo[0].GetFile());
	}


}  // namespace
