#pragma once

#include <afxwin.h>
#include <afxext.h>

class CMyReBar : public CReBar
{
public:
	CMyReBar();
	void LoadStateFromString(const CString& state);
	CString GenerateStateString();

protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	DECLARE_MESSAGE_MAP()
};