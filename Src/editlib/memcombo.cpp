///////////////////////////////////////////////////////////////////////////
//  File:    memcombo.cpp
//  Version: 1.1.0.4
//  Updated: 19-Jul-1998
//
//  Copyright:  Ferdinand Prantl
//  E-mail:     prantl@ff.cuni.cz
//
//  Combo-box saving last typed expressions
//
//  You are free to use or modify this code to the following restrictions:
//  - Acknowledge me somewhere in your about box, simple "Parts of code by.."
//  will be enough. If you can't (or don't want to), contact me personally.
//  - LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "memcombo.h"
#include "registry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define REMEMBER_COUNT  64

#ifdef  _DEBUG
#include "memcombo.inl"
#endif // _DEBUG

#ifdef INLINE
#undef INLINE
#endif
#define INLINE

/////////////////////////////////////////////////////////////////////////////
// CMemComboBox

BEGIN_MESSAGE_MAP (CMemComboBox, CComboBox)
//{{AFX_MSG_MAP(CMemComboBox)
ON_WM_KILLFOCUS ()
ON_CONTROL_REFLECT (CBN_SETFOCUS, OnSetfocus)
ON_WM_DESTROY ()
//}}AFX_MSG_MAP
END_MESSAGE_MAP ()

/////////////////////////////////////////////////////////////////////////////
// CMemComboBox message handlers

CMap < CString, LPCTSTR, CString, LPCTSTR > CMemComboBox::groups;

void SetComboBoxHeight(CComboBox &Control)
{
	int			nHeight = Control.GetCount(), nMax = ::GetSystemMetrics(SM_CYSCREEN) - 48;
	CRect		rc;

	Control.GetClientRect(rc);
	Control.ClientToScreen(rc);
	nHeight = rc.Height() * nHeight + 16;
	if(rc.top + nHeight > nMax)
		nHeight = nMax - rc.top;
	Control.SetWindowPos(NULL, 0, 0, rc.Width(), nHeight, SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOREDRAW);
}

void SetComboBoxWidth(CComboBox &Control, LPCTSTR lpszText = NULL)
{
	int					cnt = Control.GetCount();

	if(!cnt)
		return;
	CClientDC			dc(&Control);
	NONCLIENTMETRICS	info;
	CFont				oFont, *oldFont;
	int					width = 0, nMax = ::GetSystemMetrics(SM_CXSCREEN) - 48;
	CRect				rc;
	CSize				size;

	memset(&info.lfMenuFont, 0, sizeof(LOGFONT));
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(info), &info, 0);
	info.lfMenuFont.lfHeight = -MulDiv(9, dc.GetDeviceCaps(LOGPIXELSY), 72);
	info.lfMenuFont.lfWidth = 0;
	info.lfMenuFont.lfWeight = FW_THIN;
	info.lfMenuFont.lfItalic = FALSE;
	info.lfMenuFont.lfUnderline = FALSE;
	info.lfMenuFont.lfCharSet = DEFAULT_CHARSET;
	info.lfMenuFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	info.lfMenuFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	info.lfMenuFont.lfQuality = DEFAULT_QUALITY;
	info.lfMenuFont.lfPitchAndFamily = FF_SWISS;
	_tcscpy(info.lfMenuFont.lfFaceName, _T("MS Sans Serif"));
	oFont.CreateFontIndirect(&info.lfMenuFont);
	oldFont = dc.SelectObject(&oFont);
	if(lpszText && *lpszText) {
		size = dc.GetTextExtent(lpszText);
		width = size.cx;
	} else {
		CString				item;

		for(int i = 0; i < cnt; i++) {
			Control.GetLBText(i, item);
			size = dc.GetTextExtent(item);
			if(size.cx > width)
				width = size.cx;
		}
	}
	Control.GetClientRect(rc);
	Control.ClientToScreen(rc);
	if(rc.left + width > nMax)
		width = nMax - rc.left;
	Control.SetDroppedWidth(width);
	dc.SelectObject(oldFont);
}

