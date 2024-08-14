#pragma once

#include <afxwin.h>
#include <afxext.h>

class CMyReBar : public CReBar
{
public:
	CMyReBar();
	void SetStateString(const CString& state);
	CString MakeStateString();

protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	DECLARE_MESSAGE_MAP()
};