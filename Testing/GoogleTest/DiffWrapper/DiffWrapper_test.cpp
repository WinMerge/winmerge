#include "pch.h"
#include <gtest/gtest.h>
#include "DiffWrapper.h"
#include "PathContext.h"
#include "paths.h"
#include "TempFile.h"
#include "UniFile.h"
#include "LineFiltersList.h"
#include "SubstitutionFiltersList.h"
#include "SyntaxParserRegistry.h"
#include "CrystalLineSyntaxParser.h"

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

TEST(DiffWrapper, RunFileDiff_IgnoreBlankLinesAndWhitespaceWithMovedBlocks)
{
	CDiffWrapper dw;
	DIFFOPTIONS options{};
	DiffList diffList;
	DIFFRANGE dr;

	options.nIgnoreWhitespace = 2;
	options.bIgnoreBlankLines = true;
	options.bCompletelyBlankOutIgnoredChanges = true;
	const TempFile left = WriteToTempFile(_T("\t\na\nbb\nc\n;"));
	const TempFile right = WriteToTempFile(_T("0\na\nbbb\n\t\t\n;"));

	dw.SetDetectMovedBlocks(true);
	dw.SetCreateDiffList(&diffList);
	dw.SetPaths({ left.GetPath(), right.GetPath() }, false);
	dw.SetOptions(&options);
	dw.RunFileDiff();

	ASSERT_EQ(3, diffList.GetSize());
	diffList.GetDiff(0, dr);
	EXPECT_EQ(OP_DIFF, dr.op);
	EXPECT_EQ(0, dr.begin[0]);
	EXPECT_EQ(0, dr.begin[1]);
	EXPECT_EQ(0, dr.end[0]);
	EXPECT_EQ(0, dr.end[1]);

	diffList.GetDiff(1, dr);
	EXPECT_EQ(OP_DIFF, dr.op);
	EXPECT_EQ(2, dr.begin[0]);
	EXPECT_EQ(2, dr.begin[1]);
	EXPECT_EQ(3, dr.end[0]);
	EXPECT_EQ(2, dr.end[1]);

	// The final ignored blank-line change is an insertion before the common
	// final line. The missing-newline adjustment must not move its range back
	// over the preceding diff.
	diffList.GetDiff(2, dr);
	EXPECT_EQ(OP_TRIVIAL, dr.op);
	EXPECT_EQ(4, dr.begin[0]);
	EXPECT_EQ(3, dr.begin[1]);
	EXPECT_EQ(3, dr.end[0]);
	EXPECT_EQ(3, dr.end[1]);
}

TEST(DiffWrapper, RunFileDiff_IgnoreCommentsWithMissingNewline)
{
	CDiffWrapper dw;
	DIFFOPTIONS options{};
	DIFFRANGE dr;
	options.bFilterCommentsLines = true;
	options.bCompletelyBlankOutIgnoredChanges = true;

	LangServices::SyntaxParserRegistry::GetInstance().RegisterFactory(&CrystalLineSyntaxParserFactory::GetInstance());
	dw.SetFilterCommentsSourceDef(_T("cpp"));

	{
		// The filtered comment is the final sub-hunk produced by PostFilter,
		// but a common suffix means that this sub-hunk does not reach EOF.
		DiffList diffList;
		const TempFile left = WriteToTempFile(_T("a\nb1\nc\n;"));
		const TempFile right = WriteToTempFile(_T("a\nb2\n/* ignored */\nc\n;"));
		dw.SetCreateDiffList(&diffList);
		dw.SetPaths({ left.GetPath(), right.GetPath() }, false);
		dw.SetOptions(&options);
		dw.RunFileDiff();

		EXPECT_EQ(2, diffList.GetSize());
		if (diffList.GetSize() == 2)
		{
			diffList.GetDiff(0, dr);
			EXPECT_EQ(OP_DIFF, dr.op);
			EXPECT_EQ(1, dr.begin[0]);
			EXPECT_EQ(1, dr.begin[1]);
			EXPECT_EQ(1, dr.end[0]);
			EXPECT_EQ(1, dr.end[1]);
			diffList.GetDiff(1, dr);
			EXPECT_EQ(OP_TRIVIAL, dr.op);
			EXPECT_EQ(2, dr.begin[0]);
			EXPECT_EQ(2, dr.begin[1]);
			EXPECT_EQ(1, dr.end[0]);
			EXPECT_EQ(2, dr.end[1]);
		}
	}

	{
		// When the filtered comment itself reaches EOF, the missing-newline
		// adjustment still removes it from the ignored-difference range.
		DiffList diffList;
		const TempFile left = WriteToTempFile(_T("a\nb1"));
		const TempFile right = WriteToTempFile(_T("a\nb2\n/* ignored */"));
		dw.SetCreateDiffList(&diffList);
		dw.SetPaths({ left.GetPath(), right.GetPath() }, false);
		dw.SetOptions(&options);
		dw.RunFileDiff();

		EXPECT_EQ(1, diffList.GetSize());
		if (diffList.GetSize() == 1)
		{
			diffList.GetDiff(0, dr);
			EXPECT_EQ(OP_DIFF, dr.op);
			EXPECT_EQ(1, dr.begin[0]);
			EXPECT_EQ(1, dr.begin[1]);
			EXPECT_EQ(1, dr.end[0]);
			EXPECT_EQ(1, dr.end[1]);
		}
	}

	LangServices::SyntaxParserRegistry::GetInstance().UnregisterFactory(&CrystalLineSyntaxParserFactory::GetInstance());
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

	LangServices::SyntaxParserRegistry::GetInstance().RegisterFactory(&CrystalLineSyntaxParserFactory::GetInstance());

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

	LangServices::SyntaxParserRegistry::GetInstance().UnregisterFactory(&CrystalLineSyntaxParserFactory::GetInstance());
}

