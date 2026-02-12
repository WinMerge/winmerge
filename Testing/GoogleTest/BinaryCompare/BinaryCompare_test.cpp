#include "pch.h"
#include <gtest/gtest.h>
#include "DiffContext.h"
#include "PathContext.h"
#include "CompareEngines/BinaryCompare.h"
#include "DiffWrapper.h"
#include <fstream>

namespace
{
	struct TempFile
	{
		TempFile(const std::string& filename, const char *data, size_t len) : m_filename(filename)
		{
			std::ofstream ostr(filename.c_str(), std::ios::out|std::ios::binary|std::ios::trunc);
			ostr.write(data, len);
		}
		~TempFile()
		{
			remove(m_filename.c_str());
		}
		std::string m_filename;
	};

	// The fixture for testing paths functions.
	class BinaryCompareTest : public testing::Test
	{
	protected:
		// You can remove any or all of the following functions if its body
		// is	empty.

		BinaryCompareTest()
		{
			// You can do set-up work for each test	here.
		}

		virtual ~BinaryCompareTest()
		{
			// You can do clean-up work	that doesn't throw exceptions here.
		}

		// If	the	constructor	and	destructor are not enough for setting up
		// and cleaning up each test, you can define the following methods:

		virtual void SetUp()
		{
			// before each test).
		}

		virtual void TearDown()
		{
			// Code	here will be called	immediately	after each test	(right
			// before the destructor).
		}

		// Objects declared here can be used by all tests in the test case for Foo.
	};

	TEST_F(BinaryCompareTest, SameSize)
	{
		PathContext paths;
		paths.SetLeft(_T("."));
		paths.SetRight(_T("."));
		CDiffContext ctxt(paths, CMP_BINARY_CONTENT);
		CompareEngines::BinaryCompare bc(ctxt);
		PathContext paths3;
		paths3.SetLeft(_T("."));
		paths3.SetMiddle(_T("."));
		paths3.SetRight(_T("."));
		CDiffContext ctxt3(paths3, CMP_BINARY_CONTENT);
		CompareEngines::BinaryCompare bc3(ctxt3);
		DIFFITEM di;

		{
			TempFile l1("A", "1", 1);
			TempFile r1("B", "1", 1);
			di.diffcode.setSideFlag(0);
			di.diffcode.setSideFlag(1);
			di.diffFileInfo[0].SetFile(_T("A"));
			di.diffFileInfo[1].SetFile(_T("B"));
			di.diffFileInfo[0].size = 1;
			di.diffFileInfo[1].size = 1;
			EXPECT_EQ(int(DIFFCODE::SAME), bc.CompareFiles(di));
		}

		{
			TempFile l1("A", "1", 1);
			TempFile r1("B", "2", 1);
			di.diffcode.setSideFlag(0);
			di.diffcode.setSideFlag(1);
			di.diffFileInfo[0].SetFile(_T("A"));
			di.diffFileInfo[1].SetFile(_T("B"));
			di.diffFileInfo[0].size = 1;
			di.diffFileInfo[1].size = 1;
			EXPECT_EQ(int(DIFFCODE::DIFF), bc.CompareFiles(di));
		}

		{
			TempFile l1("A", "1", 1);
			TempFile m1("B", "1", 1);
			TempFile r1("C", "1", 1);
			di.diffcode.setSideFlag(0);
			di.diffcode.setSideFlag(1);
			di.diffcode.setSideFlag(2);
			di.diffFileInfo[0].SetFile(_T("A"));
			di.diffFileInfo[1].SetFile(_T("B"));
			di.diffFileInfo[2].SetFile(_T("C"));
			di.diffFileInfo[0].size = 1;
			di.diffFileInfo[1].size = 1;
			di.diffFileInfo[2].size = 1;
			EXPECT_EQ(int(DIFFCODE::SAME), bc3.CompareFiles(di));
		}

		{
			TempFile l1("A", "1", 1);
			TempFile m1("B", "1", 1);
			TempFile r1("C", "2", 1);
			di.diffcode.setSideFlag(0);
			di.diffcode.setSideFlag(1);
			di.diffcode.setSideFlag(2);
			di.diffFileInfo[0].SetFile(_T("A"));
			di.diffFileInfo[1].SetFile(_T("B"));
			di.diffFileInfo[2].SetFile(_T("C"));
			di.diffFileInfo[0].size = 1;
			di.diffFileInfo[1].size = 1;
			di.diffFileInfo[2].size = 1;
			EXPECT_EQ(int(DIFFCODE::DIFF | DIFFCODE::DIFF3RDONLY), bc3.CompareFiles(di));
		}

		{
			TempFile l1("A", "1", 1);
			TempFile m1("B", "2", 1);
			TempFile r1("C", "1", 1);
			di.diffcode.setSideFlag(0);
			di.diffcode.setSideFlag(1);
			di.diffcode.setSideFlag(2);
			di.diffFileInfo[0].SetFile(_T("A"));
			di.diffFileInfo[1].SetFile(_T("B"));
			di.diffFileInfo[2].SetFile(_T("C"));
			di.diffFileInfo[0].size = 1;
			di.diffFileInfo[1].size = 1;
			di.diffFileInfo[2].size = 1;
			EXPECT_EQ(int(DIFFCODE::DIFF | DIFFCODE::DIFF2NDONLY), bc3.CompareFiles(di));
		}

		{
			TempFile l1("A", "2", 1);
			TempFile m1("B", "1", 1);
			TempFile r1("C", "1", 1);
			di.diffcode.setSideFlag(0);
			di.diffcode.setSideFlag(1);
			di.diffcode.setSideFlag(2);
			di.diffFileInfo[0].SetFile(_T("A"));
			di.diffFileInfo[1].SetFile(_T("B"));
			di.diffFileInfo[2].SetFile(_T("C"));
			di.diffFileInfo[0].size = 1;
			di.diffFileInfo[1].size = 1;
			di.diffFileInfo[2].size = 1;
			EXPECT_EQ(int(DIFFCODE::DIFF | DIFFCODE::DIFF1STONLY), bc3.CompareFiles(di));
		}

		{
			TempFile l1("A", "1", 1);
			TempFile m1("B", "2", 1);
			TempFile r1("C", "3", 1);
			di.diffcode.setSideFlag(0);
			di.diffcode.setSideFlag(1);
			di.diffcode.setSideFlag(2);
			di.diffFileInfo[0].SetFile(_T("A"));
			di.diffFileInfo[1].SetFile(_T("B"));
			di.diffFileInfo[2].SetFile(_T("C"));
			di.diffFileInfo[0].size = 1;
			di.diffFileInfo[1].size = 1;
			di.diffFileInfo[2].size = 1;
			EXPECT_EQ(int(DIFFCODE::DIFF), bc3.CompareFiles(di));
		}
	}

