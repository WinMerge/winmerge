#include "pch.h"
#include <gtest/gtest.h>
#include <Poco/Timestamp.h>
#include "DiffContext.h"
#include "PathContext.h"
#include "DiffItem.h"
#include "DiffWrapper.h"
#include "CompareEngines/ExistenceCompare.h"

namespace
{
	class ExistenceCompareTest : public testing::Test
	{
	protected:
		ExistenceCompareTest() : m_pCtxt2(nullptr), m_pCtxt3(nullptr) {}
		virtual ~ExistenceCompareTest() {}

		virtual void SetUp()
		{
			PathContext paths2;
			paths2.SetLeft(_T(""));
			paths2.SetRight(_T(""));
			m_pCtxt2.reset(new CDiffContext(paths2, CMP_EXISTENCE));
			m_pCompare2.reset(new CompareEngines::ExistenceCompare(*m_pCtxt2));

			PathContext paths3;
			paths3.SetLeft(_T(""));
			paths3.SetMiddle(_T(""));
			paths3.SetRight(_T(""));
			m_pCtxt3.reset(new CDiffContext(paths3, CMP_EXISTENCE));
			m_pCompare3.reset(new CompareEngines::ExistenceCompare(*m_pCtxt3));
		}

		virtual void TearDown()
		{
			m_pCompare2.reset();
			m_pCompare3.reset();
			m_pCtxt2.reset();
			m_pCtxt3.reset();
		}

		std::unique_ptr<CDiffContext> m_pCtxt2;
		std::unique_ptr<CDiffContext> m_pCtxt3;
		std::unique_ptr<CompareEngines::ExistenceCompare> m_pCompare2;
		std::unique_ptr<CompareEngines::ExistenceCompare> m_pCompare3;
	};

	TEST_F(ExistenceCompareTest, Identical)
	{
		DIFFITEM di;

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime + 10000000;
		di.diffFileInfo[2].mtime = di.diffFileInfo[0].mtime + 20000000;

		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 2;
		EXPECT_EQ(int(DIFFCODE::SAME), m_pCompare2->CompareFiles(di));

		di.diffcode.setSideFlag(2);
		di.diffcode.diffcode |= DIFFCODE::THREEWAY;

		di.diffFileInfo[0].size = 0;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 2;
		EXPECT_EQ(int(DIFFCODE::SAME), m_pCompare3->CompareFiles(di));
	}

	TEST_F(ExistenceCompareTest, UniqueFile)
	{
		DIFFITEM di;

		di.diffFileInfo[0].mtime = Poco::Timestamp();
		di.diffFileInfo[1].mtime = di.diffFileInfo[0].mtime + 10000000;
		di.diffFileInfo[2].mtime = di.diffFileInfo[0].mtime + 20000000;

		di.diffcode.setSideNone();
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		EXPECT_EQ(int(DIFFCODE::SAME), m_pCompare2->CompareFiles(di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		EXPECT_EQ(int(DIFFCODE::DIFF), m_pCompare2->CompareFiles(di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = 1;
		EXPECT_EQ(int(DIFFCODE::DIFF), m_pCompare2->CompareFiles(di));

		di.diffcode.diffcode |= DIFFCODE::THREEWAY;

		di.diffcode.setSideNone();
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[2].size = DirItem::FILE_SIZE_NONE;
		EXPECT_EQ(int(DIFFCODE::SAME), m_pCompare3->CompareFiles(di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 2;
		EXPECT_EQ(int(DIFFCODE::DIFF | DIFFCODE::DIFF1STONLY), m_pCompare3->CompareFiles(di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[2].size = 2;
		EXPECT_EQ(int(DIFFCODE::DIFF | DIFFCODE::DIFF2NDONLY), m_pCompare3->CompareFiles(di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 2;
		di.diffFileInfo[2].size = DirItem::FILE_SIZE_NONE;
		EXPECT_EQ(int(DIFFCODE::DIFF | DIFFCODE::DIFF3RDONLY), m_pCompare3->CompareFiles(di));


		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(0);
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[2].size = DirItem::FILE_SIZE_NONE;
		EXPECT_EQ(int(DIFFCODE::DIFF | DIFFCODE::DIFF1STONLY), m_pCompare3->CompareFiles(di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = DirItem::FILE_SIZE_NONE;
		EXPECT_EQ(int(DIFFCODE::DIFF | DIFFCODE::DIFF2NDONLY), m_pCompare3->CompareFiles(di));

		di.diffcode.setSideNone();
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[2].size = 1;
		EXPECT_EQ(int(DIFFCODE::DIFF | DIFFCODE::DIFF3RDONLY), m_pCompare3->CompareFiles(di));
	}

}  // namespace
