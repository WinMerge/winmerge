#pragma once
#include <afxwin.h>

class CTransStatic : public CStatic
{
	DECLARE_DYNAMIC(CTransStatic)

public:
	CTransStatic();
	virtual ~CTransStatic();

	void SetTextColor(COLORREF crText, bool bUseCustomColor = true);
	void ResetTextColor();

protected:
	COLORREF m_crText = RGB(0, 0, 0);
	bool m_bUseCustomColor = false;

	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg LRESULT OnSetText(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

private:
	void InvalidateParentArea();
};