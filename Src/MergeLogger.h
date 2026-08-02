/** 
 * @file  MergeLogger.h
 * @brief Header file for MergeLogger class, which provides logging functionality for file comparisons and merge operations.
 */
#pragma once

#include "UnicodeString.h"

class PathContext;
class PrediffingInfo;
class PackingInfo;
class CompareStats;
struct FileLocation;
struct IMergeDoc;
struct CEPoint;

namespace MergeLogger
{
	void LogComparisonStart(int nFiles, const FileLocation ifileloc[], const String descs[], const PackingInfo* infoUnpacker, const PrediffingInfo* infoPrediffer);
	void LogComparisonStart(const PathContext& paths, const String descs[], const PackingInfo* infoUnpacker, const PrediffingInfo* infoPrediffer);
	void LogComparisonCompleted(const IMergeDoc& mergeDoc);
	void LogComparisonCompleted(const CompareStats& stats);
	void LogFileSaved(const String& path);
	void LogCopyDiff(int srcPane, int dstPane, int nDiff);
	void LogCopyLines(int srcPane, int dstPane, int firstLine, int lastLine);
	void LogCopyInlineDiffs(int srcPane, int dstPane, int nDiff, int firstLine, int lastLine);
	void LogCopyCharacters(int srcPane, int dstPane,  int nDiff, const CEPoint& ptStart, const CEPoint& ptEnd);
	void LogUndo();
	void LogRedo();
}
