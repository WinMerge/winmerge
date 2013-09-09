/*
Frhed - Free hex editor
Copyright (C) 2000 Raihan Kibria

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License, version 2, as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.

Last change: 2013-04-10 by Jochen Neubeck
*/
/** 
 * @file  hexwnd.cpp
 *
 * @brief Implementation of the hex window.
 *
 */
//============================================================================================
// Frhed main definition file.
#include "precomp.h"
#include "resource.h"
#include "Constants.h"
#include "AnsiConvert.h"
#include "Simparr.h"
#include "StringTable.h"
#include "VersionData.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "shtools.h"
#include "gktools.h"
#include "OSTools.h"
#include "hgstream.h"
#include "InvokeHtmlHelp.h"
#include "clipboard.h"
#include "BinTrans.h"
#include "LangArray.h"
#include "Registry.h"
#include "Template.h"
#include "HexDump.h"
#include "LoadHexFile.h"
#include "FindUtil.h"
#include "FindCtxt.h"

#include "idt.h"
#include "ids.h"
#include "ido.h"

const TCHAR DefaultHexdumpFile[] = _T("hexdump.txt");
const int DefaultOffsetLength = 6;
const int DefaultBPL = 16;
const int LineSpaceAbove = 1;
const int LineSpaceBelow = 1;
const int LineAdditionalSpace = LineSpaceAbove + LineSpaceBelow;

//CF_RTF defined in Richedit.h, but we don't include it cause that would be overkill
#ifndef CF_RTF
	#define CF_RTF TEXT("Rich Text Format")
#endif

const CLIPFORMAT CF_BINARYDATA = (CLIPFORMAT)RegisterClipboardFormat(_T("BinaryData"));
const CLIPFORMAT CF_RICH_TEXT_FORMAT = (CLIPFORMAT)RegisterClipboardFormat(CF_RTF);


int HexEditorWindow::MouseOpDist = GetProfileInt(_T("Windows"), _T("DragMinDist"), DD_DEFDRAGMINDIST);
int HexEditorWindow::MouseOpDelay = GetProfileInt(_T("Windows"), _T("DragDelay"), DD_DEFDRAGDELAY);
//We use the size of the font instead
//int HexEditorWindow::ScrollInset = GetProfileInt("Windows", "DragScrollInset", DD_DEFSCROLLINSET);
int HexEditorWindow::ScrollDelay = GetProfileInt(_T("Windows"), _T("DragScrollDelay"), DD_DEFSCROLLDELAY);
int HexEditorWindow::ScrollInterval = GetProfileInt(_T("Windows"), _T("DragScrollInterval"), DD_DEFSCROLLINTERVAL);

bool HexEditorWindow::bPasteAsText = false;
int HexEditorWindow::iPasteTimes = 1;
int HexEditorWindow::iPasteSkip = 0;

TCHAR HexEditorWindow::TexteditorName[MAX_PATH] = _T("NOTEPAD.EXE");

//Temporary stuff for CMD_move_copy
int iMovePos;
OPTYP iMoveOpTyp;

/**
 * @brief Constructor.
 */
HexEditorWindow::HexEditorWindow()
{
	Drive = 0;
	SelectedPartitionInfo = 0;
	CurrentSectorNumber = 0;
//Pabs inserted
	bSaveIni = FALSE;
	bCenterCaret = FALSE;
	bMakeBackups = 0;

	enable_drop = TRUE;
	enable_drag = TRUE;
	enable_scroll_delay_dd = TRUE;
	enable_scroll_delay_sel = FALSE;
	always_pick_move_copy = FALSE;
	prefer_CF_HDROP = TRUE; // pabs: make the default 2 accept files FALSE;
	prefer_CF_BINARYDATA = TRUE;
	prefer_CF_TEXT = FALSE;
	output_CF_BINARYDATA = TRUE;
	output_CF_TEXT = TRUE;
	output_text_special = FALSE;
	output_text_hexdump_display = TRUE;
	output_CF_RTF = FALSE;
//end

	iWindowX = CW_USEDEFAULT;
	iWindowY = CW_USEDEFAULT;
	iWindowWidth = CW_USEDEFAULT;
	iWindowHeight = CW_USEDEFAULT;
	iWindowShowCmd = SW_SHOW;

	iBmkColor = RGB(255, 0, 0);
	iSelBkColorValue = RGB(255, 255, 0);
	iSelTextColorValue = RGB(0, 0, 0);

	bOpenReadOnly = bReadOnly = FALSE;
	iPartialOffset = 0;
	bPartialStats = false;
	bPartialOpen = false;
	iBmkCount = 0;

	iMRU_count = MRUMAX;
	while (iMRU_count)
		_stprintf(strMRU[--iMRU_count], _T("dummy%d"), iMRU_count);

	bFilestatusChanged = true;
	iBinaryMode = ENDIAN_LITTLE;
	bUnsignedView = true;
	iFontSize = 10;
	iFontZoom = 0;
	bInsertMode = false;
	iTextColorValue = RGB(0,0,0);
	iBkColorValue = RGB(255,255,255);
	iSepColorValue = RGB(192,192,192);
	iDiffBkColorValue = RGB(239,203,5);
	iDiffTextColorValue = RGB(0,0,0);
	iSelDiffBkColorValue = RGB(239,119,116);
	iSelDiffTextColorValue = RGB(0,0,0);
	sibling = this;
	iAutomaticBPL = 1;
	bSelected = false;
	bSelecting = false;
	dragging = false;
	iStartOfSelection = 0;
	iEndOfSelection = 0;
	pwnd = 0;
	hwndMain = 0;
	pwndToolBar = 0;
	pwndStatusBar = 0;
	hAccel = 0;
	iMinOffsetLen = iMaxOffsetLen = DefaultOffsetLength; // Max is same as min because there is no data
	bAutoOffsetLen = TRUE;
	iByteSpace = 2;
	iBytesPerLine = DefaultBPL;
	iCharSpace = 1;
	iEnteringMode = BYTES;
	iFileChanged = FALSE;
	bFileNeverSaved = true;

	iCharacterSet = ANSI_FIXED_FONT;

	bSelected = false;
	iVscrollMax = 0;
	iVscrollPos = 0;
	iHscrollMax = 0;
	iHscrollPos = 0;

	iCurByte = 0;
	m_dataArray.ClearAll();
	m_dataArray.SetGrowBy(100);
	_tcscpy(filename, GetLangString(IDS_UNTITLED));
	area = AREA_NONE;

	m_pFindCtxt = new FindCtxt();
}

/**
 * Destructor.
 * Release resources and free memory before the app closes.
 */
HexEditorWindow::~HexEditorWindow()
{
	if (pFont)
		pFont->DeleteObject();
	delete Drive;

	delete m_pFindCtxt;
}

//--------------------------------------------------------------------------------------------
unsigned HexEditorWindow::get_interface_version()
{
	return HEKSEDIT_INTERFACE_VERSION;
}

//--------------------------------------------------------------------------------------------
BYTE *HexEditorWindow::get_buffer(int len)
{
	if (!m_dataArray.SetSize(len))
		return 0;
	m_dataArray.ExpandToSize();
	return m_dataArray;
}

//--------------------------------------------------------------------------------------------
int HexEditorWindow::get_length()
{
	return m_dataArray.GetLength();
}

void HexEditorWindow::set_sibling(IHexEditorWindow *p)
{
	sibling = p;
}

HexEditorWindow::Colors *HexEditorWindow::get_colors()
{
	return this;
}

HexEditorWindow::Settings *HexEditorWindow::get_settings()
{
	return this;
}

HexEditorWindow::Status *HexEditorWindow::get_status()
{
	return this;
}

int HexEditorWindow::translate_accelerator(MSG *pMsg)
{
	return TranslateAccelerator(pwnd->m_hWnd, hAccel, pMsg);
}

static LPWSTR NTAPI LoadStringResource(HMODULE hModule, UINT uStringID)
{
	LPWSTR pwchMem = 0;
	HRSRC hResource = FindResourceEx(
		hModule, RT_STRING,
		MAKEINTRESOURCE(uStringID / 16 + 1),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT));
	if (hResource)
	{
		pwchMem = (LPWSTR)LoadResource(hModule, hResource);
		if (pwchMem)
		{
			while (uStringID & 15)
			{
				pwchMem += *pwchMem + 1;
				--uStringID;
			}
		}
	}
	return pwchMem;
}

static LPTSTR NTAPI LoadResString(UINT uStringID)
{
	HINSTANCE hinst = langArray.m_hLangDll ? langArray.m_hLangDll : hMainInstance;
	LPTSTR text = 0;
	if (LPWSTR p = LoadStringResource(hinst, uStringID))
	{
		UINT n = (WORD)*p++;
#ifdef UNICODE
		text = SysAllocStringLen(p, n);
#else
		UINT cb = WideCharToMultiByte(CP_ACP, 0, p, n, 0, 0, 0, 0);
		text = (PSTR)SysAllocStringByteLen(0, cb);
		WideCharToMultiByte(CP_ACP, 0, p, n, text, cb, 0, 0);
#endif
		if (langArray.m_hLangDll)
		{
			int line = 0;
			if (LPTSTR p = _tcschr(text, _T(':')))
				line = _ttoi(p + 1);
			SysFreeString((BSTR)text);
			text = langArray.TranslateString(line);
		}
	}
	return text;
}

void HexEditorWindow::LoadStringTable()
{
	for (int i = 0 ; i < RTL_NUMBER_OF(S) ; ++i)
		S[i] = LoadResString(IDS[i]);
}

void HexEditorWindow::FreeStringTable()
{
	for (int i = 0 ; i < RTL_NUMBER_OF(S) ; ++i)
		SysFreeString((BSTR)S[i]);

}

static void TrimFileDialogFilters(LPTSTR p)
{
	if (p)
	{
		while (*p)
		{
			if (*p == _T('|'))
				*p = _T('\0');
			++p;
		}
	}
}

/**
 * @brief Load translations for given language if present.
 */
BOOL HexEditorWindow::load_lang(LANGID langid, LPCWSTR langdir)
{
	if (langdir == NULL)
		langdir = LangFileSubFolder;
	FreeStringTable();
	BOOL bDone = langArray.Load(hMainInstance, langid, static_cast<W2T>(langdir));
	LoadStringTable();
	TrimFileDialogFilters(GetLangString(IDS_OPEN_ALL_FILES));
	TrimFileDialogFilters(GetLangString(IDS_HDUMP_FILE_PATTERN));
	if (hwndMain)
	{
		if (HMENU hMenu = load_menu(IDR_MAINMENU))
		{
			HMENU hMenuDestroy = ::GetMenu(hwndMain);
			if (::SetMenu(hwndMain, hMenu))
				::DestroyMenu(hMenuDestroy);
		}
	}
	return bDone;
}

/**
 * @brief Load a string.
 */
LPTSTR HexEditorWindow::load_string(UINT uStringID)
{
	return LoadResString(uStringID);
}

/**
 * @brief Free a string obtained through load_string().
 */
void HexEditorWindow::free_string(LPTSTR text)
{
	SysFreeString((BSTR)text);
}

/**
 * @brief Load a menu.
 */
HMENU HexEditorWindow::load_menu(UINT id)
{
	return langArray.LoadMenu(hMainInstance, MAKEINTRESOURCE(id));
}

/**
 * @brief Load a file.
 * @param [in] fname Name of file to load.
 * @return TRUE if the file was loaded succesfully, FALSE otherwise.
 */
int HexEditorWindow::load_file(LPCTSTR fname)
{
	WaitCursor wc;
	int bLoaded = FALSE;
	int filehandle = _topen(fname, _O_RDONLY|_O_BINARY);
	if (filehandle != -1)
	{
		int filelen = _filelength(filehandle);
		m_dataArray.ClearAll();
		// Try to allocate memory for the file.
		if (m_dataArray.SetSize(filelen))
		{
			m_dataArray.ExpandToSize();
			// If read-only mode on opening is enabled or the file is read only:
			bReadOnly = bOpenReadOnly || -1 == _taccess(fname, 02); //Pabs added call to _access
			// Load the file.
			if (filelen == 0 || _read(filehandle, m_dataArray, filelen) != -1)
			{
				// This is an empty file. Don't need to read anything.
				GetLongPathNameWin32(fname, filename);
				bLoaded = TRUE;
			}
			else
			{
				MessageBox(pwnd, GetLangString(IDS_ERR_READING_FILE), MB_ICONERROR);
			}
		}
		else
		{
			MessageBox(pwnd, GetLangString(IDS_NO_MEM_FOR_FILE), MB_ICONERROR);
		}
		_close(filehandle);
	}
	else
	{
		TCHAR buf[500];
		_stprintf(buf, GetLangString(IDS_ERR_FILE_OPEN_CODE), errno, fname);
		MessageBox(pwnd, buf, MB_ICONERROR);
	}
	if (bLoaded)
	{
		bFileNeverSaved = false;
		bPartialStats = false;
		bPartialOpen = false;
		// Update MRU list.
		update_MRU();
		bFilestatusChanged = true;
		iVscrollMax = 0;
		iVscrollPos = 0;
		iHscrollMax = 0;
		iHscrollPos = 0;
		iCurByte = 0;
		iCurNibble = 0;
		bSelected = false;
		iFileChanged = FALSE;
	}
	resize_window();
	return bLoaded;
}

/**
 * @brief Open a file specified on the command line.
 * @note Function takes a wide string even in ANSI build.
 */
int HexEditorWindow::open_file(LPCWSTR wszCmdLine)
{
	W2T szCmdLine = wszCmdLine;
	int bLoaded = FALSE;
	TCHAR lpszPath[MAX_PATH] = {0};
	HRESULT hres = ResolveIt(szCmdLine, lpszPath);
	if (SUCCEEDED(hres) && _tcslen(lpszPath) && _tcscmp(szCmdLine, lpszPath) != 0)
	{
		// Trying to open a link file: decision by user required.
		int ret = MessageBox(pwnd, GetLangString(IDS_OPEN_SHORTCUT),
				MB_YESNOCANCEL | MB_ICONQUESTION);
		switch (ret)
		{
		case IDYES:
			bLoaded = load_file(lpszPath);
			break;
		case IDNO:
			bLoaded = load_file(szCmdLine);
			break;
		case IDCANCEL:
			break;
		}
	}
	else
	{
		bLoaded = load_file(szCmdLine);
	}
	return bLoaded;
}

//--------------------------------------------------------------------------------------------
int HexEditorWindow::file_is_loadable(LPCTSTR fname)
{
	int filehandle = _topen(fname, _O_RDONLY | _O_BINARY);
	if (filehandle == -1)
		return FALSE;
	_close(filehandle);
	return TRUE;
}

//--------------------------------------------------------------------------------------------
int HexEditorWindow::at_window_create(HWND hw, HINSTANCE hI)
{
	pwnd = reinterpret_cast<HWindow *>(hw);
	hInstance = hI;
	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

	iVscrollMax = 0;
	iVscrollPos = 0;
	iHscrollMax = 0;
	iHscrollPos = 0;

	iCurByte = 0;
	iCurNibble = 0;

	target = new CDropTarget(*this);
	if (target)
	{
		CoLockObjectExternal(target, TRUE, FALSE);
		if (enable_drop)
			RegisterDragDrop(pwnd->m_hWnd, target);
	}
	return TRUE;
}

//-------------------------------------------------------------------
// Set member for passed-in control bar handle.
void HexEditorWindow::set_control_bar(HWindow *pwndBar)
{
	if (pwndBar == pwnd->FindWindowEx(NULL, _T("ToolbarWindow32")))
	{
		pwndToolBar = static_cast<HToolBar *>(pwndBar);
	}
	else if (pwndBar == pwnd->FindWindowEx(NULL, _T("msctls_statusbar32")))
	{
		pwndStatusBar = static_cast<HStatusBar *>(pwndBar);
	}
	pwndBar->SendMessage(CCM_SETUNICODEFORMAT, sizeof(TCHAR) == sizeof(WCHAR), 0);
}

/**
 * @brief Determine maximum length of the offset in the buffer.
 * @param [in] size Size of the buffer.
 * @return Max length (in chars) of the offset.
 */
static int DetermineMaxOffsetLen(INT64 size)
{
	int maxLen = 1;
	while (size & ~(INT64)0 << 4 * maxLen)
		++maxLen;
	return maxLen;
}

/**
 * @brief Re-calculate draw settings after window has been resized.
 * This function re-calculates offset length, bytes per line etc after the
 * window has been resized.
 */
void HexEditorWindow::resize_window()
{
	RECT rcClient;
	pwnd->GetClientRect(&rcClient);
	// Get font data.
	HSurface *pdc = pwnd->GetDC();
	make_font();
	HGdiObj *of = pdc->SelectObject(pFont);
	TEXTMETRIC tm;
	pdc->GetTextMetrics(&tm);
	cxChar = tm.tmAveCharWidth;
	cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2;
	// Reserve 2 pixels for things like bookmark indicators
	cyChar = tm.tmHeight + tm.tmExternalLeading + LineAdditionalSpace;
	pdc->SelectObject(of);
	pwnd->ReleaseDC(pdc);

	int length = get_length();
	INT64 x = length; // Value of the last offset
	if (bPartialStats)
		x += iPartialOffset;

	cxClient = rcClient.right;
	cyClient = rcClient.bottom;
	cxBuffer = max(1, cxClient / cxChar);
	cyBuffer = max(1, cyClient / cyChar);

	iMaxOffsetLen = DetermineMaxOffsetLen(x);
	if (bAutoOffsetLen)
		iMinOffsetLen = iMaxOffsetLen;
	else if (iMaxOffsetLen < iMinOffsetLen)
		iMaxOffsetLen = iMinOffsetLen;

	// Adjust bytes per line to width of window.
	// cxBuffer = maximal width of client-area in chars.
	if (iAutomaticBPL)
	{
		int bytemax = cxBuffer - iMaxOffsetLen - iByteSpace - iCharSpace;
		iBytesPerLine = bytemax / 4;
		if (iBytesPerLine < 1)
			iBytesPerLine = 1;
	}

	// Caret or end of selection will be vertically centered if line not visible.
	if (bCenterCaret)
	{
		int iCenter = (bSelected ? iEndOfSelection : iCurByte) / iBytesPerLine;
		if (iCenter < iVscrollPos || iCenter > iVscrollPos + cyBuffer)
			iVscrollPos = max(0, iCenter - cyBuffer / 2);
	}
//Pabs removed - scrollbar fix

//Pabs replaced "iOffsetLen" with "iMaxOffsetLen"
	int iCharsPerLine = iGetCharsPerLine(); //iMaxOffsetLen + iByteSpace + iBytesPerLine*3 + iCharSpace + iBytesPerLine;
//end

	// Get number of lines to display.
	iNumlines = (length + iBytesPerLine) / iBytesPerLine;

	int sibling_length = sibling->get_length();
	if (length < sibling_length)
		length = sibling_length;

	iVscrollMax = (length + iBytesPerLine) / iBytesPerLine;

//Pabs inserted "ffff" after each 0xffff - 32bit scrolling
	if (iVscrollMax <= 0xffffffff)
		iVscrollMax = iVscrollMax - 1;
	else
		iVscrollMax = 0xffffffff;
//end
//Pabs reorganised this bit to fix the scrollbars
	SCROLLINFO SI;
	SI.cbSize = sizeof SI;
	SI.fMask = SIF_RANGE | SIF_POS | SIF_DISABLENOSCROLL | SIF_PAGE;
	if (iVscrollPos > iVscrollMax - cyBuffer + 1)
		iVscrollPos = iVscrollMax - cyBuffer + 1;
	if (iVscrollPos < 0)
		iVscrollPos = 0;
	SI.nPage = cyBuffer;
	SI.nPos = iVscrollPos;
	SI.nMin = 0;
	SI.nMax = iVscrollMax;
	pwnd->SetScrollInfo(SB_VERT, &SI, TRUE);

	iHscrollMax = iCharsPerLine - 1;
	if (iHscrollPos > iHscrollMax - cxBuffer + 1)
		iHscrollPos = iHscrollMax - cxBuffer + 1;
	if (iHscrollPos < 0)
		iHscrollPos = 0;
	SI.nPage = cxBuffer;
	SI.nPos = iHscrollPos;
	SI.nMax = iHscrollMax;
	pwnd->SetScrollInfo(SB_HORZ, &SI, TRUE);

	set_wnd_title();
	repaint();

	if (pwnd == HWindow::GetFocus() && sibling != this)
	{
		sibling->resize_window();
	}
}

/**
 * @brief Set window focus.
 * This function shows caret when window gets focus.
 */
void HexEditorWindow::set_focus()
{
	pwnd->CreateCaret(NULL, cxChar, cyChar - LineAdditionalSpace);
	set_caret_pos();
	pwnd->ShowCaret();
	mark_char(0);
}

//--------------------------------------------------------------------------------------------
// What to do when focus is lost.
void HexEditorWindow::kill_focus()
{
	pwnd->HideCaret();
	DestroyCaret();
	mark_char(0);
	if (sibling != this)
	{
		Status *sibling_status = sibling->get_status();
		int sibling_length = sibling->get_length();
		sibling_status->iEnteringMode = iEnteringMode;
		sibling_status->iCurByte = iCurByte < sibling_length ? iCurByte : sibling_length;
		sibling_status->iCurNibble = iCurNibble;
	}
}

void HexEditorWindow::snap_caret()
{
	int col = iMaxOffsetLen + iByteSpace;
	if (bSelecting ? area == AREA_BYTES : iEnteringMode == BYTES)
		col += (iCurByte % iBytesPerLine) * 3 + iCurNibble;
	else
		col += iBytesPerLine * 3 + iCharSpace + iCurByte % iBytesPerLine;

	if (col >= iHscrollPos + cxBuffer)
	{
		iHscrollPos = col - (cxBuffer - 1);
		adjust_hscrollbar();
	}
	else if (col < iHscrollPos)
	{
		iHscrollPos = col;
		adjust_hscrollbar();
	}

	int row = iCurByte / iBytesPerLine;
	if (row < iVscrollPos)
	{
		iVscrollPos = row;
		adjust_vscrollbar();
	}
	else if (row >= iVscrollPos + cyBuffer)
	{
		iVscrollPos = row - (cyBuffer - 1);
		if (iVscrollPos < 0)
			iVscrollPos = 0;
		adjust_vscrollbar();
	}
}

//--------------------------------------------------------------------------------------------
// Handler for non-character keys (arrow keys, page up/down etc.)
void HexEditorWindow::keydown(int key)
{
	//Pabs rewrote the rest of this function
	//Only handle these
	switch (key)
	{
	case VK_ESCAPE:
	case VK_LEFT: case VK_RIGHT: case VK_UP: case VK_DOWN:
	case VK_PRIOR: case VK_NEXT: case VK_HOME: case VK_END:
		break;
	default:
		return;
	}

	const bool shiftDown = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
	const bool ctrlDown = (GetAsyncKeyState(VK_CONTROL) &0x8000) != 0;

	int *a = NULL;//Data to update
	int b = 0;//How much to update it by
	int c =  0;//The original value
	const bool sel = bSelected;

	if (shiftDown)
	{
		a = &iEndOfSelection;
		if (!bSelected)
		{
			iStartOfSelection = iEndOfSelection = c = iCurByte;
			bSelected = true;
		}
		else
		{
			c = iEndOfSelection;
		}
	}
	if (!shiftDown || ctrlDown || key == VK_ESCAPE)
	{
		if (!bSelecting)
		{
			c = iCurByte;
			if (bSelected)
			{
				int iStartOfSelSetting = iGetStartOfSelection();
				int iEndOfSelSetting = iGetEndOfSelection();

				switch (key)
				{
				case VK_ESCAPE:
					if (dragging)
						return;
					pwnd->ShowCaret();
					// fall through
				case VK_LEFT:
				case VK_UP:
				case VK_PRIOR:
				case VK_HOME:
					iCurByte = iStartOfSelSetting;
					c = iEndOfSelSetting;
					iCurNibble = 0;
					break;
				case VK_RIGHT:
				case VK_DOWN:
				case VK_NEXT:
				case VK_END:
					iCurByte = iEndOfSelSetting;
					c = iStartOfSelSetting;
					iCurNibble = 1;
					break;
				}
			}
			else
			{
				iStartOfSelection = iCurByte;
			}
			if (ctrlDown && shiftDown)
			{
				a = &iEndOfSelection;
				c = iEndOfSelection;
				bSelected = true;
			}
			else
			{
				bSelected = false;
				a = &iCurByte;
			}
		}
		else
		{
			a = &iStartOfSelection;
			c = iStartOfSelection;
			if (key == VK_ESCAPE)
			{
				iStartOfSelection = iEndOfSelection = new_pos;
			}
		}
	}

	int icn = iCurNibble;

	int lastbyte = m_dataArray.GetUpperBound();
	if (!bSelected)
		lastbyte++;

	switch(key)
	{
	case VK_LEFT:
		b = iEnteringMode == CHARS || bSelected ? 1 : iCurNibble = !iCurNibble;
		break;
	case VK_RIGHT:
		b = iEnteringMode == CHARS || bSelected ? 1 : iCurNibble;
		if (iEnteringMode == BYTES)
			iCurNibble = !iCurNibble;
		break;
	case VK_UP: case VK_DOWN:
		b = iBytesPerLine;
		break;
	case VK_PRIOR: case VK_NEXT:
		b = cyBuffer * iBytesPerLine;
		break;
	}

	switch(key)
	{
	case VK_LEFT: case VK_UP:  case VK_PRIOR:
		*a -= b;
		break;
	case VK_RIGHT: case VK_DOWN: case VK_NEXT:
		*a += b;
		break;
	case VK_HOME:
		if (ctrlDown)
			*a = 0;
		else
			*a = *a / iBytesPerLine * iBytesPerLine;
		iCurNibble = 0;
		break;
	case VK_END:
		if (ctrlDown)
			*a = lastbyte;
		else
			*a = (*a / iBytesPerLine + 1) * iBytesPerLine - 1;
		iCurNibble = 1;
		break;
	}
	if (bSelected)
	{
		if (lastbyte < 0)
		{
			bSelected = false;
			iCurNibble = iCurByte = 0;
		}
		else
		{
			if (iStartOfSelection > lastbyte)
			{
				iStartOfSelection = lastbyte;
				iCurNibble = 1;
			}
			if (iStartOfSelection < 0)
			{
				iStartOfSelection = 0;
				iCurNibble = 0;
			}
			if (iEndOfSelection > lastbyte)
			{
				iEndOfSelection = lastbyte;
				iCurNibble = 1;
			}
			if (iEndOfSelection < 0)
			{
				iEndOfSelection = 0;
				iCurNibble = 0;
			}
		}
	}
	else
	{
		if (iCurByte > lastbyte)
		{
			iCurByte = lastbyte;
			iCurNibble = 1;
		}
		if (iCurByte < 0)
		{
			iCurByte = 0;
			iCurNibble = 0;
		}
	}
	// Repaint from line c to line a
	if (c != *a || icn != iCurNibble || sel != bSelected)
	{
		if (bSelected)
		{
			iCurByte = *a;
			iCurNibble = 0;
		}
		snap_caret();
		repaint(*a / iBytesPerLine, c / iBytesPerLine);
	}
}

