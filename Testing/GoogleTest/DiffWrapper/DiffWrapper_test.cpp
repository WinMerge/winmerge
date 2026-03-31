#include "pch.h"
#include <gtest/gtest.h>
#include "DiffWrapper.h"
#include "PathContext.h"
#include "paths.h"
#include "TempFile.h"
#include "UniFile.h"
#include "LineFiltersList.h"
#include "SubstitutionFiltersList.h"
#include <cstdio>
#include <vector>

const TempFile WriteToTempFile(const String& text)
{
	TempFile tmpfile;
	tmpfile.Create();
	UniStdioFile file;
	file.OpenCreateUtf8(tmpfile.GetPath());
	file.WriteString(text);
	file.Close();
	return tmpfile;
}

const TempFile WriteBinaryToTempFile(const std::vector<unsigned char>& bytes)
{
	TempFile tmpfile;
	tmpfile.Create();
	FILE* file = _wfopen(tmpfile.GetPath().c_str(), L"wb");
	if (file == nullptr)
		return tmpfile;

	if (!bytes.empty())
		fwrite(bytes.data(), sizeof(unsigned char), bytes.size(), file);
	fclose(file);
	return tmpfile;
}

String MakeLargeTextFixture(size_t lines, size_t changedLine)
{
	String text;
	text.reserve(lines * 24);
	for (size_t i = 0; i < lines; ++i)
	{
		text += _T("line:");
		text += std::to_wstring(i);
		text += (i == changedLine) ? _T(":changed\n") : _T(":stable\n");
	}
	return text;
}

TEST(DiffWrapper, RunFileDiff_NoEol)
{
	CDiffWrapper dw;
	DIFFOPTIONS options{};
	DIFFRANGE dr;

	for (auto algo : { DIFF_ALGORITHM_DEFAULT, DIFF_ALGORITHM_MINIMAL, DIFF_ALGORITHM_PATIENCE, DIFF_ALGORITHM_HISTOGRAM, DIFF_ALGORITHM_NONE })
	{
		options.nDiffAlgorithm = algo;

		{
			DiffList diffList;
			TempFile left = WriteToTempFile(_T("a\nb\nc1"));
			TempFile right = WriteToTempFile(_T("a\nb\nc2"));
			dw.SetCreateDiffList(&diffList);
			dw.SetPaths({ left.GetPath(), right.GetPath() }, false);
			dw.SetOptions(&options);
			dw.RunFileDiff();
			EXPECT_EQ(1, diffList.GetSize());
			diffList.GetDiff(0, dr);
			EXPECT_EQ(2, dr.begin[0]);
			EXPECT_EQ(2, dr.begin[1]);
			EXPECT_EQ(2, dr.end[0]);
			EXPECT_EQ(2, dr.end[1]);
		}

		{
			DiffList diffList;
			TempFile left = WriteToTempFile(_T("a\nb\nc1\n"));
			TempFile right = WriteToTempFile(_T("a\nb\nc2"));
			dw.SetCreateDiffList(&diffList);
			dw.SetPaths({ left.GetPath(), right.GetPath() }, false);
			dw.SetOptions(&options);
			dw.RunFileDiff();
			EXPECT_EQ(1, diffList.GetSize());
			diffList.GetDiff(0, dr);
			EXPECT_EQ(2, dr.begin[0]);
			EXPECT_EQ(2, dr.begin[1]);
			EXPECT_EQ(2, dr.end[0]);
			EXPECT_EQ(2, dr.end[1]);
		}

		{
			DiffList diffList;
			TempFile left = WriteToTempFile(_T("a\nb\nc1"));
			TempFile right = WriteToTempFile(_T("a\nb\nc2\n"));
			dw.SetCreateDiffList(&diffList);
			dw.SetPaths({ left.GetPath(), right.GetPath() }, false);
			dw.SetOptions(&options);
			dw.RunFileDiff();
			EXPECT_EQ(1, diffList.GetSize());
			diffList.GetDiff(0, dr);
			EXPECT_EQ(2, dr.begin[0]);
			EXPECT_EQ(2, dr.begin[1]);
			EXPECT_EQ(2, dr.end[0]);
			EXPECT_EQ(2, dr.end[1]);
		}

		{
			DiffList diffList;
			TempFile left = WriteToTempFile(_T("a\nb1\nc"));
			TempFile right = WriteToTempFile(_T("a\nb2\nc"));
			dw.SetCreateDiffList(&diffList);
			dw.SetPaths({ left.GetPath(), right.GetPath() }, false);
			dw.SetOptions(&options);
			dw.RunFileDiff();
			EXPECT_EQ(1, diffList.GetSize());
			diffList.GetDiff(0, dr);
			EXPECT_EQ(1, dr.begin[0]);
			EXPECT_EQ(1, dr.begin[1]);
			EXPECT_EQ(1, dr.end[0]);
			EXPECT_EQ(1, dr.end[1]);
		}
	}
}