/*void FillComboBox(CComboBox &Control, const CList<CString, LPCTSTR> &Items, LPCTSTR lpszItem, BOOL bMustContains)
{
	POSITION	pos = Items.GetHeadPosition();
	CString		item, maxitem;
	int			maxlen = 0, len;

	Control.ResetContent();
	while(pos) {
		item = Items.GetNext(pos);
		len = item.GetLength();
		if(maxlen < len) {
			maxlen = len;
			maxitem = item;
		}
		Control.AddString(item);
	}
	if(Items.Find(lpszItem))
		Control.SelectString(-1, lpszItem);
	else if(!bMustContains)
		Control.SetWindowText(lpszItem);
	SetComboBoxHeight(Control);
	SetComboBoxWidth(Control, maxitem);
}*/

/*int ListToString (CString &sResult, CList<CString, LPCTSTR> listSource)
{
  POSITION pos = listSource.GetHeadPosition ();
  CString item;
  while (pos)
    {
      item = listSource.GetNext (pos);
      sResult += item + _T ('\n');
    }
  return listSource.GetCount ();
}*/

/*int StringToList (CList<CString, LPCTSTR> listResult, CString sSource)
{
  int pos;
  while ((pos = sSource.Find (_T ('\n'))) != -1)
    {
      listResult.AddTail (sSource.Left (pos));
      sSource = sSource.Mid (pos + 1);
    }
  if (*(LPCTSTR) sSource)
    {
      listResult.AddTail (sSource);
    }
  return listResult.GetCount ();
}*/

void CMemComboBox::FillCurrent ()
{
  CString strText;
  GetWindowText (strText);
  Fill (strText);
}

void CMemComboBox::
Fill (LPCTSTR text)
{
  if (text && *text)
    {
      int nPos = FindStringExact (-1, text);
      if (nPos  != CB_ERR)
        DeleteString (nPos);
      InsertString (0, text);
      int l = GetCount ();
      if (l > REMEMBER_COUNT)
        DeleteString (--l);
      SetComboBoxHeight (*this);
      SetComboBoxWidth (*this);
      SetCurSel (0);
      if (!m_sGroup.IsEmpty ())
        {
          CString item, items;
          for (int i = 0; i < l; i++)
            {
              GetLBText (i, item);
              items += item + _T ('\n');
            }
          groups.SetAt (m_sGroup, items);
        }
    }
}

void CMemComboBox::
LoadSettings ()
{
  CReg reg;
  if (reg.Open (HKEY_CURRENT_USER, _T ("SOFTWARE\\EditPad"), KEY_READ))
    {
      POSITION pos = groups.GetStartPosition ();
      static LPCTSTR name[] = { _T("FindText"), _T("ReplaceText") };
      CString value;

      for (int i = 0; i < countof (name); i++)
        {
          if (reg.LoadString (name[i], value))
            {
              groups.SetAt (name[i], value);
            }
        }
    }
}

void CMemComboBox::
SaveSettings ()
{
  CReg reg;
  if (reg.Create (HKEY_CURRENT_USER, _T ("SOFTWARE\\EditPad"), KEY_WRITE))
    {
      POSITION pos = groups.GetStartPosition ();
      CString name, value;

      while (pos)
        {
          groups.GetNextAssoc (pos, name, value);
          VERIFY (reg.SaveString (name, value));
        }
    }
}

void CMemComboBox::
OnKillFocus (CWnd * /*pNewWnd*/)
{
  FillCurrent ();
}

void CMemComboBox::
OnSetfocus ()
{
  if (m_bFirstFocus && !m_sGroup.IsEmpty ())
    {
      m_bFirstFocus = FALSE;
      CString items, strText;
      GetWindowText (strText);
      if (groups.Lookup (m_sGroup, items))
        {
          int p;
          while ((p = items.Find (_T ('\n'))) != -1)
            {
              AddString (items.Left (p));
              items = items.Mid (p + 1);
            }
          SetComboBoxHeight (*this);
          SetComboBoxWidth (*this);
          SetCurSel (0);
          if (!strText.IsEmpty ())
            SetWindowText (strText);
        }
    }
}

void CMemComboBox::
OnDestroy ()
{
  FillCurrent ();
  CComboBox::OnDestroy ();
}
