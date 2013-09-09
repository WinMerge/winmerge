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
 * @file  hexwnd.h
 *
 * @brief Declaration of the hex editor window class.
 *
 */
#ifndef hexwnd_h
#define hexwnd_h

#include "Simparr.h"
#include "IDT.h"

#include "heksedit.h"

// This is frhed vCURRENT_VERSION.SUB_RELEASE_NO
// Its needed here even though nothing in this file uses macros!
#include "version.h"

#include "LangTools.h"

#include "PhysicalDrive.h"
#include "PMemoryBlock.h"

//--------------------------------------------------------------------------------------------
// Callback functions for dialogue boxes.
INT_PTR CALLBACK MultiDropDlgProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK TmplDisplayDlgProc(HWND, UINT, WPARAM, LPARAM);

//--------------------------------------------------------------------------------------------

#define ANSI_SET ANSI_FIXED_FONT
#define OEM_SET OEM_FIXED_FONT
#define SCROLL_TIMER_ID 1
#define SCROLL_DELAY_TIMER_ID 2
#define MOUSE_OP_DELAY_TIMER_ID 3
#define MRUMAX 9
#define BMKMAX 9
#define BMKTEXTMAX 256

/**
 * @brief Bookmark.
 */
typedef struct
{
	int offset; /**< Offset of the bookmark in bookmarks list. */
	String name; /**< Name of the bookmark. */
} bookmark;

//--------------------------------------------------------------------------------------------
enum ClickArea { AREA_NONE, AREA_OFFSETS, AREA_BYTES, AREA_CHARS };
enum SCROLL_TYPE { SCROLL_NONE, SCROLL_BACK, SCROLL_FORWARD };
enum EnteringMode { BYTES, CHARS };

class hexfile_stream;
class HexFile;
class FindCtxt;

class hexfile_stream;
class load_hexfile_0;
class load_hexfile_1;

class CDropTarget;

/**
 * @brief Hex editor window.
 * This class is the class handling the hex editor window. Dialogs are also
 * derived from this class.
 */
