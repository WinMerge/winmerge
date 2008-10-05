/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  hexwnd.cpp
 *
 * @brief Implementation of the hex window.
 *
 */
// ID line follows -- this is updated by SVN
// $Id: hexwnd.cpp 19 2008-08-18 15:03:12Z kimmov $

//============================================================================================
// Frhed main definition file.
#include "precomp.h"
#include "resource.h"
#include "StringTable.h"
#include "VersionData.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "shtools.h"
#include "gktools.h"
#include "regtools.h"
#include "hgstream.h"
#include "InvokeHtmlHelp.h"
#include "clipboard.h"
#include "BinTrans.h"
#include "LoadHexFile.h"
#include "LangArray.h"

#include "idt.h"
#include "ids.h"
#include "ido.h"

static const char appname[] = "frhed";

//CF_RTF defined in Richedit.h, but we don't include it cause that would be overkill
#ifndef CF_RTF
	#define CF_RTF TEXT("Rich Text Format")
#endif

const CLIPFORMAT CF_BINARYDATA = (CLIPFORMAT)RegisterClipboardFormat("BinaryData");
const CLIPFORMAT CF_RICH_TEXT_FORMAT = (CLIPFORMAT)RegisterClipboardFormat(CF_RTF);


int HexEditorWindow::MouseOpDist = GetProfileInt("Windows", "DragMinDist", DD_DEFDRAGMINDIST);
int HexEditorWindow::MouseOpDelay = GetProfileInt("Windows", "DragDelay", DD_DEFDRAGDELAY);
//We use the size of the font instead
//int HexEditorWindow::ScrollInset = GetProfileInt("Windows", "DragScrollInset", DD_DEFSCROLLINSET);
int HexEditorWindow::ScrollDelay = GetProfileInt("Windows", "DragScrollDelay", DD_DEFSCROLLDELAY);
int HexEditorWindow::ScrollInterval = GetProfileInt("Windows", "DragScrollInterval", DD_DEFSCROLLINTERVAL);

int HexEditorWindow::iPasteAsText = 0;
int HexEditorWindow::iPasteTimes = 1;
int HexEditorWindow::iPasteSkip = 0;

SimpleString HexEditorWindow::TexteditorName = "notepad.exe";
SimpleString HexEditorWindow::EncodeDlls;

//Temporary stuff for CMD_move_copy
int iMovePos;
OPTYP iMoveOpTyp;

// RK: function by pabs.
BOOL contextpresent();
BOOL defaultpresent();
BOOL unknownpresent();
BOOL oldpresent();
BOOL linkspresent();
BOOL frhedpresent();

//--------------------------------------------------------------------------------------------
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
	bPartialStats = 0;
	bPartialOpen = FALSE;
	iBmkCount = 0;

	iMRU_count = MRUMAX;
	while (iMRU_count)
		sprintf(strMRU[--iMRU_count], "dummy%d", iMRU_count);

	bFilestatusChanged = TRUE;
	iBinaryMode = LITTLEENDIAN_MODE;
	bUnsignedView = TRUE;
	iFontSize = 10;
	iInsertMode = FALSE;
	iTextColorValue = RGB(0,0,0);
	iBkColorValue = RGB(255,255,255);
	iSepColorValue = RGB(192,192,192);
	iDiffBkColorValue = RGB(239,203,5);
	iDiffTextColorValue = RGB(0,0,0);
	iSelDiffBkColorValue = RGB(239,119,116);
	iSelDiffTextColorValue = RGB(0,0,0);
	sibling = this;
	iAutomaticBPL = 1;
	bSelected = FALSE;
	bSelecting = FALSE;
	dragging = FALSE;
	iStartOfSelection = 0;
	iEndOfSelection = 0;
	hwnd = 0;
	hwndMain = 0;
	hwndToolBar = 0;
	hwndStatusBar = 0;
	hAccel = 0;
	iMinOffsetLen = iMaxOffsetLen = 6;//max is same as min because there is no data
	bAutoOffsetLen = TRUE;
	iByteSpace = 2;
	iBytesPerLine = 16;
	iCharSpace = 1;
	iEnteringMode = BYTES;
	iFileChanged = FALSE;
	bFileNeverSaved = TRUE;

	iCharacterSet = ANSI_FIXED_FONT;

	bSelected = FALSE;
	iVscrollMax = 0;
	iVscrollPos = 0;
	iHscrollMax = 0;
	iHscrollPos = 0;

	iCurByte = 0;
	DataArray.ClearAll();
	DataArray.SetGrowBy(100);
	strcpy(filename, "Untitled");
	area = AREA_NONE;
}

//--------------------------------------------------------------------------------------------
HexEditorWindow::~HexEditorWindow()
{
	if (hFont)
		DeleteObject(hFont);
	delete Drive;
}

//--------------------------------------------------------------------------------------------
unsigned char *HexEditorWindow::get_buffer(int len)
{
	if (!DataArray.SetSize(len))
		return 0;
	DataArray.ExpandToSize();
	return DataArray;
}

//--------------------------------------------------------------------------------------------
int HexEditorWindow::get_length()
{
	return DataArray.GetLength();
}

void HexEditorWindow::set_sibling(IHexEditorWindow *p)
{
	sibling = p;
}

void HexEditorWindow::set_status_bar(HWND h)
{
	hwndStatusBar = h;
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
	return TranslateAccelerator(hwnd, hAccel, pMsg);
}

LangArray langArray;

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

static LPWSTR NTAPI LoadResString(UINT uStringID)
{
	HINSTANCE hinst = langArray.m_hLangDll ? langArray.m_hLangDll : hMainInstance;
	LPWSTR text = LoadStringResource(hinst, uStringID);
	if (text)
	{
		text = SysAllocStringLen(text + 1, (WORD)*text);
		if (langArray.m_hLangDll)
		{
			int line = 0;
			if (LPWSTR p = wcschr(text, L':'))
				line = _wtoi(p + 1);
			SysFreeString(text);
			text = langArray.TranslateStringW(line);
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
		SysFreeString(S[i]);
}

/**
 * @brief Load translations for given language if present.
 */
BOOL HexEditorWindow::load_lang(LANGID langid)
{
	FreeStringTable();
	BOOL bDone = langArray.Load(hMainInstance, langid);
	LoadStringTable();
	return bDone;
}

/**
 * @brief Load a string.
 */
BSTR HexEditorWindow::load_string(UINT uStringID)
{
	return LoadResString(uStringID);
}

/**
 * @brief Free a string obtained through load_string().
 */
void HexEditorWindow::free_string(BSTR text)
{
	SysFreeString(text);
}

//--------------------------------------------------------------------------------------------
int HexEditorWindow::load_file(const char *fname)
{
	WaitCursor wc;
	int bLoaded = FALSE;
	int filehandle = _open(fname, _O_RDONLY|_O_BINARY);
	if (filehandle != -1)
	{
		int filelen = _filelength(filehandle);
		DataArray.ClearAll();
		// Try to allocate memory for the file.
		if (DataArray.SetSize(filelen))
		{
			DataArray.ExpandToSize();
			// If read-only mode on opening is enabled or the file is read only:
			bReadOnly = bOpenReadOnly || -1 == _access(fname, 02); //Pabs added call to _access
			// Load the file.
			if (filelen == 0 || _read(filehandle, DataArray, filelen) != -1)
			{
				// This is an empty file. Don't need to read anything.
				GetLongPathNameWin32(fname, filename);
				bLoaded = TRUE;
			}
			else
			{
				MessageBox(hwnd, "Error while reading from file.", "Load error", MB_ICONERROR);
			}
		}
		else
		{
			MessageBox(hwnd, "Not enough memory to load file.", "Load error", MB_ICONERROR);
		}
		_close(filehandle);
	}
	else
	{
		char buf[500];
		sprintf(buf, "Error code 0x%x occured while opening file %s.", errno, fname);
		MessageBox(hwnd, buf, "Load error", MB_ICONERROR);
	}
	if (bLoaded)
	{
		bFileNeverSaved = FALSE;
		bPartialStats = 0;
		bPartialOpen = FALSE;
		// Update MRU list.
		update_MRU();
		bFilestatusChanged = TRUE;
		iVscrollMax = 0;
		iVscrollPos = 0;
		iHscrollMax = 0;
		iHscrollPos = 0;
		iCurByte = 0;
		iCurNibble = 0;
		bSelected = FALSE;
		iFileChanged = FALSE;
	}
	resize_window();
	return bLoaded;
}

//--------------------------------------------------------------------------------------------
int HexEditorWindow::file_is_loadable(const char *fname)
{
	int filehandle = _open(fname, _O_RDONLY|_O_BINARY);
	if (filehandle == -1)
		return FALSE;
	_close(filehandle);
	return TRUE;
}

//--------------------------------------------------------------------------------------------
int HexEditorWindow::at_window_create(HWND hw, HINSTANCE hI)
{
	hwnd = hw;
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
			RegisterDragDrop(hwnd, target);
	}
	return TRUE;
}

//--------------------------------------------------------------------------------------------
// Window was resized to new width of cx and new height of cy.
void HexEditorWindow::resize_window()
{
	RECT rcClient;
	GetClientRect(hwnd, &rcClient);
	// Get font data.
	HDC hdc = GetDC(hwnd);
	make_font();
	HFONT of = (HFONT) SelectObject(hdc, hFont);
	TEXTMETRIC tm;
	GetTextMetrics (hdc, &tm);
	cxChar = tm.tmAveCharWidth;
	cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2;
	cyChar = tm.tmHeight + tm.tmExternalLeading;
	SelectObject (hdc, of);
	ReleaseDC (hwnd, hdc);

	//--------------------------------------------
	int length = get_length();

//Pabs inserted
	int x = length;//value of the last offset
	if (bPartialStats)
		x += iPartialOffset;
	iMaxOffsetLen = 1;
	//length of last offset
	while (x & ~0 << 4 * iMaxOffsetLen)
		++iMaxOffsetLen;
	if (iMaxOffsetLen < iMinOffsetLen)
		iMaxOffsetLen = iMinOffsetLen;
//end

	cxClient = rcClient.right;
	cyClient = rcClient.bottom;
	cxBuffer = max(1, cxClient / cxChar);
	cyBuffer = max(1, cyClient / cyChar);
	// Adjust bytes per line to width of window.
	// cxBuffer = maximal width of client-area in chars.
	if (iAutomaticBPL)
	{
//Pabs replaced "iOffsetLen" with "iMaxOffsetLen"
		int bytemax = cxBuffer - iMaxOffsetLen - iByteSpace - iCharSpace;
//end
		iBytesPerLine = bytemax / 4;
		if (iBytesPerLine < 1)
			iBytesPerLine = 1;
	}

//Pabs inserted
	x = length / iBytesPerLine * iBytesPerLine;//value of the last offset
	if (bPartialStats)
		x += iPartialOffset;
	iMaxOffsetLen = 1;
	//length of last offset
	while (x & ~0 << 4 * iMaxOffsetLen)
		++iMaxOffsetLen;
	if (bAutoOffsetLen)
		iMinOffsetLen = iMaxOffsetLen;
	else if (iMaxOffsetLen < iMinOffsetLen)
		iMaxOffsetLen = iMinOffsetLen;
//end

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
	SetScrollInfo(hwnd, SB_VERT, &SI, TRUE);

	iHscrollMax = iCharsPerLine - 1;
	if(iHscrollPos>iHscrollMax-cxBuffer+1)iHscrollPos = iHscrollMax-cxBuffer+1;
	if(iHscrollPos<0)iHscrollPos=0;
	SI.nPage = cxBuffer;
	SI.nPos = iHscrollPos;
	SI.nMax = iHscrollMax;
	SetScrollInfo(hwnd, SB_HORZ, &SI, TRUE);

	set_wnd_title();
	repaint();

	if (hwnd == GetFocus() && sibling != this)
	{
		sibling->resize_window();
	}
}

//--------------------------------------------------------------------------------------------
// When focus is gained.
void HexEditorWindow::set_focus()
{
	CreateCaret(hwnd, NULL, cxChar, cyChar);
	set_caret_pos();
	ShowCaret(hwnd);
	mark_char(0);
}

//--------------------------------------------------------------------------------------------
// What to do when focus is lost.
void HexEditorWindow::kill_focus()
{
	HideCaret(hwnd);
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

int HexEditorWindow::snap_caret()
{
	int repall = 0;
	int adjusth = 0;
	int adjustv = 0;

	int col = iMaxOffsetLen + iByteSpace;
	if (bSelecting ? area == AREA_BYTES : iEnteringMode == BYTES)
		col += (iCurByte % iBytesPerLine) * 3 + iCurNibble; //BYTES_LOGICAL_COLUMN;
	else
		col += iBytesPerLine * 3 + iCharSpace + iCurByte % iBytesPerLine; //CHARS_LOGICAL_COLUMN;

	if (col >= iHscrollPos + cxBuffer)
	{
		iHscrollPos = col - (cxBuffer - 1);
		adjusth = 1;
	}
	else if (col < iHscrollPos)
	{
		iHscrollPos = column;
		adjusth = 1;
	}

	if (adjusth)
	{
		adjust_hscrollbar();
		repall = 1;
	}

	int row = iCurByte / iBytesPerLine;
	if (row < iVscrollPos)
	{
		iVscrollPos = row;
		adjustv = 1;
	}
	else if (row >= iVscrollPos + cyBuffer)
	{
		iVscrollPos = row - (cyBuffer - 1);
		if (iVscrollPos < 0)
			iVscrollPos = 0;
		adjustv = 1;
	}

	if (adjustv)
	{
		adjust_vscrollbar();
		repall = 1;
	}
	return repall;
}

//--------------------------------------------------------------------------------------------
// Handler for non-character keys (arrow keys, page up/down etc.)
void HexEditorWindow::keydown(int key)
{
	if (filename[0] == '\0' || iCurByte<0)
		return;

	//Pabs rewrote the rest of this function
	//Only handle these
	switch (key)
	{
	case VK_ESCAPE: case VK_LEFT: case VK_RIGHT: case VK_UP: case VK_DOWN:
	case VK_PRIOR: case VK_NEXT: case VK_HOME: case VK_END:
		break;
	default:
		return;
	}

	int shift = GetKeyState(VK_SHIFT) < 0;
	int ctrl = GetKeyState(VK_CONTROL) < 0;

	int *a;//Data to update
	int b;//How much to update it by
	int c;//The original value
	int sel = bSelected;

	if (shift && !ctrl && key != VK_ESCAPE)
	{
		a = &iEndOfSelection;
		if (!bSelected)
		{
			iStartOfSelection = iEndOfSelection = c = iCurByte;
			bSelected = TRUE;
		}
		else c = iEndOfSelection;
	}
	else if (!bSelecting)
	{
		c = iCurByte;
		if (bSelected)
		{
			int iStartOfSelSetting = iStartOfSelection;
			int iEndOfSelSetting = iEndOfSelection;
			if (iStartOfSelSetting > iEndOfSelSetting)
				swap(iStartOfSelSetting, iEndOfSelSetting);

			switch (key)
			{
			case VK_ESCAPE:
				if (dragging)
					return;
				ShowCaret(hwnd);
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
		if (ctrl && shift)
		{
			a = &iEndOfSelection;
			c = iEndOfSelection;
			bSelected = TRUE;
		}
		else
		{
			bSelected = FALSE;
			a = &iCurByte;
		}
	}
	else /*if( bSelected && bSelecting )*/
	{
		a = &iStartOfSelection;
		c = iStartOfSelection;
		if (key == VK_ESCAPE)
		{
			iStartOfSelection = iEndOfSelection = new_pos;
		}
	}

	int icn = iCurNibble;

	int lastbyte = DataArray.GetUpperBound();
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
		case VK_LEFT: case VK_UP:  case VK_PRIOR: *a-=b; break;
		case VK_RIGHT:case VK_DOWN:case VK_NEXT:  *a+=b; break;
		case VK_HOME:
			if(ctrl) *a = 0;
			else *a=*a/iBytesPerLine*iBytesPerLine;
			iCurNibble = 0;
		break;
		case VK_END:
			if(ctrl) *a = lastbyte;
			else *a=(*a/iBytesPerLine+1)*iBytesPerLine-1;
			iCurNibble = 1;
			break;
	}

	if(bSelected){
		if(lastbyte<0){bSelected=iCurNibble=iCurByte=0;}
		else{
			if(iStartOfSelection>lastbyte){iStartOfSelection=lastbyte;iCurNibble=1;}
			if(iStartOfSelection<0){iStartOfSelection=0;iCurNibble=0;}
			if(iEndOfSelection>lastbyte){iEndOfSelection=lastbyte;iCurNibble=1;}
			if(iEndOfSelection<0){iEndOfSelection=0;iCurNibble=0;}
		}
	} else {
		if(iCurByte>lastbyte){iCurByte=lastbyte;iCurNibble=1;}
		if(iCurByte<0){iCurByte=0;iCurNibble=0;}
	}
	//repaint from line c to line a
	if(c!=*a||icn!=iCurNibble||sel!=bSelected){

		if (bSelected)
		{
			iCurByte = *a;
			iCurNibble = 0;
		}
		if (snap_caret())
			repaint();
		else
			repaint(*a / iBytesPerLine, c / iBytesPerLine);
	}
}

//--------------------------------------------------------------------------------------------
// Handler for character keys.
void HexEditorWindow::character(char ch)
{
	if (bSelecting)
		return;

//Pabs inserted
	//This will be handled in HexEditorWindow::keydown
	if (ch == VK_ESCAPE)
		return;
//end

	// If we are in read-only mode, give a warning and return,
	// except if TAB was pressed.
	if (bReadOnly && ch != '\t')
	{
		MessageBox(hwnd, "Can't change file because read-only mode is engaged!", "Keyboard editing", MB_ICONERROR );
		return;
	}

	char c = (char)tolower(ch);
	if (ch == '\t') // TAB => change EnteringMode.
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
		DataArray.RemoveAt(iCurByte + 1, iEndByte - iCurByte);//Remove extraneous data
		DataArray[iCurByte] = 0;//Will overwrite below
		bSelected = FALSE;//Deselect
		resize_window();//Redraw stuff
	}
	else if (iInsertMode && iCurNibble == 0 || iCurByte == DataArray.GetLength())
	{
		// caret at EOF
		if (!DataArray.InsertAtGrow(iCurByte, 0, 1))
		{
			MessageBox (hwnd, "Not enough memory for inserting character.", "Insert mode error", MB_ICONERROR);
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
			DataArray[iCurByte] = (BYTE)((DataArray[iCurByte] & 0x0f) | (c << 4));
			iCurNibble = 1;
		}
		else
		{
			DataArray[iCurByte] = (BYTE)((DataArray[iCurByte] & 0xf0) | c);
			++iCurByte;
			iCurNibble = 0;
		}
	}
	else // Char-mode.
	{
		switch (iCharacterSet)
		{
		case ANSI_FIXED_FONT:
			DataArray[iCurByte] = ch;
			break;
		case OEM_FIXED_FONT:
			CharToOemBuff(&ch, (char *)&DataArray[iCurByte], 1);
			break;
		}
		++iCurByte;
	}
	iFileChanged = TRUE;
	bFilestatusChanged = TRUE;
	if (snap_caret())
		repaint();
	else
		repaint(iByteLine, iCurByte / iBytesPerLine);
}

//--------------------------------------------------------------------------------------------
// Handler for vertical scrollbar.
void HexEditorWindow::vscroll(int cmd)
{
	SCROLLINFO SI;
	SI.cbSize = sizeof SI;
	SI.fMask = SIF_TRACKPOS;
	GetScrollInfo(hwnd, SB_VERT, &SI);
	int pos = SI.nTrackPos;
	if (filename[0] == '\0' || DataArray.GetLength() == 0)
		return;
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
		iVscrollPos = pos;
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

	SI.fMask  = SIF_POS | SIF_DISABLENOSCROLL;
	SI.nPos   = iVscrollPos;
	SetScrollInfo(hwnd, SB_VERT, &SI, TRUE);

	if (iVscrollPos > iNumlines - 0)
		iVscrollPos = iNumlines - 0;
	repaint();
}

//--------------------------------------------------------------------------------------------
// Handler for horizontal scrollbar.
void HexEditorWindow::hscroll(int cmd)
{
	SCROLLINFO SI;
	SI.cbSize = sizeof SI;
	SI.fMask = SIF_TRACKPOS;
	GetScrollInfo(hwnd, SB_HORZ, &SI);
	int pos = SI.nTrackPos;
	if (filename[0] == '\0' || DataArray.GetLength()==0)
		return;

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
		iHscrollInc = pos - iHscrollPos;
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
	SI.fMask = SIF_POS | SIF_DISABLENOSCROLL;
	SI.nPos = iHscrollPos;
	SetScrollInfo(hwnd, SB_HORZ, &SI, TRUE);
	repaint();
}

//--------------------------------------------------------------------------------------------
// WM_PAINT handler.
int HexEditorWindow::paint()
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);
	//-------------------------------------------------------
	HideCaret (hwnd);
	// Delete remains of last position.
	int a = iVscrollPos + ps.rcPaint.top / cyChar;
	int b = iVscrollPos + ps.rcPaint.bottom / cyChar;
	if (b >= iNumlines)
		b = iNumlines - 1;
	int iBkColor = PALETTERGB(GetRValue(iBkColorValue), GetGValue(iBkColorValue), GetBValue(iBkColorValue));
	RECT rc;
	HBRUSH hbr = CreateSolidBrush(iBkColor);
	// Delete lower border if there are empty lines on screen.
	GetClientRect(hwnd, &rc);
	rc.top = (b - iVscrollPos + 1) * cyChar;
	if (rc.top < rc.bottom)
		FillRect(hdc, &rc, hbr);
	// Delete right border.
	GetClientRect(hwnd, &rc);
	rc.left = (iHscrollMax + 1 - iHscrollPos) * cxChar;
	if (rc.left < rc.right)
		FillRect(hdc, &rc, hbr);
	DeleteObject(hbr);

	// Get font.
	HGDIOBJ oldfont = SelectObject(hdc, hFont);
	HPEN sep_pen = CreatePen(PS_SOLID, 1, iSepColorValue);
	HGDIOBJ oldpen = SelectObject(hdc, sep_pen);
	hbr = CreateSolidBrush(iBmkColor);

	for (int i = a ; i <= b ; i++)
	{
		print_line(hdc, i, hbr);
		// Mark character.
		if (i == iCurByte / iBytesPerLine && !bSelected && GetFocus() == hwnd)
			mark_char(hdc);
	}
	DeleteObject(hbr);
	SelectObject(hdc, oldpen);
	DeleteObject(sep_pen);
	SelectObject(hdc, oldfont);
	ShowCaret(hwnd);
	EndPaint(hwnd, &ps);
	return 0;
}

