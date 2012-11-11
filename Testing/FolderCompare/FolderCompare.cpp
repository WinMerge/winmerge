#include "DiffContext.h"
#include "CompareStats.h"
#include "DiffThread.h"
#include "DiffWrapper.h"
#include "FileFilterHelper.h"
#include <Poco/Thread.h>

int main()
{
	CompareStats cmpstats(2);

	FileFilterHelper filter;
	filter.UseMask(true);
	filter.SetMask(_T("*.cpp;*.c;*.h"));

	CDiffContext ctx(
		PathContext(_T("d:/dev/winmerge/winmerge-3pane/winmerge-v2/Src"), _T("d:/dev/winmerge/winmerge-3pane/trunk/Src")),
		CMP_CONTENT);

	DIFFOPTIONS options = {0};
	options.nIgnoreWhitespace = false;
	options.bIgnoreBlankLines = false;
	options.bFilterCommentsLines = false;
	options.bIgnoreCase = false;
	options.bIgnoreEol = false;

	ctx.CreateCompareOptions(CMP_CONTENT, options);

	ctx.m_iGuessEncodingType = (50001 << 16) + 2;
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

	Poco::UIntPtr pos = ctx.GetFirstDiffPosition();
	while (pos)
	{
		const DIFFITEM& di = ctx.GetNextDiffPosition(pos);
#ifdef _UNICODE
		std::wcout << di.diffFileInfo[0].filename << ":" << di.diffcode.isResultDiff() << std::endl;
#else
		std::cout << di.diffFileInfo[0].filename << ":" << di.diffcode.isResultDiff() << std::endl;
#endif
	}

	return 0;
}