#ifndef hexwnd_h
#define hexwnd_h

#include "IDT.h"

#include "heksedit.h"

#define SHARPEN_A(X) #X
#define SHARPEN_W(X) L#X
#define SHARPEN(T,X) SHARPEN_##T(X)

#define CURRENT_VERSION SHARPEN(A,FRHED_MAJOR_VERSION) "." SHARPEN(A,FRHED_MINOR_VERSION)
#define SUB_RELEASE_NO SHARPEN(A,FRHED_SUB_RELEASE_NO)
#define BUILD_NO SHARPEN(A,FRHED_BUILD_NO)

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
#define LITTLEENDIAN_MODE 0
#define BIGENDIAN_MODE 1
#define SCROLL_TIMER_ID 1
#define SCROLL_DELAY_TIMER_ID 2
#define MOUSE_OP_DELAY_TIMER_ID 3
#define MRUMAX 9
#define BMKMAX 9
#define BMKTEXTMAX 256
#define TPL_TYPE_MAXLEN 16
#define TPL_NAME_MAXLEN 128

typedef struct
{
	int offset;
	char* name;
} bookmark;

//--------------------------------------------------------------------------------------------
// Global variables.
#include "simparr.h"

//--------------------------------------------------------------------------------------------
enum ClickArea { AREA_NONE, AREA_OFFSETS, AREA_BYTES, AREA_CHARS };
enum SCROLL_TYPE { SCROLL_NONE, SCROLL_BACK, SCROLL_FORWARD };
enum EnteringMode { BYTES, CHARS };

class hexfile_stream;
class load_hexfile_0;
class load_hexfile_1;

class LangArray;
extern LangArray langArray;

interface CDropTarget;

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
	void CMD_move_copy(bool redraw = 1);
	void CMD_move_copy(int iMove1stEnd, int iMove2ndEndorLen, bool redraw);
	void CMD_reverse();
	bool load_hexfile(hexfile_stream &);
	void CMD_open_hexdump();
	virtual void STDMETHODCALLTYPE status_bar_click(bool left);
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
	HGLOBAL RTF_hexdump(int start, int end, DWORD* plen = NULL);
	int CMD_OLEDD_options();
	CDropTarget* target;
	void start_mouse_operation();
	ClickArea area;
	void get_pos(long x, long y);
	void set_drag_caret(long x, long y, bool Copying, bool Overwrite );
	void fix_scroll_timers(long x, long y);
	void kill_scroll_timers();
	int lbd_pos;
	int nibblenum, bytenum, column, line, new_pos, old_pos, old_col, old_row;
	int bMouseOpDelayTimerSet;
	SCROLL_TYPE prev_vert;
	SCROLL_TYPE prev_horz;

//Pabs removed CMD_explorersettings
	int OnWndMsg( HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam );
	void CMD_summon_text_edit();
	void STDMETHODCALLTYPE CMD_find();
	void STDMETHODCALLTYPE CMD_findprev();
	void STDMETHODCALLTYPE CMD_findnext();
	void STDMETHODCALLTYPE CMD_replace();
	BOOL STDMETHODCALLTYPE select_next_diff(BOOL bFromStart);
	BOOL STDMETHODCALLTYPE select_prev_diff(BOOL bFromEnd);
	void CMD_colors_to_default();
	void CMD_goto();
	int read_tpl_token( char* pcTpl, int tpl_len, int& index, char* name );
	int ignore_non_code( char* pcTpl, int tpl_len, int& index );
	void apply_template_on_memory( char* pcTpl, int tpl_len, SimpleArray<char>& ResultArray );
	void apply_template(char *pcTemplate);
	void CMD_apply_template();
	virtual void STDMETHODCALLTYPE dropfiles(HDROP);
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
	void CMD_select_all();
	void CMD_on_backspace ();
	void CMD_toggle_insertmode();
	void CMD_character_set();
	void CMD_manipulate_bits();
	void CMD_edit_append();
	virtual void STDMETHODCALLTYPE save_ini_data();
//Pabs inserted "char* key = NULL"
	virtual void STDMETHODCALLTYPE read_ini_data(char *key = 0);
