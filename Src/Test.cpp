#include "StdAfx.h"
#ifdef TEST_WINMERGE
#include "Merge.h"
#include "Environment.h"
#include "paths.h"
#include "MainFrm.h"
#include "MergeDoc.h"
#include "ccrystaltextview.h"
#include "MergeEditView.h"
#include "DirDoc.h"
#include "DirView.h"
#include "ImgMergeFrm.h"
#include "DiffContext.h"
#include "CompareStats.h"
#include "unicoder.h"
#include "OptionsMgr.h"
#include "OptionsDef.h"
#include "SyntaxColors.h"
#include "MergeCmdLineInfo.h"
#include "gtest/gtest.h"

String getProjectRoot()
{
#ifdef _WIN64
	return paths::ConcatPath(env::GetProgPath(), L"../../..");
#else
	return paths::ConcatPath(env::GetProgPath(), L"../../");
#endif
}

TEST(CodepageTest, UCS2)
{
	String projectRoot = getProjectRoot();
	PathContext files = {
		paths::ConcatPath(projectRoot, L"Testing/Data/Unicode/UCS-2BE/DiffItem.h"),
		paths::ConcatPath(projectRoot, L"Testing/Data/Unicode/UCS-2LE/DiffItem.h")
	};
	EXPECT_TRUE(!!GetMainFrame()->DoFileOpen(&files));
	CFrameWnd *pFrame = GetMainFrame()->GetActiveFrame();
	CMergeDoc *pDoc = dynamic_cast<CMergeDoc *>(pFrame->GetActiveDocument());
	ASSERT_NE(nullptr, pDoc);
	EXPECT_EQ(ucr::UCS2BE, pDoc->m_ptBuf[0]->getEncoding().m_unicoding);
	EXPECT_TRUE(pDoc->m_ptBuf[0]->getEncoding().m_bom);
	EXPECT_EQ(ucr::UCS2LE, pDoc->m_ptBuf[1]->getEncoding().m_unicoding);
	EXPECT_TRUE(pDoc->m_ptBuf[1]->getEncoding().m_bom);
	pFrame->PostMessage(WM_CLOSE);
}

TEST(CodepageTest, UTF8)
{
	String projectRoot = getProjectRoot();
	PathContext files = {
		paths::ConcatPath(projectRoot, L"Testing/Data/Unicode/UTF-8/DiffItem.h"),
		paths::ConcatPath(projectRoot, L"Testing/Data/Unicode/UTF-8-NOBOM/DiffItem.h")
	};
	EXPECT_TRUE(!!GetMainFrame()->DoFileOpen(&files));
	CFrameWnd *pFrame = GetMainFrame()->GetActiveFrame();
	CMergeDoc *pDoc = dynamic_cast<CMergeDoc *>(pFrame->GetActiveDocument());
	EXPECT_NE(nullptr, pDoc);
	EXPECT_EQ(ucr::UTF8, pDoc->m_ptBuf[0]->getEncoding().m_unicoding);
	EXPECT_TRUE(pDoc->m_ptBuf[0]->getEncoding().m_bom);
	EXPECT_EQ(ucr::UTF8, pDoc->m_ptBuf[1]->getEncoding().m_unicoding);
	EXPECT_FALSE(pDoc->m_ptBuf[1]->getEncoding().m_bom);
	pFrame->PostMessage(WM_CLOSE);
}

TEST(SyntaxHighlight, Verilog)
{
	String projectRoot = getProjectRoot();
	PathContext files = {
		paths::ConcatPath(projectRoot, L"Testing/FileFormats/Verilog.v"),
		paths::ConcatPath(projectRoot, L"Testing/FileFormats/Verilog.v")
	};
	CMessageBoxDialog dlg(nullptr, IDS_FILESSAME, 0U, 0U, IDS_FILESSAME);
	const int nPrevFormerResult = dlg.SetFormerResult(IDOK);
	EXPECT_TRUE(!!GetMainFrame()->DoFileOpen(&files));
	CFrameWnd *pFrame = GetMainFrame()->GetActiveFrame();
	CMergeDoc *pDoc = dynamic_cast<CMergeDoc *>(pFrame->GetActiveDocument());
	EXPECT_NE(nullptr, pDoc);

	std::vector<CCrystalTextView::TEXTBLOCK> blocks;
	blocks = pDoc->GetView(0)->GetTextBlocks(0);
	EXPECT_EQ(COLORINDEX_COMMENT, blocks[0].m_nColorIndex);
	blocks = pDoc->GetView(0)->GetTextBlocks(2);
	EXPECT_EQ(COLORINDEX_KEYWORD, blocks[0].m_nColorIndex);
	blocks = pDoc->GetView(0)->GetTextBlocks(37);
	EXPECT_EQ(COLORINDEX_PREPROCESSOR, blocks[0].m_nColorIndex);
	blocks = pDoc->GetView(0)->GetTextBlocks(38);
	EXPECT_EQ(COLORINDEX_USER1, blocks[1].m_nColorIndex);
	EXPECT_EQ(COLORINDEX_STRING, blocks[3].m_nColorIndex);

	pFrame->PostMessage(WM_CLOSE);
	dlg.SetFormerResult(nPrevFormerResult);
}

