////////////////////////////////////////////////////////////////////////////
//  File:       ccrystaltextview.cpp
//  Version:    1.2.0.5
//  Created:    29-Dec-1998
//
//  Author:     Stcherbatchenko Andrei
//  E-mail:     windfall@gmx.de
//
//  Implementation of the CCrystalTextView class, a part of Crystal Edit -
//  syntax coloring text editor.
//
//  You are free to use or modify this code to the following restrictions:
//  - Acknowledge me somewhere in your about box, simple "Parts of code by.."
//  will be enough. If you can't (or don't want to), contact me personally.
//  - LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//  17-Feb-99
//  FIX:    missing UpdateCaret() in CCrystalTextView::SetFont
//  FIX:    missing UpdateCaret() in CCrystalTextView::RecalcVertScrollBar
//  FIX:    mistype in CCrystalTextView::RecalcPageLayouts + instead of +=
//  FIX:    removed condition 'm_nLineHeight < 20' in
//      CCrystalTextView::CalcLineCharDim(). This caused painting defects
//      when using very small fonts.
//
//  FEATURE:    Some experiments with smooth scrolling, controlled by
//      m_bSmoothScroll member variable, by default turned off.
//      See ScrollToLine function for implementation details.
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//  21-Feb-99
//      Paul Selormey, James R. Twine
//  +   FEATURE: description for Undo/Redo actions
//  +   FEATURE: multiple MSVC-like bookmarks
//  +   FEATURE: 'Disable backspace at beginning of line' option
//  +   FEATURE: 'Disable drag-n-drop editing' option
//
//  +   FIX:  ResetView() now virtual
//  +   FEATURE: Added OnEditOperation() virtual: base for auto-indent,
//      smart indent etc.
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//  19-Jul-99
//      Ferdinand Prantl:
//  +   FEATURE: regular expressions, go to line and things ...
//  +   FEATURE: plenty of syntax highlighting definitions
//  +   FEATURE: corrected bug in syntax highlighting C comments
//  +   FEATURE: extended registry support for saving settings
//  +   FEATURE: some other things I've forgotten ...
//
//  ... it's being edited very rapidly so sorry for non-commented
//        and maybe "ugly" code ...
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//	01-Jun-99 to 31-Aug-99
//		Sven Wiegand (search for "//BEGIN SW" to find my changes):
//
//	+ FEATURE: support for language switching on the fly with class 
//			CCrystalParser
//	+	FEATURE: word wrapping
//	+ FIX:	Setting m_nIdealCharPos, when choosing cursor position by mouse
//	+ FIX:	Backward search
//	+ FEATURE: incremental search
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//	24-Oct-99
//		Sven Wiegand
//
//	+ FIX: Opening large files won't crash anymore and will go very fast
//	       (removed call to RecalcVertScrollBar() in WrapLineCached())
//	+ FIX: Problems with repainting and cursor-position by resizing window 
//	       fixed by adding call to ScrollToSubLine() in OnSize().
//	+ FEATURE: Supporting [Return] to exit incremental-search-mode
//		     (see OnChar())
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <malloc.h>
#include "editcmd.h"
#include "editreg.h"
#include "ccrystaltextview.h"
#include "ccrystaltextbuffer.h"
#include "cfindtextdlg.h"
#include "fpattern.h"
#include "filesup.h"
#include "registry.h"
#include "gotodlg.h"
#include "Merge.h"

#ifndef __AFXPRIV_H__
#pragma message("Include <afxpriv.h> in your stdafx.h to avoid this message")
#include <afxpriv.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// These maybe should be converted with some winapi function to make sure
// they are printable and not garbage on different fonts...
// (These are for Latin1)
#define TAB_CHARACTER               _T('\xBB') // U+BB: RIGHT POINTING DOUBLE ANGLE QUOTATION MARK
#define SPACE_CHARACTER             _T('\xB7') // U+B7: MIDDLE DOT
#define CR_CHARACTER                _T('\xA7') // U+A7: SECTION SIGN
#define LF_CHARACTER                _T('\xB6') // U+B6: PILCROW SIGN

#define DEFAULT_PRINT_MARGIN        1000    //  10 millimeters

const UINT	MAX_TAB_LEN	= 64; 
#define SMOOTH_SCROLL_FACTOR        6

#pragma warning ( disable : 4100 )
////////////////////////////////////////////////////////////////////////////
// CCrystalTextView

LOGFONT CCrystalTextView::m_LogFont;

IMPLEMENT_DYNCREATE (CCrystalTextView, CView)

HINSTANCE CCrystalTextView::s_hResourceInst = NULL;

