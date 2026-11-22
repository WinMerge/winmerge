#include "stdafx.h"
#include "TransStatic.h"

IMPLEMENT_DYNAMIC(CTransStatic, CStatic)

BEGIN_MESSAGE_MAP(CTransStatic, CStatic)
	ON_WM_CTLCOLOR_REFLECT()
	ON_MESSAGE(WM_SETTEXT, &CTransStatic::OnSetText)
END_MESSAGE_MAP()

CTransStatic::CTransStatic()
{}

CTransStatic::~CTransStatic()
{}

void CTransStatic::SetTextColor(COLORREF crText, bool bUseCustomColor)
{
	m_crText = crText;
	m_bUseCustomColor = bUseCustomColor;
	if (::IsWindow(m_hWnd))
		InvalidateParentArea();
}

void CTransStatic::ResetTextColor()
{
	m_bUseCustomColor = false;
	if (::IsWindow(m_hWnd))
		InvalidateParentArea();
}

HBRUSH CTransStatic::CtlColor(CDC* pDC, UINT nCtlColor)
{
	UNREFERENCED_PARAMETER(nCtlColor);

	if (pDC == nullptr)
		return static_cast<HBRUSH>(GetStockObject(HOLLOW_BRUSH));
	pDC->SetBkMode(TRANSPARENT);
	if (m_bUseCustomColor)
		pDC->SetTextColor(m_crText);

	return static_cast<HBRUSH>(GetStockObject(HOLLOW_BRUSH));
}

LRESULT CTransStatic::OnSetText(WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = Default();
	InvalidateParentArea();
	return lResult;
}

void CTransStatic::InvalidateParentArea()
{
	if (!::IsWindow(m_hWnd))
		return;

	CWnd* pParent = GetParent();
	if (pParent != nullptr && ::IsWindow(pParent->GetSafeHwnd()))
	{
		CRect rc;
		GetWindowRect(&rc);
		pParent->ScreenToClient(&rc);
		pParent->InvalidateRect(&rc, TRUE);
	}
}