TEST(FolderCompare, IgnoreEOL)
{
	String projectRoot = getProjectRoot();
	PathContext dirsArray[] = {
		{
			paths::ConcatPath(projectRoot, L"Testing/selftests/u/"),
			paths::ConcatPath(projectRoot, L"Testing/selftests/m/")
		},
		{
			paths::ConcatPath(projectRoot, L"Testing/selftests/w/"),
			paths::ConcatPath(projectRoot, L"Testing/selftests/u/")
		},
		{
			paths::ConcatPath(projectRoot, L"Testing/selftests/w/"),
			paths::ConcatPath(projectRoot, L"Testing/selftests/m/")
		}
	};

	for (auto& dirs : dirsArray)
	{
		GetOptionsMgr()->Set(OPT_CMP_METHOD, 0/* Full Contents*/);
		GetOptionsMgr()->Set(OPT_CMP_IGNORE_EOL, true);
		EXPECT_TRUE(!!GetMainFrame()->DoFileOpen(&dirs));
		CFrameWnd *pFrame = GetMainFrame()->GetActiveFrame();
		CDirDoc *pDoc = dynamic_cast<CDirDoc *>(pFrame->GetActiveDocument());
		EXPECT_NE(nullptr, pDoc);
		CDirView *pView = pDoc->GetMainView();
		const CDiffContext& ctxt = pDoc->GetDiffContext();
		const CompareStats *pStats = ctxt.m_pCompareStats;
		do { Sleep(100); } while (!pStats->IsCompareDone());
		EXPECT_EQ(10, pStats->GetTotalItems());
		EXPECT_EQ(10, pStats->GetCount(CompareStats::RESULT::RESULT_SAME));
		uintptr_t pos = ctxt.GetFirstDiffPosition();
		while (pos)
		{
			const DIFFITEM& di = ctxt.GetNextDiffPosition(pos);
			for (int i = 0; i < 2; ++i)
			{
				if (dirs[i].find(L"/w/") != String::npos)
				{
					EXPECT_LT(0, di.diffFileInfo[i].m_textStats.ncrlfs);
					EXPECT_EQ(0, di.diffFileInfo[i].m_textStats.nlfs);
					EXPECT_EQ(0, di.diffFileInfo[i].m_textStats.ncrs);
				}
				else if (dirs[i].find(L"/u/") != String::npos)
				{
					EXPECT_LT(0, di.diffFileInfo[i].m_textStats.nlfs);
					EXPECT_EQ(0, di.diffFileInfo[i].m_textStats.ncrlfs);
					EXPECT_EQ(0, di.diffFileInfo[i].m_textStats.ncrs);
				}
				else if (dirs[i].find(L"/m/") != String::npos)
				{
					EXPECT_LT(0, di.diffFileInfo[i].m_textStats.ncrs);
					EXPECT_EQ(0, di.diffFileInfo[i].m_textStats.nlfs);
					EXPECT_EQ(0, di.diffFileInfo[i].m_textStats.ncrlfs);
				}
			}
		}

		GetOptionsMgr()->Set(OPT_CMP_METHOD, 0/* Full Contents*/);
		GetOptionsMgr()->Set(OPT_CMP_IGNORE_EOL, false);
		pDoc->Rescan();
		do { Sleep(100); } while (!pStats->IsCompareDone());
		EXPECT_EQ(10, pStats->GetTotalItems());
		EXPECT_EQ(10, pStats->GetCount(CompareStats::RESULT::RESULT_DIFF));

		GetOptionsMgr()->Set(OPT_CMP_METHOD, 1/* Quick Contents*/);
		GetOptionsMgr()->Set(OPT_CMP_IGNORE_EOL, true);
		pDoc->Rescan();
		do { Sleep(100); } while (!pStats->IsCompareDone());
		EXPECT_EQ(10, pStats->GetTotalItems());
		EXPECT_EQ(10, pStats->GetCount(CompareStats::RESULT::RESULT_SAME));

		GetOptionsMgr()->Set(OPT_CMP_METHOD, 1/* Quick Contents*/);
		GetOptionsMgr()->Set(OPT_CMP_IGNORE_EOL, false);
		pDoc->Rescan();
		do { Sleep(100); } while (!pStats->IsCompareDone());
		EXPECT_EQ(10, pStats->GetTotalItems());
		EXPECT_EQ(10, pStats->GetCount(CompareStats::RESULT::RESULT_DIFF));

		pFrame->PostMessage(WM_CLOSE);
	}
}