//--------------------------------------------------------------------------------------------
// Receives WM_COMMAND messages and passes either them to their handler functions or
// processes them here.
void HexEditorWindow::command(int cmd)
{
	if (!queryCommandEnabled(cmd))
		return;
	HMENU hMenu = GetMenu(hwndMain);
	switch (cmd)
	{
//Pabs changed - line insert
//File Menu
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
		static_cast<dialog<DragDropOptionsDlg>*>(this)->DoModal(hwnd);
		break;
//Registry menu
	case IDM_SHORTCUTS:
		static_cast<dialog<ShortcutsDlg>*>(this)->DoModal(hwnd);
		break;
	case IDM_UPGRADE:
		static_cast<dialog<UpgradeDlg>*>(this)->DoModal(hwnd);
		break;
	case IDM_REMOVE:
		int res,r,r0;r=r0=0;//r&r0 used to determine if the user has removed all frhed data
		res = MessageBox(hwnd, "Are you sure you want to remove frhed ?", "Remove frhed", MB_YESNO);
		if (res != IDYES)
			return;
		//Can assume registry data exists
		res = linkspresent();
		if (res)
		{
			r0++;
			res = MessageBox(hwnd, "Remove known links ?", "Remove frhed", MB_YESNO);
			if (res == IDYES)
			{
				r++;
				//Remove known links & registry entries of those links
				HKEY hk;
				char valnam[_MAX_PATH+1]="";
				DWORD valnamsize = _MAX_PATH+1,typ;
				char valbuf[_MAX_PATH+1]="";
				DWORD valbufsize = _MAX_PATH+1,ret;
				if(ERROR_SUCCESS==RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\frhed\\v"CURRENT_VERSION"." SUB_RELEASE_NO "\\links",0,KEY_ALL_ACCESS,&hk)){
					for(DWORD i = 0;;i++){
						typ=0;
						valnamsize = valbufsize = _MAX_PATH+1;
						valbuf[0]=valnam[0]=0;
						ret = RegEnumValue(hk,i,valnam,&valnamsize,0,&typ,(BYTE*) valbuf,&valbufsize);
						if(typ==REG_SZ && valbuf[0]!=0 && PathIsDirectory(valbuf)){
							PathAddBackslash(valbuf);
							strcat(valbuf,"frhed.lnk");
							remove(valbuf);
						}
						if(ERROR_NO_MORE_ITEMS==ret)break;
					}
					RegCloseKey(hk);
				}
				RegDeleteKey(HKEY_CURRENT_USER,"Software\\frhed\\v"CURRENT_VERSION"." SUB_RELEASE_NO "\\links");
			}
		}
		res = contextpresent()||unknownpresent();
		if (res)
		{
			r0++;
			res = MessageBox(hwnd,"Remove 'Open in frhed' command(s) ?","Remove frhed",MB_YESNO);
			if(res==IDYES) {
				r++;
				//Remove 'Open in frhed' command registry entries
				RegDeleteKey( HKEY_CLASSES_ROOT, "*\\shell\\Open in frhed\\command" ); //WinNT requires the key to have no subkeys
				RegDeleteKey( HKEY_CLASSES_ROOT, "*\\shell\\Open in frhed" );
				RegDeleteKey( HKEY_CLASSES_ROOT, "Unknown\\shell\\Open in frhed\\command" ); //WinNT requires the key to have no subkeys
				RegDeleteKey( HKEY_CLASSES_ROOT, "Unknown\\shell\\Open in frhed" );
				char stringval[ _MAX_PATH ]="";
				long len = _MAX_PATH;
				RegQueryValue(HKEY_CLASSES_ROOT,"Unknown\\shell",stringval,&len);
				if(!strcmp(stringval, "Open in frhed")){
					HKEY hk;
					if(ERROR_SUCCESS==RegOpenKey(HKEY_CLASSES_ROOT, "Unknown\\shell",&hk)){
						RegDeleteValue(hk,NULL);
						RegCloseKey(hk);
					}
				}
			}
		}
		HKEY tmp;
		res = RegOpenKey(HKEY_CURRENT_USER,"Software\\frhed\\v"CURRENT_VERSION"." SUB_RELEASE_NO ,&tmp);
		if (res == ERROR_SUCCESS)
		{
			RegCloseKey(tmp);
			r0++;
			res = MessageBox(hwnd, "Remove registry entries?", "Remove frhed", MB_YESNO);
			if (res == IDYES)
			{
				r++;
				bSaveIni = FALSE;//Don't save ini data when the user quits (and hope other instances are not running now (they will write new data)
				OSVERSIONINFO ver;
				ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
				GetVersionEx(&ver);
				if(ver.dwPlatformId==VER_PLATFORM_WIN32_NT)
					RegDeleteWinNTKey(HKEY_CURRENT_USER,"Software\\frhed\\v"CURRENT_VERSION"." SUB_RELEASE_NO );
				else
					RegDeleteKey(HKEY_CURRENT_USER,"Software\\frhed\\v"CURRENT_VERSION"." SUB_RELEASE_NO );
				res = oldpresent();
				if(res){
					res = MessageBox(hwnd,"Registry entries from previous versions of frhed were found\n"
						"Should they all be removed ?","Remove frhed",MB_YESNO);
					if(res==IDYES){
						if(ver.dwPlatformId==VER_PLATFORM_WIN32_NT)
							RegDeleteWinNTKey(HKEY_CURRENT_USER,"Software\\frhed");
						else
							RegDeleteKey(HKEY_CURRENT_USER,"Software\\frhed");
					}
				}
			}
		}
#if 0
		for(;;i++){
			RegEnumKey (HKEY_USERS,i,buf,MAX_PATH+1);
			if(res==ERROR_NO_MORE_ITEMS)break;
			if(!stricmp(buf,".Default")){
				if(ERROR_SUCCESS==RegOpenKeyEx(HKEY_USERS,buf,0,KEY_ALL_ACCESS,&hk)){
					removefrhedfromuser(hk);
					RegCloseKey(hk);
				}
			}
		}
#endif // 0
		if (r == r0)
		{
			MessageBox(hwnd,
				"Now all that remains to remove frhed from this computer is to:\n"
				"1. Quit all other instances of frhed(after turning off \"Save ini...\" in each one)\n"
				"2. Quit this instance of frhed\n"
				"3. Check that the registry data was removed (just in case)\n"
				"4. Delete the directory where frhed currently resides\n"
				"5. If you have an email account please take the time to\n"
				"    email the author/s and give the reason/s why you have\n"
				"    removed frhed from your computer",
				"Remove frhed", MB_OK);
		}
		break;
	case IDM_SAVEINI:
		bSaveIni = !bSaveIni;//!(MF_CHECKED==GetMenuState(hMenu,IDM_SAVEINI,0));
		break;
	case IDM_CHANGEINST: //switch instance
		static_cast<dialog<ChangeInstDlg>*>(this)->DoModal(hwnd);
		break;
	case IDM_CONTEXT:
		if (MF_CHECKED == GetMenuState(hMenu, IDM_CONTEXT, 0))
		{
			RegDeleteKey(HKEY_CLASSES_ROOT, "*\\shell\\Open in frhed\\command"); //WinNT requires the key to have no subkeys
			RegDeleteKey(HKEY_CLASSES_ROOT, "*\\shell\\Open in frhed");
		}
		else
		{
			HKEY key1;
			LONG res = RegCreateKey(HKEY_CLASSES_ROOT,
				"*\\shell\\Open in frhed\\command",
				&key1);
			if (res == ERROR_SUCCESS)
			{
				char cmd[MAX_PATH];
				int len = sprintf(cmd, "%s %%1", _pgmptr);
				RegSetValue(key1, NULL, REG_SZ, cmd, len);
			}
		}
		break;
	case IDM_UNKNOWN:
		if (MF_CHECKED == GetMenuState(hMenu, IDM_UNKNOWN, 0))
		{
			HKEY hk;
			RegDeleteKey(HKEY_CLASSES_ROOT, "Unknown\\shell\\Open in frhed\\command"); //WinNT requires the key to have no subkeys
			RegDeleteKey(HKEY_CLASSES_ROOT, "Unknown\\shell\\Open in frhed");
			if (ERROR_SUCCESS == RegOpenKey(HKEY_CLASSES_ROOT, "Unknown\\shell", &hk))
			{
				RegDeleteValue(hk, NULL);
				RegCloseKey(hk);
			}
		}
		else
		{
			HKEY key1;
			LONG res = RegCreateKey(HKEY_CLASSES_ROOT,
				"Unknown\\shell\\Open in frhed\\command",
				&key1);
			if (res == ERROR_SUCCESS)
			{
				char cmd[MAX_PATH];
				int len = sprintf(cmd, "%s %%1", _pgmptr);
				RegSetValue(key1, NULL, REG_SZ, cmd, len);
			}
		}
		break;
	case IDM_DEFAULT:
		if (MF_CHECKED == GetMenuState(hMenu, IDM_DEFAULT, 0))
		{
			HKEY hk;
			if (ERROR_SUCCESS == RegOpenKey(HKEY_CLASSES_ROOT, "Unknown\\shell", &hk))
			{
				RegDeleteValue(hk, NULL);
				RegCloseKey(hk);
			}
		}
		else
		{
			RegSetValue(HKEY_CLASSES_ROOT, "Unknown\\shell", REG_SZ, "Open in frhed", 13);
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
			char buf[500];
			char *buf2 = buf;
			buf[0] = '\0';
			union
			{
				float fval;
				double dval;
				BYTE bytes[1];
			} u;
			int iBytesAhead = DataArray.GetLength() - iCurByte;
			if (iBytesAhead >= sizeof u.fval)
			{
				// Space enough for float.
				memcpy(&u, &DataArray[iCurByte], sizeof u.fval);
				if (iBinaryMode != LITTLEENDIAN_MODE) // BIGENDIAN_MODE
					reverse_bytes(u.bytes, u.bytes + sizeof u.fval - 1);
				buf2 += sprintf(buf2, "float size value:\n%g\n", u.fval);
			}
			else
			{
				buf2 += sprintf(buf2, "Not enough space for float size value.\n");
			}
			if (iBytesAhead >= sizeof u.dval)
			{
				// Space enough for double.
				memcpy(&u, &DataArray[iCurByte], sizeof u.dval);
				if (iBinaryMode != LITTLEENDIAN_MODE) // BIGENDIAN_MODE
					reverse_bytes(u.bytes, u.bytes + sizeof u.dval - 1);
				buf2 += sprintf(buf2, "\ndouble size value:\n%g\n", u.dval);
			}
			else
			{
				buf2 += sprintf(buf2, "\nNot enough space for double size value.\n");
			}
			MessageCopyBox(hwnd, buf, "Floating point values", MB_ICONINFORMATION);
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
			char buf[4000];
			char *buf2 = buf;
			buf2 += sprintf(buf2, "Data length: %d\n", DataArray.GetLength());
			buf2 += sprintf(buf2, "Upper Bound: %d\n", DataArray.GetUpperBound());
			buf2 += sprintf(buf2, "Data size: %d\n", DataArray.GetSize());
			buf2 += sprintf(buf2, "cxChar: %d\n", cxChar);
			buf2 += sprintf(buf2, "cyChar: %d\n", cyChar);
			buf2 += sprintf(buf2, "iNumlines: %d\n", iNumlines);
			buf2 += sprintf(buf2, "iVscrollPos: %d\n", iVscrollPos);
			buf2 += sprintf(buf2, "iCurByte: %d\n", iCurByte);
			buf2 += sprintf(buf2, "cyBuffer: %d\n", cyBuffer);
			buf2 += sprintf(buf2, "cxBuffer: %d\n", cxBuffer);
			buf2 += sprintf(buf2, "iMRU_count: %d\n", iMRU_count);
			int i;
			for (i = 0 ; i < MRUMAX ; i++)
			{
				buf2 += sprintf(buf2, "MRU %d=%s\n", i + 1, strMRU[i]);
			}
			MessageBox(hwnd, buf, "Internal status", MB_OK);
		}
		break;
#endif

	case IDM_EDIT_CUT:
		CMD_edit_cut();
		break;

	case IDM_HELP_TOPICS:
		ShowHtmlHelp(HELP_CONTENTS, 0, hwnd);
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
		ShowModalDialog(IDD_ABOUTDIALOG, hwnd, AboutDlgProc, this);
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
		CMD_new("New");
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
		{
			char buf[500];
			sprintf(buf, "Unknown COMMAND-ID %d.", cmd);
			MessageBox(hwnd, buf, "frhed ERROR", MB_OK);
		}
		break;
	}
}

//--------------------------------------------------------------------------------------------
int HexEditorWindow::destroy_window()
{
	RevokeDragDrop(hwnd);
	if (target)
		CoLockObjectExternal(target, FALSE, TRUE);
	return 0;
}

//--------------------------------------------------------------------------------------------
// Set the window title and the statusbar text.
//Pabs hugely revamped for less code
void HexEditorWindow::set_wnd_title()
{
	char buf[512];

	if (filename[0] != '\0')
	{
		// Change window title.
		if (bFilestatusChanged && hwndMain)
		{
			sprintf (buf, "[%s", filename);
			if (iFileChanged)
				strcat (buf, " *");
			strcat (buf, "]");
			if (bPartialOpen==TRUE)
				strcat (buf, " - P");
			strcat (buf, " - ");
			strcat (buf, appname);
			SetWindowText (hwndMain, buf);
			bFilestatusChanged = FALSE;
		}
		// Selection going on.
		if (bSelected)
		{
			if (iEndOfSelection >= iStartOfSelection)
			{
				sprintf (buf, "Selected: Offset %d=0x%x to %d=0x%x (%d byte(s))", iStartOfSelection, iStartOfSelection,
					iEndOfSelection, iEndOfSelection, iEndOfSelection-iStartOfSelection+1);
			}
			else
			{
				sprintf (buf, "Selected: Offset %d=0x%x to %d=0x%x (%d byte(s))", iEndOfSelection, iEndOfSelection,
					iStartOfSelection, iStartOfSelection, iStartOfSelection-iEndOfSelection+1);
			}
			SendMessage (hwndStatusBar, SB_SETTEXT, 0, (LPARAM) buf);
		}
		else // Normal display.
		{
//Pabs changed - \t removed from end of string literal ("Offset %d=0x%x\t" -> "Offset %d=0x%x")
			sprintf (buf, "Offset %d=0x%x", iCurByte, iCurByte);
//end
			int wordval, longval;
//Pabs changed - buf2 declaration moved so that selection bugfix works properly
			char buf2[80];
//end
//Pabs changed - line insert
			if (DataArray.GetLength()-iCurByte > 0){//if we are not looking at the End byte
				// R. Kibria: changed the output slightly (used to be "Bits = 0b").
				strcat (buf, "   Bits=");//append stuff to status text
				unsigned char zzz=DataArray[iCurByte];//quicker to have a tmp var than to call operator[] 8 times
				int i;
				for(i=0;i<8;i++)buf2[i]=((zzz>>i)&0x1?'1':'0');//Extract bits
				for(i=0;i<4;i++)swap(buf2[i],buf2[7-i]);//flip order-for some reason it doesn't display correctly going i-- or i++ in for loop
				buf2[8]='\0';//terminate string
				strcat (buf, buf2);//append to status text
			}
			strcat (buf, "\t");//add that \t back on to the status text
//end
			if (bUnsignedView) // Values signed/unsigned?
			{
				// UNSIGNED
				if (iBinaryMode == LITTLEENDIAN_MODE)
				{
					// UNSIGNED LITTLEENDIAN_MODE
					// Decimal value of byte.
					if (DataArray.GetLength ()-iCurByte >= 1)
					{
						sprintf (buf2, "\tUnsigned: B:%u", (unsigned int) DataArray[iCurByte]);
						strcat (buf, buf2);
					}
					else
					{
						sprintf (buf2, "\tEND");
						strcat (buf, buf2);
					}
					// Space enough for a word?
					if (DataArray.GetLength ()-iCurByte >= 2)
					{
						// Space enough for a word.
						wordval = (DataArray[iCurByte+1] << 8) | DataArray[iCurByte];
						sprintf (buf2, ",W:%u", (unsigned int) wordval);
						strcat (buf, buf2);
					}
					if (DataArray.GetLength ()-iCurByte >= 4)
					{
						// Space enough for a longword.
						longval = wordval | (((DataArray[iCurByte + 3] << 8) | DataArray[iCurByte + 2]) << 16);
						sprintf (buf2, ",L:%u", (unsigned int) longval);
						strcat (buf, buf2);
					}
				}
				else
				{
					// UNSIGNED BIGENDIAN_MODE
					// Decimal value of byte.
					if (DataArray.GetLength ()-iCurByte >= 1)
					{
						sprintf (buf2, "\tUnsigned: B:%u", (unsigned int) DataArray[iCurByte]);
						strcat (buf, buf2);
					}
					else
					{
						sprintf (buf2, "\tEND");
						strcat (buf, buf2);
					}
					// Space enough for a word?
					if (DataArray.GetLength ()-iCurByte >= 2)
					{
						// Space enough for a word.
						wordval = (DataArray[iCurByte] << 8) | DataArray[iCurByte+1];
						sprintf (buf2, ",W:%u", (unsigned int) wordval);
						strcat (buf, buf2);
					}
					if (DataArray.GetLength ()-iCurByte >= 4)
					{
						// Space enough for a longword.
						longval = (wordval<<16) | (DataArray[iCurByte+2]<<8) | (DataArray[iCurByte+3]);
						sprintf (buf2, ",L:%u", (unsigned int) longval);
						strcat (buf, buf2);
					}
				}
			}
			else // SIGNED
			{
				if (iBinaryMode == LITTLEENDIAN_MODE)
				{
					// SIGNED LITTLEENDIAN_MODE
					// Decimal value of byte.
					if (DataArray.GetLength ()-iCurByte >= 1)
					{
						sprintf (buf2, "\tSigned: B:%d", (int) (signed char) DataArray[iCurByte]);
						strcat (buf, buf2);
					}
					else
					{
						sprintf (buf2, "\tEND");
						strcat (buf, buf2);
					}
					// Space enough for a word?
					if (DataArray.GetLength ()-iCurByte >= 2)
					{
						// Space enough for a word.
						wordval = (DataArray[iCurByte + 1] << 8) | DataArray[iCurByte];
						sprintf (buf2, ",W:%d", (int) (signed short) wordval);
						strcat (buf, buf2);
					}
					if (DataArray.GetLength ()-iCurByte >= 4)
					{
						// Space enough for a longword.
						longval = wordval | (((DataArray[iCurByte + 3] << 8) | DataArray[iCurByte + 2]) << 16);
						sprintf (buf2, ",L:%d", (signed int) longval);
						strcat (buf, buf2);
					}
				}
				else
				{
					// SIGNED BIGENDIAN_MODE
					// Decimal value of byte.
					if (DataArray.GetLength ()-iCurByte >= 1)
					{
						sprintf (buf2, "\tSigned: B:%d", (signed char) DataArray[iCurByte]);
						strcat (buf, buf2);
					}
					else
					{
						sprintf (buf2, "\tEND");
						strcat (buf, buf2);
					}
					// Space enough for a word.
					if (DataArray.GetLength ()-iCurByte >= 2)
					{
						// Space enough for a longword.
						wordval = (DataArray[iCurByte] << 8) | DataArray[iCurByte+1];
						sprintf (buf2, ",W:%d", (int) (signed short) wordval);
						strcat (buf, buf2);
					}
					if (DataArray.GetLength ()-iCurByte >= 4)
					{
						// Space enough for a longword.
						longval = (wordval<<16) | (DataArray[iCurByte+2]<<8) | (DataArray[iCurByte+3]);
						sprintf (buf2, ",L:%d", (signed int) longval);
						strcat (buf, buf2);
					}
				}
			}
			SendMessage (hwndStatusBar, SB_SETTEXT, 0, (LPARAM) buf);
		}
		// Character set, input mode or read-only, binary mode.
		switch (iCharacterSet)
		{
		case ANSI_FIXED_FONT:
			sprintf (buf, "\tANSI");
			break;

		case OEM_FIXED_FONT:
			sprintf (buf, "\tOEM");
			break;
		}

		if (bReadOnly)
		{
			strcat (buf, " / READ");
		}
		else if (iInsertMode)
		{
			strcat(buf, " / INS");
		}
		else
		{
			strcat(buf, " / OVR");
		}
		if (iBinaryMode == LITTLEENDIAN_MODE)
		{
			strcat(buf, " / L"); // Intel
		}
		else if (iBinaryMode == BIGENDIAN_MODE)
		{
			strcat(buf, " / B"); // Motorola
		}
		SendMessage(hwndStatusBar, SB_SETTEXT, 1, (LPARAM) buf);

		// File size.
		sprintf(buf, "\tSize: %u", DataArray.GetLength ());
		SendMessage(hwndStatusBar, SB_SETTEXT, 2, (LPARAM) buf);
	}
	else
	{
		if (hwndMain)
			SetWindowText(hwndMain, appname);
		SendMessage(hwndStatusBar, WM_SETTEXT, 0, (LPARAM) "No file loaded");
	}
}

