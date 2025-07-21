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
#define ID_UPDATE_TIMER 1002
#define VALIDATE_DELAY_MS 700
#define UPDATE_DELAY_MS 200

CValidatingEdit::CValidatingEdit()
	: m_hasError(false)
	, m_errorColor(RGB(255, 200, 200))
	, m_errorBrush(m_errorColor)
	, m_toolItem{}
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

void CValidatingEdit::SetBackColor(COLORREF color)
{
	m_errorColor = color;
	m_errorBrush.DeleteObject();
	m_errorBrush.CreateSolidBrush(m_errorColor);
}

void CValidatingEdit::Validate()
{
	CString text;
	GetWindowText(text);

	const CString prevErrorMessage = m_errorMessage;
	m_hasError = false;
	m_errorMessage.Empty();

	if (m_validator)
	{
		CString msg;
		if (!m_validator(text, msg))
		{
			m_hasError = true;
			m_errorMessage = msg;
		}
	}
	if (prevErrorMessage != m_errorMessage)
	{
		KillTimer(ID_UPDATE_TIMER);
		SetTimer(ID_UPDATE_TIMER, UPDATE_DELAY_MS, nullptr);
	}

	Invalidate();
}

HBRUSH CValidatingEdit::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	if (m_hasError)
	{
		pDC->SetBkColor(m_errorColor);
		return (HBRUSH)m_errorBrush.GetSafeHandle();
	}
	return nullptr;
}

BOOL CValidatingEdit::PreTranslateMessage(MSG* pMsg)
{
	if (m_toolTip.m_hWnd)
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
		KillTimer(ID_VALIDATE_TIMER);
		Validate();
		return;
	}
	if (nIDEvent == ID_UPDATE_TIMER)
	{
		KillTimer(ID_UPDATE_TIMER);

		if (m_hasError)
		{
			CRect rc;
			GetWindowRect(&rc);

			if (!m_toolTip.GetSafeHwnd())
			{
				m_toolTip.Create(this, TTS_NOPREFIX | TTS_BALLOON | TTS_ALWAYSTIP);
				m_toolItem.cbSize = sizeof(TOOLINFO);
				m_toolItem.uFlags = TTF_TRACK | TTF_ABSOLUTE;
				m_toolItem.hwnd = m_hWnd;
				m_toolItem.uId = 1;
				m_toolItem.lpszText = _T("");
				m_toolTip.SendMessage(TTM_ADDTOOL, 0, (LPARAM)&m_toolItem);
				m_toolTip.SetMaxTipWidth(rc.Width());
			}

			m_toolItem.lpszText = (LPTSTR)(LPCTSTR)m_errorMessage;
			m_toolTip.SetToolInfo(&m_toolItem);

			POINT pt{ rc.left, rc.bottom };
			m_toolTip.SendMessage(TTM_TRACKPOSITION, 0, (LPARAM)MAKELONG(pt.x, pt.y));

			m_toolTip.SendMessage(TTM_TRACKACTIVATE, TRUE, (LPARAM)&m_toolItem);
		}
		else
		{
			if (m_toolTip.GetSafeHwnd())
			{
				m_toolTip.UpdateTipText(_T(""), this);
				m_toolTip.SendMessage(TTM_TRACKACTIVATE, FALSE, (LPARAM)&m_toolItem);
			}
		}
		return;
	}
	CEdit::OnTimer(nIDEvent);
}

