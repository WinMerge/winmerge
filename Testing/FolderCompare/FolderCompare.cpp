#include "pch.h"
#include "DiffContext.h"
#include "CompareStats.h"
#include "DiffThread.h"
#include "DiffWrapper.h"
#include "FileFilterHelper.h"
#include "FolderCmp.h"
#include "DirScan.h"
#include "paths.h"
#include <iostream>
#include <Poco/Thread.h>
#ifdef _MSC_VER
#include <crtdbg.h>
#endif

std::vector<std::wstring> ParseQuotedArgs(const std::wstring& input)
{
	std::vector<std::wstring> tokens;
	std::wstring current;
	bool inQuotes = false;

	for (size_t i = 0; i < input.length(); ++i)
	{
		wchar_t ch = input[i];
		if (ch == L'"')
		{
			inQuotes = !inQuotes;
			if (!inQuotes && !current.empty())
			{
				tokens.push_back(current);
				current.clear();
			}
		}
		else if (iswspace(ch) && !inQuotes)
		{
			if (!current.empty())
			{
				tokens.push_back(current);
				current.clear();
			}
		}
		else
		{
			current += ch;
		}
	}

	if (!current.empty())
		tokens.push_back(current);

	return tokens;
}

int main()
{
#ifdef _MSC_VER
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	int dm = CMP_CONTENT; // Default compare method
	PathContext paths(_T(""), _T("")); // Default empty paths
	FileFilterHelper filter;
	filter.UseMask(true);
	filter.SetMask(_T("*.*"));

	std::wcout << L"WinMerge folder comparison test tool\n";
	std::wcout << L"Type 'h' for help.\n";

	std::wstring cmd;
	while (true)
	{
		std::wcout << L"> ";
		std::getline(std::wcin, cmd);
		if (cmd.empty())
			continue;

		if (cmd[0] == L'q')
		{
			break;
		}
		if (cmd[0] == L'h')
		{
			std::wcout << L"\nAvailable commands:\n";
			std::wcout << L"  p <left-path> <right-path>   : Set folder paths to compare\n";
			std::wcout << L"  f <filter-mask>              : Set file mask filter (e.g., *.c;*.h)\n";
			std::wcout << L"  m <compare-method>           : Set compare method (FullContents, Date, etc.)\n";
			std::wcout << L"  c                            : Start folder comparison\n";
			std::wcout << L"  q                            : Quit the program\n";
			std::wcout << L"  h                            : Show this help message\n\n";
		}
		else if (cmd[0] == L'p') // Set path
		{
			std::vector<std::wstring> args = ParseQuotedArgs(cmd.substr(2));
			if (args.size() == 2 || args.size() == 3)
			{
				paths = PathContext(args);
			}
			else
			{
				std::wcout << L"Usage: p \"left_path\" \"right_path\"\n";
			}
		}
		else if (cmd[0] == L'f') // Set file mask
		{
			std::wstring mask = cmd.substr(2);
			filter.SetMask(mask.c_str());
		}
		else if (cmd[0] == L'm') // Set method
		{
			std::wstring method = cmd.substr(2);
			if (method == L"FullContents") dm = CMP_CONTENT;
			else if (method == L"QuickContents") dm = CMP_QUICK_CONTENT;
			else if (method == L"BinaryContents") dm = CMP_BINARY_CONTENT;
			else if (method == L"Date") dm = CMP_DATE;
			else if (method == L"DateSize") dm = CMP_DATE_SIZE;
			else if (method == L"Size") dm = CMP_SIZE;
			else {
				std::wcout << L"Unknown compare method\n";
				continue;
			}
		}
		else if (cmd[0] == L'c') // Compare
		{
			CompareStats cmpstats(paths.GetSize());

			CDiffContext ctx(paths, dm);

			DIFFOPTIONS options = {0};
			options.nIgnoreWhitespace = false;
			options.bIgnoreBlankLines = false;
			options.bFilterCommentsLines = false;
			options.bIgnoreCase = false;
			options.bIgnoreEol = false;
			
			ctx.InitDiffItemList();
			ctx.CreateCompareOptions(dm, options);
			ctx.m_iGuessEncodingType = 0;
			ctx.m_bIgnoreSmallTimeDiff = true;
			ctx.m_bStopAfterFirstDiff = false;
			ctx.m_nQuickCompareLimit = 4 * 1024 * 1024;
			ctx.m_bPluginsEnabled = false;
			ctx.m_bWalkUniques = true;
			ctx.m_pCompareStats = &cmpstats;
			ctx.m_bRecursive = true;
			ctx.m_piFilterGlobal = &filter;

			CDiffThread diffThread;
			diffThread.SetContext(&ctx);
			diffThread.SetCollectFunction([](DiffFuncStruct* myStruct) {
				bool casesensitive = false;
				int depth = myStruct->context->m_bRecursive ? -1 : 0;
				PathContext paths = myStruct->context->GetNormalizedPaths();
				String subdir[3] = { _T(""), _T(""), _T("") };
				DirScan_GetItems(paths, subdir, myStruct,
					casesensitive, depth, nullptr, myStruct->context->m_bWalkUniques);
				});
			diffThread.SetCompareFunction([](DiffFuncStruct* myStruct) {
				DirScan_CompareItems(myStruct, nullptr);
				});
			diffThread.CompareDirectories();

			while (diffThread.GetThreadState() != CDiffThread::THREAD_COMPLETED)
			{
				Poco::Thread::sleep(200);
				std::wcout << L"Comparing " << cmpstats.GetComparedItems() << L" items...\r";
			}
			std::wcout << L"\nComparison completed.\n";

			DIFFITEM* pos = ctx.GetFirstDiffPosition();
			while (pos)
			{
				DIFFITEM& di = ctx.GetNextDiffRefPosition(pos);
				if ((paths.GetSize() == 2 && ctx.m_piFilterGlobal->includeFile(
						paths::ConcatPath(di.diffFileInfo[0].path, di.diffFileInfo[0].filename), 
						paths::ConcatPath(di.diffFileInfo[1].path, di.diffFileInfo[1].filename))
					||
					(paths.GetSize() == 3 && ctx.m_piFilterGlobal->includeFile(
						paths::ConcatPath(di.diffFileInfo[0].path, di.diffFileInfo[0].filename), 
						paths::ConcatPath(di.diffFileInfo[1].path, di.diffFileInfo[1].filename),
						paths::ConcatPath(di.diffFileInfo[1].path, di.diffFileInfo[2].filename)))))
				{
					FolderCmp folderCmp(&ctx);
					folderCmp.prepAndCompareFiles(di);
					if (di.diffcode.isResultDiff())
						std::wcout << di.diffFileInfo[0].filename << L": " << L"Different" << std::endl;
				}
			}
		}
		else
		{
			std::wcout << L"Unknown command: " << cmd << std::endl;
		}
	}

	return 0;
}
