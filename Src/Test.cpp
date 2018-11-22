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
	PathContext tFiles = {
		paths::ConcatPath(projectRoot, L"Testing/Data/Unicode/UCS-2BE/DiffItem.h"),
		paths::ConcatPath(projectRoot, L"Testing/Data/Unicode/UCS-2LE/DiffItem.h")
	};
	EXPECT_TRUE(GetMainFrame()->DoFileOpen(&tFiles));
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
	PathContext tFiles = {
		paths::ConcatPath(projectRoot, L"Testing/Data/Unicode/UTF-8/DiffItem.h"),
		paths::ConcatPath(projectRoot, L"Testing/Data/Unicode/UTF-8-NOBOM/DiffItem.h")
	};
	EXPECT_TRUE(GetMainFrame()->DoFileOpen(&tFiles));
	CFrameWnd *pFrame = GetMainFrame()->GetActiveFrame();
	CMergeDoc *pDoc = dynamic_cast<CMergeDoc *>(pFrame->GetActiveDocument());
	EXPECT_NE(nullptr, pDoc);
	if (nullptr == pDoc)
		return;
	EXPECT_EQ(ucr::UTF8, pDoc->m_ptBuf[0]->getEncoding().m_unicoding);
	EXPECT_TRUE(pDoc->m_ptBuf[0]->getEncoding().m_bom);
	EXPECT_EQ(ucr::UTF8, pDoc->m_ptBuf[1]->getEncoding().m_unicoding);
	EXPECT_FALSE(pDoc->m_ptBuf[1]->getEncoding().m_bom);
	pFrame->PostMessage(WM_CLOSE);
}

TEST(SyntaxHighlight, Verilog)
{
	String projectRoot = getProjectRoot();
	PathContext tFiles = {
		paths::ConcatPath(projectRoot, L"Testing/FileFormats/Verilog.v"),
		paths::ConcatPath(projectRoot, L"Testing/FileFormats/Verilog.v")
	};
	CMessageBoxDialog dlg(nullptr, IDS_FILESSAME, 0U, 0U, IDS_FILESSAME);
	const int nPrevFormerResult = dlg.SetFormerResult(IDOK);
	EXPECT_TRUE(GetMainFrame()->DoFileOpen(&tFiles));
	CFrameWnd *pFrame = GetMainFrame()->GetActiveFrame();
	CMergeDoc *pDoc = dynamic_cast<CMergeDoc *>(pFrame->GetActiveDocument());
	EXPECT_NE(nullptr, pDoc);
	if (nullptr == pDoc)
		return;

	std::vector<CCrystalTextView::TEXTBLOCK> blocks;
	blocks = pDoc->GetView(0, 0)->GetTextBlocks(0);
	EXPECT_EQ(COLORINDEX_COMMENT, blocks[0].m_nColorIndex);
	blocks = pDoc->GetView(0, 0)->GetTextBlocks(2);
	EXPECT_EQ(COLORINDEX_KEYWORD, blocks[0].m_nColorIndex);
	blocks = pDoc->GetView(0, 0)->GetTextBlocks(37);
	EXPECT_EQ(COLORINDEX_PREPROCESSOR, blocks[0].m_nColorIndex);
	blocks = pDoc->GetView(0, 0)->GetTextBlocks(38);
	EXPECT_EQ(COLORINDEX_USER1, blocks[1].m_nColorIndex);
	EXPECT_EQ(COLORINDEX_STRING, blocks[3].m_nColorIndex);

	pFrame->PostMessage(WM_CLOSE);
	dlg.SetFormerResult(nPrevFormerResult);
}

TEST(FileCompare, LastLineEOL)
{
	String projectRoot = getProjectRoot();
	const static String filelist[] = {
		_T("0None.txt"),
		_T("0CRLF.txt"),
		_T("1None.txt"),
		_T("1CRLF.txt"),
		_T("2None.txt"),
		_T("2CRLF.txt"),
		_T("3None.txt"),
		_T("3CRLF.txt")
	};
	CMessageBoxDialog dlg(nullptr, IDS_FILESSAME, 0U, 0U, IDS_FILESSAME);
	const int nPrevFormerResult = dlg.SetFormerResult(IDOK);
	CMergeDoc *pDoc = nullptr;
	CFrameWnd *pFrame = nullptr;
	for (size_t l = 0; l < std::size(filelist); ++l)
	{
		for (size_t r = 0; r < std::size(filelist); ++r)
		{
			PathContext tFiles = {
				paths::ConcatPath(projectRoot, paths::ConcatPath(_T("Testing/Data/LastLineEOL"), filelist[l])),
				paths::ConcatPath(projectRoot, paths::ConcatPath(_T("Testing/Data/LastLineEOL"), filelist[r])),
			};
			if (l == 0 && r == 0)
			{
				EXPECT_TRUE(GetMainFrame()->DoFileOpen(&tFiles));
				pFrame = GetMainFrame()->GetActiveFrame();
				pDoc = dynamic_cast<CMergeDoc *>(pFrame->GetActiveDocument());
				EXPECT_NE(nullptr, pDoc);
				if (pDoc == nullptr)
					return;
			}
			else if (r == 0)
			{
				pDoc->ChangeFile(0, tFiles[0]);
			}
			pDoc->ChangeFile(1, tFiles[1]);

			if (tFiles[0] == tFiles[1])
			{
				EXPECT_EQ(0, pDoc->m_diffList.GetSize());
			}
			else
			{
				CMergeEditView *pView = pDoc->GetView(0, 0);
				// merge
				EXPECT_EQ(1, pDoc->m_diffList.GetSize());
				pDoc->CopyAllList(0, 1);
				EXPECT_EQ(0, pDoc->m_diffList.GetSize());
				// undo
				pView->SendMessage(WM_COMMAND, ID_EDIT_UNDO);
				EXPECT_EQ(1, pDoc->m_diffList.GetSize());
				// insert a character at the last line
				pView->GotoLine(pDoc->m_ptBuf[0]->GetLineCount() - 1, false, 0);
				pView->SendMessage(WM_CHAR, 'a');
				// undo
				pView->SendMessage(WM_COMMAND, ID_EDIT_UNDO);
			}
		}
	}
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
		EXPECT_TRUE(GetMainFrame()->DoFileOpen(&dirs));
		CFrameWnd *pFrame = GetMainFrame()->GetActiveFrame();
		CDirDoc *pDoc = dynamic_cast<CDirDoc *>(pFrame->GetActiveDocument());
		EXPECT_NE(nullptr, pDoc);
		if (nullptr == pDoc)
		{
			pFrame->PostMessage(WM_CLOSE);
			continue;
		}
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
	pDoc->m_ptBuf[1]->SetModified(false);
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
	pDoc->m_ptBuf[1]->SetModified(false);
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
	pDoc->m_ptBuf[1]->SetModified(false);
	pFrame->PostMessage(WM_CLOSE);
}

