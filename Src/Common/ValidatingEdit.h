/** 
 * @file  ValidatingEdit.h
 *
 * @brief Declaration file for CValidatingEdit class
 */
#pragma once

class CValidatingEdit : public CEdit
{
	DECLARE_DYNAMIC(CValidatingEdit);
public:

	CValidatingEdit();
	virtual ~CValidatingEdit();

	std::function<bool(const CString&, CString&)> m_validator;

	void SetBackColor(COLORREF color);
	void SetAppliedColor(COLORREF color);
	void SetApplied(bool applied);
	void Validate();
	void OnEnChange();

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	CToolTipCtrl m_toolTip;
	bool m_hasError;
	bool m_isApplied;
	COLORREF m_errorColor;
	COLORREF m_appliedColor;
	CString m_errorMessage;
	CBrush m_errorBrush;
	CBrush m_appliedBrush;
	TOOLINFO m_toolItem;
};