//--------------------------------------------------------------------------------------------
// Set Caret position.
void HexEditorWindow::set_caret_pos ()
{
	set_wnd_title ();
	if (bSelected)
	{
		SetCaretPos (-cxChar, -cyChar );
		return;
	}

	int iCaretLine = iCurByte / iBytesPerLine,
		iBottomLine = iVscrollPos + cyBuffer - 1;

	switch (iEnteringMode)
	{
	case CHARS:
		if (iCaretLine >= iVscrollPos && iCaretLine <= iBottomLine && filename[0] != '\0')
		{
			int y = iCaretLine - iVscrollPos;
//Pabs replaced "iOffsetLen" with "iMaxOffsetLen"
			int x = iMaxOffsetLen+iByteSpace+iBytesPerLine*3+iCharSpace
//end
				- iHscrollPos + (iCurByte%iBytesPerLine);
			SetCaretPos ( x*cxChar, y*cyChar );
		}
		else
			SetCaretPos ( -cxChar, -cyChar );
		break;
	case BYTES:
		// If caret in vertical visible area...
		if (iCaretLine >= iVscrollPos && iCaretLine <= iBottomLine && filename[0] != '\0')
		{
			int y = iCaretLine - iVscrollPos;
//Pabs replaced "iOffsetLen" with "iMaxOffsetLen"
			int x = iMaxOffsetLen+iByteSpace + (iCurByte%iBytesPerLine)*3 - iHscrollPos + iCurNibble;
//end
			SetCaretPos ( x*cxChar, y*cyChar );
		}
		else
			SetCaretPos ( -cxChar, -cyChar );
	}
}

//--------------------------------------------------------------------------------------------
// Repaints the whole window.
void HexEditorWindow::repaint( int from, int to )
{
	if (hwnd == GetFocus())
	{
		set_caret_pos();
		if (sibling != this)
			sibling->repaint(from, to);
	}

	RECT rc;
	GetClientRect(hwnd, &rc);
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
	InvalidateRect(hwnd, &rc, FALSE);
	UpdateWindow(hwnd);
}

void HexEditorWindow::repaint(int line)
{
	repaint(line, line);
}

//--------------------------------------------------------------------------------------------
// Clear everything up.
void HexEditorWindow::clear_all ()
{
//Pabs changed "iOffsetLen" replaced with "iMinOffsetLen = iMaxOffsetLen" and '8' with '6'
	iMaxOffsetLen = iMinOffsetLen = 6;
//end
	iByteSpace = 2;
	iBytesPerLine = 16;
	iCharSpace = 1;
//Pabs replaced "iOffsetLen" with "iMaxOffsetLen"
//	iCharsPerLine = iMaxOffsetLen + iByteSpace + iBytesPerLine*3 + iCharSpace + iBytesPerLine;
//end
	DataArray.ClearAll ();
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
	SI.nMin = 0;
	SI.nMax = iVscrollMax;
	SI.nPos = iVscrollPos;
	SetScrollInfo(hwnd, SB_VERT, &SI, TRUE);
}

//--------------------------------------------------------------------------------------------
// Set the horizontal scrollbar position.
void HexEditorWindow::adjust_hscrollbar()
{
	SCROLLINFO SI;
	SI.cbSize = sizeof SI;
	SI.fMask = SIF_RANGE | SIF_POS | SIF_DISABLENOSCROLL;
	SI.nMin = 0;
	SI.nMax = iHscrollMax;
	SI.nPos = iHscrollPos;
	SetScrollInfo(hwnd, SB_HORZ, &SI, TRUE);
}

//--------------------------------------------------------------------------------------------
// Highlight (invert) the character/byte at the current offset.
void HexEditorWindow::mark_char(HDC hdc)
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
	if (hdc)
		InvertRect(hdc, &r);
	else
		InvalidateRect(hwnd, &r, FALSE);
}

void HexEditorWindow::print_text(HDC hdc, int x, int y, char *pch, int cch)
{
	RECT rc = { x * cxChar, y * cyChar, rc.left + cch * cxChar, rc.top + cyChar };
	ExtTextOut(hdc, rc.left, rc.top, ETO_OPAQUE, &rc, pch, cch, 0);
}

//--------------------------------------------------------------------------------------------
// Repaint one line in the window.
void HexEditorWindow::print_line(HDC hdc, int line, HBRUSH hbr)
{
	// line = absolute line number.
	int startpos = line * iBytesPerLine;

	// Return if this line does not even contain the end-of-file double
	// underscore (at index upperbound+1).
	assert(startpos <= DataArray.GetLength());

	int length = get_length();
	unsigned char *buffer = get_buffer(length);
	int sibling_length = sibling->get_length();
	unsigned char *sibling_buffer = sibling->get_buffer(sibling_length);

	char linbuf[16];

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

	// Write offset.
	int i = sprintf(linbuf, "%*.*x", iMinOffsetLen, iMinOffsetLen, bPartialStats ? startpos + iPartialOffset : startpos);
	memset(linbuf + i, ' ', m - i);

	SetTextColor(hdc, iTextColor);
	SetBkColor(hdc, iBkColor);
	print_text(hdc, x, y, linbuf, m);

	RECT r;

	x += m;
	int z = x + (iByteSpace + 1) * iBytesPerLine + iCharSpace;

	// Write offset to chars.
	m = iCharSpace + 1;
	memset(linbuf, ' ', sizeof linbuf);
	SetTextColor (hdc, iTextColor);
	SetBkColor (hdc, iBkColor);
	print_text(hdc, z - m, y, linbuf, m);
	// Last line reached? Then only write rest of bytes.
	// startpos+iBytesPerLine-1 = Last byte in current line.
	int endpos = startpos + iBytesPerLine - 1;
	// Write bytes.
	for (i = startpos ; i <= endpos ; i++)
	{
		unsigned char u = ' ';
		if (i < length)
		{
			u = buffer[i];
			char c = (char)(u >> 4);
			if( c < 10 )
				c += '0';
			else
				c = (char)(c - 10 + 'a');
			linbuf[0] = c;
			c = (char)(u & 0x0f);
			if( c < 10 )
				c += '0';
			else
				c = (char)(c - 10 + 'a');
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
			SetTextColor(hdc, bDiff ? iSelDiffTextColor : iSelTextColor);
			SetBkColor(hdc, bDiff ? iSelDiffBkColor : iSelBkColor);
		}
		else
		{
			SetTextColor(hdc, bDiff ? iDiffTextColor : iTextColor);
			SetBkColor(hdc, bDiff ? iDiffBkColor : iBkColor);
		}
		m = iByteSpace;
		if (i < endpos)
			++m;
		print_text(hdc, x, y, linbuf, m);
		x += m;
		print_text(hdc, z, y, (char *)&u, 1);
		z += 1;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Separators.
	r.top = (line - iVscrollPos) * cyChar;
	r.bottom = r.top + cyChar;
	for (i = 0 ; i <= iBytesPerLine ; i += 4)
	{
		r.left = r.right = (iMaxOffsetLen + iByteSpace - iHscrollPos + i * 3) * cxChar - cxChar / 2;
		Polyline(hdc, (LPPOINT)&r, 2);
	}
	// Separator for chars.
	r.left = r.right = (iMaxOffsetLen + iByteSpace - iHscrollPos + iBytesPerLine * 3 + iCharSpace) * cxChar - cxChar / 2 - 1;
	Polyline(hdc, (LPPOINT)&r, 2);
	// Second separator.
	OffsetRect(&r, 2, 0);
	Polyline(hdc, (LPPOINT)&r, 2);

	// Print bookmark indicators.
	// Are there bookmarks in this line?
	int el = startpos + iBytesPerLine - 1;
	int chpos;
	// Brush for bookmark borders.
	for( i = 0; i < iBmkCount; i++ )
	{
		int offset = pbmkList[i].offset;
		// Print the bookmark if it is within the file.
		if (offset >= startpos && offset <= el && offset < length)
		{
			// Found a bookmark in this line.
			// Mark hex.
//Pabs replaced "iOffsetLen" with "iMaxOffsetLen"
			chpos = iMaxOffsetLen + iByteSpace + (offset % iBytesPerLine) * 3 - iHscrollPos;
//end
			r.left = chpos * cxChar;
			r.top = (offset / iBytesPerLine - iVscrollPos) * cyChar;
			r.right = r.left + 2*cxChar;
			r.bottom = (offset / iBytesPerLine - iVscrollPos + 1) * cyChar;
			FrameRect(hdc, &r, hbr);

			// Mark char.
//Pabs replaced "iOffsetLen" with "iMaxOffsetLen"
			chpos = iMaxOffsetLen + iByteSpace + iBytesPerLine * 3 + iCharSpace
//end
				+ (offset % iBytesPerLine) - iHscrollPos;
			r.left = chpos * cxChar;
			r.top = (offset / iBytesPerLine - iVscrollPos) * cyChar;
			r.right = (chpos + 1) * cxChar;
			r.bottom = (offset / iBytesPerLine - iVscrollPos + 1) * cyChar;
			FrameRect(hdc, &r, hbr);
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
		return !bReadOnly;
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
		return DataArray.GetLength() != 0;
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
		// "Delete" is allowed if there is a selection or the caret is on a byte.
		// It is not allowed in read-only mode.
		return (bSelected || iCurByte < DataArray.GetLength()) && !bReadOnly;
	case IDM_EDIT_COPY:
		// "Copy" is allowed if there is a selection or the caret is on a byte.
		return bSelected || iCurByte < DataArray.GetLength();
	case IDM_EDIT_PASTE:
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
		// "Append" is allowed if read-only is disabled
		// and there is no selection going on.
		return !bReadOnly && !bSelected;
	case IDM_FILL_WITH:
		//"Fill with" is allowed if the file is not empty and read-only is disabled.
		//If there is no selection the whole file is filled
	case IDM_REPLACE:
		// "Replace" is allowed if the file is not empty and read-only is disabled.
		return DataArray.GetLength() && !bReadOnly;
	case IDM_FINDNEXT:
	case IDM_FINDPREV:
		// "Find next" and "Find previous" are allowed if the file is not empty,
		// and there is a findstring OR there is a selection
		// (which will be searched for).
		return DataArray.GetLength() && (FindDlg::pcFindDlgBuffer || bSelected);
	case IDM_EDIT_ENTERDECIMALVALUE:
		// "Enter decimal value" is allowed if read-only is disabled, the file is not empty,
		// the caret is on a byte and there is no selection going on.
	case IDM_EDIT_MANIPULATEBITS:
		// "Manipulate bits" is allowed if the caret is on a byte, read-only is disabled
		// and there is no selection going on.
		return !bReadOnly && iCurByte < DataArray.GetLength() && !bSelected;
	case IDM_COMPARE:
		// "Compare from current offset" is allowed if the caret is on a byte
		// and there is no selection going on.
	case IDM_READFLOAT:
		// "Get floating point value" is allowed if the caret is on a byte
		// and there is no selection going on.
	case IDM_APPLYTEMPLATE:
		// "Apply template" is allowed if the caret is on a byte
		// and there is no selection going on.
		return iCurByte < DataArray.GetLength() && !bSelected;
	case IDM_DEFAULT:
		return unknownpresent();
	case IDM_UPGRADE:
		return oldpresent();
	case IDM_REMOVE:
		return frhedpresent() || unknownpresent() || contextpresent();
	case IDM_ADDBOOKMARK:
		// "Add bookmark" is allowed if the file is not
		// empty and there is no selection going on.
		return DataArray.GetLength() && !bSelected;
	case IDM_REMOVE_BKM:
	case IDM_CLEARALL_BMK:
		// "Remove bookmark" and "Clear all bookmarks" are allowed if there are bookmarks set.
		return iBmkCount > 0;
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

//--------------------------------------------------------------------------------------------
// Handler on window closing.
int HexEditorWindow::close(const char *caption)
{
	if (iFileChanged)
	{
		int res = MessageBox(hwnd, "Do you want to save your changes?",
			caption ? caption : "Exit", MB_YESNOCANCEL | MB_ICONQUESTION);
		if (res == IDCANCEL || res == IDYES && !(bFileNeverSaved ? CMD_save_as() : CMD_save()))
			//User doesn't want to quit or User wants to save and the save was unsuccessful
			return 0;//Don't exit
	}
	return 1;
}

//--------------------------------------------------------------------------------------------
INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		TranslateDialog(hDlg);
		// Set the version information.
		SetDlgItemText(hDlg, IDC_STATIC1,
			"frhed - free hex editor for 32-bit Windows\nVersion "CURRENT_VERSION"."
			SUB_RELEASE_NO"."BUILD_NO"\n(c) Raihan Kibria 2000"
			"\nFill with by Pabs Dec 1999"
			"\nDisk-Access, Code/Decode Extension and some other bits by Gerson Kurz."
			"\nDLL interface by Jochen Neubeck.");
		// Set the email-addresses.
		SetDlgItemText(hDlg, IDC_EDIT1,
			"rkibria@hrz1.hrz.tu-darmstadt.de"
			"\r\nPabs: pabs3@zip.to");
		// Set the homepage URL.
		SetDlgItemText(hDlg, IDC_EDIT2, "http://www.kibria.de");
		// Set the icon.
		if (HWND hwndParent = GetParent(hDlg))
			if (DWORD dwIcon = GetClassLong(hwndParent, GCLP_HICON))
				SendDlgItemMessage(hDlg, IDC_APPICON, STM_SETICON, dwIcon, 0);
		return TRUE;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
		case IDCANCEL:
			EndDialog(hDlg, wParam);
			return TRUE;

		case IDC_BUTTON1:
			{
				HINSTANCE hi = ShellExecute(hDlg, "open", "http://www.kibria.de", 0, NULL, SW_SHOWNORMAL);
				if ((UINT)hi <= HINSTANCE_ERROR)
					MessageBox(hDlg, "Could not call browser.", "Go to homepage", MB_ICONERROR);
			}
		}
		break;
	}
	return FALSE;
}

//--------------------------------------------------------------------------------------------
// Required for the find function.
char HexEditorWindow::equal(char c)
{
	return c;
}

char HexEditorWindow::lower_case(char c)
{
	if (c >= 'A' && c <= 'Z')
		c = (char)('a' + c - 'A');
	return c;
}

//--------------------------------------------------------------------------------------------
// FIND_BYTES
// Arguments:
// ps   = Start position.
// ls   = Length of src array.
// pb   = Start of searchstring.
// lb   = Length searchstring.
// mode = -1 : backwards search.
//         1 : forward search.
// cmp  = pointer to function that is applied to data before comparing.
//
// Return:
// Position of found string or -1 if not there.

int HexEditorWindow::find_bytes(char* ps, int ls, char* pb, int lb, int mode, char (*cmp)(char))
{
	int start, inc, end, i;
	if (mode == 1)
	{
		start = 0;
		inc = 1;
		end = ls - lb + 1;
	}
	else
	{
		start = ls - lb;
		inc = -1;
		end = 1;
	}

	for (; mode * start < end; start += inc)
	{
		for (i = start; i < start + lb; i++)
		{
			if (cmp (ps[i]) != cmp (pb[i - start]))
				break;
		}
		if (i == start + lb)
			return start;
	}

	return -1;
}


//Pabs rewrote lbuttonup, mousemove, lbuttondown for drag-drop editing
//--------------------------------------------------------------------------------------------
// WM_LBUTTONUP handler.
int HexEditorWindow::lbuttonup (int xPos, int yPos)
{
	// Kill timer.
	kill_scroll_timers();
	KillTimer (hwnd, MOUSE_OP_DELAY_TIMER_ID);
	bMouseOpDelayTimerSet = FALSE;
	// Release mouse.
	if (GetCapture() != hwnd)
		return 0;
	ReleaseCapture ();

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
			if(iCurNibble==2)return 0;
			iEnteringMode = BYTES;
			break;
		default:
			iEnteringMode = CHARS;
		break;
		}

		SetCursor( LoadCursor( NULL, IDC_IBEAM ) );
		if(!bSelected){
			int a = iCurByte/iBytesPerLine;
			int b = new_pos/iBytesPerLine;
			iCurByte = new_pos;
			//if(a!=b)repaint(b);
			repaint(a,b);
		}
		else{
			int a = iStartOfSelection/iBytesPerLine;
			int b = iEndOfSelection/iBytesPerLine;
			if(a>b)swap(a,b);
			bSelected = FALSE;
			iCurByte = new_pos;
			int c = iCurByte/iBytesPerLine;
			if(c<a||c>b)repaint(c);
			repaint(a,b);
		}
	}

	//dragging = false;

	bSelecting = FALSE;

	return 0;
}