TEST(DiffWrapper, RunFileDiff_IgnoreComments)
{
	CDiffWrapper dw;
	DIFFOPTIONS options{};
	DIFFRANGE dr;

	LangServices::SyntaxParserRegistry::GetInstance().RegisterFactory(&CrystalLineSyntaxParserFactory::GetInstance());

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

	LangServices::SyntaxParserRegistry::GetInstance().UnregisterFactory(&CrystalLineSyntaxParserFactory::GetInstance());
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

// Both sides add a block in the same place: that is one difference, and a
// conflict unless the blocks are the same. The two pairwise comparisons can
// place a block that ends like the text in front of it one line apart; the
// additions must not come out as two unrelated one-sided differences, which
// an automatic merge would take both.
TEST(DiffWrapper, RunFileDiff_3way_BothSidesAddInTheSamePlace)
{
	const String block0 = _T("comment 0\nmember 0\nresult\n\n");
	const String block1 = _T("comment 1\nmember 1\nresult\n\n");
	const String block2 = _T("comment 2\nmember 2\nresult\n\n");
	const String early = _T("comment e\nmember e\n\n");
	const String blockX = _T("comment x\nmember x\nresult\n\n");
	const String blockY = _T("comment y\nmember y\nother\n\n");

	for (auto algo : { DIFF_ALGORITHM_DEFAULT, DIFF_ALGORITHM_MINIMAL, DIFF_ALGORITHM_PATIENCE, DIFF_ALGORITHM_HISTOGRAM })
	for (bool movedBlocks : { false, true })
	{
		DIFFOPTIONS options{};
		options.nDiffAlgorithm = algo;

		{
			// the left side adds more than the right side: a conflict
			CDiffWrapper dw;
			DiffList diffList;
			DIFFRANGE dr;
			TempFile left = WriteToTempFile(block0 + early + block1 + blockX + blockY + block2);
			TempFile middle = WriteToTempFile(block0 + block1 + block2);
			TempFile right = WriteToTempFile(block0 + block1 + blockX + block2);
			dw.SetCreateDiffList(&diffList);
			dw.SetPaths({ left.GetPath(), middle.GetPath(), right.GetPath() }, false);
			dw.SetOptions(&options);
			dw.SetDetectMovedBlocks(movedBlocks);
			dw.RunFileDiff();
			ASSERT_EQ(2, diffList.GetSize());
			diffList.GetDiff(0, dr);
			EXPECT_EQ(OP_1STONLY, dr.op);
			diffList.GetDiff(1, dr);
			EXPECT_EQ(OP_DIFF, dr.op);
			EXPECT_EQ(8, dr.end[0] - dr.begin[0] + 1);
			EXPECT_EQ(0, dr.end[1] - dr.begin[1] + 1);
			EXPECT_EQ(4, dr.end[2] - dr.begin[2] + 1);
		}

		{
			// The left block can be at either side of the "}" line, the right
			// block only in front of it: they are in the same place
			CDiffWrapper dw;
			DiffList diffList;
			DIFFRANGE dr;
			TempFile left = WriteToTempFile(_T("x\n}\nfoo\n}\ny\n"));
			TempFile middle = WriteToTempFile(_T("x\n}\ny\n"));
			TempFile right = WriteToTempFile(_T("x\nbar\n}\ny\n"));
			dw.SetCreateDiffList(&diffList);
			dw.SetPaths({ left.GetPath(), middle.GetPath(), right.GetPath() }, false);
			dw.SetOptions(&options);
			dw.SetDetectMovedBlocks(movedBlocks);
			dw.RunFileDiff();
			ASSERT_EQ(1, diffList.GetSize());
			diffList.GetDiff(0, dr);
			EXPECT_EQ(OP_DIFF, dr.op);
		}

		{
			// both sides add the same block: not a conflict, and not two additions
			CDiffWrapper dw;
			DiffList diffList;
			DIFFRANGE dr;
			TempFile left = WriteToTempFile(block0 + early + block1 + blockX + block2);
			TempFile middle = WriteToTempFile(block0 + block1 + block2);
			TempFile right = WriteToTempFile(block0 + block1 + blockX + block2);
			dw.SetCreateDiffList(&diffList);
			dw.SetPaths({ left.GetPath(), middle.GetPath(), right.GetPath() }, false);
			dw.SetOptions(&options);
			dw.SetDetectMovedBlocks(movedBlocks);
			dw.RunFileDiff();
			ASSERT_EQ(2, diffList.GetSize());
			diffList.GetDiff(0, dr);
			EXPECT_EQ(OP_1STONLY, dr.op);
			diffList.GetDiff(1, dr);
			EXPECT_EQ(OP_2NDONLY, dr.op);
			EXPECT_EQ(4, dr.end[0] - dr.begin[0] + 1);
			EXPECT_EQ(0, dr.end[1] - dr.begin[1] + 1);
			EXPECT_EQ(4, dr.end[2] - dr.begin[2] + 1);
		}
	}
}
