/** 
 * @file  MergeFrameCommon.h
 *
 * @brief interface of the CMergeFrameCommon class
 *
 */
#pragma once

#include "UnicodeString.h"
#include "PathContext.h"

class PrediffingInfo;
class PackingInfo;
class CompareStats;
struct FileLocation;
struct IMergeDoc;
struct CEPoint;

class CMergeFrameCommon: public CMDIChildWnd
{
	DECLARE_DYNCREATE(CMergeFrameCommon)
public:
	CMergeFrameCommon(int nIdenticalIcon  = -1, int nDifferentIcon = -1);
	bool IsActivated() const { return m_bActivated; }
	void ActivateFrame(int nCmdShow);
	void SetLastCompareResult(int nResult);
	static void ShowIdenticalMessage(const PathContext& paths, bool bIdenticalAll, bool bExactCompareAsync = false);
	static String GetPluginInfoString(const PackingInfo* infoUnpacker, const PrediffingInfo* infoPrediffer);
	static String GetDiffStatusString(int curDiff, int diffCount);
	static String GetTitleString(const IMergeDoc& mergeDoc);
	static String GetTooltipString(const IMergeDoc& mergeDoc);
	static String GetTooltipString(const PathContext& paths, const String desc[], const PackingInfo* pInfoUnpacker, const PrediffingInfo* pInfoPrediffer, bool hasTrivialDiffs = false);
	static void LogComparisonStart(int nFiles, const FileLocation ifileloc[], const String descs[], const PackingInfo* infoUnpackerconst , const PrediffingInfo* infoPrediffer);
	static void LogComparisonStart(const PathContext& paths, const String descs[], const PackingInfo* infoUnpacker, const PrediffingInfo* infoPrediffer);
	static void LogComparisonCompleted(const IMergeDoc& mergeDoc);
	static void LogComparisonCompleted(const CompareStats& stats);
	static void LogFileSaved(const String& path);
	static void LogCopyDiff(int srcPane, int dstPane, int nDiff);
	static void LogCopyLines(int srcPane, int dstPane, int firstLine, int lastLine);
	static void LogCopyInlineDiffs(int srcPane, int dstPane, int nDiff, int firstLine, int lastLine);
	static void LogCopyCharacters(int srcPane, int dstPane,  int nDiff, const CEPoint& ptStart, const CEPoint& ptEnd);
	static void LogUndo();
	static void LogRedo();
	static void ChangeMergeMenuText(int srcPane, int dstPane, CCmdUI* pCmdUI);
	static std::pair<int, int> MenuIDtoXY(UINT nID, int nActivePane, int nBuffers);
	void SaveWindowState();
	void SetSharedMenu(HMENU hMenu) { m_hMenuShared = hMenu; }
	virtual BOOL IsTabbedMDIChild()
	{
		return TRUE; // https://stackoverflow.com/questions/35553955/getting-rid-of-3d-look-of-mdi-frame-window
	}
protected:
	int m_nLastSplitPos[3];
private:
	bool m_bActivated;
	HICON m_hCurrent;
	HICON m_hIdentical;
	HICON m_hDifferent;

protected:
	virtual ~CMergeFrameCommon();

protected:
	//{{AFX_MSG(CMergeFrameCommon)
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnDestroy();
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
