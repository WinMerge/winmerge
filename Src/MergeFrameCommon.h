/** 
 * @file  MergeFrameCommon.h
 *
 * @brief interface of the CMergeFrameCommon class
 *
 */
#pragma once

class CMergeFrameCommon: public CMDIChildWnd
{
	DECLARE_DYNCREATE(CMergeFrameCommon)
public:
	CMergeFrameCommon::CMergeFrameCommon(int nIdenticalIcon  = -1, int nDifferentIcon = -1);
	bool IsActivated() const { return m_bActivated; }
	void ActivateFrame(int nCmdShow);
	void SetLastCompareResult(int nResult);
	void SaveWindowState();
	void SetSharedMenu(HMENU hMenu) { m_hMenuShared = hMenu; };
protected:
	int m_nLastSplitPos[2];
private:
	bool m_bActivated;
	HICON m_hIdentical;
	HICON m_hDifferent;

	//{{AFX_MSG(CMergeFrameCommon)
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