//end
	void CMD_color_settings(COLORREF* pColor);
	void CMD_view_settings();
	void adjust_view_for_selection ();
	int CMD_select_with_arrowkeys (int key);
	void CMD_open();
	int CMD_save();
	int CMD_save_as();
	int CMD_new(const char *title = 0);
	int create_bc_translation(char** ppd, char* src, int srclen);
	void CMD_edit_enterdecimalvalue();
	int CMD_copy_hexdump(int iCopyHexdumpMode, int iCopyHexdumpType, int iCopyHexdumpDlgStart, int iCopyHexdumpDlgEnd, char *mem = 0, int memlen = 0);
	void CMD_copy_hexdump();
	void CMD_edit_cut(int iMode);
	void STDMETHODCALLTYPE CMD_edit_cut();
	void STDMETHODCALLTYPE CMD_edit_copy();
	void STDMETHODCALLTYPE CMD_edit_paste();
	void STDMETHODCALLTYPE CMD_edit_clear();
	void STDMETHODCALLTYPE CMD_fast_paste();
	int mousemove(int xPos, int yPos);
	int lbuttonup(int xPos, int yPos);

	void EnableDriveButtons(BOOL bEnable);
	BOOL queryCommandEnabled(UINT id);

	virtual int STDMETHODCALLTYPE close(const char *caption = 0);
	virtual int STDMETHODCALLTYPE initmenupopup(WPARAM w, LPARAM l);
	void adjust_view_for_caret();
	void print_line(HDC hdc, int line, HBRUSH hbr);
	void mark_char(HDC hdc);
	void STDMETHODCALLTYPE adjust_hscrollbar();
	void STDMETHODCALLTYPE adjust_vscrollbar();
	void clear_all();
	void repaint(int line = -1);
	void synch_sibling(BOOL bSynchSelection = FALSE);
	void STDMETHODCALLTYPE repaint(int from, int to);//Pabs inserted

	HexEditorWindow();
	~HexEditorWindow();

	void STDMETHODCALLTYPE resize_window();
	unsigned char *STDMETHODCALLTYPE get_buffer(int);
	int STDMETHODCALLTYPE get_length();
	void STDMETHODCALLTYPE set_sibling(IHexEditorWindow *);
	void STDMETHODCALLTYPE set_status_bar(HWND);
	Colors *STDMETHODCALLTYPE get_colors();
	Settings *STDMETHODCALLTYPE get_settings();
	Status *STDMETHODCALLTYPE get_status();
	int STDMETHODCALLTYPE translate_accelerator(MSG *);
	BOOL STDMETHODCALLTYPE load_lang(LANGID);
	BSTR STDMETHODCALLTYPE load_string(UINT);
	void STDMETHODCALLTYPE free_string(BSTR);

	virtual int STDMETHODCALLTYPE load_file(const char* fname);
	int file_is_loadable(const char* fname);
	int at_window_create(HWND hw, HINSTANCE hI);
	void set_focus();
	void kill_focus();
	int lbuttondown( int nFlags, int xPos, int yPos);
	int snap_caret();
	void keydown(int key);
	void character(char ch);
	void vscroll(int cmd);
	void hscroll(int cmd);
	int paint();
	virtual void STDMETHODCALLTYPE command(int cmd);
	int destroy_window();
	virtual void STDMETHODCALLTYPE set_wnd_title();
	void set_caret_pos();
	void print_text(HDC hdc, int x, int y, char *pch, int cch);
	virtual HRESULT STDMETHODCALLTYPE ResolveIt(LPCSTR lpszLinkFile, LPSTR lpszPath);

	static void LoadStringTable();
	static void FreeStringTable();

public:
	int iWindowShowCmd, iWindowX, iWindowY, iWindowWidth, iWindowHeight;
	int iInstCount;
	HWND hwndMain;
	HWND hwndToolBar;
	HWND hwndStatusBar;
	HACCEL hAccel;

protected:
//Pabs inserted
	int bMakeBackups;//Backup the file when saving
//end
	static int ScrollDelay;
	static int ScrollInterval;
	static int MouseOpDist;
	static int MouseOpDelay;
	static SimpleString TexteditorName;
	static SimpleString EncodeDlls;
	static int iPasteAsText;
	static int iPasteTimes;
	static int iPasteSkip;

	int bOpenReadOnly;//Pabs inserted ", iPartialOpenLen, iPartialFileLen, bPartialStats"
	int iPartialOffset, bPartialOpen, iPartialOpenLen, iPartialFileLen, bPartialStats;
	int iBmkCount;
	bookmark pbmkList[BMKMAX];
	int iMRU_count;
	char strMRU[MRUMAX][_MAX_PATH];
	int bFilestatusChanged;
	int bScrollTimerSet;
	int iMouseX, iMouseY;
	int bUnsignedView;
	HFONT hFont;
	int iInsertMode;
	int bFileNeverSaved;
	SimpleArray<unsigned char> DataArray;
	int bLButtonDown, bSelecting, iLBDownX, iLBDownY;
	int cxChar, cxCaps, cyChar, cxClient, cyClient, cxBuffer, cyBuffer, iNumlines;
	int iByteSpace, iCharSpace;
	char filename[_MAX_PATH];
	HWND hwnd;
	HINSTANCE hInstance;
	IHexEditorWindow *sibling;

	IPhysicalDrive *Drive;
	PartitionInfo *SelectedPartitionInfo;

	PMemoryBlock Track;
	INT64 CurrentSectorNumber;

	static char equal(char);
	static char lower_case(char);
	static int find_bytes(char* ps, int ls, char* pb, int lb, int mode, char (*cmp)(char));
};

void reverse_bytes(BYTE *, BYTE *);

void NTAPI TranslateDialog(HWND);
INT_PTR NTAPI ShowModalDialog(UINT, HWND, DLGPROC, LPVOID);

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
			SetWindowLong(hWnd, DWLP_USER, lParam);
			TranslateDialog(hWnd);
		}
		return ((T *)GetWindowLong(hWnd, DWLP_USER))->DlgProc(hWnd, uMsg, wParam, lParam);
	}
public:
	INT_PTR DoModal(HWND hWnd)
	{
		return ShowModalDialog(IDD, hWnd, DlgProc, this);
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
