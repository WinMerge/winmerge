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

Last change: 2017-06-18 by Jochen Neubeck
*/
/** 
 * @file  heksedit.h
 *
 * @brief Interface definition for the hekseditor window.
 *
 */
#ifndef _HEKSEDIT_H_
#define _HEKSEDIT_H_

#ifndef HEKSEDIT_INTERFACE_VERSION
#define HEKSEDIT_INTERFACE_VERSION 3
#define const(x) x
#endif

/**
 * @brief Interface for the Hexeditor window.
 * @note This file is published to hosting applications. Do not add global declarations.
 */
class IHexEditorWindow
{
public:
	/**
	 * @brief Byte endians.
	 */
	enum BYTE_ENDIAN
	{
		ENDIAN_LITTLE, /**< Little endian used e.g. in Intel processors. */
		ENDIAN_BIG /**< Big endian used e.g. in Motorola processors. */
	};

	struct Colors
	{
		COLORREF iTextColorValue, iBkColorValue, iSepColorValue;
		COLORREF iSelBkColorValue, iSelTextColorValue;
		COLORREF iBmkColor;
		COLORREF iDiffBkColorValue, iDiffTextColorValue, iSelDiffBkColorValue, iSelDiffTextColorValue;
	};

	/** @brief Frhed settings. */
	struct Settings
	{
		int iBytesPerLine; /**< How many bytes in one line in hex view. */
		int iAutomaticBPL; /**< Add max amount of bytes that fit to view. */
		BYTE_ENDIAN iBinaryMode; /**< Binary mode, little/big endian. */
		int bReadOnly; /**< Is editor in read-only mode? */
		int bSaveIni; /**< Save INI file when required. */
		int iFontSize;
		int iCharacterSet; /**< Use OEM or ANSI character set? */
		int iMinOffsetLen; /**< Minimum numbers used in offset. */
		int iMaxOffsetLen; /**< Maximum numbers used in offset. */
		int bAutoOffsetLen; /**< Determine offset length automatically. */
		int bCenterCaret;
		int iFontZoom;
	};

	struct Status
	{
		int iFileChanged;
		int const(iEnteringMode);
		size_t const(iCurByte);
		int const(iCurNibble);
		int64_t const(iVscrollMax64);
		int64_t const(iVscrollPos64);
		int const(iHscrollMax);
		int const(iHscrollPos);
		bool const(bSelected);
		size_t const(iStartOfSelection);
		size_t const(iEndOfSelection);
	};

	struct SharedUndoRecords;

	virtual unsigned STDMETHODCALLTYPE get_interface_version() = 0;
	virtual unsigned char *STDMETHODCALLTYPE get_buffer(size_t) = 0;
	virtual size_t STDMETHODCALLTYPE get_length() = 0;
	virtual void STDMETHODCALLTYPE set_sibling(IHexEditorWindow *) = 0;
	virtual Colors *STDMETHODCALLTYPE get_colors() = 0;
	virtual Settings *STDMETHODCALLTYPE get_settings() = 0;
	virtual Status *STDMETHODCALLTYPE get_status() = 0;
	virtual void STDMETHODCALLTYPE resize_window() = 0;
	virtual void STDMETHODCALLTYPE repaint(size_t from, size_t to) = 0;
	virtual void STDMETHODCALLTYPE adjust_hscrollbar() = 0;
	virtual void STDMETHODCALLTYPE adjust_vscrollbar() = 0;
	virtual int STDMETHODCALLTYPE translate_accelerator(MSG *) = 0;
	virtual void STDMETHODCALLTYPE CMD_find() = 0;
	virtual void STDMETHODCALLTYPE CMD_findprev() = 0;
	virtual void STDMETHODCALLTYPE CMD_findnext() = 0;
	virtual void STDMETHODCALLTYPE CMD_replace() = 0;
	virtual void STDMETHODCALLTYPE CMD_edit_cut() = 0;
	virtual void STDMETHODCALLTYPE CMD_edit_copy() = 0;
	virtual void STDMETHODCALLTYPE CMD_edit_paste() = 0;
	virtual void STDMETHODCALLTYPE CMD_edit_clear() = 0;
	virtual BOOL STDMETHODCALLTYPE select_next_diff(BOOL bFromStart) = 0;
	virtual BOOL STDMETHODCALLTYPE select_prev_diff(BOOL bFromEnd) = 0;
	virtual BOOL STDMETHODCALLTYPE load_lang(LANGID langid, LPCWSTR langdir = NULL) = 0;
	virtual void STDMETHODCALLTYPE CMD_zoom(int) = 0;
	virtual void STDMETHODCALLTYPE CMD_select_all() = 0;
#if HEKSEDIT_INTERFACE_VERSION < 2
private:
#endif
	virtual void STDMETHODCALLTYPE copy_sel_from(IHexEditorWindow *) = 0;
	virtual void STDMETHODCALLTYPE copy_all_from(IHexEditorWindow *) = 0;
	virtual void STDMETHODCALLTYPE CMD_edit_undo() = 0;
	virtual void STDMETHODCALLTYPE CMD_edit_redo() = 0;
	virtual BOOL STDMETHODCALLTYPE can_undo() const = 0;
	virtual BOOL STDMETHODCALLTYPE can_redo() const = 0;
	virtual void STDMETHODCALLTYPE set_savepoint() = 0;
	virtual void STDMETHODCALLTYPE clear_undorecords() = 0;
	virtual SharedUndoRecords *STDMETHODCALLTYPE share_undorecords(SharedUndoRecords *) = 0;
	virtual void STDMETHODCALLTYPE set_sibling2(IHexEditorWindow *, IHexEditorWindow *) = 0;
	virtual void STDMETHODCALLTYPE CMD_view_settings() = 0;
	virtual void STDMETHODCALLTYPE CMD_binarymode() = 0;
	virtual void STDMETHODCALLTYPE CMD_character_set() = 0;
	virtual void STDMETHODCALLTYPE read_ini_data(TCHAR *key = 0) = 0;
	virtual void STDMETHODCALLTYPE CMD_goto() = 0;
#if HEKSEDIT_INTERFACE_VERSION >= 3
	enum WINDOW_TYPE { WINDOW_DIALOG = 0, };
	typedef void (*ApplyThemeCallback)(HWND hwnd, WINDOW_TYPE windowType);
	virtual void STDMETHODCALLTYPE set_theme_callback(ApplyThemeCallback callback) = 0;
#endif
};

#undef const

#endif // _HEKSEDIT_H_
