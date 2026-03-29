/** 
 * @file ComparisonResultFilterDlg.h
 *
 * @brief Declaration of CComparisonResultFilterDlg class, which implements the dialog for filtering comparison results based on various criteria such as identical, different, left-only, right-only, etc.
 */
#pragma once

#include "TrDialogs.h"
#include "resource.h"

/**
 * @brief Dialog for filtering by comparison result
 */
class CComparisonResultFilterDlg : public CTrDialog
{
public:
	CComparisonResultFilterDlg(bool is3Way, CWnd* pParent = nullptr);

	enum { IDD = IDD_COMPARISON_RESULT_FILTER };

	String m_sExpression;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBnClickedInclude();
	afx_msg void OnBnClickedExclude();
	DECLARE_MESSAGE_MAP()

private:
	void UpdateCheckboxStates();
	String BuildExpression() const;

	bool m_is3Way;
	int m_nIncludeExclude; // 0 = Include, 1 = Exclude
	BOOL m_bIdentical;
	BOOL m_bDifferent;
	BOOL m_bLeftOnly;
	BOOL m_bRightOnly;
	BOOL m_bSkipped;
	BOOL m_bMiddleOnly;
	BOOL m_bLeftOnlyDifferent;
	BOOL m_bMiddleOnlyDifferent;
	BOOL m_bRightOnlyDifferent;
	BOOL m_bLeftOnlyMissing;
	BOOL m_bMiddleOnlyMissing;
	BOOL m_bRightOnlyMissing;
};