class HexEditorWindow
: public IHexEditorWindow
, public IHexEditorWindow::Colors
, public IHexEditorWindow::Settings
, public IHexEditorWindow::Status
{
	friend CDropTarget;
	friend load_hexfile_0;
	friend load_hexfile_1;
public:
	int iGetCharsPerLine();
	int iGetStartOfSelection();
	int iGetEndOfSelection();
	virtual int STDMETHODCALLTYPE CMD_setselection(int iSelStart, int iSelEnd);// MF new function

	//GK20AUG2K
	void CMD_GotoDllExports();
	void CMD_GotoDllImports();
	void CMD_EncodeDecode();
	void CMD_OpenDrive();
	void CMD_DriveGotoFirstTrack();
	void CMD_DriveGotoNextTrack();
	void CMD_DriveGotoPrevTrack();
	void CMD_DriveGotoLastTrack();
	void CMD_DriveGotoTrackNumber();
	void RefreshCurrentTrack();

	void CMD_CloseDrive();
	void OnContextMenu(LPARAM);

//Pabs changed - line insert
	bool dragging;
	void CMD_fw();//fill selection with command
	void CMD_revert();
	void CMD_saveselas();
	void CMD_deletefile();
	void CMD_insertfile();
	void CMD_move_copy(bool redraw = true);
	void CMD_move_copy(int iMove1stEnd, int iMove2ndEndorLen, bool redraw);
	void CMD_reverse();
	bool load_hexfile(hexfile_stream &);
	void CMD_open_hexdump();
	void status_bar_click(bool left);
	void CMD_adopt_colours();

	//General OLEDD options
	int enable_drop;
	int enable_drag;
	int enable_scroll_delay_dd;
	int enable_scroll_delay_sel;
	int always_pick_move_copy;
	//Input OLEDD options
	int prefer_CF_HDROP;
	int prefer_CF_BINARYDATA;
	int prefer_CF_TEXT;
	//Output OLEDD options
	int output_CF_BINARYDATA;
	int output_CF_TEXT;
	int output_text_special;
	int output_text_hexdump_display;
	int output_CF_RTF;
	HGLOBAL RTF_hexdump(int start, int end, SIZE_T *plen = NULL);
	int CMD_OLEDD_options();
	CDropTarget* target;
	void start_mouse_operation();
	ClickArea area;
	void get_pos(long x, long y);
	void set_drag_caret(long x, long y, bool Copying, bool Overwrite );
	void fix_scroll_timers(long x, long y);
	void kill_scroll_timers();
	void reset();
	int lbd_pos;
	int nibblenum, bytenum, column, line, new_pos, old_pos, old_col, old_row;
	int bMouseOpDelayTimerSet;
	SCROLL_TYPE prev_vert;
	SCROLL_TYPE prev_horz;

	LRESULT OnWndMsg(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
	void CMD_summon_text_edit();
	void STDMETHODCALLTYPE CMD_find();
	void STDMETHODCALLTYPE CMD_findprev();
	void STDMETHODCALLTYPE CMD_findnext();
	void STDMETHODCALLTYPE CMD_replace();
	BOOL STDMETHODCALLTYPE select_next_diff(BOOL bFromStart);
	BOOL STDMETHODCALLTYPE select_prev_diff(BOOL bFromEnd);
	void CMD_colors_to_default();
	void CMD_goto();
	void apply_template(TCHAR *pcTemplate);
	void CMD_apply_template();
	void dropfiles(HDROP);
	void CMD_open_partially();
	void CMD_clear_all_bmk();
	void CMD_remove_bkm();
	void CMD_goto_bookmark(int i);
	void make_bookmark_list(HMENU);
	void CMD_add_bookmark ();
	void CMD_MRU_selected(int i);
	void make_MRU_list(HMENU);
	void update_MRU();
	void CMD_select_block();
	void timer(WPARAM, LPARAM);
	void CMD_binarymode();
	void CMD_compare();
	void CMD_properties();
	void make_font();
	void CMD_on_backspace ();
	void CMD_toggle_insertmode();
	void CMD_character_set();
	void CMD_manipulate_bits();
	void CMD_edit_append();
	virtual void STDMETHODCALLTYPE save_ini_data();
//Pabs inserted "char* key = NULL"
	virtual void STDMETHODCALLTYPE read_ini_data(TCHAR *key = 0);
//end
	void CMD_color_settings(COLORREF* pColor);
	void CMD_view_settings();
	void adjust_view_for_selection ();
	int CMD_select_with_arrowkeys (int key);
	void CMD_open();
	int CMD_save();
	int CMD_save_as();
	int CMD_new();
	void CMD_edit_enterdecimalvalue();
	int CMD_copy_hexdump(int iCopyHexdumpMode, int iCopyHexdumpType, int iCopyHexdumpDlgStart, int iCopyHexdumpDlgEnd, char *mem = 0, DWORD memlen = 0);
	void CMD_copy_hexdump();
	void STDMETHODCALLTYPE CMD_edit_cut();
	void STDMETHODCALLTYPE CMD_edit_copy();
	void STDMETHODCALLTYPE CMD_edit_paste();
	void STDMETHODCALLTYPE CMD_edit_clear();
	void STDMETHODCALLTYPE CMD_fast_paste();
	void STDMETHODCALLTYPE CMD_select_all();
	void STDMETHODCALLTYPE CMD_zoom(int);
	int mousemove(int xPos, int yPos);
	void mousewheel(int delta);
	int lbuttonup(int xPos, int yPos);

	void EnableDriveButtons(BOOL bEnable);
	BOOL queryCommandEnabled(UINT id);

	virtual bool STDMETHODCALLTYPE close();
	virtual int STDMETHODCALLTYPE initmenupopup(WPARAM w, LPARAM l);
	void adjust_view_for_caret();
	void print_line(HSurface *pdc, int line, HBrush *pbr);
	void PrintBookmarkIndicators(HSurface *pdc, HBrush *pbr, int startpos);
	void mark_char(HSurface *pdc);
	void STDMETHODCALLTYPE adjust_hscrollbar();
	void STDMETHODCALLTYPE adjust_vscrollbar();
	void clear_all();
	void repaint(int line = -1);
	void synch_sibling(BOOL bSynchSelection = FALSE);
	void STDMETHODCALLTYPE repaint(int from, int to);//Pabs inserted

	HexEditorWindow();
	~HexEditorWindow();

	unsigned STDMETHODCALLTYPE get_interface_version();
	void STDMETHODCALLTYPE resize_window();
	BYTE *STDMETHODCALLTYPE get_buffer(int);
	int STDMETHODCALLTYPE get_length();
	void STDMETHODCALLTYPE set_sibling(IHexEditorWindow *);
	Colors *STDMETHODCALLTYPE get_colors();
	Settings *STDMETHODCALLTYPE get_settings();
	Status *STDMETHODCALLTYPE get_status();
	int STDMETHODCALLTYPE translate_accelerator(MSG *);
	BOOL STDMETHODCALLTYPE load_lang(LANGID langid, LPCWSTR langdir = NULL);
	LPTSTR STDMETHODCALLTYPE load_string(UINT);
	void STDMETHODCALLTYPE free_string(LPTSTR);
	HMENU STDMETHODCALLTYPE load_menu(UINT);

	int load_file(LPCTSTR);
	virtual int STDMETHODCALLTYPE open_file(LPCWSTR);
	int file_is_loadable(LPCTSTR fname);
	int at_window_create(HWND, HINSTANCE);
	void set_control_bar(HWindow *);
	void set_focus();
	void kill_focus();
	int lbuttondown(int nFlags, int xPos, int yPos);
	void snap_caret();
	void keydown(int key);
	void character(char ch);
	void scroll_window(int dx, int dy);
	void vscroll(int cmd);
	void hscroll(int cmd);
	int paint();
	virtual void STDMETHODCALLTYPE command(int cmd);
	void notify(NMHDR *);
	int destroy_window();
	virtual void STDMETHODCALLTYPE set_wnd_title();
	void set_caret_pos();
	void print_text(HSurface *pdc, int x, int y, TCHAR *pch, int cch);
	virtual HRESULT STDMETHODCALLTYPE ResolveIt(LPCTSTR lpszLinkFile, LPTSTR lpszPath);
	static void OnHelp(HWindow *);

	static void LoadStringTable();
	static void FreeStringTable();

public:
	int iWindowShowCmd, iWindowX, iWindowY, iWindowWidth, iWindowHeight;
	int iInstCount;
	HWND hwndMain; /**< Handle to main window. */
	HToolBar *pwndToolBar; /**< Handle to main window's toolbar. */
	HStatusBar *pwndStatusBar; /**< Handle to main window's statusbar. */
	HACCEL hAccel; /**< Handle to accelerator keys list. */

protected:
//Pabs inserted
	int bMakeBackups; /**< Backup the file when saving. */
//end
	static int ScrollDelay;
	static int ScrollInterval;
	static int MouseOpDist;
	static int MouseOpDelay;
	static TCHAR TexteditorName[MAX_PATH];
	static TCHAR EncodeDlls[MAX_PATH];
	static bool bPasteAsText; /**< Paste binary data as text? */
	static int iPasteTimes;
	static int iPasteSkip;

	int bOpenReadOnly; /**< Open files read-only. */
	INT64 iPartialOffset, iPartialFileLen;
	bool bPartialOpen; /**< Was file opened partially? */
	int iPartialOpenLen; /**< Length of the partially opened block from file. */
	bool bPartialStats;
	int iBmkCount; /**< Count of bookmarks. */
	bookmark pbmkList[BMKMAX]; /**< List of bookmarks. */
	int iMRU_count; /**< Count of MRU list items active. */
	TCHAR strMRU[MRUMAX][_MAX_PATH]; /**< List of MRU items. */
	bool bFilestatusChanged;
	bool bScrollTimerSet;
	int iMouseX; /**< Current mouse X position. */
	int iMouseY; /**< Current mouse Y position. */
	bool bUnsignedView; /**< View shows byte values as unsigned? */
	HFont *pFont;
	bool bInsertMode; /**< Editing mode is in insert-mode? */
	bool bFileNeverSaved;
	SimpleArray<BYTE> m_dataArray;
	int bSelecting; /**< Is user selecting bytes/text? */
	int iLBDownX, iLBDownY;
	int cxChar, cxCaps, cyChar, cxClient, cyClient, cxBuffer, cyBuffer, iNumlines;
	int iByteSpace, iCharSpace;
	TCHAR filename[_MAX_PATH];
	HWindow *pwnd;
	HINSTANCE hInstance;
	IHexEditorWindow *sibling;

	IPhysicalDrive *Drive;
	PartitionInfo *SelectedPartitionInfo;

	PMemoryBlock Track;
	INT64 CurrentSectorNumber;

	void set_and_format_title();
	void format_bit_string(TCHAR* buf, BYTE by);
	void statusbar_chset_and_editmode();

	FindCtxt *m_pFindCtxt;
};

void reverse_bytes(BYTE *, BYTE *);

class WaitCursor
{
private:
	HCURSOR cur;
public:
	WaitCursor()
	{
		cur = SetCursor(LoadCursor(0, IDC_WAIT));
	}
	~WaitCursor()
	{
		SetCursor(cur);
	}
};

template<class T>
class dialog : public T
{
	static INT_PTR CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == WM_INITDIALOG)
		{
			SetWindowLongPtr(hWnd, DWLP_USER, lParam);
			TranslateDialog(hWnd);
		}
		return ((T *)GetWindowLongPtr(hWnd, DWLP_USER))->DlgProc(
			reinterpret_cast<HWindow *>(hWnd), uMsg, wParam, lParam);
	}
public:
	int DoModal(HWindow *pWnd)
	{
		return ShowModalDialog(IDD, pWnd, DlgProc, this);
	}
};

template<class T> inline void swap(T& x, T& y)
{
	T temp = x;
	x = y;
	y = temp;
}

extern HINSTANCE hMainInstance;
extern int iMovePos;
enum OPTYP { OPTYP_MOVE, OPTYP_COPY };
extern OPTYP iMoveOpTyp;
extern const CLIPFORMAT CF_BINARYDATA;
extern const CLIPFORMAT CF_RICH_TEXT_FORMAT;
#endif // hexwnd_h