TEST(DiffWrapper, RunFileDiff_IgnoreMissingTrailingEol)
{
	CDiffWrapper dw;
	DIFFOPTIONS options{};
	DIFFRANGE dr;

	options.bIgnoreMissingTrailingEol = true;
	for (auto algo : { DIFF_ALGORITHM_DEFAULT, DIFF_ALGORITHM_MINIMAL, DIFF_ALGORITHM_PATIENCE, DIFF_ALGORITHM_HISTOGRAM, DIFF_ALGORITHM_NONE })
	{
		options.nDiffAlgorithm = algo;

		{
			DiffList diffList;
			TempFile left = WriteToTempFile(_T("a\nb\nc1"));
			TempFile right = WriteToTempFile(_T("a\nb\nc2"));
			dw.SetCreateDiffList(&diffList);
			dw.SetPaths({ left.GetPath(), right.GetPath() }, false);
			dw.SetOptions(&options);
			dw.RunFileDiff();
			EXPECT_EQ(1, diffList.GetSize());
			diffList.GetDiff(0, dr);
			EXPECT_EQ(2, dr.begin[0]);
			EXPECT_EQ(2, dr.begin[1]);
			EXPECT_EQ(2, dr.end[0]);
			EXPECT_EQ(2, dr.end[1]);
			EXPECT_EQ(OP_DIFF, dr.op);
		}

		{
			DiffList diffList;
			TempFile left = WriteToTempFile(_T("a\nb\nc1\n"));
			TempFile right = WriteToTempFile(_T("a\nb\nc2"));
			dw.SetCreateDiffList(&diffList);
			dw.SetPaths({ left.GetPath(), right.GetPath() }, false);
			dw.SetOptions(&options);
			dw.RunFileDiff();
			EXPECT_EQ(1, diffList.GetSize());
			diffList.GetDiff(0, dr);
			EXPECT_EQ(2, dr.begin[0]);
			EXPECT_EQ(2, dr.begin[1]);
			EXPECT_EQ(2, dr.end[0]);
			EXPECT_EQ(2, dr.end[1]);
			EXPECT_EQ(OP_DIFF, dr.op);
		}

		{
			DiffList diffList;
			TempFile left = WriteToTempFile(_T("a\nb\nc1"));
			TempFile right = WriteToTempFile(_T("a\nb\nc2\n"));
			dw.SetCreateDiffList(&diffList);
			dw.SetPaths({ left.GetPath(), right.GetPath() }, false);
			dw.SetOptions(&options);
			dw.RunFileDiff();
			EXPECT_EQ(1, diffList.GetSize());
			diffList.GetDiff(0, dr);
			EXPECT_EQ(2, dr.begin[0]);
			EXPECT_EQ(2, dr.begin[1]);
			EXPECT_EQ(2, dr.end[0]);
			EXPECT_EQ(2, dr.end[1]);
			EXPECT_EQ(OP_DIFF, dr.op);
		}

		{
			DiffList diffList;
			TempFile left = WriteToTempFile(_T("a\nb1\nc"));
			TempFile right = WriteToTempFile(_T("a\nb2\nc"));
			dw.SetCreateDiffList(&diffList);
			dw.SetPaths({ left.GetPath(), right.GetPath() }, false);
			dw.SetOptions(&options);
			dw.RunFileDiff();
			EXPECT_EQ(1, diffList.GetSize());
			diffList.GetDiff(0, dr);
			EXPECT_EQ(1, dr.begin[0]);
			EXPECT_EQ(1, dr.begin[1]);
			EXPECT_EQ(1, dr.end[0]);
			EXPECT_EQ(1, dr.end[1]);
			EXPECT_EQ(OP_DIFF, dr.op);
		}

		for (const auto& eol : { _T("\n"), _T("\r"), _T("\r\n") })
		{
			DiffList diffList;
			TempFile left = WriteToTempFile(_T("a\nb\nc"));
			TempFile right = WriteToTempFile(_T("a\nb\nc") + String(eol));
			dw.SetCreateDiffList(&diffList);
			dw.SetPaths({ left.GetPath(), right.GetPath() }, false);
			dw.SetOptions(&options);
			dw.RunFileDiff();
			EXPECT_EQ(1, diffList.GetSize());
			diffList.GetDiff(0, dr);
			EXPECT_EQ(2, dr.begin[0]);
			EXPECT_EQ(2, dr.begin[1]);
			EXPECT_EQ(2, dr.end[0]);
			EXPECT_EQ(2, dr.end[1]);
			EXPECT_EQ(OP_TRIVIAL, dr.op);
		}

		for (const auto& eol : { _T("\n"), _T("\r"), _T("\r\n") })
		{
			DiffList diffList;
			TempFile left = WriteToTempFile(_T("a\nb\nc") + String(eol));
			TempFile right = WriteToTempFile(_T("a\nb\nc"));
			dw.SetCreateDiffList(&diffList);
			dw.SetPaths({ left.GetPath(), right.GetPath() }, false);
			dw.SetOptions(&options);
			dw.RunFileDiff();
			EXPECT_EQ(1, diffList.GetSize());
			diffList.GetDiff(0, dr);
			EXPECT_EQ(2, dr.begin[0]);
			EXPECT_EQ(2, dr.begin[1]);
			EXPECT_EQ(2, dr.end[0]);
			EXPECT_EQ(2, dr.end[1]);
			EXPECT_EQ(OP_TRIVIAL, dr.op);
		}

	}
}