TEST(ImageCompareTest, Open)
{
	EXPECT_TRUE(CImgMergeFrame::IsLoadable());
	if (!CImgMergeFrame::IsLoadable())
		return;

	String projectRoot = getProjectRoot();
	PathContext tFiles = {
		paths::ConcatPath(projectRoot, L"Src/res/right_to_middle.bmp"),
		paths::ConcatPath(projectRoot, L"Src/res/right_to_left.bmp")
	};
	CMessageBoxDialog dlg(nullptr, IDS_FILESSAME, 0U, 0U, IDS_FILESSAME);
	const int nPrevFormerResult = dlg.SetFormerResult(IDOK);
	EXPECT_TRUE(GetMainFrame()->DoFileOpen(&tFiles));
	CFrameWnd *pFrame = GetMainFrame()->GetActiveFrame();
	CImgMergeFrame *pDoc = dynamic_cast<CImgMergeFrame *>(pFrame);
	EXPECT_NE(nullptr, pDoc);

	pFrame->PostMessage(WM_CLOSE);
	dlg.SetFormerResult(nPrevFormerResult);
}

TEST(FileMenu, New)
{
	CFrameWnd *pFrame;
	GetMainFrame()->FileNew(2);
	pFrame = GetMainFrame()->GetActiveFrame();
	pFrame->PostMessage(WM_CLOSE);
	GetMainFrame()->FileNew(3);
	pFrame = GetMainFrame()->GetActiveFrame();
	pFrame->PostMessage(WM_CLOSE);
}

TEST(FileMenu, OpenConflictFile)
{
	String conflictFile = paths::ConcatPath(getProjectRoot(), L"Testing/Data/big_file.conflict");
	GetMainFrame()->DoOpenConflict(conflictFile);
	CFrameWnd *pFrame = GetMainFrame()->GetActiveFrame();
	CMergeDoc *pDoc = dynamic_cast<CMergeDoc *>(pFrame->GetActiveDocument());
	ASSERT_NE(nullptr, pDoc);
	pDoc->m_ptBuf[1]->SetModified(false);
	pFrame->PostMessage(WM_CLOSE);
}

TEST(FileMenu, OpenConflictFile3)
{
	String conflictFile = paths::ConcatPath(getProjectRoot(), L"Testing/Data/dif3.conflict");
	GetMainFrame()->DoOpenConflict(conflictFile);
	CFrameWnd *pFrame = GetMainFrame()->GetActiveFrame();
	CMergeDoc *pDoc = dynamic_cast<CMergeDoc *>(pFrame->GetActiveDocument());
	ASSERT_NE(nullptr, pDoc);
	pDoc->m_ptBuf[2]->SetModified(false);
	pFrame->PostMessage(WM_CLOSE);
}

TEST(FileMenu, OpenProject)
{
	String projectFile = paths::ConcatPath(getProjectRoot(), L"Testing/Data/Dir2.WinMerge");
	SetCurrentDirectory(paths::GetParentPath(projectFile).c_str());
	theApp.LoadAndOpenProjectFile(projectFile);
	CFrameWnd *pFrame = GetMainFrame()->GetActiveFrame();
	EXPECT_NE(nullptr, pFrame);
	if (pFrame)
		pFrame->PostMessage(WM_CLOSE);
}

TEST(FileMenu, OpenProject3)
{
	String projectFile = paths::ConcatPath(getProjectRoot(), L"Testing/Data/Dir3.WinMerge");
	SetCurrentDirectory(paths::GetParentPath(projectFile).c_str());
	theApp.LoadAndOpenProjectFile(projectFile);
	CFrameWnd *pFrame = GetMainFrame()->GetActiveFrame();
	EXPECT_NE(nullptr, pFrame);
	if (pFrame)
		pFrame->PostMessage(WM_CLOSE);
}

#endif