//--------------------------------------------------------------------------------------------
// Handler for character keys.
void HexEditorWindow::character(char ch)
{
	if (bSelecting)
		return;

	//This will be handled in HexEditorWindow::keydown
	if (ch == VK_ESCAPE)
		return;

	char c = (char)tolower(ch);
	// TAB switches between binary and text panes.
	if (ch == '\t')
	{
		iEnteringMode ^= CHARS ^ BYTES;
		if (bSelected)
			adjust_view_for_selection();
		else
			adjust_view_for_caret();
		repaint();
		return;
	}

	// If read-only mode, return.
	if (bReadOnly)
		return;

	// If in bytes and char is not a hex digit, return.
	if (iEnteringMode == BYTES)
	{
		if (c >= 'a' && c <= 'f')
			c -= 'a' - '\n';
		else if (c >= '0' && c <= '9')
			c -= '0';
		else return;
	}
	// If there is selection replace.
	if (bSelected)
	{
		iCurByte = iStartOfSelection;
		int iEndByte = iEndOfSelection;
		if (iCurByte > iEndByte)
			swap(iCurByte, iEndByte);
		m_dataArray.RemoveAt(iCurByte + 1, iEndByte - iCurByte);//Remove extraneous data
		m_dataArray[iCurByte] = 0;//Will overwrite below
		bSelected = false; // Deselect
		resize_window();//Redraw stuff
	}
	else if (bInsertMode && iCurNibble == 0 || iCurByte == m_dataArray.GetLength())
	{
		// caret at EOF
		if (!m_dataArray.InsertAtGrow(iCurByte, 0, 1))
		{
			MessageBox(pwnd, _T("Not enough memory for inserting character."), MB_ICONERROR);
			return;
		}
		iCurNibble = 0;
		resize_window();
	}
	int iByteLine = iCurByte / iBytesPerLine;
	if (iEnteringMode == BYTES) // Byte-mode
	{
		if (iCurNibble == 0)
		{
			m_dataArray[iCurByte] = (BYTE)((m_dataArray[iCurByte] & 0x0f) | (c << 4));
			iCurNibble = 1;
		}
		else
		{
			m_dataArray[iCurByte] = (BYTE)((m_dataArray[iCurByte] & 0xf0) | c);
			++iCurByte;
			iCurNibble = 0;
		}
	}
	else // Char-mode.
	{
		switch (iCharacterSet)
		{
		case ANSI_FIXED_FONT:
			m_dataArray[iCurByte] = ch;
			break;
		case OEM_FIXED_FONT:
			CharToOemBuffA(&ch, (char *)&m_dataArray[iCurByte], 1);
			break;
		}
		++iCurByte;
	}
	iFileChanged = TRUE;
	bFilestatusChanged = true;
	snap_caret();
	repaint(iByteLine, iCurByte / iBytesPerLine);
}

void HexEditorWindow::scroll_window(int dx, int dy)
{
	pwnd->ScrollWindow(dx * cxChar, dy * cyChar, 0, 0);
	if (pwnd == HWindow::GetFocus())
		set_caret_pos();
}

//--------------------------------------------------------------------------------------------
// Handler for vertical scrollbar.
void HexEditorWindow::vscroll(int cmd)
{
	SCROLLINFO SI;
	SI.cbSize = sizeof SI;
	SI.fMask = SIF_POS | SIF_TRACKPOS;
	pwnd->GetScrollInfo(SB_VERT, &SI);
	SI.nPos = iVscrollPos;
	switch (cmd)
	{
	case SB_TOP:
		iVscrollPos = 0;
		break;
	case SB_BOTTOM:
		iVscrollPos = iNumlines - 1;
		break;
	case SB_LINEUP:
		if (iVscrollPos > 0)
			iVscrollPos -= 1;
		break;
	case SB_LINEDOWN:
		if (iVscrollPos < iNumlines)
			iVscrollPos += 1;
		break;
	case SB_PAGEUP:
		if (iVscrollPos >= cyBuffer)
			iVscrollPos -= cyBuffer;
		else
			iVscrollPos = 0;
		break;
	case SB_PAGEDOWN:
		if (iVscrollPos <= iNumlines - cyBuffer)
			iVscrollPos += cyBuffer;
		else
			iVscrollPos = iNumlines;
		break;
	case SB_THUMBTRACK:
		iVscrollPos = SI.nTrackPos;
		break;
	default:
		return;//break;//Pabs put return here - don't repaint if don't need to
	}
//Pabs inserted
	if (iVscrollPos > iVscrollMax - cyBuffer + 1)
		iVscrollPos = iVscrollMax - cyBuffer + 1;
	if (iVscrollPos < 0)
		iVscrollPos = 0;
//end
	scroll_window(0, SI.nPos - iVscrollPos);
	SI.fMask = SIF_POS | SIF_DISABLENOSCROLL;
	SI.nPos = iVscrollPos;
	pwnd->SetScrollInfo(SB_VERT, &SI, TRUE);
}

//--------------------------------------------------------------------------------------------
// Handler for horizontal scrollbar.
void HexEditorWindow::hscroll(int cmd)
{
	SCROLLINFO SI;
	SI.cbSize = sizeof SI;
	SI.fMask = SIF_POS | SIF_TRACKPOS;
	pwnd->GetScrollInfo(SB_HORZ, &SI);
	SI.nPos = iHscrollPos;
	int iHscrollInc = 0;
	switch (cmd)
	{
	case SB_TOP:
		iHscrollInc = -iHscrollPos;
		break;
	case SB_BOTTOM:
		iHscrollInc = iHscrollMax - iHscrollPos;
		break;
	case SB_LINEUP:
		if (iHscrollPos > 0)
			iHscrollInc = -1;
		break;
	case SB_LINEDOWN:
		if (iHscrollPos < iHscrollMax)
			iHscrollInc = 1;
		break;
	case SB_PAGEUP:
		if (iHscrollPos >= cxBuffer)
			iHscrollInc = -cxBuffer;
		else
			iHscrollInc = -iHscrollPos;
		break;
	case SB_PAGEDOWN:
		if (iHscrollPos <= iHscrollMax-cxBuffer)
			iHscrollInc = cxBuffer;
		else
			iHscrollInc = iHscrollMax - iHscrollPos;
		break;
	case SB_THUMBTRACK:
		iHscrollInc = SI.nTrackPos - iHscrollPos;
		break;
	default:
		return;//break;//Pabs put return here - don't repaint if don't need to
	}
	iHscrollPos += iHscrollInc;
//Pabs inserted
	if (iHscrollPos > iHscrollMax - cxBuffer + 1)
		iHscrollPos = iHscrollMax - cxBuffer + 1;
	if (iHscrollPos < 0)
		iHscrollPos = 0;
//end
	scroll_window(SI.nPos - iHscrollPos, 0);
	SI.fMask = SIF_POS | SIF_DISABLENOSCROLL;
	SI.nPos = iHscrollPos;
	pwnd->SetScrollInfo(SB_HORZ, &SI, TRUE);
}

//--------------------------------------------------------------------------------------------
// WM_PAINT handler.
int HexEditorWindow::paint()
{
	PAINTSTRUCT ps;
	HSurface *pdc = pwnd->BeginPaint(&ps);
	//-------------------------------------------------------
	pwnd->HideCaret();
	// Delete remains of last position.
	int a = iVscrollPos + ps.rcPaint.top / cyChar;
	int b = iVscrollPos + ps.rcPaint.bottom / cyChar;
	if (b >= iNumlines)
		b = iNumlines - 1;
	int iBkColor = PALETTERGB(GetRValue(iBkColorValue), GetGValue(iBkColorValue), GetBValue(iBkColorValue));
	RECT rc;
	HBrush *pbr = HBrush::CreateSolidBrush(iBkColor);
	// Delete lower border if there are empty lines on screen.
	pwnd->GetClientRect(&rc);
	rc.top = (b - iVscrollPos + 1) * cyChar;
	if (rc.top < rc.bottom)
		pdc->FillRect(&rc, pbr);
	// Delete right border.
	pwnd->GetClientRect(&rc);
	rc.left = (iHscrollMax + 1 - iHscrollPos) * cxChar;
	if (rc.left < rc.right)
		pdc->FillRect(&rc, pbr);
	pbr->DeleteObject();

	// Get font.
	HGdiObj *oldfont = pdc->SelectObject(pFont);
	HPen *sep_pen = HPen::Create(PS_SOLID, 1, iSepColorValue);
	HGdiObj *oldpen = pdc->SelectObject(sep_pen);
	pbr = HBrush::CreateSolidBrush(iBmkColor);

	for (int i = a ; i <= b ; i++)
	{
		print_line(pdc, i, pbr);
		// Mark character.
		if (i == iCurByte / iBytesPerLine && !bSelected && HWindow::GetFocus() == pwnd)
			mark_char(pdc);
	}
	pbr->DeleteObject();
	pdc->SelectObject(oldpen);
	DeleteObject(sep_pen);
	pdc->SelectObject(oldfont);
	pwnd->ShowCaret();
	pwnd->EndPaint(&ps);
	return 0;
}

//--------------------------------------------------------------------------------------------
// Receives WM_COMMAND messages and passes either them to their handler functions or
// processes them here.
void HexEditorWindow::command(int cmd)
{
	if (!queryCommandEnabled(cmd))
		return;
	HRESULT hr = S_OK;
	HMENU hMenu = GetMenu(hwndMain);
	switch (cmd)
	{
//Pabs changed - line insert
//File Menu
	// What is this??
	case 0xE140:
		assert(FALSE);
		break;
	case IDM_REVERT:
		CMD_revert();
		break;
	case IDM_SAVESELAS:
		CMD_saveselas();
		break;
	case IDM_DELETEFILE:
		CMD_deletefile();
		break;
	case IDM_INSERTFILE:
		CMD_insertfile();
		break;
	case IDM_OPEN_HEXDUMP:
		CMD_open_hexdump();
		break;
//Edit Menu
	case IDM_FILL_WITH:
		CMD_fw();
		break;
	case IDM_EDIT_MOVE_COPY:
		CMD_move_copy();
		break;
	case IDM_EDIT_REVERSE:
		CMD_reverse();
		break;
//Options menu
	case IDM_MAKE_BACKUPS:
		bMakeBackups = !bMakeBackups;
		break;
	case IDM_ADOPT_COLOURS:
		CMD_adopt_colours();
		break;
	case IDM_OLE_DRAG_DROP:
		static_cast<dialog<DragDropOptionsDlg>*>(this)->DoModal(pwnd);
		break;
//Registry menu
	case IDM_SHORTCUTS:
		static_cast<dialog<ShortcutsDlg>*>(this)->DoModal(pwnd);
		break;
	case IDM_SAVEINI:
		bSaveIni = !bSaveIni;//!(MF_CHECKED==GetMenuState(hMenu,IDM_SAVEINI,0));
		break;
	case IDM_CHANGEINST: //switch instance
		static_cast<dialog<ChangeInstDlg>*>(this)->DoModal(pwnd);
		break;
	case IDM_CONTEXT:
		if (MF_CHECKED == GetMenuState(hMenu, IDM_CONTEXT, 0))
		{
			if (LONG err = RegDeleteKey(HKEY_CLASSES_ROOT, _T("*\\shell\\Open in Frhed\\command"))) //WinNT requires the key to have no subkeys
				hr = HRESULT_FROM_WIN32(err);
			if (LONG err = RegDeleteKey(HKEY_CLASSES_ROOT, _T("*\\shell\\Open in Frhed")))
				hr = HRESULT_FROM_WIN32(err);
		}
		else
		{
			hr = CreateShellCommand(_T("*\\shell\\Open in Frhed\\command"));
		}
		break;
	case IDM_UNKNOWN:
		if (MF_CHECKED == GetMenuState(hMenu, IDM_UNKNOWN, 0))
		{
			HKEY hk;
			if (LONG err = RegDeleteKey(HKEY_CLASSES_ROOT, _T("Unknown\\shell\\Open in Frhed\\command"))) //WinNT requires the key to have no subkeys
				hr = HRESULT_FROM_WIN32(err);
			if (LONG err = RegDeleteKey(HKEY_CLASSES_ROOT, _T("Unknown\\shell\\Open in Frhed")))
				hr = HRESULT_FROM_WIN32(err);
			if (LONG err = RegOpenKey(HKEY_CLASSES_ROOT, _T("Unknown\\shell"), &hk))
				hr = HRESULT_FROM_WIN32(err);
			else
			{
				if (LONG err = RegDeleteValue(hk, NULL))
					if (err != ERROR_FILE_NOT_FOUND) // No complaint for not having been the default command.
						hr = HRESULT_FROM_WIN32(err);
				RegCloseKey(hk);
			}
		}
		else
		{
			hr = CreateShellCommand(_T("Unknown\\shell\\Open in Frhed\\command"));
		}
		break;
	case IDM_DEFAULT:
		if (MF_CHECKED == GetMenuState(hMenu, IDM_DEFAULT, 0))
		{
			HKEY hk;
			if (LONG err = RegOpenKey(HKEY_CLASSES_ROOT, _T("Unknown\\shell"), &hk))
				hr = HRESULT_FROM_WIN32(err);
			else
			{
				if (LONG err = RegDeleteValue(hk, NULL))
					hr = HRESULT_FROM_WIN32(err);
				RegCloseKey(hk);
			}
		}
		else
		{
			if (LONG err = RegSetValue(HKEY_CLASSES_ROOT, _T("Unknown\\shell"), REG_SZ, _T("Open in Frhed"), 13))
				hr = HRESULT_FROM_WIN32(err);
		}
		break;
//end
	case IDM_REPLACE:
		CMD_replace();
		break;

//Pabs removed IDM_EXPLORERSETTINGS

	case IDM_OPEN_TEXT:
		CMD_summon_text_edit();
		break;

	case IDM_FINDPREV:
		CMD_findprev();
		break;

	case IDM_FINDNEXT:
		CMD_findnext();
		break;

	case IDM_BMK_COLOR:
		CMD_color_settings(&iBmkColor);
		break;

	case IDM_RESET_COLORS:
		CMD_colors_to_default();
		break;

	case IDM_EDIT_READONLYMODE:
		bReadOnly = !bReadOnly;
		set_wnd_title();
		break;

	case IDM_APPLYTEMPLATE:
		CMD_apply_template();
		break;

	case IDM_PARTIAL_OPEN:
		CMD_open_partially();
		break;

	case IDM_CLEARALL_BMK:
		CMD_clear_all_bmk();
		break;

	case IDM_REMOVE_BKM:
		CMD_remove_bkm();
		break;

	case IDM_BOOKMARK1: case IDM_BOOKMARK2: case IDM_BOOKMARK3:
	case IDM_BOOKMARK4: case IDM_BOOKMARK5: case IDM_BOOKMARK6:
	case IDM_BOOKMARK7: case IDM_BOOKMARK8: case IDM_BOOKMARK9:
		CMD_goto_bookmark(cmd - IDM_BOOKMARK1);
		break;

	case IDM_ADDBOOKMARK:
		CMD_add_bookmark();
		break;

	case IDM_MRU1: case IDM_MRU2: case IDM_MRU3: case IDM_MRU4: case IDM_MRU5:
	case IDM_MRU6: case IDM_MRU7: case IDM_MRU8: case IDM_MRU9:
		CMD_MRU_selected(cmd - IDM_MRU1);
		break;

	case IDM_SELECT_BLOCK:
		CMD_select_block();
		break;

	case IDM_BINARYMODE:
		CMD_binarymode();
		break;

	case IDM_COMPARE:
		CMD_compare();
		break;

	case IDM_READFLOAT:
		{
			TCHAR buf[500];
			TCHAR *buf2 = buf;
			buf[0] = '\0';
			union
			{
				float fval;
				double dval;
				BYTE bytes[1];
			} u;
			int iBytesAhead = m_dataArray.GetLength() - iCurByte;
			if (iBytesAhead >= sizeof u.fval)
			{
				// Space enough for float.
				memcpy(&u, &m_dataArray[iCurByte], sizeof u.fval);
				if (iBinaryMode == ENDIAN_BIG)
					reverse_bytes(u.bytes, u.bytes + sizeof u.fval - 1);
				buf2 += _stprintf(buf2, GetLangString(IDS_FLOAT_SIZE), u.fval);
			}
			else
			{
				buf2 += _stprintf(buf2, GetLangString(IDS_FLOAT_NOSPACE));
			}
			if (iBytesAhead >= sizeof u.dval)
			{
				// Space enough for double.
				memcpy(&u, &m_dataArray[iCurByte], sizeof u.dval);
				if (iBinaryMode == ENDIAN_BIG)
					reverse_bytes(u.bytes, u.bytes + sizeof u.dval - 1);
				buf2 += _stprintf(buf2, GetLangString(IDS_DOUBLE_SIZE), u.dval);
			}
			else
			{
				buf2 += _stprintf(buf2, GetLangString(IDS_DOUBLE_NOSPACE));
			}
			MessageCopyBox(pwnd, buf, MB_ICONINFORMATION);
		}
		break;

	case IDM_PROPERTIES:
		CMD_properties();
		break;

	case IDM_SELECT_ALL:
		CMD_select_all();
		break;

	case IDA_BACKSPACE:
		CMD_on_backspace();
		break;

	case IDA_INSERTMODETOGGLE:
		CMD_toggle_insertmode();
		break;

	case IDA_DELETEKEY:
		CMD_edit_clear();
		break;

	case IDM_CHARACTER_SET:
		CMD_character_set();
		break;

	case IDM_ZOOMIN:
		CMD_zoom(+1);
		break;

	case IDM_ZOOMOUT:
		CMD_zoom(-1);
		break;

	case IDM_ZOOMNORMAL:
		CMD_zoom(0);
		break;

	case IDM_EDIT_MANIPULATEBITS:
		CMD_manipulate_bits();
		break;

	case IDM_EDIT_APPEND:
		CMD_edit_append();
		break;

	case IDM_SELTEXT_COLOR:
		CMD_color_settings(&iSelTextColorValue);
		break;

	case IDM_SELBACK_COLOR:
		CMD_color_settings(&iSelBkColorValue);
		break;

	case IDM_SEP_COLOR:
		CMD_color_settings(&iSepColorValue);
		break;

	case IDM_TEXT_COLOR:
		CMD_color_settings(&iTextColorValue);
		break;

	case IDM_BK_COLOR:
		CMD_color_settings(&iBkColorValue);
		break;

	case IDM_VIEW_SETTINGS:
		CMD_view_settings();
		break;

#ifdef _DEBUG
	case IDM_INTERNALSTATUS:
		{
			PString<4000> buf;
			unsigned n = 0;
			n += buf[n].Format(_T("Data length: %d\n"), m_dataArray.GetLength());
			n += buf[n].Format(_T("Upper Bound: %d\n"), m_dataArray.GetUpperBound());
			n += buf[n].Format(_T("Data size: %d\n"), m_dataArray.GetSize());
			n += buf[n].Format(_T("cxChar: %d\n"), cxChar);
			n += buf[n].Format(_T("cyChar: %d\n"), cyChar);
			n += buf[n].Format(_T("iNumlines: %d\n"), iNumlines);
			n += buf[n].Format(_T("iVscrollPos: %d\n"), iVscrollPos);
			n += buf[n].Format(_T("iCurByte: %d\n"), iCurByte);
			n += buf[n].Format(_T("cyBuffer: %d\n"), cyBuffer);
			n += buf[n].Format(_T("cxBuffer: %d\n"), cxBuffer);
			n += buf[n].Format(_T("iMRU_count: %d\n"), iMRU_count);
			int i;
			for (i = 0 ; i < MRUMAX ; i++)
			{
				n += buf[n].Format(_T("MRU %d=%s\n"), i + 1, strMRU[i]);
			}
			MessageBox(pwnd, buf, MB_OK);
		}
		break;
#endif

	case IDM_EDIT_CUT:
		CMD_edit_cut();
		break;

	case IDM_HELP_TOPICS:
		ShowHtmlHelp(HELP_CONTENTS, 0, pwnd);
		break;

	case IDM_EDIT_ENTERDECIMALVALUE:
		CMD_edit_enterdecimalvalue();
		break;

	case IDM_COPY_HEXDUMP:
		CMD_copy_hexdump();
		break;

	case IDM_EDIT_COPY:
		CMD_edit_copy();
		break;

	case IDM_PASTE_WITH_DLG:
		CMD_edit_paste();
		break;

	case IDM_EDIT_PASTE:
		CMD_fast_paste();
		break;

	case IDM_ABOUT:
		static_cast<dialog<AboutDlg>*>(this)->DoModal(pwnd);
		break;

	case IDM_FIND:
		CMD_find ();
		break;

	case IDM_GO_TO:
		CMD_goto();
		break;

	case IDM_CHANGE_MODE:
		character('\t');
		break;

	case IDM_SAVE_AS:
		CMD_save_as();
		break;

	case IDM_SAVE:
		CMD_save();
		break;

	case IDM_SCROLL_PRIOR:
		vscroll(SB_PAGEUP);
		break;

	case IDM_SCROLL_NEXT:
		vscroll(SB_PAGEDOWN);
		break;

	case IDM_SCROLL_UP:
		vscroll(SB_LINEUP);
		break;

	case IDM_SCROLL_DOWN:
		vscroll(SB_LINEDOWN);
		break;

	case IDM_SCROLL_LEFT:
		hscroll(SB_LINELEFT);
		break;

	case IDM_SCROLL_RIGHT:
		hscroll(SB_LINERIGHT);
		break;

	case IDM_OPEN:
		CMD_open();
		break;

	case IDM_NEW:
		CMD_new();
		break;

	//GK20AUG2K
	case ID_MISC_GOTO_DLL_EXPORTS:
		CMD_GotoDllExports();
		break;
	case ID_MISC_GOTO_DLL_IMPORTS:
		CMD_GotoDllImports();
		break;
	case ID_MISC_ENCODEDECODE:
		CMD_EncodeDecode();
		break;
	case ID_DISK_OPEN_DRIVE:
		CMD_OpenDrive();
		break;
	case ID_DISK_CLOSEDRIVE:
		CMD_CloseDrive();
		break;
	case ID_DISK_GOTOFIRSTTRACK:
		CMD_DriveGotoFirstTrack();
		break;
	case ID_DISK_GOTONEXTTRACK:
		CMD_DriveGotoNextTrack();
		break;
	case ID_DISK_GOTOPREVIOUSTRACK:
		CMD_DriveGotoPrevTrack();
		break;
	case ID_DISK_GOTOLASTTRACK:
		CMD_DriveGotoLastTrack();
		break;
	case ID_DISK_GOTOTRACK:
		CMD_DriveGotoTrackNumber();
		break;

	default:
		_RPTF1(_CRT_ERROR, "Unknown COMMAND-ID %d.", cmd);
		break;
	}
	CheckHResult(pwnd, hr);
}

//--------------------------------------------------------------------------------------------
void HexEditorWindow::notify(NMHDR *pNMHDR)
{
	HWND hwndFrom = pNMHDR->hwndFrom;
	UINT code = pNMHDR->code;
	if (hwndFrom == pwndStatusBar->m_hWnd)
	{
		if (code == NM_CLICK || code == NM_RCLICK)
			status_bar_click(code == NM_CLICK);
	}
	else if (hwndFrom == pwndToolBar->m_hWnd)
	{
		if (code == TBN_GETINFOTIP)
		{
			NMTBGETINFOTIP *pNM = reinterpret_cast<NMTBGETINFOTIP *>(pNMHDR);
			if (LPTSTR text = load_string(pNM->iItem))
			{
				StrCpyN(pNM->pszText, text, pNM->cchTextMax);
				free_string(text);
			}
		}
	}
}

//--------------------------------------------------------------------------------------------
int HexEditorWindow::destroy_window()
{
	RevokeDragDrop(pwnd->m_hWnd);
	if (target)
		CoLockObjectExternal(target, FALSE, TRUE);
	return 0;
}

/**
 * @brief Format and set the window title.
 */
void HexEditorWindow::set_and_format_title()
{
	TCHAR buf[512];
	_stprintf(buf, _T("[%s"), filename);
	if (iFileChanged)
		_tcscat(buf, _T(" *"));
	_tcscat(buf, _T("]"));
	if (bPartialOpen)
		_tcscat(buf, _T(" - P"));
	_tcscat(buf, _T(" - "));
	_tcscat(buf, ApplicationName);
	SetWindowText(hwndMain, buf);
}

/**
 * @brief Format bits in byte to a string.
 * @param [in] buf Buffer where to format the string.
 * @param [in] by Byte whose bits are formatted.
 */
void HexEditorWindow::format_bit_string(TCHAR* buf, BYTE by)
{
	int i;
	for (i = 0; i < 8; i++)
		buf[i] = ((by >> i) & 0x1 ? '1' : '0');//Extract bits
	for (i = 0;i < 4; i++)
		swap(buf[i], buf[7 - i]);//flip order-for some reason it doesn't display correctly going i-- or i++ in for loop
	buf[8]='\0';//terminate string
}

/**
 * @brief Set characterset and edit mode to statusbar.
 */
void HexEditorWindow::statusbar_chset_and_editmode()
{
	TCHAR buf[20];
	buf[0] = _T('\t');
	buf[1] = _T('\0');
	// Character set, input mode or read-only, binary mode.
	switch (iCharacterSet)
	{
	case ANSI_FIXED_FONT:
		_tcscat(buf, GetLangString(IDS_FONT_ANSI));
		break;
	case OEM_FIXED_FONT:
		_tcscat(buf, GetLangString(IDS_FONT_OEM));
		break;
	}

	_tcscat(buf, _T(" / "));
	if (bReadOnly)
	{
		_tcscat(buf, GetLangString(IDS_MODE_READONLY));
	}
	else if (bInsertMode)
	{
		_tcscat(buf, GetLangString(IDS_MODE_INSERT));
	}
	else
	{
		_tcscat(buf, GetLangString(IDS_MODE_OVERWRITE));
	}
	_tcscat(buf, _T(" / "));
	if (iBinaryMode == ENDIAN_LITTLE)
	{
		_tcscat(buf, GetLangString(IDS_STATUS_LITTLE_ENDIAN)); // Intel
	}
	else if (iBinaryMode == ENDIAN_BIG)
	{
		_tcscat(buf, GetLangString(IDS_STATUS_BIG_ENDIAN)); // Motorola
	}
	pwndStatusBar->SetPartText(1, buf);
}

/**
 * @brief Set the window title and the statusbar text.
 */