//--------------------------------------------------------------------------------------------
// WM_MOUSEMOVE handler.
int HexEditorWindow::mousemove(int xPos, int yPos)
{
	iMouseX = xPos;
	iMouseY = yPos;

	bool bLButtonDown = GetCapture() == hwnd && GetKeyState(VK_LBUTTON) < 0;

	if (!bLButtonDown || bSelecting)
		get_pos(xPos, yPos);

	if (bLButtonDown)
	{
		if (bSelecting)
		{
			fix_scroll_timers(xPos, yPos);

			int lastbyte = DataArray.GetUpperBound();
			if (new_pos > lastbyte)
				new_pos = lastbyte;
			if (iEndOfSelection != new_pos)
			{
				bSelected = TRUE;
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
int HexEditorWindow::lbuttondown ( int nFlags, int xPos, int yPos )
{
	iLBDownX = xPos;
	iLBDownY = yPos;

	if (filename[0] == '\0')
		return 0;
	SetCapture( hwnd );

//Pabs inserted - after reading the fuzz report
	//Someone sent us invalid data
	if ( ( nFlags & ( ~ (MK_CONTROL|MK_LBUTTON|MK_MBUTTON|MK_RBUTTON|MK_SHIFT) ) ) ||
		( xPos < 0 || xPos > cxClient ) ||
		( yPos < 0 || yPos > cyClient ) ){
		return 0;
	}
//end
	get_pos(xPos, yPos);
	switch (area)
	{
		case AREA_OFFSETS:
			if(!bAutoOffsetLen && iMinOffsetLen>1)
			{
				iMinOffsetLen--;
				save_ini_data ();
				resize_window ();
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
			int length = DataArray.GetLength();
			if (new_pos == length)
				new_pos--;
			if (new_pos < length)
			{
				bSelecting = TRUE;
				if (iEndOfSelection != new_pos)
				{
					int oeos = iEndOfSelection;
					iEndOfSelection = new_pos;
					if (!bSelected)
					{
						bSelected = TRUE;
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
		SetTimer( hwnd, MOUSE_OP_DELAY_TIMER_ID, MouseOpDelay, NULL );
		bMouseOpDelayTimerSet = TRUE;
//		SetCapture( hwnd );
	}

	return 0;
}
//end

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

	int lastbyte = DataArray.GetUpperBound();
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
	int lastbyte = DataArray.GetUpperBound();
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

	if( new_pos > lastbyte+1 )
	{
		new_pos = lastbyte+1;
		update = 1;
	}
	else if( new_pos < 0 )
	{
		new_pos = 0;
		update = 1;
	}


	if( update ){
		bytenum = new_pos % iBytesPerLine;
		line = new_pos / iBytesPerLine;
	}

	y = line;
	x = bytenum;

	if( area == AREA_BYTES ){
		x = x*3 + iStartofBytes - 1;
	}
	else if( area == AREA_CHARS ){
		x = iGetCharsPerLine() - iBytesPerLine + x;
	}

	x -= iHscrollPos;
	y -= iVscrollPos;

	if ( x != old_col || y != old_row ){
		old_col = x; old_row = y;

		x = x*cxChar;
		y = y*cyChar;
		if( area == AREA_BYTES ){
			x += cxChar / 2;
		}

		//Set caret position & show it
		SetCaretPos(x, y);
		ShowCaret( hwnd );
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
				SetTimer(hwnd, SCROLL_DELAY_TIMER_ID, ScrollDelay, NULL);
			}
			else
			{
				bScrollTimerSet = TRUE;
				SetTimer(hwnd, SCROLL_TIMER_ID, ScrollInterval, NULL);
			}
		}
		else if (dragging)
		{
			if (enable_scroll_delay_dd)
			{
				SetTimer(hwnd, SCROLL_DELAY_TIMER_ID, ScrollDelay, NULL);
			}
			else
			{
				bScrollTimerSet = TRUE;
				SetTimer(hwnd, SCROLL_TIMER_ID, ScrollInterval, NULL);
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
	KillTimer(hwnd, SCROLL_DELAY_TIMER_ID);
	KillTimer(hwnd, SCROLL_TIMER_ID);
	bScrollTimerSet = FALSE;
	prev_vert = prev_horz = SCROLL_NONE;
}

//end

//-------------------------------------------------------------------
// On find command.
void HexEditorWindow::CMD_find()
{
	static_cast<dialog<FindDlg>*>(this)->DoModal(hwnd);
}

//-------------------------------------------------------------------
// On copy command.
void HexEditorWindow::CMD_edit_copy()
{
	static_cast<dialog<CopyDlg>*>(this)->DoModal(hwnd);
}

//-------------------------------------------------------------------
// On hexdump to file/clipboard command.
int HexEditorWindow::CMD_copy_hexdump(int iCopyHexdumpMode, int iCopyHexdumpType,
	int iCopyHexdumpDlgStart, int iCopyHexdumpDlgEnd, char* mem, int memlen)
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
	int buflen;
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
		if (mem && buflen > memlen) return 0;
//end
		// Create hexdump.
		char buf2[128];
//Pabs changed - "char*" removed - see further up
		if (!mem)
		{
			pMem = new char[buflen];
			if(!pMem) return 0;
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
				if (a + j > DataArray.GetUpperBound())
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
					sprintf(buf2, "%2.2x ", DataArray[a + j]);
					pMem[k + l + j*3    ] = buf2[0];
					pMem[k + l + j*3 + 1] = buf2[1];
					pMem[k + l + j*3 + 2] = buf2[2];
					// Write char.
					if( iCharacterSet == OEM_FIXED_FONT && DataArray[a + j] != 0 )
						pMem[k + l + iBytesPerLine*3 + iCharSpace + j] = DataArray[a + j];
					else if( (DataArray[a + j] >= 32 && DataArray[a + j] <= 126) || (DataArray[a + j]>=160 && DataArray[a + j] <= 255) || (DataArray[a + j] >= 145 && DataArray[a + j] <= 146) )
						pMem[k + l + iBytesPerLine*3 + iCharSpace + j] = DataArray[a + j];
					else
						pMem[k + l + iBytesPerLine*3 + iCharSpace + j] = '.';
				}
			}
			pMem[k + iCharsPerLine] = '\r';
			pMem[k + iCharsPerLine + 1] = '\n';
		}
		pMem[buflen-1] = '\0';
//Pabs changed - line insert
	}
	else if (iCopyHexdumpType == IDC_EXPORTDIGITS)
	{
		// Req'd mem for lines:
		int numchar = iCopyHexdumpDlgEnd - iCopyHexdumpDlgStart + 1;
		buflen = numchar * 2 + 1;
		if( mem && buflen > memlen ) return 0;
		// Create hexdump.
		if (!mem)
		{
			pMem = new char[buflen];
			if (!pMem)
				return 0;
		}
		for (int i = 0 ; i < numchar ; ++i)
		{
			sprintf(pMem + i * 2, "%2.2x", DataArray[iCopyHexdumpDlgStart + i]);
		}
		pMem[buflen - 1] = '\0';
	}
	else if (iCopyHexdumpType == IDC_EXPORTRTF)
	{
		if (mem)
			return 0;
		//Bit of a kludge here
		pMem = (char*)RTF_hexdump(iCopyHexdumpDlgStart, iCopyHexdumpDlgEnd, (DWORD*)&buflen);
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
		char szFileName[_MAX_PATH];
		strcpy(szFileName, "hexdump.txt");
		HGLOBAL hg = NULL;
		if( iCopyHexdumpType == IDC_EXPORTRTF ){
			hg = (HGLOBAL)pMem;
			pMem = (char*)GlobalLock(hg);
			if( !pMem ){
				GlobalFree( hg );
				return 0;
			}
			strcpy(&szFileName[8],"rtf");
		}

		// to file.
		OPENFILENAME ofn;
		ZeroMemory(&ofn, sizeof ofn);
		ofn.lStructSize = sizeof ofn;
		ofn.hwndOwner = hwnd;
		ofn.lpstrFilter = "All Files (*.*)\0*.*\0\0";
		ofn.lpstrFile = szFileName;
		ofn.nMaxFile = _MAX_PATH;
		ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT;
		if (GetSaveFileName(&ofn))
		{
			int filehandle = _open(szFileName, _O_RDWR|_O_CREAT|_O_TRUNC|_O_BINARY, _S_IREAD|_S_IWRITE);
			if (filehandle != -1)
			{
				// Write file.
				if (_write(filehandle, pMem, buflen - 1) != -1)
					MessageBox(hwnd, "Hexdump saved.", "Export hexdump", MB_ICONINFORMATION);
				else
					MessageBox(hwnd, "Could not save Hexdump.", "Export hexdump", MB_ICONERROR);
				_close(filehandle);
			}
			else
				MessageBox(hwnd, "Could not save Hexdump.", "Export hexdump", MB_ICONERROR);
		}//end
		if (iCopyHexdumpType == IDC_EXPORTRTF)
		{
			GlobalUnlock( hg );
			GlobalFree( hg );
			hg = pMem = NULL;
		}
	}
	else if (iCopyHexdumpType != IDC_EXPORTRTF)
	{
		// To clipboard.
		if (HGLOBAL hGlobal = GlobalAlloc(GHND, buflen))
		{
			char *pDest = (char*)GlobalLock(hGlobal);
			memcpy (pDest, pMem, buflen);
			GlobalUnlock (hGlobal);
			OpenClipboard (hwnd);
			EmptyClipboard ();
			SetClipboardData (CF_TEXT, hGlobal);
			CloseClipboard ();
		}
		else
			MessageBox (hwnd, "Not enough memory for hexdump to clipboard.", "Export hexdump", MB_ICONERROR);
	}
	else //if (iCopyHexdumpType == IDC_EXPORTRTF)
	{
		// To clipboard.
		if (OpenClipboard(hwnd))
		{
			EmptyClipboard();
			SetClipboardData(CF_RICH_TEXT_FORMAT, (HGLOBAL)pMem);
			CloseClipboard();
		}
		else
			MessageBox (hwnd, "Could not hexdump to clipboard.", "Export hexdump", MB_ICONERROR);
		pMem = NULL;
	}
	delete [] pMem;
	return 1;
}

void HexEditorWindow::CMD_copy_hexdump()
{
	static_cast<dialog<CopyHexdumpDlg>*>(this)->DoModal(hwnd);
}

//-------------------------------------------------------------------
// On paste command.
void HexEditorWindow::CMD_edit_paste()
{
	static_cast<dialog<PasteDlg>*>(this)->DoModal(hwnd);
}

//-------------------------------------------------------------------
// On "enter decimal value" command.
void HexEditorWindow::CMD_edit_enterdecimalvalue()
{
	static_cast<dialog<EnterDecimalValueDlg>*>(this)->DoModal(hwnd);
}

//-------------------------------------------------------------------
// Create translation of bytecode-string.
// Return: Length of resulting string.
// ppd = pointer to pointer to result, must be delete[]-ed later.
// If the input string was empty, no translated array is created and zero is returned.
int HexEditorWindow::create_bc_translation(char **ppd, char *src, int srclen)
{
	int destlen = Text2BinTranslator::iLengthOfTransToBin(src, srclen);
	if (destlen > 0)
	{
		*ppd = new char[destlen];
		Text2BinTranslator::iCreateBcTranslation(*ppd, src, srclen, iCharacterSet, iBinaryMode);
		return destlen;
	}
	else
	{
		// Empty input string => don't allocate anything and return 0.
		*ppd = NULL;
		return 0;
	}
}

//-------------------------------------------------------------------
void HexEditorWindow::CMD_edit_cut(int iMode)
{
	CutDlg::iCutMode = iMode;
	static_cast<dialog<CutDlg>*>(this)->DoModal(hwnd);
}

void HexEditorWindow::CMD_edit_cut()
{
	CMD_edit_cut(1);
}

void HexEditorWindow::CMD_edit_clear()
{
	CMD_edit_cut(0);
}

//-------------------------------------------------------------------
int HexEditorWindow::CMD_new(const char *title)
{
	if (!close(title))
		return 0;
	EnableDriveButtons(FALSE);

	delete Drive;
	Drive = 0;

	bFileNeverSaved = TRUE;
	bSelected = FALSE;
//Pabs replaced bLButtonIsDown with bSelecting & inserted
	//bMoving = FALSE;
	bSelecting = FALSE;
	//bDroppedHere = FALSE;
//end
	iFileChanged = FALSE;
	bFilestatusChanged = TRUE;
	iVscrollMax = 0;
	iVscrollPos = 0;
	iHscrollMax = 0;
	iHscrollPos = 0;
	iCurByte = 0;
	iCurNibble = 0;
	bPartialStats = 0;
	bPartialOpen = FALSE;
	// Delete old data.
	DataArray.ClearAll();
	strcpy(filename, "Untitled");
	resize_window();
	return 1;
}

//-------------------------------------------------------------------
int HexEditorWindow::CMD_save_as()
{
	char szFileName[_MAX_PATH];
	szFileName[0] = '\0';
	if (Drive == 0)
		strcpy(szFileName, filename);
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof ofn);
	ofn.lStructSize = sizeof ofn;
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "All Files (*.*)\0*.*\0\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = _MAX_PATH;
	ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	if (!GetSaveFileName(&ofn))
		return 0;
	WaitCursor w1;
	int filehandle = _open(szFileName, _O_RDWR|_O_CREAT|_O_TRUNC|_O_BINARY, _S_IREAD|_S_IWRITE);
	if (filehandle == -1)
	{
		MessageBox(hwnd, "Could not save file.", "Save as", MB_ICONERROR);
		return 0;
	}
	int done = 0;
	if (_write(filehandle, DataArray, DataArray.GetLength()) != -1)
	{
		// File was saved.
		GetLongPathNameWin32(szFileName, filename);
		iFileChanged = FALSE;
		bFilestatusChanged = TRUE;
		bFileNeverSaved = FALSE;
		bPartialStats = 0;
		bPartialOpen = FALSE;
		update_MRU();
		done = 1;
	}
	else
	{
		MessageBox(hwnd, "Could not save file.", "Save as", MB_ICONERROR);
	}
	_close(filehandle);
	set_wnd_title();
	return done;
}
//-------------------------------------------------------------------
//Pabs changed this function so it would return 0 when unsuccessful
int HexEditorWindow::CMD_save()
{
	WaitCursor w1;

	// File was not saved before => name must be chosen.
	if (bFileNeverSaved)
		return CMD_save_as();

//Pabs inserted
	if (bMakeBackups)
	{//Assume the filename is valid & has a length
		int len = strlen(filename);
		char newname[sizeof filename + 4];
		strcpy(newname, filename);
		strcat(newname, ".bak");
		remove(newname);
		//Must use Win32 here as the CRT has no copy function only rename
		//& we need a copy of the file to be present for saving a partially opened file
		if (!CopyFile(filename, newname, TRUE))
		{
			MessageBox(hwnd,
				"Could not backup file\n"
				"Backup aborted, Save continuing", "Backup", MB_ICONWARNING);
		}
	}
	int done = 0;
	// File is partially loaded => must be saved partially or saved as.
	if (bPartialOpen)
	{
		int filehandle = _open(filename, _O_RDWR|_O_BINARY);
		if (filehandle == -1)
		{
			MessageBox(hwnd, "Could not save partially opened file.", "Save", MB_ICONERROR);
			return 0;
		}
		int nbl = DataArray.GetLength(); // Length of the DataArray
		if (nbl != iPartialOpenLen)
		{
			int i = iPartialOffset + iPartialOpenLen; // loop var & start of loop
			int e = iPartialFileLen - 1; // end of loop
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
				_lseek(filehandle, i, SEEK_SET);
				_read(filehandle, &tmp, 1);
				_lseek(filehandle, i + r, SEEK_SET);
				if (-1 == _write(filehandle, &tmp, 1))
				{
					MessageBox(hwnd, "Could not move data in the file.", "Save", MB_ICONERROR);
					_close(filehandle);
					return 0;
				}
				i += n;
			} while (i != e);
			if (nbl < iPartialOpenLen)
			{//If the new file is bigger than the first _write will resize the file properly otherwise we need to specifically resize the file
				if (-1 == _chsize(filehandle, iPartialFileLen + r))
				{
					MessageBox(hwnd, "Could not resize the file.", "Save", MB_ICONERROR);
					_close(filehandle);
					return 0;
				}
			}
		}
		if (_lseek(filehandle, iPartialOffset, 0) == -1)
		{
			MessageBox(hwnd, "Could not seek in file.", "Save", MB_ICONERROR);
		}
		else if (_write( filehandle, DataArray, nbl) == -1)
		{
			MessageBox(hwnd, "Could not write data to file.", "Save", MB_ICONERROR);
		}
		else
		{
			done = 1;
			iFileChanged = FALSE;
			bFilestatusChanged = TRUE;
			set_wnd_title();
		}
		_close(filehandle);
		return done;
	}

	int filehandle = _open(filename, _O_RDWR|_O_CREAT|_O_TRUNC|_O_BINARY, _S_IREAD|_S_IWRITE);
	if (filehandle == -1)
	{
		MessageBox(hwnd, "Could not save file.", "Save", MB_ICONERROR);
		return 0;
	}
	if (_write(filehandle, DataArray, DataArray.GetLength()) == -1)
	{
		MessageBox(hwnd, "Could not write data to file.", "Save", MB_ICONERROR);
	}
	else
	{
		done = 1;
		iFileChanged = FALSE;
		bFilestatusChanged = TRUE;
		bPartialStats = 0;
		bPartialOpen = FALSE;
		set_wnd_title();
	}
	_close(filehandle);
	return done;
}

//-------------------------------------------------------------------
void HexEditorWindow::CMD_open()
{
	if (!close("Open"))
		return;
	char szFileName[_MAX_PATH];
	szFileName[0] = '\0';
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof ofn);
	ofn.lStructSize = sizeof ofn;
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "All Files (*.*)\0*.*\0\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = _MAX_PATH;
	ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT;
	if (GetOpenFileName(&ofn))
		load_file(szFileName);
}

//-------------------------------------------------------------------
void HexEditorWindow::adjust_view_for_selection()
{
	if( bSelected )
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
	static_cast<dialog<ViewSettingsDlg>*>(this)->DoModal(hwnd);
}

//-------------------------------------------------------------------
// Change the color indicated by pColor.
void HexEditorWindow::CMD_color_settings(COLORREF *pColor)
{
	CHOOSECOLOR cc;
	COLORREF crCustColors[16];
	cc.lStructSize = sizeof (CHOOSECOLOR);
	cc.hwndOwner = hwnd;
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

//-------------------------------------------------------------------
void HexEditorWindow::read_ini_data(char *key)
{
	// Is there any data for frhed in the registry?
	HKEY key1;
	char keyname[64];
	LONG res;
	if (key == 0)
		key = "v"CURRENT_VERSION"." SUB_RELEASE_NO;
	sprintf(keyname, "Software\\frhed\\%s\\%d", key, iInstCount);

	res = RegOpenKeyEx(HKEY_CURRENT_USER, keyname, 0, KEY_ALL_ACCESS, &key1);
	if (res == ERROR_SUCCESS)
	{
		// There is data: read it.
		DWORD datasize = sizeof( int );
		LONG res;
		res = RegQueryValueEx( key1, "iTextColorValue", NULL, NULL, (BYTE*) &iTextColorValue, &datasize );
		res = RegQueryValueEx( key1, "iBkColorValue", NULL, NULL, (BYTE*) &iBkColorValue, &datasize );
		res = RegQueryValueEx( key1, "iSepColorValue", NULL, NULL, (BYTE*) &iSepColorValue, &datasize );
		res = RegQueryValueEx( key1, "iSelTextColorValue", NULL, NULL, (BYTE*) &iSelTextColorValue, &datasize );
		res = RegQueryValueEx( key1, "iSelBkColorValue", NULL, NULL, (BYTE*) &iSelBkColorValue, &datasize );
		res = RegQueryValueEx( key1, "iBmkColor", NULL, NULL, (BYTE*) &iBmkColor, &datasize );

		res = RegQueryValueEx( key1, "iAutomaticBPL", NULL, NULL, (BYTE*) &iAutomaticBPL, &datasize );
		res = RegQueryValueEx( key1, "iBytesPerLine", NULL, NULL, (BYTE*) &iBytesPerLine, &datasize );
		res = RegQueryValueEx( key1, "iOffsetLen", NULL, NULL, (BYTE*) &iMinOffsetLen, &datasize );//Pabs replaced "iOffsetLen" with "iMinOffsetLen"
		res = RegQueryValueEx( key1, "iCharacterSet", NULL, NULL, (BYTE*) &iCharacterSet, &datasize );
		res = RegQueryValueEx( key1, "iFontSize", NULL, NULL, (BYTE*) &iFontSize, &datasize );
		res = RegQueryValueEx( key1, "bOpenReadOnly", NULL, NULL, (BYTE*) &bOpenReadOnly, &datasize );

//Pabs inserted
		res = RegQueryValueEx( key1, "bMakeBackups", NULL, NULL, (BYTE*) &bMakeBackups, &datasize );
		res = RegQueryValueEx( key1, "bAutoOffsetLen", NULL, NULL, (BYTE*) &bAutoOffsetLen, &datasize );
		res = RegQueryValueEx( key1, "enable_drop", NULL, NULL, (BYTE*) &enable_drop, &datasize );
		res = RegQueryValueEx( key1, "enable_drag", NULL, NULL, (BYTE*) &enable_drag, &datasize );
		res = RegQueryValueEx( key1, "enable_scroll_delay_dd", NULL, NULL, (BYTE*) &enable_scroll_delay_dd, &datasize );
		res = RegQueryValueEx( key1, "enable_scroll_delay_sel", NULL, NULL, (BYTE*) &enable_scroll_delay_sel, &datasize );
		res = RegQueryValueEx( key1, "always_pick_move_copy", NULL, NULL, (BYTE*) &always_pick_move_copy, &datasize );
		res = RegQueryValueEx( key1, "prefer_CF_HDROP", NULL, NULL, (BYTE*) &prefer_CF_HDROP, &datasize );
		res = RegQueryValueEx( key1, "prefer_CF_BINARYDATA", NULL, NULL, (BYTE*) &prefer_CF_BINARYDATA, &datasize );
		res = RegQueryValueEx( key1, "prefer_CF_TEXT", NULL, NULL, (BYTE*) &prefer_CF_TEXT, &datasize );
		res = RegQueryValueEx( key1, "output_CF_BINARYDATA", NULL, NULL, (BYTE*) &output_CF_BINARYDATA, &datasize );
		res = RegQueryValueEx( key1, "output_CF_TEXT", NULL, NULL, (BYTE*) &output_CF_TEXT, &datasize );
		res = RegQueryValueEx( key1, "output_text_special", NULL, NULL, (BYTE*) &output_text_special, &datasize );
		res = RegQueryValueEx( key1, "output_text_hexdump_display", NULL, NULL, (BYTE*) &output_text_hexdump_display, &datasize );
//end
		res = RegQueryValueEx( key1, "always_pick_move_copy", NULL, NULL, (BYTE*) &always_pick_move_copy, &datasize );

		LCID lcid = 0;
		res = RegQueryValueEx( key1, "locale", NULL, NULL, (BYTE*) &lcid, &datasize );
		load_lang((LANGID)lcid);

		char szPath[ _MAX_PATH + 1 ];
		datasize = _MAX_PATH + 1;
		res = RegQueryValueEx( key1, "TexteditorName", NULL, NULL, (BYTE*) &szPath, &datasize );
		TexteditorName = szPath;

		datasize = _MAX_PATH + 1;
		strcpy(szPath, "FRHEXDES.DLL;FRHEDX.DLL"); // default
		res = RegQueryValueEx( key1, "EncodeDlls", NULL, NULL, (BYTE*) &szPath, &datasize );
		EncodeDlls = szPath;

		res = RegQueryValueEx( key1, "iWindowShowCmd", NULL, NULL, (BYTE*) &iWindowShowCmd, &datasize );
		res = RegQueryValueEx( key1, "iWindowX", NULL, NULL, (BYTE*) &iWindowX, &datasize );
		res = RegQueryValueEx( key1, "iWindowY", NULL, NULL, (BYTE*) &iWindowY, &datasize );
		res = RegQueryValueEx( key1, "iWindowWidth", NULL, NULL, (BYTE*) &iWindowWidth, &datasize );
		res = RegQueryValueEx( key1, "iWindowHeight", NULL, NULL, (BYTE*) &iWindowHeight, &datasize );

		res = RegQueryValueEx( key1, "iMRU_count", NULL, NULL, (BYTE*) &iMRU_count, &datasize );
		int i;
		for (i = 0 ; i < MRUMAX ; i++)
		{
			char fname[64];
			sprintf(fname, "MRU_File%d", i + 1);
			datasize = _MAX_PATH;
			res = RegQueryValueEx(key1, fname, NULL, NULL, (BYTE*) strMRU[i], &datasize);
		}
		// Close registry.
		RegCloseKey(key1);
	}
	else
	{
		// There is no data. Write with default values.
		MessageBox(hwnd, "Frhed is being started for the first time\n"
			"and will be attempting to write to the registry.", "Initialize registry", MB_OK);
		save_ini_data();//end
	}
}

//-------------------------------------------------------------------
void HexEditorWindow::save_ini_data()
{
	if (!bSaveIni)
		return;//just return if we are not to save ini data

	HKEY key1;

	char keyname[64];
	sprintf(keyname, "Software\\frhed\\v" CURRENT_VERSION "." SUB_RELEASE_NO "\\%d", iInstCount);

	LONG res = RegCreateKey(HKEY_CURRENT_USER, keyname, &key1);

	if (res == ERROR_SUCCESS)
	{
		RegSetValueEx( key1, "iTextColorValue", 0, REG_DWORD, (CONST BYTE*) &iTextColorValue, sizeof( int ) );
		RegSetValueEx( key1, "iBkColorValue", 0, REG_DWORD, (CONST BYTE*) &iBkColorValue, sizeof( int ) );
		RegSetValueEx( key1, "iSepColorValue", 0, REG_DWORD, (CONST BYTE*) &iSepColorValue, sizeof( int ) );
		RegSetValueEx( key1, "iSelTextColorValue", 0, REG_DWORD, (CONST BYTE*) &iSelTextColorValue, sizeof( int ) );
		RegSetValueEx( key1, "iSelBkColorValue", 0, REG_DWORD, (CONST BYTE*) &iSelBkColorValue, sizeof( int ) );
		RegSetValueEx( key1, "iBmkColor", 0, REG_DWORD, (CONST BYTE*) &iBmkColor, sizeof( int ) );

		RegSetValueEx( key1, "iAutomaticBPL", 0, REG_DWORD, (CONST BYTE*) &iAutomaticBPL, sizeof( int ) );
		RegSetValueEx( key1, "iBytesPerLine", 0, REG_DWORD, (CONST BYTE*) &iBytesPerLine, sizeof( int ) );
		RegSetValueEx( key1, "iOffsetLen", 0, REG_DWORD, (CONST BYTE*) &iMinOffsetLen, sizeof( int ) );//Pabs replaced "iOffsetLen" with "iMinOffsetLen"
		RegSetValueEx( key1, "iCharacterSet", 0, REG_DWORD, (CONST BYTE*) &iCharacterSet, sizeof( int ) );
		RegSetValueEx( key1, "iFontSize", 0, REG_DWORD, (CONST BYTE*) &iFontSize, sizeof( int ) );
		RegSetValueEx( key1, "bOpenReadOnly", 0, REG_DWORD, (CONST BYTE*) &bOpenReadOnly, sizeof( int ) );

//Pabs inserted
		RegSetValueEx( key1, "bMakeBackups", 0, REG_DWORD, (CONST BYTE*) &bMakeBackups, sizeof( int ) );
		RegSetValueEx( key1, "bAutoOffsetLen", 0, REG_DWORD, (CONST BYTE*) &bAutoOffsetLen, sizeof( int ) );
		RegSetValueEx( key1, "enable_drop", 0, REG_DWORD, (CONST BYTE*) &enable_drop, sizeof( int ) );
		RegSetValueEx( key1, "enable_drag", 0, REG_DWORD, (CONST BYTE*) &enable_drag, sizeof( int ) );
		RegSetValueEx( key1, "enable_scroll_delay_dd", 0, REG_DWORD, (CONST BYTE*) &enable_scroll_delay_dd, sizeof( int ) );
		RegSetValueEx( key1, "enable_scroll_delay_sel", 0, REG_DWORD, (CONST BYTE*) &enable_scroll_delay_sel, sizeof( int ) );
		RegSetValueEx( key1, "always_pick_move_copy", 0, REG_DWORD, (CONST BYTE*) &always_pick_move_copy, sizeof( int ) );
		RegSetValueEx( key1, "prefer_CF_HDROP", 0, REG_DWORD, (CONST BYTE*) &prefer_CF_HDROP, sizeof( int ) );
		RegSetValueEx( key1, "prefer_CF_BINARYDATA", 0, REG_DWORD, (CONST BYTE*) &prefer_CF_BINARYDATA, sizeof( int ) );
		RegSetValueEx( key1, "prefer_CF_TEXT", 0, REG_DWORD, (CONST BYTE*) &prefer_CF_TEXT, sizeof( int ) );
		RegSetValueEx( key1, "output_CF_BINARYDATA", 0, REG_DWORD, (CONST BYTE*) &output_CF_BINARYDATA, sizeof( int ) );
		RegSetValueEx( key1, "output_CF_TEXT", 0, REG_DWORD, (CONST BYTE*) &output_CF_TEXT, sizeof( int ) );
		RegSetValueEx( key1, "output_text_special", 0, REG_DWORD, (CONST BYTE*) &output_text_special, sizeof( int ) );
		RegSetValueEx( key1, "output_text_hexdump_display", 0, REG_DWORD, (CONST BYTE*) &output_text_hexdump_display, sizeof( int ) );
		RegSetValueEx( key1, "output_CF_RTF", 0, REG_DWORD, (CONST BYTE*) &output_CF_RTF, sizeof( int ) );
//end
		LCID lcid = MAKELCID(langArray.m_langid, 0);
		RegSetValueEx( key1, "locale", 0, REG_DWORD, (CONST BYTE*) &lcid, sizeof lcid);

		RegSetValueEx( key1, "TexteditorName", 0, REG_SZ, (CONST BYTE*) (char*) TexteditorName, TexteditorName.StrLen() + 1 );

		RegSetValueEx( key1, "iWindowShowCmd", 0, REG_DWORD, (CONST BYTE*) &iWindowShowCmd, sizeof( int ) );
		RegSetValueEx( key1, "iWindowX", 0, REG_DWORD, (CONST BYTE*) &iWindowX, sizeof( int ) );
		RegSetValueEx( key1, "iWindowY", 0, REG_DWORD, (CONST BYTE*) &iWindowY, sizeof( int ) );
		RegSetValueEx( key1, "iWindowWidth", 0, REG_DWORD, (CONST BYTE*) &iWindowWidth, sizeof( int ) );
		RegSetValueEx( key1, "iWindowHeight", 0, REG_DWORD, (CONST BYTE*) &iWindowHeight, sizeof( int ) );

		RegSetValueEx( key1, "iMRU_count", 0, REG_DWORD, (CONST BYTE*) &iMRU_count, sizeof( int ) );
		int i;
		char fname[ 64 ];
		for( i = 1; i <= MRUMAX; i++ )
		{
			sprintf( fname, "MRU_File%d", i );
			RegSetValueEx( key1, fname, 0, REG_SZ, (CONST BYTE*) &(strMRU[i-1][0]), strlen( &(strMRU[i-1][0]) ) + 1 );
		}

		// Close registry.
		RegCloseKey( key1 );
	}
	else
	{
		MessageBox(hwnd, "Could not save preferences to registry.", appname, MB_ICONERROR);
	}
}

//-------------------------------------------------------------------
void HexEditorWindow::CMD_edit_append()
{
	static_cast<dialog<AppendDlg>*>(this)->DoModal(hwnd);
}

//-------------------------------------------------------------------
void HexEditorWindow::CMD_manipulate_bits()
{
	if (DataArray.GetLength() == 0)
	{
		MessageBox(hwnd, "File is empty.", "Manipulate Bits", MB_ICONERROR);
		return;
	}
	if (iCurByte < 0 || iCurByte >= DataArray.GetLength())
	{
		MessageBox(hwnd, "Must choose byte in the file.", "Manipulate Bits", MB_ICONERROR);
		return;
	}
	static_cast<dialog<BitManipDlg>*>(this)->DoModal(hwnd);
}

//-------------------------------------------------------------------
void HexEditorWindow::CMD_character_set()
{
	static_cast<dialog<CharacterSetDlg>*>(this)->DoModal(hwnd);
}

//-------------------------------------------------------------------
void HexEditorWindow::CMD_toggle_insertmode()
{
	iInsertMode = !iInsertMode;
	set_wnd_title();
}

//-------------------------------------------------------------------
void HexEditorWindow::CMD_on_backspace()
{
	if (iCurByte <= 0)
		return;
	if (iInsertMode)
	{
		// INSERT-mode: If one exists delete previous byte.
		DataArray.RemoveAt(--iCurByte, 1);
		iFileChanged = TRUE;
		bFilestatusChanged = TRUE;
		set_wnd_title();
		resize_window();
	}
	else
	{
		// Only step back.
		int iByteLine = iCurByte-- / iBytesPerLine;
		if (snap_caret())
			repaint();
		else
			repaint(iByteLine, iCurByte / iBytesPerLine);
	}
}

//-------------------------------------------------------------------
void HexEditorWindow::CMD_select_all()
{
	if (DataArray.GetLength() <= 0)
		return;
	bSelected = TRUE;
	iStartOfSelection = 0;
	iEndOfSelection = DataArray.GetUpperBound();
	adjust_view_for_selection();
	repaint();
}

//-------------------------------------------------------------------
void HexEditorWindow::make_font()
{
	if (hFont)
		DeleteObject(hFont);
	HDC hdc = GetDC(hwnd);
	int nHeight = -MulDiv(iFontSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	ReleaseDC(hwnd, hdc);
	int cset;
	if (iCharacterSet == ANSI_FIXED_FONT)
		cset = ANSI_CHARSET;
	else
		cset = OEM_CHARSET;
	hFont = CreateFont(nHeight,0,0,0,0,0,0,0,cset,OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FIXED_PITCH | FF_DONTCARE,0);
}

//-------------------------------------------------------------------
void HexEditorWindow::CMD_properties()
{
	char buf[1000];
	char *pc;
	sprintf(buf, "File name and path: ");
	GetFullPathName(filename, 500, buf + strlen(buf), &pc);
	if (bPartialOpen)
	{
		sprintf(buf + strlen(buf), "\nPartially opened at offset 0x%x = %d.\n"
			"Number of bytes read: %d = %d kilobytes.\n",
			iPartialOffset, iPartialOffset, DataArray.GetLength(), DataArray.GetLength()/1024);
	}
	else
	{
		sprintf(buf + strlen(buf), "\nFile size: %d bytes = %d kilobytes.\n", DataArray.GetLength(), DataArray.GetLength()/1024);
	}
	sprintf(buf + strlen(buf), "\nNumber of hexdump lines: %d.\n", iNumlines);
	MessageCopyBox(hwnd, buf, "File properties", MB_ICONINFORMATION);
}

//-------------------------------------------------------------------
void HexEditorWindow::CMD_compare()
{
	static_cast<dialog<ChooseDiffDlg>*>(this)->DoModal(hwnd);
}

//-------------------------------------------------------------------
void HexEditorWindow::CMD_binarymode()
{
	static_cast<dialog<BinaryModeDlg>*>(this)->DoModal(hwnd);
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
		KillTimer(hwnd, SCROLL_DELAY_TIMER_ID);
		SetTimer(hwnd, SCROLL_TIMER_ID, ScrollInterval, NULL);
		bScrollTimerSet = TRUE;
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
			if (adjusth || adjustv)
				repaint();
		}
		break;
	}
}

//Pabs inserted
void HexEditorWindow::start_mouse_operation()
{
	KillTimer(hwnd, MOUSE_OP_DELAY_TIMER_ID);
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
					memcpy(p + sizeof(UINT), &DataArray[iStartOfSelSetting], iEndOfSelSetting - iStartOfSelSetting + 1);
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
				destlen = Text2BinTranslator::iBytes2BytecodeDestLen((char*) &DataArray[iStartOfSelSetting], iEndOfSelSetting-iStartOfSelSetting+1);
				sm.hGlobal = GlobalAlloc(GHND|GMEM_DDESHARE, destlen);
				if (sm.hGlobal)
				{
					if (char *p = (char *)GlobalLock(sm.hGlobal))
					{
						Text2BinTranslator::iTranslateBytesToBC(p, &DataArray[iStartOfSelSetting], iEndOfSelSetting-iStartOfSelSetting+1);
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
			DataArray.RemoveAt(iStartOfSelSetting, iEndOfSelSetting - iStartOfSelSetting + 1);
			bSelected = FALSE;
			iFileChanged = bFilestatusChanged = TRUE;
			iCurByte = iStartOfSelSetting;
			resize_window();
		}
		dragging = FALSE;
	}
	else
	{
		int lastbyte = DataArray.GetUpperBound();
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
		bSelected = bSelecting = TRUE;
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
	static_cast<dialog<SelectBlockDlg>*>(this)->DoModal(hwnd);
}

//-------------------------------------------------------------------
void HexEditorWindow::update_MRU()
{
	int i = 0;
	while (i < iMRU_count && strcmp(strMRU[i], filename) != 0)
		++i;
	if (i == iMRU_count) // Name not yet in list.
		if (iMRU_count < MRUMAX)
			++iMRU_count;
		else
			--i;
	while (int j = i)
		strcpy(strMRU[j], strMRU[--i]);
	strcpy(strMRU[0], filename); // Put chosen name at top.
	save_ini_data();
}

//-------------------------------------------------------------------
// Creates the MRU list and inserts it into the File menu.
void HexEditorWindow::make_MRU_list(HMENU menu)
{
//Pabs changed - 15 used to be 9
	do; while (RemoveMenu(menu, 15, MF_BYPOSITION));
	if (iMRU_count > 0)
	{
		AppendMenu(menu, MF_SEPARATOR, 0, 0);
		char buf[_MAX_PATH + 1 + 30];
		for (int i = 0 ; i < iMRU_count ; i++)
		{
			sprintf(buf, "&%d %s", i+1, strMRU[i]);
			AppendMenu(menu, MF_ENABLED | MF_STRING, IDM_MRU1 + i, buf);
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
		if (!close("Open"))
			return;
		load_file(strMRU[i]);
	}
	else
	{
		MessageBox(hwnd, "This file could not be accessed and\n"
			"will be removed from the MRU list.", "MRU list", MB_ICONERROR);
		--iMRU_count;
		for ( ; i < iMRU_count ; i++)
			strcpy(strMRU[i], strMRU[i + 1]);
		save_ini_data();
	}
}

//-------------------------------------------------------------------
void HexEditorWindow::CMD_add_bookmark()
{
	if (DataArray.GetLength() <= 0)
	{
		MessageBox(hwnd, "Can not set bookmark in empty file.", "Add bookmark", MB_ICONERROR);
		return;
	}
	if (iBmkCount >= BMKMAX)
	{
		MessageBox(hwnd, "Can not set any more bookmarks.", "Add bookmark", MB_ICONERROR);
		return;
	}
	static_cast<dialog<AddBmkDlg>*>(this)->DoModal(hwnd);
}

//-------------------------------------------------------------------
// Insert the bookmark list into the menu.
void HexEditorWindow::make_bookmark_list(HMENU menu)
{
	do; while (RemoveMenu(menu, 3, MF_BYPOSITION));
	if (iBmkCount > 0)
	{
		AppendMenu(menu, MF_SEPARATOR, 0, 0);
		char buf[128];
		for (int i = 0 ; i < iBmkCount ; i++)
		{
			sprintf(buf, pbmkList[i].name ? "&%d 0x%x:%s" : "&%d 0x%x", i + 1,
				pbmkList[i].offset, pbmkList[i].name);
			AppendMenu(menu, pbmkList[i].offset <= DataArray.GetLength() ?
				MF_ENABLED : MF_GRAYED, IDM_BOOKMARK1 + i, buf);
		}
	}
}

//-------------------------------------------------------------------
void HexEditorWindow::CMD_goto_bookmark(int i)
{
	if (pbmkList[i].offset >= 0 && pbmkList[i].offset <= DataArray.GetLength())
	{
		iCurByte = pbmkList[i].offset;
		iCurNibble = 0;
		bSelected = FALSE;
		resize_window();
		adjust_vscrollbar();
	}
	else
	{
		MessageBox(hwnd, "Bookmark points to invalid position.", "Go to bookmark", MB_ICONERROR);
	}
}

//-------------------------------------------------------------------
void HexEditorWindow::CMD_remove_bkm()
{
	if (iBmkCount == 0)
	{
		MessageBox(hwnd, "No bookmarks to remove.", "Remove bookmark", MB_ICONERROR);
		return;
	}
	static_cast<dialog<RemoveBmkDlg>*>(this)->DoModal(hwnd);
}

//-------------------------------------------------------------------
void HexEditorWindow::CMD_clear_all_bmk()
{
	int response = MessageBox(hwnd, "Really clear all bookmarks?",
		"Clear all bookmarks", MB_YESNO | MB_ICONQUESTION);
	if (response != IDYES)
		return;
	while (iBmkCount)
		if (char *name = pbmkList[--iBmkCount].name)
			free(name);
	repaint();
}

//-------------------------------------------------------------------
void HexEditorWindow::CMD_open_partially()
{
	if (!close("Open partially"))
		return;
	char szFileName[_MAX_PATH];
	szFileName[0] = '\0';
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof ofn);
	ofn.lStructSize = sizeof ofn;
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "All Files (*.*)\0*.*\0\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = _MAX_PATH;
	ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT;
	if (!GetOpenFileName(&ofn))
		return;
	OpenPartiallyDlg::filehandle = _open(szFileName, _O_RDONLY|_O_BINARY);
	if (OpenPartiallyDlg::filehandle == -1)
	{
		char buf[500];
		sprintf(buf, "Error code 0x%x occured while opening file %s.", errno, szFileName);
		MessageBox(hwnd, buf, "Open partially", MB_ICONERROR);
		return;
	}
	int response = static_cast<dialog<OpenPartiallyDlg>*>(this)->DoModal(hwnd);
	_close(OpenPartiallyDlg::filehandle);
	if (response != IDOK)
		return;
	// If read-only mode on opening is enabled or the file is read only:
	bReadOnly = bOpenReadOnly || -1 == _access(szFileName, 02); //Pabs added call to _access
	strcpy(filename, szFileName);
	bFileNeverSaved = FALSE;
	bFilestatusChanged = TRUE;
	bFileNeverSaved = FALSE;
	bPartialOpen = TRUE;
	resize_window();
}

//-------------------------------------------------------------------
void HexEditorWindow::CMD_fast_paste()
{
	static_cast<dialog<FastPasteDlg>*>(this)->DoModal(hwnd);
}

//-------------------------------------------------------------------
//Pabs inserted
INT_PTR CALLBACK MultiDropDlgProc(HWND h, UINT m, WPARAM w, LPARAM l)
{
	switch (m)
	{
	case WM_INITDIALOG:
		{
			SetWindowText(h, "Open");
			SetDlgItemText(h, 0xFFFF, "Choose a file to open:");
			SetDlgItemText(h, IDOK, "Open");
			HWND hwndList = GetDlgItem(h, IDC_LIST1);
			SetWindowLong(hwndList, GWL_STYLE, GetWindowLong(hwndList,GWL_STYLE) & ~LBS_SORT | WS_HSCROLL);

			char file[_MAX_PATH + 1];
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
			EndDialog(h, SendDlgItemMessage(h, IDC_LIST1, LB_GETCURSEL, 0, 0));
			return TRUE;
		case IDCANCEL:
			EndDialog(h, SendDlgItemMessage(h, IDC_LIST1, LB_GETCOUNT, 0, 0));
			return TRUE;
		}
		break;
	}
	return FALSE;
}
//end

// Handler for WM_DROPFILES
void HexEditorWindow::dropfiles(HDROP hDrop)
{
	char lpszFile[_MAX_PATH];
//Pabs inserted
	UINT numfiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0 );
//end
//Pabs inserted
	UINT i = 0;
	if (numfiles > 1)
	{
		i = ShowModalDialog(IDD_REMOVEBMK_DIALOG, hwnd, MultiDropDlgProc, hDrop);
		if (i >= numfiles)
		{
			DragFinish(hDrop);
			return;
		}
	}
	DragQueryFile(hDrop, i, lpszFile, _MAX_PATH);
//end
	DragFinish(hDrop); // handle to memory to free
	if (hwndMain)
		SetForegroundWindow(hwndMain);

	if (!close("Open"))
		return;

	char lpszTarget[_MAX_PATH];
	// Is this a link file?
	HRESULT hres = ResolveIt(lpszFile, lpszTarget);
	char *lpszFileToOpen = lpszFile;
	if (SUCCEEDED(hres))
	{
		// Trying to open a link file: decision by user required.
		int ret = MessageBox( hwnd,
			"You are trying to open a link file.\n"
			"Click on Yes if you want to open the file linked to,\n"
			"or click on No if you want to open the link file itself.\n"
			"Choose Cancel if you want to abort opening.",
			appname, MB_YESNOCANCEL | MB_ICONQUESTION );
		switch( ret )
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
	if (DataArray.GetLength() == 0)
	{
		MessageBox(hwnd, "File is empty.", "Template error", MB_ICONERROR);
		return;
	}
	// Get name of template file.
	char szTemplateName[_MAX_PATH];
	szTemplateName[0] = '\0';
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof ofn);
	ofn.lStructSize = sizeof ofn;
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "Template files (*.tpl)\0*.tpl\0\0";
	ofn.lpstrFile = szTemplateName;
	ofn.nMaxFile = _MAX_PATH;
	ofn.lpstrTitle = "Choose template file";
	ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT;
	if (GetOpenFileName(&ofn))
	{
		// szTemplateName contains name of chosen tpl file.
		apply_template(szTemplateName);
	}
}

//-------------------------------------------------------------------
void HexEditorWindow::apply_template(char *pcTemplate)
{
	// Load template file.
	int filehandle = _open(pcTemplate, _O_RDONLY|_O_BINARY);
	if (filehandle == -1)
	{
		char buf[500];
		sprintf(buf, "Error code 0x%x occured while\nopening template file %s.", errno, pcTemplate);
		MessageBox(hwnd, buf, "Template error", MB_ICONERROR);
		return;
	}
	int tpl_filelen = _filelength(filehandle);
	if (tpl_filelen > 0)
	{
		char *pcTpl = new char[tpl_filelen];
		if (pcTpl && _read(filehandle, pcTpl, tpl_filelen) != -1)
		{
			// Template file is loaded into pcTpl.
			SimpleArray<char> TplResult;
			TplResult.SetSize(1, 100);
			// Print filename and current offset to output.
			TplResult.AppendArray("File: ", 6);
			TplResult.AppendArray(filename, strlen(filename));
			TplResult.AppendArray("\xd\xa", 2);
			TplResult.AppendArray("Template file: ", 15);
			TplResult.AppendArray(pcTemplate, strlen(pcTemplate));
			TplResult.AppendArray("\xd\xa", 2);
			TplResult.AppendArray("Applied at offset: ", 19);
			char buf[16];
			sprintf(buf, "%d\xd\xa\xd\xa", iCurByte);
			TplResult.AppendArray(buf, strlen(buf));
			// Apply template on file in memory.
			apply_template_on_memory(pcTpl, tpl_filelen, TplResult);
			TplResult.Append('\0');
			// Display template data.
			char *pcTmplText = TplResult;
			ShowModalDialog(IDD_TMPL_RESULT_DIALOG, hwnd, TmplDisplayDlgProc, pcTmplText);
		}
		else//Pabs replaced NULL w hwnd
			MessageBox(hwnd, "Template file could not be loaded.", "Template error", MB_ICONERROR);
		// Delete template data.
		delete [] pcTpl;
	}
	else
		MessageBox(hwnd, "Template file is empty.", "Template error", MB_ICONERROR);
	_close(filehandle);
}

//-------------------------------------------------------------------
// Applies the template code in pcTpl of length tpl_len on the current file
// from the current offset and outputs the result to the ResultArray.
void HexEditorWindow::apply_template_on_memory( char* pcTpl, int tpl_len, SimpleArray<char>& ResultArray )
{
	// Use source code in pcTpl to decipher data in file.
	int index = 0, fpos = iCurByte;
	// While there is still code left...
	while( index < tpl_len )
	{
		// Read in the var type.
		if( ignore_non_code( pcTpl, tpl_len, index ) == TRUE )
		{
			// index now points to first code character.
			// Get var type.
			char cmd[ TPL_TYPE_MAXLEN ]; // This holds the variable type, like byte or word.
			if( read_tpl_token( pcTpl, tpl_len, index, cmd ) == TRUE )
			{
				// cmd holds 0-terminated var type, index set to position of first space-
				// character after the type. Now test if valid type was given.
				//---- type BYTE ---------------------------------
				if( strcmp( cmd, "BYTE" ) == 0 || strcmp( cmd, "char" ) == 0 )
				{
					// This is a byte/char.
					if( ignore_non_code( pcTpl, tpl_len, index ) == TRUE )
					{
						// Enough space for a byte?
						if( DataArray.GetLength() - fpos >= 1 )
						{
							// Read var name.
							char name[ TPL_NAME_MAXLEN ];
							// index is set to a non-space character by last call to ignore_non_code.
							// Therefore the variable name can be read into buffer name.
							read_tpl_token( pcTpl, tpl_len, index, name );
							// Write variable type and name to output.
							ResultArray.AppendArray( cmd, strlen(cmd) );
							ResultArray.Append( ' ' );
							ResultArray.AppendArray( name, strlen(name) );
							// Write value to output.
							char buf[ TPL_NAME_MAXLEN + 200];
							if( DataArray[fpos] != 0 )
								sprintf( buf, " = %d (signed) = %u (unsigned) = 0x%x = \'%c\'\xd\xa", (int) (signed char) DataArray[fpos], DataArray[fpos], DataArray[fpos], DataArray[fpos] );
							else
								sprintf( buf, " = %d (signed) = %u (unsigned) = 0x%x\xd\xa", (int) (signed char) DataArray[fpos], DataArray[fpos], DataArray[fpos] );
							ResultArray.AppendArray( buf, strlen(buf) );
							// Increase pointer for next variable.
							fpos += 1;
						}
						else
						{
							ResultArray.AppendArray( "ERROR: not enough space for byte-size datum.", 45 );
							return;
						}
					}
					else
					{
						// No non-spaces after variable type up to end of array, so
						// no space for variable name.
						ResultArray.AppendArray( "ERROR: missing variable name.", 29 );
						return;
					}
				}
				else if( strcmp( cmd, "WORD" ) == 0 || strcmp( cmd, "short" ) == 0 )
				{
					// This is a word.
					if( ignore_non_code( pcTpl, tpl_len, index ) == TRUE )
					{
						// Enough space for a word?
						if( DataArray.GetLength() - fpos >= 2 )
						{
							// Read var name.
							char name[ TPL_NAME_MAXLEN ];
							read_tpl_token( pcTpl, tpl_len, index, name );
							// Write variable type to output.
							ResultArray.AppendArray( cmd, strlen(cmd) );
							ResultArray.Append( ' ' );
							// Write variable name to output.
							ResultArray.AppendArray( name, strlen(name) );
							WORD wd;
							// Get value depending on binary mode.
							if( iBinaryMode == LITTLEENDIAN_MODE )
							{
								wd = *( (WORD*)( &DataArray[ fpos ] ) );
							}
							else // BIGENDIAN_MODE
							{
								int i;
								for( i=0; i<2; i++ )
									((char*)&wd)[ i ] = DataArray[ fpos + 1 - i ];
							}
							char buf[ TPL_NAME_MAXLEN + 200 ];
							sprintf( buf, " = %d (signed) = %u (unsigned) = 0x%x\xd\xa", (int) (signed short) wd, wd, wd );
							ResultArray.AppendArray( buf, strlen(buf) );
							fpos += 2;
						}
						else
						{
							ResultArray.AppendArray( "ERROR: not enough space for WORD.", 34 );
							return;
						}
					}
					else
					{
						ResultArray.AppendArray( "ERROR: missing variable name.", 29 );
						return; // No more code: missing name.
					}
				}
				else if( strcmp( cmd, "DWORD" ) == 0 || strcmp( cmd, "int" ) == 0 ||
					strcmp( cmd, "long" ) == 0 || strcmp( cmd, "LONG" ) == 0 )
				{
					// This is a longword.
					if( ignore_non_code( pcTpl, tpl_len, index ) == TRUE )
					{
						// Enough space for a longword?
						if( DataArray.GetLength() - fpos >= 4 )
						{
							// Read var name.
							char name[ TPL_NAME_MAXLEN ];
							read_tpl_token( pcTpl, tpl_len, index, name );
							// Write variable type to output.
							ResultArray.AppendArray( cmd, strlen(cmd) );
							ResultArray.Append( ' ' );
							// Write variable name to output.
							ResultArray.AppendArray( name, strlen(name) );
							DWORD dw;
							// Get value depending on binary mode.
							if( iBinaryMode == LITTLEENDIAN_MODE )
							{
								dw = *( (DWORD*)( &DataArray[ fpos ] ) );
							}
							else // BIGENDIAN_MODE
							{
								int i;
								for( i=0; i<4; i++ )
									((char*)&dw)[ i ] = DataArray[ fpos + 3 - i ];
							}
							char buf[ TPL_NAME_MAXLEN + 200 ];
							sprintf( buf, " = %d (signed) = %u (unsigned) = 0x%x\xd\xa", (signed long) dw, (unsigned long) dw, dw );
							ResultArray.AppendArray( buf, strlen(buf) );
							fpos += 4;
						}
						else
						{
							ResultArray.AppendArray( "ERROR: not enough space for DWORD.", 34 );
							return;
						}
					}
					else
					{
						ResultArray.AppendArray( "ERROR: missing variable name.", 29 );
						return; // No more code: missing name.
					}
				}
				else if( strcmp( cmd, "float" ) == 0 )
				{
					// This is a float.
					if( ignore_non_code( pcTpl, tpl_len, index ) == TRUE )
					{
						// Enough space for a float?
						if( DataArray.GetLength() - fpos >= 4 )
						{
							// Read var name.
							char name[ TPL_NAME_MAXLEN ];
							read_tpl_token( pcTpl, tpl_len, index, name );
							// Write variable type to output.
							ResultArray.AppendArray( cmd, strlen(cmd) );
							ResultArray.Append( ' ' );
							// Write variable name to output.
							ResultArray.AppendArray( name, strlen(name) );
							float f;
							// Get value depending on binary mode.
							if( iBinaryMode == LITTLEENDIAN_MODE )
							{
								f = *( (float*)( &DataArray[ fpos ] ) );
							}
							else // BIGENDIAN_MODE
							{
								int i;
								for( i=0; i<4; i++ )
									((char*)&f)[ i ] = DataArray[ fpos + 3 - i ];
							}
							char buf[ TPL_NAME_MAXLEN + 200 ];
							sprintf( buf, " = %f = 0x%x\xd\xa", f, (unsigned long) *((int*) &f) );
							ResultArray.AppendArray( buf, strlen(buf) );
							fpos += 4;
						}
						else
						{
							ResultArray.AppendArray( "ERROR: not enough space for float.", 34 );
							return;
						}
					}
					else
					{
						ResultArray.AppendArray( "ERROR: missing variable name.", 29 );
						return; // No more code: missing name.
					}
				}
				else if( strcmp( cmd, "double" ) == 0 )
				{
					// This is a double.
					if( ignore_non_code( pcTpl, tpl_len, index ) == TRUE )
					{
						// Enough space for a double?
						if( DataArray.GetLength() - fpos >= 8 )
						{
							// Read var name.
							char name[ TPL_NAME_MAXLEN ];
							read_tpl_token( pcTpl, tpl_len, index, name );
							// Write variable type to output.
							ResultArray.AppendArray( cmd, strlen(cmd) );
							ResultArray.Append( ' ' );
							// Write variable name to output.
							ResultArray.AppendArray( name, strlen(name) );
							double d;
							// Get value depending on binary mode.
							if( iBinaryMode == LITTLEENDIAN_MODE )
							{
								d = *( (double*)( &DataArray[ fpos ] ) );
							}
							else // BIGENDIAN_MODE
							{
								int i;
								for( i=0; i<8; i++ )
									((char*)&d)[ i ] = DataArray[ fpos + 7 - i ];
							}
							char buf[ TPL_NAME_MAXLEN + 200 ];
							sprintf( buf, " = %g\xd\xa", d );
							ResultArray.AppendArray( buf, strlen(buf) );
							fpos += 8;
						}
						else
						{
							ResultArray.AppendArray( "ERROR: not enough space for double.", 35 );
							return;
						}
					}
					else
					{
						ResultArray.AppendArray( "ERROR: missing variable name.", 29 );
						return; // No more code: missing name.
					}
				}
				else
				{
					ResultArray.AppendArray( "ERROR: Unknown variable type \"", 30 );
					ResultArray.AppendArray( cmd, strlen( cmd ) );
					ResultArray.Append( '\"' );
					return;
				}
			}
			else
			{
				// After the type there is only the array end. Therefore
				// no space for a variable name.
				ResultArray.AppendArray( "ERROR: Missing variable name.", 29 );
				return;
			}
		}
		else
		{
			// No non-spaces up to the end of the array.
			break;
		}
	}
	// No more code left in pcTpl.
	char buf[128];
	sprintf(buf, "\xd\xa-> Length of template = %d bytes.\xd\xa", fpos - iCurByte);
	ResultArray.AppendArray(buf, strlen(buf));
}

//-------------------------------------------------------------------
// This will set index to the position of the next non-space-character.
// Return is FALSE if there are no non-spaces left up to the end of the array.
int HexEditorWindow::ignore_non_code( char* pcTpl, int tpl_len, int& index )
{
	while( index < tpl_len )
	{
		// If code found, return.
		switch( pcTpl[ index ] )
		{
		case ' ': case '\t': case 0x0d: case 0x0a:
			break;

		default:
			return TRUE;
		}
		index++;
	}
	return FALSE;
}

//-------------------------------------------------------------------
// Writes all non-space characters from index to dest and closes dest
// with a zero-byte. index is set to position of the first space-
// character. Return is false úƒ there is only the array end after the
// keyword. In that case index is set to tpl_len.
int HexEditorWindow::read_tpl_token( char* pcTpl, int tpl_len, int& index, char* dest )
{
	int i = 0;
	while( index + i < tpl_len )
	{
		switch( pcTpl[ index + i ] )
		{
		case ' ': case '\t': case 0x0d: case 0x0a:
			dest[i] = '\0';
			index += i;
			return TRUE;

		default:
			dest[i] = pcTpl[ index + i ];
		}
		i++;
	}
	dest[i] = '\0';
	index += i;
	return FALSE;
}

//-------------------------------------------------------------------
// TmplDisplayDlgProc
INT_PTR CALLBACK TmplDisplayDlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		SetDlgItemText(hDlg, IDC_EDIT1, (LPCTSTR)lParam);
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
	static_cast<dialog<GoToDlg>*>(this)->DoModal(hwnd);
}

