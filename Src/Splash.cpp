/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file Src/Splash.cpp
 *
 * @brief Implementation of splashscreen class
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "resource.h"

#include "Picture.h"
#include "Splash.h"
#include "version.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/** 
 * @brief Area for version text in splash image.
 * Text is right-aligned, so reserve space to left side for longer text.
 * @note Translations may have language name after version number.
 */
static const RECT VersionTextArea = { 255, 5, 469, 20 };

/** @brief Area for developers list. */
static const RECT DevelopersArea = { 20, 88, 190, 210 };

/** @brief Area for copyright text. */
static const RECT CopyrightArea = { 20, 210, 190, 330 };

/** @brief ID for the timer closing splash screen. */
static const UINT_PTR SplashTimerID = 1;

/////////////////////////////////////////////////////////////////////////////
//   Splash Screen class

BOOL CSplashWnd::c_bShowSplashWnd;
CSplashWnd* CSplashWnd::c_pSplashWnd;

/** 
 * @brief Default constructor.
 */
CSplashWnd::CSplashWnd()
 : m_pPicture(NULL)
{
}

/** 
 * @brief Default destructor.
 */
CSplashWnd::~CSplashWnd()
{
	// Clear the static window pointer.
	ASSERT(c_pSplashWnd == this);
	c_pSplashWnd = NULL;
}

BEGIN_MESSAGE_MAP(CSplashWnd, CWnd)
	//{{AFX_MSG_MAP(CSplashWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Enables/disables splashscreen.
 */
void CSplashWnd::EnableSplashScreen(BOOL bEnable /*= TRUE*/)
{
	c_bShowSplashWnd = bEnable;
}

/** 
 * @brief Shows splashscreen.
 */
void CSplashWnd::ShowSplashScreen(CWnd* pParentWnd /*= NULL*/)
{
	if (!c_bShowSplashWnd || c_pSplashWnd != NULL)
		return;

	// Allocate a new splash screen, and create the window.
	c_pSplashWnd = new CSplashWnd;
	if (!c_pSplashWnd->Create(pParentWnd))
		delete c_pSplashWnd;
	else
		c_pSplashWnd->UpdateWindow();
}

/** 
 * @brief Hide splashscreen after user presses any key.
 */
BOOL CSplashWnd::PreTranslateAppMessage(MSG* pMsg)
{
	if (c_pSplashWnd == NULL)
		return FALSE;

	// If we get a keyboard or mouse message, hide the splash screen.
	if (pMsg->message == WM_KEYDOWN ||
	    pMsg->message == WM_SYSKEYDOWN ||
	    pMsg->message == WM_LBUTTONDOWN ||
	    pMsg->message == WM_RBUTTONDOWN ||
	    pMsg->message == WM_MBUTTONDOWN ||
	    pMsg->message == WM_NCLBUTTONDOWN ||
	    pMsg->message == WM_NCRBUTTONDOWN ||
	    pMsg->message == WM_NCMBUTTONDOWN)
	{
		c_pSplashWnd->HideSplashScreen();
		return TRUE;	// message handled here
	}

	return FALSE;	// message not handled
}

/** 
 * @brief Loads splashscreen image.
 * Loads splashscreen image from resource and creates window with same size.
 */
BOOL CSplashWnd::Create(CWnd* pParentWnd /*= NULL*/)
{
	if (m_pPicture == NULL)
		m_pPicture = new CPicture();

	if (m_pPicture == NULL)
		return FALSE;

	if (!m_pPicture->Load(IDR_SPLASH))
		return FALSE;

	CSize imgSize = m_pPicture->GetImageSize();

	return CreateEx(0,
		AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW)),
		NULL, WS_POPUP | WS_VISIBLE, 0, 0, imgSize.cx, imgSize.cy, pParentWnd->GetSafeHwnd(), NULL);
}

/** 
 * @brief Destroy the window and splash image then update the mainframe.
 */
void CSplashWnd::HideSplashScreen()
{
	KillTimer(SplashTimerID);
	m_pPicture->Free();
	delete m_pPicture;
	m_pPicture = NULL;
	DestroyWindow();
	AfxGetMainWnd()->UpdateWindow();
}

/** 
 * @brief Free the C++ class.
 */
void CSplashWnd::PostNcDestroy()
{
	delete this;
}

/** 
 * @brief Center splash screen and start timer for closing.
 */
int CSplashWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Center the window.
	CenterWindow();

	// Set a timer to destroy the splash screen.
	SetTimer(SplashTimerID, 5000, NULL);

	return 0;
}

/** 
 * @brief Paint splashscreen.
 * Draws image to window size (which is already set to image size).
 * Then adds texts over image.
 */
void CSplashWnd::OnPaint()
{
	CPaintDC dc(this);
	m_pPicture->Render(&dc);

	CVersionInfo version(TRUE);
	CString s;
	CFont versionFont;
	CFont textFont;
	CFont *oldfont = NULL;

	int fontHeight = -MulDiv(9, dc.GetDeviceCaps(LOGPIXELSY), 72);
	BOOL fontSuccess = versionFont.CreateFont(fontHeight, 0, 0, 0, FW_BOLD, FALSE, FALSE,
		0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));
	if (fontSuccess)
		oldfont = dc.SelectObject(&versionFont);

	CString sVersion = version.GetFixedProductVersion();
	LangFormatString1(s, IDS_VERSION_FMT, sVersion);
	dc.SetBkMode(TRANSPARENT);
	
	RECT area = VersionTextArea;
	dc.DrawText(s, &area, DT_RIGHT | DT_TOP);

	fontSuccess = textFont.CreateFont(fontHeight, 0, 0, 0, 0, FALSE, FALSE,
		0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));
	if (fontSuccess)
	{
		// We already stored oldfont in previous font change
		if (oldfont == NULL)
			oldfont = dc.SelectObject(&textFont);
		else
			dc.SelectObject(&textFont);
	}

	String text = LoadResString(IDS_SPLASH_DEVELOPERS);
	// Turn spaces between parts of names into non-breaking spaces

	// avoid dereference of empty strings and the NULL termiated character
	if (text.length() >= 0)
	{
		for (TCHAR *p = &*(text.end() - 1) ; p > &*text.begin() && *p != '\n' ; --p)
		{
			if (*p == _T('\x20') && p[-1] != _T(','))
				*p = _T('\xA0');
		}
	}

	area = DevelopersArea;
	dc.DrawText(text.c_str(), &area, DT_NOPREFIX | DT_TOP | DT_WORDBREAK);
	text = LoadResString(IDS_SPLASH_GPLTEXT);
	area = CopyrightArea;
	dc.DrawText(text.c_str(), &area, DT_NOPREFIX | DT_TOP | DT_WORDBREAK);

	if (oldfont != NULL)
		dc.SelectObject(oldfont);
}

/** 
 * @brief Hide splashscreen after specified time.
 */
void CSplashWnd::OnTimer(UINT_PTR nIDEvent)
{
	// Destroy the splash screen window.
	HideSplashScreen();
}
