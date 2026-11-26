#include "pch.h"
#include "DiffContext.h"
#include "CompareStats.h"
#include "DiffThread.h"
#include "DiffWrapper.h"
#include "FileFilterHelper.h"
#include "FilterErrorMessages.h"
#include "DirScan.h"
#include "paths.h"
#include "DirItem.h"
#include "DirViewColItems.h"
#include "RenameMoveDetection.h"
#include "FilterExpression.h"
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

std::wstring GetColumnDisplay(const DirViewColItems& colItems, CDiffContext* pCtxt, const DIFFITEM* pdi, const String& columnName)
{
	for (int col = 0; col < colItems.GetColCount(); ++col)
	{
		const DirColInfo* colInfo = colItems.GetDirColInfo(col);
		if (colInfo && colInfo->regName == columnName)
		{
			return colItems.ColGetTextToDisplay(pCtxt, col, *pdi);
		}
	}
	return L"";
}

void PrintDiffItemTree(const DirViewColItems& colItems, CDiffContext* pCtxt, DIFFITEM* pdi, int indent, int nDirs, bool showDetails = false)
{
	if (!pdi)
		return;

	std::wstring indentStr(indent * 2, L' ');

	// Get item name and status using DirViewColItems
	std::wstring itemName = GetColumnDisplay(colItems, pCtxt, pdi, _T("Name"));
	std::wstring status = GetColumnDisplay(colItems, pCtxt, pdi, _T("StatusAbbr"));
	std::wstring typeStr = pdi->diffcode.isDirectory() ? L"[DIR] " : L"[FILE]";
	std::wcout << indentStr << typeStr << L" " << itemName << L": " << status;

	if (showDetails)
	{
		std::wcout << L" | ";

		// Show size information using DirViewColItems
		if (!pdi->diffcode.isDirectory())
		{
			if (nDirs < 3)
			{
				if (pdi->diffcode.existsFirst())
				{
					std::wstring size = GetColumnDisplay(colItems, pCtxt, pdi, _T("LsizeShort"));
					if (!size.empty())
						std::wcout << L"L:" << size << L" ";
				}
				if (pdi->diffcode.existsSecond())
				{
					std::wstring size = GetColumnDisplay(colItems, pCtxt, pdi, _T("RsizeShort"));
					if (!size.empty())
						std::wcout << L"R:" << size << L" ";
				}
			}
			else
			{
				if (pdi->diffcode.existsFirst())
				{
					std::wstring size = GetColumnDisplay(colItems, pCtxt, pdi, _T("LsizeShort"));
					if (!size.empty())
						std::wcout << L"L:" << size << L" ";
				}
				if (pdi->diffcode.existsSecond())
				{
					std::wstring size = GetColumnDisplay(colItems, pCtxt, pdi, _T("MsizeShort"));
					if (!size.empty())
						std::wcout << L"M:" << size << L" ";
				}
				if (pdi->diffcode.existsThird())
				{
					std::wstring size = GetColumnDisplay(colItems, pCtxt, pdi, _T("RsizeShort"));
					if (!size.empty())
						std::wcout << L"R:" << size << L" ";
				}
			}
			std::wcout << L"| ";
		}

		// Show date information using DirViewColItems
		if (nDirs < 3)
		{
			if (pdi->diffcode.existsFirst())
			{
				std::wstring date = GetColumnDisplay(colItems, pCtxt, pdi, _T("Lmtime"));
				if (!date.empty())
					std::wcout << L"L:" << date << L" ";
			}
			if (pdi->diffcode.existsSecond())
			{
				std::wstring date = GetColumnDisplay(colItems, pCtxt, pdi, _T("Rmtime"));
				if (!date.empty())
					std::wcout << L"R:" << date << L" ";
			}
		}
		else
		{
			if (pdi->diffcode.existsFirst())
			{
				std::wstring date = GetColumnDisplay(colItems, pCtxt, pdi, _T("Lmtime"));
				if (!date.empty())
					std::wcout << L"L:" << date << L" ";
			}
			if (pdi->diffcode.existsSecond())
			{
				std::wstring date = GetColumnDisplay(colItems, pCtxt, pdi, _T("Mmtime"));
				if (!date.empty())
					std::wcout << L"M:" << date << L" ";
			}
			if (pdi->diffcode.existsThird())
			{
				std::wstring date = GetColumnDisplay(colItems, pCtxt, pdi, _T("Rmtime"));
				if (!date.empty())
					std::wcout << L"R:" << date << L" ";
			}
		}

		// Show diff counts using DirViewColItems
		if (pdi->diffcode.isResultDiff() && !pdi->diffcode.isDirectory())
		{
			std::wstring diffs = GetColumnDisplay(colItems, pCtxt, pdi, _T("Snsdiffs"));
			if (!diffs.empty())
				std::wcout << L"| Diffs:" << diffs;
			std::wstring ignoredDiffs = GetColumnDisplay(colItems, pCtxt, pdi, _T("Snidiffs"));
			if (!ignoredDiffs.empty() && ignoredDiffs != _T("0"))
				std::wcout << L" Ignored:" << ignoredDiffs;
		}
	}

	std::wcout << std::endl;

	DIFFITEM* child = pdi->GetFirstChild();
	while (child)
	{
		PrintDiffItemTree(colItems, pCtxt, child, indent + 1, nDirs, showDetails);
		child = child->GetFwdSiblingLink();
	}
}