TEST(CommandLineTest, Desc)
{
	String projectRoot = getProjectRoot();
	MergeCmdLineInfo cmdInfo((String(GetCommandLineW()) + L" /dl TestL /dr TestR " + 
		paths::ConcatPath(projectRoot, L"Testing/Data/Unicode/UCS-2BE/DiffItem.h") + L" " + 
		paths::ConcatPath(projectRoot, L"Testing/Data/Unicode/UCS-2LE/DiffItem.h")).c_str()
	);
	theApp.ParseArgsAndDoOpen(cmdInfo, GetMainFrame());
	CFrameWnd *pFrame = GetMainFrame()->GetActiveFrame();
	CMergeDoc *pDoc = dynamic_cast<CMergeDoc *>(pFrame->GetActiveDocument());
	ASSERT_NE(nullptr, pDoc);
	EXPECT_EQ(L"TestL", pDoc->GetDescription(0));
	EXPECT_EQ(L"TestR", pDoc->GetDescription(1));
	pFrame->PostMessage(WM_CLOSE);
}

TEST(CommandLineTest, Desc2)
{
	String projectRoot = getProjectRoot();
	MergeCmdLineInfo cmdInfo((String(GetCommandLineW()) + L" /dl TestL /dr TestR " + 
		paths::ConcatPath(projectRoot, L"Testing/Data/big_file.conflict")).c_str()
	);
	theApp.ParseArgsAndDoOpen(cmdInfo, GetMainFrame());
	CFrameWnd *pFrame = GetMainFrame()->GetActiveFrame();
	CMergeDoc *pDoc = dynamic_cast<CMergeDoc *>(pFrame->GetActiveDocument());
	ASSERT_NE(nullptr, pDoc);
	EXPECT_EQ(L"TestL", pDoc->GetDescription(0));
	EXPECT_EQ(L"TestR", pDoc->GetDescription(1));
	pFrame->PostMessage(WM_CLOSE);
}

TEST(CommandLineTest, Desc3)
{
	String projectRoot = getProjectRoot();
	MergeCmdLineInfo cmdInfo((String(GetCommandLineW()) + L" /dr TestR " + 
		paths::ConcatPath(projectRoot, L"Testing/Data/big_file.conflict")).c_str()
	);
	theApp.ParseArgsAndDoOpen(cmdInfo, GetMainFrame());
	CFrameWnd *pFrame = GetMainFrame()->GetActiveFrame();
	CMergeDoc *pDoc = dynamic_cast<CMergeDoc *>(pFrame->GetActiveDocument());
	ASSERT_NE(nullptr, pDoc);
	EXPECT_EQ(L"Theirs File", pDoc->GetDescription(0));
	EXPECT_EQ(L"TestR", pDoc->GetDescription(1));
	pFrame->PostMessage(WM_CLOSE);
}

TEST(CommandLineTest, Desc4)
{
	String projectRoot = getProjectRoot();
	MergeCmdLineInfo cmdInfo((String(GetCommandLineW()) + L" /dl TestL " + 
		paths::ConcatPath(projectRoot, L"Testing/Data/big_file.conflict")).c_str()
	);
	theApp.ParseArgsAndDoOpen(cmdInfo, GetMainFrame());
	CFrameWnd *pFrame = GetMainFrame()->GetActiveFrame();
	CMergeDoc *pDoc = dynamic_cast<CMergeDoc *>(pFrame->GetActiveDocument());
	ASSERT_NE(nullptr, pDoc);
	EXPECT_EQ(L"TestL", pDoc->GetDescription(0));
	EXPECT_EQ(L"Mine File", pDoc->GetDescription(1));
	pFrame->PostMessage(WM_CLOSE);
}

TEST(ImageCompareTest, Open)
{
	String projectRoot = getProjectRoot();
	PathContext files = {
		paths::ConcatPath(projectRoot, L"Src/res/right_to_middle.bmp"),
		paths::ConcatPath(projectRoot, L"Src/res/right_to_left.bmp")
	};
	CMessageBoxDialog dlg(nullptr, IDS_FILESSAME, 0U, 0U, IDS_FILESSAME);
	const int nPrevFormerResult = dlg.SetFormerResult(IDOK);
	EXPECT_TRUE(!!GetMainFrame()->DoFileOpen(&files));
	CFrameWnd *pFrame = GetMainFrame()->GetActiveFrame();
	CImgMergeFrame *pDoc = dynamic_cast<CImgMergeFrame *>(pFrame);
	EXPECT_NE(nullptr, pDoc);

	pFrame->PostMessage(WM_CLOSE);
	dlg.SetFormerResult(nPrevFormerResult);

}

#endif