//-------------------------------------------------------------------
// Resolve link files for opening from command line.
// Copied from compiler documentation.

HRESULT HexEditorWindow::ResolveIt(LPCSTR lpszLinkFile, LPSTR lpszPath)
{
	return ::ResolveIt(hwndMain, lpszLinkFile, lpszPath);
}

void NTAPI TranslateDialog(HWND hwnd)
{
	if (IsWindowUnicode(hwnd))
		langArray.TranslateDialogW(hwnd);
	else
		langArray.TranslateDialogA(hwnd);
}

INT_PTR NTAPI ShowModalDialog(UINT idd, HWND hwnd, DLGPROC dlgproc, LPVOID param)
{
	HINSTANCE hinst = langArray.m_hLangDll ? langArray.m_hLangDll : hMainInstance;
	return IsWindowUnicode(hwnd)
	? DialogBoxParamW(hinst, MAKEINTRESOURCEW(idd), hwnd, dlgproc, (LPARAM)param)
	: DialogBoxParamA(hinst, MAKEINTRESOURCEA(idd), hwnd, dlgproc, (LPARAM)param);
}

//-------------------------------------------------------------------
void HexEditorWindow::CMD_colors_to_default()
{
	if (MessageBox(hwnd, "Really reset colors to default values?",
			appname, MB_YESNO | MB_ICONQUESTION) == IDYES)
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
	if( GetDllExportNames( filename, &ulOffset, &ulSize ) )
	{
		bSelected = TRUE;
		iStartOfSelection = (int)ulOffset;
		iEndOfSelection = (int)(ulOffset + ulSize - 1);
		adjust_view_for_selection();
		repaint();
	}
}