void HexEditorWindow::set_wnd_title()
{
	assert(filename[0] != _T('\0'));
	TCHAR buf[512];
	// Change window title.
	if (bFilestatusChanged && hwndMain)
	{
		set_and_format_title();
		bFilestatusChanged = false;
	}
	// Selection going on.
	if (bSelected)
	{
		if (iEndOfSelection >= iStartOfSelection)
		{
			_stprintf(buf, GetLangString(IDS_SBAR_SELECTED_OFFSET),
				iStartOfSelection, iStartOfSelection,
				iEndOfSelection, iEndOfSelection, iEndOfSelection - iStartOfSelection+1);
		}
		else
		{
			_stprintf(buf, GetLangString(IDS_SBAR_SELECTED_OFFSET),
				iEndOfSelection, iEndOfSelection,
				iStartOfSelection, iStartOfSelection, iStartOfSelection-iEndOfSelection+1);
		}
		pwndStatusBar->SetPartText(0, buf);
	}
	else // Normal display.
	{
		_stprintf(buf, GetLangString(IDS_SBAR_NSEL_OFFSET), iCurByte, iCurByte);
		int wordval = 0;
		int longval = 0;
		TCHAR buf2[80];
		if (m_dataArray.GetLength() - iCurByte > 0)
		{//if we are not looking at the End byte
			_tcscat(buf, _T("   "));
			_tcscat(buf, GetLangString(IDS_SBAR_BITS));
			_tcscat(buf, _T("="));//append stuff to status text
			BYTE by = m_dataArray[iCurByte];
			format_bit_string(buf2, by);
			_tcscat(buf, buf2);//append to status text
		}
		_tcscat(buf, _T("\t"));//add that \t back on to the status text
		if (bUnsignedView) // Values signed/unsigned?
		{
			// UNSIGNED
			if (iBinaryMode == ENDIAN_LITTLE)
			{
				// UNSIGNED LITTLEENDIAN_MODE
				// Decimal value of byte.
				if (m_dataArray.GetLength() - iCurByte >= 1)
				{
					_stprintf(buf2, _T("\t%s: %s:%u"), GetLangString(IDS_SBAR_UNSIGNED),
						GetLangString(IDS_SBAR_BYTE_SHORT), (unsigned int) m_dataArray[iCurByte]);
				}
				else
					_stprintf(buf2, _T("\t%s"), GetLangString(IDS_SBAR_END));
				_tcscat(buf, buf2);

				// Space enough for a word?
				if (m_dataArray.GetLength() - iCurByte >= 2)
				{
					// Space enough for a word.
					wordval = (m_dataArray[iCurByte + 1] << 8) | m_dataArray[iCurByte];
					_stprintf(buf2, _T(",%s:%u"), GetLangString(IDS_SBAR_WORD_SHORT),
							(unsigned int) wordval);
					_tcscat(buf, buf2);
				}
				if (m_dataArray.GetLength() - iCurByte >= 4)
				{
					// Space enough for a longword.
					longval = wordval | (((m_dataArray[iCurByte + 3] << 8) |
							m_dataArray[iCurByte + 2]) << 16);
					_stprintf(buf2, _T(",%s:%u"), GetLangString(IDS_SBAR_LONG_SHORT),
							(unsigned int) longval);
					_tcscat(buf, buf2);
				}
			}
			else
			{
				// UNSIGNED BIGENDIAN_MODE
				// Decimal value of byte.
				if (m_dataArray.GetLength() - iCurByte >= 1)
					_stprintf(buf2, _T("\t%s: %s:%u"), GetLangString(IDS_SBAR_UNSIGNED),
							GetLangString(IDS_SBAR_BYTE_SHORT), (unsigned int) m_dataArray[iCurByte]);
				else
					_stprintf(buf2, _T("\t%s"), GetLangString(IDS_SBAR_END));
				_tcscat(buf, buf2);

				// Space enough for a word?
				if (m_dataArray.GetLength() - iCurByte >= 2)
				{
					// Space enough for a word.
					wordval = (m_dataArray[iCurByte] << 8) | m_dataArray[iCurByte + 1];
					_stprintf(buf2, _T(",%s:%u"), GetLangString(IDS_SBAR_WORD_SHORT),
							(unsigned int) wordval);
					_tcscat(buf, buf2);
				}
				if (m_dataArray.GetLength() - iCurByte >= 4)
				{
					// Space enough for a longword.
					longval = (wordval << 16) | (m_dataArray[iCurByte + 2] <<	8) |
							(m_dataArray[iCurByte + 3]);
					_stprintf(buf2, _T(",%s:%u"), GetLangString(IDS_SBAR_LONG_SHORT),
							(unsigned int) longval);
					_tcscat(buf, buf2);
				}
			}
		}
		else // SIGNED
		{
			if (iBinaryMode == ENDIAN_LITTLE)
			{
				// SIGNED LITTLEENDIAN_MODE
				// Decimal value of byte.
				if (m_dataArray.GetLength() - iCurByte >= 1)
					_stprintf(buf2, _T("\t%s: %s:%d"), GetLangString(IDS_SBAR_SIGNED),
							GetLangString(IDS_SBAR_BYTE_SHORT),
							(int) (signed char) m_dataArray[iCurByte]);
				else
					_stprintf(buf2, _T("\t%s"), GetLangString(IDS_SBAR_END));
				_tcscat(buf, buf2);

				// Space enough for a word?
				if (m_dataArray.GetLength() - iCurByte >= 2)
				{
					// Space enough for a word.
					wordval = (m_dataArray[iCurByte + 1] << 8) | m_dataArray[iCurByte];
					_stprintf(buf2, _T(",%s:%d"), GetLangString(IDS_SBAR_WORD_SHORT),
							(int) (signed short) wordval);
					_tcscat(buf, buf2);
				}
				if (m_dataArray.GetLength() - iCurByte >= 4)
				{
					// Space enough for a longword.
					longval = wordval | (((m_dataArray[iCurByte + 3] << 8) |
							m_dataArray[iCurByte + 2]) << 16);
					_stprintf(buf2, _T(",%s:%d"), GetLangString(IDS_SBAR_LONG_SHORT),
							(signed int) longval);
					_tcscat(buf, buf2);
				}
			}
			else
			{
				// SIGNED BIGENDIAN_MODE
				// Decimal value of byte.
				if (m_dataArray.GetLength() - iCurByte >= 1)
					_stprintf(buf2, _T("\t%s: %s:%d"), GetLangString(IDS_SBAR_SIGNED),
							GetLangString(IDS_SBAR_BYTE_SHORT),
							(signed char) m_dataArray[iCurByte]);
				else
					_stprintf(buf2, _T("\t%s"), GetLangString(IDS_SBAR_END));
				_tcscat (buf, buf2);

				// Space enough for a word.
				if (m_dataArray.GetLength() - iCurByte >= 2)
				{
					// Space enough for a longword.
					wordval = (m_dataArray[iCurByte] << 8) | m_dataArray[iCurByte + 1];
					_stprintf(buf2, _T(",%s:%d"), GetLangString(IDS_SBAR_WORD_SHORT),
							(int) (signed short) wordval);
					_tcscat(buf, buf2);
				}
				if (m_dataArray.GetLength() - iCurByte >= 4)
				{
					// Space enough for a longword.
					longval = (wordval << 16) | (m_dataArray[iCurByte + 2] << 8) |
							(m_dataArray[iCurByte + 3]);
					_stprintf(buf2, _T(",%s:%d"), GetLangString(IDS_SBAR_LONG_SHORT),
							(signed int) longval);
					_tcscat(buf, buf2);
				}
			}
		}
		pwndStatusBar->SetPartText(0, buf);
	}

	statusbar_chset_and_editmode();

	// File size.
	_stprintf(buf, _T("\t%s: %u"), GetLangString(IDS_SBAR_SIZE), m_dataArray.GetLength());
	pwndStatusBar->SetPartText(2, buf);
}

/**
 * @brief Set caret position.
 */
void HexEditorWindow::set_caret_pos()
{
	set_wnd_title();
	int iCaretByte = bSelected ? iEndOfSelection : iCurByte;
	int iCaretLine = iCaretByte / iBytesPerLine;
	int x = iMaxOffsetLen + iByteSpace - iHscrollPos;
	int y = iCaretLine - iVscrollPos;
	if (y >= 0 && y < cyBuffer && !bSelecting)
	{
		switch (iEnteringMode)
		{
		case CHARS:
			x += iBytesPerLine * 3 + iCharSpace + (iCaretByte % iBytesPerLine);
			break;
		case BYTES:
			x += (iCaretByte % iBytesPerLine) * 3 + iCurNibble;
			break;
		}
	}
	else
	{
		x = y = -1;
	}
	SetCaretPos(x * cxChar, y * cyChar + LineSpaceAbove);
}

//--------------------------------------------------------------------------------------------
// Repaints the whole window.
void HexEditorWindow::repaint(int from, int to)
{
	if (pwnd == HWindow::GetFocus())
	{
		set_caret_pos();
		if (sibling != this)
			sibling->repaint(from, to);
	}

	RECT rc;
	pwnd->GetClientRect(&rc);
	if (from > to)
		swap(from, to);
	if (from != -1)
	{
		from -= iVscrollPos;
		if (from < 0)
			from = 0;
		else if (from > cyBuffer)
			from = cyBuffer;
		rc.top = from * cyChar;
	}
	if (to != -1)
	{
		to -= iVscrollPos - 1;
		if (to < 0)
			to = 0;
		else if (to > cyBuffer)
			to = cyBuffer + 1; // cope with partialy visible line
		rc.bottom = to * cyChar;
	}
	pwnd->InvalidateRect(&rc, FALSE);
	pwnd->UpdateWindow();
}

void HexEditorWindow::repaint(int line)
{
	repaint(line, line);
}

/**
 * @brief Reset hex window properties.
 * This function resets hex window properties (e.g. offset length) to
 * default values.
 */
void HexEditorWindow::clear_all()
{
	iMaxOffsetLen = iMinOffsetLen = DefaultOffsetLength;
	iByteSpace = 2;
	iBytesPerLine = DefaultBPL;
	iCharSpace = 1;
	m_dataArray.ClearAll();
	filename[0] = '\0';
	iVscrollMax = 0;
	iVscrollPos = 0;
	iHscrollMax = 0;
	iHscrollPos = 0;
	iCurByte = 0;
	iCurNibble = 0;
}

//--------------------------------------------------------------------------------------------
// Set the vertical scrollbar position.
void HexEditorWindow::adjust_vscrollbar()
{
	SCROLLINFO SI;
	SI.cbSize = sizeof SI;
	SI.fMask = SIF_RANGE | SIF_POS | SIF_DISABLENOSCROLL;
	pwnd->GetScrollInfo(SB_VERT, &SI);
	scroll_window(0, SI.nPos - iVscrollPos);
	SI.nMin = 0;
	SI.nMax = iVscrollMax;
	SI.nPos = iVscrollPos;
	pwnd->SetScrollInfo(SB_VERT, &SI, TRUE);
}

//--------------------------------------------------------------------------------------------
// Set the horizontal scrollbar position.
void HexEditorWindow::adjust_hscrollbar()
{
	SCROLLINFO SI;
	SI.cbSize = sizeof SI;
	SI.fMask = SIF_RANGE | SIF_POS | SIF_DISABLENOSCROLL;
	pwnd->GetScrollInfo(SB_HORZ, &SI);
	scroll_window(SI.nPos - iHscrollPos, 0);
	SI.nMin = 0;
	SI.nMax = iHscrollMax;
	SI.nPos = iHscrollPos;
	pwnd->SetScrollInfo(SB_HORZ, &SI, TRUE);
}

/**
 * @brief Highlight current character / byte.
 * This function highlights character / byte at the current offset. The
 * highlighting is done by inverting the character's block area.
 * @param [in] hdc If non-NULL invert the char rect else invalidate it.
 */
void HexEditorWindow::mark_char(HSurface *pdc)
{
	RECT r;
	r.left = iMaxOffsetLen + iByteSpace - iHscrollPos;
	switch (iEnteringMode)
	{
	case CHARS:
		r.left += (iCurByte % iBytesPerLine) * 3;
		r.right = r.left + 2;
		break;
	case BYTES:
		r.left += iBytesPerLine * 3 + iCharSpace + (iCurByte % iBytesPerLine);
		r.right = r.left + 1;
		break;
	default:
		assert(FALSE);
		return;
	}
	r.top = iCurByte / iBytesPerLine - iVscrollPos;
	r.bottom = r.top + 1;
	r.left *= cxChar;
	r.right *= cxChar;
	r.top *= cyChar;
	r.bottom *= cyChar;

	if (pdc)
	{
		// Ignore extra area (for bookmarks etc) when inverting
		r.top += LineSpaceAbove;
		r.bottom -= LineSpaceBelow;
		pdc->InvertRect(&r);
	}
	else
		pwnd->InvalidateRect(&r, FALSE);
}

/**
 * @brief Print text to screen.
 * @param [in] hdc Handle to draw context (DC).
 * @param [in] x X-pos in chars where to draw.
 * @param [in] y Y-pos in chars where to draw.
 * @param [in] pch String to draw.
 * @param [in] cch Number of characters to print.
 */
void HexEditorWindow::print_text(HSurface *pdc, int x, int y, TCHAR *pch, int cch)
{
	const RECT rc = { x * cxChar, y * cyChar,
			rc.left + cch * cxChar, rc.top + cyChar };
	pdc->ExtTextOut(rc.left, rc.top + LineSpaceAbove, ETO_OPAQUE, &rc, pch, cch, 0);
}

/**
 * @brief Print one line in the editor.
 * @param [in] hdc Handle to device context (DC).
 * @param [in] line Absolute line number to print.
 * @param [in] hbr Paint brush.
 */