BEGIN_MESSAGE_MAP (CCrystalTextView, CView)
//{{AFX_MSG_MAP(CCrystalTextView)
ON_WM_DESTROY ()
ON_WM_ERASEBKGND ()
ON_WM_SIZE ()
ON_WM_VSCROLL ()
ON_WM_SETCURSOR ()
ON_WM_LBUTTONDOWN ()
ON_WM_SETFOCUS ()
ON_WM_HSCROLL ()
ON_WM_LBUTTONUP ()
ON_WM_MOUSEMOVE ()
ON_WM_TIMER ()
ON_WM_KILLFOCUS ()
ON_WM_LBUTTONDBLCLK ()
ON_COMMAND (ID_EDIT_COPY, OnEditCopy)
ON_UPDATE_COMMAND_UI (ID_EDIT_COPY, OnUpdateEditCopy)
ON_COMMAND (ID_EDIT_SELECT_ALL, OnEditSelectAll)
ON_UPDATE_COMMAND_UI (ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
ON_WM_RBUTTONDOWN ()
ON_WM_SYSCOLORCHANGE ()
ON_WM_CREATE ()
ON_COMMAND (ID_EDIT_FIND, OnEditFind)
ON_COMMAND (ID_EDIT_REPEAT, OnEditRepeat)
ON_UPDATE_COMMAND_UI (ID_EDIT_REPEAT, OnUpdateEditRepeat)
ON_COMMAND (ID_EDIT_FIND_PREVIOUS, OnEditFindPrevious)
ON_UPDATE_COMMAND_UI (ID_EDIT_FIND_PREVIOUS, OnUpdateEditFindPrevious)
ON_WM_MOUSEWHEEL ()
//}}AFX_MSG_MAP
ON_COMMAND (ID_EDIT_CHAR_LEFT, OnCharLeft)
ON_COMMAND (ID_EDIT_EXT_CHAR_LEFT, OnExtCharLeft)
ON_COMMAND (ID_EDIT_CHAR_RIGHT, OnCharRight)
ON_COMMAND (ID_EDIT_EXT_CHAR_RIGHT, OnExtCharRight)
ON_COMMAND (ID_EDIT_WORD_LEFT, OnWordLeft)
ON_COMMAND (ID_EDIT_EXT_WORD_LEFT, OnExtWordLeft)
ON_COMMAND (ID_EDIT_WORD_RIGHT, OnWordRight)
ON_COMMAND (ID_EDIT_EXT_WORD_RIGHT, OnExtWordRight)
ON_COMMAND (ID_EDIT_LINE_UP, OnLineUp)
ON_COMMAND (ID_EDIT_EXT_LINE_UP, OnExtLineUp)
ON_COMMAND (ID_EDIT_LINE_DOWN, OnLineDown)
ON_COMMAND (ID_EDIT_EXT_LINE_DOWN, OnExtLineDown)
ON_COMMAND (ID_EDIT_SCROLL_UP, ScrollUp)
ON_COMMAND (ID_EDIT_SCROLL_DOWN, ScrollDown)
ON_COMMAND (ID_EDIT_PAGE_UP, OnPageUp)
ON_COMMAND (ID_EDIT_EXT_PAGE_UP, OnExtPageUp)
ON_COMMAND (ID_EDIT_PAGE_DOWN, OnPageDown)
ON_COMMAND (ID_EDIT_EXT_PAGE_DOWN, OnExtPageDown)
ON_COMMAND (ID_EDIT_LINE_END, OnLineEnd)
ON_COMMAND (ID_EDIT_EXT_LINE_END, OnExtLineEnd)
ON_COMMAND (ID_EDIT_HOME, OnHome)
ON_COMMAND (ID_EDIT_EXT_HOME, OnExtHome)
ON_COMMAND (ID_EDIT_TEXT_BEGIN, OnTextBegin)
ON_COMMAND (ID_EDIT_EXT_TEXT_BEGIN, OnExtTextBegin)
ON_COMMAND (ID_EDIT_TEXT_END, OnTextEnd)
ON_COMMAND (ID_EDIT_EXT_TEXT_END, OnExtTextEnd)
//  Standard printing commands
ON_COMMAND (ID_FILE_PAGE_SETUP, OnFilePageSetup)
ON_COMMAND (ID_FILE_PRINT, CView::OnFilePrint)
ON_COMMAND (ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
ON_COMMAND (ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
//  Status
ON_UPDATE_COMMAND_UI (ID_EDIT_INDICATOR_CRLF, OnUpdateIndicatorCRLF)
ON_UPDATE_COMMAND_UI (ID_EDIT_INDICATOR_POSITION, OnUpdateIndicatorPosition)
//  Bookmarks
ON_COMMAND_RANGE (ID_EDIT_TOGGLE_BOOKMARK0, ID_EDIT_TOGGLE_BOOKMARK9, OnToggleBookmark)
ON_COMMAND_RANGE (ID_EDIT_GO_BOOKMARK0, ID_EDIT_GO_BOOKMARK9, OnGoBookmark)
ON_COMMAND (ID_EDIT_CLEAR_BOOKMARKS, OnClearBookmarks)
// More Bookmarks
ON_COMMAND (ID_EDIT_TOGGLE_BOOKMARK, OnToggleBookmark)
ON_COMMAND (ID_EDIT_GOTO_NEXT_BOOKMARK, OnNextBookmark)
ON_COMMAND (ID_EDIT_GOTO_PREV_BOOKMARK, OnPrevBookmark)
ON_COMMAND (ID_EDIT_CLEAR_ALL_BOOKMARKS, OnClearAllBookmarks)
ON_UPDATE_COMMAND_UI (ID_EDIT_GOTO_NEXT_BOOKMARK, OnUpdateNextBookmark)
ON_UPDATE_COMMAND_UI (ID_EDIT_GOTO_PREV_BOOKMARK, OnUpdatePrevBookmark)
ON_UPDATE_COMMAND_UI (ID_EDIT_CLEAR_ALL_BOOKMARKS, OnUpdateClearAllBookmarks)
// Ferdi's source type chnages
ON_COMMAND_RANGE (ID_SOURCE_PLAIN, ID_SOURCE_TEX, OnSourceType)
ON_UPDATE_COMMAND_UI_RANGE (ID_SOURCE_PLAIN, ID_SOURCE_TEX, OnUpdateSourceType)
ON_COMMAND (ID_EDIT_MATCHBRACE, OnMatchBrace)
ON_UPDATE_COMMAND_UI (ID_EDIT_MATCHBRACE, OnUpdateMatchBrace)
ON_COMMAND (ID_EDIT_GOTO, OnEditGoTo)
ON_UPDATE_COMMAND_UI (ID_VIEW_TOGGLE_SRC_HDR, OnUpdateToggleSourceHeader)
ON_COMMAND (ID_VIEW_TOGGLE_SRC_HDR, OnToggleSourceHeader)
ON_UPDATE_COMMAND_UI (ID_VIEW_SELMARGIN, OnUpdateSelMargin)
ON_COMMAND (ID_VIEW_SELMARGIN, OnSelMargin)
ON_UPDATE_COMMAND_UI (ID_VIEW_WORDWRAP, OnUpdateWordWrap)
ON_COMMAND (ID_VIEW_WORDWRAP, OnWordWrap)
ON_COMMAND (ID_FORCE_REDRAW, OnForceRedraw)
  //BEGIN SW
  // incremental search
  ON_COMMAND(ID_EDIT_FIND_INCREMENTAL_FORWARD, OnEditFindIncrementalForward)
  ON_COMMAND(ID_EDIT_FIND_INCREMENTAL_BACKWARD, OnEditFindIncrementalBackward)
  ON_UPDATE_COMMAND_UI(ID_EDIT_FIND_INCREMENTAL_FORWARD, OnUpdateEditFindIncrementalForward)
  ON_UPDATE_COMMAND_UI(ID_EDIT_FIND_INCREMENTAL_BACKWARD, OnUpdateEditFindIncrementalBackward)
  //END SW
END_MESSAGE_MAP ()

#define EXPAND_PRIMITIVE(impl, func)    \
void CCrystalTextView::On##func() { impl(FALSE); }  \
void CCrystalTextView::OnExt##func() { impl(TRUE); }
EXPAND_PRIMITIVE (MoveLeft, CharLeft)
EXPAND_PRIMITIVE (MoveRight, CharRight)
EXPAND_PRIMITIVE (MoveWordLeft, WordLeft)
EXPAND_PRIMITIVE (MoveWordRight, WordRight)
EXPAND_PRIMITIVE (MoveUp, LineUp)
EXPAND_PRIMITIVE (MoveDown, LineDown)
EXPAND_PRIMITIVE (MovePgUp, PageUp)
EXPAND_PRIMITIVE (MovePgDn, PageDown)
EXPAND_PRIMITIVE (MoveHome, Home)
EXPAND_PRIMITIVE (MoveEnd, LineEnd)
EXPAND_PRIMITIVE (MoveCtrlHome, TextBegin)
EXPAND_PRIMITIVE (MoveCtrlEnd, TextEnd)
#undef EXPAND_PRIMITIVE

CCrystalTextView::TextDefinition CCrystalTextView::m_SourceDefs[] =
  {
    CCrystalTextView::SRC_PLAIN, _T ("Plain"), _T ("txt,doc,diz"), CCrystalTextView::ParseLinePlain, SRCOPT_AUTOINDENT, 4, _T (""), _T (""), _T (""), (DWORD)-1,
    CCrystalTextView::SRC_ASP, _T ("ASP"), _T ("asp"), CCrystalTextView::ParseLineAsp, SRCOPT_AUTOINDENT|SRCOPT_BRACEGNU, 2, _T (""), _T (""), _T ("'"), (DWORD)-1,
    CCrystalTextView::SRC_BASIC, _T ("Basic"), _T ("bas,vb,vbs,frm,dsm"), CCrystalTextView::ParseLineBasic, SRCOPT_AUTOINDENT, 4, _T (""), _T (""), _T ("\'"), (DWORD)-1,
    CCrystalTextView::SRC_BATCH, _T ("Batch"), _T ("bat,btm,cmd"), CCrystalTextView::ParseLineBatch, SRCOPT_INSERTTABS|SRCOPT_AUTOINDENT, 4, _T (""), _T (""), _T ("rem "), (DWORD)-1,
    CCrystalTextView::SRC_C, _T ("C"), _T ("c,cc,cpp,cxx,h,hpp,hxx,hm,inl,rh,tlh,tli,xs"), CCrystalTextView::ParseLineC, SRCOPT_AUTOINDENT|SRCOPT_BRACEGNU, 2, _T ("/*"), _T ("*/"), _T ("//"), (DWORD)-1,
    CCrystalTextView::SRC_DCL, _T ("DCL"), _T ("dcl,dcc"), CCrystalTextView::ParseLineDcl, SRCOPT_AUTOINDENT|SRCOPT_BRACEGNU, 2, _T ("/*"), _T ("*/"), _T ("//"), (DWORD)-1,
    CCrystalTextView::SRC_FORTRAN, _T ("Fortran"), _T ("f,f90,f9p,fpp,for,f77"), CCrystalTextView::ParseLineFortran, SRCOPT_INSERTTABS|SRCOPT_AUTOINDENT, 8, _T (""), _T (""), _T ("!"), (DWORD)-1,
    CCrystalTextView::SRC_HTML, _T ("HTML"), _T ("html,htm,shtml,ihtml,ssi,phtml,stm,stml"), CCrystalTextView::ParseLineHtml, SRCOPT_AUTOINDENT|SRCOPT_BRACEANSI, 2, _T ("<!--"), _T ("-->"), _T (""), (DWORD)-1,
    CCrystalTextView::SRC_INSTALLSHIELD, _T ("InstallShield"), _T ("rul"), CCrystalTextView::ParseLineIS, SRCOPT_AUTOINDENT|SRCOPT_BRACEANSI, 2, _T ("/*"), _T ("*/"), _T ("//"), (DWORD)-1,
    CCrystalTextView::SRC_JAVA, _T ("Java"), _T ("java,jav,js"), CCrystalTextView::ParseLineJava, SRCOPT_AUTOINDENT|SRCOPT_BRACEGNU, 2, _T ("/*"), _T ("*/"), _T ("//"), (DWORD)-1,
    CCrystalTextView::SRC_LISP, _T ("AutoLISP"), _T ("lsp"), CCrystalTextView::ParseLineLisp, SRCOPT_AUTOINDENT|SRCOPT_BRACEANSI, 2, _T (";|"), _T ("|;"), _T (";"), (DWORD)-1,
    CCrystalTextView::SRC_PASCAL, _T ("Pascal"), _T ("pas"), CCrystalTextView::ParseLinePascal, SRCOPT_AUTOINDENT|SRCOPT_BRACEGNU, 2, _T ("{"), _T ("}"), _T (""), (DWORD)-1,
    CCrystalTextView::SRC_PERL, _T ("Perl"), _T ("pl"), CCrystalTextView::ParseLinePerl, SRCOPT_AUTOINDENT|SRCOPT_EOLNUNIX, 4, _T (""), _T (""), _T ("#"), (DWORD)-1,
    CCrystalTextView::SRC_PHP, _T ("PHP"), _T ("php,php3,php4"), CCrystalTextView::ParseLinePhp, SRCOPT_AUTOINDENT|SRCOPT_BRACEGNU, 2, _T ("/*"), _T ("*/"), _T ("//"), (DWORD)-1,
    CCrystalTextView::SRC_PYTHON, _T ("Python"), _T ("py"), CCrystalTextView::ParseLinePython, SRCOPT_AUTOINDENT|SRCOPT_BRACEGNU, 2, _T ("/*"), _T ("*/"), _T ("//"), (DWORD)-1,
    CCrystalTextView::SRC_REXX, _T ("REXX"), _T ("rex,rexx,cmd"), CCrystalTextView::ParseLineRexx, SRCOPT_AUTOINDENT, 4, _T ("/*"), _T ("*/"), _T ("//"), (DWORD)-1,
    CCrystalTextView::SRC_RSRC, _T ("Resources"), _T ("rc,dlg,r16,r32,rc2"), CCrystalTextView::ParseLineRsrc, SRCOPT_AUTOINDENT, 4, _T ("/*"), _T ("*/"), _T ("//"), (DWORD)-1,
    CCrystalTextView::SRC_SGML, _T ("Sgml"), _T ("sgml"), CCrystalTextView::ParseLineSgml, SRCOPT_AUTOINDENT|SRCOPT_BRACEANSI, 2, _T ("<!--"), _T ("-->"), _T (""), (DWORD)-1,
    CCrystalTextView::SRC_SH, _T ("Shell"), _T ("sh,conf"), CCrystalTextView::ParseLineSh, SRCOPT_INSERTTABS|SRCOPT_AUTOINDENT|SRCOPT_EOLNUNIX, 4, _T (""), _T (""), _T ("#"), (DWORD)-1,
    CCrystalTextView::SRC_SIOD, _T ("SIOD"), _T ("scm"), CCrystalTextView::ParseLineSiod, SRCOPT_AUTOINDENT|SRCOPT_BRACEGNU, 2, _T (";|"), _T ("|;"), _T (";"), (DWORD)-1,
    CCrystalTextView::SRC_SQL, _T ("SQL"), _T ("sql"), CCrystalTextView::ParseLineSql, SRCOPT_AUTOINDENT, 4, _T ("/*"), _T ("*/"), _T ("//"), (DWORD)-1,
    CCrystalTextView::SRC_TCL, _T ("TCL"), _T ("tcl"), CCrystalTextView::ParseLineTcl, SRCOPT_AUTOINDENT|SRCOPT_BRACEGNU|SRCOPT_EOLNUNIX, 2, _T (""), _T (""), _T ("#"), (DWORD)-1,
    CCrystalTextView::SRC_TEX, _T ("TEX"), _T ("tex,sty,cls,clo,ltx,fd,dtx"), CCrystalTextView::ParseLineTex, SRCOPT_AUTOINDENT, 4, _T (""), _T (""), _T ("%"), (DWORD)-1
  };

/////////////////////////////////////////////////////////////////////////////
// CCrystalTextView construction/destruction

BOOL
MatchType (CString pattern, LPCTSTR lpszExt)
{
  CString part;
  int pos, len = pattern.GetLength ();

  while ((pos = pattern.Find (_T (','))) != -1)
    {
      part = pattern.Left (pos);
      if (!part.IsEmpty () && fpattern_isvalid (part))
        {
          if (fpattern_matchn (part, lpszExt))
            {
              return TRUE;
            }
        }
      len -= pos + 1;
      pattern = pattern.Right (len);
    }
  if (!pattern.IsEmpty () && fpattern_isvalid (pattern))
    {
      if (fpattern_matchn (pattern, lpszExt))
        {
          return TRUE;
        }
    }
  return FALSE;
}

bool CCrystalTextView::
DoSetTextType (TextDefinition *def)
{
  m_CurSourceDef = def;
  SetFlags (def->flags);

// Do not set these
// EOL is determined from file, tabsize and viewtabs are
// global WinMerge settings, selection margin is not needed
// and wordwrapping must be false always
#if 0
  SetWordWrapping ((def->flags & SRCOPT_WORDWRAP) != FALSE);
  SetSelectionMargin ((def->flags & SRCOPT_SELMARGIN) != FALSE);
  SetTabSize (def->tabsize);
  SetViewTabs ((def->flags & SRCOPT_SHOWTABS) != FALSE);
  int nEoln;
  if (def->flags & SRCOPT_EOLNDOS)
    {
      nEoln = 0;
    }
  else if (def->flags & SRCOPT_EOLNUNIX)
    {
      nEoln = 1;
    }
  else if (def->flags & SRCOPT_EOLNMAC)
    {
      nEoln = 2;
    }
  else /* eoln auto */
    {
      nEoln = -1;
    }
  SetCRLFMode (nEoln);
#endif
  return true;
}

CCrystalTextView::TextDefinition* CCrystalTextView::
GetTextType (LPCTSTR pszExt)
{
  TextDefinition *def;
  CString sExt = pszExt;

  def = CCrystalTextView::m_SourceDefs;
  sExt.MakeLower ();
  for (int i = 0; i < countof (CCrystalTextView::m_SourceDefs); i++, def++)
    if (MatchType (def->exts, sExt))
      return def;
  return NULL;
}

bool CCrystalTextView::
SetTextType (LPCTSTR pszExt)
{
  m_CurSourceDef = m_SourceDefs;

  TextDefinition *def = GetTextType (pszExt);

  return SetTextType (def);
}

bool CCrystalTextView::
SetTextType (CCrystalTextView::TextType enuType)
{
  TextDefinition *def;

  m_CurSourceDef = def = m_SourceDefs;
  for (int i = 0; i < countof (m_SourceDefs); i++, def++)
    {
      if (def->type == enuType)
        {
          return SetTextType (def);
        }
    }
  return false;
}

bool CCrystalTextView::
SetTextType (CCrystalTextView::TextDefinition *def)
{
  if (def)
    if (m_CurSourceDef != def)
      return DoSetTextType (def);
    else
      return true;
  return false;
}

void CCrystalTextView::
LoadSettings ()
{
  TextDefinition *def = m_SourceDefs;
  bool bFontLoaded;
  CReg reg;
  if (reg.Open (HKEY_CURRENT_USER, REG_EDITPAD, KEY_READ))
    {
      reg.LoadNumber (_T ("DefaultEncoding"), (DWORD*) &CCrystalTextBuffer::m_nDefaultEncoding);
      for (int i = 0; i < countof (m_SourceDefs); i++, def++)
        {
          CReg reg1;
          if (reg1.Open (reg.hKey, def->name, KEY_READ))
            {
              reg1.LoadString (_T ("Extensions"), def->exts, countof (def->exts));
              reg1.LoadNumber (_T ("Flags"), &def->flags);
//              reg1.LoadNumber (_T ("TabSize"), &def->tabsize);
              reg1.LoadString (_T ("OpenComment"), def->opencomment, countof (def->opencomment));
              reg1.LoadString (_T ("CloseComment"), def->closecomment, countof (def->closecomment));
              reg1.LoadString (_T ("CommentLine"), def->commentline, countof (def->commentline));
              reg1.LoadNumber (_T ("DefaultEncoding"), &def->encoding);
            }
        }
      bFontLoaded = reg.LoadBinary (_T ("LogFont"), (LPBYTE) &m_LogFont, sizeof (m_LogFont));
    }
  else
    bFontLoaded = false;
  if (!bFontLoaded)
    {
      CWindowDC dc (CWnd::GetDesktopWindow ());
      NONCLIENTMETRICS info;
      info.cbSize = sizeof(info);
      SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(info), &info, 0);
      memcpy (&m_LogFont, &info.lfMessageFont, sizeof (LOGFONT));
      m_LogFont.lfHeight = -MulDiv (11, dc.GetDeviceCaps (LOGPIXELSY), 72);
      m_LogFont.lfWeight = FW_NORMAL;
      m_LogFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
      m_LogFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
      m_LogFont.lfQuality = DEFAULT_QUALITY;
      m_LogFont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
      _tcscpy (m_LogFont.lfFaceName, _T ("Courier New"));
    }
}

void CCrystalTextView::
SaveSettings ()
{
  TextDefinition *def = m_SourceDefs;
  CReg reg;
  if (reg.Create (HKEY_CURRENT_USER, REG_EDITPAD, KEY_WRITE))
    {
      VERIFY (reg.SaveNumber (_T ("DefaultEncoding"), (DWORD) CCrystalTextBuffer::m_nDefaultEncoding));
      for (int i = 0; i < countof (m_SourceDefs); i++, def++)
        {
          CReg reg1;
          if (reg1.Create (reg.hKey, def->name, KEY_WRITE))
            {
              VERIFY (reg1.SaveString (_T ("Extensions"), def->exts));
              VERIFY (reg1.SaveNumber (_T ("Flags"), def->flags));
//              VERIFY (reg1.SaveNumber (_T ("TabSize"), def->tabsize));
              VERIFY (reg1.SaveString (_T ("OpenComment"), def->opencomment));
              VERIFY (reg1.SaveString (_T ("CloseComment"), def->closecomment));
              VERIFY (reg1.SaveString (_T ("CommentLine"), def->commentline));
              VERIFY (reg1.SaveNumber (_T ("DefaultEncoding"), def->encoding));
            }
        }
      VERIFY (reg.SaveBinary (_T ("LogFont"), (LPBYTE) &m_LogFont, sizeof (m_LogFont)));
    }
}

CCrystalTextView::CCrystalTextView ()
{
  AFX_ZERO_INIT_OBJECT (CView);
  m_rxnode = NULL;
  m_pszMatched = NULL;
  m_bSelMargin = FALSE;
  m_bWordWrap = FALSE;
  m_bDragSelection = FALSE;
  //BEGIN SW
  m_panSubLines = new CArray<int, int>();
  ASSERT( m_panSubLines );
  m_panSubLines->SetSize( 0, 4096 );

  m_pstrIncrementalSearchString = new CString;
  ASSERT( m_pstrIncrementalSearchString );
  m_pstrIncrementalSearchStringOld = new CString;
  ASSERT( m_pstrIncrementalSearchStringOld );
  //END SW
  ResetView ();
  SetTextType (SRC_PLAIN);
  m_bSingle = false; // needed to be set in descendat classes
  m_bRememberLastPos = false;
}

CCrystalTextView::~CCrystalTextView ()
{
  ASSERT (m_hAccel == NULL);
  ASSERT (m_pCacheBitmap == NULL);
  ASSERT (m_pTextBuffer == NULL);   //  Must be correctly detached
  
  if (m_pszLastFindWhat != NULL)
    {
      free (m_pszLastFindWhat);
      m_pszLastFindWhat=NULL;
    }
  if (m_pdwParseCookies != NULL)
    {
      delete[] m_pdwParseCookies;
      m_pdwParseCookies = NULL;
    }
  if (m_pnActualLineLength != NULL)
    {
      delete[] m_pnActualLineLength;
      m_pnActualLineLength = NULL;
    }
  if (m_rxnode)
    {
      RxFree (m_rxnode);
      m_rxnode = NULL;
    }
  if (m_pszMatched)
    {
      delete[] m_pszMatched;
      m_pszMatched = NULL;
    }
  //BEGIN SW
  if( m_panSubLines )
    {
      delete m_panSubLines;
      m_panSubLines = NULL;
    }
  if( m_pstrIncrementalSearchString )
    {
      delete m_pstrIncrementalSearchString;
      m_pstrIncrementalSearchString = NULL;
    }
  if( m_pstrIncrementalSearchStringOld )
    {
      delete m_pstrIncrementalSearchStringOld;
      m_pstrIncrementalSearchStringOld = NULL;
    }
  //END SW
}

BOOL CCrystalTextView::
PreCreateWindow (CREATESTRUCT & cs)
{
  CWnd *pParentWnd = CWnd::FromHandlePermanent (cs.hwndParent);
  if (pParentWnd == NULL || !pParentWnd->IsKindOf (RUNTIME_CLASS (CSplitterWnd)))
    {
      //  View must always create its own scrollbars,
      //  if only it's not used within splitter
    //BEGIN SW
    if( m_bWordWrap )
      // we do not need a horizontal scroll bar, if we wrap the lines
      cs.style|= WS_VSCROLL;
    else
      cs.style |= (WS_HSCROLL | WS_VSCROLL);
    /*ORIGINAL
    cs.style |= (WS_HSCROLL | WS_VSCROLL);
    */
    //END SW
    }
  cs.lpszClass = AfxRegisterWndClass (CS_DBLCLKS);
  return CView::PreCreateWindow (cs);
}


/////////////////////////////////////////////////////////////////////////////
// CCrystalTextView drawing

void CCrystalTextView::
GetSelection (CPoint & ptStart, CPoint & ptEnd)
{
  PrepareSelBounds ();
  ptStart = m_ptDrawSelStart;
  ptEnd = m_ptDrawSelEnd;
}

CCrystalTextBuffer *CCrystalTextView::
LocateTextBuffer ()
{
  return NULL;
}

int CCrystalTextView::
GetLineActualLength (int nLineIndex)
{
  const int nLineCount = GetLineCount ();
  ASSERT (nLineCount > 0);
  ASSERT (nLineIndex >= 0 && nLineIndex < nLineCount);
  if (m_pnActualLineLength == NULL)
    {
      m_pnActualLineLength = new int[nLineCount];
      ASSERT( m_pnActualLineLength );
      if (!m_pnActualLineLength)
        return 0;		// TODO: what to do if alloc fails...???
      memset (m_pnActualLineLength, 0xff, sizeof (int) * nLineCount);
      m_nActualLengthArraySize = nLineCount;
    }

  if (m_pnActualLineLength[nLineIndex] >= 0)
    return m_pnActualLineLength[nLineIndex];

  //  Actual line length is not determined yet, let's calculate a little
  int nActualLength = 0;
  int nLength = GetLineLength (nLineIndex);
  if (nLength > 0)
    {
      LPCTSTR pszLine = GetLineChars (nLineIndex);
      LPTSTR pszChars = new TCHAR[nLength + 1];
      ASSERT( pszChars );
      if (!pszChars)
        return 0;		// TODO: what to do if alloc fails...???
      memcpy (pszChars, pszLine, sizeof (TCHAR) * nLength);
      pszChars[nLength] = 0;
      LPTSTR pszCurrent = pszChars;

      const int nTabSize = GetTabSize ();
      int ind = 0;
      nLength--;

      while (ind < nLength)
        {
           if (pszCurrent[ind] == _T('\t'))
          nActualLength += (nTabSize - nActualLength % nTabSize);
           else
             nActualLength++;

           ind++;
            }

      delete[] pszChars;
    }

  m_pnActualLineLength[nLineIndex] = nActualLength;
  return nActualLength;
}

void CCrystalTextView::
ScrollToChar (int nNewOffsetChar, BOOL bNoSmoothScroll /*= FALSE*/ , BOOL bTrackScrollBar /*= TRUE*/ )
{
  //BEGIN SW
  // no horizontal scrolling, when word wrapping is enabled
  if( m_bWordWrap )
    return;
  //END SW
  
  //  For now, ignoring bNoSmoothScroll and m_bSmoothScroll
  if (m_nOffsetChar != nNewOffsetChar)
    {
      int nScrollChars = m_nOffsetChar - nNewOffsetChar;
      m_nOffsetChar = nNewOffsetChar;
      CRect rcScroll;
      GetClientRect (&rcScroll);
      rcScroll.left += GetMarginWidth ();
      ScrollWindow (nScrollChars * GetCharWidth (), 0, &rcScroll, &rcScroll);
      UpdateWindow ();
      if (bTrackScrollBar)
        RecalcHorzScrollBar (TRUE);
    }
}

//BEGIN SW
void CCrystalTextView::ScrollToSubLine( int nNewTopSubLine, 
                  BOOL bNoSmoothScroll /*= FALSE*/, BOOL bTrackScrollBar /*= TRUE*/ )
{
  if (m_nTopSubLine != nNewTopSubLine)
    {
      if (bNoSmoothScroll || ! m_bSmoothScroll)
        {
          int nScrollLines = m_nTopSubLine - nNewTopSubLine;
          m_nTopSubLine = nNewTopSubLine;
          // OnDraw() uses m_nTopLine to determine topline
          m_nTopLine = m_nTopSubLine;
          ScrollWindow(0, nScrollLines * GetLineHeight());
          UpdateWindow();
          if (bTrackScrollBar)
          RecalcVertScrollBar(TRUE);
        }
      else
        {
          //	Do smooth scrolling
          int nLineHeight = GetLineHeight();
          if (m_nTopSubLine > nNewTopSubLine)
            {
              int nIncrement = (m_nTopSubLine - nNewTopSubLine) / SMOOTH_SCROLL_FACTOR + 1;
              while (m_nTopSubLine != nNewTopSubLine)
                {
                  int nTopSubLine = m_nTopSubLine - nIncrement;
                  if (nTopSubLine < nNewTopSubLine)
                    nTopSubLine = nNewTopSubLine;
                  int nScrollLines = nTopSubLine - m_nTopSubLine;
                  m_nTopSubLine = nTopSubLine;
                  ScrollWindow(0, - nLineHeight * nScrollLines);
                  UpdateWindow();
                  if (bTrackScrollBar)
                    RecalcVertScrollBar(TRUE);
                }
            }
          else
            {
              int nIncrement = (nNewTopSubLine - m_nTopSubLine) / SMOOTH_SCROLL_FACTOR + 1;
              while (m_nTopSubLine != nNewTopSubLine)
                {
                  int nTopSubLine = m_nTopSubLine + nIncrement;
                  if (nTopSubLine > nNewTopSubLine)
                  nTopSubLine = nNewTopSubLine;
                  int nScrollLines = nTopSubLine - m_nTopSubLine;
                  m_nTopSubLine = nTopSubLine;
                  ScrollWindow(0, - nLineHeight * nScrollLines);
                  UpdateWindow();
                  if (bTrackScrollBar)
                    RecalcVertScrollBar(TRUE);
                }
            }
        }
      int nDummy;
      GetLineBySubLine( m_nTopSubLine, m_nTopLine, nDummy );
      InvalidateRect( NULL );	// repaint whole window
    }
}

/*void CCrystalTextView::GoToLine( int nLine )
{
  SetCursorPos( CPoint( 0, (nLine > 0)? nLine - 1 : 0 ) );
  EnsureVisible( GetCursorPos() );
}*/
//END SW

void CCrystalTextView::
ScrollToLine (int nNewTopLine, BOOL bNoSmoothScroll /*= FALSE*/ , BOOL bTrackScrollBar /*= TRUE*/ )
{
  //BEGIN SW
  if( m_nTopLine != nNewTopLine )
    ScrollToSubLine( GetSubLineIndex( nNewTopLine ), bNoSmoothScroll, bTrackScrollBar );

  /*ORIGINAL
  if (m_nTopLine != nNewTopLine)
  {
    if (bNoSmoothScroll || ! m_bSmoothScroll)
    {
      int nScrollLines = m_nTopLine - nNewTopLine;
      m_nTopLine = nNewTopLine;
      ScrollWindow(0, nScrollLines * GetLineHeight());
      UpdateWindow();
      if (bTrackScrollBar)
        RecalcVertScrollBar(TRUE);
    }
    else
    {
      //	Do smooth scrolling
      int nLineHeight = GetLineHeight();
      if (m_nTopLine > nNewTopLine)
      {
        int nIncrement = (m_nTopLine - nNewTopLine) / SMOOTH_SCROLL_FACTOR + 1;
        while (m_nTopLine != nNewTopLine)
        {
          int nTopLine = m_nTopLine - nIncrement;
          if (nTopLine < nNewTopLine)
            nTopLine = nNewTopLine;
          int nScrollLines = nTopLine - m_nTopLine;
          m_nTopLine = nTopLine;
          ScrollWindow(0, - nLineHeight * nScrollLines);
          UpdateWindow();
          if (bTrackScrollBar)
            RecalcVertScrollBar(TRUE);
        }
      }
      else
      {
        int nIncrement = (nNewTopLine - m_nTopLine) / SMOOTH_SCROLL_FACTOR + 1;
        while (m_nTopLine != nNewTopLine)
        {
          int nTopLine = m_nTopLine + nIncrement;
          if (nTopLine > nNewTopLine)
            nTopLine = nNewTopLine;
          int nScrollLines = nTopLine - m_nTopLine;
          m_nTopLine = nTopLine;
          ScrollWindow(0, - nLineHeight * nScrollLines);
          UpdateWindow();
          if (bTrackScrollBar)
            RecalcVertScrollBar(TRUE);
        }
      }
    }
  }
  *///END SW
}

void CCrystalTextView::
ExpandChars (LPCTSTR pszChars, int nOffset, int nCount, CString & line)
{
  if (nCount <= 0)
    {
      line = _T("");
      return;
    }

  const int nTabSize = GetTabSize ();

  // Calculate offset from begin of line
  int nActualOffset = 0;
  for (int i = 0; i < nOffset; i++)
    {
      if (pszChars[i] == _T ('\t'))
        nActualOffset += (nTabSize - nActualOffset % nTabSize);
      else
        nActualOffset++;
    }

  pszChars += nOffset;
  int nLength = nCount;

  int nTabCount = 0;
  for (i = 0; i < nLength; i++)
    {
      if (pszChars[i] == _T('\t'))
        nTabCount++;
    }

  LPTSTR pszBuf = line.GetBuffer(nLength + nTabCount * (nTabSize - 1) + 1);
  int nCurPos = 0;

  if (nTabCount > 0 || m_bViewTabs)
    {
      for (i = 0; i < nLength; i++)
        {
          if (pszChars[i] == _T('\t'))
            {
              int nSpaces = nTabSize - (nActualOffset + nCurPos) % nTabSize;
              if (m_bViewTabs)
                {
                  pszBuf[nCurPos++] = TAB_CHARACTER;
                  nSpaces--;
                }
              while (nSpaces > 0)
                {
                  pszBuf[nCurPos++] = _T(' ');
                  nSpaces--;
                }
            }
          else
            {
              if (pszChars[i] == ' ' && m_bViewTabs)
                pszBuf[nCurPos] = SPACE_CHARACTER;
              else if (pszChars[i] == '\r' && m_bViewEols)
                pszBuf[nCurPos] = CR_CHARACTER;
              else if (pszChars[i] == '\n' && m_bViewEols)
                pszBuf[nCurPos] = LF_CHARACTER;
              else
                pszBuf[nCurPos] = pszChars[i];

              nCurPos++;
            }
        }
    }
  else
    {
      CopyMemory(pszBuf, pszChars, sizeof(TCHAR) * nLength);
      nCurPos = nLength;
    }
  line.ReleaseBuffer(nCurPos);
}

void CCrystalTextView::
DrawLineHelperImpl (CDC * pdc, CPoint & ptOrigin, const CRect & rcClip,
                    LPCTSTR pszChars, int nOffset, int nCount)
{
  ASSERT (nCount >= 0);
  if (nCount > 0)
    {
      CString line;
      ExpandChars (pszChars, nOffset, nCount, line);
      const int lineLen = line.GetLength();
      int nWidth = rcClip.right - ptOrigin.x;
      const int nCharWidth = GetCharWidth();

      if (nWidth > 0)
        {
          int nCount = lineLen;
          int nCountFit = nWidth / nCharWidth + 1;
          if (nCount > nCountFit)
            nCount = nCountFit;
#ifdef _DEBUG
          //CSize sz = pdc->GetTextExtent(line, nCount);
          //ASSERT(sz.cx == m_nCharWidth * nCount);
#endif
          /*
             CRect rcBounds = rcClip;
             rcBounds.left = ptOrigin.x;
             rcBounds.right = rcBounds.left + GetCharWidth() * nCount;
             pdc->ExtTextOut(rcBounds.left, rcBounds.top, ETO_OPAQUE, &rcBounds, NULL, 0, NULL);
           */
          
          // Table of charwidths as CCrystalEditor thinks they are
          // Seems that CrystalEditor's and ExtTextOut()'s charwidths aren't
          // same with some fonts and text is drawn only partially
          // if this table is not used.
          int* pnWidths = new int[nCount];
          ASSERT(pnWidths);

          if (pnWidths)
            {
              for (int i = 0; i < nCount; i++)
                pnWidths[i] = nCharWidth;
            }

          // Because ExtTextOut() can handle 8192 chars at max.
          // we have to draw longer lines in 8192 char blocks
          if (nCount > 8192)
            {
              CPoint ptDraw = ptOrigin;
              LPCTSTR szText = line;
              DWORD dwDrawnChars = 0;
              DWORD dwCharsToDraw = 0;

              while (dwDrawnChars < nCount)
                {
                  if ((nCount - dwDrawnChars) > 8192)
                    dwCharsToDraw = 8192;
                  else
                    dwCharsToDraw = nCount - dwDrawnChars;

                  VERIFY(pdc->ExtTextOut(ptDraw.x, ptDraw.y, ETO_CLIPPED,
                      &rcClip, &szText[dwDrawnChars], dwCharsToDraw,
                      &pnWidths[dwDrawnChars]));

                  dwDrawnChars += dwCharsToDraw;
                  ptDraw.x += nCharWidth * dwCharsToDraw;
                }
            }
          else
            {
              VERIFY(pdc->ExtTextOut(ptOrigin.x, ptOrigin.y, ETO_CLIPPED,
                  &rcClip, line, nCount, pnWidths));
            }

          if (pnWidths)
            delete [] pnWidths;
        }
      ptOrigin.x += nCharWidth * lineLen;
    }
}

void CCrystalTextView::
DrawLineHelper (CDC * pdc, CPoint & ptOrigin, const CRect & rcClip, int nColorIndex,
                LPCTSTR pszChars, int nOffset, int nCount, CPoint ptTextPos)
{
  if (nCount > 0)
    {
      if (m_bFocused || m_bShowInactiveSelection)
        {
          int nSelBegin = 0, nSelEnd = 0;
          if (m_ptDrawSelStart.y > ptTextPos.y)
            {
              nSelBegin = nCount;
            }
          else if (m_ptDrawSelStart.y == ptTextPos.y)
            {
              nSelBegin = m_ptDrawSelStart.x - ptTextPos.x;
              if (nSelBegin < 0)
                nSelBegin = 0;
              if (nSelBegin > nCount)
                nSelBegin = nCount;
            }
          if (m_ptDrawSelEnd.y > ptTextPos.y)
            {
              nSelEnd = nCount;
            }
          else if (m_ptDrawSelEnd.y == ptTextPos.y)
            {
              nSelEnd = m_ptDrawSelEnd.x - ptTextPos.x;
              if (nSelEnd < 0)
                nSelEnd = 0;
              if (nSelEnd > nCount)
                nSelEnd = nCount;
            }

          ASSERT (nSelBegin >= 0 && nSelBegin <= nCount);
          ASSERT (nSelEnd >= 0 && nSelEnd <= nCount);
          ASSERT (nSelBegin <= nSelEnd);

          //  Draw part of the text before selection
          if (nSelBegin > 0)
            {
              DrawLineHelperImpl (pdc, ptOrigin, rcClip, pszChars, nOffset, nSelBegin);
            }
          if (nSelBegin < nSelEnd)
            {
              COLORREF crOldBk = pdc->SetBkColor (GetColor (COLORINDEX_SELBKGND));
              COLORREF crOldText = pdc->SetTextColor (GetColor (COLORINDEX_SELTEXT));
              DrawLineHelperImpl (pdc, ptOrigin, rcClip, pszChars, nOffset + nSelBegin, nSelEnd - nSelBegin);
              pdc->SetBkColor (crOldBk);
              pdc->SetTextColor (crOldText);
            }
          if (nSelEnd < nCount)
            {
              DrawLineHelperImpl (pdc, ptOrigin, rcClip, pszChars, nOffset + nSelEnd, nCount - nSelEnd);
            }
        }
      else
        {
          DrawLineHelperImpl (pdc, ptOrigin, rcClip, pszChars, nOffset, nCount);
        }
    }
}

void CCrystalTextView::
GetLineColors (int nLineIndex, COLORREF & crBkgnd,
               COLORREF & crText, BOOL & bDrawWhitespace)
{
  DWORD dwLineFlags = GetLineFlags (nLineIndex);
  bDrawWhitespace = TRUE;
  crText = RGB (255, 255, 255);
  if (dwLineFlags & LF_EXECUTION)
    {
      crBkgnd = RGB (0, 128, 0);
      return;
    }
  if (dwLineFlags & LF_BREAKPOINT)
    {
      crBkgnd = RGB (255, 0, 0);
      return;
    }
  if (dwLineFlags & LF_INVALID_BREAKPOINT)
    {
      crBkgnd = RGB (128, 128, 0);
      return;
    }
  if (dwLineFlags & LF_DIFF)
    {
      crBkgnd = RGB(255,255,92);
      return;
    }
  if (dwLineFlags & LF_GHOST)
    {
      crBkgnd = RGB(192,192,192);
      return;
    }
  crBkgnd = CLR_NONE;
  crText = CLR_NONE;
  bDrawWhitespace = FALSE;
}

DWORD CCrystalTextView::
GetParseCookie (int nLineIndex)
{
  int nLineCount = GetLineCount ();
  if (m_pdwParseCookies == NULL)
    {
      m_nParseArraySize = nLineCount;
      m_pdwParseCookies = new DWORD[nLineCount];
      memset (m_pdwParseCookies, 0xff, nLineCount * sizeof (DWORD));
    }

  if (nLineIndex < 0)
    return 0;
  if (m_pdwParseCookies[nLineIndex] != (DWORD) - 1)
    return m_pdwParseCookies[nLineIndex];

  int L = nLineIndex;
  while (L >= 0 && m_pdwParseCookies[L] == (DWORD) - 1)
    L--;
  L++;

  int nBlocks;
  while (L <= nLineIndex)
    {
      DWORD dwCookie = 0;
      if (L > 0)
        dwCookie = m_pdwParseCookies[L - 1];
      ASSERT (dwCookie != (DWORD) - 1);
      m_pdwParseCookies[L] = ParseLine (dwCookie, L, NULL, nBlocks);
      ASSERT (m_pdwParseCookies[L] != (DWORD) - 1);
      L++;
    }

  return m_pdwParseCookies[nLineIndex];
}

//BEGIN SW
void CCrystalTextView::WrapLine( int nLineIndex, int nMaxLineWidth, int *anBreaks, int &nBreaks )
{
  // There must be a parser attached to this view
  if( !m_pParser )
    return;

  m_pParser->WrapLine( nLineIndex, nMaxLineWidth, anBreaks, nBreaks );
}


void CCrystalTextView::WrapLineCached( 
                    int nLineIndex, int nMaxLineWidth, int *anBreaks, int &nBreaks )
{
  // if word wrap is not active, there is not any break in the line
  if( !m_bWordWrap )
  {
    nBreaks = 0;
    return;
  }

  // word wrap is active
  if( nLineIndex < m_panSubLines->GetSize() && !anBreaks && (*m_panSubLines)[nLineIndex] > -1 )
    // return cached data
    nBreaks = (*m_panSubLines)[nLineIndex] - 1;
  else
  {
    // recompute line wrap
    nBreaks = 0;
    WrapLine( nLineIndex, nMaxLineWidth, anBreaks, nBreaks );

    // cache data
    ASSERT( nBreaks > -1 );
    m_panSubLines->SetAtGrow( nLineIndex, nBreaks + 1 );

    // RecalcVertScrollBar();
  }
}


void CCrystalTextView::InvalidateLineCache( int nLineIndex1, int nLineIndex2 /*= -1*/ )
{
  // invalidate cached sub line count

  if( nLineIndex2 == -1 && nLineIndex1 < m_panSubLines->GetSize() )
    for( int i = nLineIndex1; i < m_panSubLines->GetSize(); i++ )
      (*m_panSubLines)[i] = -1;
  else
    {
      if( nLineIndex1 > nLineIndex2 )
        {
          int	nStorage = nLineIndex1;
          nLineIndex1 = nLineIndex2;
          nLineIndex2 = nStorage;
        }

    if( nLineIndex1 >= m_panSubLines->GetSize() )
      return;

    if( nLineIndex2 >= m_panSubLines->GetSize() )
      nLineIndex2 = m_panSubLines->GetUpperBound();

    for( int i = nLineIndex1; i <= nLineIndex2; i++ )
      if( i >= 0 && i < m_panSubLines->GetSize() )
        (*m_panSubLines)[i] = -1;
  }
}


void CCrystalTextView::DrawScreenLine( CDC *pdc, CPoint &ptOrigin, const CRect &rcClip,
         TEXTBLOCK *pBuf, int nBlocks, int &nActualItem, 
         COLORREF crText, COLORREF crBkgnd, BOOL bDrawWhitespace,
         LPCTSTR pszChars, int nOffset, int nCount, CPoint ptTextPos )
{
  CPoint	originalOrigin = ptOrigin;
  CRect		frect = rcClip;
  const int nLineLength = GetLineLength( ptTextPos.y );
  const int nLineHeight = GetLineHeight();

  frect.top = ptOrigin.y;
  frect.bottom = frect.top + nLineHeight;

  ASSERT( nActualItem < nBlocks );

  if (crText == CLR_NONE)
    pdc->SetTextColor(GetColor(pBuf[nActualItem].m_nColorIndex));
  pdc->SelectObject(
    GetFont(GetItalic(pBuf[nActualItem].m_nColorIndex), 
    GetBold(pBuf[nActualItem].m_nColorIndex)));

  if( nBlocks > 0 && nActualItem < nBlocks - 1 && 
    pBuf[nActualItem + 1].m_nCharPos >= nOffset && 
    pBuf[nActualItem + 1].m_nCharPos <= nOffset + nCount )
    {
      ASSERT(pBuf[nActualItem].m_nCharPos >= 0 &&
         pBuf[nActualItem].m_nCharPos <= nLineLength);

      /*
      pdc->SelectObject(GetFont(GetItalic(COLORINDEX_NORMALTEXT), GetBold(COLORINDEX_NORMALTEXT)));
      DrawLineHelper(
        pdc, ptOrigin, rcClip, pBuf[nActualItem].m_nColorIndex, pszChars, 
        nOffset, pBuf[nActualItem].m_nCharPos, ptTextPos);
      */
      for (int I = nActualItem; I < nBlocks - 1 &&
        pBuf[I + 1].m_nCharPos <= nOffset + nCount; I ++)
        {
          ASSERT(pBuf[I].m_nCharPos >= 0 && pBuf[I].m_nCharPos <= nLineLength);
          if (crText == CLR_NONE)
            pdc->SetTextColor(GetColor(pBuf[I].m_nColorIndex));

          // GetFont() returns font in internal font table
          pdc->SelectObject(GetFont(GetItalic(pBuf[I].m_nColorIndex),
            GetBold(pBuf[I].m_nColorIndex)));

          int nOffsetToUse = (nOffset > pBuf[I].m_nCharPos) ?
             nOffset : pBuf[I].m_nCharPos;
          DrawLineHelper(pdc, ptOrigin, rcClip, pBuf[I].m_nColorIndex, pszChars,
              (nOffset > pBuf[I].m_nCharPos)? nOffset : pBuf[I].m_nCharPos, 
              pBuf[I + 1].m_nCharPos - nOffsetToUse,
              CPoint( nOffsetToUse, ptTextPos.y ));
        }

      nActualItem = I;

      ASSERT(pBuf[nActualItem].m_nCharPos >= 0 &&
        pBuf[nActualItem].m_nCharPos <= nLineLength);
      if (crText == CLR_NONE)
        pdc->SetTextColor(GetColor(pBuf[nActualItem].m_nColorIndex));

      pdc->SelectObject(GetFont(GetItalic(pBuf[nActualItem].m_nColorIndex),
                GetBold(pBuf[nActualItem].m_nColorIndex)));
      DrawLineHelper(pdc, ptOrigin, rcClip, pBuf[nActualItem].m_nColorIndex,
              pszChars, pBuf[nActualItem].m_nCharPos,
              nOffset + nCount - pBuf[nActualItem].m_nCharPos,
              CPoint(pBuf[nActualItem].m_nCharPos, ptTextPos.y));
    }
  else
    {
      DrawLineHelper(
              pdc, ptOrigin, rcClip, pBuf[nActualItem].m_nColorIndex, 
              pszChars, nOffset, nCount, ptTextPos);
    }

  // Draw space on the right of the text

  frect.left = ptOrigin.x;

  if ((m_bFocused || m_bShowInactiveSelection) 
    && IsInsideSelBlock(CPoint(nLineLength, ptTextPos.y)) 
    && (nOffset + nCount) == nLineLength )
    {
      if (frect.left >= rcClip.left)
        {
          const int nCharWidth = GetCharWidth();
          pdc->FillSolidRect(frect.left, frect.top, nCharWidth, frect.Height(),
              GetColor(COLORINDEX_SELBKGND));
          frect.left += nCharWidth;
        }
    }
  if (frect.left < rcClip.left)
    frect.left = rcClip.left;

  if (frect.right > frect.left)
    pdc->FillSolidRect(frect, bDrawWhitespace ?
      crBkgnd : GetColor(COLORINDEX_WHITESPACE));

  // set origin to beginning of next screen line
  ptOrigin.x = originalOrigin.x;
  ptOrigin.y+= nLineHeight;
}
//END SW

class IntArray : public CArray<int, int>
{
public:
  explicit IntArray(int len) { SetSize(len); }
};

void CCrystalTextView::
DrawSingleLine (CDC * pdc, const CRect & rc, int nLineIndex)
{
  const int nCharWidth = GetCharWidth();
  ASSERT (nLineIndex >= -1 && nLineIndex < GetLineCount ());

  if (nLineIndex == -1)
    {
      //  Draw line beyond the text
      pdc->FillSolidRect (rc, GetColor (COLORINDEX_WHITESPACE));
      return;
    }

  //  Acquire the background color for the current line
  BOOL bDrawWhitespace = FALSE;
  COLORREF crBkgnd, crText;
  GetLineColors (nLineIndex, crBkgnd, crText, bDrawWhitespace);
  if (crBkgnd == CLR_NONE)
    crBkgnd = GetColor (COLORINDEX_BKGND);

  int nLength = GetLineLength (nLineIndex);
  LPCTSTR pszChars = GetLineChars (nLineIndex);
  if (m_bViewEols)
    { // Display EOL (end of line) characters
      if (pszChars[nLength]=='\r' && pszChars[nLength+1]=='\n')
        nLength += 2;
      else if (pszChars[nLength]=='\r' || pszChars[nLength]=='\n')
        nLength += 1;
    }
  //  Parse the line
  DWORD dwCookie = GetParseCookie (nLineIndex - 1);
  TEXTBLOCK *pBuf = new TEXTBLOCK[(nLength+1) * 3]; // be aware of nLength == 0
  int nBlocks = 0;
  //BEGIN SW
  // insert at least one textblock of normal color at the beginning
  pBuf[0].m_nCharPos = 0;
  pBuf[0].m_nColorIndex = COLORINDEX_NORMALTEXT;
  nBlocks++;
  //END SW
  m_pdwParseCookies[nLineIndex] = ParseLine (dwCookie, nLineIndex, pBuf, nBlocks);
  ASSERT (m_pdwParseCookies[nLineIndex] != (DWORD) - 1);

  //BEGIN SW
  int nActualItem = 0;

  // Wrap the line
  IntArray anBreaks(nLength);
  int	nBreaks = 0;

  WrapLineCached( nLineIndex, GetScreenChars(), anBreaks.GetData(), nBreaks );
  //END SW

  //  Draw the line text
  CPoint origin (rc.left - m_nOffsetChar * nCharWidth, rc.top);
  pdc->SetBkColor (crBkgnd);
  if (crText != CLR_NONE)
    pdc->SetTextColor (crText);
// BOOL bColorSet = FALSE;

  //BEGIN SW
  /*ORIGINAL
  if (nBlocks > 0)
  {
    ASSERT(pBuf[0].m_nCharPos >= 0 && pBuf[0].m_nCharPos <= nLength);
    if (crText == CLR_NONE)
      pdc->SetTextColor(GetColor(COLORINDEX_NORMALTEXT));
    pdc->SelectObject(GetFont(GetItalic(COLORINDEX_NORMALTEXT), GetBold(COLORINDEX_NORMALTEXT)));
    DrawLineHelper(pdc, origin, rc, COLORINDEX_NORMALTEXT, pszChars, 0, pBuf[0].m_nCharPos, CPoint(0, nLineIndex));
    for (int I = 0; I < nBlocks - 1; I ++)
    {
      ASSERT(pBuf[I].m_nCharPos >= 0 && pBuf[I].m_nCharPos <= nLength);
      if (crText == CLR_NONE)
        pdc->SetTextColor(GetColor(pBuf[I].m_nColorIndex));
      pdc->SelectObject(GetFont(GetItalic(pBuf[I].m_nColorIndex), GetBold(pBuf[I].m_nColorIndex)));
      DrawLineHelper(pdc, origin, rc, pBuf[I].m_nColorIndex, pszChars,
              pBuf[I].m_nCharPos, pBuf[I + 1].m_nCharPos - pBuf[I].m_nCharPos,
              CPoint(pBuf[I].m_nCharPos, nLineIndex));
    }
    ASSERT(pBuf[nBlocks - 1].m_nCharPos >= 0 && pBuf[nBlocks - 1].m_nCharPos <= nLength);
    if (crText == CLR_NONE)
      pdc->SetTextColor(GetColor(pBuf[nBlocks - 1].m_nColorIndex));
    pdc->SelectObject(GetFont(GetItalic(pBuf[nBlocks - 1].m_nColorIndex),
              GetBold(pBuf[nBlocks - 1].m_nColorIndex)));
    DrawLineHelper(pdc, origin, rc, pBuf[nBlocks - 1].m_nColorIndex, pszChars,
              pBuf[nBlocks - 1].m_nCharPos, nLength - pBuf[nBlocks - 1].m_nCharPos,
              CPoint(pBuf[nBlocks - 1].m_nCharPos, nLineIndex));
  }
  else
  */
  //END SW

  // BEGIN SW
  if( nBreaks > 0 )
    {
      // Draw all the screen lines of the wrapped line
      ASSERT( anBreaks[0] < nLength );
    
      // draw start of line to first break
      DrawScreenLine(
        pdc, origin, rc,
        pBuf, nBlocks, nActualItem,
        crText, crBkgnd, bDrawWhitespace,
        pszChars, 0, anBreaks[0], CPoint( 0, nLineIndex ) );
    
      // draw from first break to last break
      for( int i = 0; i < nBreaks - 1; i++ )
        {
          ASSERT( anBreaks[i] >= 0 && anBreaks[i] < nLength );
          DrawScreenLine(
            pdc, origin, rc,
            pBuf, nBlocks, nActualItem,
            crText, crBkgnd, bDrawWhitespace,
            pszChars, anBreaks[i], anBreaks[i + 1] - anBreaks[i],
            CPoint( anBreaks[i], nLineIndex ) );
        }
    
      // draw from last break till end of line
      DrawScreenLine(
        pdc, origin, rc,
        pBuf, nBlocks, nActualItem,
        crText, crBkgnd, bDrawWhitespace,
        pszChars, anBreaks[i], nLength - anBreaks[i],
        CPoint( anBreaks[i], nLineIndex ) );
    }
  else
      DrawScreenLine(
        pdc, origin, rc,
        pBuf, nBlocks, nActualItem,
        crText, crBkgnd, bDrawWhitespace,
        pszChars, 0, nLength, CPoint(0, nLineIndex));

  //	Draw whitespaces to the left of the text
  //BEGIN SW
  // Drawing is now realized by DrawScreenLine()
  /*ORIGINAL
  CRect frect = rc;
  if (origin.x > frect.left)
    frect.left = origin.x;
  if (frect.right > frect.left)
  {
    if ((m_bFocused || m_bShowInactiveSelection) && IsInsideSelBlock(CPoint(nLength, nLineIndex)))
    {
      pdc->FillSolidRect(frect.left, frect.top, GetCharWidth(), frect.Height(),
                        GetColor(COLORINDEX_SELBKGND));
      frect.left += GetCharWidth();
    }
    if (frect.right > frect.left)
      pdc->FillSolidRect(frect, bDrawWhitespace ? crBkgnd : GetColor(COLORINDEX_WHITESPACE));
  }
  */
  //END SW
  delete[] pBuf;
}

COLORREF CCrystalTextView::
GetColor (int nColorIndex)
{
  switch (nColorIndex)
    {
    case COLORINDEX_WHITESPACE :
    case COLORINDEX_BKGND:
      return::GetSysColor (COLOR_WINDOW);
    case COLORINDEX_NORMALTEXT:
      return::GetSysColor (COLOR_WINDOWTEXT);
    case COLORINDEX_SELMARGIN:
      return::GetSysColor (COLOR_SCROLLBAR);
    case COLORINDEX_PREPROCESSOR:
      return RGB (0, 128, 192);
    case COLORINDEX_COMMENT:
      //      return RGB(128, 128, 128);
      return RGB (0, 128, 0);
      //  [JRT]: Enabled Support For Numbers...
    case COLORINDEX_NUMBER:
      //      return RGB(0x80, 0x00, 0x00);
      return RGB (0xff, 0x00, 0x00);
      //  [JRT]: Support For C/C++ Operators
    case COLORINDEX_OPERATOR:
      //      return RGB(0x00, 0x00, 0x00);
      return RGB (96, 96, 96);
    case COLORINDEX_KEYWORD:
      return RGB (0, 0, 255);
    case COLORINDEX_FUNCNAME:
      return RGB (128, 0, 128);
    case COLORINDEX_USER1:
      return RGB (0, 0, 128);
    case COLORINDEX_USER2:
      return RGB (0, 128, 192);
    case COLORINDEX_SELBKGND:
      return RGB (0, 0, 0);
    case COLORINDEX_SELTEXT:
      return RGB (255, 255, 255);
    }
  //  return RGB(255, 0, 0);
  return RGB (128, 0, 0);
}

DWORD CCrystalTextView::
GetLineFlags (int nLineIndex)
{
  if (m_pTextBuffer == NULL)
    return 0;
  return m_pTextBuffer->GetLineFlags (nLineIndex);
}

void CCrystalTextView::
DrawMargin (CDC * pdc, const CRect & rect, int nLineIndex)
{
  if (!m_bSelMargin)
    {
      pdc->FillSolidRect (rect, GetColor (COLORINDEX_BKGND));
      return;
    }

  pdc->FillSolidRect (rect, GetColor (COLORINDEX_SELMARGIN));

  int nImageIndex = -1;
  if (nLineIndex >= 0)
    {
      DWORD dwLineFlags = GetLineFlags (nLineIndex);
      static const DWORD adwFlags[] =
        {
          LF_EXECUTION,
          LF_BREAKPOINT,
          LF_COMPILATION_ERROR,
          LF_BOOKMARK (1),
          LF_BOOKMARK (2),
          LF_BOOKMARK (3),
          LF_BOOKMARK (4),
          LF_BOOKMARK (5),
          LF_BOOKMARK (6),
          LF_BOOKMARK (7),
          LF_BOOKMARK (8),
          LF_BOOKMARK (9),
          LF_BOOKMARK (0),
          LF_BOOKMARKS,
          LF_INVALID_BREAKPOINT
        };
      for (int I = 0; I <= sizeof (adwFlags) / sizeof (adwFlags[0]); I++)
        {
          if ((dwLineFlags & adwFlags[I]) != 0)
            {
              nImageIndex = I;
              break;
            }
        }
    }

  if (nImageIndex >= 0)
    {
      if (m_pIcons == NULL)
        {
          m_pIcons = new CImageList;
          VERIFY (m_pIcons->Create (IDR_MARGIN_ICONS, 16, 16, RGB (255, 255, 255)));
        }
    //BEGIN SW
    CPoint pt(rect.left + 2, rect.top + (GetLineHeight() - 16) / 2);
    /*ORIGINAL
    CPoint pt(rect.left + 2, rect.top + (rect.Height() - 16) / 2);
    *///END SW
    VERIFY(m_pIcons->Draw(pdc, nImageIndex, pt, ILD_TRANSPARENT));
      VERIFY (m_pIcons->Draw (pdc, nImageIndex, pt, ILD_TRANSPARENT));
    }
}

BOOL CCrystalTextView::
IsInsideSelBlock (CPoint ptTextPos)
{
  ASSERT_VALIDTEXTPOS (ptTextPos);
  if (ptTextPos.y < m_ptDrawSelStart.y)
    return FALSE;
  if (ptTextPos.y > m_ptDrawSelEnd.y)
    return FALSE;
  if (ptTextPos.y < m_ptDrawSelEnd.y && ptTextPos.y > m_ptDrawSelStart.y)
    return TRUE;
  if (m_ptDrawSelStart.y < m_ptDrawSelEnd.y)
    {
      if (ptTextPos.y == m_ptDrawSelEnd.y)
        return ptTextPos.x < m_ptDrawSelEnd.x;
      ASSERT (ptTextPos.y == m_ptDrawSelStart.y);
      return ptTextPos.x >= m_ptDrawSelStart.x;
    }
  ASSERT (m_ptDrawSelStart.y == m_ptDrawSelEnd.y);
  return ptTextPos.x >= m_ptDrawSelStart.x && ptTextPos.x < m_ptDrawSelEnd.x;
}

BOOL CCrystalTextView::
IsInsideSelection (const CPoint & ptTextPos)
{
  PrepareSelBounds ();
  return IsInsideSelBlock (ptTextPos);
}

void CCrystalTextView::
PrepareSelBounds ()
{
  if (m_ptSelStart.y < m_ptSelEnd.y ||
        (m_ptSelStart.y == m_ptSelEnd.y && m_ptSelStart.x < m_ptSelEnd.x))
    {
      m_ptDrawSelStart = m_ptSelStart;
      m_ptDrawSelEnd = m_ptSelEnd;
    }
  else
    {
      m_ptDrawSelStart = m_ptSelEnd;
      m_ptDrawSelEnd = m_ptSelStart;
    }
}

void CCrystalTextView::
OnDraw (CDC * pdc)
{
  CRect rcClient;
  GetClientRect (rcClient);

  const int nLineCount = GetLineCount ();
  const int nLineHeight = GetLineHeight ();
  PrepareSelBounds ();

  CDC cacheDC;
  VERIFY (cacheDC.CreateCompatibleDC (pdc));
  if (m_pCacheBitmap == NULL)
    {
      m_pCacheBitmap = new CBitmap;
      VERIFY(m_pCacheBitmap->CreateCompatibleBitmap(pdc, rcClient.Width(), nLineHeight));
    }
  CBitmap *pOldBitmap = cacheDC.SelectObject (m_pCacheBitmap);

  CRect rcLine;
  rcLine = rcClient;
  rcLine.bottom = rcLine.top + nLineHeight;
  CRect rcCacheMargin (0, 0, GetMarginWidth (), nLineHeight);
  CRect rcCacheLine (GetMarginWidth (), 0, rcLine.Width (), nLineHeight);
  //BEGIN SW
  // initialize rects
  int		nSubLineOffset = GetSubLineIndex( m_nTopLine ) - m_nTopSubLine;
  if( nSubLineOffset < 0 )
  {
    rcCacheMargin.OffsetRect( 0, nSubLineOffset * nLineHeight );
    rcCacheLine.OffsetRect( 0, nSubLineOffset * nLineHeight );
  }

  int		nBreaks;
  const int nMaxLineChars = GetScreenChars();
  //END SW

  int nCurrentLine = m_nTopLine;
  while (rcLine.top < rcClient.bottom)
    {
      //BEGIN SW
      nBreaks = 0;
      if( nCurrentLine < nLineCount /*&& GetLineLength( nCurrentLine ) > nMaxLineChars*/ )
        WrapLineCached( nCurrentLine, nMaxLineChars, NULL, nBreaks );

      rcLine.bottom = rcLine.top + (nBreaks + 1) * nLineHeight;
      rcCacheLine.bottom = rcCacheLine.top + rcLine.Height();
      rcCacheMargin.bottom = rcCacheMargin.top + rcLine.Height();

      if( rcCacheLine.top < 0 )
        rcLine.bottom+= rcCacheLine.top;
      //END SW
      if (nCurrentLine < nLineCount)
        {
          DrawMargin (&cacheDC, rcCacheMargin, nCurrentLine);
          DrawSingleLine (&cacheDC, rcCacheLine, nCurrentLine);
        }
      else
        {
          DrawMargin (&cacheDC, rcCacheMargin, -1);
          DrawSingleLine (&cacheDC, rcCacheLine, -1);
        }

      VERIFY (pdc->BitBlt (rcLine.left, rcLine.top, rcLine.Width (), rcLine.Height (), &cacheDC, 0, 0, SRCCOPY));

      nCurrentLine++;
      //BEGIN SW
      rcLine.top = rcLine.bottom;
      rcCacheLine.top = 0;
      rcCacheMargin.top = 0;
      /*ORIGINAL
      rcLine.OffsetRect(0, nLineHeight);
      */
      //END SW
    }

  cacheDC.SelectObject (pOldBitmap);
  cacheDC.DeleteDC ();
}

void CCrystalTextView::
ResetView ()
{
  // m_bWordWrap = FALSE;
  m_nTopLine = 0;
  m_nOffsetChar = 0;
  m_nLineHeight = -1;
  m_nCharWidth = -1;
  m_nMaxLineLength = -1;
  m_nScreenLines = -1;
  m_nScreenChars = -1;
  m_nIdealCharPos = -1;
  m_ptAnchor.x = 0;
  m_ptAnchor.y = 0;
  if (m_pIcons != NULL)
    {
      delete m_pIcons;
      m_pIcons = NULL;
    }
  for (int I = 0; I < 4; I++)
    {
      if (m_apFonts[I] != NULL)
        {
          m_apFonts[I]->DeleteObject ();
          delete m_apFonts[I];
          m_apFonts[I] = NULL;
        }
    }
  if (m_pdwParseCookies != NULL)
    {
      delete[] m_pdwParseCookies;
      m_pdwParseCookies = NULL;
    }
  if (m_pnActualLineLength != NULL)
    {
      delete[] m_pnActualLineLength;
      m_pnActualLineLength = NULL;
    }
  m_nParseArraySize = 0;
  m_nActualLengthArraySize = 0;
  m_ptCursorPos.x = 0;
  m_ptCursorPos.y = 0;
  m_ptSelStart = m_ptSelEnd = m_ptCursorPos;
  if (m_bDragSelection)
    {
      ReleaseCapture ();
      KillTimer (m_nDragSelTimer);
    }
  m_bDragSelection = FALSE;
  m_bVertScrollBarLocked = FALSE;
  m_bHorzScrollBarLocked = FALSE;
  if (::IsWindow (m_hWnd))
    UpdateCaret ();
  m_bLastSearch = FALSE;
  m_bShowInactiveSelection = TRUE; // FP: reverted because I like it
  m_bPrintHeader = FALSE;
  m_bPrintFooter = TRUE;

  m_bBookmarkExist = FALSE;     // More bookmarks

  m_bMultipleSearch = FALSE;    // More search

}

void CCrystalTextView::
UpdateCaret ()
{
  ASSERT_VALIDTEXTPOS (m_ptCursorPos);
  if (m_bFocused && !m_bCursorHidden &&
        CalculateActualOffset (m_ptCursorPos.y, m_ptCursorPos.x) >= m_nOffsetChar)
    {
      if (m_bOverrideCaret)  //UPDATE
        CreateSolidCaret(GetCharWidth(), GetLineHeight());
      else
        CreateSolidCaret (2, GetLineHeight ());
      
      SetCaretPos (TextToClient (m_ptCursorPos));
      ShowCaret ();
    }
  else
    {
      HideCaret ();
    }
  OnUpdateCaret();
}

void CCrystalTextView::
OnUpdateCaret()
{
}

int CCrystalTextView::
GetCRLFMode ()
{
  if (m_pTextBuffer)
    {
      return m_pTextBuffer->GetCRLFMode ();
    }
  return -1;
}

void CCrystalTextView::
SetCRLFMode (int nCRLFMode)
{
  if (m_pTextBuffer)
    {
      m_pTextBuffer->SetCRLFMode (nCRLFMode);
    }
}

int CCrystalTextView::
GetTabSize ()
{
  if (m_pTextBuffer == NULL)
    return 4;

  return m_pTextBuffer->GetTabSize();

}

void CCrystalTextView::
SetTabSize (int nTabSize)
{
  ASSERT (nTabSize >= 0 && nTabSize <= 64);
  if (m_pTextBuffer == NULL)
    return;

  if (m_pTextBuffer->GetTabSize() != nTabSize)
    {
      m_pTextBuffer->SetTabSize( nTabSize );

      if (m_pnActualLineLength != NULL)
        {
          delete[] m_pnActualLineLength;
          m_pnActualLineLength = NULL;
        }
      m_nActualLengthArraySize = 0;
      m_nMaxLineLength = -1;
      RecalcHorzScrollBar ();
      Invalidate ();
      UpdateCaret ();
    }
}

CFont *CCrystalTextView::
GetFont (BOOL bItalic /*= FALSE*/ , BOOL bBold /*= FALSE*/ )
{
  int nIndex = 0;
  if (bBold)
    nIndex |= 1;
  if (bItalic)
    nIndex |= 2;

  if (m_apFonts[nIndex] == NULL)
    {
      m_apFonts[nIndex] = new CFont;
      if (!m_lfBaseFont.lfHeight)
        {
          CClientDC dc (GetDesktopWindow ());
          m_lfBaseFont.lfHeight = -MulDiv (11, dc.GetDeviceCaps (LOGPIXELSY), 72);
        }
      m_lfBaseFont.lfWeight = bBold && !_tcsnicmp (m_lfBaseFont.lfFaceName, _T ("Courier"), 7) ? FW_BOLD : FW_NORMAL;
      m_lfBaseFont.lfItalic = (BYTE) (bItalic && !_tcsnicmp (m_lfBaseFont.lfFaceName, _T ("Courier"), 7));
      if (!m_apFonts[nIndex]->CreateFontIndirect (&m_lfBaseFont))
        {
          delete m_apFonts[nIndex];
          m_apFonts[nIndex] = NULL;
          return CView::GetFont ();
        }

    }
  return m_apFonts[nIndex];
}

void CCrystalTextView::
CalcLineCharDim ()
{
  CDC *pdc = GetDC ();
  CFont *pOldFont = pdc->SelectObject (GetFont ());
  CSize szCharExt = pdc->GetTextExtent (_T ("X"));
  m_nLineHeight = szCharExt.cy;
  if (m_nLineHeight < 1)
    m_nLineHeight = 1;
  m_nCharWidth = szCharExt.cx;
  /*
     TEXTMETRIC tm;
     if (pdc->GetTextMetrics(&tm))
     m_nCharWidth -= tm.tmOverhang;
   */
  pdc->SelectObject (pOldFont);
  ReleaseDC (pdc);
}

int CCrystalTextView::
GetLineHeight ()
{
  if (m_nLineHeight == -1)
    CalcLineCharDim ();
  return m_nLineHeight;
}

//BEGIN SW
int CCrystalTextView::GetSubLines( int nLineIndex )
{
  /*
  if( GetLineLength( nLineIndex ) <= GetScreenChars() )
    return 1;
  */
  // get number of wrapped lines, this line contains of
  int	nBreaks = 0;
  WrapLineCached( nLineIndex, GetScreenChars(), NULL, nBreaks );

  return nBreaks + 1;
}

int CCrystalTextView::CharPosToPoint( int nLineIndex, int nCharPos, CPoint &charPoint )
{
  // if we do not wrap lines, y is allways 0 and x is equl to nCharPos
  if( !m_bWordWrap )
    {
      charPoint.x = nCharPos;
      charPoint.y = 0;
    }

  // calculate point out of char pos
  /*
  if( GetLineLength( nLineIndex ) <= GetScreenChars() )
  {
    // line is not wrapped
    charPoint.x = nCharPos;
    charPoint.y = 0;
    return 0;
  }
  */

  // line is wrapped
  int *anBreaks = new int[GetLineLength( nLineIndex )];
  int	nBreaks = 0;

  WrapLineCached( nLineIndex, GetScreenChars(), anBreaks, nBreaks );

  for( int i = nBreaks - 1; i >= 0 && nCharPos < anBreaks[i]; i-- );

  charPoint.x = (i >= 0)? nCharPos - anBreaks[i] : nCharPos;
  charPoint.y = i + 1;

  int nReturnVal = (i >= 0)? anBreaks[i] : 0;
  delete[] anBreaks;

  return nReturnVal;
}

int CCrystalTextView::CursorPointToCharPos( int nLineIndex, const CPoint &curPoint )
{
  // calculate char pos out of point
  const int nLength = GetLineLength( nLineIndex );
  const int nScreenChars = GetScreenChars();
  LPCTSTR	szLine = GetLineChars( nLineIndex );

  // wrap line
  int *anBreaks = new int[nLength];
  int	nBreaks = 0;

  WrapLineCached( nLineIndex, nScreenChars, anBreaks, nBreaks );

  // find char pos that matches cursor position
  int nXPos = 0;
  int nYPos = 0;
  int	nCurPos = 0;
  const int nTabSize = GetTabSize();

  for( int nIndex = 0; nIndex < nLength; nIndex++ )
    {
      if( nBreaks && nIndex == anBreaks[nYPos] )
        {
          nXPos = 0;
          nYPos++;
        }

      if (szLine[nIndex] == _T('\t'))
        {
          const int nOffset = nTabSize - nCurPos % nTabSize;
          nXPos += nOffset;
          nCurPos += nOffset;
        }
      else
        {
          nXPos++;
          nCurPos++;
        }

      if( nXPos > curPoint.x && nYPos == curPoint.y )
        break;
      else if( nYPos > curPoint.y )
        {
          nIndex--;
          break;
        }
    }
  delete[] anBreaks;

  return nIndex;	
}

void CCrystalTextView::SubLineCursorPosToTextPos( const CPoint &subLineCurPos, CPoint &textPos )
{
  // Get line breaks
  int	nSubLineOffset, nLine;

  GetLineBySubLine( subLineCurPos.y, nLine, nSubLineOffset );

  // compute cursor-position
  textPos.x = CursorPointToCharPos( nLine, CPoint( subLineCurPos.x, nSubLineOffset ) );
  textPos.y = nLine;
}

int CCrystalTextView::SubLineEndToCharPos( int nLineIndex, int nSubLineOffset )
{
  const int nLength = GetLineLength( nLineIndex );

  // if word wrapping is disabled, the end is equal to the length of the line -1
  if( !m_bWordWrap /*|| nLength <= GetScreenChars()*/ )
    return nLength;

  // wrap line
  int *anBreaks = new int[nLength];
  int	nBreaks = 0;

  WrapLineCached( nLineIndex, GetScreenChars(), anBreaks, nBreaks );

  // if there is no break inside the line or the given subline is the last
  // one in this line...
  if( !nBreaks || nSubLineOffset == nBreaks )
    return nLength;

  // compute character position for end of subline
  ASSERT( nSubLineOffset >= 0 && nSubLineOffset <= nBreaks );
  
  int nReturnVal = anBreaks[nSubLineOffset] - 1;
  delete[] anBreaks;

  return nReturnVal;
}

int CCrystalTextView::SubLineHomeToCharPos( int nLineIndex, int nSubLineOffset )
{
  int		nLength = GetLineLength( nLineIndex );

  // if word wrapping is disabled, the start is 0
  if( !m_bWordWrap /*|| nLength <= GetScreenChars() */|| nSubLineOffset == 0 )
    return 0;

  // wrap line
  int *anBreaks = new int[nLength];
  int	nBreaks = 0;

  WrapLineCached( nLineIndex, GetScreenChars(), anBreaks, nBreaks );

  // if there is no break inside the line...
  if( !nBreaks )
    return 0;

  // compute character position for end of subline
  ASSERT( nSubLineOffset > 0 && nSubLineOffset <= nBreaks );
  
  int nReturnVal = anBreaks[nSubLineOffset - 1];
  delete[] anBreaks;

  return nReturnVal;
}
//END SW

int CCrystalTextView::
GetCharWidth ()
{
  if (m_nCharWidth == -1)
    CalcLineCharDim ();
  return m_nCharWidth;
}

int CCrystalTextView::
GetMaxLineLength ()
{
  if (m_nMaxLineLength == -1)
    {
      m_nMaxLineLength = 0;
      const int nLineCount = GetLineCount ();
      for (int I = 0; I < nLineCount; I++)
        {
          int nActualLength = GetLineActualLength (I);
          if (m_nMaxLineLength < nActualLength)
            m_nMaxLineLength = nActualLength;
        }
    }
  return m_nMaxLineLength;
}

CCrystalTextView *CCrystalTextView::
GetSiblingView (int nRow, int nCol)
{
  CSplitterWnd *pSplitter = GetParentSplitter (this, FALSE);
  if (pSplitter == NULL)
    return NULL;
  CWnd *pWnd = CWnd::FromHandlePermanent (
                 ::GetDlgItem (pSplitter->m_hWnd, pSplitter->IdFromRowCol (nRow, nCol)));
  if (pWnd == NULL || !pWnd->IsKindOf (RUNTIME_CLASS (CCrystalTextView)))
    return NULL;
  return (CCrystalTextView *) pWnd;
}

void CCrystalTextView::
GoToLine (int nLine, bool bRelative)
{
  int nLines = m_pTextBuffer->GetLineCount () - 1;
  CPoint ptCursorPos = GetCursorPos ();
  if (bRelative)
    {
      nLine += ptCursorPos.y;
    }
  if (nLine)
    {
      nLine--;
    }
  if (nLine > nLines)
    {
      nLine = nLines;
    }
  if (nLine >= 0)
    {
      int nChars = m_pTextBuffer->GetLineLength (nLine);
      if (nChars)
        {
          nChars--;
        }
      if (ptCursorPos.x > nChars)
        {
          ptCursorPos.x = nChars;
        }
      if (ptCursorPos.x >= 0)
        {
          ptCursorPos.y = nLine;
          ASSERT_VALIDTEXTPOS (ptCursorPos);
          SetAnchor (ptCursorPos);
          SetSelection (ptCursorPos, ptCursorPos);
          SetCursorPos (ptCursorPos);
          EnsureVisible (ptCursorPos);
        }
    }
}

void CCrystalTextView::
OnInitialUpdate ()
{
  CView::OnInitialUpdate ();
  CString sDoc = GetDocument ()->GetPathName (), sExt = GetExt (sDoc);
  SetTextType (sExt);
  AttachToBuffer (NULL);

  CSplitterWnd *pSplitter = GetParentSplitter (this, FALSE);
  if (pSplitter != NULL)
    {
      //  See CSplitterWnd::IdFromRowCol() implementation
      int nRow = (GetDlgCtrlID () - AFX_IDW_PANE_FIRST) / 16;
      int nCol = (GetDlgCtrlID () - AFX_IDW_PANE_FIRST) % 16;
      ASSERT (nRow >= 0 && nRow < pSplitter->GetRowCount ());
      ASSERT (nCol >= 0 && nCol < pSplitter->GetColumnCount ());

      if (nRow > 0)
        {
          CCrystalTextView *pSiblingView = GetSiblingView (0, nCol);
          if (pSiblingView != NULL && pSiblingView != this)
            {
              m_nOffsetChar = pSiblingView->m_nOffsetChar;
              ASSERT (m_nOffsetChar >= 0 && m_nOffsetChar <= GetMaxLineLength ());
            }
        }

      if (nCol > 0)
        {
          CCrystalTextView *pSiblingView = GetSiblingView (nRow, 0);
          if (pSiblingView != NULL && pSiblingView != this)
            {
              m_nTopLine = pSiblingView->m_nTopLine;
              ASSERT (m_nTopLine >= 0 && m_nTopLine < GetLineCount ());
            }
        }
    }
  SetTextType (sExt);
  SetFont (m_LogFont);
  if (m_bRememberLastPos && !sDoc.IsEmpty ())
    {
      DWORD dwLastPos[3];
      CString sKey = REG_EDITPAD;
      sKey += _T ("\\Remembered");
      CReg reg;
      if (reg.Open (HKEY_CURRENT_USER, sKey, KEY_READ) &&
        reg.LoadBinary (sDoc, (LPBYTE) dwLastPos, sizeof (dwLastPos)))
        {
          CPoint ptCursorPos;
          ptCursorPos.x = dwLastPos[1];
          ptCursorPos.y = dwLastPos[2];
          if (IsValidTextPosY (ptCursorPos))
            {
              if (!IsValidTextPosX (ptCursorPos))
                ptCursorPos.x = 0;
              ASSERT_VALIDTEXTPOS (ptCursorPos);
              SetCursorPos (ptCursorPos);
              SetSelection (ptCursorPos, ptCursorPos);
              SetAnchor (ptCursorPos);
              EnsureVisible (ptCursorPos);
            }
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
// CCrystalTextView printing

void CCrystalTextView::
OnPrepareDC (CDC * pDC, CPrintInfo * pInfo)
{
  CView::OnPrepareDC (pDC, pInfo);

  if (pInfo != NULL)
    {
      pInfo->m_bContinuePrinting = TRUE;
      if (m_pnPages != NULL && (int) pInfo->m_nCurPage > m_nPrintPages)
        pInfo->m_bContinuePrinting = FALSE;
    }
}

BOOL CCrystalTextView::
OnPreparePrinting (CPrintInfo * pInfo)
{
  return DoPreparePrinting (pInfo);
}

int CCrystalTextView::
PrintLineHeight (CDC * pdc, int nLine)
{
  ASSERT (nLine >= 0 && nLine < GetLineCount ());
  ASSERT (m_nPrintLineHeight > 0);
  int nLength = GetLineLength (nLine);
  if (nLength == 0)
    return m_nPrintLineHeight;

  CString line;
  LPCTSTR pszChars = GetLineChars (nLine);
  ExpandChars (pszChars, 0, nLength, line);
  CRect rcPrintArea = m_rcPrintArea;
  pdc->DrawText (line, &rcPrintArea, DT_LEFT | DT_NOPREFIX | DT_TOP | DT_WORDBREAK | DT_CALCRECT);
  return rcPrintArea.Height ();
}

void CCrystalTextView::
GetPrintHeaderText (int nPageNum, CString & text)
{
  ASSERT (m_bPrintHeader);
  text = _T ("");
}

void CCrystalTextView::
GetPrintFooterText (int nPageNum, CString & text)
{
  ASSERT (m_bPrintFooter);
  text.Format (_T ("Page %d/%d"), nPageNum, m_nPrintPages);
}

void CCrystalTextView::
PrintHeader (CDC * pdc, int nPageNum)
{
  CRect rcHeader = m_rcPrintArea;
  rcHeader.bottom = rcHeader.top;
  rcHeader.top -= (m_nPrintLineHeight + m_nPrintLineHeight / 2);

  CString text;
  GetPrintHeaderText (nPageNum, text);
  if (!text.IsEmpty ())
    pdc->DrawText (text, &rcHeader, DT_CENTER | DT_NOPREFIX | DT_TOP | DT_SINGLELINE);
}

void CCrystalTextView::
PrintFooter (CDC * pdc, int nPageNum)
{
  CRect rcFooter = m_rcPrintArea;
  rcFooter.top = rcFooter.bottom;
  rcFooter.bottom += (m_nPrintLineHeight + m_nPrintLineHeight / 2);

  CString text;
  GetPrintFooterText (nPageNum, text);
  if (!text.IsEmpty ())
    pdc->DrawText (text, &rcFooter, DT_CENTER | DT_NOPREFIX | DT_BOTTOM | DT_SINGLELINE);
}

void CCrystalTextView::
RecalcPageLayouts (CDC * pdc, CPrintInfo * pInfo)
{
  m_ptPageArea = pInfo->m_rectDraw;
  m_ptPageArea.NormalizeRect ();

  m_nPrintLineHeight = pdc->GetTextExtent (_T ("X")).cy;

  m_rcPrintArea = m_ptPageArea;
  CSize szTopLeft, szBottomRight;
  CWinApp *pApp = AfxGetApp ();
  ASSERT (pApp != NULL);
  szTopLeft.cx = DEFAULT_PRINT_MARGIN;
  szBottomRight.cx = DEFAULT_PRINT_MARGIN;
  szTopLeft.cy = DEFAULT_PRINT_MARGIN;
  szBottomRight.cy = DEFAULT_PRINT_MARGIN;
  CReg reg;
  if (reg.Open (HKEY_CURRENT_USER, REG_EDITPAD, KEY_READ))
    {
      DWORD dwTemp;
      if (reg.LoadNumber (_T ("PageLeft"), &dwTemp))
        szTopLeft.cx = dwTemp;
      if (reg.LoadNumber (_T ("PageRight"), &dwTemp))
        szBottomRight.cx = dwTemp;
      if (reg.LoadNumber (_T ("PageTop"), &dwTemp))
        szTopLeft.cy = dwTemp;
      if (reg.LoadNumber (_T ("PageBottom"), &dwTemp))
        szBottomRight.cy = dwTemp;
    }
  pdc->HIMETRICtoLP (&szTopLeft);
  pdc->HIMETRICtoLP (&szBottomRight);
  m_rcPrintArea.left += szTopLeft.cx;
  m_rcPrintArea.right -= szBottomRight.cx;
  m_rcPrintArea.top += szTopLeft.cy;
  m_rcPrintArea.bottom -= szBottomRight.cy;
  if (m_bPrintHeader)
    m_rcPrintArea.top += m_nPrintLineHeight + m_nPrintLineHeight / 2;
  if (m_bPrintFooter)
    m_rcPrintArea.bottom += m_nPrintLineHeight + m_nPrintLineHeight / 2;

  int nLimit = 32;
  m_nPrintPages = 1;
  m_pnPages = new int[nLimit];
  m_pnPages[0] = 0;

  int nLineCount = GetLineCount ();
  int nLine = 1;
  int y = m_rcPrintArea.top + PrintLineHeight (pdc, 0);
  while (nLine < nLineCount)
    {
      int nHeight = PrintLineHeight (pdc, nLine);
      if (y + nHeight <= m_rcPrintArea.bottom)
        {
          y += nHeight;
        }
      else
        {
          ASSERT (nLimit >= m_nPrintPages);
          if (nLimit <= m_nPrintPages)
            {
              nLimit += 32;
              int *pnNewPages = new int[nLimit];
              memcpy (pnNewPages, m_pnPages, sizeof (int) * m_nPrintPages);
              delete[] m_pnPages;
              m_pnPages = pnNewPages;
            }
          ASSERT (nLimit > m_nPrintPages);
          m_pnPages[m_nPrintPages++] = nLine;
          y = m_rcPrintArea.top + nHeight;
        }
      nLine++;
    }
}

void CCrystalTextView::
OnBeginPrinting (CDC * pdc, CPrintInfo * pInfo)
{
  ASSERT (m_pnPages == NULL);
  ASSERT (m_pPrintFont == NULL);
  CFont *pDisplayFont = GetFont ();

  LOGFONT lf;
  pDisplayFont->GetLogFont (&lf);

  CDC *pDisplayDC = GetDC ();
  lf.lfHeight = MulDiv (lf.lfHeight, pdc->GetDeviceCaps (LOGPIXELSY), pDisplayDC->GetDeviceCaps (LOGPIXELSY) * 2);
  lf.lfWidth = MulDiv (lf.lfWidth, pdc->GetDeviceCaps (LOGPIXELSX), pDisplayDC->GetDeviceCaps (LOGPIXELSX) * 2);
  ReleaseDC (pDisplayDC);

  m_pPrintFont = new CFont;
  if (!m_pPrintFont->CreateFontIndirect (&lf))
    {
      delete m_pPrintFont;
      m_pPrintFont = NULL;
      return;
    }

  pdc->SelectObject (m_pPrintFont);
}

void CCrystalTextView::
OnEndPrinting (CDC * pdc, CPrintInfo * pInfo)
{
  if (m_pPrintFont != NULL)
    {
      delete m_pPrintFont;
      m_pPrintFont = NULL;
    }
  if (m_pnPages != NULL)
    {
      delete[] m_pnPages;
      m_pnPages = NULL;
    }
  m_nPrintPages = 0;
  m_nPrintLineHeight = 0;
}

void CCrystalTextView::
OnPrint (CDC * pdc, CPrintInfo * pInfo)
{
  if (m_pnPages == NULL)
    {
      RecalcPageLayouts (pdc, pInfo);
      ASSERT (m_pnPages != NULL);
    }

  ASSERT (pInfo->m_nCurPage >= 1 && (int) pInfo->m_nCurPage <= m_nPrintPages);
  int nLine = m_pnPages[pInfo->m_nCurPage - 1];
  int nEndLine = GetLineCount ();
  if ((int) pInfo->m_nCurPage < m_nPrintPages)
    nEndLine = m_pnPages[pInfo->m_nCurPage];
  TRACE (_T ("Printing page %d of %d, lines %d - %d\n"), pInfo->m_nCurPage, m_nPrintPages,
         nLine, nEndLine - 1);

  if (m_bPrintHeader)
    PrintHeader (pdc, pInfo->m_nCurPage);
  if (m_bPrintFooter)
    PrintFooter (pdc, pInfo->m_nCurPage);

  int y = m_rcPrintArea.top;
  for (; nLine < nEndLine; nLine++)
    {
      int nLineLength = GetLineLength (nLine);
      if (nLineLength == 0)
        {
          y += m_nPrintLineHeight;
          continue;
        }

      CRect rcPrintRect = m_rcPrintArea;
      rcPrintRect.top = y;
      LPCTSTR pszChars = GetLineChars (nLine);
      CString line;
      ExpandChars (pszChars, 0, nLineLength, line);
      y += pdc->DrawText (line, &rcPrintRect, DT_LEFT | DT_NOPREFIX | DT_TOP | DT_WORDBREAK);
    }
}


/////////////////////////////////////////////////////////////////////////////
// CCrystalTextView message handlers

int CCrystalTextView::
GetLineCount ()
{
  if (m_pTextBuffer == NULL)
    return 1;                   //  Single empty line

  int nLineCount = m_pTextBuffer->GetLineCount ();
  ASSERT (nLineCount > 0);
  return nLineCount;
}

//BEGIN SW
int CCrystalTextView::GetSubLineCount()
{
  const int nLineCount = GetLineCount();

  // if we do not wrap words, number of sub lines is
  // equal to number of lines
  if( !m_bWordWrap )
    return nLineCount;

  // calculate number of sub lines
  int nSubLineCount = 0;

  for( int i = 0; i < nLineCount; i++ )
    nSubLineCount+= GetSubLines( i );

  return nSubLineCount;
}

int CCrystalTextView::GetSubLineIndex( int nLineIndex )
{
  // if we do not wrap words, subline index of this line is equal to its index
  if( !m_bWordWrap )
    return nLineIndex;

  // calculate subline index of the line
  int	nSubLineCount = 0;
  int nLineCount = GetLineCount();

  if( nLineIndex >= nLineCount )
    nLineIndex = nLineCount - 1;

  for( int i = 0; i < nLineIndex; i++ )
    nSubLineCount+= GetSubLines( i );

  return nSubLineCount;
}

void CCrystalTextView::GetLineBySubLine( int nSubLineIndex, int &nLine, int &nSubLine )
{
  ASSERT( nSubLineIndex < GetSubLineCount() );

  // if we do not wrap words, nLine is equal to nSubLineIndex and nSubLine is allways 0
  if( !m_bWordWrap )
    {
      nLine = nSubLineIndex;
      nSubLine = 0;
    }

  // compute result
  int	nSubLineCount = 0;
  int nLastSubLines = 0;
  const int nLineCount = GetLineCount();

  for( int i = 0; i < nLineCount; i++ )
    {
      nLastSubLines = GetSubLines( i );
      nSubLineCount += nLastSubLines;
      if( !(nSubLineCount <= nSubLineIndex) )
        break;
    }

  ASSERT( i < nLineCount );
  nLine = i;
  nSubLine = nSubLineIndex - (nSubLineCount - nLastSubLines);
}
//END SW

int CCrystalTextView::
GetLineLength (int nLineIndex)
{
  if (m_pTextBuffer == NULL)
    return 0;
  return m_pTextBuffer->GetLineLength (nLineIndex);
}

LPCTSTR CCrystalTextView::
GetLineChars (int nLineIndex)
{
  if (m_pTextBuffer == NULL)
    return NULL;
  return m_pTextBuffer->GetLineChars (nLineIndex);
}

void CCrystalTextView::
AttachToBuffer (CCrystalTextBuffer * pBuf /*= NULL*/ )
{
  if (m_pTextBuffer != NULL)
    m_pTextBuffer->RemoveView (this);
  if (pBuf == NULL)
    {
      pBuf = LocateTextBuffer ();
      //  ...
    }
  m_pTextBuffer = pBuf;
  if (m_pTextBuffer != NULL)
    m_pTextBuffer->AddView (this);
  ResetView ();

  //  Init scrollbars
  CScrollBar *pVertScrollBarCtrl = GetScrollBarCtrl (SB_VERT);
  if (pVertScrollBarCtrl != NULL)
    pVertScrollBarCtrl->EnableScrollBar (GetScreenLines () >= GetLineCount ()?
                                         ESB_DISABLE_BOTH : ESB_ENABLE_BOTH);
  CScrollBar *pHorzScrollBarCtrl = GetScrollBarCtrl (SB_HORZ);
  if (pHorzScrollBarCtrl != NULL)
    pHorzScrollBarCtrl->EnableScrollBar (GetScreenChars () >= GetMaxLineLength ()?
                                         ESB_DISABLE_BOTH : ESB_ENABLE_BOTH);

  //  Update scrollbars
  RecalcVertScrollBar ();
  RecalcHorzScrollBar ();
}

void CCrystalTextView::
DetachFromBuffer ()
{
  if (m_pTextBuffer != NULL)
    {
      m_pTextBuffer->RemoveView (this);
      m_pTextBuffer = NULL;
      ResetView ();
    }
}

int CCrystalTextView::
GetScreenLines ()
{
  if (m_nScreenLines == -1)
    {
      CRect rect;
      GetClientRect (&rect);
      m_nScreenLines = rect.Height () / GetLineHeight ();
    }
  return m_nScreenLines;
}

BOOL CCrystalTextView::
GetItalic (int nColorIndex)
{
  // WINMERGE - since italic text has problems,
  // lets disable it. E.g. "_" chars disappear and last
  // char may be cropped.
  return FALSE;
  
  // return nColorIndex == COLORINDEX_COMMENT;
}

BOOL CCrystalTextView::
GetBold (int nColorIndex)
{
  return nColorIndex == COLORINDEX_KEYWORD;
  //  return FALSE;
}

int CCrystalTextView::
GetScreenChars ()
{
  if (m_nScreenChars == -1)
    {
      CRect rect;
      GetClientRect (&rect);
      m_nScreenChars = (rect.Width () - GetMarginWidth ()) / GetCharWidth ();
    }
  return m_nScreenChars;
}

void CCrystalTextView::
OnDestroy ()
{
  GetFont ()->GetLogFont (&m_lfBaseFont);
  DetachFromBuffer ();
  m_hAccel = NULL;

  CView::OnDestroy ();

  for (int I = 0; I < 4; I++)
    {
      if (m_apFonts[I] != NULL)
        {
          m_apFonts[I]->DeleteObject ();
          delete m_apFonts[I];
          m_apFonts[I] = NULL;
        }
    }
  if (m_pCacheBitmap != NULL)
    {
      delete m_pCacheBitmap;
      m_pCacheBitmap = NULL;
    }
}

BOOL CCrystalTextView::
OnEraseBkgnd (CDC * pdc)
{
  UNREFERENCED_PARAMETER(pdc);
  return TRUE;
}

void CCrystalTextView::
OnSize (UINT nType, int cx, int cy)
{
  CView::OnSize (nType, cx, cy);

  //BEGIN SW
  // get char position of top left visible character with old cached word wrap
  CPoint	topPos;
  SubLineCursorPosToTextPos( CPoint( 0, m_nTopSubLine ), topPos );
  //END SW

  if (m_pCacheBitmap != NULL)
    {
      m_pCacheBitmap->DeleteObject ();
      delete m_pCacheBitmap;
      m_pCacheBitmap = NULL;
    }
  m_nScreenLines = -1;
  m_nScreenChars = -1;

  //BEGIN SW
  // we have to recompute the line wrapping
  InvalidateLineCache( 0, -1 );

  // compute new top sub line
  CPoint	topSubLine;
  CharPosToPoint( topPos.y, topPos.x, topSubLine );
  m_nTopSubLine = topPos.y + topSubLine.y;

  // set caret to right position
  UpdateCaret();
  //END SW

  RecalcVertScrollBar ();
  RecalcHorzScrollBar ();
  PostMessage (WM_COMMAND, ID_FORCE_REDRAW);
}

void CCrystalTextView::
UpdateSiblingScrollPos (BOOL bHorz)
{
  CSplitterWnd *pSplitterWnd = GetParentSplitter (this, FALSE);
  if (pSplitterWnd != NULL)
    {
      //  See CSplitterWnd::IdFromRowCol() implementation for details
      int nCurrentRow = (GetDlgCtrlID () - AFX_IDW_PANE_FIRST) / 16;
      int nCurrentCol = (GetDlgCtrlID () - AFX_IDW_PANE_FIRST) % 16;
      ASSERT (nCurrentRow >= 0 && nCurrentRow < pSplitterWnd->GetRowCount ());
      ASSERT (nCurrentCol >= 0 && nCurrentCol < pSplitterWnd->GetColumnCount ());

      if (bHorz)
    {
          int nCols = pSplitterWnd->GetColumnCount ();
          for (int nCol = 0; nCol < nCols; nCol++)
      {
              if (nCol != nCurrentCol)  //  We don't need to update ourselves
        {
          CCrystalTextView *pSiblingView = GetSiblingView (nCurrentRow, nCol);
          if (pSiblingView != NULL)
            pSiblingView->OnUpdateSibling (this, FALSE);
        }
      }
    }
      else
    {
          int nRows = pSplitterWnd->GetRowCount ();
          for (int nRow = 0; nRow < nRows; nRow++)
      {
              if (nRow != nCurrentRow)  //  We don't need to update ourselves
        {
          CCrystalTextView *pSiblingView = GetSiblingView (nRow, nCurrentCol);
          if (pSiblingView != NULL)
            pSiblingView->OnUpdateSibling (this, FALSE);
        }
      }
    }
    

    }
}

void CCrystalTextView::
OnUpdateSibling (CCrystalTextView * pUpdateSource, BOOL bHorz)
{
  if (pUpdateSource != this)
    {
      ASSERT (pUpdateSource != NULL);
      ASSERT_KINDOF (CCrystalTextView, pUpdateSource);
      if (bHorz)
        {
          ASSERT (pUpdateSource->m_nTopLine >= 0);
          ASSERT (pUpdateSource->m_nTopLine < GetLineCount ());
          if (pUpdateSource->m_nTopLine != m_nTopLine)
            {
              ScrollToLine (pUpdateSource->m_nTopLine, TRUE, FALSE);
              UpdateCaret ();
            }
        }
      else
        {
          ASSERT (pUpdateSource->m_nOffsetChar >= 0);
          ASSERT (pUpdateSource->m_nOffsetChar < GetMaxLineLength ());
          if (pUpdateSource->m_nOffsetChar != m_nOffsetChar)
            {
              ScrollToChar (pUpdateSource->m_nOffsetChar, TRUE, FALSE);
              UpdateCaret ();
            }
        }
    }
}

void CCrystalTextView::
RecalcVertScrollBar (BOOL bPositionOnly /*= FALSE*/ )
{
  SCROLLINFO si;
  si.cbSize = sizeof (si);
  if (bPositionOnly)
    {
      si.fMask = SIF_POS;
      si.nPos = m_nTopSubLine;
    }
  else
    {
      const int nScreenLines = GetScreenLines();
      if( nScreenLines >= GetSubLineCount() && m_nTopSubLine > 0 )
        {
          m_nTopLine = 0;
          Invalidate ();
          UpdateCaret ();
        }
      si.fMask = SIF_DISABLENOSCROLL | SIF_PAGE | SIF_POS | SIF_RANGE;
      si.nMin = 0;
      si.nMax = GetSubLineCount() - 1;
      si.nPage = nScreenLines;
      si.nPos = m_nTopSubLine;
    }
  VERIFY (SetScrollInfo (SB_VERT, &si));
}

void CCrystalTextView::
OnVScroll (UINT nSBCode, UINT nPos, CScrollBar * pScrollBar)
{
  CView::OnVScroll (nSBCode, nPos, pScrollBar);

  // Note we cannot use nPos because of its 16-bit nature
  SCROLLINFO si = {0};
  si.cbSize = sizeof (si);
  si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_TRACKPOS;
  VERIFY (GetScrollInfo (SB_VERT, &si));

  // Get the minimum and maximum scroll-bar positions.
  int nMinPos = si.nMin;
  int nMaxPos = si.nMax;

  // Get the current position of scroll box.
  int nCurPos = si.nPos;

  BOOL bDisableSmooth = TRUE;
  switch (nSBCode)
    {
    case SB_TOP:			// Scroll to top.
      nCurPos = nMinPos;
      bDisableSmooth = FALSE;
      break;

    case SB_BOTTOM:			// Scroll to bottom.
      nCurPos = nMaxPos;
      bDisableSmooth = FALSE;
      break;

    case SB_LINEUP:			// Scroll one line up.
      if (nCurPos > nMinPos)
        nCurPos--;
      break;

    case SB_LINEDOWN:		// Scroll one line down.
      if (nCurPos < nMaxPos)
        nCurPos++;
      break;

    case SB_PAGEUP:			// Scroll one page up.
      nCurPos = max(nMinPos, nCurPos - (int) si.nPage + 1);
      bDisableSmooth = FALSE;
      break;

    case SB_PAGEDOWN:		// Scroll one page down.
      nCurPos = min(nMaxPos, nCurPos + (int) si.nPage - 1);
      bDisableSmooth = FALSE;
      break;

    case SB_THUMBPOSITION:		// Scroll to absolute position. nPos is the position
      nCurPos = si.nTrackPos;	// of the scroll box at the end of the drag operation.
      break;

    case SB_THUMBTRACK:			// Drag scroll box to specified position. nPos is the
      nCurPos = si.nTrackPos;	// position that the scroll box has been dragged to.
      break;
    }
  ScrollToSubLine(nCurPos, bDisableSmooth);
}

void CCrystalTextView::
RecalcHorzScrollBar (BOOL bPositionOnly /*= FALSE*/ )
{
  //  Again, we cannot use nPos because it's 16-bit
  SCROLLINFO si = {0};
  const int nScreenChars = GetScreenChars();
  const int nMaxLineLen = GetMaxLineLength ();
  si.cbSize = sizeof (si);
  if (bPositionOnly)
    {
      si.fMask = SIF_POS;
      si.nPos = m_nOffsetChar;
    }
  else
    {
      if (nScreenChars >= nMaxLineLen && m_nOffsetChar > 0)
        {
          m_nOffsetChar = 0;
          Invalidate ();
          UpdateCaret ();
        }
      si.fMask = SIF_DISABLENOSCROLL | SIF_PAGE | SIF_POS | SIF_RANGE;
      si.nMin = 0;

      // Horiz scroll limit to longest line + one screenwidth 
      si.nMax = nMaxLineLen + nScreenChars + 1;
      si.nPage = nScreenChars;
      si.nPos = m_nOffsetChar;
    }
  VERIFY (SetScrollInfo (SB_HORZ, &si));
}

void CCrystalTextView::
OnHScroll (UINT nSBCode, UINT nPos, CScrollBar * pScrollBar)
{
  // Default handler not needed
  //CView::OnHScroll (nSBCode, nPos, pScrollBar);
  SCROLLINFO si = {0};
  si.cbSize = sizeof (si);
  si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_TRACKPOS;
  VERIFY (GetScrollInfo (SB_HORZ, &si));

  // Get the minimum and maximum scroll-bar positions.
  int nMinPos = si.nMin;
  int nMaxPos = si.nMax;

  // Get the current position of scroll box.
  int nCurPos = si.nPos;

  switch (nSBCode)
    {
    case SB_LEFT:			// Scroll to far left.
      nCurPos = nMinPos;
      break;

    case SB_RIGHT:			// Scroll to far right.
      nCurPos = nMaxPos;
      break;

    case SB_ENDSCROLL:		// End scroll.
      break;

    case SB_LINELEFT:		// Scroll left.
      if (nCurPos > nMinPos)
        nCurPos--;
      break;

    case SB_LINERIGHT:		// Scroll right.
      if (nCurPos < nMaxPos)
        nCurPos++;
      break;

    case SB_PAGELEFT:		// Scroll one page left.
      nCurPos = max(nMinPos, nCurPos - (int) si.nPage + 1);
      break;

    case SB_PAGERIGHT:		// Scroll one page right.
      nCurPos = min(nMaxPos, nCurPos + (int) si.nPage - 1);
      break;

    case SB_THUMBPOSITION:		// Scroll to absolute position. nPos is the position
      nCurPos = si.nTrackPos;	// of the scroll box at the end of the drag operation.
      break;

    case SB_THUMBTRACK:			// Drag scroll box to specified position. nPos is the
      nCurPos = si.nTrackPos;	// position that the scroll box has been dragged to.
      break;
    }
  ScrollToChar (nCurPos, TRUE);
  UpdateCaret ();
  UpdateSiblingScrollPos (TRUE);
}

BOOL CCrystalTextView::
OnSetCursor (CWnd * pWnd, UINT nHitTest, UINT message)
{
  if (nHitTest == HTCLIENT)
    {
      CPoint pt;
      ::GetCursorPos (&pt);
      ScreenToClient (&pt);
      if (pt.x < GetMarginWidth ())
        {
          ::SetCursor (::LoadCursor (GetResourceHandle (), MAKEINTRESOURCE (IDR_MARGIN_CURSOR)));
        }
      else
        {
          CPoint ptText = ClientToText (pt);
          PrepareSelBounds ();
          if (IsInsideSelBlock (ptText))
            {
              //  [JRT]:  Support For Disabling Drag and Drop...
              if (!m_bDisableDragAndDrop)   // If Drag And Drop Not Disabled

                ::SetCursor (::LoadCursor (NULL, MAKEINTRESOURCE (IDC_ARROW)));     // Set To Arrow Cursor

            }
          else
            ::SetCursor (::LoadCursor (NULL, MAKEINTRESOURCE (IDC_IBEAM)));
        }
      return TRUE;
    }
  return CView::OnSetCursor (pWnd, nHitTest, message);
}

CPoint CCrystalTextView::
ClientToText (const CPoint & point)
{
  //BEGIN SW
  const int nSubLineCount = GetSubLineCount();
  const int nLineCount = GetLineCount();

  CPoint pt;
  pt.y = m_nTopSubLine + point.y / GetLineHeight();
  if (pt.y >= nSubLineCount)
    pt.y = nSubLineCount - 1;
  if (pt.y < 0)
    pt.y = 0;

  int nLine;
  int nSubLineOffset;
  int nOffsetChar = m_nOffsetChar;

  GetLineBySubLine( pt.y, nLine, nSubLineOffset );
  pt.y = nLine;

  LPCTSTR pszLine = NULL;
  int	nLength = 0;
  int *anBreaks = NULL;
  int	nBreaks = 0;

  if (pt.y >= 0 && pt.y < nLineCount)
    {
      nLength = GetLineLength( pt.y );
      anBreaks = new int[nLength];
      pszLine = GetLineChars(pt.y);
      WrapLineCached( pt.y, GetScreenChars(), anBreaks, nBreaks );

      if( nSubLineOffset > 0 )
        nOffsetChar = anBreaks[nSubLineOffset - 1];
      if( nBreaks > nSubLineOffset )
        nLength = anBreaks[nSubLineOffset] - 1;
    }

  int nPos = nOffsetChar + (point.x - GetMarginWidth()) / GetCharWidth();
  if (nPos < 0)
    nPos = 0;

  int nIndex = 0, nCurPos = 0, n = 0, i = 0;
  const int nTabSize = GetTabSize();

  /*
  if( m_bWordWrap )
    nCurPos = nIndex = nOffsetChar;
  */

  while (nIndex < nLength)
    {
      if( nBreaks && nIndex == anBreaks[i] )
        {
          n = nIndex;
          i++;
        }

      if (pszLine[nIndex] == _T('\t'))
        {
          const int nOffset = nTabSize - nCurPos % nTabSize;
          n += nOffset;
          nCurPos += nOffset;
        }
      else
        {
          n++;
          nCurPos ++;
        }

      if (n > nPos && i == nSubLineOffset)
        break;

      nIndex ++;
    }

  delete[] anBreaks;

  ASSERT(nIndex >= 0 && nIndex <= nLength);
  pt.x = nIndex;
  return pt;

  /*ORIGINAL
  int nLineCount = GetLineCount();

  CPoint pt;
  pt.y = m_nTopLine + point.y / GetLineHeight();
  if (pt.y >= nLineCount)
    pt.y = nLineCount - 1;
  if (pt.y < 0)
    pt.y = 0;

  int nLength = 0;
  LPCTSTR pszLine = NULL;
  if (pt.y >= 0 && pt.y < nLineCount)
  {
    nLength = GetLineLength(pt.y);
    pszLine = GetLineChars(pt.y);
  }

  int nPos = m_nOffsetChar + (point.x - GetMarginWidth()) / GetCharWidth();
  if (nPos < 0)
    nPos = 0;

  int nIndex = 0, nCurPos = 0;
  int nTabSize = GetTabSize();
  while (nIndex < nLength)
  {
    if (pszLine[nIndex] == _T('\t'))
      nCurPos += (nTabSize - nCurPos % nTabSize);
    else
      nCurPos ++;

    if (nCurPos > nPos)
      break;

    nIndex ++;
  }

  ASSERT(nIndex >= 0 && nIndex <= nLength);
  pt.x = nIndex;
  return pt;
  *///END SW
}

#ifdef _DEBUG
void CCrystalTextView::
AssertValidTextPos (const CPoint & point)
{
  if (GetLineCount () > 0)
    {
      ASSERT (m_nTopLine >= 0 && m_nOffsetChar >= 0);
      ASSERT (point.y >= 0 && point.y < GetLineCount ());
      ASSERT (point.x >= 0 && point.x <= GetLineLength (point.y));
    }
}
#endif

bool CCrystalTextView::
IsValidTextPos (const CPoint &point)
{
  return GetLineCount () > 0 && m_nTopLine >= 0 && m_nOffsetChar >= 0 &&
    point.y >= 0 && point.y < GetLineCount () && point.x >= 0 && point.x <= GetLineLength (point.y);
}

bool CCrystalTextView::
IsValidTextPosX (const CPoint &point)
{
  return GetLineCount () > 0 && m_nTopLine >= 0 && m_nOffsetChar >= 0 &&
    point.y >= 0 && point.y < GetLineCount () && point.x >= 0 && point.x <= GetLineLength (point.y);
}

bool CCrystalTextView::
IsValidTextPosY (const CPoint &point)
{
  return GetLineCount () > 0 && m_nTopLine >= 0 && m_nOffsetChar >= 0 &&
    point.y >= 0 && point.y < GetLineCount ();
}

CPoint CCrystalTextView::
TextToClient (const CPoint & point)
{
  ASSERT_VALIDTEXTPOS (point);
  LPCTSTR pszLine = GetLineChars (point.y);

  CPoint pt;
  //BEGIN SW
  CPoint	charPoint;
  int			nSubLineStart = CharPosToPoint( point.y, point.x, charPoint );
  charPoint.y+= GetSubLineIndex( point.y );

  // compute y-position
  pt.y = (charPoint.y - m_nTopSubLine) * GetLineHeight();

  // if pt.x is null, we know the result
  if( charPoint.x == 0 )
    {
      pt.x = GetMarginWidth();
      return pt;
    }

  // we have to calculate x-position
  int	nPreOffset = 0;
  /*ORIGINAL
  pt.y = (point.y - m_nTopLine) * GetLineHeight();
  */
  //END SW
  pt.x = 0;
  int nTabSize = GetTabSize ();
  for (int nIndex = 0; nIndex < point.x; nIndex++)
    {
      //BEGIN SW
      if( nIndex == nSubLineStart )
        nPreOffset = pt.x;
      //END SW
      if (pszLine[nIndex] == _T ('\t'))
        pt.x += (nTabSize - pt.x % nTabSize);
      else
        pt.x++;
    }
  //BEGIN SW
  pt.x-= nPreOffset;
  //END SW

  pt.x = (pt.x - m_nOffsetChar) * GetCharWidth () + GetMarginWidth ();
  return pt;
}

void CCrystalTextView::
InvalidateLines (int nLine1, int nLine2, BOOL bInvalidateMargin /*= FALSE*/ )
{
  bInvalidateMargin = TRUE;
  const int nLineHeight = GetLineHeight();
  if (nLine2 == -1)
    {
      CRect rcInvalid;
      GetClientRect (&rcInvalid);
      if (!bInvalidateMargin)
        rcInvalid.left += GetMarginWidth ();
      //BEGIN SW
      rcInvalid.top = (GetSubLineIndex( nLine1 ) - m_nTopSubLine) * nLineHeight;
      /*ORIGINAL
      rcInvalid.top = (nLine1 - m_nTopLine) * GetLineHeight();
      */
      //END SW
      InvalidateRect (&rcInvalid, FALSE);
    }
  else
    {
      if (nLine2 < nLine1)
        {
          int nTemp = nLine1;
          nLine1 = nLine2;
          nLine2 = nTemp;
        }
      CRect rcInvalid;
      GetClientRect (&rcInvalid);
      if (!bInvalidateMargin)
        rcInvalid.left += GetMarginWidth ();
      //BEGIN SW
      rcInvalid.top = (GetSubLineIndex( nLine1 ) - m_nTopSubLine) * nLineHeight;
      rcInvalid.bottom = (GetSubLineIndex( nLine2 ) - m_nTopSubLine + GetSubLines( nLine2 )) * nLineHeight;
      /*ORIGINAL
      rcInvalid.top = (nLine1 - m_nTopLine) * GetLineHeight();
      rcInvalid.bottom = (nLine2 - m_nTopLine + 1) * GetLineHeight();
      */
      //END SW
      InvalidateRect (&rcInvalid, FALSE);
    }
}

void CCrystalTextView::
SetSelection (const CPoint & ptStart, const CPoint & ptEnd)
{
  ASSERT_VALIDTEXTPOS (ptStart);
  ASSERT_VALIDTEXTPOS (ptEnd);
  if (m_ptSelStart == ptStart)
    {
      if (m_ptSelEnd != ptEnd)
        InvalidateLines (ptEnd.y, m_ptSelEnd.y);
    }
  else
    {
      InvalidateLines (ptStart.y, ptEnd.y);
      InvalidateLines (m_ptSelStart.y, m_ptSelEnd.y);
    }
  m_ptSelStart = ptStart;
  m_ptSelEnd = ptEnd;
}

void CCrystalTextView::
AdjustTextPoint (CPoint & point)
{
  point.x += GetCharWidth () / 2;   //todo

}

void CCrystalTextView::
OnSetFocus (CWnd * pOldWnd)
{
  CView::OnSetFocus (pOldWnd);

  m_bFocused = TRUE;
  if (m_ptSelStart != m_ptSelEnd)
    InvalidateLines (m_ptSelStart.y, m_ptSelEnd.y);
  UpdateCaret ();
}

DWORD CCrystalTextView::
ParseLinePlain (DWORD dwCookie, int nLineIndex, TEXTBLOCK * pBuf, int &nActualItems)
{
  return 0;
}

DWORD CCrystalTextView::
ParseLine (DWORD dwCookie, int nLineIndex, TEXTBLOCK * pBuf, int &nActualItems)
{
  return (this->*(m_CurSourceDef->ParseLineX)) (dwCookie, nLineIndex, pBuf, nActualItems);
}

int CCrystalTextView::
CalculateActualOffset (int nLineIndex, int nCharIndex)
{
  const int nLength = GetLineLength (nLineIndex);
  ASSERT (nCharIndex >= 0 && nCharIndex <= nLength);
  LPCTSTR pszChars = GetLineChars (nLineIndex);
  int nOffset = 0;
  const int nTabSize = GetTabSize ();
  //BEGIN SW
  int			*anBreaks = new int[nLength];
  int			nBreaks = 0;

  /*if( nLength > GetScreenChars() )*/
  WrapLineCached( nLineIndex, GetScreenChars(), anBreaks, nBreaks );

  int	nPreOffset = 0;
  int	nPreBreak = 0;

  if( nBreaks )
  {
    for( int J = nBreaks - 1; J >= 0 && nCharIndex < anBreaks[J]; J-- );
    nPreBreak = anBreaks[J];
  }
  delete[] anBreaks;
  //END SW
  for (int I = 0; I < nCharIndex; I++)
    {
      //BEGIN SW
      if( nPreBreak == I && nBreaks )
      nPreOffset = nOffset;
      //END SW
      if (pszChars[I] == _T ('\t'))
        nOffset += (nTabSize - nOffset % nTabSize);
      else
        nOffset++;
    }
  //BEGIN SW
  if( nPreBreak == I && nBreaks )
    return 0;
  else
    return nOffset - nPreOffset;
  /*ORIGINAL
  return nOffset;
  *///END SW
}

int CCrystalTextView::
ApproxActualOffset (int nLineIndex, int nOffset)
{
  if (nOffset == 0)
    return 0;

  int nLength = GetLineLength (nLineIndex);
  LPCTSTR pszChars = GetLineChars (nLineIndex);
  int nCurrentOffset = 0;
  int nTabSize = GetTabSize ();
  for (int I = 0; I < nLength; I++)
    {
      if (pszChars[I] == _T ('\t'))
        nCurrentOffset += (nTabSize - nCurrentOffset % nTabSize);
      else
        nCurrentOffset++;
      if (nCurrentOffset >= nOffset)
        {
          if (nOffset <= nCurrentOffset - nTabSize / 2)
            return I;
          return I + 1;
        }
    }
  return nLength;
}

void CCrystalTextView::
EnsureVisible (CPoint pt)
{
  //  Scroll vertically
  //BEGIN SW
  int			nSubLineCount = GetSubLineCount();
  int			nNewTopSubLine = m_nTopSubLine;
  CPoint	subLinePos;

  CharPosToPoint( pt.y, pt.x, subLinePos );
  subLinePos.y+= GetSubLineIndex( pt.y );

  if( subLinePos.y >= nNewTopSubLine + GetScreenLines() )
    nNewTopSubLine = subLinePos.y - GetScreenLines() + 1;
  if( subLinePos.y < nNewTopSubLine )
    nNewTopSubLine = subLinePos.y;

  if( nNewTopSubLine < 0 )
    nNewTopSubLine = 0;
  if( nNewTopSubLine >= nSubLineCount )
    nNewTopSubLine = nSubLineCount - 1;
  
  // WINMERGE: This line fixes (cursor) slowdown after merges!
  // I don't know exactly why, but propably we are setting
  // m_nTopLine to zero in ResetView() and are not setting to
  // valid value again.  Maybe this is a good place to set it?
  m_nTopLine = nNewTopSubLine;

  if( nNewTopSubLine != m_nTopSubLine )
    {
      ScrollToSubLine( nNewTopSubLine );
      UpdateCaret();
      UpdateSiblingScrollPos( FALSE );
    }
  /*ORIGINAL
  int nLineCount = GetLineCount();
  int nNewTopLine = m_nTopLine;
  if (pt.y >= nNewTopLine + GetScreenLines())
  {
    nNewTopLine = pt.y - GetScreenLines() + 1;
  }
  if (pt.y < nNewTopLine)
  {
    nNewTopLine = pt.y;
  }

  if (nNewTopLine < 0)
    nNewTopLine = 0;
  if (nNewTopLine >= nLineCount)
    nNewTopLine = nLineCount - 1;

  if (m_nTopLine != nNewTopLine)
  {
    ScrollToLine(nNewTopLine);
    UpdateSiblingScrollPos(TRUE);
  }
  */
  //END SW

  //  Scroll horizontally
  //BEGIN SW
  // we do not need horizontally scrolling, if we wrap the words
  if( m_bWordWrap )
    return;
  //END SW
  int nActualPos = CalculateActualOffset (pt.y, pt.x);
  int nNewOffset = m_nOffsetChar;
  const int nScreenChars = GetScreenChars ();
  if (nActualPos > nNewOffset + nScreenChars)
    {
      // Add 10 chars width space after line
      nNewOffset = nActualPos - nScreenChars + 10;
    }
  if (nActualPos < nNewOffset)
    {
      nNewOffset = nActualPos;
    }

  // Horiz scroll limit to longest line + one screenwidth
  const int nMaxLineLen = GetMaxLineLength ();
  if (nNewOffset >= nMaxLineLen + nScreenChars + 1)
    nNewOffset = nMaxLineLen + nScreenChars;
  if (nNewOffset < 0)
    nNewOffset = 0;

  if (m_nOffsetChar != nNewOffset)
    {
      ScrollToChar (nNewOffset);
      UpdateCaret ();
      UpdateSiblingScrollPos (TRUE);
    }
}

void CCrystalTextView::
OnKillFocus (CWnd * pNewWnd)
{
  CView::OnKillFocus (pNewWnd);

  m_bFocused = FALSE;
  UpdateCaret ();
  if (m_ptSelStart != m_ptSelEnd)
    InvalidateLines (m_ptSelStart.y, m_ptSelEnd.y);
  if (m_bDragSelection)
    {
      ReleaseCapture ();
      KillTimer (m_nDragSelTimer);
      m_bDragSelection = FALSE;
    }
}

void CCrystalTextView::
OnSysColorChange ()
{
  CView::OnSysColorChange ();
  Invalidate ();
}

void CCrystalTextView::
GetText (const CPoint & ptStart, const CPoint & ptEnd, CString & text)
{
  if (m_pTextBuffer != NULL)
    m_pTextBuffer->GetText (ptStart.y, ptStart.x, ptEnd.y, ptEnd.x, text);
  else
    text = _T ("");
}

void CCrystalTextView::
UpdateView (CCrystalTextView * pSource, CUpdateContext * pContext,
            DWORD dwFlags, int nLineIndex /*= -1*/ )
{
  // SetTextType (GetExt (GetDocument ()->GetPathName ()));
  if (dwFlags & UPDATE_RESET)
    {
      ResetView ();
      RecalcVertScrollBar ();
      RecalcHorzScrollBar ();
      return;
    }

  int nLineCount = GetLineCount ();
  ASSERT (nLineCount > 0);
  ASSERT (nLineIndex >= -1 && nLineIndex < nLineCount);
  if ((dwFlags & UPDATE_SINGLELINE) != 0)
    {
      ASSERT (nLineIndex != -1);
      //  All text below this line should be reparsed
      if (m_pdwParseCookies != NULL)
        {
          ASSERT (m_nParseArraySize == nLineCount);
          memset (m_pdwParseCookies + nLineIndex, 0xff, sizeof (DWORD) * (m_nParseArraySize - nLineIndex));
        }
      //  This line'th actual length must be recalculated
      if (m_pnActualLineLength != NULL)
        {
          ASSERT (m_nActualLengthArraySize == nLineCount);
          m_pnActualLineLength[nLineIndex] = -1;
      //BEGIN SW
      InvalidateLineCache( nLineIndex, nLineIndex );
      //END SW
        }
      //  Repaint the lines
      InvalidateLines (nLineIndex, -1, TRUE);
    }
  else
    {
      if (nLineIndex == -1)
        nLineIndex = 0;         //  Refresh all text
      //  All text below this line should be reparsed

      if (m_pdwParseCookies != NULL)
        {
          if (m_nParseArraySize != nLineCount)
            {
              //  Reallocate cookies array
              DWORD *pdwNewArray = new DWORD[nLineCount];
              if (nLineIndex > 0)
                memcpy (pdwNewArray, m_pdwParseCookies, sizeof (DWORD) * nLineIndex);
              delete[] m_pdwParseCookies;
              m_nParseArraySize = nLineCount;
              m_pdwParseCookies = pdwNewArray;
            }
          memset (m_pdwParseCookies + nLineIndex, 0xff, sizeof (DWORD) * (m_nParseArraySize - nLineIndex));
        }
      //  Recalculate actual length for all lines below this
      if (m_pnActualLineLength != NULL)
        {
          if (m_nActualLengthArraySize != nLineCount)
            {
              //  Reallocate actual length array
              int *pnNewArray = new int[nLineCount];
              if (nLineIndex > 0)
                memcpy (pnNewArray, m_pnActualLineLength, sizeof (int) * nLineIndex);
              delete[] m_pnActualLineLength;
              m_nActualLengthArraySize = nLineCount;
              m_pnActualLineLength = pnNewArray;
            }
          memset (m_pnActualLineLength + nLineIndex, 0xff, sizeof (DWORD) * (m_nActualLengthArraySize - nLineIndex));
        }
    //BEGIN SW
    InvalidateLineCache( nLineIndex, -1 );
    //END SW
      //  Repaint the lines
      InvalidateLines (nLineIndex, -1, TRUE);
    }

  //  All those points must be recalculated and validated
  if (pContext != NULL)
    {
      pContext->RecalcPoint (m_ptCursorPos);
      pContext->RecalcPoint (m_ptSelStart);
      pContext->RecalcPoint (m_ptSelEnd);
      pContext->RecalcPoint (m_ptAnchor);
      ASSERT_VALIDTEXTPOS (m_ptCursorPos);
      ASSERT_VALIDTEXTPOS (m_ptSelStart);
      ASSERT_VALIDTEXTPOS (m_ptSelEnd);
      ASSERT_VALIDTEXTPOS (m_ptAnchor);
      if (m_bDraggingText)
        {
          pContext->RecalcPoint (m_ptDraggedTextBegin);
          pContext->RecalcPoint (m_ptDraggedTextEnd);
          ASSERT_VALIDTEXTPOS (m_ptDraggedTextBegin);
          ASSERT_VALIDTEXTPOS (m_ptDraggedTextEnd);
        }
      CPoint ptTopLine (0, m_nTopLine);
      pContext->RecalcPoint (ptTopLine);
      ASSERT_VALIDTEXTPOS (ptTopLine);
      m_nTopLine = ptTopLine.y;
      UpdateCaret ();
    }

  //  Recalculate vertical scrollbar, if needed
  if ((dwFlags & UPDATE_VERTRANGE) != 0)
    {
      if (!m_bVertScrollBarLocked)
        RecalcVertScrollBar ();
    }

  //  Recalculate horizontal scrollbar, if needed
  if ((dwFlags & UPDATE_HORZRANGE) != 0)
    {
      m_nMaxLineLength = -1;
      if (!m_bHorzScrollBarLocked)
        RecalcHorzScrollBar ();
    }
}

HINSTANCE CCrystalTextView::
GetResourceHandle ()
{
#ifdef CRYSEDIT_RES_HANDLE
  return CRYSEDIT_RES_HANDLE;
#else
  if (s_hResourceInst != NULL)
    return s_hResourceInst;
  return AfxGetResourceHandle ();
#endif
}

int CCrystalTextView::
OnCreate (LPCREATESTRUCT lpCreateStruct)
{
  memset (&m_lfBaseFont, 0, sizeof (m_lfBaseFont));
  _tcscpy (m_lfBaseFont.lfFaceName, _T ("FixedSys"));
  m_lfBaseFont.lfHeight = 0;
  m_lfBaseFont.lfWeight = FW_NORMAL;
  m_lfBaseFont.lfItalic = FALSE;
  m_lfBaseFont.lfCharSet = DEFAULT_CHARSET;
  m_lfBaseFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
  m_lfBaseFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
  m_lfBaseFont.lfQuality = DEFAULT_QUALITY;
  m_lfBaseFont.lfPitchAndFamily = DEFAULT_PITCH;

  if (CView::OnCreate (lpCreateStruct) == -1)
    return -1;

  ASSERT (m_hAccel == NULL);
             // vvv GetResourceHandle () ???
  HINSTANCE hInst = AfxFindResourceHandle (MAKEINTRESOURCE(IDR_DEFAULT_ACCEL), RT_ACCELERATOR);
  ASSERT (hInst);
  m_hAccel =::LoadAccelerators (hInst, MAKEINTRESOURCE (IDR_DEFAULT_ACCEL));
  ASSERT (m_hAccel != NULL);
  return 0;
}

void CCrystalTextView::
SetAnchor (const CPoint & ptNewAnchor)
{
  ASSERT_VALIDTEXTPOS (ptNewAnchor);
  m_ptAnchor = ptNewAnchor;
}

void CCrystalTextView::
OnEditOperation (int nAction, LPCTSTR pszText)
{
}

BOOL CCrystalTextView::
PreTranslateMessage (MSG * pMsg)
{
  if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
    {
      if (m_hAccel != NULL)
        {
          if (::TranslateAccelerator (m_hWnd, m_hAccel, pMsg))
            return TRUE;
        }
    }

  return CView::PreTranslateMessage (pMsg);
}

CPoint CCrystalTextView::
GetCursorPos ()
{
  return m_ptCursorPos;
}

void CCrystalTextView::
SetCursorPos (const CPoint & ptCursorPos)
{
  ASSERT_VALIDTEXTPOS (ptCursorPos);
  m_ptCursorPos = ptCursorPos;
  m_nIdealCharPos = CalculateActualOffset (m_ptCursorPos.y, m_ptCursorPos.x);
  UpdateCaret ();
}

void CCrystalTextView::
SetSelectionMargin (BOOL bSelMargin)
{
  if (m_bSelMargin != bSelMargin)
    {
      m_bSelMargin = bSelMargin;
      if (::IsWindow (m_hWnd))
        {
          m_nScreenChars = -1;
          Invalidate ();
          RecalcHorzScrollBar ();
        }
    }
}

void CCrystalTextView::
GetFont (LOGFONT & lf)
{
  lf = m_lfBaseFont;
}

void CCrystalTextView::
SetFont (const LOGFONT & lf)
{
  m_lfBaseFont = lf;
  m_nScreenLines = -1;
  m_nScreenChars = -1;
  m_nCharWidth = -1;
  m_nLineHeight = -1;
  if (m_pCacheBitmap != NULL)
    {
      m_pCacheBitmap->DeleteObject ();
      delete m_pCacheBitmap;
      m_pCacheBitmap = NULL;
    }
  for (int I = 0; I < 4; I++)
    {
      if (m_apFonts[I] != NULL)
        {
          m_apFonts[I]->DeleteObject ();
          delete m_apFonts[I];
          m_apFonts[I] = NULL;
        }
    }
  if (::IsWindow (m_hWnd))
    {
      RecalcVertScrollBar ();
      RecalcHorzScrollBar ();
      UpdateCaret ();
      Invalidate ();
    }
}

void CCrystalTextView::
OnUpdateIndicatorPosition (CCmdUI * pCmdUI)
{
  ASSERT_VALIDTEXTPOS (m_ptCursorPos);
  CString stat;
                                                   // VVV m_ptCursorPos.x + 1 ???
  stat.Format (_T ("Ln %d, Col %d"), m_ptCursorPos.y + 1, m_nIdealCharPos + 1);
  pCmdUI->SetText (stat);
  //BEGIN SW
  if( pCmdUI->m_pOther && pCmdUI->m_pOther->IsKindOf( RUNTIME_CLASS(CStatusBar) ) )
    OnUpdateStatusMessage( (CStatusBar*)pCmdUI->m_pOther );
  //END SW
}

void CCrystalTextView::
OnUpdateIndicatorCRLF (CCmdUI * pCmdUI)
{
  if (m_pTextBuffer != NULL)
    {
      int crlfMode = m_pTextBuffer->GetCRLFMode ();
      switch (crlfMode)
        {
        case CRLF_STYLE_DOS:
          pCmdUI->SetText (_T ("DOS"));
          pCmdUI->Enable (TRUE);
          break;
        case CRLF_STYLE_UNIX:
          pCmdUI->SetText (_T ("UNIX"));
          pCmdUI->Enable (TRUE);
          break;
        case CRLF_STYLE_MAC:
          pCmdUI->SetText (_T ("MAC"));
          pCmdUI->Enable (TRUE);
          break;
        default:
          pCmdUI->SetText (NULL);
          pCmdUI->Enable (FALSE);
        }
    }
  else
    {
      pCmdUI->SetText (NULL);
      pCmdUI->Enable (FALSE);
    }
}

void CCrystalTextView::
OnToggleBookmark (UINT nCmdID)
{
  int nBookmarkID = nCmdID - ID_EDIT_TOGGLE_BOOKMARK0;
  ASSERT (nBookmarkID >= 0 && nBookmarkID <= 9);
  if (m_pTextBuffer != NULL)
    {
      DWORD dwFlags = GetLineFlags (m_ptCursorPos.y);
      DWORD dwMask = LF_BOOKMARK (nBookmarkID);
      m_pTextBuffer->SetLineFlag (m_ptCursorPos.y, dwMask, (dwFlags & dwMask) == 0);
    }
}

void CCrystalTextView::
OnGoBookmark (UINT nCmdID)
{
  int nBookmarkID = nCmdID - ID_EDIT_GO_BOOKMARK0;
  ASSERT (nBookmarkID >= 0 && nBookmarkID <= 9);
  if (m_pTextBuffer != NULL)
    {
      int nLine = m_pTextBuffer->GetLineWithFlag (LF_BOOKMARK (nBookmarkID));
      if (nLine >= 0)
        {
          CPoint pt (0, nLine);
          ASSERT_VALIDTEXTPOS (pt);
          SetCursorPos (pt);
          SetSelection (pt, pt);
          SetAnchor (pt);
          EnsureVisible (pt);
        }
    }
}

void CCrystalTextView::
OnClearBookmarks ()
{
  if (m_pTextBuffer != NULL)
    {
      for (int nBookmarkID = 0; nBookmarkID <= 9; nBookmarkID++)
        {
          int nLine = m_pTextBuffer->GetLineWithFlag (LF_BOOKMARK (nBookmarkID));
          if (nLine >= 0)
            {
              m_pTextBuffer->SetLineFlag (nLine, LF_BOOKMARK (nBookmarkID), FALSE);
            }
        }

    }
}

void CCrystalTextView::
ShowCursor ()
{
  m_bCursorHidden = FALSE;
  UpdateCaret ();
}

void CCrystalTextView::
HideCursor ()
{
  m_bCursorHidden = TRUE;
  UpdateCaret ();
}

void CCrystalTextView::
PopCursor ()
{
  if (IsValidTextPosY (m_ptCursorLast))
    {
      if (!IsValidTextPosX (m_ptCursorLast))
        m_ptCursorLast.x = 0;
      ASSERT_VALIDTEXTPOS (m_ptCursorLast);
      CPoint ptCursorPos = m_ptCursorLast;
      SetCursorPos (ptCursorPos);
      SetSelection (ptCursorPos, ptCursorPos);
      SetAnchor (ptCursorPos);
      EnsureVisible (ptCursorPos);
    }
}

void CCrystalTextView::
PushCursor ()
{
  m_ptCursorLast = m_ptCursorPos;
}

DROPEFFECT CCrystalTextView::
GetDropEffect ()
{
  return DROPEFFECT_COPY;
}

void CCrystalTextView::
OnDropSource (DROPEFFECT de)
{
  ASSERT (de == DROPEFFECT_COPY);
}

HGLOBAL CCrystalTextView::
PrepareDragData ()
{
  PrepareSelBounds ();
  if (m_ptDrawSelStart == m_ptDrawSelEnd)
    return NULL;

  CString text;
  GetText (m_ptDrawSelStart, m_ptDrawSelEnd, text);
  HGLOBAL hData =::GlobalAlloc (GMEM_MOVEABLE | GMEM_DDESHARE, _tcslen (text) + 1);
  if (hData == NULL)
    return NULL;

  LPTSTR pszData = (LPTSTR)::GlobalLock (hData);
  _tcscpy (pszData, text.GetBuffer (0));
  text.ReleaseBuffer ();
  ::GlobalUnlock (hData);

  m_ptDraggedTextBegin = m_ptDrawSelStart;
  m_ptDraggedTextEnd = m_ptDrawSelEnd;
  return hData;
}

static int
FindStringHelper (LPCTSTR pszFindWhere, LPCTSTR pszFindWhat, DWORD dwFlags, int &nLen, RxNode *&rxnode, RxMatchRes *rxmatch)
{
  if (dwFlags & FIND_REGEXP)
    {
      int pos;

      if (rxnode)
        RxFree (rxnode);
      rxnode = RxCompile (pszFindWhat);
      if (rxnode && RxExec (rxnode, pszFindWhere, _tcslen (pszFindWhere), pszFindWhere, rxmatch, (dwFlags & FIND_MATCH_CASE) != 0 ? RX_CASE : 0))
        {
          pos = rxmatch->Open[0];
          nLen = rxmatch->Close[0] - rxmatch->Open[0];
        }
      else
        {
          pos = -1;
        }
      return pos;
    }
  else
    {
      ASSERT (pszFindWhere != NULL);
      ASSERT (pszFindWhat != NULL);
      int nCur = 0;
      int nLength = _tcslen (pszFindWhat);
      nLen = nLength;
      for (;;)
        {
          LPCTSTR pszPos = _tcsstr (pszFindWhere, pszFindWhat);
          if (pszPos == NULL)
            return -1;
          if ((dwFlags & FIND_WHOLE_WORD) == 0)
            return nCur + (pszPos - pszFindWhere);
          if (pszPos > pszFindWhere && xisalnum (pszPos[-1]))
            {
              nCur += (pszPos - pszFindWhere);
              pszFindWhere = pszPos + 1;
              continue;
            }
          if (xisalnum (pszPos[nLength]))
            {
              nCur += (pszPos - pszFindWhere + 1);
              pszFindWhere = pszPos + 1;
              continue;
            }
          return nCur + (pszPos - pszFindWhere);
        }
    }
  ASSERT (FALSE);               // Unreachable

  return -1;
}

BOOL CCrystalTextView::
HighlightText (const CPoint & ptStartPos, int nLength, BOOL bReverse /*= FALSE*/)
{
  ASSERT_VALIDTEXTPOS (ptStartPos);
  CPoint ptEndPos = ptStartPos;
  int nCount = GetLineLength (ptEndPos.y) - ptEndPos.x;
  if (nLength <= nCount)
    {
      ptEndPos.x += nLength;
    }
  else
    {
      while (nLength > nCount)
        {
          nLength -= nCount + 1;
          nCount = GetLineLength (++ptEndPos.y);
        }
      ptEndPos.x = nLength;
    }
  ASSERT_VALIDTEXTPOS (m_ptCursorPos);  //  Probably 'nLength' is bigger than expected...

  m_ptCursorPos = bReverse ? ptStartPos : ptEndPos;
  m_ptAnchor = m_ptCursorPos;
  SetSelection (ptStartPos, ptEndPos);
  UpdateCaret ();
  EnsureVisible (m_ptCursorPos);
  return TRUE;
}

BOOL CCrystalTextView::
FindText (LPCTSTR pszText, const CPoint & ptStartPos, DWORD dwFlags,
          BOOL bWrapSearch, CPoint * pptFoundPos)
{
  int nLineCount = GetLineCount ();
  return FindTextInBlock (pszText, ptStartPos, CPoint (0, 0),
                          CPoint (GetLineLength (nLineCount - 1), nLineCount - 1),
                          dwFlags, bWrapSearch, pptFoundPos);
}

int HowManyStr (LPCTSTR s, LPCTSTR m)
{
  LPCTSTR p = s;
  int n = 0, l = _tcslen (m);
  while ((p = _tcsstr (p, m)) != NULL)
    {
      n++;
      p += l;
    }
  return n;
}

int HowManyStr (LPCTSTR s, TCHAR c)
{
  LPCTSTR p = s;
  int n = 0;
  while ((p = _tcschr (p, c)) != NULL)
    {
      n++;
      p++;
    }
  return n;
}

BOOL CCrystalTextView::
FindTextInBlock (LPCTSTR pszText, const CPoint & ptStartPosition,
                 const CPoint & ptBlockBegin, const CPoint & ptBlockEnd,
                 DWORD dwFlags, BOOL bWrapSearch, CPoint * pptFoundPos)
{
  CPoint ptCurrentPos = ptStartPosition;

  ASSERT (pszText != NULL && _tcslen (pszText) > 0);
  ASSERT_VALIDTEXTPOS (ptCurrentPos);
  ASSERT_VALIDTEXTPOS (ptBlockBegin);
  ASSERT_VALIDTEXTPOS (ptBlockEnd);
  ASSERT (ptBlockBegin.y < ptBlockEnd.y || ptBlockBegin.y == ptBlockEnd.y &&
          ptBlockBegin.x <= ptBlockEnd.x);
  if (ptBlockBegin == ptBlockEnd)
    return FALSE;
  CWaitCursor waitCursor;
  if (ptCurrentPos.y < ptBlockBegin.y || ptCurrentPos.y == ptBlockBegin.y &&
        ptCurrentPos.x < ptBlockBegin.x)
    ptCurrentPos = ptBlockBegin;

  CString what = pszText;
  int nEolns;
  if (dwFlags & FIND_REGEXP)
    {
      nEolns = HowManyStr (what, _T("\\n"));
    }
  else
    {
      nEolns = 0;
      if ((dwFlags & FIND_MATCH_CASE) == 0)
        what.MakeUpper ();
    }
  if (dwFlags & FIND_DIRECTION_UP)
    {
      //  Let's check if we deal with whole text.
      //  At this point, we cannot search *up* in selection
      ASSERT (ptBlockBegin.x == 0 && ptBlockBegin.y == 0);
      ASSERT (ptBlockEnd.x == GetLineLength (GetLineCount () - 1) &&
              ptBlockEnd.y == GetLineCount () - 1);

      //  Proceed as if we have whole text search.
      for (;;)
        {
          while (ptCurrentPos.y >= 0)
            {
              int nLineLength;
              CString line;
              if (dwFlags & FIND_REGEXP)
                {
                  for (int i = 0; i <= nEolns && ptCurrentPos.y >= i; i++)
                    {
                      CString item;
                      LPCTSTR pszChars = GetLineChars (ptCurrentPos.y - i);
                      if (i)
                        {
                          nLineLength = GetLineLength (ptCurrentPos.y - i);
                          ptCurrentPos.x = 0;
                          line = _T ('\n') + line;
                        }
                      else
                        {
                          nLineLength = ptCurrentPos.x != -1 ? ptCurrentPos.x : GetLineLength (ptCurrentPos.y - i);
                        }
                      if (nLineLength > 0)
                        {
                          LPTSTR pszBuf = item.GetBuffer (nLineLength + 1);
                          _tcsncpy (pszBuf, pszChars, nLineLength);
                          item.ReleaseBuffer (nLineLength);
                          line = item + line;
                        }
                    }
                  nLineLength = line.GetLength ();
                  if (ptCurrentPos.x == -1)
                    ptCurrentPos.x = 0;
                }
              else
                {
                  nLineLength = GetLineLength(ptCurrentPos.y);
                  //BEGIN SW+FP
                  if (ptCurrentPos.x == -1)
                    {
                      ptCurrentPos.x = nLineLength;
                    }
                  else
                    if( ptCurrentPos.x >= nLineLength )
                      ptCurrentPos.x = nLineLength - 1;
                  /*ORIGINAL
                  if (ptCurrentPos.x == -1)
                    {
                      nLineLength = GetLineLength (ptCurrentPos.y);
                      ptCurrentPos.x = nLineLength;
                    }
                  else
                    nLineLength = ptCurrentPos.x;
                  if (nLineLength <= 0)
                    {
                      ptCurrentPos.x = -1;
                      ptCurrentPos.y--;
                      continue;
                    }
                  *///END SW

                  LPCTSTR pszChars = GetLineChars (ptCurrentPos.y);
                  //BEGIN SW
                  _tcsncpy(line.GetBuffer(ptCurrentPos.x + 2), pszChars, ptCurrentPos.x + 1);
                  /*ORIGINAL
                  LPTSTR pszBuf = line.GetBuffer (nLineLength + 1);
                  _tcsncpy (pszBuf, pszChars, nLineLength);
                  pszBuf[nLineLength] = _T ('\0');
                  *///END SW
                  line.ReleaseBuffer (ptCurrentPos.x + 1);
                  if ((dwFlags & FIND_MATCH_CASE) == 0)
                    line.MakeUpper ();
                }

              //BEGIN SW
              int	nFoundPos = -1;
              int	nMatchLen = what.GetLength();
              int	nLineLen = line.GetLength();
              int	nPos;
              do
                {
                  nPos = ::FindStringHelper(line, what, dwFlags, m_nLastFindWhatLen, m_rxnode, &m_rxmatch);
                  if( nPos >= 0 )
                    {
                      nFoundPos = (nFoundPos == -1)? nPos : nFoundPos + nPos;
                      nFoundPos+= nMatchLen;
                      line = line.Right( nLineLen - (nMatchLen + nPos) );
                      nLineLen = line.GetLength();
                    }
                }
              while( nPos >= 0 );

              if( nFoundPos >= 0 )	// Found text!
                {
                  ptCurrentPos.x = nFoundPos - nMatchLen;
                  *pptFoundPos = ptCurrentPos;
                  return TRUE;
                }

              ptCurrentPos.y--;
              if( ptCurrentPos.y >= 0 )
                ptCurrentPos.x = GetLineLength( ptCurrentPos.y );
              /*ORIGINAL
              int nPos =::FindStringHelper (line, what, dwFlags, m_nLastFindWhatLen, m_rxnode, &m_rxmatch);
              if (nPos >= 0)    //  Found text!
                {
                  LPTSTR pszText = line.GetBuffer (nLineLength + 1);
                  m_pszMatched = _tcsdup (pszText);
                  line.ReleaseBuffer ();
                  // m_sMatched = line.Mid (nPos);
                  if (nEolns)
                    {
                      CString item = line.Left (nPos);
                      ptCurrentPos.y -= nEolns - HowManyStr (item, _T('\n'));
                      if (ptCurrentPos.y < 0)
                        ptCurrentPos.y = 0;
                      LPCTSTR current = _tcsrchr (item, _T('\n'));
                      if (current)
                        current++;
                      else
                        current = item;
                      ptCurrentPos.x = nPos - (current - (LPCTSTR) item);
                      if (ptCurrentPos.x < 0)
                        ptCurrentPos.x = 0;
                    }
                  else
                    {
                      ptCurrentPos.x = nPos;
                    }
                  *pptFoundPos = ptCurrentPos;
                  return TRUE;
                }
              else
                m_pszMatched = NULL;

              ptCurrentPos.x = -1;
              ptCurrentPos.y--;
              *///END SW
            }

          //  Beginning of text reached
          if (!bWrapSearch)
            return FALSE;

          //  Start again from the end of text
          bWrapSearch = FALSE;
          ptCurrentPos = CPoint (0, GetLineCount () - 1);
        }
    }
  else
    {
      for (;;)
        {
          while (ptCurrentPos.y <= ptBlockEnd.y)
            {
              int nLineLength, nLines;
              CString line;
              if (dwFlags & FIND_REGEXP)
                {
                  nLines = m_pTextBuffer->GetLineCount ();
                  for (int i = 0; i <= nEolns && ptCurrentPos.y + i < nLines; i++)
                    {
                      CString item;
                      LPCTSTR pszChars = GetLineChars (ptCurrentPos.y + i);
                      nLineLength = GetLineLength (ptCurrentPos.y + i);
                      if (i)
                        {
                          line += _T ('\n');
                        }
                      else
                        {
                          pszChars += ptCurrentPos.x;
                          nLineLength -= ptCurrentPos.x;
                        }
                      if (nLineLength > 0)
                        {
                          LPTSTR pszBuf = item.GetBuffer (nLineLength + 1);
                          _tcsncpy (pszBuf, pszChars, nLineLength);
                          item.ReleaseBuffer (nLineLength);
                          line += item;
                        }
                    }
                  nLineLength = line.GetLength ();
                }
              else
                {
                  nLineLength = GetLineLength (ptCurrentPos.y) - ptCurrentPos.x;
                  if (nLineLength <= 0)
                    {
                      ptCurrentPos.x = 0;
                      ptCurrentPos.y++;
                      continue;
                    }

                  LPCTSTR pszChars = GetLineChars (ptCurrentPos.y);
                  pszChars += ptCurrentPos.x;

                  //  Prepare necessary part of line
                  LPTSTR pszBuf = line.GetBuffer (nLineLength + 1);
                  _tcsncpy (pszBuf, pszChars, nLineLength);
                  line.ReleaseBuffer (nLineLength);
                  if ((dwFlags & FIND_MATCH_CASE) == 0)
                    line.MakeUpper ();
                }

              //  Perform search in the line
              int nPos =::FindStringHelper (line, what, dwFlags, m_nLastFindWhatLen, m_rxnode, &m_rxmatch);
              if (nPos >= 0)
                {
                  m_pszMatched = _tcsdup (line);
                  if (nEolns)
                    {
                      CString item = line.Left (nPos);
                      LPCTSTR current = _tcsrchr (item, _T('\n'));
                      if (current)
                        current++;
                      else
                        current = item;
                      nEolns = HowManyStr (item, _T('\n'));
                      if (nEolns)
                        {
                          ptCurrentPos.y += nEolns;
                          ptCurrentPos.x = nPos - (current - (LPCTSTR) item);
                        }
                      else
                        {
                          ptCurrentPos.x += nPos - (current - (LPCTSTR) item);
                        }
                      if (ptCurrentPos.x < 0)
                        ptCurrentPos.x = 0;
                    }
                  else
                    {
                      ptCurrentPos.x += nPos;
                    }
                  //  Check of the text found is outside the block.
                  if (ptCurrentPos.y == ptBlockEnd.y && ptCurrentPos.x >= ptBlockEnd.x)
                    break;

                  *pptFoundPos = ptCurrentPos;
                  return TRUE;
                }
              else
                m_pszMatched = NULL;

              //  Go further, text was not found
              ptCurrentPos.x = 0;
              ptCurrentPos.y++;
            }

          //  End of text reached
          if (!bWrapSearch)
            return FALSE;

          //  Start from the beginning
          bWrapSearch = FALSE;
          ptCurrentPos = ptBlockBegin;
        }
    }

  ASSERT (FALSE);               // Unreachable

  return FALSE;
}

void CCrystalTextView::
OnEditFind ()
{
  CWinApp *pApp = AfxGetApp ();
  ASSERT (pApp != NULL);

  CFindTextDlg dlg (this);
  LastSearchInfos * lastSearch = dlg.GetLastSearchInfos();

  if (m_bLastSearch)
    {
      //  Get the latest search parameters
      lastSearch->m_bMatchCase = (m_dwLastSearchFlags & FIND_MATCH_CASE) != 0;
      lastSearch->m_bWholeWord = (m_dwLastSearchFlags & FIND_WHOLE_WORD) != 0;
      lastSearch->m_bRegExp = (m_dwLastSearchFlags & FIND_REGEXP) != 0;
      lastSearch->m_nDirection = (m_dwLastSearchFlags & FIND_DIRECTION_UP) != 0 ? 0 : 1;
      if (m_pszLastFindWhat != NULL)
        lastSearch->m_sText = m_pszLastFindWhat;
    }
  else
    {
      DWORD dwFlags;
      if (!RegLoadNumber (HKEY_CURRENT_USER, REG_EDITPAD, _T ("FindFlags"), &dwFlags))
        dwFlags = 0;
      lastSearch->m_bMatchCase = (dwFlags & FIND_MATCH_CASE) != 0;
      lastSearch->m_bWholeWord = (dwFlags & FIND_WHOLE_WORD) != 0;
      lastSearch->m_bRegExp = (dwFlags & FIND_REGEXP) != 0;
      lastSearch->m_nDirection = (dwFlags & FIND_DIRECTION_UP) == 0;
      // lastSearch->m_sText = pApp->GetProfileString (REG_FIND_SUBKEY, REG_FIND_WHAT, _T (""));
    }
  dlg.UseLastSearch ();

  //  Take the current selection, if any
  if (IsSelection ())
    {
      CPoint ptSelStart, ptSelEnd;
      GetSelection (ptSelStart, ptSelEnd);
      if (ptSelStart.y == ptSelEnd.y)
        {
          LPCTSTR pszChars = GetLineChars (ptSelStart.y);
          int nChars = ptSelEnd.x - ptSelStart.x;
          _tcsncpy (dlg.m_sText.GetBuffer (nChars + 1), pszChars + ptSelStart.x, nChars);
          dlg.m_sText.ReleaseBuffer (nChars);
        }
    }
  else
    {
      CPoint ptCursorPos = GetCursorPos (), ptStart = WordToLeft (ptCursorPos), ptEnd = WordToRight (ptCursorPos);
      if (IsValidTextPos (ptStart) && IsValidTextPos (ptEnd) && ptStart != ptEnd)
        GetText (ptStart, ptEnd, dlg.m_sText);
    }

  //  Execute Find dialog
  dlg.m_ptCurrentPos = m_ptCursorPos;   //  Search from cursor position

  // m_bShowInactiveSelection = TRUE; // FP: removed because I like it
  dlg.DoModal ();
  // m_bShowInactiveSelection = FALSE; // FP: removed because I like it

  // actually this value doesn't change during doModal, but it may in the future
  lastSearch = dlg.GetLastSearchInfos();

  //  Save search parameters for 'F3' command
  m_bLastSearch = TRUE;
  if (m_pszLastFindWhat != NULL)
    free (m_pszLastFindWhat);
  m_pszLastFindWhat = _tcsdup (lastSearch->m_sText);
  m_dwLastSearchFlags = 0;
  if (lastSearch->m_bMatchCase)
    m_dwLastSearchFlags |= FIND_MATCH_CASE;
  if (lastSearch->m_bWholeWord)
    m_dwLastSearchFlags |= FIND_WHOLE_WORD;
  if (lastSearch->m_bRegExp)
    m_dwLastSearchFlags |= FIND_REGEXP;
  if (lastSearch->m_nDirection == 0)
    m_dwLastSearchFlags |= FIND_DIRECTION_UP;

  //  Save search parameters to registry
  VERIFY (RegSaveNumber (HKEY_CURRENT_USER, REG_EDITPAD, _T ("FindFlags"), m_dwLastSearchFlags));
  // pApp->WriteProfileString (REG_FIND_SUBKEY, REG_FIND_WHAT, lastSearch->m_sText);
}

void CCrystalTextView::
OnEditRepeat ()
{
  BOOL bEnable = m_bLastSearch;
  CString sText;
  if (bEnable)
    sText = m_pszLastFindWhat;
  else
    {
      bEnable = CMemComboBox::groups.Lookup (_T ("FindText"), sText) && !sText.IsEmpty ();
      if (bEnable)
        {
          int pos = sText.Find (_T('\n'));
          if (pos >= 0)
            sText = sText.Left (pos);
        }
    }
  if (bEnable)
    {
      CPoint ptFoundPos;
      //BEGIN SW
      // for correct backward search we need some changes:
      CPoint ptSearchPos = m_ptCursorPos;
      if( m_dwLastSearchFlags & FIND_DIRECTION_UP && IsSelection() )
        {
          CPoint	ptDummy;
          GetSelection( ptSearchPos, ptDummy );
        }

      if (! FindText(sText, ptSearchPos, m_dwLastSearchFlags, TRUE, &ptFoundPos))
      /*ORIGINAL
      if (! FindText(sText, m_ptCursorPos, m_dwLastSearchFlags, TRUE, &ptFoundPos))
      *///END SW
        {
          CString prompt;
          prompt.Format (IDS_EDIT_TEXT_NOT_FOUND, sText);
          AfxMessageBox (prompt);
          return;
        }
      HighlightText (ptFoundPos, m_nLastFindWhatLen, (m_dwLastSearchFlags & FIND_DIRECTION_UP) != 0);
      m_bMultipleSearch = TRUE; // More search

    }
}

void CCrystalTextView::
OnUpdateEditRepeat (CCmdUI * pCmdUI)
{
  BOOL bEnable = m_bLastSearch;
  if (!bEnable)
    {
      CString sText;
      bEnable = CMemComboBox::groups.Lookup (_T ("FindText"), sText) && !sText.IsEmpty ();
    }
  pCmdUI->Enable (bEnable);
}

void CCrystalTextView::
OnEditFindPrevious ()
{
  DWORD dwSaveSearchFlags = m_dwLastSearchFlags;
  if ((m_dwLastSearchFlags & FIND_DIRECTION_UP) != 0)
    m_dwLastSearchFlags &= ~FIND_DIRECTION_UP;
  else
    m_dwLastSearchFlags |= FIND_DIRECTION_UP;
  OnEditRepeat ();
  m_dwLastSearchFlags = dwSaveSearchFlags;
}

void CCrystalTextView::
OnUpdateEditFindPrevious (CCmdUI * pCmdUI)
{
  BOOL bEnable = m_bLastSearch;
  if (!bEnable)
    {
      CString sText;
      bEnable = CMemComboBox::groups.Lookup (_T ("FindText"), sText) && !sText.IsEmpty ();
    }
  pCmdUI->Enable (bEnable);
}

void CCrystalTextView::
OnFilePageSetup ()
{
  CWinApp *pApp = AfxGetApp ();
  ASSERT (pApp != NULL);

  CPageSetupDialog dlg;
  //dlg.m_psd.Flags &= ~PSD_INTHOUSANDTHSOFINCHES;
  dlg.m_psd.Flags = PSD_INHUNDREDTHSOFMILLIMETERS|PSD_MARGINS;
  dlg.m_psd.rtMargin.left = DEFAULT_PRINT_MARGIN;
  dlg.m_psd.rtMargin.right = DEFAULT_PRINT_MARGIN;
  dlg.m_psd.rtMargin.top = DEFAULT_PRINT_MARGIN;
  dlg.m_psd.rtMargin.bottom = DEFAULT_PRINT_MARGIN;
  CReg reg;
  if (reg.Open (HKEY_CURRENT_USER, REG_EDITPAD, KEY_READ))
    {
      DWORD dwTemp;
      if (reg.LoadNumber (_T ("PageWidth"), &dwTemp))
        dlg.m_psd.ptPaperSize.x = dwTemp;
      if (reg.LoadNumber (_T ("PageHeight"), &dwTemp))
        dlg.m_psd.ptPaperSize.y = dwTemp;
      if (reg.LoadNumber (_T ("PageLeft"), &dwTemp))
        dlg.m_psd.rtMargin.left = dwTemp;
      if (reg.LoadNumber (_T ("PageRight"), &dwTemp))
        dlg.m_psd.rtMargin.right = dwTemp;
      if (reg.LoadNumber (_T ("PageTop"), &dwTemp))
        dlg.m_psd.rtMargin.top = dwTemp;
      if (reg.LoadNumber (_T ("PageBottom"), &dwTemp))
        dlg.m_psd.rtMargin.bottom = dwTemp;
    }
  if (dlg.DoModal () == IDOK)
    {
      CReg reg;
      if (reg.Create (HKEY_CURRENT_USER, REG_EDITPAD, KEY_WRITE))
        {
          VERIFY (reg.SaveNumber (_T ("PageWidth"), dlg.m_psd.ptPaperSize.x));
          VERIFY (reg.SaveNumber (_T ("PageHeight"), dlg.m_psd.ptPaperSize.y));
          VERIFY (reg.SaveNumber (_T ("PageLeft"), dlg.m_psd.rtMargin.left));
          VERIFY (reg.SaveNumber (_T ("PageRight"), dlg.m_psd.rtMargin.right));
          VERIFY (reg.SaveNumber (_T ("PageTop"), dlg.m_psd.rtMargin.top));
          VERIFY (reg.SaveNumber (_T ("PageBottom"), dlg.m_psd.rtMargin.bottom));
        }
    }
}

void CCrystalTextView::
OnToggleBookmark ()
{
  if (m_pTextBuffer != NULL)
    {
      DWORD dwFlags = GetLineFlags (m_ptCursorPos.y);
      DWORD dwMask = LF_BOOKMARKS;
      m_pTextBuffer->SetLineFlag (m_ptCursorPos.y, dwMask, (dwFlags & dwMask) == 0, FALSE);
    }
  int nLine = m_pTextBuffer->GetLineWithFlag (LF_BOOKMARKS);
  if (nLine >= 0)
    m_bBookmarkExist = TRUE;
  else
    m_bBookmarkExist = FALSE;
}

void CCrystalTextView::
OnNextBookmark ()
{
  if (m_pTextBuffer != NULL)
    {
      int nLine = m_pTextBuffer->FindNextBookmarkLine (m_ptCursorPos.y);
      if (nLine >= 0)
        {
          CPoint pt (0, nLine);
          ASSERT_VALIDTEXTPOS (pt);
          SetCursorPos (pt);
          SetSelection (pt, pt);
          SetAnchor (pt);
          EnsureVisible (pt);
        }
    }
}

void CCrystalTextView::
OnPrevBookmark ()
{
  if (m_pTextBuffer != NULL)
    {
      int nLine = m_pTextBuffer->FindPrevBookmarkLine (m_ptCursorPos.y);
      if (nLine >= 0)
        {
          CPoint pt (0, nLine);
          ASSERT_VALIDTEXTPOS (pt);
          SetCursorPos (pt);
          SetSelection (pt, pt);
          SetAnchor (pt);
          EnsureVisible (pt);
        }
    }
}

void CCrystalTextView::
OnClearAllBookmarks ()
{
  if (m_pTextBuffer != NULL)
    {
      int nLineCount = GetLineCount ();
      for (int I = 0; I < nLineCount; I++)
        {
          if (m_pTextBuffer->GetLineFlags (I) & LF_BOOKMARKS)
            m_pTextBuffer->SetLineFlag (I, LF_BOOKMARKS, FALSE);
        }
      m_bBookmarkExist = FALSE;
    }
}

void CCrystalTextView::
OnUpdateNextBookmark (CCmdUI * pCmdUI)
{
  pCmdUI->Enable (m_bBookmarkExist);
}

void CCrystalTextView::
OnUpdatePrevBookmark (CCmdUI * pCmdUI)
{
  pCmdUI->Enable (m_bBookmarkExist);
}

void CCrystalTextView::
OnUpdateClearAllBookmarks (CCmdUI * pCmdUI)
{
  pCmdUI->Enable (m_bBookmarkExist);
}

BOOL CCrystalTextView::
GetViewTabs ()
{
  return m_bViewTabs;
}

void CCrystalTextView::
SetViewTabs (BOOL bViewTabs)
{
  if (bViewTabs != m_bViewTabs)
    {
      m_bViewTabs = bViewTabs;
      if (::IsWindow (m_hWnd))
        Invalidate ();
    }
}

void CCrystalTextView::
SetViewEols (BOOL bViewEols)
{
  if (bViewEols != m_bViewEols)
    {
      m_bViewEols = bViewEols;
      if (::IsWindow (m_hWnd))
        Invalidate ();
    }
}

DWORD CCrystalTextView::
GetFlags ()
{
  return m_dwFlags;
}

void CCrystalTextView::
SetFlags (DWORD dwFlags)
{
  if (m_dwFlags != dwFlags)
    {
      m_dwFlags = dwFlags;
      if (::IsWindow (m_hWnd))
        Invalidate ();
    }
}

BOOL CCrystalTextView::
GetSelectionMargin ()
{
  return m_bSelMargin;
}

int CCrystalTextView::
GetMarginWidth ()
{
  return m_bSelMargin ? 20 : 1;
}

BOOL CCrystalTextView::
GetSmoothScroll ()
const
{
  return m_bSmoothScroll;
}

void CCrystalTextView::SetSmoothScroll (BOOL bSmoothScroll)
{
  m_bSmoothScroll = bSmoothScroll;
}

//  [JRT]
BOOL CCrystalTextView::
GetDisableDragAndDrop ()
const
{
  return m_bDisableDragAndDrop;
}

//  [JRT]
void CCrystalTextView::SetDisableDragAndDrop (BOOL bDDAD)
{
  m_bDisableDragAndDrop = bDDAD;
}

//
// Mouse wheel event.  zDelta is in multiples of 120.
// Divide by 40 so each click is 3 lines.  I know some
// drivers let you set the ammount of scroll, but I
// don't know how to retrieve this or if they just
// adjust the zDelta you get here.
BOOL CCrystalTextView::
OnMouseWheel (UINT nFlags, short zDelta, CPoint pt)
{
  // -> HE
// int nPageLines = GetScreenLines();
  int nSubLineCount = GetSubLineCount();

  int nNewTopSubLine= m_nTopSubLine - zDelta / 40;

  if (nNewTopSubLine < 0)
    nNewTopSubLine = 0;
  if (nNewTopSubLine >= nSubLineCount)
    nNewTopSubLine = nSubLineCount - 1;

  ScrollToSubLine(nNewTopSubLine, TRUE);
  UpdateSiblingScrollPos(FALSE);
  // <- HE
/* Old
  int nLineCount = GetLineCount ();

  int nNewTopLine = m_nTopLine - zDelta / 40;

  if (nNewTopLine < 0)
    nNewTopLine = 0;
  if (nNewTopLine >= nLineCount)
    nNewTopLine = nLineCount - 1;

  if (m_nTopLine != nNewTopLine)
    {
      int nScrollLines = m_nTopLine - nNewTopLine;
      m_nTopLine = nNewTopLine;
      ScrollWindow (0, nScrollLines * GetLineHeight ());
      UpdateWindow ();
    }

  RecalcVertScrollBar (TRUE);
*/

  return CView::OnMouseWheel (nFlags, zDelta, pt);
}

void CCrystalTextView::
OnSourceType (UINT nId)
{
  SetTextType ((CCrystalTextView::TextType) (nId - ID_SOURCE_PLAIN));
  Invalidate ();
}

void CCrystalTextView::
OnUpdateSourceType (CCmdUI * pCmdUI)
{
  pCmdUI->SetRadio (m_SourceDefs + (pCmdUI->m_nID - ID_SOURCE_PLAIN) == m_CurSourceDef);
}

int bracetype (TCHAR c);

void CCrystalTextView::
OnMatchBrace ()
{
  CPoint ptCursorPos = GetCursorPos ();
  int nLength = m_pTextBuffer->GetLineLength (ptCursorPos.y);
  LPCTSTR pszText = m_pTextBuffer->GetLineChars (ptCursorPos.y), pszEnd = pszText + ptCursorPos.x;
  bool bAfter = false;
  int nType = 0;
  if (ptCursorPos.x < nLength)
    {
      nType = bracetype (*pszEnd);
      if (nType)
        {
          bAfter = false;
        }
      else if (!nType && ptCursorPos.x > 0)
        {
          nType = bracetype (pszEnd[-1]);
          bAfter = true;
        }
    }
  else if (ptCursorPos.x > 0)
    {
      nType = bracetype (pszEnd[-1]);
      bAfter = true;
    }
  if (nType)
    {
      int nOther, nCount = 0, nComment = 0;
      if (bAfter)
        {
          nOther = ((nType - 1) ^ 1) + 1;
          if (nOther & 1)
            pszEnd--;
        }
      else
        {
          nOther = ((nType - 1) ^ 1) + 1;
          if (!(nOther & 1))
            pszEnd++;
        }
      LPCTSTR pszOpenComment = m_CurSourceDef->opencomment,
        pszCloseComment = m_CurSourceDef->closecomment,
        pszCommentLine = m_CurSourceDef->commentline, pszTest;
      int nOpenComment = _tcslen (pszOpenComment),
        nCloseComment = _tcslen (pszCloseComment),
        nCommentLine = _tcslen (pszCommentLine);
      if (nOther & 1)
        {
          for (;;)
            {
              while (--pszEnd >= pszText)
                {
                  pszTest = pszEnd - nOpenComment + 1;
                  if (pszTest >= pszText && !_tcsnicmp (pszTest, pszOpenComment, nOpenComment))
                    {
                      nComment--;
                      pszEnd = pszTest;
                      if (--pszEnd < pszText)
                        {
                          break;
                        }
                    }
                  pszTest = pszEnd - nCloseComment + 1;
                  if (pszTest >= pszText && !_tcsnicmp (pszTest, pszCloseComment, nCloseComment))
                    {
                      nComment++;
                      pszEnd = pszTest;
                      if (--pszEnd < pszText)
                        {
                          break;
                        }
                    }
                  if (!nComment)
                    {
                      pszTest = pszEnd - nCommentLine + 1;
                      if (pszTest >= pszText && !_tcsnicmp (pszTest, pszCommentLine, nCommentLine))
                        {
                          break;
                        }
                      if (bracetype (*pszEnd) == nType)
                        {
                          nCount++;
                        }
                      else if (bracetype (*pszEnd) == nOther)
                        {
                          if (!nCount--)
                            {
                              ptCursorPos.x = pszEnd - pszText;
                              if (bAfter)
                                ptCursorPos.x++;
                              SetCursorPos (ptCursorPos);
                              SetSelection (ptCursorPos, ptCursorPos);
                              SetAnchor (ptCursorPos);
                              EnsureVisible (ptCursorPos);
                              return;
                            }
                        }
                    }
                }
              if (ptCursorPos.y)
                {
                  ptCursorPos.x = m_pTextBuffer->GetLineLength (--ptCursorPos.y);
                  pszText = m_pTextBuffer->GetLineChars (ptCursorPos.y);
                  pszEnd = pszText + ptCursorPos.x;
                }
              else
                break;
            }
        }
      else
        {
          LPCTSTR pszBegin = pszText;
          pszText = pszEnd;
          pszEnd = pszBegin + nLength;
          int nLines = m_pTextBuffer->GetLineCount ();
          for (;;)
            {
              while (pszText < pszEnd)
                {
                  pszTest = pszText + nCloseComment;
                  if (pszTest <= pszEnd && !_tcsnicmp (pszText, pszCloseComment, nCloseComment))
                    {
                      nComment--;
                      pszText = pszTest;
                      if (pszText > pszEnd)
                        {
                          break;
                        }
                    }
                  pszTest = pszText + nOpenComment;
                  if (pszTest <= pszEnd && !_tcsnicmp (pszText, pszOpenComment, nOpenComment))
                    {
                      nComment++;
                      pszText = pszTest;
                      if (pszText > pszEnd)
                        {
                          break;
                        }
                    }
                  if (!nComment)
                    {
                      pszTest = pszText + nCommentLine;
                      if (pszTest <= pszEnd && !_tcsnicmp (pszText, pszCommentLine, nCommentLine))
                        {
                          break;
                        }
                      if (bracetype (*pszText) == nType)
                        {
                          nCount++;
                        }
                      else if (bracetype (*pszText) == nOther)
                        {
                          if (!nCount--)
                            {
                              ptCursorPos.x = pszText - pszBegin;
                              if (bAfter)
                                ptCursorPos.x++;
                              SetCursorPos (ptCursorPos);
                              SetSelection (ptCursorPos, ptCursorPos);
                              SetAnchor (ptCursorPos);
                              EnsureVisible (ptCursorPos);
                              return;
                            }
                        }
                    }
                  pszText++;
                }
              if (ptCursorPos.y < nLines)
                {
                  ptCursorPos.x = 0;
                  nLength = m_pTextBuffer->GetLineLength (++ptCursorPos.y);
                  pszBegin = pszText = m_pTextBuffer->GetLineChars (ptCursorPos.y);
                  pszEnd = pszBegin + nLength;
                }
              else
                break;
            }
        }
    }
}

void CCrystalTextView::
OnUpdateMatchBrace (CCmdUI * pCmdUI)
{
  CPoint ptCursorPos = GetCursorPos ();
  int nLength = m_pTextBuffer->GetLineLength (ptCursorPos.y);
  LPCTSTR pszText = m_pTextBuffer->GetLineChars (ptCursorPos.y) + ptCursorPos.x;
  pCmdUI->Enable (ptCursorPos.x < nLength && (bracetype (*pszText) || ptCursorPos.x > 0 && bracetype (pszText[-1])) || ptCursorPos.x > 0 && bracetype (pszText[-1]));
}

void CCrystalTextView::
OnEditGoTo ()
{
  CGotoDlg dlg (this);
  dlg.DoModal ();
}

void CCrystalTextView::
OnUpdateToggleSourceHeader (CCmdUI * pCmdUI)
{
  pCmdUI->Enable (m_CurSourceDef->type == SRC_C);
}

void CCrystalTextView::
OnToggleSourceHeader ()
{
  if (m_CurSourceDef->type == SRC_C)
    {
      CDocument *pDoc = GetDocument ();
      ASSERT (pDoc);
      CString sFilePath = pDoc->GetPathName (), sOriginalPath = sFilePath;
      if (!_tcsicmp (sFilePath.Right (2), _T (".c")))
        {
          sFilePath = sFilePath.Left (sFilePath.GetLength () - 1) + _T ('h');
        }
      else if (!_tcsicmp (sFilePath.Right (4), _T (".cpp")))
        {
          sFilePath = sFilePath.Left (sFilePath.GetLength () - 3) + _T ('h');
        }
      else if (!_tcsicmp (sFilePath.Right (4), _T (".inl")))
        {
          sFilePath = sFilePath.Left (sFilePath.GetLength () - 3) + _T ('c');
          if (!FileExist(sFilePath))
            {
              sFilePath = sFilePath + _T ("pp");
            }
        }
      else if (!_tcsicmp (sFilePath.Right (4), _T (".hpp")))
        {
          sFilePath = sFilePath.Left (sFilePath.GetLength () - 3) + _T ("inl");
          if (!FileExist(sFilePath))
            {
              sFilePath = sFilePath.Left (sFilePath.GetLength () - 3) + _T ('c');
              if (!FileExist(sFilePath))
                {
                  sFilePath = sFilePath + _T ("pp");
                }
            }
        }
      else if (!_tcsicmp (sFilePath.Right (2), _T (".h")))
        {
          sFilePath = sFilePath.Left (sFilePath.GetLength () - 1) + _T ("hpp");
          if (!FileExist(sFilePath))
            {
              sFilePath = sFilePath.Left (sFilePath.GetLength () - 3) + _T ("inl");
              if (!FileExist(sFilePath))
                {
                  sFilePath = sFilePath.Left (sFilePath.GetLength () - 3) + _T ('c');
                  if (!FileExist(sFilePath))
                    {
                      sFilePath = sFilePath + _T ("pp");
                    }
                }
            }
        }
      if (FileExist(sFilePath))
        {
          if (!m_bSingle || !pDoc->IsModified () || pDoc->DoSave (sOriginalPath))
            {
              AfxGetApp ()->OpenDocumentFile (sFilePath);
              if (m_bSingle)
                {
                  m_ptCursorLast.x = m_ptCursorLast.y = 0;
                  ASSERT_VALIDTEXTPOS (m_ptCursorLast);
                  CPoint ptCursorPos = m_ptCursorLast;
                  SetCursorPos (ptCursorPos);
                  SetSelection (ptCursorPos, ptCursorPos);
                  SetAnchor (ptCursorPos);
                  EnsureVisible (ptCursorPos);
                  Invalidate ();
                }
            }
        }
    }
}

void CCrystalTextView::
OnUpdateSelMargin (CCmdUI * pCmdUI)
{
  pCmdUI->SetCheck (m_bSelMargin);
}

void CCrystalTextView::
OnSelMargin ()
{
  ASSERT (m_CurSourceDef);
  if (m_bSelMargin)
    {
      m_CurSourceDef->flags &= ~SRCOPT_SELMARGIN;
      SetSelectionMargin (FALSE);
    }
  else
    {
      m_CurSourceDef->flags |= SRCOPT_SELMARGIN;
      SetSelectionMargin (TRUE);
    }
}

void CCrystalTextView::
OnUpdateWordWrap (CCmdUI * pCmdUI)
{
  pCmdUI->SetCheck (m_bWordWrap);
}

void CCrystalTextView::
OnWordWrap ()
{
  ASSERT (m_CurSourceDef);
  if (m_bWordWrap)
    {
      m_CurSourceDef->flags &= ~SRCOPT_WORDWRAP;
      SetWordWrapping (FALSE);
    }
  else
    {
      m_CurSourceDef->flags |= SRCOPT_WORDWRAP;
      SetWordWrapping (TRUE);
    }
}

void CCrystalTextView::
OnForceRedraw ()
{
  //Invalidate ();
  RedrawWindow (NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW);
}

//BEGIN SW
BOOL CCrystalTextView::GetWordWrapping() const
{
  return m_bWordWrap;
}

void CCrystalTextView::SetWordWrapping( BOOL bWordWrap )
{
  m_bWordWrap = bWordWrap;

  if( IsWindow( m_hWnd ) )
    InvalidateLines( 0, -1, TRUE );
}

CCrystalParser *CCrystalTextView::SetParser( CCrystalParser *pParser )
{
  CCrystalParser	*pOldParser = m_pParser;

  m_pParser = pParser;

  if( pParser )
    pParser->m_pTextView = this;

  return pOldParser;
}
//END SW


//BEGIN SW
// incremental search imlementation
BOOL CCrystalTextView::OnCmdMsg( UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO *pHandlerInfo )
{
  // just look for commands
  if( nCode != CN_COMMAND || pExtra )
    return CView::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );

  // handle code:
  // each command that is not related to incremental search
  // ends the incremental search
  if( nID == ID_EDIT_FIND_INCREMENTAL_FORWARD || 
    nID == ID_EDIT_FIND_INCREMENTAL_BACKWARD || 
    nID == ID_EDIT_DELETE_BACK )
    return CView::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );

  if( nID >= ID_EDIT_FIRST && nID <= ID_EDIT_LAST )
    m_bIncrementalSearchForward = m_bIncrementalSearchBackward = FALSE;

  return CView::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
}

void CCrystalTextView::OnChar( UINT nChar, UINT nRepCnt, UINT nFlags )
{
  CView::OnChar( nChar, nRepCnt, nFlags );

  // we only have to handle character-input, if we are in incremental search mode
  if( !m_bIncrementalSearchForward && !m_bIncrementalSearchBackward )
    return;

  // exit incremental search, when Escape is pressed
  if( nChar == VK_ESCAPE )
    {
      // if not end incremental search
      m_bIncrementalSearchForward = m_bIncrementalSearchBackward = FALSE;
      SetSelection( m_selStartBeforeIncrementalSearch, m_selEndBeforeIncrementalSearch );
      SetCursorPos( m_cursorPosBeforeIncrementalSearch );
      EnsureVisible( m_cursorPosBeforeIncrementalSearch );
      return;
    }

  // exit incremental search without destroying selection
  if( nChar == VK_RETURN )
    {
      m_bIncrementalSearchForward = m_bIncrementalSearchBackward = FALSE;
      return;
    }

  // is the character valid for incremental search?
  if( !_istgraph( nChar ) && !(nChar == _T(' ')) && !(nChar == _T('\t')) )
    {
      // if not end incremental search
      m_bIncrementalSearchForward = m_bIncrementalSearchBackward = FALSE;
      return;
    }

  // if last search was not successfull do not add a new character
  if( !m_bIncrementalFound )
    {
      MessageBeep( MB_OK );
      return;
    }

  // add character to incremental search string and search
  *m_pstrIncrementalSearchString += (TCHAR) nChar;
  OnEditFindIncremental();
}


void CCrystalTextView::OnEditDeleteBack() 
{
  if( !m_bIncrementalSearchForward && !m_bIncrementalSearchBackward )
    return;

  // remove last character from search string
  if( m_pstrIncrementalSearchString->IsEmpty() )
    return;

  *m_pstrIncrementalSearchString = m_pstrIncrementalSearchString->Left( m_pstrIncrementalSearchString->GetLength() - 1 );
  OnEditFindIncremental();
}


void CCrystalTextView::OnEditFindIncremental( BOOL bFindNextOccurence /*= FALSE*/ )
{
  // when string is empty, then goto position where the search starts
  if( m_pstrIncrementalSearchString->IsEmpty() )
    {
      SetSelection( m_incrementalSearchStartPos, m_incrementalSearchStartPos );
      SetCursorPos( m_incrementalSearchStartPos );
      EnsureVisible( m_incrementalSearchStartPos );
      return;
    }

  // otherwise search next occurence of search string, 
  // starting at current cursor position
  CPoint	matchStart, matchEnd;

  // calculate start point for search
  if( bFindNextOccurence )
    {
      CPoint	selStart, selEnd;
      GetSelection( selStart, selEnd );
      m_incrementalSearchStartPos = (m_bIncrementalSearchBackward)? selStart : selEnd;
    }

  m_bIncrementalFound = FindText( 
    *m_pstrIncrementalSearchString,
    m_incrementalSearchStartPos,
    m_bIncrementalSearchBackward? FIND_DIRECTION_UP : 0,
    TRUE,
    &matchStart );

  if( !m_bIncrementalFound )
    {
      MessageBeep( MB_OK );
      return;
    }

  // select found text and set cursor to end of match
  matchEnd = matchStart;
  matchEnd.x+= m_pstrIncrementalSearchString->GetLength();
  SetSelection( matchStart, matchEnd );
  SetCursorPos( matchEnd );
  EnsureVisible( matchEnd );
}



void CCrystalTextView::OnEditFindIncrementalForward()
{
  if( !m_bIncrementalSearchForward && !m_bIncrementalSearchBackward )
    {
      // initialize
      if( !m_pstrIncrementalSearchString->IsEmpty() )
        *m_pstrIncrementalSearchStringOld = *m_pstrIncrementalSearchString;
      m_pstrIncrementalSearchString->Empty();
      m_incrementalSearchStartPos = m_cursorPosBeforeIncrementalSearch = m_ptCursorPos;
      GetSelection( m_selStartBeforeIncrementalSearch, m_selEndBeforeIncrementalSearch );
    }
  else if( m_bIncrementalSearchForward )
    {
      if( m_pstrIncrementalSearchString->IsEmpty() )
        {
          *m_pstrIncrementalSearchString = *m_pstrIncrementalSearchStringOld;
          m_pstrIncrementalSearchStringOld->Empty();
          OnEditFindIncremental();
        }
      else
        OnEditFindIncremental( TRUE );

      return;
    }

  m_bIncrementalSearchForward = TRUE;
  m_bIncrementalSearchBackward = FALSE;
  m_bIncrementalFound = TRUE;
  OnEditFindIncremental();
}

void CCrystalTextView::OnEditFindIncrementalBackward()
{
  if( !m_bIncrementalSearchForward && !m_bIncrementalSearchBackward )
    {
      // initialize
      if( !m_pstrIncrementalSearchString->IsEmpty() )
        *m_pstrIncrementalSearchStringOld = *m_pstrIncrementalSearchString;
      m_pstrIncrementalSearchString->Empty();
      GetSelection( m_selStartBeforeIncrementalSearch, m_selEndBeforeIncrementalSearch );
      m_incrementalSearchStartPos = m_cursorPosBeforeIncrementalSearch = m_ptCursorPos;
    }
  else if( m_bIncrementalSearchBackward )
    {
      if( m_pstrIncrementalSearchString->IsEmpty() )
        {
          *m_pstrIncrementalSearchString = *m_pstrIncrementalSearchStringOld;
          m_pstrIncrementalSearchStringOld->Empty();
          OnEditFindIncremental();
        }
      else
        OnEditFindIncremental( TRUE );

      return;
    }

  m_bIncrementalSearchForward = FALSE;
  m_bIncrementalSearchBackward = TRUE;
  m_bIncrementalFound = TRUE;
  OnEditFindIncremental();
}

void CCrystalTextView::OnUpdateEditFindIncrementalForward(CCmdUI* pCmdUI)
{
  if (m_pTextBuffer)
    {
      int nLines = m_pTextBuffer->GetLineCount ();
      int nChars = m_pTextBuffer->GetLineLength (m_ptCursorPos.y);
      pCmdUI->Enable(m_ptCursorPos.y < nLines - 1 || m_ptCursorPos.x < nChars);
      return;
    }
  pCmdUI->Enable(FALSE);
}

void CCrystalTextView::OnUpdateEditFindIncrementalBackward(CCmdUI* pCmdUI)
{
  if (m_pTextBuffer)
    {
      pCmdUI->Enable(m_ptCursorPos.y > 0 || m_ptCursorPos.x > 0);
      return;
    }
  pCmdUI->Enable(FALSE);
}

void CCrystalTextView::OnUpdateStatusMessage( CStatusBar *pStatusBar )
{
  static BOOL	bUpdatedAtLastCall = FALSE;

  ASSERT( pStatusBar && IsWindow( pStatusBar->m_hWnd ) );
  if( !pStatusBar || !IsWindow( pStatusBar->m_hWnd ) )
    return;

  if( !m_bIncrementalSearchForward && !m_bIncrementalSearchBackward )
    {
      if( bUpdatedAtLastCall )
        pStatusBar->SetPaneText( 0, CString( (LPCTSTR)AFX_IDS_IDLEMESSAGE ) );

      bUpdatedAtLastCall = FALSE;

      return;
    }

  CString	strFormat;
  if( !m_bIncrementalFound )
    strFormat.Format( IDS_FIND_INCREMENTAL_FAILED, *m_pstrIncrementalSearchString );
  else if( m_bIncrementalSearchForward )
    strFormat.Format( IDS_FIND_INCREMENTAL_FORWARD, *m_pstrIncrementalSearchString );
  else if( m_bIncrementalSearchBackward )
    strFormat.Format( IDS_FIND_INCREMENTAL_BACKWARD, *m_pstrIncrementalSearchString );

  if( strFormat.IsEmpty() )
    return;

  pStatusBar->SetPaneText( 0, strFormat );
  bUpdatedAtLastCall = TRUE;
}
//END SW

BOOL CCrystalTextView::IsTextBufferInitialized () const
{
  return m_pTextBuffer && m_pTextBuffer->IsTextBufferInitialized(); 
}

CString CCrystalTextView::GetTextBufferEol(int nLine) const
{
  return m_pTextBuffer->GetLineEol(nLine); 
}

int CCrystalTextView::ComputeRealLine (int nApparentLine) const
{
  return m_pTextBuffer->ComputeRealLine(nApparentLine);
}

int CCrystalTextView::ComputeApparentLine (int nRealLine) const
{
  return m_pTextBuffer->ComputeApparentLine(nRealLine);
}
////////////////////////////////////////////////////////////////////////////
#pragma warning ( default : 4100 )
