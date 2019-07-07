/** 
 * @file  DirCompProgressBar.h
 *
 * @brief Declaration file for Directory compare statusdialog class
 */
#pragma once

#include "TrDialogs.h"
#include "CompareStats.h"

class ITaskBarList3;

/////////////////////////////////////////////////////////////////////////////
// DirCompProgressBar dialog

/**
 * @brief Class for directory compare statusdialog.
 * 
 * Implements non-modal dialog directory compares. We create this
 * modeless dialog when we start the compare and destroy it after
 * compare is ready. Dialog itself shows counts of total found items
 * and items compared so far. And nice progressbar for user to have some
 * idea how compare is going.
 *
 * Status updates are fired by periodic timer events. We have shared
 * datastructure with compare code. Compare code updates status information
 * to datastructure during compare. When timer event fires, dialog reads
 * that datastructure and updates the GUI.
 * 
 * @todo Now we update total count of items with same timer than we update
 * compared items count. Maybe we should use different timer and bigger
 * interval for total count updates?
 */
class DirCompProgressBar : public CTrDialogBar
{
// Construction
public:
	DirCompProgressBar();   // standard constructor
	~DirCompProgressBar();
	BOOL Create(CWnd* pParentWnd);
	void SetCompareStat(CompareStats * pCompareStats);
	void StartUpdating();
	void EndUpdating();
	void SetPaused(bool paused);

// Dialog Data
	//{{AFX_DATA(DirCompProgressBar)
	enum { IDD = IDD_DIRCOMP_PROGRESS };
	//}}AFX_DATA

// Implementation
protected:
	void ClearStat();
	void SetProgressState(int comparedItems, int totalItems);

	// Generated message map functions
	//{{AFX_MSG(DirCompProgressBar)
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CompareStats *m_pCompareStats; /**< Pointer to comparestats */
	CompareStats::CMP_STATE m_prevState; /**< Previous state for compare (to track changes) */
	bool m_bCompareReady; /**< Compare ready, waiting for closing? */
#ifdef __ITaskbarList3_INTERFACE_DEFINED__
	ITaskbarList3 *m_pTaskbarList;
#endif
};

/**
 * @brief Set pointer to compare stats.
 * @param [in] pCompareStats Pointer to stats.
 */
inline void DirCompProgressBar::SetCompareStat(CompareStats * pCompareStats)
{
	m_pCompareStats = pCompareStats;
}

