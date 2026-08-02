/** 
 * @file  MergeLogger.cpp
 * @brief Implementation file for MergeLogger class, which provides logging functionality for file comparisons and merge operations.
 */
#include "pch.h"
#include "MergeLogger.h"
#include "MergeTextFormatter.h"
#include "FileLocation.h"
#include "Logger.h"
#include "CompareStats.h"
#include "PathContext.h"
#include "IMergeDoc.h"
#include "cepoint.h"
#include "I18n.h"

void MergeLogger::LogComparisonStart(int nFiles, const FileLocation ifileloc[], const String descs[], const PackingInfo* infoUnpacker, const PrediffingInfo* infoPrediffer)
{
	String str[3];
	for (int i = 0; i < nFiles; ++i)
	{
		str[i] = ifileloc[i].filepath;
		if (descs && !descs[i].empty())
			str[i] += _T("(") + descs[i] + _T(")");
	}
	String s = (nFiles < 3 ?
		strutils::format_string2(_("Comparing %1 with %2"), str[0], str[1]) :
		strutils::format_string3(_("Comparing %1 with %2 and %3"), str[0], str[1], str[2])
		);
	RootLogger::Info(s + MergeTextFormatter::GetPluginInfoString(infoUnpacker, infoPrediffer));
}

void MergeLogger::LogComparisonStart(const PathContext& paths, const String descs[], const PackingInfo* infoUnpacker, const PrediffingInfo* infoPrediffer)
{
	String str[3];
	for (int i = 0; i < paths.GetSize(); ++i)
	{
		str[i] = paths[i];
		if (descs && !descs[i].empty())
			str[i] += _T("(") + descs[i] + _T(")");
	}
	String s = (paths.GetSize() < 3) ?
			strutils::format_string2(_("Comparing %1 with %2"), str[0], str[1]) : 
			strutils::format_string3(_("Comparing %1 with %2 and %3"), str[0], str[1], str[2]);
	RootLogger::Info(s + MergeTextFormatter::GetPluginInfoString(infoUnpacker, infoPrediffer));
}

void MergeLogger::LogComparisonCompleted(const IMergeDoc& mergeDoc)
{
	RootLogger::Info(_("Comparison completed") + _T(": ") + MergeTextFormatter::GetTitleStringFlags(mergeDoc) + MergeTextFormatter::GetDiffStatusString(-1, mergeDoc.GetDiffCount()));
}

void MergeLogger::LogComparisonCompleted(const CompareStats& stats)
{
	const int errorCount = stats.GetCount(CompareStats::RESULT_ERROR);
	if (errorCount > 0)
	{
		String s = errorCount == 1 ? _("1 Error Found") :
			  strutils::format_string1(_("%1 Errors Found"), strutils::to_str(errorCount));
		RootLogger::Warn(_("Comparison completed") + _T(": ") + s);
		return;
	}
	int diffCount = 0;
	for (auto type : {
		CompareStats::RESULT_DIFF, CompareStats::RESULT_BINDIFF,
		CompareStats::RESULT_LUNIQUE, CompareStats::RESULT_MUNIQUE, CompareStats::RESULT_RUNIQUE,
		CompareStats::RESULT_LMISSING, CompareStats::RESULT_MMISSING, CompareStats::RESULT_RMISSING
		})
		diffCount += stats.GetCount(type);
	RootLogger::Info(_("Comparison completed") + _T(": ") + MergeTextFormatter::GetDiffStatusString(-1, diffCount));
}

void MergeLogger::LogFileSaved(const String& path)
{
	RootLogger::Info(_("File saved") + _T(": ") + path);
}

void MergeLogger::LogCopyDiff(int srcPane, int dstPane, int nDiff)
{
	RootLogger::Info(strutils::format(_T("Copy diff: pane %d to %d (hunk %d)"),
		srcPane, dstPane, nDiff));
}

void MergeLogger::LogCopyLines(int srcPane, int dstPane, int firstLine, int lastLine)
{
	RootLogger::Info(strutils::format(_T("Copy lines: pane %d to %d (vline %d-%d)"),
		srcPane, dstPane, firstLine, lastLine));
}

void MergeLogger::LogCopyInlineDiffs(int srcPane, int dstPane, int nDiff, int firstWordDiff, int lastWordDiff)
{
	RootLogger::Info(strutils::format(_T("Copy inline diffs: pane %d to %d (hunk %d, wdiff %d-%d)"),
		srcPane, dstPane, nDiff, firstWordDiff, lastWordDiff));
}

void MergeLogger::LogCopyCharacters(int srcPane, int dstPane,  int nDiff, const CEPoint& ptStart, const CEPoint& ptEnd)
{
	RootLogger::Info(strutils::format(_T("Copy chars: pane %d to %d (hunk %d, vline %d,%d-%d,%d)"),
		srcPane, dstPane, nDiff, ptStart.y, ptStart.x, ptEnd.y, ptEnd.x));
}

void MergeLogger::LogUndo()
{
	RootLogger::Info(_("Undo"));
}

void MergeLogger::LogRedo()
{
	RootLogger::Info(_("Redo"));
}

