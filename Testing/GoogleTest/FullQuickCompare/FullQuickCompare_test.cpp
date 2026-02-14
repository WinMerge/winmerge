#include "pch.h"
#include <gtest/gtest.h>
#include "DiffContext.h"
#include "PathContext.h"
#include "CompareEngines/FullQuickCompare.h"
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

	class FullQuickCompareTest : public testing::Test
	{
	protected:
		FullQuickCompareTest()
		{
		}

		virtual ~FullQuickCompareTest()
		{
		}

		virtual void SetUp()
		{
		}

		virtual void TearDown()
		{
		}
	};

	TEST_F(FullQuickCompareTest, FullCompare_SameText)
	{
		PathContext paths;
		paths.SetLeft(_T("."));
		paths.SetRight(_T("."));
		CDiffContext ctxt(paths, CMP_CONTENT);
		DIFFOPTIONS options{};
		ctxt.CreateCompareOptions(CMP_CONTENT, options);
		CompareEngines::FullQuickCompare fc(ctxt, CMP_CONTENT);

		DIFFITEM di;

		{
			TempFile l1("A.txt", "line1\r\nline2\r\nline3\r\n", 21);
			TempFile r1("B.txt", "line1\r\nline2\r\nline3\r\n", 21);
			di.diffcode.setSideFlag(0);
			di.diffcode.setSideFlag(1);
			di.diffFileInfo[0].SetFile(_T("A.txt"));
			di.diffFileInfo[1].SetFile(_T("B.txt"));
			di.diffFileInfo[0].size = 21;
			di.diffFileInfo[1].size = 21;
			fc.CompareFiles(di);
			EXPECT_EQ(int(DIFFCODE::TEXT|DIFFCODE::FILE|DIFFCODE::SAME), di.diffcode.diffcode & (DIFFCODE::TEXTFLAGS | DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS));
		}
	}

	TEST_F(FullQuickCompareTest, FullCompare_DifferentText)
	{
		PathContext paths;
		paths.SetLeft(_T("."));
		paths.SetRight(_T("."));
		CDiffContext ctxt(paths, CMP_CONTENT);
		DIFFOPTIONS options{};
		ctxt.CreateCompareOptions(CMP_CONTENT, options);
		CompareEngines::FullQuickCompare fc(ctxt, CMP_CONTENT);

		DIFFITEM di;

		{
			TempFile l1("A.txt", "line1\r\nline2\r\nline3\r\n", 21);
			TempFile r1("B.txt", "line1\r\nline2X\r\nline3\r\n", 22);
			di.diffcode.setSideFlag(0);
			di.diffcode.setSideFlag(1);
			di.diffFileInfo[0].SetFile(_T("A.txt"));
			di.diffFileInfo[1].SetFile(_T("B.txt"));
			di.diffFileInfo[0].size = 21;
			di.diffFileInfo[1].size = 22;
			fc.CompareFiles(di);
			EXPECT_EQ(int(DIFFCODE::TEXT|DIFFCODE::FILE|DIFFCODE::DIFF), di.diffcode.diffcode & (DIFFCODE::TEXTFLAGS | DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS));
		}
	}

	TEST_F(FullQuickCompareTest, QuickCompare_SameBytes)
	{
		PathContext paths;
		paths.SetLeft(_T("."));
		paths.SetRight(_T("."));
		CDiffContext ctxt(paths, CMP_QUICK_CONTENT);
		DIFFOPTIONS options{};
		ctxt.CreateCompareOptions(CMP_CONTENT, options);
		CompareEngines::FullQuickCompare qc(ctxt, CMP_QUICK_CONTENT);

		DIFFITEM di;

		{
			TempFile l1("A.bin", "\x00\x01\x02\x03\x04", 5);
			TempFile r1("B.bin", "\x00\x01\x02\x03\x04", 5);
			di.diffcode.setSideFlag(0);
			di.diffcode.setSideFlag(1);
			di.diffFileInfo[0].SetFile(_T("A.bin"));
			di.diffFileInfo[1].SetFile(_T("B.bin"));
			di.diffFileInfo[0].size = 5;
			di.diffFileInfo[1].size = 5;
			qc.CompareFiles(di);
			EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::BIN|DIFFCODE::BINSIDE1|DIFFCODE::BINSIDE2|DIFFCODE::SAME), di.diffcode.diffcode & (DIFFCODE::TEXTFLAGS | DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS));
		}
	}

	TEST_F(FullQuickCompareTest, QuickCompare_DifferentBytes)
	{
		PathContext paths;
		paths.SetLeft(_T("."));
		paths.SetRight(_T("."));
		CDiffContext ctxt(paths, CMP_QUICK_CONTENT);
		DIFFOPTIONS options{};
		ctxt.CreateCompareOptions(CMP_CONTENT, options);
		CompareEngines::FullQuickCompare qc(ctxt, CMP_QUICK_CONTENT);

		DIFFITEM di;

		{
			TempFile l1("A.bin", "\x00\x01\x02\x03\x04", 5);
			TempFile r1("B.bin", "\x00\x01\xFF\x03\x04", 5);
			di.diffcode.setSideFlag(0);
			di.diffcode.setSideFlag(1);
			di.diffFileInfo[0].SetFile(_T("A.bin"));
			di.diffFileInfo[1].SetFile(_T("B.bin"));
			di.diffFileInfo[0].size = 5;
			di.diffFileInfo[1].size = 5;
			qc.CompareFiles(di);
			EXPECT_EQ(int(DIFFCODE::FILE|DIFFCODE::BIN|DIFFCODE::BINSIDE1|DIFFCODE::BINSIDE2|DIFFCODE::DIFF), di.diffcode.diffcode & (DIFFCODE::TEXTFLAGS | DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS));
		}
	}

	TEST_F(FullQuickCompareTest, ThreeWay_FullCompare_AllSame)
	{
		PathContext paths3;
		paths3.SetLeft(_T("."));
		paths3.SetMiddle(_T("."));
		paths3.SetRight(_T("."));
		CDiffContext ctxt3(paths3, CMP_CONTENT);
		DIFFOPTIONS options{};
		ctxt3.CreateCompareOptions(CMP_CONTENT, options);
		CompareEngines::FullQuickCompare fc3(ctxt3, CMP_CONTENT);

		DIFFITEM di;

		{
			TempFile l1("A.txt", "line1\r\nline2\r\nline3\r\n", 21);
			TempFile m1("B.txt", "line1\r\nline2\r\nline3\r\n", 21);
			TempFile r1("C.txt", "line1\r\nline2\r\nline3\r\n", 21);
			di.diffcode.setSideFlag(0);
			di.diffcode.setSideFlag(1);
			di.diffcode.setSideFlag(2);
			di.diffcode.diffcode |= DIFFCODE::THREEWAY;
			di.diffFileInfo[0].SetFile(_T("A.txt"));
			di.diffFileInfo[1].SetFile(_T("B.txt"));
			di.diffFileInfo[2].SetFile(_T("C.txt"));
			di.diffFileInfo[0].size = 21;
			di.diffFileInfo[1].size = 21;
			di.diffFileInfo[2].size = 21;
			fc3.CompareFiles(di);
			EXPECT_EQ(int(DIFFCODE::TEXT|DIFFCODE::FILE|DIFFCODE::SAME), di.diffcode.diffcode & (DIFFCODE::TEXTFLAGS | DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));
		}
	}

	TEST_F(FullQuickCompareTest, ThreeWay_FullCompare_MiddleDifferent)
	{
		PathContext paths3;
		paths3.SetLeft(_T("."));
		paths3.SetMiddle(_T("."));
		paths3.SetRight(_T("."));
		CDiffContext ctxt3(paths3, CMP_CONTENT);
		DIFFOPTIONS options{};
		ctxt3.CreateCompareOptions(CMP_CONTENT, options);
		CompareEngines::FullQuickCompare fc3(ctxt3, CMP_CONTENT);

		DIFFITEM di;

		{
			TempFile l1("A.txt", "line1\r\nline2\r\nline3\r\n", 21);
			TempFile m1("B.txt", "line1\r\nline2X\r\nline3\r\n", 22);
			TempFile r1("C.txt", "line1\r\nline2\r\nline3\r\n", 21);
			di.diffcode.setSideFlag(0);
			di.diffcode.setSideFlag(1);
			di.diffcode.setSideFlag(2);
			di.diffcode.diffcode |= DIFFCODE::THREEWAY;
			di.diffFileInfo[0].SetFile(_T("A.txt"));
			di.diffFileInfo[1].SetFile(_T("B.txt"));
			di.diffFileInfo[2].SetFile(_T("C.txt"));
			di.diffFileInfo[0].size = 21;
			di.diffFileInfo[1].size = 22;
			di.diffFileInfo[2].size = 21;
			fc3.CompareFiles(di);
			EXPECT_EQ(int(DIFFCODE::TEXT|DIFFCODE::FILE|DIFFCODE::DIFF | DIFFCODE::DIFF2NDONLY), di.diffcode.diffcode & (DIFFCODE::TEXTFLAGS | DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));
		}
	}

	TEST_F(FullQuickCompareTest, ThreeWay_FullCompare_FirstDifferent)
	{
		PathContext paths3;
		paths3.SetLeft(_T("."));
		paths3.SetMiddle(_T("."));
		paths3.SetRight(_T("."));
		CDiffContext ctxt3(paths3, CMP_CONTENT);
		DIFFOPTIONS options{};
		ctxt3.CreateCompareOptions(CMP_CONTENT, options);
		CompareEngines::FullQuickCompare fc3(ctxt3, CMP_CONTENT);

		DIFFITEM di;

		{
			TempFile l1("A.txt", "line1X\r\nline2\r\nline3\r\n", 22);
			TempFile m1("B.txt", "line1\r\nline2\r\nline3\r\n", 21);
			TempFile r1("C.txt", "line1\r\nline2\r\nline3\r\n", 21);
			di.diffcode.setSideFlag(0);
			di.diffcode.setSideFlag(1);
			di.diffcode.setSideFlag(2);
			di.diffcode.diffcode |= DIFFCODE::THREEWAY;
			di.diffFileInfo[0].SetFile(_T("A.txt"));
			di.diffFileInfo[1].SetFile(_T("B.txt"));
			di.diffFileInfo[2].SetFile(_T("C.txt"));
			di.diffFileInfo[0].size = 22;
			di.diffFileInfo[1].size = 21;
			di.diffFileInfo[2].size = 21;
			fc3.CompareFiles(di);
			EXPECT_EQ(int(DIFFCODE::TEXT|DIFFCODE::FILE|DIFFCODE::DIFF | DIFFCODE::DIFF1STONLY), di.diffcode.diffcode & (DIFFCODE::TEXTFLAGS | DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));
		}
	}

	TEST_F(FullQuickCompareTest, ThreeWay_FullCompare_ThirdDifferent)
	{
		PathContext paths3;
		paths3.SetLeft(_T("."));
		paths3.SetMiddle(_T("."));
		paths3.SetRight(_T("."));
		CDiffContext ctxt3(paths3, CMP_CONTENT);
		DIFFOPTIONS options{};
		ctxt3.CreateCompareOptions(CMP_CONTENT, options);
		CompareEngines::FullQuickCompare fc3(ctxt3, CMP_CONTENT);

		DIFFITEM di;

		{
			TempFile l1("A.txt", "line1\r\nline2\r\nline3\r\n", 21);
			TempFile m1("B.txt", "line1\r\nline2\r\nline3\r\n", 21);
			TempFile r1("C.txt", "line1\r\nline2X\r\nline3\r\n", 22);
			di.diffcode.setSideFlag(0);
			di.diffcode.setSideFlag(1);
			di.diffcode.setSideFlag(2);
			di.diffcode.diffcode |= DIFFCODE::THREEWAY;
			di.diffFileInfo[0].SetFile(_T("A.txt"));
			di.diffFileInfo[1].SetFile(_T("B.txt"));
			di.diffFileInfo[2].SetFile(_T("C.txt"));
			di.diffFileInfo[0].size = 21;
			di.diffFileInfo[1].size = 21;
			di.diffFileInfo[2].size = 22;
			fc3.CompareFiles(di);
			EXPECT_EQ(int(DIFFCODE::TEXT|DIFFCODE::FILE|DIFFCODE::DIFF | DIFFCODE::DIFF3RDONLY), di.diffcode.diffcode & (DIFFCODE::TEXTFLAGS | DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));
		}
	}

	TEST_F(FullQuickCompareTest, ThreeWay_QuickCompare_AllSame)
	{
		PathContext paths3;
		paths3.SetLeft(_T("."));
		paths3.SetMiddle(_T("."));
		paths3.SetRight(_T("."));
		CDiffContext ctxt3(paths3, CMP_QUICK_CONTENT);
		DIFFOPTIONS options{};
		ctxt3.CreateCompareOptions(CMP_CONTENT, options);
		CompareEngines::FullQuickCompare qc3(ctxt3, CMP_QUICK_CONTENT);

		DIFFITEM di;

		{
			TempFile l1("A.bin", "\x00\x01\x02\x03\x04", 5);
			TempFile m1("B.bin", "\x00\x01\x02\x03\x04", 5);
			TempFile r1("C.bin", "\x00\x01\x02\x03\x04", 5);
			di.diffcode.setSideFlag(0);
			di.diffcode.setSideFlag(1);
			di.diffcode.setSideFlag(2);
			di.diffcode.diffcode |= DIFFCODE::THREEWAY;
			di.diffFileInfo[0].SetFile(_T("A.bin"));
			di.diffFileInfo[1].SetFile(_T("B.bin"));
			di.diffFileInfo[2].SetFile(_T("C.bin"));
			di.diffFileInfo[0].size = 5;
			di.diffFileInfo[1].size = 5;
			di.diffFileInfo[2].size = 5;
			qc3.CompareFiles(di);
			EXPECT_EQ(int(DIFFCODE::BIN|DIFFCODE::BINSIDE1|DIFFCODE::BINSIDE2|DIFFCODE::BINSIDE3|DIFFCODE::FILE|DIFFCODE::SAME), di.diffcode.diffcode & (DIFFCODE::TEXTFLAGS | DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));
		}
	}

	TEST_F(FullQuickCompareTest, ThreeWay_QuickCompare_FirstDifferent)
	{
		PathContext paths3;
		paths3.SetLeft(_T("."));
		paths3.SetMiddle(_T("."));
		paths3.SetRight(_T("."));
		CDiffContext ctxt3(paths3, CMP_QUICK_CONTENT);
		DIFFOPTIONS options{};
		ctxt3.CreateCompareOptions(CMP_CONTENT, options);
		CompareEngines::FullQuickCompare qc3(ctxt3, CMP_QUICK_CONTENT);

		DIFFITEM di;

		{
			TempFile l1("A.bin", "\x00\xFF\x02\x03\x04", 5);
			TempFile m1("B.bin", "\x00\x01\x02\x03\x04", 5);
			TempFile r1("C.bin", "\x00\x01\x02\x03\x04", 5);
			di.diffcode.setSideFlag(0);
			di.diffcode.setSideFlag(1);
			di.diffcode.setSideFlag(2);
			di.diffcode.diffcode |= DIFFCODE::THREEWAY;
			di.diffFileInfo[0].SetFile(_T("A.bin"));
			di.diffFileInfo[1].SetFile(_T("B.bin"));
			di.diffFileInfo[2].SetFile(_T("C.bin"));
			di.diffFileInfo[0].size = 5;
			di.diffFileInfo[1].size = 5;
			di.diffFileInfo[2].size = 5;
			qc3.CompareFiles(di);
			EXPECT_EQ(int(DIFFCODE::BIN|DIFFCODE::BINSIDE1|DIFFCODE::BINSIDE2|DIFFCODE::BINSIDE3|DIFFCODE::FILE|DIFFCODE::DIFF | DIFFCODE::DIFF1STONLY), di.diffcode.diffcode & (DIFFCODE::TEXTFLAGS | DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));
		}
	}

	TEST_F(FullQuickCompareTest, ThreeWay_QuickCompare_MiddleDifferent)
	{
		PathContext paths3;
		paths3.SetLeft(_T("."));
		paths3.SetMiddle(_T("."));
		paths3.SetRight(_T("."));
		CDiffContext ctxt3(paths3, CMP_QUICK_CONTENT);
		DIFFOPTIONS options{};
		ctxt3.CreateCompareOptions(CMP_CONTENT, options);
		CompareEngines::FullQuickCompare qc3(ctxt3, CMP_QUICK_CONTENT);

		DIFFITEM di;

		{
			TempFile l1("A.bin", "\x00\x01\x02\x03\x04", 5);
			TempFile m1("B.bin", "\x00\xFF\x02\x03\x04", 5);
			TempFile r1("C.bin", "\x00\x01\x02\x03\x04", 5);
			di.diffcode.setSideFlag(0);
			di.diffcode.setSideFlag(1);
			di.diffcode.setSideFlag(2);
			di.diffcode.diffcode |= DIFFCODE::THREEWAY;
			di.diffFileInfo[0].SetFile(_T("A.bin"));
			di.diffFileInfo[1].SetFile(_T("B.bin"));
			di.diffFileInfo[2].SetFile(_T("C.bin"));
			di.diffFileInfo[0].size = 5;
			di.diffFileInfo[1].size = 5;
			di.diffFileInfo[2].size = 5;
			qc3.CompareFiles(di);
			EXPECT_EQ(int(DIFFCODE::BIN|DIFFCODE::BINSIDE1|DIFFCODE::BINSIDE2|DIFFCODE::BINSIDE3|DIFFCODE::FILE|DIFFCODE::DIFF | DIFFCODE::DIFF2NDONLY), di.diffcode.diffcode & (DIFFCODE::TEXTFLAGS | DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));
		}
	}

	TEST_F(FullQuickCompareTest, ThreeWay_QuickCompare_ThirdDifferent)
	{
		PathContext paths3;
		paths3.SetLeft(_T("."));
		paths3.SetMiddle(_T("."));
		paths3.SetRight(_T("."));
		CDiffContext ctxt3(paths3, CMP_QUICK_CONTENT);
		DIFFOPTIONS options{};
		ctxt3.CreateCompareOptions(CMP_CONTENT, options);
		CompareEngines::FullQuickCompare qc3(ctxt3, CMP_QUICK_CONTENT);

		DIFFITEM di;

		{
			TempFile l1("A.bin", "\x00\x01\x02\x03\x04", 5);
			TempFile m1("B.bin", "\x00\x01\x02\x03\x04", 5);
			TempFile r1("C.bin", "\x00\x01\x02\xFF\x04", 5);
			di.diffcode.setSideFlag(0);
			di.diffcode.setSideFlag(1);
			di.diffcode.setSideFlag(2);
			di.diffcode.diffcode |= DIFFCODE::THREEWAY;
			di.diffFileInfo[0].SetFile(_T("A.bin"));
			di.diffFileInfo[1].SetFile(_T("B.bin"));
			di.diffFileInfo[2].SetFile(_T("C.bin"));
			di.diffFileInfo[0].size = 5;
			di.diffFileInfo[1].size = 5;
			di.diffFileInfo[2].size = 5;
			qc3.CompareFiles(di);
			EXPECT_EQ(int(DIFFCODE::BIN|DIFFCODE::BINSIDE1|DIFFCODE::BINSIDE2|DIFFCODE::BINSIDE3|DIFFCODE::FILE|DIFFCODE::DIFF | DIFFCODE::DIFF3RDONLY), di.diffcode.diffcode & (DIFFCODE::TEXTFLAGS | DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY));
		}
	}

}  // namespace