void HexEditorWindow::print_line(HSurface *pdc, int line, HBrush *pbr)
{
	int startpos = line * iBytesPerLine;

	// Return if this line does not even contain the end-of-file double
	// underscore (at index upperbound+1).
	assert(startpos <= m_dataArray.GetLength());

	int length = get_length();
	BYTE *buffer = get_buffer(length);
	int sibling_length = sibling->get_length();
	BYTE *sibling_buffer = sibling->get_buffer(sibling_length);

	int iBkColor = PALETTERGB (GetRValue(iBkColorValue),GetGValue(iBkColorValue),GetBValue(iBkColorValue));
	int iTextColor = PALETTERGB (GetRValue(iTextColorValue),GetGValue(iTextColorValue),GetBValue(iTextColorValue));
	int iSelBkColor = PALETTERGB (GetRValue(iSelBkColorValue),GetGValue(iSelBkColorValue),GetBValue(iSelBkColorValue));
	int iSelTextColor = PALETTERGB (GetRValue(iSelTextColorValue),GetGValue(iSelTextColorValue),GetBValue(iSelTextColorValue));
	int iDiffBkColor = PALETTERGB (GetRValue(iDiffBkColorValue),GetGValue(iDiffBkColorValue),GetBValue(iDiffBkColorValue));
	int iDiffTextColor = PALETTERGB (GetRValue(iDiffTextColorValue),GetGValue(iDiffTextColorValue),GetBValue(iDiffTextColorValue));
	int iSelDiffBkColor = PALETTERGB (GetRValue(iSelDiffBkColorValue),GetGValue(iSelDiffBkColorValue),GetBValue(iSelDiffBkColorValue));
	int iSelDiffTextColor = PALETTERGB (GetRValue(iSelDiffTextColorValue),GetGValue(iSelDiffTextColorValue),GetBValue(iSelDiffTextColorValue));

	int iSelLower = min(iStartOfSelection, iEndOfSelection);
	int iSelUpper = max(iStartOfSelection, iEndOfSelection);

	int y = line - iVscrollPos;
	int x = -iHscrollPos;

	int m = iMaxOffsetLen + iByteSpace;

	TCHAR linbuf[19]; // up to 16 hex digits, plus 2 spaces, plus null termination

	// Write offset, filling rest of linbuf with spaces.
	linbuf[RTL_NUMBER_OF(linbuf) - 1] = _T('\0');
	_sntprintf(linbuf, RTL_NUMBER_OF(linbuf) - 1, _T("%-18.*I64x"), iMinOffsetLen,
		bPartialStats ? startpos + iPartialOffset : startpos);

	pdc->SetTextColor(iTextColor);
	pdc->SetBkColor(iBkColor);
	print_text(pdc, x, y, linbuf, m);

	RECT r;

	x += m;
	int z = x + (iByteSpace + 1) * iBytesPerLine + iCharSpace;

	// Fill linebuf with spaces.
	_tcsset(linbuf, _T(' '));

	// Write offset to chars.
	m = iCharSpace + 1;
	pdc->SetTextColor(iTextColor);
	pdc->SetBkColor(iBkColor);
	print_text(pdc, z - m, y, linbuf, m);
	// Last line reached? Then only write rest of bytes.
	// startpos+iBytesPerLine-1 = Last byte in current line.
	int endpos = startpos + iBytesPerLine - 1;

	int i;
	// Write bytes.
	for (i = startpos ; i <= endpos ; i++)
	{
		TBYTE u = ' ';
		if (i < length)
		{
			u = buffer[i];
			TCHAR c = (TCHAR)(u >> 4);
			if (c < 10)
				c += '0';
			else
				c = (TCHAR)(c - 10 + 'a');
			linbuf[0] = c;
			c = (TCHAR)(u & 0x0f);
			if (c < 10)
				c += '0';
			else
				c = (TCHAR)(c - 10 + 'a');
			linbuf[1] = c;
			if (!(iCharacterSet == OEM_FIXED_FONT && u != 0 && u != '\r' && u != '\n' ||
				u >= 32 && u <= 126 ||
				u >= 160 && u <= 255 ||
				u >= 145 && u <= 146))
			{
				u = '.';
			}
		}
		else
		{
			linbuf[0] = linbuf[1] = i == length ? '_' : ' ';
		}
		bool bDiff = i < length && i < sibling_length && buffer[i] != sibling_buffer[i];
		if (bSelected && i >= iSelLower && i <= iSelUpper)
		{
			pdc->SetTextColor(bDiff ? iSelDiffTextColor : iSelTextColor);
			pdc->SetBkColor(bDiff ? iSelDiffBkColor : iSelBkColor);
		}
		else
		{
			pdc->SetTextColor(bDiff ? iDiffTextColor : iTextColor);
			pdc->SetBkColor(bDiff ? iDiffBkColor : iBkColor);
		}
		m = iByteSpace;
		if (i < endpos)
			++m;
		print_text(pdc, x, y, linbuf, m);
		x += m;
		print_text(pdc, z, y, (TCHAR *)&u, 1);
		z += 1;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Separators.
	r.top = (line - iVscrollPos) * cyChar;
	r.bottom = r.top + cyChar;
	for (i = 0 ; i <= iBytesPerLine ; i += 4)
	{
		r.left = r.right = (iMaxOffsetLen + iByteSpace - iHscrollPos + i * 3) * cxChar - cxChar / 2;
		pdc->Polyline(reinterpret_cast<POINT *>(&r), 2);
	}
	// Separator for chars.
	r.left = r.right = (iMaxOffsetLen + iByteSpace - iHscrollPos + iBytesPerLine * 3 + iCharSpace) * cxChar - cxChar / 2 - 1;
	pdc->Polyline(reinterpret_cast<POINT *>(&r), 2);
	// Second separator.
	OffsetRect(&r, 2, 0);
	pdc->Polyline(reinterpret_cast<POINT *>(&r), 2);

	PrintBookmarkIndicators(pdc, pbr, startpos);
}

/**
 * @brief Print bookmark indicators to line.
 * @param [in] hdc Handle to device context.
 * @param [in] hbr Paint brush.
 * @param [in] startpos Start position of the line.
 */
void HexEditorWindow::PrintBookmarkIndicators(HSurface *pdc, HBrush *pbr, int startpos)
{
	// Print bookmark indicators.
	// Are there bookmarks in this line?
	int length = get_length();
	int el = startpos + iBytesPerLine - 1;
	int chpos;
	// Brush for bookmark borders.
	for (int i = 0; i < iBmkCount; i++)
	{
		int offset = pbmkList[i].offset;
		// Print the bookmark if it is within the file.
		if (offset >= startpos && offset <= el && offset < length)
		{
			// Found a bookmark in this line.
			// Mark hex num.
			chpos = iMaxOffsetLen + iByteSpace + (offset % iBytesPerLine) * 3 - iHscrollPos;
			RECT r;
			r.left = chpos * cxChar;
			r.top = (offset / iBytesPerLine - iVscrollPos) * cyChar;
			r.right = r.left + 2 * cxChar;
			r.bottom = (offset / iBytesPerLine - iVscrollPos + 1) * cyChar;
			r.top--;
			r.left -= 2;
			r.right += 2;
			pdc->FrameRect(&r, pbr);

			// Mark character.
			chpos = iMaxOffsetLen + iByteSpace + iBytesPerLine * 3 + iCharSpace
				+ (offset % iBytesPerLine) - iHscrollPos;
			r.left = chpos * cxChar;
			r.top = (offset / iBytesPerLine - iVscrollPos) * cyChar;
			r.right = (chpos + 1) * cxChar;
			r.bottom = (offset / iBytesPerLine - iVscrollPos + 1) * cyChar;
			r.top--;
			pdc->FrameRect(&r, pbr);
		}
	}
}

//--------------------------------------------------------------------------------------------
// Set horizontal scroll position so that caret is visible.
void HexEditorWindow::adjust_view_for_caret()
{
	int col = iMaxOffsetLen + iByteSpace;
	if (iEnteringMode == BYTES)
		col += (iCurByte % iBytesPerLine) * 3 + iCurNibble; //BYTES_LOGICAL_COLUMN;
	else
		col += iBytesPerLine * 3 + iCharSpace + iCurByte % iBytesPerLine; //CHARS_LOGICAL_COLUMN;

//Pabs changed to put cursor in center of screen
	if ( col < iHscrollPos
	  || col >= iHscrollPos + cxBuffer )
		iHscrollPos = col - cxBuffer / 2;
	if( iHscrollPos > iHscrollMax - cxBuffer + 1 )
		iHscrollPos = iHscrollMax - cxBuffer + 1;
	if( iHscrollPos < 0 )
		iHscrollPos = 0;
	adjust_hscrollbar();
	int row = iCurByte / iBytesPerLine;
	if( row < iVscrollPos
	 || row >= iVscrollPos + cyBuffer )
		iVscrollPos = row - cyBuffer / 2;
	if( iVscrollPos > iVscrollMax - cyBuffer + 1 )
		iVscrollPos = iVscrollMax - cyBuffer + 1;
	if( iVscrollPos < 0 )
		iVscrollPos = 0;
	adjust_vscrollbar();
//end
}

/**
 * @brief Determine if menuitem will be enabled or disabled.
 * @param [in] id Resource ID of the menuitem.
 * @return TRUE if menuitem is enabled, FALSE otherwise.
 */
BOOL HexEditorWindow::queryCommandEnabled(UINT id)
{
	switch (id)
	{
	case IDM_REVERT:
		return iFileChanged;
	case IDM_DELETEFILE:
		return !bFileNeverSaved && !bReadOnly;
	case IDM_INSERTFILE:
		//"Insert file" is allowed if read-only is disabled.
	case IDM_EDIT_MOVE_COPY:
		//"Move/Copy bytes" is allowed if read-only is disabled.
	case IDM_EDIT_REVERSE:
		//"Reverse bytes" is allowed if read-only is disabled.
	case IDA_INSERTMODETOGGLE:
		// "Toggle entering mode" is allowed if read-only is disabled.
	case IDM_PASTE_WITH_DLG:
		// "Paste with dialogue" is allowed if read-only is disabled
		// All kinds of editing disabled for drives
		return !bReadOnly && Drive == 0;
	case IDM_COPY_HEXDUMP:
	case IDM_SAVESELAS:
	case IDM_SELECT_ALL:
		// "Select All" is allowed if file is not empty.
	case IDM_SELECT_BLOCK:
		// "Select block" is allowed if file is not empty.
	case IDM_FIND:
		// "Find" is allowed if the file is not empty.
	case IDM_GO_TO:
		// "Go to" is allowed if the file is not empty.
		return m_dataArray.GetLength() != 0;
	case ID_DISK_CLOSEDRIVE:
	case ID_DISK_READMFT:
	case ID_DISK_GOTOFIRSTTRACK:
	case ID_DISK_GOTONEXTTRACK:
	case ID_DISK_GOTOPREVIOUSTRACK:
	case ID_DISK_GOTOLASTTRACK:
	case ID_DISK_GOTOTRACK:
		return Drive != 0;
	case IDM_OPEN_TEXT:
		// "Open in text editor" is allowed if file has been saved before.
		return !bFileNeverSaved;
	case IDM_EDIT_CUT:
		// "Cut" is allowed if there is a selection or the caret is on a byte.
		// It is not allowed in read-only mode.
	case IDA_DELETEKEY:
		// No editing of drives
		if (Drive != 0)
			return FALSE;
		// "Delete" is allowed if there is a selection or the caret is on a byte.
		// It is not allowed in read-only mode.
		return (bSelected || iCurByte < m_dataArray.GetLength()) && !bReadOnly;
	case IDM_EDIT_COPY:
		// "Copy" is allowed if there is a selection or the caret is on a byte.
		return bSelected || iCurByte < m_dataArray.GetLength();
	case IDM_EDIT_PASTE:
		// No editing of drives
		if (Drive != 0)
			return FALSE;
		// "Paste" is allowed if the clipboard contains text,
		// there is no selection going on and read-only is disabled.
		id = 0;
		if (!bReadOnly && OpenClipboard(0))
		{
			id = EnumClipboardFormats(0);
			CloseClipboard();
		}
		return id != 0;
	case IDM_EDIT_APPEND:
		// No editing of drives
		if (Drive != 0)
			return FALSE;
		// "Append" is allowed if read-only is disabled
		// and there is no selection going on.
		return !bReadOnly && !bSelected;
	case IDM_FILL_WITH:
		//"Fill with" is allowed if the file is not empty and read-only is disabled.
		//If there is no selection the whole file is filled
	case IDM_REPLACE:
		// No editing of drives
		if (Drive != 0)
			return FALSE;
		// "Replace" is allowed if the file is not empty and read-only is disabled.
		return m_dataArray.GetLength() && !bReadOnly;
	case IDM_FINDNEXT:
	case IDM_FINDPREV:
		// "Find next" and "Find previous" are allowed if the file is not empty,
		// and there is a findstring OR there is a selection
		// (which will be searched for).
		return m_dataArray.GetLength() && (m_pFindCtxt->HasText() || bSelected);
	case IDM_EDIT_ENTERDECIMALVALUE:
		// "Enter decimal value" is allowed if read-only is disabled, the file is not empty,
		// the caret is on a byte and there is no selection going on.
	case IDM_EDIT_MANIPULATEBITS:
		// No editing of drives
		if (Drive != 0)
			return FALSE;
		// "Manipulate bits" is allowed if the caret is on a byte, read-only is disabled
		// and there is no selection going on.
		return !bReadOnly && iCurByte < m_dataArray.GetLength() && !bSelected;
	case IDM_COMPARE:
		// "Compare from current offset" is allowed if the caret is on a byte
		// and there is no selection going on.
	case IDM_READFLOAT:
		// "Get floating point value" is allowed if the caret is on a byte
		// and there is no selection going on.
	case IDM_APPLYTEMPLATE:
		// "Apply template" is allowed if the caret is on a byte
		// and there is no selection going on.
		return iCurByte < m_dataArray.GetLength() && !bSelected;
	case IDM_DEFAULT:
		return unknownpresent();
	case IDM_ADDBOOKMARK:
		// "Add bookmark" is allowed if the file is not
		// empty and there is no selection going on.
		return m_dataArray.GetLength() && !bSelected;
	case IDM_REMOVE_BKM:
	case IDM_CLEARALL_BMK:
		// "Remove bookmark" and "Clear all bookmarks" are allowed if there are bookmarks set.
		return iBmkCount > 0;
	case IDM_EDIT_READONLYMODE:
		// No editing of drives
		return (Drive == 0);
	case IDM_SAVE:
		// Definitely no saving for drives!
		return (Drive == 0);
	}
	return TRUE;
}
//--------------------------------------------------------------------------------------------
// Initialize main menu items.
int HexEditorWindow::initmenupopup(WPARAM w, LPARAM l)
{
	HMENU h = (HMENU) w;
	int i = GetMenuItemCount(h);
	while (i)
	{
		if (UINT id = GetMenuItemID(h, --i))
		{
			EnableMenuItem(h, i, queryCommandEnabled(id) ?
				MF_ENABLED | MF_BYPOSITION : MF_GRAYED | MF_BYPOSITION);
		}
	}

	enum { File, Disk, Edit, View, Options, Registry, Bookmarks, Help };
	switch (l)
	{
	case File:
		// Create the MRU list.
		make_MRU_list(h);
		break;
	case Disk:
		break;
	case Edit:
		CheckMenuItem(h, IDM_EDIT_READONLYMODE, bReadOnly ? MF_CHECKED : MF_UNCHECKED);
		break;
	case View:
		break;
	case Options:
		CheckMenuItem(h, IDM_MAKE_BACKUPS, bMakeBackups ? MF_CHECKED : MF_UNCHECKED);
		break;
	case Registry:
		CheckMenuItem(h, IDM_CONTEXT, contextpresent() ? MF_CHECKED : MF_UNCHECKED);
		CheckMenuItem(h, IDM_UNKNOWN, unknownpresent() ? MF_CHECKED : MF_UNCHECKED);
		CheckMenuItem(h, IDM_SAVEINI, bSaveIni ? MF_CHECKED : MF_UNCHECKED);
		CheckMenuItem(h, IDM_DEFAULT, defaultpresent() ? MF_CHECKED : MF_UNCHECKED);
		break;
	case Bookmarks:
		// Create the bookmark list.
		make_bookmark_list(h);
		break;
	case Help:
		break;
	}
	return 0;
}

/**
 * @brief Handler on window closing.
 * @return 0 if Frhed cannot be closed, 1 if we can close Frhed.
 */
bool HexEditorWindow::close()
{
	if (iFileChanged)
	{
		int res = MessageBox(pwnd, GetLangString(IDS_SAVE_MODIFIED), MB_YESNOCANCEL | MB_ICONWARNING);
		if (res == IDCANCEL || res == IDYES && !(bFileNeverSaved ? CMD_save_as() : CMD_save()))
			//User doesn't want to quit or User wants to save and the save was unsuccessful
			return false;//Don't exit
	}
	EnableDriveButtons(FALSE);
	delete Drive;
	Drive = 0;
	return true;
}

//--------------------------------------------------------------------------------------------
// WM_LBUTTONUP handler.
int HexEditorWindow::lbuttonup(int xPos, int yPos)
{
	// Kill timer.
	kill_scroll_timers();
	pwnd->KillTimer(MOUSE_OP_DELAY_TIMER_ID);
	bMouseOpDelayTimerSet = FALSE;
	// Release mouse.
	if (HWindow::GetCapture() != pwnd)
		return 0;
	ReleaseCapture();

	if (bSelecting)
	{
		get_pos(xPos, yPos);
		iCurNibble = 0;
		switch (area)
		{
		case AREA_BYTES:
		case AREA_OFFSETS:
			iEnteringMode = BYTES;
			break;
		default:
			iEnteringMode = CHARS;
			break;
		}
	}
	else if (!dragging)
	{
		get_pos(xPos, yPos);
		switch (area)
		{
		case AREA_BYTES:
		case AREA_OFFSETS:
			iCurNibble = nibblenum;
			if (iCurNibble == 2)
				return 0;
			iEnteringMode = BYTES;
			break;
		default:
			iEnteringMode = CHARS;
			break;
		}

		SetCursor(LoadCursor(NULL, IDC_IBEAM));
		if (!bSelected)
		{
			int a = iCurByte / iBytesPerLine;
			int b = new_pos / iBytesPerLine;
			iCurByte = new_pos;
			repaint(a, b);
		}
		else
		{
			int a = iStartOfSelection / iBytesPerLine;
			int b = iEndOfSelection / iBytesPerLine;
			if (a > b)
				swap(a, b);
			bSelected = false;
			iCurByte = new_pos;
			int c = iCurByte / iBytesPerLine;
			if (c < a || c > b)
				repaint(c);
			repaint(a, b);
		}
	}
	bSelecting = false;
	set_caret_pos();

	return 0;
}

//--------------------------------------------------------------------------------------------
// WM_MOUSEMOVE handler.
int HexEditorWindow::mousemove(int xPos, int yPos)
{
	iMouseX = xPos;
	iMouseY = yPos;

	const bool captured = HWindow::GetCapture() == pwnd;
	const bool bLButtonDown = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 && captured;

	if (!bLButtonDown || bSelecting)
		get_pos(xPos, yPos);

	if (bLButtonDown)
	{
		if (bSelecting)
		{
			fix_scroll_timers(xPos, yPos);

			int lastbyte = m_dataArray.GetUpperBound();
			if (new_pos > lastbyte)
				new_pos = lastbyte;
			if (iEndOfSelection != new_pos)
			{
				bSelected = true;
				int oeos = iEndOfSelection;
				iEndOfSelection = new_pos;
				repaint(oeos / iBytesPerLine, new_pos / iBytesPerLine);
			}
		}
		/*else if (bPullScrolling)
		{
			if (!GetCapture())
			{
				SetCapture(hwnd);
				SetCursor(LoadCursor(NULL, IDC_SIZEALL));
			}
			scroll_to_point(xPos, yPos);
		}*/
		else if (bMouseOpDelayTimerSet && (
			xPos < iLBDownX - MouseOpDist || xPos > iLBDownX + MouseOpDist ||
			yPos < iLBDownY - MouseOpDist || yPos > iLBDownX + MouseOpDist))
		{
			start_mouse_operation();
		}
	}
	else
	{
		SetCursor(LoadCursor(NULL,
			enable_drag && bSelected &&
			new_pos >= iGetStartOfSelection() &&
			new_pos <= iGetEndOfSelection() ?
			IDC_ARROW : IDC_IBEAM));
	}

	old_pos = new_pos;

	return 0;
}

//--------------------------------------------------------------------------------------------
// WM_LBUTTONDOWN handler.
int HexEditorWindow::lbuttondown(int nFlags, int xPos, int yPos)
{
	iLBDownX = xPos;
	iLBDownY = yPos;

	pwnd->SetCapture();

	//Someone sent us invalid data
	if ((nFlags & (~ (MK_CONTROL | MK_LBUTTON | MK_MBUTTON | MK_RBUTTON | MK_SHIFT))) ||
		(xPos < 0 || xPos > cxClient) ||
		(yPos < 0 || yPos > cyClient))
	{
		return 0;
	}

	get_pos(xPos, yPos);
	switch (area)
	{
	case AREA_OFFSETS:
		if (!bAutoOffsetLen && iMinOffsetLen > 1)
		{
			iMinOffsetLen--;
			save_ini_data();
			resize_window();
			return 0;
		}
	default:
		lbd_pos = old_pos = new_pos;
	}

	//User wants to extend selection
	if (nFlags & MK_SHIFT)
	{
		if (nibblenum != 2)
		{
			const int length = m_dataArray.GetLength();
			if (new_pos == length)
				new_pos--;
			if (new_pos < length)
			{
				bSelecting = true;
				if (iEndOfSelection != new_pos)
				{
					const int oeos = iEndOfSelection;
					iEndOfSelection = new_pos;
					if (!bSelected)
					{
						bSelected = true;
						iStartOfSelection = iCurByte;
					}
					repaint(oeos / iBytesPerLine, new_pos / iBytesPerLine);
				}
			}
		}
	}
	else
	{
		/*Set the timer and wait until we know that the user wants some kind of
		mouse operation. We know this because they will hold the mouse down for
		some time or they will move the mouse around.*/
		pwnd->SetTimer(MOUSE_OP_DELAY_TIMER_ID, MouseOpDelay, NULL);
		bMouseOpDelayTimerSet = TRUE;
	}
	return 0;
}

/**
 * @brief Handle mousewheel messages.
 * This function handles WM_MOUSEWHEEL messages and scrolls or zooms the view.
 * @param [in] delta Mouse wheel movement.
 */
void HexEditorWindow::mousewheel(int delta)
{
	const bool ctrlDown = (GetAsyncKeyState(VK_CONTROL) &0x8000) != 0;
	if (!ctrlDown)
	{
		// If CTRL-key not pressed, scroll
		int pos = pwnd->GetScrollPos(SB_VERT) - delta / (WHEEL_DELTA / 3);
		pwnd->SetScrollPos(SB_VERT, pos, TRUE);
		vscroll(SB_THUMBTRACK);
	}
	else
	{
		// If CTRL-key pressed, zoom
		if (delta > 0)
			CMD_zoom(+1);
		else
			CMD_zoom(-1);
	}
}

//Pabs inserted
void HexEditorWindow::get_pos(long x, long y)
{
	area = AREA_NONE;

	int scr_column = x / cxChar; // Column on the screen.
	int scr_row = y / cyChar; // Line on the screen.
	column = scr_column + iHscrollPos; // Absolute column number.
	line = iVscrollPos + scr_row; // Absolute line number.

	int iStartofChars = iGetCharsPerLine() - iBytesPerLine;
	int iStartofBytes = iMaxOffsetLen + iByteSpace;
	new_pos = bytenum = nibblenum = -1;

	//Click on offsets
	if (column >= 0 && column < iStartofBytes)
	{
		nibblenum = 0;
		bytenum = 0;
		new_pos = line * iBytesPerLine;
		area = AREA_OFFSETS;
	}
	// Click on bytes.
	else if ( column >= iStartofBytes && column < iStartofChars - iCharSpace - 1)
	{
		int relpos = column - iStartofBytes;
		nibblenum = relpos%3;
		bytenum = relpos/3;
		area = AREA_BYTES;
		if( nibblenum==2 ){
			if( x%cxChar > cxChar/2 ){
				bytenum++;
				nibblenum = 0;
			}
			else nibblenum = 1;
		}
	}
	// Click between bytes and chars
	else if ( column >= iStartofChars - iCharSpace - 1 && column < iStartofChars )
	{
		if( x + iHscrollPos*cxChar <= cxChar*(iStartofChars-1)-cxChar*iCharSpace/2 ){
			area = AREA_BYTES;
			nibblenum = 1;
			bytenum = iBytesPerLine - (dragging ? 0 : 1);
		} else {
			area = AREA_CHARS;
			bytenum = 0;
		}
	}
	// Click on chars.
	else if (column >= iStartofChars && column < iGetCharsPerLine())
	{
		bytenum = column - iStartofChars;
		area = AREA_CHARS;
	}
	//Click after chars
	else
	{
		bytenum = iBytesPerLine;
		new_pos = (line+1) * iBytesPerLine - 1;
	}

	if( area >= AREA_BYTES ){
		new_pos = line * iBytesPerLine + bytenum;
	}

	int lastbyte = m_dataArray.GetUpperBound();
	if( new_pos > lastbyte+1 )
	{
		nibblenum = 1;
		new_pos = lastbyte+1;
		bytenum = new_pos%iBytesPerLine;
		line = new_pos/iBytesPerLine;
	}
	else if( new_pos < 0 )
		line = bytenum = nibblenum = new_pos = 0;
}

void HexEditorWindow::set_drag_caret(long x, long y, bool Copying, bool Overwrite)
{
	get_pos(x, y);

	int iStartofBytes = iMaxOffsetLen + iByteSpace;

	if( area == AREA_OFFSETS ){
		area = AREA_BYTES;
	}
	else if( column >= iGetCharsPerLine() ){
		new_pos++;
		area = AREA_CHARS;
	}

	int update = 0;
	int lastbyte = m_dataArray.GetUpperBound();
	if (bSelected /*&& !bAllowDropInSel*/ )
	{
		int iStartOfSelSetting = iStartOfSelection;
		int iEndOfSelSetting = iEndOfSelection;
		if (iStartOfSelSetting > iEndOfSelSetting)
			swap(iStartOfSelSetting, iEndOfSelSetting);
		if (Copying || !dragging || Overwrite)
		{
			if (new_pos >= iStartOfSelSetting + 1 && new_pos <= iEndOfSelSetting)
			{
				if (new_pos < (iStartOfSelSetting + iEndOfSelSetting) / 2)
					new_pos = iStartOfSelSetting;
				else
					new_pos = iEndOfSelSetting + 1;
				update = 1;
			}
		}
		else //if ( Moving )
		{
			if (new_pos >= iStartOfSelSetting && new_pos <= iEndOfSelSetting + 1)
			{
				if (!iStartOfSelSetting && iEndOfSelSetting == lastbyte)
				{
					new_pos = new_pos >= (iStartOfSelSetting + iEndOfSelSetting) / 2 ? lastbyte + 1 : 0;
				}
				else if (new_pos <= (iStartOfSelSetting + iEndOfSelSetting) / 2)
				{
					if (iStartOfSelSetting)
						new_pos = iStartOfSelSetting - 1;
					else
						new_pos = iEndOfSelSetting + 2;
				}
				else
				{
					if (iEndOfSelSetting == lastbyte)
						new_pos = iStartOfSelSetting - 1;
					else
						new_pos = iEndOfSelSetting + 2;
				}
				update = 1;
			}
		}
	}

	if (new_pos > lastbyte + 1)
	{
		new_pos = lastbyte + 1;
		update = 1;
	}
	else if (new_pos < 0)
	{
		new_pos = 0;
		update = 1;
	}

	if (update)
	{
		bytenum = new_pos % iBytesPerLine;
		line = new_pos / iBytesPerLine;
	}

	y = line;
	x = bytenum;

	if (area == AREA_BYTES)
	{
		x = x * 3 + iStartofBytes - 1;
	}
	else if (area == AREA_CHARS)
	{
		x = iGetCharsPerLine() - iBytesPerLine + x;
	}

	x -= iHscrollPos;
	y -= iVscrollPos;

	if (x != old_col || y != old_row)
	{
		old_col = x; old_row = y;

		x *= cxChar;
		y *= cyChar;
		if (area == AREA_BYTES)
		{
			x += cxChar / 2;
		}
		//Set caret position & show it
		SetCaretPos(x, y);
		pwnd->ShowCaret();
	}
}

void HexEditorWindow::fix_scroll_timers(long x, long y)
{
	SCROLL_TYPE vert = x < cxChar ? SCROLL_BACK : x <= (cxClient / cxChar - 1) * cxChar ? SCROLL_NONE : SCROLL_FORWARD;
	SCROLL_TYPE horz = y < cyChar ? SCROLL_BACK : y <= (cyClient / cyChar - 1) * cyChar ? SCROLL_NONE : SCROLL_FORWARD;
	if (prev_vert == SCROLL_NONE && vert != SCROLL_NONE || prev_horz == SCROLL_NONE && horz != SCROLL_NONE)
	{
		if (bSelecting)
		{
			if (enable_scroll_delay_sel)
			{
				pwnd->SetTimer(SCROLL_DELAY_TIMER_ID, ScrollDelay, NULL);
			}
			else
			{
				bScrollTimerSet = true;
				pwnd->SetTimer(SCROLL_TIMER_ID, ScrollInterval, NULL);
			}
		}
		else if (dragging)
		{
			if (enable_scroll_delay_dd)
			{
				pwnd->SetTimer(SCROLL_DELAY_TIMER_ID, ScrollDelay, NULL);
			}
			else
			{
				bScrollTimerSet = true;
				pwnd->SetTimer(SCROLL_TIMER_ID, ScrollInterval, NULL);
			}
		}
	}
	else if (prev_vert != SCROLL_NONE && vert == SCROLL_NONE || prev_horz != SCROLL_NONE && horz == SCROLL_NONE)
	{
		kill_scroll_timers();
	}
	prev_vert = vert;
	prev_horz = horz;
}

void HexEditorWindow::kill_scroll_timers()
{
	pwnd->KillTimer(SCROLL_DELAY_TIMER_ID);
	pwnd->KillTimer(SCROLL_TIMER_ID);
	bScrollTimerSet = false;
	prev_vert = prev_horz = SCROLL_NONE;
}

//end

void HexEditorWindow::reset()
{
	bFileNeverSaved = true;
	bSelected = false;
	bSelecting = false;
	iFileChanged = FALSE;
	bFilestatusChanged = true;
	iVscrollMax = 0;
	iVscrollPos = 0;
	iHscrollMax = 0;
	iHscrollPos = 0;
	iCurByte = 0;
	iCurNibble = 0;
	bPartialStats = false;
	bPartialOpen = false;
	// Delete old data.
	m_dataArray.ClearAll();
	_tcscpy(filename, GetLangString(IDS_UNTITLED));
	resize_window();
}

//-------------------------------------------------------------------
// On find command.
void HexEditorWindow::CMD_find()
{
	static_cast<dialog<FindDlg>*>(this)->DoModal(pwnd);
}

//-------------------------------------------------------------------
// On copy command.
void HexEditorWindow::CMD_edit_copy()
{
	static_cast<dialog<CopyDlg>*>(this)->DoModal(pwnd);
}

//-------------------------------------------------------------------
// On hexdump to file/clipboard command.
int HexEditorWindow::CMD_copy_hexdump(int iCopyHexdumpMode, int iCopyHexdumpType,
	int iCopyHexdumpDlgStart, int iCopyHexdumpDlgEnd, char* mem, DWORD memlen)
{
	int iCharsPerLine = iGetCharsPerLine();

//Pabs changed - bugfix insert
	if (iCopyHexdumpDlgEnd < iCopyHexdumpDlgStart)
		swap(iCopyHexdumpDlgStart, iCopyHexdumpDlgEnd);
//end
	// Show wait cursor.

//Pabs removed - see further down
//Done so that you can select partial lines for non-display output
//If both on the same line in display output just that line is output

//Pabs changed - line insert & following lines indented
	char *pMem = mem;
	SIZE_T buflen;
	if (iCopyHexdumpType == IDC_EXPORTDISPLAY)
	{
		iCopyHexdumpDlgStart = iCopyHexdumpDlgStart / iBytesPerLine * iBytesPerLine;//cut back to the line start
		iCopyHexdumpDlgEnd = iCopyHexdumpDlgEnd / iBytesPerLine * iBytesPerLine;//cut back to the line start
//end
		// Number of lines to copy:
		int linecount = (iCopyHexdumpDlgEnd - iCopyHexdumpDlgStart) / iBytesPerLine + 1;
		// Req'd mem for lines:
		// (Every line ended with CR+LF ('\r'+'\n'))
//Pabs changed - "int" removed - see further up
		buflen = linecount * (iGetCharsPerLine() + 2) + 1;
		if (mem && buflen > memlen)
			return 0;
//end
		// Create hexdump.
		char buf2[128];
//Pabs changed - "char*" removed - see further up
		if (!mem)
		{
			pMem = new char[buflen];
			if (!pMem)
				return 0;
		}
//end
		memset(pMem, ' ', buflen);
		// Write hexdump.
		// a = first byte of first line of hexdump.
		// b = first byte of last line of hexdump.
		//b = iCopyHexdumpDlgEnd;
		// a = Offset of current line.
		// k = Offset in text array.
		for (int k = 0, a = iCopyHexdumpDlgStart; a <= iCopyHexdumpDlgEnd; a += iBytesPerLine, k += iCharsPerLine + 2)
		{
			// Write offset.
			int m = sprintf(buf2, "%*.*x", iMinOffsetLen, iMinOffsetLen, bPartialStats ? a + iPartialOffset : a);

			memset(buf2 + m, ' ', iMaxOffsetLen + iByteSpace - m);
			buf2[iMaxOffsetLen + iByteSpace] = '\0';

			int l = 0; // l = Offset in line, relative to k.
			int n = 0;
			while (buf2[n] != '\0')
				pMem[k + (l++)] = buf2[n++]; // Copy Offset. l = next empty place after spaces.
			// Write bytes and chars.
			for (int j = 0 ; j < iBytesPerLine ; j++)
			{
				if (a + j > m_dataArray.GetUpperBound())
				{
					// Nonexistant byte.
					pMem[k + l + j*3    ] = ' ';
					pMem[k + l + j*3 + 1] = ' ';
					pMem[k + l + j*3 + 2] = ' ';
					// Nonexistant char.
					pMem[k + l + iBytesPerLine*3 + iCharSpace + j] = ' ';
				}
				else
				{
					// Write byte.
					sprintf(buf2, "%2.2x ", m_dataArray[a + j]);
					pMem[k + l + j*3    ] = buf2[0];
					pMem[k + l + j*3 + 1] = buf2[1];
					pMem[k + l + j*3 + 2] = buf2[2];
					// Write char.
					if( iCharacterSet == OEM_FIXED_FONT && m_dataArray[a + j] != 0 )
						pMem[k + l + iBytesPerLine*3 + iCharSpace + j] = m_dataArray[a + j];
					else if( (m_dataArray[a + j] >= 32 && m_dataArray[a + j] <= 126) || (m_dataArray[a + j]>=160 && m_dataArray[a + j] <= 255) || (m_dataArray[a + j] >= 145 && m_dataArray[a + j] <= 146) )
						pMem[k + l + iBytesPerLine*3 + iCharSpace + j] = m_dataArray[a + j];
					else
						pMem[k + l + iBytesPerLine*3 + iCharSpace + j] = '.';
				}
			}
			pMem[k + iCharsPerLine] = '\r';
			pMem[k + iCharsPerLine + 1] = '\n';
		}
		pMem[buflen - 1] = '\0';
//Pabs changed - line insert
	}
	else if (iCopyHexdumpType == IDC_EXPORTDIGITS)
	{
		// Req'd mem for lines:
		int numchar = iCopyHexdumpDlgEnd - iCopyHexdumpDlgStart + 1;
		buflen = numchar * 2 + 1;
		if (mem && buflen > memlen)
			return 0;
		// Create hexdump.
		if (!mem)
		{
			pMem = new char[buflen];
			if (!pMem)
				return 0;
		}
		for (int i = 0 ; i < numchar ; ++i)
		{
			sprintf(pMem + i * 2, "%2.2x", m_dataArray[iCopyHexdumpDlgStart + i]);
		}
		pMem[buflen - 1] = '\0';
	}
	else if (iCopyHexdumpType == IDC_EXPORTRTF)
	{
		if (mem)
			return 0;
		//Bit of a kludge here
		pMem = (char*)RTF_hexdump(iCopyHexdumpDlgStart, iCopyHexdumpDlgEnd, &buflen);
		if (!pMem)
			return 0;
	}
	else
		return 0;
	if (mem)
		return 1;
	if (iCopyHexdumpMode == 0)
	{
		// to file.
		TCHAR szFileName[_MAX_PATH];
		TCHAR szTitleName[_MAX_FNAME + _MAX_EXT];
		_tcscpy(szFileName, _T("hexdump.txt"));
		HGLOBAL hg = NULL;
		if (iCopyHexdumpType == IDC_EXPORTRTF)
		{
			hg = (HGLOBAL)pMem;
			pMem = (char*)GlobalLock(hg);
			if (!pMem)
			{
				GlobalFree(hg);
				return 0;
			}
			PathRenameExtension(szFileName, _T(".rtf"));
		}

		// to file.
		OPENFILENAME ofn;
		ZeroMemory(&ofn, sizeof ofn);
		ofn.lStructSize = sizeof ofn;
		ofn.hwndOwner = pwnd->m_hWnd;
		ofn.lpstrFilter = GetLangString(IDS_OPEN_ALL_FILES);
		ofn.lpstrFile = szFileName;
		ofn.nMaxFile = _MAX_PATH;
		ofn.lpstrFileTitle = szTitleName;
		ofn.nMaxFileTitle = _MAX_FNAME + _MAX_EXT;
		ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT;
		if (GetSaveFileName(&ofn))
		{
			int filehandle = _topen(szFileName, _O_RDWR|_O_CREAT|_O_TRUNC|_O_BINARY,_S_IREAD|_S_IWRITE);
			if (filehandle != -1)
			{
				// Write file.
				if (_write(filehandle, pMem, buflen - 1) != -1)
				{//Pabs replaced NULL w hwnd
					MessageBox(pwnd, GetLangString(IDS_HDUMP_SAVED), MB_ICONINFORMATION);
				}
				else
					MessageBox(pwnd, GetLangString(IDS_HDUMP_SAVE_FAIL), MB_ICONERROR);
				_close(filehandle);
			}
			else
				MessageBox(pwnd, GetLangString(IDS_HDUMP_SAVE_FAIL), MB_ICONERROR);
		}//end
		if (iCopyHexdumpType == IDC_EXPORTRTF)
		{
			GlobalUnlock(hg);
			GlobalFree(hg);
			hg = pMem = NULL;
		}
	}
	else if (iCopyHexdumpType != IDC_EXPORTRTF)
	{
		// To clipboard.
		if (pwnd->OpenClipboard())
		{
			if (HGLOBAL hGlobal = GlobalAlloc(GHND, buflen))
			{
				if (char *pDest = reinterpret_cast<char *>(GlobalLock(hGlobal)))
				{
					memcpy(pDest, pMem, buflen);
					GlobalUnlock(hGlobal);
				}
				EmptyClipboard();
				SetClipboardData(CF_TEXT, hGlobal);
				CloseClipboard();
			}
			else
				MessageBox(pwnd, GetLangString(IDS_HDUMP_NO_MEM_CL), MB_ICONERROR);
		}
		else
			MessageBox(pwnd, GetLangString(IDS_HDUMP_CL_FAIL), MB_ICONERROR);
	}
	else //if (iCopyHexdumpType == IDC_EXPORTRTF)
	{
		// To clipboard.
		if (pwnd->OpenClipboard())
		{
			EmptyClipboard();
			SetClipboardData(CF_RICH_TEXT_FORMAT, (HGLOBAL)pMem);
			CloseClipboard();
		}
		else
			MessageBox(pwnd, GetLangString(IDS_HDUMP_CL_FAIL), MB_ICONERROR);
		pMem = NULL;
	}
	delete [] pMem;
	return 1;
}

void HexEditorWindow::CMD_copy_hexdump()
{
	static_cast<dialog<CopyHexdumpDlg>*>(this)->DoModal(pwnd);
}

//-------------------------------------------------------------------
// On paste command.
void HexEditorWindow::CMD_edit_paste()
{
	static_cast<dialog<PasteDlg>*>(this)->DoModal(pwnd);
}

//-------------------------------------------------------------------
// On "enter decimal value" command.
void HexEditorWindow::CMD_edit_enterdecimalvalue()
{
	EnterDecimalValueDlg::bSigned = !bUnsignedView;
	static_cast<dialog<EnterDecimalValueDlg>*>(this)->DoModal(pwnd);
}

/**
 * @brief Cut-command handler.
 */
void HexEditorWindow::CMD_edit_cut()
{
	static_cast<dialog<CutDlg>*>(this)->DoModal(pwnd);
}

/**
 * @brief Delete-command handler.
 */
void HexEditorWindow::CMD_edit_clear()
{
	static_cast<dialog<DeleteDlg>*>(this)->DoModal(pwnd);
}

/**
 * @brief Open a new window.
 * @param [in] title Title of the new window.
 * @return 1
 */
int HexEditorWindow::CMD_new()
{
	if (!close())
		return 0;
	reset();
	return 1;
}

/**
 * @brief Save file with another name.
 * @return 1 if saving succeeded, 0 otherwise.
 */
int HexEditorWindow::CMD_save_as()
{
	TCHAR szFileName[MAX_PATH];
	szFileName[0] = _T('\0');
	if (Drive == 0)
		_tcscpy(szFileName, filename);
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof ofn);
	ofn.lStructSize = sizeof ofn;
	ofn.hwndOwner = pwnd->m_hWnd;
	ofn.lpstrFilter = GetLangString(IDS_OPEN_ALL_FILES);
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = _MAX_PATH;
	ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	if (!GetSaveFileName(&ofn))
		return 0;
	WaitCursor w1;
	int filehandle = _topen(szFileName, _O_RDWR|_O_CREAT|_O_TRUNC|_O_BINARY, _S_IREAD|_S_IWRITE);
	if (filehandle == -1)
	{
		MessageBox(pwnd, GetLangString(IDS_FILE_SAVE_ERROR), MB_ICONERROR);
		return 0;
	}
	int done = 0;
	if (_write(filehandle, m_dataArray, m_dataArray.GetLength()) != -1)
	{
		// File was saved.
		GetLongPathNameWin32(szFileName, filename);
		iFileChanged = FALSE;
		bFilestatusChanged = true;
		bFileNeverSaved = false;
		bPartialStats = false;
		bPartialOpen = false;
		update_MRU();
		done = 1;
	}
	else
	{
		MessageBox(pwnd, GetLangString(IDS_FILE_SAVE_ERROR), MB_ICONERROR);
	}
	_close(filehandle);
	set_wnd_title();
	return done;
}
/**
 * @brief Save the file.
 * @return 1 if saving succeeded, 0 otherwise.
 */
