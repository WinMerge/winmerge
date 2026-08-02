/** 
 * @file  MergeTextFormatter.h
 * @brief Header file for MergeTextFormatter class, which provides utility functions for formatting text related to file comparisons and merge operations.
 */
#pragma once

#include "UnicodeString.h"

class PrediffingInfo;
class PackingInfo;
class CompareStats;
class IHeaderBar;
struct FileLocation;
struct IMergeDoc;
struct CEPoint;
class PathContext;

namespace MergeTextFormatter
{
	String GetPluginInfoString(const PackingInfo* infoUnpacker, const PrediffingInfo* infoPrediffer);
	String GetDiffStatusString(int curDiff, int diffCount);
	String GetTitleStringFlags(const IMergeDoc& mergeDoc);
	String GetTitleString(const IMergeDoc& mergeDoc);
	String GetReportTitleString(const IMergeDoc& mergeDoc, int pane);
	String GetTooltipString(const IMergeDoc& mergeDoc);
	String GetTooltipString(const PathContext& paths, const String desc[], const PackingInfo* pInfoUnpacker, const PrediffingInfo* pInfoPrediffer, bool hasTrivialDiffs = false);
}