	TEST_F(BinaryCompareTest, DifferentSize)
	{
		PathContext paths;
		paths.SetLeft(_T("."));
		paths.SetRight(_T("."));
		CDiffContext ctxt(paths, CMP_BINARY_CONTENT);
		CompareEngines::BinaryCompare bc(ctxt);

		PathContext paths3;
		paths3.SetLeft(_T("."));
		paths3.SetMiddle(_T("."));
		paths3.SetRight(_T("."));
		CDiffContext ctxt3(paths3, CMP_BINARY_CONTENT);
		CompareEngines::BinaryCompare bc3(ctxt3);

		DIFFITEM di;

		TempFile l1("A", "1", 1);
		TempFile m1("B", "1", 1);
		TempFile r1("C", "1", 1);
		TempFile l2("Ae", "", 0);
		TempFile m2("Be", "", 0);
		TempFile r2("Ce", "", 0);

		di.diffcode.setSideFlag(0);
		di.diffcode.unsetSideFlag(1);
		di.diffFileInfo[0].SetFile(_T("A"));
		di.diffFileInfo[1].SetFile(_T("B"));
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		EXPECT_EQ(int(DIFFCODE::DIFF), bc.CompareFiles(di));

		di.diffcode.unsetSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].SetFile(_T("A"));
		di.diffFileInfo[1].SetFile(_T("B"));
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = 1;
		EXPECT_EQ(int(DIFFCODE::DIFF), bc.CompareFiles(di));

		di.diffcode.setSideFlag(0);
		di.diffcode.unsetSideFlag(1);
		di.diffFileInfo[0].SetFile(_T("Ae"));
		di.diffFileInfo[1].SetFile(_T("NUL"));
		di.diffFileInfo[0].size = 0;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		EXPECT_EQ(int(DIFFCODE::DIFF), bc.CompareFiles(di));

