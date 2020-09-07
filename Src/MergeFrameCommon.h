/** 
 * @file  MergeFrameCommon.h
 *
 * @brief interface of the CMergeFrameCommon class
 *
 */
#pragma once

#include "utils/DpiAware.h"

class CMergeFrameCommon: public CMDIChildWnd, public DpiAware::PerMonitorDpiAwareWindow<CMergeFrameCommon>
{
	DECLARE_DYNCREATE(CMergeFrameCommon)
public:
	CMergeFrameCommon(int nIdenticalIcon  = -1, int nDifferentIcon = -1);
	bool IsActivated() const { return m_bActivated; }
	void ActivateFrame(int nCmdShow);
	void SetLastCompareResult(int nResult);
	void SaveWindowState();
	void SetSharedMenu(HMENU hMenu) { m_hMenuShared = hMenu; }
	void RemoveBarBorder();
	virtual BOOL IsTabbedMDIChild()
	{
		return TRUE; // https://stackoverflow.com/questions/35553955/getting-rid-of-3d-look-of-mdi-frame-window
	}
	virtual void CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType = adjustBorder) {
		CalcWindowRectImpl(lpClientRect, nAdjustType);
	}

protected:
	int m_nLastSplitPos[2];
private:
	bool m_bActivated;
	HICON m_hIdentical;
	HICON m_hDifferent;

protected:
	virtual ~CMergeFrameCommon();

protected:
	//{{AFX_MSG(CMergeFrameCommon)
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnDestroy();
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	afx_msg LRESULT OnDpiChangedBeforeParent(WPARAM wParam, LPARAM lParam);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