int HexEditorWindow::CMD_save()
{
	WaitCursor w1;

	// File was not saved before => name must be chosen.
	if (bFileNeverSaved)
		return CMD_save_as();

//Pabs inserted
	if (bMakeBackups)
	{//Assume the filename is valid & has a length
		TCHAR newname[RTL_NUMBER_OF(filename) + 4];
		_tcscpy(newname, filename);
		_tcscat(newname, _T(".bak"));
		_tremove(newname);
		//Must use Win32 here as the CRT has no copy function only rename
		//& we need a copy of the file to be present for saving a partially opened file
		if (!CopyFile(filename, newname, TRUE))
		{
			MessageBox(pwnd, GetLangString(IDS_FILE_BACKUP_ERROR), MB_ICONWARNING);
		}
	}
	int done = 0;
	// File is partially loaded => must be saved partially or saved as.
	if (bPartialOpen)
	{
		int filehandle = _topen(filename, _O_RDWR|_O_BINARY);
		if (filehandle == -1)
		{
			MessageBox(pwnd, GetLangString(IDS_ERR_SAVE_PARTIAL), MB_ICONERROR);
			return 0;
		}
		int nbl = m_dataArray.GetLength(); // Length of the m_dataArray
		if (nbl != iPartialOpenLen)
		{
			INT64 i = iPartialOffset + iPartialOpenLen; // loop var & start of loop
			INT64 e = iPartialFileLen - 1; // end of loop
			int n = 1; // loop increment
			if (nbl > iPartialOpenLen)
			{//Bigger .'. we need to start at the end
				n = -1;
				swap(e, i);
			}
			int r = nbl - iPartialOpenLen; // relative movement len
			//move the data
			e += n;
			do
			{
				BYTE tmp;//Temporary byte for moving data
				_lseeki64(filehandle, i, SEEK_SET);
				_read(filehandle, &tmp, 1);
				_lseeki64(filehandle, i + r, SEEK_SET);
				if (-1 == _write(filehandle, &tmp, 1))
				{
					MessageBox(pwnd, GetLangString(IDS_ERR_MOVE_DATA), MB_ICONERROR);
					_close(filehandle);
					return 0;
				}
				i += n;
			} while (i != e);
			if (nbl < iPartialOpenLen)
			{//If the new file is bigger than the first _write will resize the file properly otherwise we need to specifically resize the file
				if (_lseeki64(filehandle, iPartialFileLen + r, SEEK_SET) == -1 ||
					!SetEndOfFile((HANDLE)_get_osfhandle(filehandle)))
				{
					MessageBox(pwnd, GetLangString(IDS_ERR_RESIZE_FILE), MB_ICONERROR);
					_close(filehandle);
					return 0;
				}
			}
		}
		if (_lseeki64(filehandle, iPartialOffset, 0) == -1)
		{
			MessageBox(pwnd, GetLangString(IDS_ERR_SEEK_FILE), MB_ICONERROR);
		}
		else if (_write( filehandle, m_dataArray, nbl) == -1)
		{
			MessageBox(pwnd, GetLangString(IDS_ERR_WRITE_FILE), MB_ICONERROR);
		}
		else
		{
			done = 1;
			iFileChanged = FALSE;
			bFilestatusChanged = true;
			set_wnd_title();
		}
		_close(filehandle);
		return done;
	}

	int filehandle = _topen(filename, _O_RDWR|_O_CREAT|_O_TRUNC|_O_BINARY, _S_IREAD|_S_IWRITE);
	if (filehandle == -1)
	{
		MessageBox(pwnd, GetLangString(IDS_FILE_SAVE_ERROR), MB_ICONERROR);
		return 0;
	}
	if (_write(filehandle, m_dataArray, m_dataArray.GetLength()) == -1)
	{
		MessageBox(pwnd, GetLangString(IDS_ERR_WRITE_FILE), MB_ICONERROR);
	}
	else
	{
		done = 1;
		iFileChanged = FALSE;
		bFilestatusChanged = true;
		bPartialStats = false;
		bPartialOpen = false;
		set_wnd_title();
	}
	_close(filehandle);
	return done;
}

//-------------------------------------------------------------------
void HexEditorWindow::CMD_open()
{
	if (!close())
		return;
	TCHAR szFileName[MAX_PATH];
	szFileName[0] = '\0';
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof ofn);
	ofn.lStructSize = sizeof ofn;
	ofn.hwndOwner = pwnd->m_hWnd;
	ofn.lpstrFilter = GetLangString(IDS_OPEN_ALL_FILES);
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = _MAX_PATH;
	ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT;
	if (GetOpenFileName(&ofn))
		load_file(szFileName);
}

//-------------------------------------------------------------------
void HexEditorWindow::adjust_view_for_selection()
{
	if (bSelected)
	{
		int iCharStart = iMaxOffsetLen + iByteSpace + iBytesPerLine * 3 + iCharSpace;
//Pabs changed to put selection in center of screen
		int iStartOfSelSetting = iStartOfSelection;
		int iEndOfSelSetting = iEndOfSelection;
		if (iStartOfSelSetting > iEndOfSelSetting)
			swap(iStartOfSelSetting, iEndOfSelSetting);

		int sosline = iStartOfSelSetting / iBytesPerLine;
		int eosline = iEndOfSelSetting / iBytesPerLine;

		int soscol,eoscol,maxcols;
		if (iEnteringMode == BYTES)
		{
			soscol = iMaxOffsetLen + iByteSpace + ( iStartOfSelSetting % iBytesPerLine) * 3;
			eoscol = iMaxOffsetLen + iByteSpace + ( iEndOfSelSetting % iBytesPerLine) * 3 + 2;
			maxcols = iBytesPerLine * 3;
		}
		else
		{
			soscol = iCharStart + iStartOfSelSetting % iBytesPerLine;
			eoscol = iCharStart + iEndOfSelSetting % iBytesPerLine;
			maxcols = iBytesPerLine;
		}

		int lines = eosline - sosline + 1;
		int cols = ((eosline == sosline) ? eoscol - soscol + 1 : maxcols);

		if( lines > cyBuffer ){
			if( iVscrollPos <= (sosline+eosline-cyBuffer+1)/2 )
				iVscrollPos = sosline;
			else iVscrollPos = eosline-cyBuffer+1;
		}
		else iVscrollPos = sosline - (cyBuffer - (eosline-sosline) )/2;

		int mincol, maxcol;
		if(soscol>=eoscol){maxcol=soscol;mincol=eoscol;}
		else{maxcol=eoscol;mincol=soscol;}

		if( cols > cxBuffer && maxcol-mincol+1 > cxBuffer ){
			if( abs(iHscrollPos-mincol) < abs(iHscrollPos+cxBuffer-1-maxcol) )
				iHscrollPos = mincol-1;
			else iHscrollPos = maxcol-cxBuffer+1;
		}
		else{
			if( maxcol-mincol+1 < cxBuffer && lines > 1){
				if (iEnteringMode == BYTES){
					mincol = iMaxOffsetLen + iByteSpace;
					maxcol = iMaxOffsetLen + iByteSpace + ( iBytesPerLine - 1 ) * 3 + 2;
				}
				else{
					mincol = iCharStart;
					maxcol = iCharStart + iBytesPerLine;
				}
			}
			iHscrollPos = mincol - (cxBuffer - (maxcol-mincol) )/2;
		}

		if( iHscrollPos > iHscrollMax - cxBuffer + 1 )
			iHscrollPos = iHscrollMax - cxBuffer + 1;
		if( iHscrollPos < 0 ) iHscrollPos = 0;
		if( iVscrollPos > iVscrollMax - cyBuffer + 1 )
			iVscrollPos = iVscrollMax - cyBuffer + 1;
		if( iVscrollPos < 0 ) iVscrollPos = 0;
		adjust_hscrollbar();
		adjust_vscrollbar();
//end
	}
}

//-------------------------------------------------------------------
void HexEditorWindow::CMD_view_settings()
{
	static_cast<dialog<ViewSettingsDlg>*>(this)->DoModal(pwnd);
}

//-------------------------------------------------------------------
// Change the color indicated by pColor.
void HexEditorWindow::CMD_color_settings(COLORREF *pColor)
{
	CHOOSECOLOR cc;
	COLORREF crCustColors[16];
	cc.lStructSize = sizeof (CHOOSECOLOR);
	cc.hwndOwner = pwnd->m_hWnd;
	cc.hInstance = NULL;
	cc.rgbResult = *pColor;
	cc.lpCustColors = crCustColors;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;
	cc.lCustData = 0L;
	cc.lpfnHook;
	cc.lpTemplateName = NULL;
	if (ChooseColor (&cc))
	{
		*pColor = cc.rgbResult;
		save_ini_data();
		resize_window();
	}
}

/**
 * @brief Read options.
 * @param [in] key Sub-key in registry for options (version string).
 * @todo Some error checking for reading values would be nice...
 * @todo Remove @p key after the Upgrade-feature and dialog have been removed.
 */
void HexEditorWindow::read_ini_data(TCHAR *key)
{
	// Is there any data for frhed in the registry?
	HKEY key1;
	const int keyname_size = 64;
	TCHAR keyname[keyname_size] = {0};
	LONG res;
	if (key == 0)
		_sntprintf(keyname, keyname_size - 1, _T("%s\\%d"), OptionsRegistrySettingsPath,
				iInstCount);
	else
		_sntprintf(keyname, keyname_size - 1, _T("%s\\%s\\%d"), OptionsRegistrySettingsPath, key,
				iInstCount);

	res = RegOpenKeyEx(HKEY_CURRENT_USER, keyname, 0, KEY_ALL_ACCESS, &key1);
	if (res == ERROR_SUCCESS)
	{
		// There is data: read it.
		DWORD datasize = sizeof( int );
		LONG res;
		res = RegQueryValueEx(key1, _T("iTextColorValue"), NULL, NULL, (BYTE*) &iTextColorValue, &datasize);
		res = RegQueryValueEx(key1, _T("iBkColorValue"), NULL, NULL, (BYTE*) &iBkColorValue, &datasize);
		res = RegQueryValueEx(key1, _T("iSepColorValue"), NULL, NULL, (BYTE*) &iSepColorValue, &datasize);
		res = RegQueryValueEx(key1, _T("iSelTextColorValue"), NULL, NULL, (BYTE*) &iSelTextColorValue, &datasize);
		res = RegQueryValueEx(key1, _T("iSelBkColorValue"), NULL, NULL, (BYTE*) &iSelBkColorValue, &datasize);
		res = RegQueryValueEx(key1, _T("iBmkColor"), NULL, NULL, (BYTE*) &iBmkColor, &datasize);

		res = RegQueryValueEx(key1, _T("iAutomaticBPL"), NULL, NULL, (BYTE*) &iAutomaticBPL, &datasize);
		res = RegQueryValueEx(key1, _T("iBytesPerLine"), NULL, NULL, (BYTE*) &iBytesPerLine, &datasize);
		res = RegQueryValueEx(key1, _T("iOffsetLen"), NULL, NULL, (BYTE*) &iMinOffsetLen, &datasize);//Pabs replaced "iOffsetLen" with "iMinOffsetLen"
		res = RegQueryValueEx(key1, _T("iCharacterSet"), NULL, NULL, (BYTE*) &iCharacterSet, &datasize);
		res = RegQueryValueEx(key1, _T("iFontSize"), NULL, NULL, (BYTE*) &iFontSize, &datasize);
		res = RegQueryValueEx(key1, _T("bOpenReadOnly"), NULL, NULL, (BYTE*) &bOpenReadOnly, &datasize);

//Pabs inserted
		res = RegQueryValueEx(key1, _T("bMakeBackups"), NULL, NULL, (BYTE*) &bMakeBackups, &datasize);
		res = RegQueryValueEx(key1, _T("bAutoOffsetLen"), NULL, NULL, (BYTE*) &bAutoOffsetLen, &datasize);
		res = RegQueryValueEx(key1, _T("enable_drop"), NULL, NULL, (BYTE*) &enable_drop, &datasize);
		res = RegQueryValueEx(key1, _T("enable_drag"), NULL, NULL, (BYTE*) &enable_drag, &datasize);
		res = RegQueryValueEx(key1, _T("enable_scroll_delay_dd"), NULL, NULL, (BYTE*) &enable_scroll_delay_dd, &datasize);
		res = RegQueryValueEx(key1, _T("enable_scroll_delay_sel"), NULL, NULL, (BYTE*) &enable_scroll_delay_sel, &datasize);
		res = RegQueryValueEx(key1, _T("always_pick_move_copy"), NULL, NULL, (BYTE*) &always_pick_move_copy, &datasize);
		res = RegQueryValueEx(key1, _T("prefer_CF_HDROP"), NULL, NULL, (BYTE*) &prefer_CF_HDROP, &datasize);
		res = RegQueryValueEx(key1, _T("prefer_CF_BINARYDATA"), NULL, NULL, (BYTE*) &prefer_CF_BINARYDATA, &datasize);
		res = RegQueryValueEx(key1, _T("prefer_CF_TEXT"), NULL, NULL, (BYTE*) &prefer_CF_TEXT, &datasize);
		res = RegQueryValueEx(key1, _T("output_CF_BINARYDATA"), NULL, NULL, (BYTE*) &output_CF_BINARYDATA, &datasize);
		res = RegQueryValueEx(key1, _T("output_CF_TEXT"), NULL, NULL, (BYTE*) &output_CF_TEXT, &datasize);
		res = RegQueryValueEx(key1, _T("output_text_special"), NULL, NULL, (BYTE*) &output_text_special, &datasize);
		res = RegQueryValueEx(key1, _T("output_text_hexdump_display"), NULL, NULL, (BYTE*) &output_text_hexdump_display, &datasize);
//end
		res = RegQueryValueEx(key1, _T("always_pick_move_copy"), NULL, NULL, (BYTE*) &always_pick_move_copy, &datasize);

		LCID lcid = 0;
		res = RegQueryValueEx(key1, _T("locale"), NULL, NULL, (BYTE*) &lcid, &datasize);
		load_lang((LANGID)lcid);

		res = RegQueryValueEx(key1, _T("iWindowShowCmd"), NULL, NULL, (BYTE*) &iWindowShowCmd, &datasize);
		res = RegQueryValueEx(key1, _T("iWindowX"), NULL, NULL, (BYTE*) &iWindowX, &datasize);
		res = RegQueryValueEx(key1, _T("iWindowY"), NULL, NULL, (BYTE*) &iWindowY, &datasize);
		res = RegQueryValueEx(key1, _T("iWindowWidth"), NULL, NULL, (BYTE*) &iWindowWidth, &datasize);
		res = RegQueryValueEx(key1, _T("iWindowHeight"), NULL, NULL, (BYTE*) &iWindowHeight, &datasize);

		datasize = sizeof TexteditorName;
		res = SHGetValue(key1, 0, _T("TexteditorName"), 0, TexteditorName, &datasize);

		res = RegQueryValueEx(key1, _T("iMRU_count"), NULL, NULL, (BYTE*) &iMRU_count, &datasize);
		int i;
		for (i = 0 ; i < MRUMAX ; i++)
		{
			TCHAR fname[64];
			_stprintf(fname, _T("MRU_File%d"), i + 1);
			datasize = sizeof strMRU[i];
			res = RegQueryValueEx(key1, fname, NULL, NULL, (BYTE*) strMRU[i], &datasize);
		}
		// Close registry.
		RegCloseKey(key1);
	}
	else
	{
		// There is no data. Write with default values.
		save_ini_data();//end
	}
}

/**
 * @brief Save program options.
 */
void HexEditorWindow::save_ini_data()
{
	if (!bSaveIni)
		return;//just return if we are not to save ini data

	HKEY key1;

	TCHAR keyname[64];
	_stprintf(keyname, _T("%s\\%d"), OptionsRegistrySettingsPath, iInstCount);

	LONG res = RegCreateKey(HKEY_CURRENT_USER, keyname, &key1);
	if (res == ERROR_SUCCESS)
	{
		RegSetValueEx(key1, _T("iTextColorValue"), 0, REG_DWORD, (CONST BYTE*) &iTextColorValue, sizeof(int));
		RegSetValueEx(key1, _T("iBkColorValue"), 0, REG_DWORD, (CONST BYTE*) &iBkColorValue, sizeof(int));
		RegSetValueEx(key1, _T("iSepColorValue"), 0, REG_DWORD, (CONST BYTE*) &iSepColorValue, sizeof(int));
		RegSetValueEx(key1, _T("iSelTextColorValue"), 0, REG_DWORD, (CONST BYTE*) &iSelTextColorValue, sizeof(int));
		RegSetValueEx(key1, _T("iSelBkColorValue"), 0, REG_DWORD, (CONST BYTE*) &iSelBkColorValue, sizeof(int));
		RegSetValueEx(key1, _T("iBmkColor"), 0, REG_DWORD, (CONST BYTE*) &iBmkColor, sizeof(int));

		RegSetValueEx(key1, _T("iAutomaticBPL"), 0, REG_DWORD, (CONST BYTE*) &iAutomaticBPL, sizeof(int));
		RegSetValueEx(key1, _T("iBytesPerLine"), 0, REG_DWORD, (CONST BYTE*) &iBytesPerLine, sizeof(int));
		RegSetValueEx(key1, _T("iOffsetLen"), 0, REG_DWORD, (CONST BYTE*) &iMinOffsetLen, sizeof(int));//Pabs replaced "iOffsetLen" with "iMinOffsetLen"
		RegSetValueEx(key1, _T("iCharacterSet"), 0, REG_DWORD, (CONST BYTE*) &iCharacterSet, sizeof(int));
		RegSetValueEx(key1, _T("iFontSize"), 0, REG_DWORD, (CONST BYTE*) &iFontSize, sizeof(int));
		RegSetValueEx(key1, _T("bOpenReadOnly"), 0, REG_DWORD, (CONST BYTE*) &bOpenReadOnly, sizeof(int));

//Pabs inserted
		RegSetValueEx(key1, _T("bMakeBackups"), 0, REG_DWORD, (CONST BYTE*) &bMakeBackups, sizeof(int));
		RegSetValueEx(key1, _T("bAutoOffsetLen"), 0, REG_DWORD, (CONST BYTE*) &bAutoOffsetLen, sizeof(int));
		RegSetValueEx(key1, _T("enable_drop"), 0, REG_DWORD, (CONST BYTE*) &enable_drop, sizeof(int));
		RegSetValueEx(key1, _T("enable_drag"), 0, REG_DWORD, (CONST BYTE*) &enable_drag, sizeof(int));
		RegSetValueEx(key1, _T("enable_scroll_delay_dd"), 0, REG_DWORD, (CONST BYTE*) &enable_scroll_delay_dd, sizeof(int));
		RegSetValueEx(key1, _T("enable_scroll_delay_sel"), 0, REG_DWORD, (CONST BYTE*) &enable_scroll_delay_sel, sizeof(int));
		RegSetValueEx(key1, _T("always_pick_move_copy"), 0, REG_DWORD, (CONST BYTE*) &always_pick_move_copy, sizeof(int));
		RegSetValueEx(key1, _T("prefer_CF_HDROP"), 0, REG_DWORD, (CONST BYTE*) &prefer_CF_HDROP, sizeof(int));
		RegSetValueEx(key1, _T("prefer_CF_BINARYDATA"), 0, REG_DWORD, (CONST BYTE*) &prefer_CF_BINARYDATA, sizeof(int));
		RegSetValueEx(key1, _T("prefer_CF_TEXT"), 0, REG_DWORD, (CONST BYTE*) &prefer_CF_TEXT, sizeof(int));
		RegSetValueEx(key1, _T("output_CF_BINARYDATA"), 0, REG_DWORD, (CONST BYTE*) &output_CF_BINARYDATA, sizeof(int));
		RegSetValueEx(key1, _T("output_CF_TEXT"), 0, REG_DWORD, (CONST BYTE*) &output_CF_TEXT, sizeof(int));
		RegSetValueEx(key1, _T("output_text_special"), 0, REG_DWORD, (CONST BYTE*) &output_text_special, sizeof(int));
		RegSetValueEx(key1, _T("output_text_hexdump_display"), 0, REG_DWORD, (CONST BYTE*) &output_text_hexdump_display, sizeof(int));
		RegSetValueEx(key1, _T("output_CF_RTF"), 0, REG_DWORD, (CONST BYTE*) &output_CF_RTF, sizeof(int));
//end
		LCID lcid = MAKELCID(langArray.m_langid, 0);
		RegSetValueEx(key1, _T("locale"), 0, REG_DWORD, (CONST BYTE*) &lcid, sizeof lcid);

		SHSetValue(key1, 0, _T("TexteditorName"), REG_SZ, TexteditorName,
			static_cast<DWORD>(_tcslen(TexteditorName)) * sizeof(TCHAR));

		RegSetValueEx(key1, _T("iWindowShowCmd"), 0, REG_DWORD, (CONST BYTE*) &iWindowShowCmd, sizeof(int));
		RegSetValueEx(key1, _T("iWindowX"), 0, REG_DWORD, (CONST BYTE*) &iWindowX, sizeof(int));
		RegSetValueEx(key1, _T("iWindowY"), 0, REG_DWORD, (CONST BYTE*) &iWindowY, sizeof(int));
		RegSetValueEx(key1, _T("iWindowWidth"), 0, REG_DWORD, (CONST BYTE*) &iWindowWidth, sizeof(int));
		RegSetValueEx(key1, _T("iWindowHeight"), 0, REG_DWORD, (CONST BYTE*) &iWindowHeight, sizeof(int));

		RegSetValueEx(key1, _T("iMRU_count"), 0, REG_DWORD, (CONST BYTE*) &iMRU_count, sizeof(int));
		int i;
		for (i = 0; i < MRUMAX; i++)
		{
			TCHAR fname[64];
			_stprintf(fname, _T("MRU_File%d"), i + 1);
			RegSetValueEx(key1, fname, 0, REG_SZ, (CONST BYTE*) strMRU[i],
				static_cast<DWORD>(_tcslen(strMRU[i]) + 1) * sizeof(TCHAR));
		}
		// Close registry.
		RegCloseKey(key1);
	}
	else
	{
		MessageBox(pwnd, GetLangString(IDS_ERR_WRITING_OPTIONS), MB_ICONERROR);
	}
}

//-------------------------------------------------------------------
void HexEditorWindow::CMD_edit_append()
{
	static_cast<dialog<AppendDlg>*>(this)->DoModal(pwnd);
}

//-------------------------------------------------------------------
void HexEditorWindow::CMD_manipulate_bits()
{
	static_cast<dialog<BitManipDlg>*>(this)->DoModal(pwnd);
}

//-------------------------------------------------------------------
void HexEditorWindow::CMD_character_set()
{
	static_cast<dialog<CharacterSetDlg>*>(this)->DoModal(pwnd);
}

//-------------------------------------------------------------------
void HexEditorWindow::CMD_zoom(int iAmount)
{
	if (iAmount)
		iFontZoom += iAmount;
	else
		iFontZoom = 0;
	if (iFontZoom < 2 - iFontSize)
		iFontZoom = 2 - iFontSize;
	HWindow *pwndFocus = HWindow::GetFocus();
	if (pwnd == pwndFocus)
		kill_focus();
	resize_window();
	if (pwnd == pwndFocus)
		set_focus();
}

//-------------------------------------------------------------------
void HexEditorWindow::CMD_toggle_insertmode()
{
	bInsertMode = !bInsertMode;
	set_wnd_title();
}

//-------------------------------------------------------------------
void HexEditorWindow::CMD_on_backspace()
{
	if (iCurByte <= 0)
		return;
	if (bInsertMode)
	{
		// INSERT-mode: If one exists delete previous byte.
		m_dataArray.RemoveAt(--iCurByte, 1);
		iFileChanged = TRUE;
		bFilestatusChanged = true;
		set_wnd_title();
		resize_window();
	}
	else
	{
		// Only step back.
		int iByteLine = iCurByte-- / iBytesPerLine;
		snap_caret();
		repaint(iByteLine, iCurByte / iBytesPerLine);
	}
}

//-------------------------------------------------------------------
void HexEditorWindow::CMD_select_all()
{
	if (m_dataArray.GetLength() <= 0)
		return;
	bSelected = true;
	iStartOfSelection = 0;
	iEndOfSelection = m_dataArray.GetUpperBound();
	adjust_view_for_selection();
	repaint();
}

//-------------------------------------------------------------------
void HexEditorWindow::make_font()
{
	if (pFont)
		pFont->DeleteObject();
	HSurface *pdc = pwnd->GetDC();
	int nHeight = -MulDiv(iFontSize + iFontZoom, pdc->GetDeviceCaps(LOGPIXELSY), 72);
	pwnd->ReleaseDC(pdc);
	int cset = iCharacterSet == ANSI_FIXED_FONT ? ANSI_CHARSET : OEM_CHARSET;
	static const TCHAR facename[] = _T("Lucida Console");
	pFont = HFont::Create(nHeight, 0, 0, 0, 0, 0, 0, 0, cset, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_DONTCARE, facename);
}

//-------------------------------------------------------------------
void HexEditorWindow::CMD_properties()
{
	TCHAR buf[1000];
	TCHAR *pc;
	_stprintf(buf, GetLangString(IDS_FPROP_NAME));
	GetFullPathName(filename, 500, buf + _tcslen(buf), &pc);
	if (bPartialOpen)
	{
		_stprintf(buf + _tcslen(buf), GetLangString(IDS_FPROP_PARTIAL_OPEN),
			iPartialOffset, iPartialOffset, m_dataArray.GetLength(), m_dataArray.GetLength()/1024);
	}
	else
	{
		_stprintf(buf + _tcslen(buf), GetLangString(IDS_FPROP_FSIZE),
				m_dataArray.GetLength(), m_dataArray.GetLength()/1024);
	}
	_stprintf(buf + _tcslen(buf), GetLangString(IDS_FPROP_HDUMP_LINES),
			iNumlines);
	MessageCopyBox(pwnd, buf, MB_ICONINFORMATION);
}

//-------------------------------------------------------------------
void HexEditorWindow::CMD_compare()
{
	static_cast<dialog<ChooseDiffDlg>*>(this)->DoModal(pwnd);
}

//-------------------------------------------------------------------
void HexEditorWindow::CMD_binarymode()
{
	static_cast<dialog<BinaryModeDlg>*>(this)->DoModal(pwnd);
}

//-------------------------------------------------------------------
//Pabs added stuff for OLE drag-drop
void HexEditorWindow::timer(WPARAM w, LPARAM)
{
	switch(w)
	{
	case MOUSE_OP_DELAY_TIMER_ID:
		start_mouse_operation();
		break;
	case SCROLL_DELAY_TIMER_ID:
		pwnd->KillTimer(SCROLL_DELAY_TIMER_ID);
		pwnd->SetTimer(SCROLL_TIMER_ID, ScrollInterval, NULL);
		bScrollTimerSet = true;
		break;
	case SCROLL_TIMER_ID:
		if (bScrollTimerSet)
		{
			int adjusth = 0, adjustv = 0;
			if (iMouseY >= cyBuffer * cyChar)
			{
				// Lower border reached.
				if (iVscrollPos < iNumlines - cyBuffer)
				{
					iVscrollPos++;
					adjustv = 1;
				}
			}
			else if (iMouseY < cyChar)
			{
				// Upper border reached.
				if (iVscrollPos > 0)
				{
					iVscrollPos--;
					adjustv = 1;
				}
			}

			if (iMouseX >= cxBuffer * cxChar)
			{
				// Right border reached.
				if (iHscrollPos < iGetCharsPerLine() - cxBuffer)
				{
					iHscrollPos++;
					adjusth = 1;
				}
			}
			else if (iMouseX < cxChar)
			{
				// Left border reached.
				if (iHscrollPos > 0)
				{
					iHscrollPos--;
					adjusth = 1;
				}
			}

			if (adjusth)
				adjust_hscrollbar();
			if (adjustv)
				adjust_vscrollbar();
		}
		break;
	}
}

