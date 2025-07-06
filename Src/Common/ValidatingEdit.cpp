/** 
 * @file  ValidatingEdit.cpp
 *
 * @brief Implementation file for CValidatingEdit
 *
 */
#include "stdafx.h"
#include "ValidatingEdit.h"

IMPLEMENT_DYNAMIC(CValidatingEdit, CEdit)

#define ID_VALIDATE_TIMER 1001
#define VALIDATE_DELAY_MS 1000

CValidatingEdit::CValidatingEdit()
	: m_hasError(false)
	, m_errorBrush(RGB(255, 200, 200))
{
}

CValidatingEdit::~CValidatingEdit()
{
	m_errorBrush.DeleteObject();
}

BEGIN_MESSAGE_MAP(CValidatingEdit, CEdit)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_KILLFOCUS()
	ON_WM_TIMER()
END_MESSAGE_MAP()

void CValidatingEdit::Validate()
{
	CString text;
	GetWindowText(text);

	m_hasError = false;
	m_errorMessage.Empty();

	if (m_validator)
	{
		if (!m_toolTip.m_hWnd)
		{
			m_toolTip.Create(this, TTS_NOPREFIX);
			m_toolTip.AddTool(this, _T(""));
		}
		CString msg;
		if (!m_validator(text, msg))
		{
			m_hasError = true;
			m_errorMessage = msg;
			MSG msg2{ m_toolTip.m_hWnd, WM_MOUSEMOVE, 0, 0 };
			m_toolTip.UpdateTipText(m_errorMessage, this);
			m_toolTip.RelayEvent((MSG*)&msg2);
		}
		else
		{
			m_toolTip.UpdateTipText(_T(""), this);
			m_toolTip.Pop();
		}
	}

	Invalidate();
}

HBRUSH CValidatingEdit::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	if (m_hasError)
	{
		pDC->SetBkColor(RGB(255, 200, 200));
		return (HBRUSH)m_errorBrush.GetSafeHandle();
	}
	return nullptr;
}

BOOL CValidatingEdit::PreTranslateMessage(MSG* pMsg)
{
	if (m_toolTip.m_hWnd && pMsg->message != WM_TIMER)
		m_toolTip.RelayEvent(pMsg);
	return CEdit::PreTranslateMessage(pMsg);
}

void CValidatingEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);
	Validate();
}

void CValidatingEdit::OnEnChange()
{
	if (!m_hWnd)
		return;
	KillTimer(ID_VALIDATE_TIMER);
	SetTimer(ID_VALIDATE_TIMER, VALIDATE_DELAY_MS, nullptr);
}

void CValidatingEdit::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == ID_VALIDATE_TIMER)
	{
		Validate();
	}
	else
	{
		CEdit::OnTimer(nIDEvent);
	}
}