void HexEditorWindow::CMD_GotoDllImports()
{
	ULONG ulOffset, ulSize;
	if( GetDllImportNames( filename, &ulOffset, &ulSize ) )
	{
		bSelected = TRUE;
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

	if (filename[0] == '\0' || hwnd != WindowFromPoint(p))
		return;

	ScreenToClient(hwnd, &p);
	int log_column = p.x / cxChar + iHscrollPos;
	ClientToScreen(hwnd, &p);

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
	else if (HMENU hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_CONTEXTMENU)))
	{
		// You could use other menu indices based on context... if you like
		if (HMENU h = GetSubMenu(hMenu, Drive ? 1 : 0))
		{
			int i = GetMenuItemCount(h);
			while (i)
			{
				if (UINT id = GetMenuItemID(h, --i))
				{
					EnableMenuItem(h, i, queryCommandEnabled(id) ?
						MF_ENABLED | MF_BYPOSITION : MF_GRAYED | MF_BYPOSITION);
				}
			}
			TrackPopupMenu(h, TPM_LEFTALIGN | TPM_RIGHTBUTTON, p.x, p.y, 0, hwnd, NULL);
		}
		DestroyMenu(hMenu);
	}
}

void HexEditorWindow::CMD_CloseDrive()
{
	CMD_new("Close Drive");
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
	static_cast<dialog<GotoTrackDialog>*>(this)->DoModal(hwnd);
}