TEST(DiffWrapper, RunFileDiff_IgnoreLineBreaks)
{
	CDiffWrapper dw;
	DIFFOPTIONS options{};
	DIFFRANGE dr;

	options.bIgnoreLineBreaks = true;
	for (auto algo : { DIFF_ALGORITHM_DEFAULT, DIFF_ALGORITHM_MINIMAL, DIFF_ALGORITHM_PATIENCE, DIFF_ALGORITHM_HISTOGRAM, DIFF_ALGORITHM_NONE })
	{
		options.nDiffAlgorithm = algo;
		options.bFilterCommentsLines = false;

		options.nIgnoreWhitespace = WHITESPACE_COMPARE_ALL;
		{
			DiffList diffList;
			TempFile left = WriteToTempFile(_T("0\na\r\nb\rc\n"));
			TempFile right = WriteToTempFile(_T("0\na b c\n"));
			dw.SetCreateDiffList(&diffList);
			dw.SetPaths({ left.GetPath(), right.GetPath() }, false);
			dw.SetFilterCommentsSourceDef(_T("cpp"));
			dw.SetOptions(&options);
			dw.RunFileDiff();
			EXPECT_EQ(1, diffList.GetSize());
			diffList.GetDiff(0, dr);
			EXPECT_EQ(1, dr.begin[0]);
			EXPECT_EQ(1, dr.begin[1]);
			EXPECT_EQ(3, dr.end[0]);
			EXPECT_EQ(1, dr.end[1]);
			EXPECT_EQ(OP_TRIVIAL, dr.op);
		}

		options.nIgnoreWhitespace = WHITESPACE_IGNORE_CHANGE;
		{
			DiffList diffList;
			TempFile left = WriteToTempFile(_T("0\na\r\n b\r  c\n"));
			TempFile right = WriteToTempFile(_T("0\na b c\n"));
			dw.SetCreateDiffList(&diffList);
			dw.SetPaths({ left.GetPath(), right.GetPath() }, false);
			dw.SetFilterCommentsSourceDef(_T("cpp"));
			dw.SetOptions(&options);
			dw.RunFileDiff();
			EXPECT_EQ(1, diffList.GetSize());
			diffList.GetDiff(0, dr);
			EXPECT_EQ(1, dr.begin[0]);
			EXPECT_EQ(1, dr.begin[1]);
			EXPECT_EQ(3, dr.end[0]);
			EXPECT_EQ(1, dr.end[1]);
			EXPECT_EQ(OP_TRIVIAL, dr.op);
		}

		options.nIgnoreWhitespace = WHITESPACE_IGNORE_ALL;
		{
			DiffList diffList;
			TempFile left = WriteToTempFile(_T("0\na\r\nb\rc\n"));
			TempFile right = WriteToTempFile(_T("0\nabc\n"));
			dw.SetCreateDiffList(&diffList);
			dw.SetPaths({ left.GetPath(), right.GetPath() }, false);
			dw.SetFilterCommentsSourceDef(_T("cpp"));
			dw.SetOptions(&options);
			dw.RunFileDiff();
			EXPECT_EQ(1, diffList.GetSize());
			diffList.GetDiff(0, dr);
			EXPECT_EQ(1, dr.begin[0]);
			EXPECT_EQ(1, dr.begin[1]);
			EXPECT_EQ(3, dr.end[0]);
			EXPECT_EQ(1, dr.end[1]);
			EXPECT_EQ(OP_TRIVIAL, dr.op);
		}

		options.nIgnoreWhitespace = WHITESPACE_COMPARE_ALL;
		options.bFilterCommentsLines = true;
		{
			DiffList diffList;
			TempFile left = WriteToTempFile(_T("0\na\r\n/*b*/\rc\n"));
			TempFile right = WriteToTempFile(_T("0\na /*bb*/ c\n"));
			dw.SetCreateDiffList(&diffList);
			dw.SetPaths({ left.GetPath(), right.GetPath() }, false);
			dw.SetFilterCommentsSourceDef(_T("cpp"));
			dw.SetOptions(&options);
			dw.RunFileDiff();
			EXPECT_EQ(1, diffList.GetSize());
			diffList.GetDiff(0, dr);
			EXPECT_EQ(1, dr.begin[0]);
			EXPECT_EQ(1, dr.begin[1]);
			EXPECT_EQ(3, dr.end[0]);
			EXPECT_EQ(1, dr.end[1]);
			EXPECT_EQ(OP_TRIVIAL, dr.op);
		}
	}
}

