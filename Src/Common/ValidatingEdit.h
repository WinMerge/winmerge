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

	void Validate();
	void OnEnChange();

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg BOOL PreTranslateMessage(MSG* pMsg);
	void OnKillFocus(CWnd* pNewWnd);
	void OnTimer(UINT_PTR nIDEvent);

	CToolTipCtrl m_toolTip;
	bool m_hasError;
	CString m_errorMessage;
	CBrush m_errorBrush;
};