void HexEditorWindow::CMD_DriveGotoLastTrack()
{
	CurrentSectorNumber = SelectedPartitionInfo->m_NumberOfSectors - 1;
	RefreshCurrentTrack();
}

void HexEditorWindow::RefreshCurrentTrack()
{
	if (Drive->ReadAbsolute(Track.GetObjectMemory(), Track.GetObjectSize(), CurrentSectorNumber + SelectedPartitionInfo->m_StartingSector))
	{
		ULONG BytesPerSector = Track.GetObjectSize();
		DataArray.ClearAll();
		if (DataArray.SetSize(BytesPerSector))
		{
			DataArray.SetUpperBound(BytesPerSector-1);
			CopyMemory(DataArray, Track.GetObjectMemory(), BytesPerSector);
			bReadOnly = TRUE;
			sprintf(filename, "%s:Sector %I64d", (LPCSTR) SelectedPartitionInfo->GetNameAsString(), CurrentSectorNumber);
			bFileNeverSaved = FALSE;
			iVscrollMax = 0;
			iVscrollPos = 0;
			iHscrollMax = 0;
			iHscrollPos = 0;
			iCurByte = 0;
			iCurNibble = 0;
			iFileChanged = FALSE;
			bFilestatusChanged = TRUE;
			bFileNeverSaved = FALSE;
			bPartialOpen = TRUE;
			resize_window();
		}
	}
}

void HexEditorWindow::CMD_OpenDrive()
{
	if (!close("Open Drive"))
		return;
	static_cast<dialog<OpenDriveDialog>*>(this)->DoModal(hwnd);
}

void HexEditorWindow::CMD_EncodeDecode()
{
	static_cast<dialog<EncodeDecodeDialog>*>(this)->DoModal(hwnd);
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

		// iFindDlgLastLen = iFindDlgBufLen; // Remember old buffer length.
		// iFindDlgBufLen = Text2BinTranslator::iBytes2BytecodeDestLen( (char*) &DataArray[sel_start], select_len ); // Get the length of the bytecode representation of the selection.
		// if( pcFindDlgBuffer != NULL ) // Delete old buffer, if necessary.
		//	delete [] pcFindDlgBuffer;
		// pcFindDlgBuffer = new char[iFindDlgBufLen]; // Allocate new buffer.
		Text2BinTranslator::iTranslateBytesToBC(FindDlg::pcFindDlgBuffer, &DataArray[sel_start], select_len ); // Translate the selection into bytecode and write it into the edit box buffer.
	}

	// Is there a findstring? (Initmenupopup actually filters this already).
	if (FindDlg::pcFindDlgBuffer)
	{
		// There is a findstring. Create its translation.
		char *pcFindstring;
		int srclen = strlen(FindDlg::pcFindDlgBuffer);
		if (int destlen = create_bc_translation(&pcFindstring, FindDlg::pcFindDlgBuffer, srclen))
		{
			SetCursor(LoadCursor(NULL, IDC_WAIT));
			int i = find_bytes((char *)&DataArray[iCurByte + 1],
				DataArray.GetLength() - iCurByte - 1,
				pcFindstring, destlen, 1,
				FindDlg::iFindDlgMatchCase ? equal : lower_case);
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			if (i != -1)
			{
				iCurByte += i + 1;
				bSelected = TRUE;
				iStartOfSelection = iCurByte;
				iEndOfSelection = iCurByte + destlen - 1;
				adjust_view_for_selection();
				repaint();
			}
			else
			{
				MessageBox(hwnd, "Could not find any more occurances.", "Find next", MB_ICONERROR);
			}
			delete [] pcFindstring;
		}
		else
		{
			FindDlg::iFindDlgDirection = 1;
			CMD_find();
		}
	}
	else
	{
		//Can't call CMD_find cause it won't alloc a new buffer
		// There is no findstring.
		MessageBox(hwnd, "String to find not specified.", "Find next", MB_ICONERROR);
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

		// Remember old buffer length.
		// iFindDlgLastLen = iFindDlgBufLen;

		// Get the length of the bytecode representation of the selection.
		// iFindDlgBufLen = Text2BinTranslator::iBytes2BytecodeDestLen( (char*) &DataArray[sel_start], select_len );

		// Delete old buffer, if necessary.
		// if( pcFindDlgBuffer != NULL )
		//	delete [] pcFindDlgBuffer;

		// Allocate new buffer.
		// pcFindDlgBuffer = new char[iFindDlgBufLen];

		// Translate the selection into bytecode and write it into the edit box buffer.
		Text2BinTranslator::iTranslateBytesToBC(FindDlg::pcFindDlgBuffer, &DataArray[sel_start], select_len);
	}
	// Is there a findstring? (Initmenupopup actually filters this already).
	if (FindDlg::pcFindDlgBuffer)
	{
		// There is a findstring. Create its translation.
		char *pcFindstring;
		int srclen = strlen(FindDlg::pcFindDlgBuffer);
		if (int destlen = create_bc_translation(&pcFindstring, FindDlg::pcFindDlgBuffer, srclen))
		{
			SetCursor(LoadCursor(NULL, IDC_WAIT));
			// Search the array starting at index 0 to the current byte,
			// plus the findstring-length minus 1. If
			// you are somewhere in the middle of the findstring with the caret
			// and you choose "find previous" you usually want to find the beginning
			// of the findstring in the file.
			int i = find_bytes((char *)&DataArray[0],
				min(iCurByte + (destlen - 1), DataArray.GetLength()),
				pcFindstring, destlen, -1, FindDlg::iFindDlgMatchCase ? equal : lower_case);
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			if (i != -1)
			{
				iCurByte = i;
				bSelected = TRUE;
				iStartOfSelection = iCurByte;
				iEndOfSelection = iCurByte + destlen - 1;
				adjust_view_for_selection();
				repaint();
			}
			else
			{
				MessageBox(hwnd, "Could not find any more occurances.", "Find previous", MB_ICONERROR);
			}
			delete [] pcFindstring;
		}
		else
//Pabs replaced message with CMD_find
		{
			FindDlg::iFindDlgDirection = -1;
			CMD_find();
		}
//end
	}
	else
	{
		// There is no findstring.
		MessageBox(hwnd, "String to find not specified.", "Find previous", MB_ICONERROR);
	}
}

//-------------------------------------------------------------------
// Handler for the "Open in text editor" command.
void HexEditorWindow::CMD_summon_text_edit()
{
	if (!close("Open in text editor"))
		return;
	HINSTANCE hi = ShellExecute( hwnd, "open", TexteditorName, filename, NULL, SW_SHOWNORMAL );
	if ((int) hi <= HINSTANCE_ERROR)
		MessageBox(hwnd, "An error occured when calling the text editor.", "Open in text editor", MB_ICONERROR);
}

//-------------------------------------------------------------------
// Process and route all window messages.
int HexEditorWindow::OnWndMsg( HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam )
{
	switch (iMsg)
	{
	case WM_CREATE:
		at_window_create(hwnd, hMainInstance);
		break;

	case WM_SIZE:
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
		lbuttondown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
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

	case WM_KEYDOWN:
		keydown(wParam);
		break;

	case WM_CHAR:
		character((char)wParam);
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

	case WM_TIMER:
		timer(wParam, lParam);
		break;

	case WM_DESTROY:
		destroy_window();
		break;
	}
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

//Pabs changed - removed CMD_explorersettings

//-------------------------------------------------------------------
void HexEditorWindow::CMD_replace()
{
	static_cast<dialog<ReplaceDlg>*>(this)->DoModal(hwnd);
}

BOOL HexEditorWindow::select_next_diff(BOOL bFromStart)
{
	BOOL bDone = FALSE;
	int length = get_length();
	unsigned char *buffer = get_buffer(length);
	int sibling_length = sibling->get_length();
	unsigned char *sibling_buffer = sibling->get_buffer(sibling_length);
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
		bSelected = TRUE;
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
	unsigned char *buffer = get_buffer(length);
	int sibling_length = sibling->get_length();
	unsigned char *sibling_buffer = sibling->get_buffer(sibling_length);
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
		bSelected = TRUE;
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
		int sibling_length = sibling->get_length();
		sibling_status->iEnteringMode = iEnteringMode;
		sibling_status->iCurByte = iCurByte < sibling_length ? iCurByte : sibling_length;
		sibling_status->iCurNibble = iCurNibble;
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
		}
		sibling_status->iVscrollMax = iVscrollMax;
		sibling_status->iVscrollPos = iVscrollPos;
		sibling->adjust_vscrollbar();
		sibling->repaint(-1, -1);
	}
}

void HexEditorWindow::CMD_revert()
{
	if (bFileNeverSaved)
	{
		iFileChanged = FALSE;
		CMD_new("Revert");
		return;
	}
	if (bPartialOpen)
	{
		int filehandle = _open(filename, _O_RDONLY|_O_BINARY);
		if (filehandle != -1)
		{
			_lseek(filehandle, iPartialOffset, 0);
			if (DataArray.SetSize(iPartialOpenLen))
			{
				if (_read(filehandle, &DataArray[0], iPartialOpenLen) != -1)
				{
					bReadOnly = bOpenReadOnly || -1 == _access(filename, 02);
					iVscrollMax = iVscrollPos = iHscrollMax = iHscrollPos =
					iVscrollPos = iCurByte = iCurNibble = 0;
					iFileChanged = FALSE;
					bFilestatusChanged = TRUE;
					bFileNeverSaved = FALSE;
					bSelected = FALSE;
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
	static_cast<dialog<FillWithDialog>*>(this)->DoModal(hwnd);
}

void HexEditorWindow::CMD_deletefile()
{
	if (IDYES != MessageBox(hwnd, "Are you sure you want to delete this file?", appname, MB_ICONERROR|MB_YESNO))
		return;
	if (IDYES != MessageBox(hwnd,"Are you really really sure you want to delete this file?", appname, MB_ICONERROR|MB_YESNO))
		return;
	if (remove(filename) != 0)
	{
		MessageBox(hwnd, "Could not delete file.", "Delete file", MB_ICONERROR);
		return;
	}
	//Remove from MRU
	int i = 0;
	while (i < iMRU_count && strcmp(strMRU[i], filename) != 0)
		++i;
	--iMRU_count;
	for ( ; i < iMRU_count ; i++)
		strcpy(strMRU[i], strMRU[i + 1]);
	save_ini_data();
	iFileChanged = FALSE;
	CMD_new("Delete file");//tricky-tricky
}

void HexEditorWindow::CMD_insertfile()
{
	char szFileName[_MAX_PATH];
	szFileName[0] = '\0';
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof ofn);
	ofn.lStructSize = sizeof ofn;
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "All Files (*.*)\0*.*\0\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = _MAX_PATH;
	ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT;
	if (!GetOpenFileName(&ofn))
		return;
	// RK: don't allow inserting same file we're editing right now.
	//Pabs removed - the bug appears to have disappeared as a result of my improvements
	int fhandle = _open(szFileName, _O_RDONLY|_O_BINARY);
	if (fhandle == -1)
	{
		MessageBox(hwnd, "Error opening file", "Insert file", MB_ICONERROR);
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
		int ol = DataArray.GetSize();
		int nl = ol + inslen - rl;
		bool rssuc = inslen <= rl || get_buffer(nl); // resize succesful
		if (rssuc)
		{
			BYTE *src = &DataArray[rs + rl];
			BYTE *dst = &DataArray[rs + inslen];
			int count = ol - (rs + rl);
			if (inslen > rl) // bigger
				memmove(dst, src, count);
			bool rdsuc = _read(fhandle, &DataArray[rs], inslen) != -1; //read successful

			//In the following two if blocks DataArray.SetUpperBound(somelen-1);
			//is used instead of DataArray.SetSize(somelen);
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
					bFilestatusChanged = TRUE;
					bSelected = inslen != 0;
					resize_window();
				}
			}
			else
			{
				MessageBox(hwnd, "Could not insert data", "Insert file", MB_ICONERROR);
			}
		}
	}
	else
	{
		MessageBox(hwnd, "Error checking file size", "Insert file", MB_ICONERROR);
	}
	_close(fhandle);
}

void HexEditorWindow::CMD_saveselas()
{
	char szFileName[_MAX_PATH];
	szFileName[0] = '\0';
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof ofn);
	ofn.lStructSize = sizeof ofn;
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "All Files (*.*)\0*.*\0\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = _MAX_PATH;
	ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT;
	if (GetSaveFileName(&ofn))
	{
		const char *complain = "Could not save file.";
		int filehandle = _open(szFileName, _O_RDWR|_O_CREAT|_O_TRUNC|_O_BINARY, _S_IREAD|_S_IWRITE);
		if (filehandle != -1)
		{
			WaitCursor wc;
			int lower = 0;
			int upper = DataArray.GetUpperBound();
			if (bSelected)
			{
				lower = iGetStartOfSelection();
				upper = iGetEndOfSelection();
			}
			if (_write(filehandle, &DataArray[lower], upper - lower + 1) != -1)
				complain = 0;
			_close(filehandle);
		}
		if (complain)
			MessageBox(hwnd, complain, "Save as", MB_ICONERROR);
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
		MessageBox(hwnd, "No data present\nCannot continue!", "Import Hexdump", MB_ICONERROR);
		return false;
	}
	hexin.lhseek(0);

	char msg[150] =
		"Does this data have the same format as the frhed display?"
		"\nThis data contains ";
	strcat(msg, typ ?
		"characters other than whitespace and hexdigits. (like frhed display)" :
		"only whitespace and hexdigits. (unlike frhed display)");

	switch (MessageBox(hwnd, msg, "Import Hexdump", MB_YESNOCANCEL))
	{
	case IDYES:
		return load_hexfile_1::StreamIn(*this, hexin); //Display output
	case IDNO:
		return load_hexfile_0::StreamIn(*this, hexin); //just hex & space
	}
	return FALSE;
}

void HexEditorWindow::CMD_open_hexdump()
{
	if (!close("Import Hexdump"))
		return;

	HGLOBAL hClipMemory = 0;

	//Check if clipboard may be used
	if (OpenClipboard(NULL))
	{
		hClipMemory = GetClipboardData(CF_TEXT);
		//Check if user wants to use clipboard
		if (hClipMemory)
		{
			switch (MessageBox(hwnd,
				"There is text on the clipboard.\n"
				"Do you want to import from\n"
				"the clipboard instead of a file?",
				"Import Hexdump",
				MB_YESNOCANCEL | MB_ICONQUESTION))
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
			MessageBox(hwnd, "Could not get text from the clipboard.\nCannot continue!", "Import Hexdump", MB_ICONERROR);
		}
		CloseClipboard();
	}
	else
	{
		//Import from file
		//Initialize the struct
		char szFileName[_MAX_PATH];
		szFileName[0] = '\0';
		OPENFILENAME ofn;
		ZeroMemory(&ofn, sizeof ofn);
		ofn.lStructSize = sizeof ofn;
		ofn.hwndOwner = hwnd;
		ofn.lpstrFilter = "Hex Dump files(*.txt,*.hex)\0*.txt;*.hex\0All Files (*.*)\0*.*\0";
		ofn.lpstrFile = szFileName;
		ofn.nMaxFile = _MAX_PATH;
		ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT;
		if (!GetOpenFileName(&ofn))
			return;
		//Set up variables for the function
		FILE *f = fopen(szFileName, "rb");
		if (f == 0)
		{
			MessageBox(hwnd,"Could not get text from the file.\nCannot continue!", "Import Hexdump", MB_ICONERROR);
			return;
		}
		done = load_hexfile(fhexfile_stream(f));
		fclose(f);
		if (done)
			bReadOnly = -1 == _access(szFileName, 02);
	}
	if (done)
	{
		//Successful
		strcpy(filename, "Untitled");
		iVscrollPos = iCurByte = iCurNibble =
			bPartialOpen = bPartialStats = iFileChanged =
			bSelected =
			iVscrollMax = iVscrollPos = iHscrollMax = iHscrollPos =
			iCurByte = iCurNibble = FALSE;
		bFileNeverSaved = bFilestatusChanged = TRUE;
		// If read-only mode on opening is enabled:
		if (bOpenReadOnly)
			bReadOnly = TRUE;
		save_ini_data();
		resize_window();
	}
}

BOOL contextpresent()
{
	HKEY key1;
	LONG res = RegOpenKeyEx( HKEY_CLASSES_ROOT, "*\\shell\\Open in frhed\\command", 0, KEY_ALL_ACCESS, &key1 );
	if (res == ERROR_SUCCESS) //succeeded check if has the required keys & data
	{
		char stringval[ _MAX_PATH ];
		char exepath[ _MAX_PATH ];
		long len = 0;//dummy
		strcpy( exepath, _pgmptr );
		strcat(exepath," %1");
		RegQueryValue(key1,NULL,stringval,&len);
		RegCloseKey(key1);
		if(strcmp(stringval, exepath))
			return 1;
	}
	return 0;
}

BOOL defaultpresent()
{
	char stringval[_MAX_PATH];
	long len = _MAX_PATH;
	LONG res = RegQueryValue(HKEY_CLASSES_ROOT, "Unknown\\shell", stringval, &len);
	return res == ERROR_SUCCESS && strcmp(stringval, "Open in frhed") == 0;
}

BOOL unknownpresent()
{
	HKEY key1;
	LONG res;
	res = RegOpenKeyEx( HKEY_CLASSES_ROOT, "Unknown\\shell\\Open in frhed\\command", 0, KEY_ALL_ACCESS, &key1 );
	if( res == ERROR_SUCCESS ){//succeeded check if has the required keys & data
		char stringval[ _MAX_PATH ];
		char exepath[ _MAX_PATH ];
		long len = 0;//dummy
		strcpy( exepath, _pgmptr );
		strcat(exepath," %1");
		RegQueryValue(key1,NULL,stringval,&len);
		RegCloseKey(key1);
		if(strcmp(stringval, exepath))
			return 1;
	}
	return 0;
}