TEST(DiffWrapper, RunFileDiff_IgnoreComments)
{
	CDiffWrapper dw;
	DIFFOPTIONS options{};
	DIFFRANGE dr;

	for (auto algo : { DIFF_ALGORITHM_DEFAULT, DIFF_ALGORITHM_MINIMAL, DIFF_ALGORITHM_PATIENCE, DIFF_ALGORITHM_HISTOGRAM })
	{
		options.nDiffAlgorithm = algo;
		options.bFilterCommentsLines = true;

		{
			DiffList diffList;
			TempFile left = WriteToTempFile(_T("a\n/*b1*/\nc"));
			TempFile right = WriteToTempFile(_T("a\n/*b2*/\nc"));
			dw.SetCreateDiffList(&diffList);
			dw.SetPaths({ left.GetPath(), right.GetPath() }, false);
			dw.SetOptions(&options);
			dw.SetFilterCommentsSourceDef(_T("cpp"));
			dw.RunFileDiff();
			EXPECT_EQ(1, diffList.GetSize());
			diffList.GetDiff(0, dr);
			EXPECT_EQ(OP_TRIVIAL, dr.op);
			EXPECT_EQ(1, dr.begin[0]);
			EXPECT_EQ(1, dr.begin[1]);
			EXPECT_EQ(1, dr.end[0]);
			EXPECT_EQ(1, dr.end[1]);
		}

		{
			DiffList diffList;
			TempFile left  = WriteToTempFile(_T("a\n/*\nb1\n*/\nc"));
			TempFile right = WriteToTempFile(_T("a\n/*\nb2\nb3\n*/\nc"));
			dw.SetCreateDiffList(&diffList);
			dw.SetPaths({ left.GetPath(), right.GetPath() }, false);
			dw.SetOptions(&options);
			dw.SetFilterCommentsSourceDef(_T("cpp"));
			dw.RunFileDiff();
			EXPECT_EQ(2, diffList.GetSize());
			diffList.GetDiff(0, dr);
			EXPECT_EQ(OP_TRIVIAL, dr.op);
			EXPECT_EQ(2, dr.begin[0]);
			EXPECT_EQ(2, dr.begin[1]);
			EXPECT_EQ(2, dr.end[0]);
			EXPECT_EQ(2, dr.end[1]);
			diffList.GetDiff(1, dr);
			EXPECT_EQ(OP_TRIVIAL, dr.op);
			EXPECT_EQ(3, dr.begin[0]);
			EXPECT_EQ(3, dr.begin[1]);
			EXPECT_EQ(2, dr.end[0]);
			EXPECT_EQ(3, dr.end[1]);
		}

		{
			DiffList diffList;
			TempFile left  = WriteToTempFile(_T("a\n//b1\nc"));
			TempFile right = WriteToTempFile(_T("a\n//b2\nc"));
			dw.SetCreateDiffList(&diffList);
			dw.SetPaths({ left.GetPath(), right.GetPath() }, false);
			dw.SetOptions(&options);
			dw.SetFilterCommentsSourceDef(_T("cpp"));
			dw.RunFileDiff();
			EXPECT_EQ(1, diffList.GetSize());
			diffList.GetDiff(0, dr);
			EXPECT_EQ(OP_TRIVIAL, dr.op);
			EXPECT_EQ(1, dr.begin[0]);
			EXPECT_EQ(1, dr.begin[1]);
			EXPECT_EQ(1, dr.end[0]);
			EXPECT_EQ(1, dr.end[1]);
		}
	}
}

