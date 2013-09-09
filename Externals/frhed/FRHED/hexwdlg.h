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

Last change: 2013-02-24 by Jochen Neubeck
*/
/** 
 * @file  hexwdlg.h
 *
 * @brief Dialog class declarations.
 *
 */
#ifndef _HEXW_DLG_H_
#define _HEXW_DLG_H_

#include "Simparr.h"
#include "Resource.h"

void GetWindowText(HWindow *, SimpleString &);
void GetDlgItemText(HWindow *, int, SimpleString &);
BOOL EnableDlgItem(HWindow *, int, BOOL);
BOOL IsDlgItemEnabled(HWindow *, int);
int MessageBox(HWindow *, LPCTSTR, UINT);
int CheckHResult(HWindow *, HRESULT, UINT = MB_ICONSTOP);

/**
 * @brief About-dialog for Frhed application.
 * This dialog shows program name, version and contributor info.
 */
class AboutDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_ABOUTDIALOG };
	INT_PTR DlgProc(HWindow *, UINT, WPARAM, LPARAM);
protected:
	BOOL OnInitDialog(HWindow *);
	BOOL OnCommand(HWindow *, WPARAM, LPARAM);
};

class DragDropOptionsDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_DRAG_DROP_OPTIONS };
	INT_PTR DlgProc(HWindow *, UINT, WPARAM, LPARAM);
};

/**
 * @brief View settings dialog class.
 */
class ViewSettingsDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_VIEWSETTINGSDIALOG };
	INT_PTR DlgProc(HWindow *, UINT, WPARAM, LPARAM);
private:
	static HComboBox *pCbLang;
	static BOOL CALLBACK EnumLocalesProc(LPTSTR);
	static int FormatLangId(LPWSTR, LANGID, bool verbose = false);
	void OnDrawitemLangId(DRAWITEMSTRUCT *);
	INT_PTR OnCompareitemLangId(COMPAREITEMSTRUCT *);
	BOOL OnInitDialog(HWindow *);
	BOOL Apply(HWindow *);
	void SelectEditor(HWindow *);
};

class CharacterSetDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_CHARACTERSETDIALOG };
	INT_PTR DlgProc(HWindow *, UINT, WPARAM, LPARAM);
};

class BinaryModeDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_BINARYMODEDIALOG };
	INT_PTR DlgProc(HWindow *, UINT, WPARAM, LPARAM);
};

/**
 * A dialog for filling area with bytes.
 */
class FillWithDialog : public HexEditorWindow
{
public:
	enum { IDD = IDD_FILL_WITH };
	INT_PTR DlgProc(HWindow *, UINT, WPARAM, LPARAM);
private:
	enum { FW_MAX = 1024 }; //max bytes to fill with
	static TCHAR pcFWText[FW_MAX];//hex representation of bytes to fill with
	static BYTE buf[FW_MAX];//bytes to fill with
	static int buflen;//number of bytes to fill with
	static TCHAR szFWFileName[_MAX_PATH];//fill with file name
	static int FWFile,FWFilelen;//fill with file and len
	static LONG_PTR oldproc;//old hex box proc
	static HFONT hfon;//needed so possible to display infinity char in fill with dlg box
	static TCHAR curtyp;//filling with input-0 or file-1
	static TCHAR asstyp;
	void inittxt(HWindow *);
	static BYTE input(int);
	static BYTE file(int);
	static void hexstring2charstring();
	static void deletenonhex(HWND);
	static LRESULT CALLBACK HexProc(HWND, UINT, WPARAM, LPARAM);
};

/**
 * A dialog for reversing bytes in the selected area.
 */
class ReverseDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_REVERSE_BYTES_DIALOG };
	INT_PTR DlgProc(HWindow *, UINT, WPARAM, LPARAM);
};

class ShortcutsDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_SHORTCUTS };
	INT_PTR DlgProc(HWindow *, UINT, WPARAM, LPARAM);
private:
	BOOL OnInitDialog(HWindow *);
	BOOL OnCommand(HWindow *, WPARAM, LPARAM);
	BOOL OnNotify(HWindow *, WPARAM, LPARAM);
	static int CALLBACK BrowseCallbackProc(HWND, UINT, LPARAM, LPARAM);
	static int CALLBACK SearchCallbackProc(HWND, UINT, LPARAM, LPARAM);
};

class ChangeInstDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_CHANGEINST };
	INT_PTR DlgProc(HWindow *, UINT, WPARAM, LPARAM);
};

/**
 * @brief A find dialog.
 * This dialog allows user to give a string (as readable text) or byte
 * sequence to find from the open file. The byte sequence has special
 * syntax.
 */
class FindDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_FINDDIALOG };
	INT_PTR DlgProc(HWindow *, UINT, WPARAM, LPARAM);
private:
	BOOL OnInitDialog(HWindow *);
	BOOL OnCommand(HWindow *, WPARAM, LPARAM);
};

/**
 * @brief A replace dialog.
 * This dialog allows user to give a string (as readable text) or byte
 * sequence to replace from the open file. The byte sequence has special
 * syntax.
 */
class ReplaceDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_REPLACEDIALOG };
	INT_PTR DlgProc(HWindow *, UINT, WPARAM, LPARAM);
	int transl_text_to_binary(SimpleArray<BYTE> &out);
	static int transl_binary_to_text(const BYTE*, int);
	bool find_and_select_data(int finddir, bool case_sensitive);
	bool replace_selected_data(HWindow *);
	void find_directed(HWindow *, int finddir);
	void replace_directed(HWindow *, int finddir, bool showCount);
	static SimpleString strToReplaceData;
	static SimpleString strReplaceWithData;
};

/**
 * @brief A dialog for chooding diffed file.
 */
class ChooseDiffDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_CHOOSEDIFFDIALOG };
	INT_PTR DlgProc(HWindow *, UINT, WPARAM, LPARAM);
private:
	BOOL OnInitDialog(HWindow *);
	BOOL OnCommand(HWindow *, WPARAM, LPARAM);
	void add_diff(HListBox *, int diff, int lower, int upper);
	int get_diffs(HListBox *, BYTE *ps, int sl, BYTE *pd, int dl);
};

class CopyHexdumpDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_HEXDUMPDIALOG };
	INT_PTR DlgProc(HWindow *, UINT, WPARAM, LPARAM);
	static int iCopyHexdumpMode;
	static int iCopyHexdumpType;
private:
	BOOL OnInitDialog(HWindow *);
	BOOL OnCommand(HWindow *, WPARAM, LPARAM);
};

/**
 * @brief A dialog for adding bookmark.
 */
class AddBmkDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_ADDBMK_DIALOG };
	INT_PTR DlgProc(HWindow *, UINT, WPARAM, LPARAM);
private:
	BOOL OnInitDialog(HWindow *);
	BOOL OnCommand(HWindow *, WPARAM, LPARAM);
};

/**
 * @brief A dialog for removing bookmark.
 */
class RemoveBmkDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_REMOVEBMK_DIALOG };
	INT_PTR DlgProc(HWindow *, UINT, WPARAM, LPARAM);
private:
	BOOL OnInitDialog(HWindow *);
	BOOL OnCommand(HWindow *, WPARAM, LPARAM);
};

/**
 * @brief A dialog for selecting a block of bytes.
 */
class SelectBlockDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_SELECT_BLOCK_DIALOG };
	INT_PTR DlgProc(HWindow *, UINT, WPARAM, LPARAM);
private:
	BOOL OnInitDialog(HWindow *);
	BOOL OnCommand(HWindow *, WPARAM, LPARAM);
};

/**
 * @brief A dialog for pasting bytes to editor.
 * This version of Paste-dialog shows the data in the clipboard and allows
 * the user to edit data before pasting it.
 */
class PasteDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_PASTEDIALOG };
	INT_PTR DlgProc(HWindow *, UINT, WPARAM, LPARAM);
private:
	BOOL OnInitDialog(HWindow *);
	BOOL OnCommand(HWindow *, WPARAM, LPARAM);
	BOOL Apply(HWindow *);
};

/**
 * @brief A dialog for normal pasting of bytes to editor.
 * This is the default paste-dialog (opened e.g. from Ctrl+V).
 */
class FastPasteDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_FASTPASTE_DIALOG };
	INT_PTR DlgProc(HWindow *, UINT, WPARAM, LPARAM);
