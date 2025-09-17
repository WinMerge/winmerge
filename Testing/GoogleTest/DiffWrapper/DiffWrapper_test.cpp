#include "pch.h"
#include <gtest/gtest.h>
#include "DiffWrapper.h"
#include "PathContext.h"
#include "paths.h"
#include "TempFile.h"
#include "UniFile.h"
#include "LineFiltersList.h"
#include "SubstitutionFiltersList.h"

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

		options.nIgnoreWhitespace = WHITESPACE_COMPARE_ALL;
		{
			DiffList diffList;
			TempFile left = WriteToTempFile(_T("0\na\nb\nc\n"));
			TempFile right = WriteToTempFile(_T("0\na b c\n"));
			dw.SetCreateDiffList(&diffList);
			dw.SetPaths({ left.GetPath(), right.GetPath() }, false);
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
			TempFile left = WriteToTempFile(_T("0\na\n b\n  c\n"));
			TempFile right = WriteToTempFile(_T("0\na b c\n"));
			dw.SetCreateDiffList(&diffList);
			dw.SetPaths({ left.GetPath(), right.GetPath() }, false);
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
			TempFile left = WriteToTempFile(_T("0\na\nb\nc\n"));
			TempFile right = WriteToTempFile(_T("0\nabc\n"));
			dw.SetCreateDiffList(&diffList);
			dw.SetPaths({ left.GetPath(), right.GetPath() }, false);
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
