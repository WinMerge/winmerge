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
class IHeaderBar;
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
	static void ShowShellMenu(CWnd* pWnd, const String& path);
	static void ShowIdenticalMessage(const PathContext& paths, bool bIdenticalAll, bool bExactCompareAsync = false);
	static void ChangeMergeMenuText(int srcPane, int dstPane, CCmdUI* pCmdUI);
	static std::pair<int, int> MenuIDtoXY(UINT nID, int nActivePane, int nBuffers);
	void SaveWindowState();
	void SetSharedMenu(HMENU hMenu) { m_hMenuShared = hMenu; }
	virtual BOOL IsTabbedMDIChild()
	{
		return TRUE; // https://stackoverflow.com/questions/35553955/getting-rid-of-3d-look-of-mdi-frame-window
	}
	virtual IHeaderBar* GetHeaderInterface()
	{
		return nullptr;
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
	afx_msg void OnEditorEditPath();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