TEST(DiffWrapper, RunFileDiff_LineFilters)
{
	CDiffWrapper dw;
	DIFFOPTIONS options{};
	DIFFRANGE dr;

	for (auto algo : { DIFF_ALGORITHM_DEFAULT, DIFF_ALGORITHM_MINIMAL, DIFF_ALGORITHM_PATIENCE, DIFF_ALGORITHM_HISTOGRAM })
	{
		options.nDiffAlgorithm = algo;
		LineFiltersList lineFilterList;
		lineFilterList.AddFilter(_T("\\d{4}-\\d{2}-\\d{2}"), true);

		{
			DiffList diffList;
			TempFile left  = WriteToTempFile(_T("a\n# 2023-10-09\nc"));
			TempFile right = WriteToTempFile(_T("a\n# 2023-10-08\nc"));
			dw.SetFilterList(lineFilterList.MakeFilterList());
			dw.SetCreateDiffList(&diffList);
			dw.SetPaths({ left.GetPath(), right.GetPath() }, false);
			dw.SetOptions(&options);
			dw.RunFileDiff();
			EXPECT_EQ(1, diffList.GetSize());
			diffList.GetDiff(0, dr);
			EXPECT_EQ(OP_TRIVIAL, dr.op);
			EXPECT_EQ(1, dr.begin[0]);
			EXPECT_EQ(1, dr.begin[1]);
			EXPECT_EQ(1, dr.end[0]);
			EXPECT_EQ(1, dr.end[1]);
		}

		{
			DiffList diffList;
			TempFile left  = WriteToTempFile(_T("a\n# 2023-10-09\n# 2023-10-09\nc"));
			TempFile right = WriteToTempFile(_T("a\n# 2023-10-08\nc"));
			dw.SetFilterList(lineFilterList.MakeFilterList());
			dw.SetCreateDiffList(&diffList);
			dw.SetPaths({ left.GetPath(), right.GetPath() }, false);
			dw.SetOptions(&options);
			dw.RunFileDiff();
			EXPECT_EQ(1, diffList.GetSize());
			diffList.GetDiff(0, dr);
			EXPECT_EQ(OP_TRIVIAL, dr.op);
			EXPECT_EQ(1, dr.begin[0]);
			EXPECT_EQ(1, dr.begin[1]);
			EXPECT_EQ(2, dr.end[0]);
			EXPECT_EQ(1, dr.end[1]);
		}

		{
			DiffList diffList;
			TempFile left  = WriteToTempFile(_T("a\n# 2023-10-09\nb1\nc"));
			TempFile right = WriteToTempFile(_T("a\n# 2023-10-08\nb2\nc"));
			dw.SetFilterList(lineFilterList.MakeFilterList());
			dw.SetCreateDiffList(&diffList);
			dw.SetPaths({ left.GetPath(), right.GetPath() }, false);
			dw.SetOptions(&options);
			dw.RunFileDiff();
			EXPECT_EQ(2, diffList.GetSize());
			diffList.GetDiff(0, dr);
			EXPECT_EQ(OP_TRIVIAL, dr.op);
			EXPECT_EQ(1, dr.begin[0]);
			EXPECT_EQ(1, dr.begin[1]);
			EXPECT_EQ(1, dr.end[0]);
			EXPECT_EQ(1, dr.end[1]);
			diffList.GetDiff(1, dr);
			EXPECT_EQ(OP_DIFF, dr.op);
			EXPECT_EQ(2, dr.begin[0]);
			EXPECT_EQ(2, dr.begin[1]);
			EXPECT_EQ(2, dr.end[0]);
			EXPECT_EQ(2, dr.end[1]);
		}
	}
}