//Pabs inserted
void HexEditorWindow::start_mouse_operation()
{
	pwnd->KillTimer(MOUSE_OP_DELAY_TIMER_ID);
	bMouseOpDelayTimerSet = FALSE;

	int iStartOfSelSetting = iStartOfSelection;
	int iEndOfSelSetting = iEndOfSelection;
	if (iEndOfSelSetting < iStartOfSelSetting)
		swap(iEndOfSelSetting, iStartOfSelSetting);

	//In the selection
	if (enable_drag && bSelected && lbd_pos >= iStartOfSelSetting && lbd_pos <= iEndOfSelSetting)
	{
		DWORD dwEffect;
		CDataObject dataobj;
		CDropSource source;
		FORMATETC fe;
		STGMEDIUM sm;
		//Stick the bit[s] of data in the data object
		if (output_CF_BINARYDATA)
		{
			sm.hGlobal =  GlobalAlloc(GHND|GMEM_DDESHARE, sizeof(UINT) + iEndOfSelSetting - iStartOfSelSetting + 1);
			if (sm.hGlobal)
			{
				if (BYTE *p = (BYTE *)GlobalLock(sm.hGlobal))
				{
					*(UINT*)p = iEndOfSelSetting - iStartOfSelSetting + 1;
					memcpy(p + sizeof(UINT), &m_dataArray[iStartOfSelSetting], iEndOfSelSetting - iStartOfSelSetting + 1);
					GlobalUnlock(sm.hGlobal);
					sm.tymed = TYMED_HGLOBAL;
					sm.pUnkForRelease = NULL;
					fe.cfFormat = CF_BINARYDATA;
					fe.dwAspect = DVASPECT_CONTENT;
					fe.lindex = -1;
					fe.ptd = NULL;
					fe.tymed = TYMED_HGLOBAL;
					dataobj.SetData(&fe, &sm, TRUE);
				}
			}
		}
		if (output_CF_TEXT)
		{
			bool madedata = false;
			int destlen;
			if (output_text_special)
			{
				//The special syntax
				destlen = Text2BinTranslator::iBytes2BytecodeDestLen(
						&m_dataArray[iStartOfSelSetting],
						iEndOfSelSetting-iStartOfSelSetting + 1);
				sm.hGlobal = GlobalAlloc(GHND | GMEM_DDESHARE, destlen);
				if (sm.hGlobal)
				{
					if (char *p = (char *)GlobalLock(sm.hGlobal))
					{
						Text2BinTranslator::iTranslateBytesToBC(p, &m_dataArray[iStartOfSelSetting], iEndOfSelSetting-iStartOfSelSetting+1);
						madedata = true;
					}
					GlobalUnlock(sm.hGlobal);
				}
			}
			else
			{
				int iCopyHexdumpType;
				int iCopyHexdumpDlgStart;
				int iCopyHexdumpDlgEnd;
				//One of the two hexdump types
				if (/*Output like display*/ output_text_hexdump_display)
				{
					iCopyHexdumpType = IDC_EXPORTDISPLAY;
					iCopyHexdumpDlgStart = iStartOfSelSetting / iBytesPerLine * iBytesPerLine;//cut back to the line start
					iCopyHexdumpDlgEnd = iEndOfSelSetting / iBytesPerLine * iBytesPerLine;//cut back to the line start
					destlen = ((iCopyHexdumpDlgEnd - iCopyHexdumpDlgStart) / iBytesPerLine + 1) * (iGetCharsPerLine()+2) + 1;
				}
				else /*Just output hex digits*/
				{
					iCopyHexdumpType = IDC_EXPORTDIGITS;
					iCopyHexdumpDlgStart = iStartOfSelSetting;
					iCopyHexdumpDlgEnd = iEndOfSelSetting;
					destlen = (iEndOfSelSetting - iStartOfSelSetting) * 2 + 3;
				}
				sm.hGlobal = GlobalAlloc (GHND|GMEM_DDESHARE, destlen);
				if (sm.hGlobal)
				{
					if (char *p = (char *)GlobalLock(sm.hGlobal))
					{
						if (CMD_copy_hexdump(0, iCopyHexdumpType, iCopyHexdumpDlgStart, iCopyHexdumpDlgEnd, p, destlen))
							madedata = true;
						GlobalUnlock(sm.hGlobal);
					}
				}
			}
			if (madedata)
			{
				sm.tymed = TYMED_HGLOBAL;
				sm.pUnkForRelease = NULL;
				fe.cfFormat = CF_TEXT;
				fe.dwAspect = DVASPECT_CONTENT;
				fe.lindex = -1;
				fe.ptd = NULL;
				fe.tymed = TYMED_HGLOBAL;
				dataobj.SetData(&fe, &sm, TRUE);
			}
		}
		if (output_CF_RTF)
		{
			sm.hGlobal = RTF_hexdump(iStartOfSelSetting, iEndOfSelSetting);
			if (sm.hGlobal)
			{
				sm.tymed = TYMED_HGLOBAL;
				sm.pUnkForRelease = NULL;
				fe.cfFormat = CF_RICH_TEXT_FORMAT;
				fe.dwAspect = DVASPECT_CONTENT;
				fe.lindex = -1;
				fe.ptd = NULL;
				fe.tymed = TYMED_HGLOBAL;
				dataobj.SetData( &fe, &sm, TRUE );
			}
		}
		dataobj.DisableSetData();
		old_col = old_row = -1;
		dragging = TRUE;
		HRESULT r = DoDragDrop(&dataobj, &source, bReadOnly ? DROPEFFECT_COPY : DROPEFFECT_COPY | DROPEFFECT_MOVE, &dwEffect);
		if (r == DRAGDROP_S_DROP && (dwEffect & DROPEFFECT_MOVE) && dragging)
		{
			m_dataArray.RemoveAt(iStartOfSelSetting, iEndOfSelSetting - iStartOfSelSetting + 1);
			bSelected = false;
			iFileChanged = TRUE;
			bFilestatusChanged = true;
			iCurByte = iStartOfSelSetting;
			resize_window();
		}
		dragging = FALSE;
	}
	else
	{
		int lastbyte = m_dataArray.GetUpperBound();
		if (lastbyte == -1)
			return;
		SetCursor(LoadCursor(NULL, IDC_IBEAM));
		if( old_pos >= lastbyte+1 )
			old_pos = lastbyte;
		else if( old_pos < 0 )
			old_pos = 0;
		if( lbd_pos >= lastbyte+1 )
			lbd_pos = lastbyte;
		else if( lbd_pos < 0 )
			lbd_pos = 0;
		iStartOfSelection = lbd_pos;
		iEndOfSelection = old_pos;
		bSelected = bSelecting = true;
		repaint();
	}
	/*else
	{
		bPullScrolling = TRUE;
	}*/
}
//end
//-------------------------------------------------------------------
void HexEditorWindow::CMD_select_block()
{
	static_cast<dialog<SelectBlockDlg>*>(this)->DoModal(pwnd);
}

//-------------------------------------------------------------------
void HexEditorWindow::update_MRU()
{
	int i = 0;
	while (i < iMRU_count && _tcscmp(strMRU[i], filename) != 0)
		++i;
	if (i == iMRU_count) // Name not yet in list.
		if (iMRU_count < MRUMAX)
			++iMRU_count;
		else
			--i;
	while (int j = i)
		_tcscpy(strMRU[j], strMRU[--i]);
	_tcscpy(strMRU[0], filename); // Put chosen name at top.
	save_ini_data();
}

/**
 * @brief Create MRU file list into File-menu.
 * This method replaces the MRU placeholder items in File menu with actual
 * previously opened files.
 * @param [in] menu Handle to the File-menu.
 */
void HexEditorWindow::make_MRU_list(HMENU menu)
{
	// Remove MRU placeholder items and preceding separator
	int j = GetMenuItemCount(menu) - 2; // index of last MRU item if present
	while (j > 0)
	{
		UINT id = GetMenuItemID(menu, --j);
		if (id >= IDM_MRU1 && id <= IDM_MRU9 || id == 0)
			RemoveMenu(menu, j, MF_BYPOSITION);
		else
			break;
	}
	if (iMRU_count > 0)
	{
		InsertMenu(menu, ++j, MF_SEPARATOR | MF_BYPOSITION, 0, 0);
		for (int i = 0 ; i < iMRU_count ; i++)
		{
			TCHAR buf[MAX_PATH + 1 + 30];
			_stprintf(buf, _T("&%d %s"), i + 1, strMRU[i]);
			InsertMenu(menu, ++j, MF_STRING | MF_BYPOSITION, IDM_MRU1 + i, buf);
		}
	}
}

//-------------------------------------------------------------------
void HexEditorWindow::CMD_MRU_selected(int i)
{
	if (i >= iMRU_count)
		return;

	if (file_is_loadable(strMRU[i]))
	{
		if (!close())
			return;
		load_file(strMRU[i]);
	}
	else
	{
		MessageBox(pwnd, GetLangString(IDS_MRU_REMOVING), MB_ICONERROR);
		--iMRU_count;
		for ( ; i < iMRU_count ; i++)
			_tcscpy(strMRU[i], strMRU[i + 1]);
		save_ini_data();
	}
}

//-------------------------------------------------------------------
void HexEditorWindow::CMD_add_bookmark()
{
	if (m_dataArray.GetLength() <= 0)
	{
		MessageBox(pwnd, GetLangString(IDS_BMK_EMPTY_FILE), MB_ICONERROR);
		return;
	}
	if (iBmkCount >= BMKMAX)
	{
		MessageBox(pwnd, GetLangString(IDS_BMK_MAX_AMOUNT), MB_ICONERROR);
		return;
	}
	static_cast<dialog<AddBmkDlg>*>(this)->DoModal(pwnd);
}

/**
 * @brief Insert the bookmark list into the menu.
 * @param [in] menu Handle to the menu.
 */
void HexEditorWindow::make_bookmark_list(HMENU menu)
{
	do
		;
	while (RemoveMenu(menu, 3, MF_BYPOSITION));

	if (iBmkCount > 0)
	{
		AppendMenu(menu, MF_SEPARATOR, 0, 0);
		TCHAR buf[INFOTIPSIZE];
		for (int i = 0 ; i < iBmkCount ; i++)
		{
			if (pbmkList[i].name.length())
				_stprintf(buf, _T("&%d %s (0x%x)"), i + 1, pbmkList[i].name.c_str(), pbmkList[i].offset);
			else
				_stprintf(buf, _T("&%d 0x%x"), i + 1, pbmkList[i].offset);
			AppendMenu(menu, pbmkList[i].offset <= m_dataArray.GetLength() ?
				MF_ENABLED : MF_GRAYED, IDM_BOOKMARK1 + i, buf);
		}
	}
}

//-------------------------------------------------------------------
void HexEditorWindow::CMD_goto_bookmark(int i)
{
	if (pbmkList[i].offset >= 0 && pbmkList[i].offset <= m_dataArray.GetLength())
	{
		iCurByte = pbmkList[i].offset;
		iCurNibble = 0;
		bSelected = false;
		resize_window();
		adjust_vscrollbar();
	}
	else
	{
		MessageBox(pwnd, GetLangString(IDS_BMK_IS_INVALID), MB_ICONERROR);
	}
}

//-------------------------------------------------------------------
void HexEditorWindow::CMD_remove_bkm()
{
	if (iBmkCount == 0)
	{
		MessageBox(pwnd, GetLangString(IDS_BMK_NONE_TOREMOVE), MB_ICONERROR);
		return;
	}
	static_cast<dialog<RemoveBmkDlg>*>(this)->DoModal(pwnd);
}

/**
 * @brief Remove all bookmarks.
 */
void HexEditorWindow::CMD_clear_all_bmk()
{
	int response = MessageBox(pwnd,
		GetLangString(IDS_BMK_REMOVE_ALL), MB_YESNO | MB_ICONWARNING);
	if (response != IDYES)
		return;
	while (iBmkCount)
	{
		pbmkList[--iBmkCount].name.clear();
	}
	repaint();
}

/**
 * @brief Show the dialog to open file partially.
 */
void HexEditorWindow::CMD_open_partially()
{
	if (!close())
		return;
	TCHAR szFileName[_MAX_PATH];
	szFileName[0] = '\0';
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof ofn);
	ofn.lStructSize = sizeof ofn;
	ofn.hwndOwner = pwnd->m_hWnd;
	ofn.lpstrFilter = GetLangString(IDS_OPEN_ALL_FILES);
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = _MAX_PATH;
	ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT;
	if (!GetOpenFileName(&ofn))
		return;
	OpenPartiallyDlg::filehandle = _topen(szFileName, _O_RDONLY|_O_BINARY);
	if (OpenPartiallyDlg::filehandle == -1)
	{
		TCHAR buf[500];
		_stprintf(buf, GetLangString(IDS_ERR_FILE_OPEN_CODE), errno, szFileName);
		MessageBox(pwnd, buf, MB_ICONERROR);
		return;
	}
	int response = static_cast<dialog<OpenPartiallyDlg>*>(this)->DoModal(pwnd);
	_close(OpenPartiallyDlg::filehandle);
	if (response != IDOK)
		return;
	// If read-only mode on opening is enabled or the file is read only:
	bReadOnly = bOpenReadOnly || -1 == _taccess(szFileName, 02); //Pabs added call to _access
	_tcscpy(filename, szFileName);
	bFileNeverSaved = false;
	bFilestatusChanged = true;
	bFileNeverSaved = false;
	bPartialOpen = true;
	resize_window();
}

//-------------------------------------------------------------------
void HexEditorWindow::CMD_fast_paste()
{
	static_cast<dialog<FastPasteDlg>*>(this)->DoModal(pwnd);
}

//-------------------------------------------------------------------
//Pabs inserted
INT_PTR CALLBACK MultiDropDlgProc(HWND h, UINT m, WPARAM w, LPARAM l)
{
	switch (m)
	{
	case WM_INITDIALOG:
		{
			TranslateDialog(h);
			HWND hwndList = GetDlgItem(h, IDC_DROPPED_FILES);
			SetWindowLongPtr(hwndList, GWL_STYLE, GetWindowLongPtr(hwndList,GWL_STYLE) & ~LBS_SORT | WS_HSCROLL);

			TCHAR file[_MAX_PATH + 1];
			UINT n = DragQueryFile((HDROP)l, 0xFFFFFFFF, NULL, 0 );
			SendMessage (hwndList, LB_INITSTORAGE, n, _MAX_PATH+1);
			for (UINT i = 0 ; i < n ; i++)
			{
				DragQueryFile((HDROP)l, i, file, _MAX_PATH + 1);
				SendMessage(hwndList, LB_INSERTSTRING, i, (LPARAM)file);
			}
			SendMessage(hwndList, LB_SETCURSEL, 0, 0);
			return TRUE;
		}
	case WM_COMMAND:
		switch (w)
		{
		case IDOK:
			EndDialog(h, SendDlgItemMessage(h, IDC_DROPPED_FILES, LB_GETCURSEL, 0, 0));
			return TRUE;
		case IDCANCEL:
			EndDialog(h, SendDlgItemMessage(h, IDC_DROPPED_FILES, LB_GETCOUNT, 0, 0));
			return TRUE;
		}
		break;
	}
	return FALSE;
}
//end

/**
 * @brief Handle file(s) dropped to Frhed window.
 */
void HexEditorWindow::dropfiles(HDROP hDrop)
{
	TCHAR lpszFile[_MAX_PATH];
	UINT numfiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0 );
	UINT i = 0;
	if (numfiles > 1)
	{
		i = ShowModalDialog(IDD_MULTIDND_DIALOG, pwnd, MultiDropDlgProc, hDrop);
		if (i >= numfiles)
		{
			DragFinish(hDrop);
			return;
		}
	}
	DragQueryFile(hDrop, i, lpszFile, _MAX_PATH);
	DragFinish(hDrop); // handle to memory to free
	if (hwndMain)
		SetForegroundWindow(hwndMain);

	if (!close())
		return;

	TCHAR lpszTarget[_MAX_PATH] = {0};
	// Is this a link file?
	HRESULT hres = ResolveIt(lpszFile, lpszTarget);
	TCHAR *lpszFileToOpen = lpszFile;
	if (SUCCEEDED(hres) && _tcslen(lpszTarget) && _tcscmp(lpszFile, lpszTarget) != 0)
	{
		// Trying to open a link file: decision by user required.
		int ret = MessageBox(pwnd,
			GetLangString(IDS_OPEN_SHORTCUT), MB_YESNOCANCEL | MB_ICONWARNING);
		switch (ret)
		{
		case IDYES:
			lpszFileToOpen = lpszTarget;
			break;
		case IDCANCEL:
			return;
		}
	}
	load_file(lpszFileToOpen);
}

//-------------------------------------------------------------------
void HexEditorWindow::CMD_apply_template()
{
	if (m_dataArray.GetLength() == 0)
	{
		MessageBox(pwnd, GetLangString(IDS_ERR_EMPTY_FILE), MB_ICONERROR);
		return;
	}
	// Get name of template file.
	TCHAR szTemplateName[_MAX_PATH];
	szTemplateName[0] = '\0';
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof ofn);
	ofn.lStructSize = sizeof ofn;
	ofn.hwndOwner = pwnd->m_hWnd;
	ofn.lpstrFilter = GetLangString(IDS_TPL_FILE_PATTERN);
	ofn.lpstrFile = szTemplateName;
	ofn.nMaxFile = _MAX_PATH;
	ofn.lpstrTitle = GetLangString(IDS_TPL_CHOOSE_FILE);
	ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT;
	if (GetOpenFileName(&ofn))
	{
		// szTemplateName contains name of chosen tpl file.
		apply_template(szTemplateName);
	}
}

//-------------------------------------------------------------------
void HexEditorWindow::apply_template(TCHAR *pcTemplate)
{
	Template tmpl(m_dataArray);
	tmpl.SetOriginalFilename(filename);
	bool success = tmpl.OpenTemplate(pcTemplate);
	if (!success)
	{
		TCHAR buf[500];
		_stprintf(buf, GetLangString(IDS_TPL_CANNOT_OPEN), pcTemplate);
		MessageBox(pwnd, buf, MB_ICONERROR);
		return;
	}

	success = tmpl.LoadTemplateData();
	if (!success)
	{
		TCHAR buf[500];
		_stprintf(buf, GetLangString(IDS_TPL_CANNOT_OPEN_FROM), pcTemplate);
		MessageBox(pwnd, buf, MB_ICONERROR);
		return;
	}

	tmpl.CreateTemplateArray(iCurByte);
	tmpl.ApplyTemplate(iBinaryMode, iCurByte);

	LPCTSTR pcTmplText = tmpl.GetResult();
	ShowModalDialog(IDD_TMPL_RESULT_DIALOG, pwnd, TmplDisplayDlgProc, const_cast<LPTSTR>(pcTmplText));
}

/**
 * @brief A handler function for Result dialog (templated).
 * This function handles messages for the result dialog. The result dialog
 * is a "template" for which the result text is given as a parameter.
 * @param [in] hDlg Handle to the dialog.
 * @param [in] iMsg Message to handle.
 * @param [in] wParam First message parameter (e.g. control ID).
 * @param [in] lParam Second message parameter (e.g. result text).
 * @return TRUE if the message was handled, FALSE if not.
 */
INT_PTR CALLBACK TmplDisplayDlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		TranslateDialog(hDlg);
		SetDlgItemText(hDlg, IDC_TMPLRESULT_RESULT, (LPCTSTR)lParam);
		return TRUE;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
		case IDCANCEL:
			EndDialog(hDlg, wParam);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

//-------------------------------------------------------------------
void HexEditorWindow::CMD_goto()
{
	static_cast<dialog<GoToDlg>*>(this)->DoModal(pwnd);
}

//-------------------------------------------------------------------
// Resolve link files for opening from command line.
// Copied from compiler documentation.

HRESULT HexEditorWindow::ResolveIt(LPCTSTR lpszLinkFile, LPTSTR lpszPath)
{
	return ::ResolveIt(hwndMain, lpszLinkFile, lpszPath);
}

/**
 * @brief Reset colors to default values.
 */
void HexEditorWindow::CMD_colors_to_default()
{
	if (MessageBox(pwnd, GetLangString(IDS_RESET_COLORS), MB_YESNO | MB_ICONWARNING) == IDYES)
	{
		iBmkColor = RGB( 255, 0, 0 );
		iSelBkColorValue = RGB( 255, 255, 0 );
		iSelTextColorValue = RGB( 0, 0, 0 );
		iTextColorValue = RGB( 0, 0, 0 );
		iBkColorValue = RGB( 255, 255, 255 );
		iSepColorValue = RGB( 192, 192, 192 );
		save_ini_data();
		repaint();
	}
}

void HexEditorWindow::CMD_GotoDllExports()
{
	ULONG ulOffset, ulSize;
	if (GetDllExportNames(filename, &ulOffset, &ulSize))
	{
		bSelected = true;
		iStartOfSelection = (int)ulOffset;
		iEndOfSelection = (int)(ulOffset + ulSize - 1);
		adjust_view_for_selection();
		repaint();
	}
}

void HexEditorWindow::CMD_GotoDllImports()
{
	ULONG ulOffset, ulSize;
	if (GetDllImportNames(filename, &ulOffset, &ulSize))
	{
		bSelected = true;
		iStartOfSelection = (int)ulOffset;
		iEndOfSelection = (int)(ulOffset + ulSize - 1);
		adjust_view_for_selection();
		repaint();
	}
}

void HexEditorWindow::OnContextMenu(LPARAM lParam)
{
	POINT p;
	POINTSTOPOINT(p, lParam);

	if (filename[0] == '\0' || pwnd != HWindow::WindowFromPoint(p))
		return;

	pwnd->ScreenToClient(&p);
	int log_column = p.x / cxChar + iHscrollPos;
	pwnd->ClientToScreen(&p);

	//Click on offsets
	if (log_column < iMaxOffsetLen + iByteSpace)
	{
		if (!bAutoOffsetLen && iMinOffsetLen <= INT_MAX)
		{
			iMinOffsetLen++;
			save_ini_data();
			resize_window();
		}
	}
	else if (HMenu *pMenu = HMenu::LoadMenu(hInstance, MAKEINTRESOURCE(IDR_CONTEXTMENU)))
	{
		// You could use other menu indices based on context... if you like
		if (HMenu *pSubMenu = pMenu->GetSubMenu(Drive ? 1 : 0))
		{
			int i = pSubMenu->GetMenuItemCount();
			while (i)
			{
				if (UINT id = pSubMenu->GetMenuItemID(--i))
				{
					pSubMenu->EnableMenuItem(i, queryCommandEnabled(id) ?
						MF_ENABLED | MF_BYPOSITION : MF_GRAYED | MF_BYPOSITION);
				}
			}
			pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, p.x, p.y, pwnd);
		}
		pMenu->DestroyMenu();
	}
}

void HexEditorWindow::CMD_CloseDrive()
{
	CMD_new();
}

void HexEditorWindow::CMD_DriveGotoFirstTrack()
{
	CurrentSectorNumber = 0;
	RefreshCurrentTrack();
}

void HexEditorWindow::CMD_DriveGotoNextTrack()
{
	if (CurrentSectorNumber < SelectedPartitionInfo->m_NumberOfSectors - 1)
		CurrentSectorNumber++;
	RefreshCurrentTrack();
}


void HexEditorWindow::CMD_DriveGotoPrevTrack()
{
	if (CurrentSectorNumber)
		CurrentSectorNumber--;
	RefreshCurrentTrack();
}

void HexEditorWindow::CMD_DriveGotoTrackNumber()
{
	static_cast<dialog<GotoTrackDialog>*>(this)->DoModal(pwnd);
}

void HexEditorWindow::CMD_DriveGotoLastTrack()
{
	CurrentSectorNumber = SelectedPartitionInfo->m_NumberOfSectors - 1;
	RefreshCurrentTrack();
}

void HexEditorWindow::RefreshCurrentTrack()
{
	if (Drive->ReadAbsolute(Track.GetObjectMemory(), Track.GetObjectSize(),
			CurrentSectorNumber + SelectedPartitionInfo->m_StartingSector))
	{
		ULONG BytesPerSector = Track.GetObjectSize();
		m_dataArray.ClearAll();
		if (m_dataArray.SetSize(BytesPerSector))
		{
			m_dataArray.SetUpperBound(BytesPerSector-1);
			CopyMemory(m_dataArray, Track.GetObjectMemory(), BytesPerSector);
			bReadOnly = TRUE;
			_stprintf(filename, GetLangString(IDS_DRIVES_SECTOR),
					(LPCSTR) SelectedPartitionInfo->GetNameAsString(), CurrentSectorNumber);
			bFileNeverSaved = false;
			iVscrollMax = 0;
			iVscrollPos = 0;
			iHscrollMax = 0;
			iHscrollPos = 0;
			iCurByte = 0;
			iCurNibble = 0;
			iFileChanged = FALSE;
			bFilestatusChanged = true;
			bFileNeverSaved = false;
			bPartialOpen = true;
			resize_window();
		}
	}
}

void HexEditorWindow::CMD_OpenDrive()
{
	if (!close())
		return;
	if (!ostools_HaveAdminAccess())
	{
		//MessageBox(pwnd, GetLangString(IDS_DRIVES_NEED_ADMIN), MB_ICONSTOP);
		TCHAR path[MAX_PATH];
		GetModuleFileName(NULL, path, MAX_PATH);
		TCHAR params[32];
		wsprintf(params, _T("/i%d /c%d"), iInstCount, ID_DISK_OPEN_DRIVE);
		HINSTANCE hi = ShellExecute(NULL, _T("runas"), path, params, NULL, SW_SHOWNORMAL);
		if ((int) hi > HINSTANCE_ERROR)
		{
			reset();
			PostMessage(hwndMain, WM_CLOSE, 0, 0);
		}
		return;
	}
	static_cast<dialog<OpenDriveDialog>*>(this)->DoModal(pwnd);
}

void HexEditorWindow::CMD_EncodeDecode()
{
	static_cast<dialog<EncodeDecodeDialog>*>(this)->DoModal(pwnd);
}

//-------------------------------------------------------------------
// Find next occurance of the current findstring.
void HexEditorWindow::CMD_findnext()
{
	// If there is selected data then make it the data to find.
	if (bSelected)
	{
		// Get start/end offset and length of selection.
		int sel_start, select_len;
		if (iEndOfSelection < iStartOfSelection)
		{
			sel_start = iEndOfSelection;
			select_len = iStartOfSelection - iEndOfSelection + 1;
		}
		else
		{
			sel_start = iStartOfSelection;
			select_len = iEndOfSelection - iStartOfSelection + 1;
		}

		// Translate the selection into bytecode and write it into the edit box buffer.
		int destLen = Text2BinTranslator::iBytes2BytecodeDestLen(&m_dataArray[sel_start], select_len);
		char *tmpBuf = new char[destLen + 1];
		ZeroMemory(tmpBuf, destLen + 1);
		Text2BinTranslator::iTranslateBytesToBC(tmpBuf,
				&m_dataArray[sel_start], select_len);
		m_pFindCtxt->SetText(tmpBuf);
		delete [] tmpBuf;
	}

	// Is there a findstring? (Initmenupopup actually filters this already).
	if (m_pFindCtxt->HasText())
	{
		// There is a findstring. Create its translation.
		BYTE *pcFindstring;
		int srclen = static_cast<int>(strlen(m_pFindCtxt->GetText()));
		if (int destlen = create_bc_translation(&pcFindstring,
			m_pFindCtxt->GetText(), srclen, iCharacterSet, iBinaryMode))
		{
			SetCursor(LoadCursor(NULL, IDC_WAIT));
			int i = findutils_FindBytes(&m_dataArray[iCurByte + 1],
				m_dataArray.GetLength() - iCurByte - 1,
				pcFindstring, destlen, 1,
				m_pFindCtxt->m_bMatchCase);
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			if (i != -1)
			{
				iCurByte += i + 1;
				bSelected = true;
				iStartOfSelection = iCurByte;
				iEndOfSelection = iCurByte + destlen - 1;
				adjust_view_for_selection();
				repaint();
			}
			else
			{
				MessageBox(pwnd, GetLangString(IDS_FIND_NO_MORE), MB_ICONINFORMATION);
			}
			delete [] pcFindstring;
		}
		else
		{
			m_pFindCtxt->m_iDirection = 1;
			CMD_find();
		}
	}
	else
	{
		//Can't call CMD_find cause it won't alloc a new buffer
		// There is no findstring.
		MessageBox(pwnd, GetLangString(IDS_FIND_NO_STRING), MB_ICONERROR);
	}
}

//-------------------------------------------------------------------
// Find previous occurance of the current findstring.
void HexEditorWindow::CMD_findprev()
{
	// If there is selected data then make it the data to find.
	if (bSelected)
	{
		// Get start/end offset and length of selection.
		int sel_start, select_len;
		if( iEndOfSelection < iStartOfSelection )
		{
			sel_start = iEndOfSelection;
			select_len = iStartOfSelection - iEndOfSelection + 1;
		}
		else
		{
			sel_start = iStartOfSelection;
			select_len = iEndOfSelection - iStartOfSelection + 1;
		}

		// Translate the selection into bytecode and write it into the edit box buffer.
		int destLen = Text2BinTranslator::iBytes2BytecodeDestLen(&m_dataArray[sel_start], select_len);
		char *tmpBuf = new char[destLen + 1];
		ZeroMemory(tmpBuf, destLen + 1);
		Text2BinTranslator::iTranslateBytesToBC(tmpBuf, &m_dataArray[sel_start], select_len);
		m_pFindCtxt->SetText(tmpBuf);
		delete [] tmpBuf;
	}
	// Is there a findstring? (Initmenupopup actually filters this already).
	if (m_pFindCtxt->HasText())
	{
		// There is a findstring. Create its translation.
		BYTE *pcFindstring;
		int srclen = static_cast<int>(strlen(m_pFindCtxt->GetText()));
		if (int destlen = create_bc_translation(&pcFindstring,
			m_pFindCtxt->GetText(), srclen, iCharacterSet, iBinaryMode))
		{
			SetCursor(LoadCursor(NULL, IDC_WAIT));
			// Search the array starting at index 0 to the current byte,
			// plus the findstring-length minus 1. If
			// you are somewhere in the middle of the findstring with the caret
			// and you choose "find previous" you usually want to find the beginning
			// of the findstring in the file.
			int i = findutils_FindBytes(&m_dataArray[0],
				min(iCurByte + (destlen - 1), m_dataArray.GetLength()),
				pcFindstring, destlen, -1, m_pFindCtxt->m_bMatchCase);
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			if (i != -1)
			{
				iCurByte = i;
				bSelected = true;
				iStartOfSelection = iCurByte;
				iEndOfSelection = iCurByte + destlen - 1;
				adjust_view_for_selection();
				repaint();
			}
			else
			{
				MessageBox(pwnd, GetLangString(IDS_FIND_NO_MORE), MB_ICONINFORMATION);
			}
			delete [] pcFindstring;
		}
		else
		{
			m_pFindCtxt->m_iDirection = -1;
			CMD_find();
		}
	}
	else
	{
		// There is no findstring.
		MessageBox(pwnd, GetLangString(IDS_FIND_NO_STRING), MB_ICONERROR);
	}
}