private:
	BOOL OnInitDialog(HWindow *);
	BOOL OnCommand(HWindow *, WPARAM, LPARAM);
	BOOL Apply(HWindow *);
	void RefreshClipboardFormats(HWindow *);
};

/**
 * @brief A dialog for Moving and copying bytes.
 * This dialog allows user to move and copy bytes inside the file.
 */
class MoveCopyDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_MOVE_COPY };
	INT_PTR DlgProc(HWindow *, UINT, WPARAM, LPARAM);
private:
	BOOL OnInitDialog(HWindow *);
	bool ReadStartOffset(HWindow *, int &);
	bool ReadEndOffset(HWindow *, int &);
	bool ReadTargetOffset(HWindow *, int &);
	BOOL OnCommand(HWindow *, WPARAM, LPARAM);
	BOOL Apply(HWindow *);
};

/**
 * @brief A dialog for editing data as decimal numbers.
 */
class EnterDecimalValueDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_DECIMALDIALOG };
	INT_PTR DlgProc(HWindow *, UINT, WPARAM, LPARAM);
	static int iDecValDlgSize; /**< Value length (Byte/Word/DWord). */
	static bool bSigned; /**< Edit value as signed value? */
private:
	BOOL OnInitDialog(HWindow *);
	BOOL Apply(HWindow *);
};

/**
 * @brief A Goto-dialog.
 * This dialog allows user to jump to certain position in the file. The jump
 * can be absolute or relative. An absolute jump goes to the position counted
 * from the begin of the file. The relative jump goes given amount backwards
 * or forward in the file.
 */
class GoToDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_GOTODIALOG };
	INT_PTR DlgProc(HWindow *, UINT, WPARAM, LPARAM);
private:
	BOOL OnInitDialog(HWindow *);
	BOOL Apply(HWindow *);
};

/**
 * @brief A dialog for Cut-feature.
 * This dialog handles cutting bytes from the file. The dialog shows
 * selection (if any) start- and end-offesets. Also the selection
 * (if any) length in bytes is shown. User can edit these values before
 * the operation.
 */
class CutDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_CUTDIALOG };

	INT_PTR DlgProc(HWindow *, UINT, WPARAM, LPARAM);
private:
	BOOL OnInitDialog(HWindow *);
	BOOL Apply(HWindow *);
};

/**
 * @brief A dialog for Delete-feature.
 * This dialog handles deleting bytes from the file. The dialog shows
 * selection (if any) start- and end-offesets. Also the selection
 * (if any) length in bytes is shown. User can edit these values before
 * the operation.
 */
class DeleteDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_DELETEDIALOG };

	INT_PTR DlgProc(HWindow *, UINT, WPARAM, LPARAM);
private:
	BOOL OnInitDialog(HWindow *);
	BOOL Apply(HWindow *);
};

/**
 * A dialog for copying the bytes.
 */
class CopyDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_COPYDIALOG };
	INT_PTR DlgProc(HWindow *, UINT, WPARAM, LPARAM);
private:
	BOOL OnInitDialog(HWindow *);
	BOOL Apply(HWindow *);
};

/**
 * @brief A dialog for appending bytes to the file.
 */
class AppendDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_APPENDDIALOG };
	INT_PTR DlgProc(HWindow *, UINT, WPARAM, LPARAM);
private:
	BOOL OnInitDialog(HWindow *);
	BOOL Apply(HWindow *);
};

/**
 * @brief A dialog for manipulating single bits in selected byte.
 */
class BitManipDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_MANIPBITSDIALOG };
	INT_PTR DlgProc(HWindow *, UINT, WPARAM, LPARAM);
private:
	BOOL OnInitDialog(HWindow *);
	BOOL Apply(HWindow *, WPARAM);
};

/**
 * @brief Dialog for opening file partially.
 * This dialog allows user to select range of bytes in file to open.
 * Only the selected range is opened into the hex editor.
 */
class OpenPartiallyDlg : public HexEditorWindow
{
public:
	enum { IDD = IDD_OPEN_PARTIAL_DIALOG };
	INT_PTR DlgProc(HWindow *, UINT, WPARAM, LPARAM);
	static int filehandle;
	static bool bShowFileStatsPL;
private:
	BOOL OnInitDialog(HWindow *);
	BOOL Apply(HWindow *);
};

#endif // _HEXW_DLG_H_
