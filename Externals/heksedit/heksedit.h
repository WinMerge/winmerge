class IHexEditorWindow
{
public:
	struct Colors
	{
		COLORREF iTextColorValue, iBkColorValue, iSepColorValue;
		COLORREF iSelBkColorValue, iSelTextColorValue;
		COLORREF iBmkColor;
		COLORREF iDiffBkColorValue, iDiffTextColorValue, iSelDiffBkColorValue, iSelDiffTextColorValue;
	};
	struct Settings
	{
		int iBytesPerLine;
		int iAutomaticBPL;
		int iBinaryMode;
		int bReadOnly;
		int bSaveIni;
		int iFontSize;
		int iCharacterSet;
		int iMinOffsetLen;
		int iMaxOffsetLen;
		int bAutoOffsetLen;
		int bCenterCaret;
	};
	struct Status
	{
		int iFileChanged;
		int iEnteringMode;
		int iCurByte;
		int iCurNibble;
		int iVscrollMax;
		int iVscrollPos;
		int iHscrollMax;
		int iHscrollPos;
		int bSelected;
		int iStartOfSelection;
		int iEndOfSelection;
	};
	virtual unsigned char *STDMETHODCALLTYPE get_buffer(int) = 0;
	virtual int STDMETHODCALLTYPE get_length() = 0;
	virtual void STDMETHODCALLTYPE set_sibling(IHexEditorWindow *) = 0;
	virtual void STDMETHODCALLTYPE set_status_bar(HWND) = 0;
	virtual Colors *STDMETHODCALLTYPE get_colors() = 0;
	virtual Settings *STDMETHODCALLTYPE get_settings() = 0;
	virtual Status *STDMETHODCALLTYPE get_status() = 0;
	virtual void STDMETHODCALLTYPE resize_window() = 0;
	virtual void STDMETHODCALLTYPE repaint(int from, int to) = 0;
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
};