int main()
{
#ifdef _MSC_VER
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	int dm = CMP_CONTENT; // Default compare method
	PathContext paths(_T(""), _T("")); // Default empty paths
	FileFilterHelper filter;
	filter.SetMaskOrExpression(_T("*.*"));
	std::unique_ptr<CDiffContext> pCtx; // Store context for comparison results
	std::unique_ptr<CompareStats> pCmpStats;
	std::unique_ptr<FilterExpression> searchLineFilterExpression;

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
			std::wcout << L"  r <key-expression>           : Set rename/move detection key (e.g., {{content}})\n";
			std::wcout << L"  g <text-or-le:expression>    : Set line search filter (plain text or line expression)\n";
			std::wcout << L"  c                            : Start folder comparison\n";
			std::wcout << L"  d [detail]                   : Display comparison results (add 'detail' for Date, Size, Diff count)\n";
			std::wcout << L"  s                            : Show comparison statistics\n";
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
			filter.SetMaskOrExpression(mask.c_str());
			if (filter.GetErrorList().size() > 0)
			{
				for (const auto*error : filter.GetErrorList())
					std::wcout << FormatFilterErrorSummary(*error) << "\n";
			}
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
			else if (method == L"Existence") dm = CMP_EXISTENCE;
			else {
				std::wcout << L"Unknown compare method\n";
				continue;
			}
		}
		else if (cmd[0] == L'r') // Set rename/move detection key
		{
			if (cmd.length() < 3)
			{
				std::wcout << L"Usage: r <key-expression>\n";
				std::wcout << L"Example: r {{Name}}\n";
				std::wcout << L"Available keys: {{Name}}, {{BaseName}}, {{Size}}, {{Date}}, etc.\n";
				continue;
			}

			std::wstring keyExpr = cmd.substr(2);

			// Create or reset RenameMoveDetection object
			if (!pCtx)
				pCtx = std::make_unique<CDiffContext>(paths, dm);

			if (!pCtx->m_pRenameMoveDetection)
				pCtx->m_pRenameMoveDetection = std::make_unique<RenameMoveDetection>();

			// Parse and set the key expression
			auto expr = std::make_unique<FilterExpression>(ucr::toUTF8(keyExpr));
			if (expr->errorCode != 0)
			{
				std::wcout << L"Error parsing expression: " << FormatFilterErrorSummary(*expr) << L"\n";
				continue;
			}

			pCtx->m_pRenameMoveDetection->SetRenameMoveKeyExpression(expr.get());
			std::wcout << L"Rename/move detection key set: " << keyExpr << L"\n";
		}
		else if (cmd[0] == L'g' && cmd.length() > 1 && iswspace(cmd[1])) // Set search line filter
		{
			std::wstring search = cmd.substr(2);
			while (!search.empty() && iswspace(search.front()))
				search.erase(search.begin());
			if (search.rfind(L"le:", 0) != 0)
			{
				std::wstring quoted = L"\"";
				for (wchar_t ch : search)
				{
					quoted += ch;
					if (ch == L'\"')
						quoted += L'\"';
				}
				quoted += L"\"";
				search = L"le:Line contains " + quoted;
			}
			auto expression = std::make_unique<FilterExpression>(ucr::toUTF8(search.substr(3)));
			expression->diritem = false;
			if (expression->errorCode != FILTER_ERROR_NO_ERROR)
			{
				std::wcout << L"Error parsing search expression: " << FormatFilterErrorSummary(*expression) << L"\n";
				continue;
			}
			searchLineFilterExpression = std::move(expression);
			std::wcout << L"Search line filter set: " << search << L"\n";
		}
		else if (cmd[0] == L'c') // Compare
		{
			// Preserve RenameMoveDetection if already set
			std::unique_ptr<RenameMoveDetection> preservedRenameMoveDetection;
			if (pCtx && pCtx->m_pRenameMoveDetection)
				preservedRenameMoveDetection = std::move(pCtx->m_pRenameMoveDetection);

			pCtx = std::make_unique<CDiffContext>(paths, dm);
			pCmpStats = std::make_unique<CompareStats>(paths.GetSize());

			// Restore RenameMoveDetection
			if (preservedRenameMoveDetection)
			{
				pCtx->m_pRenameMoveDetection = std::move(preservedRenameMoveDetection);
				pCtx->m_pRenameMoveDetection->GetRenameMoveKeyExpression()->SetDiffContext(pCtx.get());
			}

			if (searchLineFilterExpression)
			{
				pCtx->m_pSearchLineFilterExpression = std::make_unique<FilterExpression>(*searchLineFilterExpression);
				pCtx->m_pSearchLineFilterExpression->SetDiffContext(pCtx.get());
			}

			DIFFOPTIONS options = {0};
			options.nIgnoreWhitespace = false;
			options.bIgnoreBlankLines = false;
			options.bFilterCommentsLines = false;
			options.bIgnoreCase = false;
			options.bIgnoreEol = false;

			pCtx->InitDiffItemList();
			pCtx->CreateCompareOptions(dm, options);
			pCtx->m_iGuessEncodingType = 0;
			pCtx->m_bIgnoreSmallTimeDiff = true;
			pCtx->m_bStopAfterFirstDiff = false;
			pCtx->m_nQuickCompareLimit = 4 * 1024 * 1024;
			pCtx->m_bPluginsEnabled = false;
			pCtx->m_bWalkUniques = true;
			pCtx->m_pCompareStats = pCmpStats.get();
			pCtx->m_bRecursive = true;
			pCtx->m_piFilterGlobal = &filter;
			filter.SetDiffContext(pCtx.get());

			CDiffThread diffThread;
			diffThread.SetContext(pCtx.get());
			diffThread.SetCollectFunction([](DiffFuncStruct* myStruct) {
				bool casesensitive = false;
				int depth = myStruct->context->m_bRecursive ? -1 : 0;
				PathContext paths = myStruct->context->GetNormalizedPaths();
				String subdir[3] = { _T(""), _T(""), _T("") };
				DirScan_GetItems(paths, subdir, myStruct,
					casesensitive, depth, nullptr, myStruct->context->m_bWalkUniques);
				if (myStruct->context->m_pRenameMoveDetection)
				{
					myStruct->context->m_pRenameMoveDetection->Detect(*myStruct->context, true);
					myStruct->context->m_pRenameMoveDetection->Merge(*myStruct->context, true);
				}
				});
			diffThread.SetCompareFunction([](DiffFuncStruct* myStruct) {
				DirScan_CompareItems(myStruct, nullptr);
				});
			diffThread.CompareDirectories();

			while (diffThread.GetThreadState() != CDiffThread::THREAD_COMPLETED)
			{
				Poco::Thread::sleep(200);
				std::wcout << L"Comparing " << pCmpStats->GetComparedItems() << L" items...\r";
			}
			std::wcout << L"\nComparison completed.\n";
		}
		else if (cmd[0] == L'd') // Display results
		{
			if (!pCtx)
			{
				std::wcout << L"No comparison results available. Run 'c' command first.\n";
				continue;
			}

			DIFFITEM* pos = pCtx->GetFirstDiffPosition();
			if (!pos)
			{
				std::wcout << L"No items found.\n";
				continue;
			}

			bool showDetails = false;
			if (cmd.length() > 2)
			{
				std::wstring option = cmd.substr(2);
				if (option == L"detail" || option == L"details")
					showDetails = true;
			}

			std::wcout << L"\nComparison Results";
			if (showDetails)
				std::wcout << L" (Detailed)";
			std::wcout << L":\n";
			std::wcout << L"==================\n";
			int nDirs = pCtx->GetCompareDirs();

			// Create DirViewColItems for professional WinMerge formatting
			std::vector<String> additionalProperties;
			DirViewColItems colItems(nDirs, additionalProperties);

			while (pos)
			{
				PrintDiffItemTree(colItems, pCtx.get(), pos, 0, nDirs, showDetails);
				pos = pos->GetFwdSiblingLink();
			}
		}
		else if (cmd[0] == L's') // Show statistics
		{
			if (!pCtx || !pCtx->m_pCompareStats)
			{
				std::wcout << L"No comparison results available. Run 'c' command first.\n";
				continue;
			}

			const CompareStats* stats = pCtx->m_pCompareStats;
			int nDirs = pCtx->GetCompareDirs();

			std::wcout << L"\n==================\n";
			std::wcout << L"Comparison Statistics\n";
			std::wcout << L"==================\n\n";

			std::wcout << L"Total Items:    " << stats->GetTotalItems() << L"\n";
			std::wcout << L"Compared Items: " << stats->GetComparedItems() << L"\n\n";

			// Files
			std::wcout << L"Files:\n";
			std::wcout << L"  Identical:       " << stats->GetCount(CompareStats::RESULT_SAME) << L"\n";
			std::wcout << L"  Different:       " << stats->GetCount(CompareStats::RESULT_DIFF) << L"\n";
			std::wcout << L"  Binary Identical:" << stats->GetCount(CompareStats::RESULT_BINSAME) << L"\n";
			std::wcout << L"  Binary Different:" << stats->GetCount(CompareStats::RESULT_BINDIFF) << L"\n";
			std::wcout << L"  Left Only:       " << stats->GetCount(CompareStats::RESULT_LUNIQUE) << L"\n";
			if (nDirs > 2)
			{
				std::wcout << L"  Middle Only:     " << stats->GetCount(CompareStats::RESULT_MUNIQUE) << L"\n";
				std::wcout << L"  Left Missing:    " << stats->GetCount(CompareStats::RESULT_LMISSING) << L"\n";
				std::wcout << L"  Middle Missing:  " << stats->GetCount(CompareStats::RESULT_MMISSING) << L"\n";
				std::wcout << L"  Right Missing:   " << stats->GetCount(CompareStats::RESULT_RMISSING) << L"\n";
			}
			std::wcout << L"  Right Only:      " << stats->GetCount(CompareStats::RESULT_RUNIQUE) << L"\n";
			std::wcout << L"  Skipped:         " << stats->GetCount(CompareStats::RESULT_SKIP) << L"\n";

			// Directories
			std::wcout << L"\nDirectories:\n";
			std::wcout << L"  Identical:       " << stats->GetCount(CompareStats::RESULT_DIRSAME) << L"\n";
			std::wcout << L"  Different:       " << stats->GetCount(CompareStats::RESULT_DIRDIFF) << L"\n";
			std::wcout << L"  Left Only:       " << stats->GetCount(CompareStats::RESULT_LDIRUNIQUE) << L"\n";
			if (nDirs > 2)
			{
				std::wcout << L"  Middle Only:     " << stats->GetCount(CompareStats::RESULT_MDIRUNIQUE) << L"\n";
				std::wcout << L"  Left Missing:    " << stats->GetCount(CompareStats::RESULT_LDIRMISSING) << L"\n";
				std::wcout << L"  Middle Missing:  " << stats->GetCount(CompareStats::RESULT_MDIRMISSING) << L"\n";
				std::wcout << L"  Right Missing:   " << stats->GetCount(CompareStats::RESULT_RDIRMISSING) << L"\n";
			}
			std::wcout << L"  Right Only:      " << stats->GetCount(CompareStats::RESULT_RDIRUNIQUE) << L"\n";
			std::wcout << L"  Skipped:         " << stats->GetCount(CompareStats::RESULT_DIRSKIP) << L"\n";

			// Errors
			std::wcout << L"\nErrors:            " << stats->GetCount(CompareStats::RESULT_ERROR) << L"\n";
			std::wcout << L"==================\n\n";
		}
		else
		{
			std::wcout << L"Unknown command: " << cmd << std::endl;
		}
	}

	return 0;
}
