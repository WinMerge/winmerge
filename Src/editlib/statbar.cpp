///////////////////////////////////////////////////////////////////////////
//  File:    statbar.cpp
//  Version: 1.1.0.4
//  Updated: 19-Jul-1998
//
//  Copyright:  Ferdinand Prantl, portions by Stcherbatchenko Andrei
//  E-mail:     prantl@ff.cuni.cz
//
//  Status bar extension
//
//  You are free to use or modify this code to the following restrictions:
//  - Acknowledge me somewhere in your about box, simple "Parts of code by.."
//  will be enough. If you can't (or don't want to), contact me personally.
//  - LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "editcmd.h"
#include "statbar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CEditStatusBar::CEditStatusBar ()
    :CStatusBar (), m_strClockFormat (_T("%d.%m. %H:%M:%S"))
{
}

CEditStatusBar:: ~ CEditStatusBar ()
{
}

BOOL CEditStatusBar::
Create (CWnd * pParentWnd, DWORD dwStyle, UINT nID)
{
  BOOL bCreatedOK = CStatusBar::Create (pParentWnd, dwStyle, nID);
  return bCreatedOK;
}

BOOL CEditStatusBar::
SetPaneFormattedText (int nIndex, BOOL bUpdate, LPCTSTR lpszFmt,...)
{
  TCHAR buffer[256];
  va_list argptr;
  va_start (argptr, lpszFmt);
  _vstprintf (buffer, lpszFmt, argptr);
  va_end (argptr);
  BOOL bResult = SetPaneText (nIndex, buffer, bUpdate);
  UpdateWindow ();
  return bResult;
}

BOOL CEditStatusBar::
SetPaneFormattedText (int nIndex, BOOL bUpdate, UINT nId,...)
{
  CString str;
  if (str.LoadString (nId))
    {
      va_list argptr;
      va_start (argptr, nId);
      BOOL bResult = SetPaneFormattedText (nIndex, bUpdate, str, argptr);
      va_end (argptr);
      return bResult;
    }
  SetPaneText (nIndex, _T (""), bUpdate);
  return FALSE;
}

BOOL CEditStatusBar::
SetPaneText (int nIndex, LPCTSTR lpszNewText, BOOL bUpdate /*= TRUE*/ )
{
  return CStatusBar::SetPaneText (nIndex, lpszNewText, bUpdate);
}

BOOL CEditStatusBar::
SetPaneText (int nIndex, UINT nId, BOOL bUpdate /*= TRUE*/ )
{
  CString str;
  if (str.LoadString (nId))
    return SetPaneText (nIndex, str, bUpdate);
  SetPaneText (nIndex, _T (""), bUpdate);
  return FALSE;
}

void CEditStatusBar::
SetClockFormat (LPCTSTR strClockFormat)
{
  m_strClockFormat = strClockFormat;
}

IMPLEMENT_DYNCREATE (CEditStatusBar, CStatusBar)

BEGIN_MESSAGE_MAP (CEditStatusBar, CStatusBar)
//{{AFX_MSG_MAP(CEditStatusBar)
ON_WM_CREATE ()
ON_WM_DESTROY ()
ON_UPDATE_COMMAND_UI (ID_INDICATOR_TIME, OnUpdateIndicatorTime)
//}}AFX_MSG_MAP
END_MESSAGE_MAP ()

int CEditStatusBar::OnCreate (LPCREATESTRUCT lpCreateStruct)
{
  // make sure time gets updated every second, even when idle
  if (CStatusBar::OnCreate (lpCreateStruct) == -1)
    return -1;
  SetFont (CFont::FromHandle ((HFONT) GetStockObject (DEFAULT_GUI_FONT)));
  SetTimer (ID_INDICATOR_TIME, 1000, NULL);
  return 0;
}

void CEditStatusBar::
OnUpdateIndicatorTime (CCmdUI * pCmdUI)
{
  pCmdUI->Enable (true);
  pCmdUI->SetText (CTime::GetCurrentTime ().Format (m_strClockFormat));
}

void CEditStatusBar::
OnDestroy ()
{
  KillTimer (ID_INDICATOR_TIME);
  CStatusBar::OnDestroy ();
}
