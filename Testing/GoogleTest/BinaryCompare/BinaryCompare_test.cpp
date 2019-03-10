#include "pch.h"
#include <gtest/gtest.h>
#include "DiffContext.h"
#include "PathContext.h"
#include "CompareEngines/BinaryCompare.h"
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
		CompareEngines::BinaryCompare bc;
		DIFFITEM di;
		PathContext files;

		{
			TempFile l1("A", "1", 1);
			TempFile r1("B", "1", 1);
			files.SetLeft(_T("A"));
			files.SetRight(_T("B"));
			di.diffFileInfo[0].size = 1;
			di.diffFileInfo[1].size = 1;
			EXPECT_EQ(DIFFCODE::SAME, bc.CompareFiles(files, di));
		}
		
		{
			TempFile l1("A", "1", 1);
			TempFile r1("B", "2", 1);
			files.SetLeft(_T("A"));
			files.SetRight(_T("B"));
			di.diffFileInfo[0].size = 1;
			di.diffFileInfo[1].size = 1;
			EXPECT_EQ(DIFFCODE::DIFF, bc.CompareFiles(files, di));
		}

		{
			TempFile l1("A", "1", 1);
			TempFile m1("B", "1", 1);
			TempFile r1("C", "1", 1);
			files.SetLeft(_T("A"));
			files.SetMiddle(_T("B"));
			files.SetRight(_T("C"));
			di.diffFileInfo[0].size = 1;
			di.diffFileInfo[1].size = 1;
			di.diffFileInfo[2].size = 1;
			EXPECT_EQ(DIFFCODE::SAME, bc.CompareFiles(files, di));
		}
		
		{
			TempFile l1("A", "1", 1);
			TempFile m1("B", "1", 1);
			TempFile r1("C", "2", 1);
			files.SetLeft(_T("A"));
			files.SetMiddle(_T("B"));
			files.SetRight(_T("C"));
			di.diffFileInfo[0].size = 1;
			di.diffFileInfo[1].size = 1;
			di.diffFileInfo[2].size = 1;
			EXPECT_EQ(DIFFCODE::DIFF | DIFFCODE::DIFF3RDONLY, bc.CompareFiles(files, di));
		}

		{
			TempFile l1("A", "1", 1);
			TempFile m1("B", "2", 1);
			TempFile r1("C", "1", 1);
			files.SetLeft(_T("A"));
			files.SetMiddle(_T("B"));
			files.SetRight(_T("C"));
			di.diffFileInfo[0].size = 1;
			di.diffFileInfo[1].size = 1;
			di.diffFileInfo[2].size = 1;
			EXPECT_EQ(DIFFCODE::DIFF | DIFFCODE::DIFF2NDONLY, bc.CompareFiles(files, di));
		}

		{
			TempFile l1("A", "2", 1);
			TempFile m1("B", "1", 1);
			TempFile r1("C", "1", 1);
			files.SetLeft(_T("A"));
			files.SetMiddle(_T("B"));
			files.SetRight(_T("C"));
			di.diffFileInfo[0].size = 1;
			di.diffFileInfo[1].size = 1;
			di.diffFileInfo[2].size = 1;
			EXPECT_EQ(DIFFCODE::DIFF | DIFFCODE::DIFF1STONLY, bc.CompareFiles(files, di));
		}

		{
			TempFile l1("A", "1", 1);
			TempFile m1("B", "2", 1);
			TempFile r1("C", "3", 1);
			files.SetLeft(_T("A"));
			files.SetMiddle(_T("B"));
			files.SetRight(_T("C"));
			di.diffFileInfo[0].size = 1;
			di.diffFileInfo[1].size = 1;
			di.diffFileInfo[2].size = 1;
			EXPECT_EQ(DIFFCODE::DIFF, bc.CompareFiles(files, di));
		}
	}

	TEST_F(BinaryCompareTest, DifferentSize)
	{
		CompareEngines::BinaryCompare bc;
		PathContext files;
		DIFFITEM di;

		TempFile l1("A", "1", 1);
		TempFile m1("B", "1", 1);
		TempFile r1("C", "1", 1);

		files.SetLeft(_T("A"));
		files.SetRight(_T("B"));
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = DirItem::FILE_SIZE_NONE;
		EXPECT_EQ(DIFFCODE::DIFF, bc.CompareFiles(files, di));

		files.SetLeft(_T("A"));
		files.SetRight(_T("B"));
		di.diffFileInfo[0].size = DirItem::FILE_SIZE_NONE;
		di.diffFileInfo[1].size = 1;
		EXPECT_EQ(DIFFCODE::DIFF, bc.CompareFiles(files, di));

		files.SetLeft(_T("A"));
		files.SetRight(_T("B"));
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 2;
		EXPECT_EQ(DIFFCODE::DIFF, bc.CompareFiles(files, di));

		files.SetLeft(_T("A"));
		files.SetMiddle(_T("B"));
		files.SetRight(_T("C"));
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 2;
		EXPECT_EQ(DIFFCODE::DIFF | DIFFCODE::DIFF3RDONLY, bc.CompareFiles(files, di));

		files.SetLeft(_T("A"));
		files.SetMiddle(_T("B"));
		files.SetRight(_T("C"));
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 2;
		di.diffFileInfo[2].size = 1;
		EXPECT_EQ(DIFFCODE::DIFF | DIFFCODE::DIFF2NDONLY, bc.CompareFiles(files, di));

		files.SetLeft(_T("A"));
		files.SetMiddle(_T("B"));
		files.SetRight(_T("C"));
		di.diffFileInfo[0].size = 2;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 1;
		EXPECT_EQ(DIFFCODE::DIFF | DIFFCODE::DIFF1STONLY, bc.CompareFiles(files, di));
	}

	TEST_F(BinaryCompareTest, Error)
	{
		CompareEngines::BinaryCompare bc;
		PathContext files;
		DIFFITEM di;

		TempFile l1("A", "1", 1);
		TempFile r1("B", "1", 1);

		files.SetLeft(_T("/1>"));
		files.SetRight(_T("B"));
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 1;
		EXPECT_EQ(DIFFCODE::CMPERR, bc.CompareFiles(files, di));

		files.SetLeft(_T("A"));
		files.SetRight(_T("/2>"));
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 1;
		EXPECT_EQ(DIFFCODE::CMPERR, bc.CompareFiles(files, di));

		files.SetLeft(_T("A"));
		files.SetMiddle(_T("B"));
		files.SetRight(_T("/3>"));
		di.diffFileInfo[0].size = 1;
		di.diffFileInfo[1].size = 1;
		di.diffFileInfo[2].size = 1;
		EXPECT_EQ(DIFFCODE::CMPERR, bc.CompareFiles(files, di));
	}

}  // namespace