//-------------------------------------------------------------------
// Handler for the "Open in text editor" command.
void HexEditorWindow::CMD_summon_text_edit()
{
	if (!close())
		return;
	HINSTANCE hi = ShellExecute(pwnd->m_hWnd, _T("open"), TexteditorName, filename, NULL, SW_SHOWNORMAL);
	if ((int) hi <= HINSTANCE_ERROR)
	{
		MessageBox(pwnd, GetLangString(IDS_ERR_EXT_EDITOR), MB_ICONERROR);
	}
}

/**
 * @brief Handle Window messages.
 * This function handles and routes all Windows messages we want to handle.
 * @param [in] hwnd Handle to the window receiving the message.
 * @param [in] iMsg Message number.
 * @param [in] wParam First message parameter.
 * @param [in] lParam Second message parameter.
 */
LRESULT HexEditorWindow::OnWndMsg(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_CREATE:
		at_window_create(hwnd, hMainInstance);
		break;

	case WM_WINDOWPOSCHANGED:
		if ((reinterpret_cast<WINDOWPOS *>(lParam)->flags & SWP_NOSIZE) == 0)
			resize_window();
		break;

	case WM_GETDLGCODE:
		return DLGC_WANTARROWS | DLGC_WANTTAB | DLGC_WANTCHARS;

	case WM_SETFOCUS:
		set_focus();
		break;

	case WM_KILLFOCUS:
		kill_focus();
		break;

	case WM_LBUTTONDOWN:
		lbuttondown(static_cast<int>(wParam), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;

	case WM_LBUTTONUP:
		lbuttonup(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;

	// GK20AUG2K
	case WM_CONTEXTMENU:
		OnContextMenu(lParam);
		break;

	case WM_MOUSEMOVE:
		mousemove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;

	case WM_MOUSEWHEEL:
		mousewheel(GET_WHEEL_DELTA_WPARAM(wParam));
		break;

	case WM_KEYDOWN:
		keydown(static_cast<int>(wParam));
		break;

	case WM_CHAR:
		character(static_cast<char>(wParam));
		break;

	case WM_VSCROLL:
		vscroll(LOWORD(wParam));
		break;

	case WM_HSCROLL:
		hscroll(LOWORD(wParam));
		break;

	case WM_PAINT:
		paint();
		return 0;

	case WM_COMMAND:
		command(LOWORD(wParam));
		break;

	case WM_NOTIFY:
		notify(reinterpret_cast<NMHDR *>(lParam));
		break;

	case WM_TIMER:
		timer(wParam, lParam);
		break;

	case WM_DESTROY:
		destroy_window();
		break;

	case WM_PARENTNOTIFY:
		switch (LOWORD(wParam))
		{
		case WM_CREATE:
			set_control_bar(reinterpret_cast<HWindow *>(lParam));
			break;
		}
		break;

	case WM_HELP:
		OnHelp(reinterpret_cast<HWindow *>(hwnd));
		break;
	}
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

void HexEditorWindow::OnHelp(HWindow *pWnd)
{
	ShowHtmlHelp(HELP_CONTENTS, 0, pWnd);
}

//Pabs changed - removed CMD_explorersettings

//-------------------------------------------------------------------
void HexEditorWindow::CMD_replace()
{
	static_cast<dialog<ReplaceDlg>*>(this)->DoModal(pwnd);
}

BOOL HexEditorWindow::select_next_diff(BOOL bFromStart)
{
	BOOL bDone = FALSE;
	int length = get_length();
	BYTE *buffer = get_buffer(length);
	int sibling_length = sibling->get_length();
	BYTE *sibling_buffer = sibling->get_buffer(sibling_length);
	int i = bFromStart ? 0 : bSelected ? iGetEndOfSelection() + 1 : iCurByte;
	while (i < length && i < sibling_length && buffer[i] == sibling_buffer[i])
		++i;
	int j = i;
	while (j < length && j < sibling_length && buffer[j] != sibling_buffer[j])
		++j;
	if (i != j)
	{
		iCurByte = i;
		iStartOfSelection = i;
		iEndOfSelection = j - 1;
		bSelected = true;
		adjust_view_for_selection();
		repaint();
		synch_sibling(TRUE);
		bDone = TRUE;
	}
	return bDone;
}

BOOL HexEditorWindow::select_prev_diff(BOOL bFromEnd)
{
	BOOL bDone = FALSE;
	int length = get_length();
	BYTE *buffer = get_buffer(length);
	int sibling_length = sibling->get_length();
	BYTE *sibling_buffer = sibling->get_buffer(sibling_length);
	int i = bFromEnd ? length : iGetStartOfSelection();
	if (i > sibling_length)
		i = sibling_length;
	do
	{
		--i;
	} while (i >= 0 && buffer[i] == sibling_buffer[i]);
	int j = i;
	while (j >= 0 && buffer[j] != sibling_buffer[j])
		--j;
	if (i != j)
	{
		iStartOfSelection = iCurByte = j + 1;
		iEndOfSelection = i;
		bSelected = true;
		adjust_view_for_selection();
		repaint();
		synch_sibling(TRUE);
		bDone = TRUE;
	}
	return bDone;
}

void HexEditorWindow::synch_sibling(BOOL bSynchSelection)
{
	if (sibling != this)
	{
		Status *sibling_status = sibling->get_status();
		Settings *sibling_settings = sibling->get_settings();

		int sibling_length = sibling->get_length();

		int iMin = sibling_status->bSelected ?
			min(sibling_status->iStartOfSelection, sibling_status->iEndOfSelection) :
			sibling_status->iCurByte;
		int iMax = sibling_status->bSelected ?
			max(sibling_status->iStartOfSelection, sibling_status->iEndOfSelection) :
			sibling_status->iCurByte;

		sibling_status->iEnteringMode = iEnteringMode;
		sibling_status->iCurByte = iCurByte < sibling_length ? iCurByte : sibling_length;
		sibling_status->iCurNibble = iCurNibble;

		sibling_status->iVscrollMax = iVscrollMax;
		sibling_status->iVscrollPos = iVscrollPos;
		sibling->adjust_vscrollbar();

		if (bSynchSelection)
		{
			sibling_status->iStartOfSelection = iStartOfSelection;
			sibling_status->iEndOfSelection = iEndOfSelection;
			sibling_status->bSelected = bSelected &&
				(iStartOfSelection < sibling_length || iEndOfSelection < sibling_length);
			if (sibling_status->iStartOfSelection >= sibling_length)
				sibling_status->iStartOfSelection = sibling_length - 1;
			if (sibling_status->iEndOfSelection >= sibling_length)
				sibling_status->iEndOfSelection = sibling_length - 1;
			int iMinNew = sibling_status->bSelected ?
				min(sibling_status->iStartOfSelection, sibling_status->iEndOfSelection) :
				sibling_status->iCurByte;
			int iMaxNew = sibling_status->bSelected ?
				max(sibling_status->iStartOfSelection, sibling_status->iEndOfSelection) :
				sibling_status->iCurByte;
			if (iMin > iMinNew)
				iMin = iMinNew;
			if (iMax < iMaxNew)
				iMax = iMaxNew;
		}
		sibling->repaint(iMin / sibling_settings->iBytesPerLine, iMax / sibling_settings->iBytesPerLine);
	}
}

void HexEditorWindow::CMD_revert()
{
	if (bFileNeverSaved)
	{
		reset();
		return;
	}
	if (bPartialOpen)
	{
		int filehandle = _topen(filename, _O_RDONLY|_O_BINARY);
		if (filehandle != -1)
		{
			_lseeki64(filehandle, iPartialOffset, 0);
			if (m_dataArray.SetSize(iPartialOpenLen))
			{
				if (_read(filehandle, &m_dataArray[0], iPartialOpenLen) != -1)
				{
					bReadOnly = bOpenReadOnly || -1 == _taccess(filename, 02);
					iVscrollMax = iVscrollPos = iHscrollMax = iHscrollPos =
					iVscrollPos = iCurByte = iCurNibble = 0;
					iFileChanged = FALSE;
					bFilestatusChanged = true;
					bFileNeverSaved = false;
					bSelected = false;
					resize_window();
				}
			}
			_close(filehandle);
		}
	}
	else
	{
		load_file(filename);
	}
}

//fill with command
void HexEditorWindow::CMD_fw()
{
	//dlgproc opens file or fills buffer from user input
	static_cast<dialog<FillWithDialog>*>(this)->DoModal(pwnd);
}

void HexEditorWindow::CMD_deletefile()
{
	if (IDYES != MessageBox(pwnd, GetLangString(IDS_ASK_DELETE_FILE), MB_ICONWARNING | MB_YESNO))
		return;
	if (_tremove(filename) != 0)
	{
		MessageBox(pwnd, GetLangString(IDS_ERR_CANNOT_DELETE), MB_ICONERROR);
		return;
	}
	//Remove from MRU
	int i = 0;
	while (i < iMRU_count && _tcscmp(strMRU[i], filename) != 0)
		++i;
	--iMRU_count;
	for ( ; i < iMRU_count ; i++)
		_tcscpy(strMRU[i], strMRU[i + 1]);
	save_ini_data();
	reset();
}

void HexEditorWindow::CMD_insertfile()
{
	TCHAR szFileName[_MAX_PATH];
	szFileName[0] = '\0';
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof ofn);
	ofn.lStructSize = sizeof ofn;
	ofn.hwndOwner = pwnd->m_hWnd;
	ofn.lpstrFilter = GetLangString(IDS_OPEN_ALL_FILES);
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = _MAX_PATH;
	ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT;
	if (!GetOpenFileName(&ofn))
		return;
	// RK: don't allow inserting same file we're editing right now.
	//Pabs removed - the bug appears to have disappeared as a result of my improvements
	int fhandle = _topen(szFileName, _O_RDONLY | _O_BINARY);
	if (fhandle == -1)
	{
		MessageBox(pwnd, GetLangString(IDS_ERR_OPENING_FILE), MB_ICONERROR);
		return;
	}
	int inslen = _filelength(fhandle);
	if (inslen != -1)
	{
		int rs,re,rl;//Remove start, end, len
		if (bSelected)
		{
			rs = iGetStartOfSelection();
			re = iGetEndOfSelection();
			rl = re + 1 - rs;
		}
		else
		{
			rs = iCurByte;
			rl = 0;
		}
		//New & old lens
		int ol = m_dataArray.GetSize();
		int nl = ol + inslen - rl;
		bool rssuc = inslen <= rl || get_buffer(nl); // resize succesful
		if (rssuc)
		{
			BYTE *src = &m_dataArray[rs + rl];
			BYTE *dst = &m_dataArray[rs + inslen];
			int count = ol - (rs + rl);
			if (inslen > rl) // bigger
				memmove(dst, src, count);
			bool rdsuc = _read(fhandle, &m_dataArray[rs], inslen) != -1; //read successful

			//In the following two if blocks m_dataArray.SetUpperBound(somelen-1);
			//is used instead of m_dataArray.SetSize(somelen);
			//to prevent the possiblity that shortening the
			//buffer might fail (way too hard to handle) because SimpleArray currently
			//uses new & delete instead of malloc & free, which,
			//together with realloc could get around this problem easily
			if (inslen < rl && rdsuc)
			{
				memmove(dst, src, count);
				get_buffer(nl);
			}
			else if (inslen > rl && !rdsuc)
			{
				memmove(src, dst, count);
				get_buffer(ol);
			}
			if (rdsuc)
			{
				// RK: Added a call to update_for_new_datasize().
				if (inslen)
				{
					iStartOfSelection = rs;
					iEndOfSelection = rs+inslen-1;
				}
				else if (bSelected)
				{
					iCurByte = rs;
					iCurNibble = 0;
				}

				if (inslen || bSelected)
				{
					iFileChanged = TRUE;
					bFilestatusChanged = true;
					bSelected = inslen != 0;
					resize_window();
				}
			}
			else
			{
				MessageBox(pwnd, GetLangString(IDS_ERR_CANNOT_INSERT), MB_ICONERROR);
			}
		}
	}
	else
	{
		MessageBox(pwnd, GetLangString(IDS_ERR_CHECK_FILESIZE), MB_ICONERROR);
	}
	_close(fhandle);
}

void HexEditorWindow::CMD_saveselas()
{
	TCHAR szFileName[_MAX_PATH];
	szFileName[0] = '\0';
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof ofn);
	ofn.lStructSize = sizeof ofn;
	ofn.hwndOwner = pwnd->m_hWnd;
	ofn.lpstrFilter = GetLangString(IDS_OPEN_ALL_FILES);
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = _MAX_PATH;
	ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT;
	if (GetSaveFileName(&ofn))
	{
		LPCTSTR complain = GetLangString(IDS_FILE_SAVE_ERROR);
		int filehandle = _topen(szFileName, _O_RDWR | _O_CREAT |
				_O_TRUNC | _O_BINARY, _S_IREAD | _S_IWRITE);
		if (filehandle != -1)
		{
			WaitCursor wc;
			int lower = 0;
			int upper = m_dataArray.GetUpperBound();
			if (bSelected)
			{
				lower = iGetStartOfSelection();
				upper = iGetEndOfSelection();
			}
			if (_write(filehandle, &m_dataArray[lower], upper - lower + 1) != -1)
				complain = 0;
			_close(filehandle);
		}
		if (complain)
		{
			MessageBox(pwnd, complain, MB_ICONERROR);
		}
	}
	repaint();
}

//-------------------------------------------------------------------
int HexEditorWindow::iGetCharsPerLine()
{
	return iMaxOffsetLen + iByteSpace + iBytesPerLine * 3 + iCharSpace + iBytesPerLine;
}

//-------------------------------------------------------------------
int HexEditorWindow::iGetStartOfSelection()
{
	return bSelected ? min(iStartOfSelection, iEndOfSelection) : iCurByte;
}

//-------------------------------------------------------------------
int HexEditorWindow::iGetEndOfSelection()
{
	return bSelected ? max(iStartOfSelection, iEndOfSelection) : iCurByte;
}

bool HexEditorWindow::load_hexfile(hexfile_stream &hexin)
{
	WaitCursor wc;
	//Variables used below & their functions
	int typ = 0;//type of file (0=just hex digits)
	//Check the type of file - if only whitespace & hex then just hex else can be used to set line len etc
	//There is probably a better way to do this
	int temp;
	while ((temp = hexin.lhgetc()) != EOF)
	{
		BYTE ct = (BYTE)temp;
		if (!(isspace(ct) || isxdigit(ct)))
		{
			typ = 1;
			break;
		}
	}
	if (hexin.lhtell() == 0)
	{
		MessageBox(pwnd, GetLangString(IDS_ERR_NO_DATA), MB_ICONERROR);
		return false;
	}
	hexin.lhseek(0);

	// Select the format
	TCHAR msg[150];
	_tcscpy(msg, GetLangString(IDS_HEXF_OPEN_FORMAT1));
	_tcscat(msg, typ ?
		GetLangString(IDS_HEXF_OPEN_FORMAT2) :
		GetLangString(IDS_HEXF_OPEN_FORMAT3));

	switch (MessageBox(pwnd, msg, MB_YESNOCANCEL))
	{
	case IDYES:
		return load_hexfile_1::StreamIn(*this, hexin); //Display output
	case IDNO:
		return load_hexfile_0::StreamIn(*this, hexin); //just hex & space
	}
	return FALSE;
}

/**
 * @brief Open hex dump to the editor.
 */
void HexEditorWindow::CMD_open_hexdump()
{
	if (!close())
		return;

	HGLOBAL hClipMemory = 0;

	//Check if clipboard may be used
	if (OpenClipboard(NULL))
	{
		hClipMemory = GetClipboardData(CF_TEXT);
		//Check if user wants to use clipboard
		if (hClipMemory)
		{
			switch (MessageBox(pwnd, GetLangString(IDS_IMPORT_FROM_CB), MB_YESNOCANCEL))
			{
			case IDCANCEL:
				CloseClipboard();
				return;
			case IDNO:
				CloseClipboard();
				hClipMemory = 0;
				break;
			}
		}
		else
		{
			CloseClipboard();
		}
	}
	bool done = false;
	if (hClipMemory)
	{
		//Import from clipboard
		if (char *hexin = (char *)GlobalLock(hClipMemory))
		{
			done = load_hexfile(chexfile_stream(hexin));
			GlobalUnlock(hClipMemory);
		}
		else
		{
			MessageBox(pwnd, GetLangString(IDS_CANNOT_GET_CB_TEXT), MB_ICONERROR);
		}
		CloseClipboard();
	}
	else
	{
		//Import from file
		//Initialize the struct
		TCHAR szFileName[_MAX_PATH];
		szFileName[0] = '\0';
		OPENFILENAME ofn;
		ZeroMemory(&ofn, sizeof ofn);
		ofn.lStructSize = sizeof ofn;
		ofn.hwndOwner = pwnd->m_hWnd;
		ofn.lpstrFilter = GetLangString(IDS_HDUMP_FILE_PATTERN);
		ofn.lpstrFile = szFileName;
		ofn.nMaxFile = _MAX_PATH;
		ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT;
		if (!GetOpenFileName(&ofn))
			return;
		//Set up variables for the function
		FILE *f = _tfopen(szFileName, _T("rb"));
		if (f == 0)
		{
			MessageBox(pwnd, GetLangString(IDS_ERR_CANNOT_GET_TEXT), MB_ICONERROR);
			return;
		}
		done = load_hexfile(fhexfile_stream(f));
		fclose(f);
		if (done)
			bReadOnly = -1 == _taccess(szFileName, 02);
	}
	if (done)
	{
		//Successful
		_tcscpy(filename, GetLangString(IDS_UNTITLED));
		bPartialOpen = bPartialStats = false;
		iFileChanged =
			iVscrollMax = iVscrollPos = iHscrollMax = iHscrollPos =
			iCurByte = iCurNibble = 0;
		bSelected = false;
		bFileNeverSaved = true;
		bFilestatusChanged = true;
		// If read-only mode on opening is enabled:
		if (bOpenReadOnly)
			bReadOnly = TRUE;
		save_ini_data();
		resize_window();
	}
}

//--------------------------------------------------------------------------------------------
// Receives WM_NOTIFY messages and passes either them to their handler functions or
// processes them here.
void HexEditorWindow::status_bar_click(bool left)
{
	//User clicked the status bar

	//Find where the user clicked
	DWORD pos = GetMessagePos();
	POINT p;
	POINTSTOPOINT(p, pos);

	//Find which status rect the user clicked in
	RECT rt;
	int i, n = -1;
	pwndStatusBar->ScreenToClient(&p);
	int np = pwndStatusBar->GetParts();
	for (i = 0 ; i < np ; i++)
	{
		pwndStatusBar->GetPartRect(i, &rt);
		if (PtInRect(&rt, p))
		{
			n = i;
			break;//Can't be in > 1 rect at a time
		}
	}

	//Som variables used below
	int r, len, cn;
	String text;
	SIZE s;
	HGdiObj *fon[2];
	HSurface *pdc;

	//Which rect are we in?
	if (n == 0 || n == 1)
	{
		//In one of the rects that requires the text & fonts
		//Initialize
		text = pwndStatusBar->GetPartText(n);
		len = text.length();
		//This font stuff plagued me for ages but know that you
		//need to put the right font in the dc as it won't have the right one
		//after a GetDC - the status bar must put it in during a paint
		pdc = pwndStatusBar->GetDC();
		fon[0] = pwndStatusBar->GetFont();
		fon[1] = pdc->SelectObject(fon[0]);
		pdc->GetTextExtent(text.c_str(), text.length(), &s);
	}

	//Do the requested function
	switch(n)
	{
	case 0:
		{
			//The user clicked in the 1st part - offset/bits/byte/word/dword
			//If there is a selection
			if (bSelected)
			{
				//Maybe the user wants to change the selection/deselect
				p.y = s.cx;
				pdc->GetTextExtent(text.c_str(), 9, &s);//len "Selected:" = 9
				if (p.x > rt.left + p.y || p.x < rt.left + s.cx)
				{
					//In the space / on the "Selected:" bit - deselect
					if (left)
					{
						iCurNibble = 0;
						iCurByte = iGetStartOfSelection();
					}
					else
					{
						iCurNibble = 1;
						iCurByte = iGetEndOfSelection();
					}
					bSelected = false;
					repaint();
				}
				else
				{ //In offsets - reselect
					CMD_select_block();//This fixes up the status text by itself
				}
				break;//Skip to the deinitialisation code
			}

			//If the caret is on the END byte
			int flen = m_dataArray.GetLength();
			if (iCurByte >= flen)
			{
				//The caret is on the END (__ ) byte
				//Just change offset/go to prev byte
				if (flen)
				{
					if (left)
					{
						CMD_goto();
					}
					else
					{
						iCurByte--;
						repaint();
					}
				}
				//or append (if file is zero size)
				else if (!bReadOnly && !bSelected)
				{
					CMD_edit_append();
				}
				break;//Skip to the deinitialisation code
			}

			//Get which bit the click is in
			//offset, bits, space, un/signed, values

			//Find the start of the un/signed bit
			TCHAR *st = text.pointer();
			for (i = 0 ; i < len ; i++)
			{
				if (st[i] == '\t' && st[i + 1] == '\t')
				{
					st[i] = '\0';//Clip the string
					st += 2;//Remember the second part
					break;
				}
			}

			//There was no '\t\t' in the middle of the string - something went wrong
			if (text == st)
				break;

			//Get the start of the un/signed bit in client coords
			len = static_cast<int>(_tcslen(st));
			pdc->GetTextExtent(st, len, &s);
			r = rt.right - s.cx;

			//Set up stuff for GetTextExtentExPoint
			if (r < p.x)
			{
				r = p.x - r;
				//Already know where the string starts
				//and how long it is
			}
			else
			{
				r = p.x - rt.left;
				st = text.pointer();
				len = static_cast<int>(_tcslen(st));
			}

			//Find which character the used clicked in.
			pdc->GetTextExtentExPoint(st, len, r, &cn, NULL, &s);

			//In the right or left aligned area?
			if (st != text.c_str())
			{
				//We're in the right aligned area

				//Find the end of the Un/signed section
				for (i = 0; i < len && st[i] != ':'; i++);
				//Where are we?
				if (cn <= i)
				{
					//We're in the un/signed area
					//Switch from Unsigned <--> Signed
					bUnsignedView = !bUnsignedView;
					//Fix up the status text
					set_wnd_title();
				}
				else if (!bReadOnly && !bSelected)
				{
					//We're in the values

					//Determine the part in which you clicked - BYTE/WORD/DWORD
					//Lots of assumptions here
					EnterDecimalValueDlg::iDecValDlgSize = 1;
					//Start from where the above loop stopped
					//If clicked this character stop
					for ( ; i < len && cn != i ; i++)
						if (st[i] == ',')
							EnterDecimalValueDlg::iDecValDlgSize <<= 1;
					//Enter decimal value
					CMD_edit_enterdecimalvalue();
				}//In the values
			}//In right aligned stuff
			else
			{
				//We're not in the right aligned stuff

				//Are we in the left aligned stuff?
				if (p.x > rt.left + s.cx + 2)
				{
					//We're in the space between left & right sides
					//Select block, goto or internal status
					if (left)
					{
						CMD_select_block();
					}
#ifdef _DEBUG
					/*In debug mode do the internal status thing
					if ctrl/alt/shift keys are down*/
					else if (GetAsyncKeyState(VK_SHIFT) & 0x8000 ||
						GetAsyncKeyState(VK_CONTROL) & 0x8000 ||
						GetAsyncKeyState(VK_MENU) & 0x8000)
					{
						command(IDM_INTERNALSTATUS);
					}
#endif
					else
					{
						CMD_goto();
					}
				}
				else
				{
					//We're in the left aligned stuff
					//Find the end of the offset
					for (i = 0; i < len && !(st[i] == ' ' && st[i + 1] == ' ' && st[i + 2] == ' ') ; i++);

					//Find if we're in the offset
					if (cn <= i + 1)
					{
						//In the Offset section
						//Goto or View settings or something else
						if (left)
							CMD_goto();
						else
							CMD_view_settings();
					}
					else if (!bReadOnly && !bSelected)
					{
						//In bits section
						//Find the end of " Bits="
						while (i < len && st[i] != '=')
							++i;

						//Are we in the actual bits or " Bits="
						if (cn <= i)
						{
							//In " Bits="
							CMD_manipulate_bits();
						}
						else
						{
							//In actual bits
							//This bit uses the old method because it is more accurate
							//Init for the below loops
							st = text.pointer() + i + 1;
							len = i + 1;
							pdc->GetTextExtent(text.c_str(), len, &s);
							r = rt.left + s.cx;
							//Flip the equivalent of a "bit"
							for (i = 0 ; i < 7 ; i++)
							{
								pdc->GetTextExtent(st, i + 1, &s);
								if (p.x <= r + s.cx + 1)
									break;
							}
							st[i] ^= '0' ^ '1';
							//Assemble all the "bits"
							BYTE c = 0x00;
							for (i = 0 ; i < 8 ; i++)
							{
								c <<= 1;
								if (st[i] == '1')
									c |= 0x01;
							}
							m_dataArray[iCurByte] = c;
							//Redraw the data & status bar etc
							bFilestatusChanged = true;
							iFileChanged = TRUE;
							repaint(iCurByte / iBytesPerLine);

						}//In actual bits
					}//In bits section
				}//In the left aligned stuff
			}//Not in the right aligned stuff
			break;
		}//In the first rect


	case 1:
		{
			//The user clicked in the 2nd part
			int area = 0;//Part - ANSI/OEM, READ/INS/OVR or L/B

			//Start of the string is at r - centered
			r = (rt.left + rt.right - s.cx) / 2;

			//Find the one we clicked in using text functions
			if (p.x > r)
			{
				//Find which character the user clicked in.
				r = p.x - r;
				pdc->GetTextExtentExPoint(text.c_str(), len, r, &cn, NULL, &s);
				//Find the part that the user clicked in.
				//If pos is in this character stop
				for (i = 0 ; i < len && i != cn ; i++)
				{
					//Increment the part
					if (text[i] == '/')
						area++;
				}
			}

			//Do what the user asked for
			switch (area)
			{
			case 0: //ANSI <--> OEM
				if (iCharacterSet == ANSI_FIXED_FONT)
					iCharacterSet = OEM_FIXED_FONT;
				else if (iCharacterSet == OEM_FIXED_FONT)
					iCharacterSet = ANSI_FIXED_FONT;
				save_ini_data();
				resize_window();
				break;
			case 1: //READ/INS/OVR
				if (left) //READ -> INS -> OVR -> READ...
				{
					// Cannot switch drive to edit mode
					if (Drive == 0)
					{
						if (bReadOnly)
						{
							bReadOnly = FALSE;
							bInsertMode = true;
						}
						else if (bInsertMode)
							bInsertMode = false;
						else
							bReadOnly = TRUE;
					}
				}
				else //READ <- INS <- OVR <- READ...
				{
					// Cannot switch drive to edit mode
					if (Drive == 0)
					{
						if (bReadOnly)
						{
							bReadOnly = FALSE;
							bInsertMode= false;
						}
						else if (bInsertMode)
							bReadOnly = TRUE;
						else
							bInsertMode = true;
					}
				}
				break;
			case 2: // L <--> B
				if (iBinaryMode == ENDIAN_LITTLE)
					iBinaryMode = ENDIAN_BIG;
				else if (iBinaryMode == ENDIAN_BIG)
					iBinaryMode = ENDIAN_LITTLE;
				break;
			}//switch ANSI/READ/L

			//Fix up the status text
			//Since set_focus does this too don't call unless have too
			if (area != 0)
				set_wnd_title();

			break;
		}//2nd rect


	case 2: //File size part
		//The user clicked in the third part - file size part - what should we do here?
		//Pabs thinking append, file properties (call shell/frhed) or something else?
		if (left && !bReadOnly && !bSelected)
			CMD_edit_append();
		else
			CMD_properties();
		/*or use OleCreatePropertyFrame to show the property sheet for that file
		or use the undocumented function below
		SHObjectProperties uFlags
		#define OPF_PRINTERNAME 0x01
		#define OPF_PATHNAME 0x02
		WINSHELLAPI BOOL WINAPI SHObjectProperties(HWND hwndOwner,UINT uFlags,LPCSTR lpstrName,LPCSTR lpstrParameters);*/
		break;
		
	}//switch( which part )

	if (n == 0 || n == 1)
	{
		//Deinitialize
		pdc->SelectObject(fon[1]);
		pwndStatusBar->ReleaseDC(pdc);
	}
}