BOOL oldpresent()
{
	HKEY hk;
	char keyname[] = "Software\\frhed";
	char subkeynam[MAX_PATH + 1] = "";
	LONG res = RegOpenKeyEx( HKEY_CURRENT_USER,keyname,0,KEY_ALL_ACCESS,&hk );
	if( res == ERROR_SUCCESS ){
		for(DWORD i = 0;; i++ ){
			res = RegEnumKey(hk,i,subkeynam,MAX_PATH + 1);
			if(res==ERROR_NO_MORE_ITEMS)break;
			else if(0!=strcmp(subkeynam,"v"CURRENT_VERSION"."SUB_RELEASE_NO)){
				RegCloseKey(hk);
				return TRUE;
			}
		}
		RegCloseKey(hk);
	}
	return FALSE;
}

BOOL frhedpresent()
{
	//Check if frhed\subreleaseno exists
	HKEY hk;
	if(ERROR_SUCCESS==RegOpenKey(HKEY_CURRENT_USER, "Software\\frhed\\v"CURRENT_VERSION"." SUB_RELEASE_NO ,&hk)){
		RegCloseKey(hk);
		return TRUE;
	}

	return FALSE;
}

BOOL linkspresent()
{
	//Check if frhed\subreleaseno\links exists
	HKEY hk;
	if(ERROR_SUCCESS==RegOpenKey(HKEY_CURRENT_USER, "Software\\frhed\\v"CURRENT_VERSION"." SUB_RELEASE_NO "\\links",&hk)){
		RegCloseKey(hk);
		return TRUE;
	}

	return FALSE;
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
	int np, i, n = -1;
	ScreenToClient(hwndStatusBar, &p);
	np = SendMessage(hwndStatusBar,SB_GETPARTS,0,0);
	for (i = 0 ; i < np ; i++)
	{
		SendMessage(hwndStatusBar, SB_GETRECT, i, (LPARAM)&rt);
		if (PtInRect(&rt,p))
		{
			n = i;
			break;//Can't be in > 1 rect at a time
		}
	}

	//Som variables used below
	int r, len, cn;
	char* text;
	SIZE s;
	HFONT fon[2];
	HDC dc;

	//Which rect are we in?
	if (n == 0 || n == 1)
	{
		//In one of the rects that requires the text & fonts
		//Initialize
		len = LOWORD(SendMessage(hwndStatusBar, SB_GETTEXTLENGTH, n, 0));
		text = new char[len+1];
		if( !text ) return;//Leave if we can't get the text
		SendMessage(hwndStatusBar, SB_GETTEXT, n, (LPARAM)text);
		//This font stuff plagued me for ages but know that you
		//need to put the right font in the dc as it won't have the right one
		//after a GetDC - the status bar must put it in during a paint
		dc = GetDC(hwndStatusBar);
		fon[0] = (HFONT) SendMessage(hwndStatusBar, WM_GETFONT, 0, 0);
		fon[1] = (HFONT) SelectObject(dc, fon[0]);
		GetTextExtentPoint32(dc,text,len,&s);
	}

	//Do the requested function
	switch(n)
	{

	case -1:
		{
			//The user clicked the status bar outside the rects
			//May want to add something useful here later
			//An easter egg will do for now
			int tmp = (IDYES==MessageBox(hwnd, "BOO","Scared ya, di'n' I, huh, huh?",MB_ICONQUESTION|MB_YESNO));
			MessageBox(hwnd, tmp?"Wwwwooooohhhhoooo!!! :-)":"Aww damn. ;-("," ",MB_ICONEXCLAMATION|MB_OK);
			break;
		}

	case 0:
		{
			//The user clicked in the 1st part - offset/bits/byte/word/dword
			//If there is a selection
			if (bSelected)
			{
				//Maybe the user wants to change the selection/deselect
				p.y = s.cx;
				GetTextExtentPoint32(dc, text, 9, &s);//len "Selected:" = 9
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
					bSelected = FALSE;
					repaint();
				}
				else
				{ //In offsets - reselect
					CMD_select_block();//This fixes up the status text by itself
				}
				break;//Skip to the deinitialisation code
			}

			//If the caret is on the END byte
			int flen = DataArray.GetLength();
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
			char *st = text;
			for (i = 0 ; i < len ; i++)
			{
				if (text[i] == '\t' && text[i + 1] == '\t')
				{
					text[i] = '\0';//Clip the string
					st = text + i + 2;//Remember the second part
					break;
				}
			}

			//There was no '\t\t' in the middle of the string - something went wrong
			if (text == st)
				break;

			//Get the start of the un/signed bit in client coords
			len = strlen(st);
			GetTextExtentPoint32(dc, st, len, &s);
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
				st = text;
				len = strlen(st);
			}

			//Find which character the used clicked in.
			GetTextExtentExPoint(dc, st, len, r, &cn, NULL, &s);

			//In the right or left aligned area?
			if (st != text )
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
					else if (GetKeyState(VK_SHIFT) & 0x8000 ||
						GetKeyState(VK_CONTROL) & 0x8000 ||
						GetKeyState(VK_MENU) & 0x8000)
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
							st = text + i + 1;
							len = i + 1;
							GetTextExtentPoint32(dc, text, len, &s);
							r = rt.left + s.cx;
							//Flip the equivalent of a "bit"
							for (i = 0 ; i < 7 ; i++)
							{
								GetTextExtentPoint32(dc, st, i + 1, &s);
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
							DataArray[iCurByte] = c;
							//Redraw the data & status bar etc
							bFilestatusChanged = iFileChanged = TRUE;
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
				GetTextExtentExPoint(dc, text, len, r, &cn, NULL, &s);
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
					if (bReadOnly)
					{
						bReadOnly = 0;
						iInsertMode = 1;
					}
					else if (iInsertMode)
						iInsertMode = 0;
					else
						bReadOnly = 1;
				}
				else //READ <- INS <- OVR <- READ...
				{
					if (bReadOnly)
					{
						bReadOnly = 0;
						iInsertMode= 0;
					}
					else if (iInsertMode)
						bReadOnly = 1;
					else
						iInsertMode = 1;
				}
				break;
			case 2: // L <--> B
				//This is not a typo - these preprocessor directives are required to make optimum code
				#if ((!LITTLEENDIAN_MODE) == BIGENDIAN_MODE) && (LITTLEENDIAN_MODE == (!BIGENDIAN_MODE))
				//If this does not work replace with the below
				iBinaryMode = !iBinaryMode;
				#else
				if (iBinaryMode == LITTLEENDIAN_MODE)
					iBinaryMode = BIGENDIAN_MODE;
				else if (iBinaryMode == BIGENDIAN_MODE)
					iBinaryMode = LITTLEENDIAN_MODE;
				#endif
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
		SelectObject(dc, fon[1]);
		ReleaseDC(hwndStatusBar, dc);
		delete [] text;
	}
}

void HexEditorWindow::CMD_adopt_colours()
{
	if( MessageBox( hwnd, "Really adopt the operating system colour scheme?", appname, MB_YESNO | MB_ICONQUESTION ) == IDYES )
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
	static_cast<dialog<MoveCopyDlg>*>(this)->DoModal(hwnd);
}

void HexEditorWindow::CMD_move_copy(int iMove1stEnd, int iMove2ndEndorLen, bool redraw)
{
	/*Call like so
	iMove1stEnd = position of start of block to move;
	iMove2ndEndorLen = position of end of block to move;
	CMD_move(1);
	*/
	int clen = DataArray.GetLength();
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
		if (!DataArray.SetSize(clen + len))
		{
			MessageBox(hwnd, "Not enough memory", "Move/Copy", MB_ICONERROR);
			return;
		}
		else
		{
			DataArray.ExpandToSize();
			memmove(&DataArray[iMovePos + len], &DataArray[iMovePos], clen - iMovePos);
			if (iMovePos > iMove1stEnd && iMovePos <= iMove2ndEndorLen)
			{
				memcpy(&DataArray[iMovePos], &DataArray[iMove1stEnd], iMovePos - iMove1stEnd);
				memcpy(&DataArray[iMovePos + iMovePos - iMove1stEnd], &DataArray[iMovePos + len], iMove1stEnd + len - iMovePos);
			}
			else
			{
				int tmp = iMovePos <= iMove1stEnd ? len : 0;
				memcpy(&DataArray[iMovePos], &DataArray[iMove1stEnd + tmp], len);
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
		reverse_bytes(&DataArray[ms], &DataArray[me]);
		reverse_bytes(&DataArray[os], &DataArray[oe]);
		reverse_bytes(&DataArray[ts], &DataArray[te]);
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
			bSelected = FALSE;
			iCurByte = iStartOfSelection + dist;
		}//If the above is not true deselect - this may change when multiple selections are allowed
	}
	//Or if the current byte was in the move/copied bit move it too
	else if (iCurByte >= iMove1stEnd && iCurByte <= iMove2ndEndorLen)
	{
		iCurByte += dist;
	}

	iFileChanged = TRUE;
	bFilestatusChanged = TRUE;
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
	static_cast<dialog<ReverseDlg>*>(this)->DoModal(hwnd);
}

int HexEditorWindow::CMD_setselection(int iSelStart, int iSelEnd) // Mike Funduc new function
{
	if (iSelStart >= 0 && iSelEnd >= iSelStart)
	{
		iStartOfSelection = iSelStart;
		iEndOfSelection = iSelEnd;
		bSelected = TRUE;
		adjust_view_for_selection();
		repaint();
		return 1;
	}
	return 0;
}

HGLOBAL HexEditorWindow::RTF_hexdump(int start, int end, DWORD* plen){
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
				char* FaceName = NULL;

				HDC hdc = GetDC(hwnd);
				HFONT hFontOld = (HFONT)SelectObject(hdc, hFont);
				UINT cbData = GetOutlineTextMetrics (hdc, 0, NULL);
				OUTLINETEXTMETRIC* otm = NULL;
				if(cbData){
					otm = (OUTLINETEXTMETRIC*)LocalAlloc(LPTR, cbData);
					if(otm) GetOutlineTextMetrics(hdc, cbData, otm);
				}
				SelectObject(hdc, hFontOld);
				ReleaseDC(hwnd,hdc);

				if(otm){
					FaceName = (char*)otm+(UINT)otm->otmpFaceName;
					PitchAndFamily = otm->otmTextMetrics.tmPitchAndFamily;
					CharSet = otm->otmTextMetrics.tmCharSet;
				} else {
					LOGFONT lf;
					GetObject(hFont,sizeof(lf), &lf);
					PitchAndFamily = lf.lfPitchAndFamily;
					CharSet = lf.lfCharSet;
					cbData = GetTextFace(hdc, 0, NULL);
					if(cbData){
						FaceName = (char*)malloc( cbData );
						if(FaceName) GetTextFace(hdc, cbData, FaceName);
					}
				}

				//Output the font family,
				switch( PitchAndFamily & /* bits 4-7<<4 */ 0xf0 ){
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
				if(otm){ s <<
					"{\\*\\panose " <<
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
				if(FaceName && FaceName[0]) s << escapefilter << FaceName;
				if(otm) LocalFree(LocalHandle(otm));
				else if(FaceName) free(FaceName);
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
			if( bSelected ){ s << //Regular selection colours
				/*Sel bck col*/"\\red" << (DWORD)GetRValue(iSelBkColorValue) << " \\green" << (DWORD)GetGValue(iSelBkColorValue) << " \\blue" << (DWORD)GetBValue(iSelBkColorValue) << ";\n"
				/*Sel txt col*/"\\red" << (DWORD)GetRValue(iSelTextColorValue) << " \\green" << (DWORD)GetGValue(iSelTextColorValue) << " \\blue" << (DWORD)GetBValue(iSelTextColorValue) << ";\n";
			} else { //Caret is the text colour inverted
				//Wish I could do iBkColorValueTmp = InvertColour(iBkColorValue)
				iBkColorValue=~iBkColorValue;iTextColorValue=~iTextColorValue; s <<
				/*Car bck col*/"\\red" << (DWORD)GetRValue(iBkColorValue) << " \\green" << (DWORD)GetGValue(iBkColorValue) << " \\blue" << (DWORD)GetBValue(iBkColorValue) << ";\n"
				/*Car txt col*/"\\red" << (DWORD)GetRValue(iTextColorValue) << " \\green" << (DWORD)GetGValue(iTextColorValue) << " \\blue" << (DWORD)GetBValue(iTextColorValue) << ";\n";
				iBkColorValue=~iBkColorValue;iTextColorValue=~iTextColorValue;
			} s <<
			/*Bookmarks  */"\\red" << (DWORD)GetRValue(iBmkColor) << " \\green" << (DWORD)GetGValue(iBmkColor) << " \\blue" << (DWORD)GetBValue(iBmkColor) << ";\n"
			//Separators */iSepColorValue is not needed because drawing objects specify their own colours (stupid eh?)
		"}\n" // \colortbl

		//This is new for RTF 1.7, but it should be ignored by older readers so who cares (older than M$ Word XP = Word 2002??)
		"{\\*\\generator frhed v"CURRENT_VERSION"."SUB_RELEASE_NO";}\n"

		//Metadata here too?
		"{\\info\n"
			//Put the filename in the title
			"{\\title " << escapefilter << filename << "}\n"
			//...
		"}\n"; // \info

		//Document formatting properties
			//Sot sure if this will have any effect
			if( bMakeBackups ) s << "\\makebackup ";
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
			escapefilter << filename <<
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
			"\\f0 \\fs" << (DWORD)iFontSize*2 << "\\cb0 \\chcbpat0 \\cf1"
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
		int endoffile = DataArray.GetUpperBound() + 1;
		if(start>endoffile)start=endoffile;
		if(end>endoffile)end=endoffile;
		start = start / iBytesPerLine * iBytesPerLine;//cut back to the line start
		end = end / iBytesPerLine * iBytesPerLine;//cut back to the line start
		int i = 0;
		//int bi = 0;
		BYTE c;
		//This bit needs to be optimized
		bool highlights_in_this_line;
		for (int l = start ; l <= end; l += iBytesPerLine)
		{
			s.precision = iMinOffsetLen;
			int bi = 0;
			bi -= s.m_dwLen;
			s <<
			//Offsets
			hex << (DWORD)l;
			
			//Bytespace
			bi += s.m_dwLen;
			for(i=0;i<iMaxOffsetLen+iByteSpace-bi;i++)
				s << "\\~";

			highlights_in_this_line = false;

			/*Wish I could do this in C++ - the alias would expire like a local variable
			alias sos iStartOfSelSetting, eos iEndOfSelSetting, bpl iBytesPerLine;*/

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
				int offset = pbmkList[i].offset;
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
				int eosl = min(iEndOfSelSetting, l+iBytesPerLine-1);
				//Bytes
				for(i=l;i<l+iBytesPerLine;i++){
					if(i==sosl) s <<
					"{\\cb2 \\chcbpat2 \\cf3 "; // iSelBkColorValue, iSelBkColorValue (Word2000 sux), iSelTextColorValue
						for( bi = 0; bi < iBmkCount; bi++ )
							if(pbmkList[bi].offset==i) break;
						if( bi < iBmkCount ){
							if(iEnteringMode == BYTES){ s <<
								"{\\*\\bkmkstart ";
									if( pbmkList[bi].name ) s << escapefilter << pbmkList[bi].name;
									else s << (DWORD)i;
									s <<
								"}";
							} s <<
							"{\\chbrdr \\brdrs \\brdrcf4 "; // iBmkColor
						}
						if( i==endoffile ) s << "__\\~";
						else if( i>endoffile ) s << "\\~\\~\\~";
						else s << hex << DataArray[i];
						if( bi < iBmkCount ){ s <<
							"}";
							if(iEnteringMode == BYTES){ s <<
								"{\\*\\bkmkend ";
									if(pbmkList[bi].name) s << escapefilter << pbmkList[bi].name;
									else s << (DWORD)i;
									s <<
								"}";
							}
						}
						if(i==eosl) s <<
					"}"; s << //Selected colours
					"\\~";
				} //Bytes
				//Charspace
				for(i=0;i<iCharSpace;i++)
					s << "\\~";
				//Chars
				for(i=l;i<l+iBytesPerLine;i++){
					if(i==sosl) s <<
					"{\\cb2 \\chcbpat2 \\cf3 "; // iSelBkColorValue, iSelBkColorValue (Word2000 sux), iSelTextColorValue
						for( bi = 0; bi < iBmkCount; bi++ )
							if(pbmkList[bi].offset==i) break;
						if( bi < iBmkCount ){
							if(iEnteringMode == CHARS){ s <<
								"{\\*\\bkmkstart ";
									if( pbmkList[bi].name ) s << escapefilter << pbmkList[bi].name;
									else s << (DWORD)i;
									s <<
								"}";
							} s <<
							"{\\chbrdr \\brdrs \\brdrcf4 "; // iBmkColor
						}
						if( i>=endoffile ) s << "\\~";
						else {
							c = DataArray[i];
							if(!( ( iCharacterSet == OEM_FIXED_FONT && c != 0 ) || ( iCharacterSet == ANSI_FIXED_FONT && ( ( c >= 32 && c <= 126) || (c>=160 && c<=255) || (c>=145 && c<=146) ) ) ))
								c = '.';
							s << nbsp << escapefilter << c;
						}
						if( bi < iBmkCount ){ s <<
							"}";
							if(iEnteringMode == CHARS){ s <<
								"{\\*\\bkmkend ";
									if(pbmkList[bi].name) s << escapefilter << pbmkList[bi].name;
									else s << (DWORD)i;
									s <<
								"}";
							}
						}
						if(i==eosl) s <<
					"}"; //Selected colours
				} //Chars
				s <<
				//End of line
				"\\line\n";
			} else /*No highlights - how boring*/ {
				//Bytes
				for(i=l;i<l+iBytesPerLine;i++){
					if( i==endoffile ) s << "__\\~";
					else if( i>endoffile ) s << "\\~\\~\\~";
					else s << hex << DataArray[i] << "\\~";
				}
				//Charspace
				for(i=0;i<iCharSpace;i++)
					s << "\\~";
				//Chars
				for(i=l;i<l+iBytesPerLine;i++){
					if( i>=endoffile ) s << "\\~";
						else {
							c = DataArray[i];
							if(!( ( iCharacterSet == OEM_FIXED_FONT && c != 0 ) || ( iCharacterSet == ANSI_FIXED_FONT && ( ( c >= 32 && c <= 126) || (c>=160 && c<=255) || (c>=145 && c<=146) ) ) ))
								c = '.';
							s << nbsp << escapefilter << c;
						}
				}
				//End of line
				s <<
				"\\line\n";
			} // No highlights
		} //for each line

		s <<
		//The vertical lines
		//8192 is a majick number used to bring the lines in front of the text
		//Count is +4 because +2 for the 2 extra lines at charstart +1 for the extra one at the bytes end +1 cause \dpcount needs to be 1 more than the no of lines
		"{\\*\\do \\dobxcolumn \\dobypara \\dodhgt8192 \\dpgroup \\dpcount" << (DWORD)(iBytesPerLine/4+4) << "\n";
			register int x;
			register int y = ((end-start)/iBytesPerLine+1)*cyChar*15;
			//They should have just used the colour table
			//but no they is Micro$oft with absolutely no clue.
			register DWORD r = GetRValue(iSepColorValue);
			register DWORD g = GetGValue(iSepColorValue);
			register DWORD b = GetBValue(iSepColorValue);
			//The lines in the bytespace
			for (i = 0; i < iBytesPerLine / 4 +1; i++){
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
	if( plen ) *plen = s.m_dwLen;
	return s.Relinquish();
}

void HexEditorWindow::EnableDriveButtons(BOOL bEnable)
{
	int IDS[] = { ID_DISK_GOTOFIRSTTRACK, ID_DISK_GOTONEXTTRACK, ID_DISK_GOTOPREVIOUSTRACK, ID_DISK_GOTOLASTTRACK, -1 };

	for( int i = 0; IDS[i] != -1; i++ )
	{
		int iCurrentFlags = (int)SendMessage(hwndToolBar, TB_GETSTATE, IDS[i], 0L);

		if( bEnable )
			iCurrentFlags |= TBSTATE_ENABLED;
		else
			iCurrentFlags &= ~TBSTATE_ENABLED;

		SendMessage(hwndToolBar, TB_SETSTATE, IDS[i], MAKELPARAM(iCurrentFlags, 0));
	}
}