TEST(DiffWrapper, RunFileDiff_SubstitutionFilters)
{
	CDiffWrapper dw;
	DIFFOPTIONS options{};
	DIFFRANGE dr;

	for (auto algo : { DIFF_ALGORITHM_DEFAULT, DIFF_ALGORITHM_MINIMAL, DIFF_ALGORITHM_PATIENCE, DIFF_ALGORITHM_HISTOGRAM })
	{
		options.nDiffAlgorithm = algo;
		SubstitutionFiltersList substitutionFilterList;
		substitutionFilterList.Add(_T("\\d{4}-\\d{2}-\\d{2}"), _T("XXXX-XX-XX"), true, false, false, true);

		{
			DiffList diffList;
			TempFile left  = WriteToTempFile(_T("a\n# 2023-10-09\nc"));
			TempFile right = WriteToTempFile(_T("a\n# 2023-10-08\nc"));
			dw.SetSubstitutionList(substitutionFilterList.MakeSubstitutionList());
			dw.SetCreateDiffList(&diffList);
			dw.SetPaths({ left.GetPath(), right.GetPath() }, false);
			dw.SetOptions(&options);
			dw.RunFileDiff();
			EXPECT_EQ(1, diffList.GetSize());
			diffList.GetDiff(0, dr);
			EXPECT_EQ(OP_TRIVIAL, dr.op);
			EXPECT_EQ(1, dr.begin[0]);
			EXPECT_EQ(1, dr.begin[1]);
			EXPECT_EQ(1, dr.end[0]);
			EXPECT_EQ(1, dr.end[1]);
		}

		{
			DiffList diffList;
			TempFile left  = WriteToTempFile(_T("a\n# 2023-10-09\nb1\nc"));
			TempFile right = WriteToTempFile(_T("a\n# 2023-10-08\nb2\nc"));
			dw.SetSubstitutionList(substitutionFilterList.MakeSubstitutionList());
			dw.SetCreateDiffList(&diffList);
			dw.SetPaths({ left.GetPath(), right.GetPath() }, false);
			dw.SetOptions(&options);
			dw.RunFileDiff();
			EXPECT_EQ(2, diffList.GetSize());
			diffList.GetDiff(0, dr);
			EXPECT_EQ(OP_TRIVIAL, dr.op);
			EXPECT_EQ(1, dr.begin[0]);
			EXPECT_EQ(1, dr.begin[1]);
			EXPECT_EQ(1, dr.end[0]);
			EXPECT_EQ(1, dr.end[1]);
			diffList.GetDiff(1, dr);
			EXPECT_EQ(OP_DIFF, dr.op);
			EXPECT_EQ(2, dr.begin[0]);
			EXPECT_EQ(2, dr.begin[1]);
			EXPECT_EQ(2, dr.end[0]);
			EXPECT_EQ(2, dr.end[1]);
		}
	}
}

