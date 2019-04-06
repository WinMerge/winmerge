#include "pch.h"
#include "DiffContext.h"
#include "CompareStats.h"
#include "DiffThread.h"
#include "DiffWrapper.h"
#include "FileFilterHelper.h"
#include "FolderCmp.h"
#include <iostream>
#include <Poco/Thread.h>
#ifdef _MSC_VER
#include <crtdbg.h>
#endif

int main()
{
#ifdef _MSC_VER
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	CompareStats cmpstats(2);

	FileFilterHelper filter;
	filter.UseMask(true);
//	filter.SetMask(_T("*.cpp;*.c;*.h;*.vcproj;*.vcxproj"));
	filter.SetMask(_T("*.*"));

	CDiffContext ctx(
		PathContext(_T("c:/windows"), _T("c:/windows")),
		CMP_CONTENT);

	DIFFOPTIONS options = {0};
	options.nIgnoreWhitespace = false;
	options.bIgnoreBlankLines = false;
	options.bFilterCommentsLines = false;
	options.bIgnoreCase = false;
	options.bIgnoreEol = false;

	ctx.InitDiffItemList();
	//ctx.CreateCompareOptions(CMP_CONTENT, options);
	ctx.CreateCompareOptions(CMP_DATE, options);

	ctx.m_iGuessEncodingType = 0;//(50001 << 16) + 2;
	ctx.m_bIgnoreSmallTimeDiff = true;
	ctx.m_bStopAfterFirstDiff = false;
	ctx.m_nQuickCompareLimit = 4 * 1024 * 1024;
	ctx.m_bPluginsEnabled = false;
	ctx.m_bWalkUniques = true;
	ctx.m_pCompareStats = &cmpstats;
	ctx.m_bRecursive = true;
	ctx.m_piFilterGlobal = &filter;

	// Folder names to compare are in the compare context
	CDiffThread diffThread;
	diffThread.SetContext(&ctx);
	diffThread.SetCompareSelected(false);
	diffThread.CompareDirectories();

	while (diffThread.GetThreadState() != CDiffThread::THREAD_COMPLETED)
	{
		Poco::Thread::sleep(200);
		std::cout << cmpstats.GetComparedItems() << std::endl;
	}

	DIFFITEM *pos = ctx.GetFirstDiffPosition();
	while (pos)
	{
		DIFFITEM& di = ctx.GetNextDiffRefPosition(pos);
		if (ctx.m_piFilterGlobal->includeFile(di.diffFileInfo[0].filename, di.diffFileInfo[1].filename))
		{
			FolderCmp folderCmp;
			folderCmp.prepAndCompareFiles(&ctx, di);
#ifdef _UNICODE
//		std::wcout << di.diffFileInfo[0].filename << ":" << di.diffcode.isResultDiff() << std::endl;
#else
//		std::cout << di.diffFileInfo[0].filename << ":" << di.diffcode.isResultDiff() << std::endl;
#endif
		}

	}

	return 0;
}
