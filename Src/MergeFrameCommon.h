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

class CMergeFrameCommon: public CMDIChildWnd
{
	DECLARE_DYNCREATE(CMergeFrameCommon)
public:
	CMergeFrameCommon(int nIdenticalIcon  = -1, int nDifferentIcon = -1);
	bool IsActivated() const { return m_bActivated; }
	void ActivateFrame(int nCmdShow);
	void SetLastCompareResult(int nResult);
	static void ShowIdenticalMessage(const PathContext& paths, bool bIdenticalAll, std::function<int (const tchar_t*, UINT, UINT)> funcMessageBox);
	static String GetTitleString(const PathContext& paths, const String desc[], const PackingInfo *pInfoUnpacker, const PrediffingInfo *pInfoPrediffer, bool hasTrivialDiffs = false);
	static String GetTooltipString(const PathContext& paths, const String desc[], const PackingInfo *pInfoUnpacker, const PrediffingInfo *pInfoPrediffer, bool hasTrivialDiffs = false);
	static void ChangeMergeMenuText(int srcPane, int dstPane, CCmdUI* pCmdUI);
	static std::pair<int, int> MenuIDtoXY(UINT nID, int nActivePane, int nBuffers);
	void SaveWindowState();
	void SetSharedMenu(HMENU hMenu) { m_hMenuShared = hMenu; }
	void RemoveBarBorder();
	virtual BOOL IsTabbedMDIChild()
	{
		return TRUE; // https://stackoverflow.com/questions/35553955/getting-rid-of-3d-look-of-mdi-frame-window
	}
protected:
	int m_nLastSplitPos[2];
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
