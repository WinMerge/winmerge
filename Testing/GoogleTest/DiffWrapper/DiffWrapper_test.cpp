#include "pch.h"
#include <gtest/gtest.h>
#include "DiffWrapper.h"
#include "PathContext.h"
#include "paths.h"
#include "TempFile.h"
#include "UniFile.h"

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