TEST(DiffWrapper, RunFileDiff_UnicodeRoundTripAndMutation)
{
	CDiffWrapper dw;
	DIFFOPTIONS options{};
	DIFFRANGE dr;

	{
		DiffList diffList;
		TempFile left = WriteToTempFile(_T("caf\u00E9\nna\u00EFve\n\u6771\u4EAC\n"));
		TempFile right = WriteToTempFile(_T("caf\u00E9\nna\u00EFve\n\u6771\u4EAC\n"));
		dw.SetCreateDiffList(&diffList);
		dw.SetPaths({ left.GetPath(), right.GetPath() }, false);
		dw.SetOptions(&options);
		dw.RunFileDiff();
		EXPECT_EQ(0, diffList.GetSize());
	}

	{
		DiffList diffList;
		TempFile left = WriteToTempFile(_T("caf\u00E9\nna\u00EFve\n\u6771\u4EAC\n"));
		TempFile right = WriteToTempFile(_T("cafe\nna\u00EFve\n\u6771\u4EAC\n"));
		dw.SetCreateDiffList(&diffList);
		dw.SetPaths({ left.GetPath(), right.GetPath() }, false);
		dw.SetOptions(&options);
		dw.RunFileDiff();
		EXPECT_EQ(1, diffList.GetSize());
		diffList.GetDiff(0, dr);
		EXPECT_EQ(OP_DIFF, dr.op);
	}
}

TEST(DiffWrapper, RunFileDiff_WhitespaceOnlyChangeBehavior)
{
	CDiffWrapper dw;
	DIFFOPTIONS options{};
	DIFFRANGE dr;

	const TempFile left = WriteToTempFile(_T("alpha beta\nsame\n"));
	const TempFile right = WriteToTempFile(_T("alpha   beta\nsame\n"));

	{
		DiffList diffList;
		dw.SetCreateDiffList(&diffList);
		dw.SetPaths({ left.GetPath(), right.GetPath() }, false);
		dw.SetOptions(&options);
		dw.RunFileDiff();
		EXPECT_EQ(1, diffList.GetSize());
		diffList.GetDiff(0, dr);
		EXPECT_EQ(OP_DIFF, dr.op);
	}

	options.nIgnoreWhitespace = WHITESPACE_IGNORE_CHANGE;
	{
		DiffList diffList;
		dw.SetCreateDiffList(&diffList);
		dw.SetPaths({ left.GetPath(), right.GetPath() }, false);
		dw.SetOptions(&options);
		dw.RunFileDiff();
		EXPECT_EQ(1, diffList.GetSize());
		diffList.GetDiff(0, dr);
		EXPECT_EQ(OP_TRIVIAL, dr.op);
	}
}

TEST(DiffWrapper, RunFileDiff_BinaryDetection)
{
	CDiffWrapper dw;
	DIFFOPTIONS options{};
	DIFFSTATUS status{};

	const TempFile left = WriteBinaryToTempFile({ 0x00, 0x11, 0x22, 0x33, 0x44, 0x55 });
	const TempFile right = WriteBinaryToTempFile({ 0x00, 0x11, 0x22, 0x99, 0x44, 0x55 });
	DiffList diffList;
	dw.SetCreateDiffList(&diffList);
	dw.SetPaths({ left.GetPath(), right.GetPath() }, false);
	dw.SetOptions(&options);
	dw.RunFileDiff();
	dw.GetDiffStatus(&status);

	EXPECT_TRUE(status.bBinaries);
	EXPECT_GE(diffList.GetSize(), 1);
}

TEST(DiffWrapper, RunFileDiff_LargeFileBaselineOver1MiB)
{
	CDiffWrapper dw;
	DIFFOPTIONS options{};
	DIFFRANGE dr;

	const size_t kLines = 75000; // >1MiB payload with current fixture shape.
	const TempFile left = WriteToTempFile(MakeLargeTextFixture(kLines, static_cast<size_t>(-1)));
	const TempFile right = WriteToTempFile(MakeLargeTextFixture(kLines, 74000));

	DiffList diffList;
	const ULONGLONG startMs = GetTickCount64();
	dw.SetCreateDiffList(&diffList);
	dw.SetPaths({ left.GetPath(), right.GetPath() }, false);
	dw.SetOptions(&options);
	dw.RunFileDiff();
	const ULONGLONG elapsedMs = GetTickCount64() - startMs;

	EXPECT_GE(diffList.GetSize(), 1);
	diffList.GetDiff(0, dr);
	EXPECT_EQ(OP_DIFF, dr.op);
	EXPECT_LT(elapsedMs, 30000ULL);
}