/**
 * @brief Adopt the OS color scheme.
 */
void HexEditorWindow::CMD_adopt_colours()
{
	if (MessageBox(pwnd, GetLangString(IDS_ADOBT_OS_COLORS), MB_YESNO | MB_ICONWARNING) == IDYES)
	{
		iTextColorValue = GetSysColor(COLOR_WINDOWTEXT);
		iBkColorValue = GetSysColor(COLOR_WINDOW);
		iSelTextColorValue = GetSysColor(COLOR_HIGHLIGHTTEXT);
		iSelBkColorValue = GetSysColor(COLOR_HIGHLIGHT);
//		What should these be?
//		iSepColorValue = RGB( 192, 192, 192 );
//		iBmkColor = RGB( 255, 0, 0 );
		save_ini_data();
		repaint();
	}
}

//Reverse the bytes between (and including) a & b
void reverse_bytes(BYTE *a, BYTE *b)
{
	while (a < b)
	{
		BYTE t = *a;
		*a = *b;
		*b = t;
		++a;
		--b;
	}
}

void HexEditorWindow::CMD_move_copy(bool redraw)
{
	static_cast<dialog<MoveCopyDlg>*>(this)->DoModal(pwnd);
}

void HexEditorWindow::CMD_move_copy(int iMove1stEnd, int iMove2ndEndorLen, bool redraw)
{
	/*Call like so
	iMove1stEnd = position of start of block to move;
	iMove2ndEndorLen = position of end of block to move;
	CMD_move(1);
	*/
	int clen = m_dataArray.GetLength();
	//Make sure all the parameters are correct
	if (iMove1stEnd < 0 || iMove1stEnd >= clen ||
		iMove2ndEndorLen < 0 || iMove2ndEndorLen >= clen ||
		iMovePos < 0)
	{
		return;
	}
	if (iMove1stEnd > iMove2ndEndorLen)
		swap(iMove1stEnd, iMove2ndEndorLen);
	int dist = iMovePos - iMove1stEnd;
	if (iMoveOpTyp == OPTYP_COPY)
	{
		if (iMovePos > clen)
			return;
	}
	else if (iMoveOpTyp == OPTYP_MOVE)
	{
		if (dist == 0)
			return;
		if (iMove2ndEndorLen + dist >= clen)
			return;
	}

	WaitCursor wc;

	if (iMoveOpTyp == OPTYP_COPY)
	{
		int len = iMove2ndEndorLen - iMove1stEnd + 1;
		if (!m_dataArray.SetSize(clen + len))
		{
			MessageBox(pwnd, GetLangString(IDS_NO_MEMORY), MB_ICONERROR);
			return;
		}
		else
		{
			m_dataArray.ExpandToSize();
			memmove(&m_dataArray[iMovePos + len], &m_dataArray[iMovePos], clen - iMovePos);
			if (iMovePos > iMove1stEnd && iMovePos <= iMove2ndEndorLen)
			{
				memcpy(&m_dataArray[iMovePos], &m_dataArray[iMove1stEnd], iMovePos - iMove1stEnd);
				memcpy(&m_dataArray[iMovePos + iMovePos - iMove1stEnd], &m_dataArray[iMovePos + len], iMove1stEnd + len - iMovePos);
			}
			else
			{
				int tmp = iMovePos <= iMove1stEnd ? len : 0;
				memcpy(&m_dataArray[iMovePos], &m_dataArray[iMove1stEnd + tmp], len);
			}
		}
	}
	else if (iMoveOpTyp == OPTYP_MOVE)
	{
		//Before we just made a copy of the block & inserted it after moving the rest into place
		//Now (thanx to a whole bunch of dice) I found out that 3 calls to reverse_bytes will do
		//it doesn't require extra memory - yay, but may take a long time if moving long distances

		//It doesn't matter what order these three calls are in, it works
		//in all cases but the values will need to be calculated differently
		//Start and end of the block we are moving
		int ms = iMove1stEnd;
		int me = iMove2ndEndorLen;
		//Start and end of the other block that is affected
		int os = iMovePos > iMove1stEnd ? iMove2ndEndorLen + 1 : iMovePos;
		int oe = iMovePos > iMove1stEnd ? iMove2ndEndorLen + dist : iMove1stEnd - 1;
		//Start and end of the total block
		int ts = min(ms, os);
		int te = max(me, oe);
		reverse_bytes(&m_dataArray[ms], &m_dataArray[me]);
		reverse_bytes(&m_dataArray[os], &m_dataArray[oe]);
		reverse_bytes(&m_dataArray[ts], &m_dataArray[te]);
	}

	if (bSelected)
	{
		//If the selection was inside the bit that was moved move it too
		if (iMove1stEnd <= iStartOfSelection && iMove1stEnd <= iEndOfSelection &&
			iMove2ndEndorLen >= iStartOfSelection && iMove2ndEndorLen >= iEndOfSelection)
		{
			iStartOfSelection += dist;
			iEndOfSelection += dist;
		}
		else
		{
			bSelected = false;
			iCurByte = iStartOfSelection + dist;
		}//If the above is not true deselect - this may change when multiple selections are allowed
	}
	//Or if the current byte was in the move/copied bit move it too
	else if (iCurByte >= iMove1stEnd && iCurByte <= iMove2ndEndorLen)
	{
		iCurByte += dist;
	}

	iFileChanged = TRUE;
	bFilestatusChanged = true;
	if (redraw)
	{
		if (iMoveOpTyp == OPTYP_COPY)
			resize_window();
		else if (iMoveOpTyp == OPTYP_MOVE)
			repaint();
	}
}

void HexEditorWindow::CMD_reverse()
{
	static_cast<dialog<ReverseDlg>*>(this)->DoModal(pwnd);
}

int HexEditorWindow::CMD_setselection(int iSelStart, int iSelEnd) // Mike Funduc new function
{
	if (iSelStart >= 0 && iSelEnd >= iSelStart)
	{
		iStartOfSelection = iSelStart;
		iEndOfSelection = iSelEnd;
		bSelected = true;
		adjust_view_for_selection();
		repaint();
		return 1;
	}
	return 0;
}

HGLOBAL HexEditorWindow::RTF_hexdump(int start, int end, SIZE_T *plen)
{
	/*
	Similar to ostrstream, but homegrown & uses GlobalRealloc & GlobalFree
	can accept strings and integers, repeat strings and filter strings for
	characters to be converted to rtf escape sequences - like \ -> \\, { -> \{,  } -> \}...
	it will relinquish all responsibility for its data if you ask it to
	it does its data allocation in chunks to improve speed & frees excess on relinquish
	*/
	HGlobalStream s;//(1024);//That number is the chunk size to use when reallocating
	s <<
	//The whole of the RTF output code is formatted according to the structure of RTF
	"{\\rtf1\n"
		"\\ansi\n"
		"\\deff0\n"
		"{\\fonttbl\n"
			"{\\f0 ";
				//Get the charactersitics of the display font
				BYTE PitchAndFamily, CharSet;
				char *FaceName = NULL;

				HSurface *pdc = pwnd->GetDC();
				HGdiObj *pFontOld = pdc->SelectObject(pFont);
				OUTLINETEXTMETRICA *otm = NULL;
				if (UINT cbData = pdc->GetOutlineTextMetricsA(0, NULL))
				{
					otm = (OUTLINETEXTMETRICA *)malloc(cbData);
					if (otm)
					{
						ZeroMemory(otm, cbData);
						pdc->GetOutlineTextMetricsA(cbData, otm);
					}
				}
				pdc->SelectObject(pFontOld);
				pwnd->ReleaseDC(pdc);

				if (otm)
				{
					FaceName = (char*)otm + (UINT)otm->otmpFaceName;
					PitchAndFamily = otm->otmTextMetrics.tmPitchAndFamily;
					CharSet = otm->otmTextMetrics.tmCharSet;
				}
				else
				{
					LOGFONT lf;
					pFont->GetLogFont(&lf);
					PitchAndFamily = lf.lfPitchAndFamily;
					CharSet = lf.lfCharSet;
					if (UINT cbData = pdc->GetTextFaceA(0, NULL))
					{
						FaceName = (char*)malloc(cbData);
						if (FaceName)
							pdc->GetTextFaceA(cbData, FaceName);
					}
				}

				//Output the font family,
				switch (PitchAndFamily & /* bits 4-7<<4 */ 0xf0)
				{
					case FF_DECORATIVE: s << "\\fdecor "; break;
					case FF_DONTCARE:
					case FF_MODERN: s << "\\fmodern "; break;
					case FF_ROMAN: s << "\\froman "; break;
					case FF_SCRIPT: s << "\\fscript "; break;
					case FF_SWISS: s << "\\fswiss "; break;
					default: s << "\\fnil "; break;
					/*These have no equivalents in Win32 (or maybe it is my ancient M$DN library)
					case FF_TECH: s << "\\ftech "; break;
					case FF_BIDI: s << "\\fbidi "; break;
					*/
				}
				/*The following have no equivalents in Win32 (or maybe it is my ancient M$DN library)
				#define RTF_FCHARSET_INVALID 3
				#define RTF_FCHARSET_VIETNAMESE 163
				#define RTF_FCHARSET_ARABIC_TRADITIONAL 179
				#define RTF_FCHARSET_ARABIC_USER 180
				#define RTF_FCHARSET_HEBREW_USER 181
				#define RTF_FCHARSET_PC_437 254
				*/
				s <<
				//the character set,
				"\\fcharset" << (DWORD)CharSet << " " <<
				//the pitch type,
				"\\fprq" << (DWORD)(PitchAndFamily&0x3) << " ";
				if (otm)
				{
					s << "{\\*\\panose " <<
						hex << (otm->otmPanoseNumber.bFamilyType) <<
						hex << (otm->otmPanoseNumber.bSerifStyle) <<
						hex << (otm->otmPanoseNumber.bWeight) <<
						hex << (otm->otmPanoseNumber.bProportion) <<
						hex << (otm->otmPanoseNumber.bContrast) <<
						hex << (otm->otmPanoseNumber.bStrokeVariation) <<
						hex << (otm->otmPanoseNumber.bArmStyle) <<
						hex << (otm->otmPanoseNumber.bLetterform) <<
						hex << (otm->otmPanoseNumber.bMidline) <<
						hex << (otm->otmPanoseNumber.bXHeight) <<
					"}";
				}
				//and the name of the font
				if (FaceName && FaceName[0])
					s << escapefilter << FaceName;
				if (otm)
					free(otm);
				else if(FaceName)
					free(FaceName);
				s <<

				/*Possible future stuff
				Embedded font data
				but why bother?
				since RTF sux anyway*/

			"}\n" // \f0

			//Font for information paragraphs
			"{\\f1 "
				"\\froman \\fcharset0 \\fprq2 Times New Roman"
			"}\n" // \f1

		"}\n" // \fonttbl

		"{\\colortbl\n" //The colour table is referenced by the document to change colours for various things
			/*Back colour*/"\\red" << (DWORD)GetRValue(iBkColorValue) << " \\green" << (DWORD)GetGValue(iBkColorValue) << " \\blue" << (DWORD)GetBValue(iBkColorValue) << ";\n"
			/*Text colour*/"\\red" << (DWORD)GetRValue(iTextColorValue) << " \\green" << (DWORD)GetGValue(iTextColorValue) << " \\blue" << (DWORD)GetBValue(iTextColorValue) << ";\n";
			if (bSelected)
			{
				s << //Regular selection colours
				/*Sel bck col*/"\\red" << (DWORD)GetRValue(iSelBkColorValue) << " \\green" << (DWORD)GetGValue(iSelBkColorValue) << " \\blue" << (DWORD)GetBValue(iSelBkColorValue) << ";\n"
				/*Sel txt col*/"\\red" << (DWORD)GetRValue(iSelTextColorValue) << " \\green" << (DWORD)GetGValue(iSelTextColorValue) << " \\blue" << (DWORD)GetBValue(iSelTextColorValue) << ";\n";
			}
			else
			{ //Caret is the text colour inverted
				//Wish I could do iBkColorValueTmp = InvertColour(iBkColorValue)
				iBkColorValue=~iBkColorValue;iTextColorValue=~iTextColorValue; s <<
				/*Car bck col*/"\\red" << (DWORD)GetRValue(iBkColorValue) << " \\green" << (DWORD)GetGValue(iBkColorValue) << " \\blue" << (DWORD)GetBValue(iBkColorValue) << ";\n"
				/*Car txt col*/"\\red" << (DWORD)GetRValue(iTextColorValue) << " \\green" << (DWORD)GetGValue(iTextColorValue) << " \\blue" << (DWORD)GetBValue(iTextColorValue) << ";\n";
				iBkColorValue=~iBkColorValue;iTextColorValue=~iTextColorValue;
			}
			s <<
			/*Bookmarks  */"\\red" << (DWORD)GetRValue(iBmkColor) << " \\green" << (DWORD)GetGValue(iBmkColor) << " \\blue" << (DWORD)GetBValue(iBmkColor) << ";\n"
			//Separators */iSepColorValue is not needed because drawing objects specify their own colours (stupid eh?)
		"}\n" // \colortbl

		//This is new for RTF 1.7, but it should be ignored by older readers so who cares (older than M$ Word XP = Word 2002??)
		"{\\*\\generator frhed v"_CRT_STRINGIZE(FRHED_VERSION_3)";}\n"

		//Metadata here too?
		"{\\info\n"
			//Put the filename in the title
			"{\\title " << escapefilter << static_cast<T2A>(filename) << "}\n"
			//...
		"}\n"; // \info

		//Document formatting properties
			//Sot sure if this will have any effect
			if (bMakeBackups)
				s << "\\makebackup ";
			s <<
			//Turn off spelling & grammar checking in our hexdump
			"\\deflang1024 \\deflangfe1024 \\noproof "
			//Indicate that this document was created from text
			"\\fromtext "
			/*We use the 'Web/Online Layout View' at 100% cause it is good
			for drawing our vertical lines & offers minimal visual fluff*/
			"\\viewkind5 \\viewscale100"
			//...
		"\n" // DFPs

		//Section formatting properties
			//...
		//"\n"  SFPs - none yet - uncomment "\n" when we get some

		"{\n" //Paragraph specifying title
			//Times New Roman 20 pt
			"\\f1 \\fs40\n" <<
			//Print the file name
			escapefilter << static_cast<T2A>(filename) <<
		"\\par\n}\n" // title para

		/*Nothing to put here yet
		"{\n" //Paragraph specifying other properties
			//Metadata
		"\\par\n}\n" // metadata para
		*/

		//Paragraph formatting properties
			//...
		//"\n"  PFPs - none yet - uncomment "\n" when we get some

		//Character formatting properties
			//Font 0 (hFont), font size, back colour 0 (iBkColorValue), iBkColorValue(Word2000 sux) text colour 1 (iTextColorValue)
			"\\f0 \\fs" << (DWORD)iFontSize * 2 << "\\cb0 \\chcbpat0 \\cf1"
			//...
		"\n"; // CFPs


		/*Warning M$ Word 2000 (& probably all of them (versions of M$ Word)) will:
			For arbitrary byte values:
				convert them into underscores (generally the control characters)
				convert them to \'hh where hh is the hex value of the byte (above 0x80)
				convert them to \tab \lquote \rquote \bullet \endash \emdash \~
				insert a unicode character (eg \u129) before the actual character
			For bookmarks:
				replace non-alphanumerics with underscores
				place "BM" in front of bookmarks that begin with a number
				hide bookmarks beginning with underscores
					can still view them with "Hidden bookmarks" checked
			For the lines:
				convert them into the newer shp format
		*/


		//The actual data, complete with highlighted selection & bookmarks (highlight & real ones)
		int iStartOfSelSetting;
		int iEndOfSelSetting;
		if (bSelected)
		{
			iStartOfSelSetting = iStartOfSelection;
			iEndOfSelSetting = iEndOfSelection;
			if (iEndOfSelSetting < iStartOfSelSetting)
				swap(iEndOfSelSetting, iStartOfSelSetting);
		}
		else
		{
			iStartOfSelSetting = iEndOfSelSetting = iCurByte;
		}
		int endoffile = m_dataArray.GetUpperBound() + 1;
		if (start > endoffile)
			start = endoffile;
		if (end > endoffile)
			end = endoffile;
		start = start / iBytesPerLine * iBytesPerLine;//cut back to the line start
		end = end / iBytesPerLine * iBytesPerLine;//cut back to the line start
		int i = 0;
		BYTE c;
		//This bit needs to be optimized
		bool highlights_in_this_line;
		for (int l = start ; l <= end; l += iBytesPerLine)
		{
			s.precision = iMinOffsetLen;
			//Offsets
			i = 0;
			i -= static_cast<int>(s.m_dwLen);
			s << hex << (DWORD)l;
			i += static_cast<int>(s.m_dwLen);
			//Bytespace
			for ( ; i < iMaxOffsetLen + iByteSpace; i++)
				s << "\\~";

			highlights_in_this_line = false;

			/*Wish I could do this in C++ - the alias would expire like a local variable
			alias sos iStartOfSelSetting, eos iEndOfSelSetting, bpl iBytesPerLine;*/

			int bi;
			//If the current line has any part selected
			if (iStartOfSelSetting <= l && iEndOfSelSetting >= l ||
				iStartOfSelSetting >= l && iEndOfSelSetting < l + iBytesPerLine ||
				iStartOfSelSetting < l + iBytesPerLine && iEndOfSelSetting >= l + iBytesPerLine)
			{
				highlights_in_this_line = true;
			}
			//If the current line contains a bookmark
			else for (bi = 0 ; bi < iBmkCount ; bi++)
			{
				int offset = pbmkList[bi].offset;
				if (offset >= l && offset < l + iBytesPerLine)
				{
					highlights_in_this_line = true;
					break;
				}
			}

			//With highlights
			if (highlights_in_this_line)
			{
				int sosl = max(iStartOfSelSetting, l);
				int eosl = min(iEndOfSelSetting, l + iBytesPerLine - 1);
				//Bytes
				for (i = l; i < l + iBytesPerLine; i++)
				{
					if (i == sosl)
						s << "{\\cb2 \\chcbpat2 \\cf3 "; // iSelBkColorValue, iSelBkColorValue (Word2000 sux), iSelTextColorValue
					for (bi = 0; bi < iBmkCount; bi++)
						if (pbmkList[bi].offset == i)
							break;
					if (bi < iBmkCount)
					{
						if (iEnteringMode == BYTES)
						{
							s << "{\\*\\bkmkstart ";
							if (pbmkList[bi].name.length())
								s << escapefilter << static_cast<T2A>(pbmkList[bi].name.c_str());
							else
								s << (DWORD)i;
							s << "}";
						}
						s << "{\\chbrdr \\brdrs \\brdrcf4 "; // iBmkColor
					}
					if (i == endoffile)
						s << "__\\~";
					else if (i > endoffile)
						s << "\\~\\~\\~";
					else
						s << hex << m_dataArray[i];
					if (bi < iBmkCount)
					{
						s << "}";
						if (iEnteringMode == BYTES)
						{
							s << "{\\*\\bkmkend ";
							if (pbmkList[bi].name.length())
								s << escapefilter << static_cast<T2A>(pbmkList[bi].name.c_str());
							else
								s << (DWORD)i;
							s << "}";
						}
					}
					if (i == eosl)
						s << "}";
					//Selected colours
					s << "\\~";
				} //Bytes
				//Charspace
				for (i = 0; i < iCharSpace; i++)
					s << "\\~";
				//Chars
				for (i = l; i < l + iBytesPerLine; i++)
				{
					if (i == sosl)
						s << "{\\cb2 \\chcbpat2 \\cf3 "; // iSelBkColorValue, iSelBkColorValue (Word2000 sux), iSelTextColorValue
					for (bi = 0; bi < iBmkCount; bi++)
						if (pbmkList[bi].offset == i)
							break;
					if (bi < iBmkCount)
					{
						if (iEnteringMode == CHARS)
						{
							s << "{\\*\\bkmkstart ";
							if (pbmkList[bi].name.length())
								s << escapefilter << static_cast<T2A>(pbmkList[bi].name.c_str());
							else
								s << (DWORD)i;
							s << "}";
						}
						s << "{\\chbrdr \\brdrs \\brdrcf4 "; // iBmkColor
					}
					if (i >= endoffile)
						s << "\\~";
					else
					{
						c = m_dataArray[i];
						if (!( ( iCharacterSet == OEM_FIXED_FONT && c != 0 ) || ( iCharacterSet == ANSI_FIXED_FONT && ( ( c >= 32 && c <= 126) || (c>=160 && c<=255) || (c>=145 && c<=146) ) ) ))
							c = '.';
						s << nbsp << escapefilter << c;
					}
					if (bi < iBmkCount)
					{
						s << "}";
						if (iEnteringMode == CHARS)
						{
							s << "{\\*\\bkmkend ";
							if (pbmkList[bi].name.length())
								s << escapefilter << static_cast<T2A>(pbmkList[bi].name.c_str());
							else
								s << (DWORD)i;
							s << "}";
						}
					}
					if (i == eosl)
						s << "}"; //Selected colours
				} //Chars
				s <<
				//End of line
				"\\line\n";
			}
			else /*No highlights - how boring*/
			{
				//Bytes
				for (i = l; i < l + iBytesPerLine; i++)
				{
					if (i == endoffile)
						s << "__\\~";
					else if (i > endoffile)
						s << "\\~\\~\\~";
					else
						s << hex << m_dataArray[i] << "\\~";
				}
				//Charspace
				for (i = 0; i < iCharSpace; i++)
					s << "\\~";
				//Chars
				for (i = l; i < l + iBytesPerLine; i++)
				{
					if(i >= endoffile)
						s << "\\~";
					else
					{
						c = m_dataArray[i];
						if (!( ( iCharacterSet == OEM_FIXED_FONT && c != 0 ) || ( iCharacterSet == ANSI_FIXED_FONT && ( ( c >= 32 && c <= 126) || (c>=160 && c<=255) || (c>=145 && c<=146) ) ) ))
							c = '.';
						s << nbsp << escapefilter << c;
					}
				}
				//End of line
				s << "\\line\n";
			} // No highlights
		} //for each line

		//The vertical lines
		//8192 is a majick number used to bring the lines in front of the text
		//Count is +4 because +2 for the 2 extra lines at charstart +1 for the extra one at the bytes end +1 cause \dpcount needs to be 1 more than the no of lines
		s << "{\\*\\do \\dobxcolumn \\dobypara \\dodhgt8192 \\dpgroup \\dpcount" << (DWORD)(iBytesPerLine/4+4) << "\n";
			register int x;
			register int y = ((end-start)/iBytesPerLine+1)*cyChar*15;
			//They should have just used the colour table
			//but no they is Micro$oft with absolutely no clue.
			register DWORD r = GetRValue(iSepColorValue);
			register DWORD g = GetGValue(iSepColorValue);
			register DWORD b = GetBValue(iSepColorValue);
			//The lines in the bytespace
			for (i = 0; i < iBytesPerLine / 4 +1; i++)
			{
				x = (i*9+iMaxOffsetLen+iByteSpace-2)*cxChar*20+cxChar*10-20;
				s << //There are 20 twips per point
				//A line
				"\\dpline"
				//Positions of the ends
				" \\dpptx" << x << " \\dppty0 \\dpptx" << x << " \\dppty" << y <<
				//Repeat for the benefit of Word - fuck M$ sux
				" \\dpx" << x << " \\dpy0 \\dpxsize0 \\dpysize" << y <<
				//Solid lines
				" \\dplinesolid"
				//Colour of the lines - fuck M$ sux
				" \\dplinecor" << r << " \\dplinecog" << g << " \\dplinecob" << b << "\n";
			}
			x = (iMaxOffsetLen+iByteSpace+iBytesPerLine*9/4+iCharSpace-1)*cxChar*20-10*cxChar;//There are 20 twips per point
			//The two lines at the start of the charspace
			s << 
			//A line
			"\\dpline"
			//Positions of the ends
			" \\dpptx" << x-30 << " \\dppty0 \\dpptx" << x-30 << " \\dppty" << y <<
			//Repeat for the benefit of Word - fuck M$ sux
			" \\dpx" << x-30 << " \\dpy0 \\dpxsize0 \\dpysize" << y <<
			//Solid lines
			" \\dplinesolid"
			//Colour of the lines - fuck M$ sux
			" \\dplinecor" << r << " \\dplinecog" << g << " \\dplinecob" << b << "\n"

			//A line
			"\\dpline"
			//Positions of the ends
			" \\dpptx" << x << " \\dppty0 \\dpptx" << x << " \\dppty" << y <<
			//Repeat for the benefit of Word - fuck M$ sux
			" \\dpx" << x << " \\dpy0 \\dpxsize0 \\dpysize" << y <<
			//Solid lines
			" \\dplinesolid"
			//Colour of the lines - fuck M$ sux
			" \\dplinecor" << r << " \\dplinecog" << g << " \\dplinecob" << b << "\n"

			//End of group
			"\\dpendgroup\n"
		"}\n" // \do
	"}" << (BYTE)'\0'; // \rtf1
	if (plen)
		*plen = s.m_dwLen;
	return s.Relinquish();
}

/**
 * @brief Enable/disable toolbar button.
 * @param [in] toolbar Resource handle to toolbar to modify.
 * @param [in] ID Resource id of button to enable/disable.
 * @param [in] bEnable If TRUE button will be enabled,
 *   if FALSE button will be disabled.
 */
static void EnableToolbarButton(HToolBar *toolbar, int ID, BOOL bEnable)
{
	UINT uCurrentFlags = toolbar->GetButtonState(ID);
	if (bEnable)
		uCurrentFlags |= TBSTATE_ENABLED;
	else
		uCurrentFlags &= ~TBSTATE_ENABLED;
	toolbar->SetButtonState(ID, uCurrentFlags);
}

/**
 * @brief Switch toolbar buttons between file/disk editing modes.
 * @param [in] bEnable If TRUE enable disk editing mode,
 *   if FALSE enable file editing mode.
 */
void HexEditorWindow::EnableDriveButtons(BOOL bEnable)
{
	const int IDS[] = { ID_DISK_GOTOFIRSTTRACK, ID_DISK_GOTONEXTTRACK,
			ID_DISK_GOTOPREVIOUSTRACK, ID_DISK_GOTOLASTTRACK, -1 };
	const int IDS_ToDisable[] = { IDM_EDIT_PASTE, IDM_EDIT_CUT, IDM_REPLACE,
			IDM_SAVE, -1 };

	int i;
	for (i = 0; IDS[i] != -1; i++)
		EnableToolbarButton(pwndToolBar, IDS[i], bEnable);
	for (i = 0; IDS_ToDisable[i] != -1; i++)
		EnableToolbarButton(pwndToolBar, IDS_ToDisable[i], !bEnable);
}