		di.diffcode.unsetSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].SetFile(_T("NUL"));
		di.diffFileInfo[1].SetFile(_T("Be"));
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = 0;
		EXPECT_EQ(int(DIFFCODE::DIFF), bc.CompareFiles(di));

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].SetFile(_T("A"));
		di.diffFileInfo[1].SetFile(_T("B"));
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 2;
		EXPECT_EQ(int(DIFFCODE::DIFF), bc.CompareFiles(di));

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].SetFile(_T("A"));
		di.diffFileInfo[1].SetFile(_T("B"));
		di.diffFileInfo[2].SetFile(_T("C"));
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 2;
		EXPECT_EQ(int(DIFFCODE::DIFF | DIFFCODE::DIFF3RDONLY), bc3.CompareFiles(di));

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].SetFile(_T("A"));
		di.diffFileInfo[1].SetFile(_T("B"));
		di.diffFileInfo[2].SetFile(_T("C"));
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 2;
		di.diffFileInfo[2].size = 1;
		EXPECT_EQ(int(DIFFCODE::DIFF | DIFFCODE::DIFF2NDONLY), bc3.CompareFiles(di));

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].SetFile(_T("A"));
		di.diffFileInfo[1].SetFile(_T("B"));
		di.diffFileInfo[2].SetFile(_T("C"));
		di.diffFileInfo[0].size = 2;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 1;
		EXPECT_EQ(int(DIFFCODE::DIFF | DIFFCODE::DIFF1STONLY), bc3.CompareFiles(di));

		di.diffcode.unsetSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].SetFile(_T("NUL"));
		di.diffFileInfo[1].SetFile(_T("Be"));
		di.diffFileInfo[2].SetFile(_T("Ce"));
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = 0;
		di.diffFileInfo[2].size = 0;
		EXPECT_EQ(int(DIFFCODE::DIFF | DIFFCODE::DIFF1STONLY), bc3.CompareFiles(di));

		di.diffcode.setSideFlag(0);
		di.diffcode.unsetSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].SetFile(_T("Ae"));
		di.diffFileInfo[1].SetFile(_T("NUL"));
		di.diffFileInfo[2].SetFile(_T("Ce"));
		di.diffFileInfo[0].size = 0;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[2].size = 0;
		EXPECT_EQ(int(DIFFCODE::DIFF | DIFFCODE::DIFF2NDONLY), bc3.CompareFiles(di));

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffcode.unsetSideFlag(2);
		di.diffFileInfo[0].SetFile(_T("Ae"));
		di.diffFileInfo[1].SetFile(_T("Be"));
		di.diffFileInfo[2].SetFile(_T("NUL"));
		di.diffFileInfo[0].size = 0;
		di.diffFileInfo[1].size = 0;
		di.diffFileInfo[2].size = DirItem::FILE_SIZE_NONE;
		EXPECT_EQ(int(DIFFCODE::DIFF | DIFFCODE::DIFF3RDONLY), bc3.CompareFiles(di));

		di.diffcode.setSideFlag(0);
		di.diffcode.unsetSideFlag(1);
		di.diffcode.unsetSideFlag(2);
		di.diffFileInfo[0].SetFile(_T("Ae"));
		di.diffFileInfo[1].SetFile(_T("NUL"));
		di.diffFileInfo[2].SetFile(_T("NUL"));
		di.diffFileInfo[0].size = 0;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[2].size = DirItem::FILE_SIZE_NONE;
		EXPECT_EQ(int(DIFFCODE::DIFF | DIFFCODE::DIFF1STONLY), bc3.CompareFiles(di));

		di.diffcode.unsetSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffcode.unsetSideFlag(2);
		di.diffFileInfo[0].SetFile(_T("NUL"));
		di.diffFileInfo[1].SetFile(_T("Be"));
		di.diffFileInfo[2].SetFile(_T("NUL"));
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = 0;
		di.diffFileInfo[2].size = DirItem::FILE_SIZE_NONE;
		EXPECT_EQ(int(DIFFCODE::DIFF | DIFFCODE::DIFF2NDONLY), bc3.CompareFiles(di));

		di.diffcode.unsetSideFlag(0);
		di.diffcode.unsetSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].SetFile(_T("NUL"));
		di.diffFileInfo[1].SetFile(_T("NUL"));
		di.diffFileInfo[2].SetFile(_T("Ce"));
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[2].size = 0;
		EXPECT_EQ(int(DIFFCODE::DIFF | DIFFCODE::DIFF3RDONLY), bc3.CompareFiles(di));

	}

	TEST_F(BinaryCompareTest, Error)
	{
		PathContext paths;
		paths.SetLeft(_T("."));
		paths.SetRight(_T("."));
		CDiffContext ctxt(paths, CMP_BINARY_CONTENT);
		CompareEngines::BinaryCompare bc(ctxt);

		PathContext paths3;
		paths3.SetLeft(_T("."));
		paths3.SetMiddle(_T("."));
		paths3.SetRight(_T("."));
		CDiffContext ctxt3(paths3, CMP_BINARY_CONTENT);
		CompareEngines::BinaryCompare bc3(ctxt3);

		DIFFITEM di;

		TempFile l1("A", "1", 1);
		TempFile r1("B", "1", 1);

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].SetFile(_T("/1>"));
		di.diffFileInfo[1].SetFile(_T("B"));
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 1;
		EXPECT_EQ(int(DIFFCODE::CMPERR), bc.CompareFiles(di));

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffFileInfo[0].SetFile(_T("A"));
		di.diffFileInfo[1].SetFile(_T("/2>"));
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 1;
		EXPECT_EQ(int(DIFFCODE::CMPERR), bc.CompareFiles(di));

		di.diffcode.setSideFlag(0);
		di.diffcode.setSideFlag(1);
		di.diffcode.setSideFlag(2);
		di.diffFileInfo[0].SetFile(_T("A"));
		di.diffFileInfo[1].SetFile(_T("B"));
		di.diffFileInfo[2].SetFile(_T("/3>"));
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 1;
		EXPECT_EQ(int(DIFFCODE::CMPERR), bc3.CompareFiles(di));
	}

}  // namespace
