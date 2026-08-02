/** 
 * @file  MergeTextFormatter.cpp
 * @brief Implementation file for MergeTextFormatter class, which provides utility functions for formatting text related to file comparisons and merge operations.
 */
#include "pch.h"
#include "MergeTextFormatter.h"
#include "paths.h"
#include "FileTransform.h"
#include "IMergeDoc.h"
#include "IDirDoc.h"
#include "I18n.h"

String MergeTextFormatter::GetDiffStatusString(int curDiffIndex, int diffCount)
{
	if (diffCount == 0)
		return _("Identical");
	if (diffCount < 0)
		return _("Different");

	if (curDiffIndex < 0)
		return diffCount == 1 ? _("1 Difference Found") :
			  strutils::format_string1(_("%1 Differences Found"), strutils::to_str(diffCount));

	return strutils::format_string2(_("Difference %1 of %2"),
		strutils::to_str(curDiffIndex + 1), strutils::to_str(diffCount));
}

String MergeTextFormatter::GetTitleStringFlags(const IMergeDoc& mergeDoc)
{
	const PackingInfo* pInfoUnpacker = mergeDoc.GetUnpacker();
	const PrediffingInfo* pInfoPrediffer = mergeDoc.GetPrediffer();
	const bool hasTrivialDiffs = mergeDoc.GetTrivialCount();
	String flags;
	if (pInfoUnpacker && !pInfoUnpacker->GetPluginPipeline().empty())
		flags += _T("U");
	if (pInfoPrediffer && !pInfoPrediffer->GetPluginPipeline().empty())
		flags += _T("P");
	if (hasTrivialDiffs)
		flags += _T("F");
	return (flags.empty() ? _T("") : (_T("(") + flags + _T(") ")));
}

String MergeTextFormatter::GetTitleString(const IMergeDoc& mergeDoc)
{
	PathContext paths;
	const int nBuffers = mergeDoc.GetFileCount();
	String sFileName[3];
	String sTitle;
	for (int nBuffer = 0; nBuffer < nBuffers; nBuffer++)
	{
		const String desc = mergeDoc.GetDescription(nBuffer);
		sFileName[nBuffer] = !desc.empty() ? desc : paths::FindFileName(mergeDoc.GetPath(nBuffer));
	}
	if (std::count(&sFileName[0], &sFileName[0] + nBuffers, sFileName[0]) == nBuffers)
		sTitle = sFileName[0] + strutils::format(_T(" x %d"), nBuffers);
	else
		sTitle = strutils::join(&sFileName[0], &sFileName[0] + nBuffers, _T(" - "));
	return GetTitleStringFlags(mergeDoc) + sTitle;
}

String MergeTextFormatter::GetReportTitleString(const IMergeDoc& mergeDoc, int pane)
{
	String strDesc = mergeDoc.GetDescription(pane);
	if (!strDesc.empty())
		return strDesc;
	String path = mergeDoc.GetPath(pane);
	IDirDoc* pDirDoc = mergeDoc.GetDirDoc();
	if (pDirDoc != nullptr && pDirDoc->IsArchiveFolders())
		pDirDoc->ApplyDisplayRoot(pane, path);
	return path;
}

String MergeTextFormatter::GetTooltipString(const IMergeDoc& mergeDoc)
{
	PathContext paths;
	String desc[3];
	const int nBuffers = mergeDoc.GetFileCount();
	for (int i = 0; i < nBuffers; ++i)
	{
		desc[i] = mergeDoc.GetDescription(i);
		paths.SetPath(i, mergeDoc.GetPath(i), false);
	}
	return GetTooltipString(paths, desc, mergeDoc.GetUnpacker(), mergeDoc.GetPrediffer(), mergeDoc.GetTrivialCount() > 0);
}

String MergeTextFormatter::GetTooltipString(const PathContext& paths, const String desc[],
	const PackingInfo *pInfoUnpacker, const PrediffingInfo *pInfoPrediffer, bool hasTrivialDiffs)
{
	String sTitle;
	for (int nBuffer = 0; nBuffer < paths.GetSize(); nBuffer++)
	{
		sTitle += strutils::format(_T("%d: "), nBuffer + 1);
		if (!desc[nBuffer].empty())
		{
			sTitle += desc[nBuffer];
			if (!paths[nBuffer].empty()) 
				sTitle += _T(" (") + paths[nBuffer] + _T(")");
		}
		else
		{
			sTitle += paths[nBuffer];
		}
		sTitle += _T(" - ");
		if (nBuffer == 0)
			sTitle += _("Left");
		else if (nBuffer == 1 && paths.GetSize() > 2)
			sTitle += _("Middle");
		else
			sTitle += _("Right");
		sTitle += _T("\n");
	}
	if (pInfoUnpacker && !pInfoUnpacker->GetPluginPipeline().empty())
		sTitle += strutils::format(_T("%s: %s\n"), _("Unpacker"), pInfoUnpacker->GetPluginPipeline());
	if (pInfoPrediffer && !pInfoPrediffer->GetPluginPipeline().empty())
		sTitle += strutils::format(_T("%s: %s\n"), _("Prediffer"), pInfoPrediffer->GetPluginPipeline());
	if (hasTrivialDiffs)
		sTitle += _("Filter applied") + _T("\n");
	return sTitle;
}

String MergeTextFormatter::GetPluginInfoString(const PackingInfo* infoUnpacker, const PrediffingInfo* infoPrediffer)
{
	String p;
	if (infoUnpacker && !infoUnpacker->GetPluginPipeline().empty())
		p = _("Unpacker") + _T(": ") + infoUnpacker->GetPluginPipeline();
	if (infoPrediffer && !infoPrediffer->GetPluginPipeline().empty())
	{
		if (!p.empty())
			p += _T(", ");
		p += _("Prediffer") + _T(": ") + infoPrediffer->GetPluginPipeline();
	}
	if (p.empty())
		return _T("");
	return _T(" (") + p + _T(")");
}

