////////////////////////////////////////////////////////////////////////////
//  File:       ccrystaltextview.h
//  Version:    1.0.0.0
//  Created:    29-Dec-1998
//
//  Author:     Stcherbatchenko Andrei
//  E-mail:     windfall@gmx.de
//
//  Interface of the CCrystalTextView class, a part of Crystal Edit -
//  syntax coloring text editor.
//
//  You are free to use or modify this code to the following restrictions:
//  - Acknowledge me somewhere in your about box, simple "Parts of code by.."
//  will be enough. If you can't (or don't want to), contact me personally.
//  - LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//  19-Jul-99
//      Ferdinand Prantl:
//  +   FEATURE: see cpps ...
//
//  ... it's being edited very rapidly so sorry for non-commented
//        and maybe "ugly" code ...
////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CCRYSTALTEXTVIEW_H__AD7F2F41_6CB3_11D2_8C32_0080ADB86836__INCLUDED_)
#define AFX_CCRYSTALTEXTVIEW_H__AD7F2F41_6CB3_11D2_8C32_0080ADB86836__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "cregexp.h"
#include "crystalparser.h"

////////////////////////////////////////////////////////////////////////////
// Forward class declarations

class CCrystalTextBuffer;
class CUpdateContext;


////////////////////////////////////////////////////////////////////////////
// CCrystalTextView class declaration

//  CCrystalTextView::FindText() flags
enum
{
  FIND_MATCH_CASE = 0x0001,
  FIND_WHOLE_WORD = 0x0002,
  FIND_REGEXP = 0x0004,
  FIND_DIRECTION_UP = 0x0010,
  REPLACE_SELECTION = 0x0100, 
  REPLACE_NO_WRAP = 0x200
};

//  CCrystalTextView::UpdateView() flags
enum
{
  UPDATE_HORZRANGE = 0x0001,  //  update horz scrollbar
  UPDATE_VERTRANGE = 0x0002, //  update vert scrollbar
  UPDATE_SINGLELINE = 0x0100,    //  single line has changed
  UPDATE_FLAGSONLY = 0x0200, //  only line-flags were changed

  UPDATE_RESET = 0x1000       //  document was reloaded, update all!
};

class EDITPADC_CLASS CCrystalTextView : public CView
  {
    DECLARE_DYNCREATE (CCrystalTextView)

	//BEGIN SW
	friend CCrystalParser;
	//END SW

protected:
    //  Search parameters
    BOOL m_bLastSearch;
    DWORD m_dwLastSearchFlags;
    LPTSTR m_pszLastFindWhat;
    BOOL m_bMultipleSearch;       // More search

private :
    BOOL m_bCursorHidden;

    //  Painting caching bitmap
    CBitmap *m_pCacheBitmap;

    //  Line/character dimensions
    int m_nLineHeight, m_nCharWidth;
    void CalcLineCharDim ();

    //  Text attributes
    BOOL m_bViewTabs;
    BOOL m_bViewEols;
    BOOL m_bDistinguishEols;
    BOOL m_bSelMargin;
    DWORD m_dwFlags;

    //  Amount of lines/characters that completely fits the client area
    int m_nScreenLines, m_nScreenChars;

    //BEGIN SW
    /**
    Contains for each line the number of sublines. If the line is not
    wrapped, the value for this line is 1. The value of a line is invalid,
    if it is -1.

    Must create pointer, because contructor uses AFX_ZERO_INIT_OBJECT to
    initialize the member objects. This would destroy a CArray object.
    */
    CArray<int, int> *m_panSubLines;
    //END SW

    int m_nMaxLineLength;
    int m_nIdealCharPos;

    BOOL m_bFocused;
protected:
    CPoint m_ptAnchor;
private:
    LOGFONT m_lfBaseFont;
    CFont *m_apFonts[4];

    //  Parsing stuff

    /**  
    This array must be initialized to (DWORD) - 1, code for invalid values (not yet computed).
    We prefer to limit the recomputing delay to the moment when we need to read
    a parseCookie value for drawing.
    GetParseCookie must always be used to read the m_ParseCookies value of a line.
    If the actual value is invalid code, GetParseCookie computes the value, 
    stores it in m_ParseCookies, and returns the new valid value.
    When we edit the text, the parse cookies value may change for the modified line
    and all the lines below (As m_ParseCookies[line i] depends on m_ParseCookies[line (i-1)])
    It would be a loss of time to recompute all these values after each action.
    So we just set all these values to invalid code (DWORD) - 1.
    */
    CArray<DWORD, DWORD> *m_ParseCookies;
    DWORD GetParseCookie (int nLineIndex);

    /**
    Pre-calculated line lengths (in characters)
    This array works as the parse cookie Array
    and must be initialized to - 1, code for invalid values (not yet computed).
    for the same reason.
    */
    CArray<int, int> *m_pnActualLineLength;

protected:
    BOOL m_bPreparingToDrag;
    BOOL m_bDraggingText;
    BOOL m_bDragSelection, m_bWordSelection, m_bLineSelection;
    UINT m_nDragSelTimer;

    CPoint m_ptDrawSelStart, m_ptDrawSelEnd;

    CPoint m_ptCursorPos, m_ptCursorLast;
    CPoint m_ptSelStart, m_ptSelEnd;
    void PrepareSelBounds ();

    //  Helper functions
    void ExpandChars (LPCTSTR pszChars, int nOffset, int nCount, CString & line);

    int ApproxActualOffset (int nLineIndex, int nOffset);
    void AdjustTextPoint (CPoint & point);
    void DrawLineHelperImpl (CDC * pdc, CPoint & ptOrigin, const CRect & rcClip,
                             LPCTSTR pszChars, int nOffset, int nCount);
    BOOL IsInsideSelBlock (CPoint ptTextPos);

    BOOL m_bBookmarkExist;        // More bookmarks

public :
    virtual void ResetView ();
    virtual int GetLineCount ();
    virtual void OnUpdateCaret ();
    BOOL IsTextBufferInitialized () const;
    CString GetTextBufferEol (int nLine) const;

protected :
    CPoint WordToRight (CPoint pt);
    CPoint WordToLeft (CPoint pt);
    BOOL           m_bOverrideCaret;

    bool m_bSingle;
    CImageList * m_pIcons;
    CCrystalTextBuffer *m_pTextBuffer;
    HACCEL m_hAccel;
    BOOL m_bVertScrollBarLocked, m_bHorzScrollBarLocked;
    CPoint m_ptDraggedTextBegin, m_ptDraggedTextEnd;
    void UpdateCaret ();
    void SetAnchor (const CPoint & ptNewAnchor);
    int GetMarginWidth ();
    bool IsValidTextPos (const CPoint &point);
    bool IsValidTextPosX (const CPoint &point);
    bool IsValidTextPosY (const CPoint &point);

    BOOL m_bShowInactiveSelection;
    //  [JRT]
    BOOL m_bDisableDragAndDrop;

    //BEGIN SW
    BOOL m_bWordWrap;
    CCrystalParser *m_pParser;
    //END SW

    CPoint ClientToText (const CPoint & point);
    CPoint TextToClient (const CPoint & point);
    void InvalidateLines (int nLine1, int nLine2, BOOL bInvalidateMargin = FALSE);
    int CalculateActualOffset (int nLineIndex, int nCharIndex);

    //  Printing
    int m_nPrintPages;
    int *m_pnPages;
    CFont *m_pPrintFont;
    int m_nPrintLineHeight;
    BOOL m_bPrintHeader, m_bPrintFooter;
    CRect m_ptPageArea, m_rcPrintArea;
    int PrintLineHeight (CDC * pdc, int nLine);
    void RecalcPageLayouts (CDC * pdc, CPrintInfo * pInfo);
    virtual void PrintHeader (CDC * pdc, int nPageNum);
    virtual void PrintFooter (CDC * pdc, int nPageNum);
    virtual void GetPrintHeaderText (int nPageNum, CString & text);
    virtual void GetPrintFooterText (int nPageNum, CString & text);

    //  Keyboard handlers
    void MoveLeft (BOOL bSelect);
    void MoveRight (BOOL bSelect);
    void MoveWordLeft (BOOL bSelect);
    void MoveWordRight (BOOL bSelect);
    void MoveUp (BOOL bSelect);
    void MoveDown (BOOL bSelect);
    void MoveHome (BOOL bSelect);
    void MoveEnd (BOOL bSelect);
    void MovePgUp (BOOL bSelect);
    void MovePgDn (BOOL bSelect);
    void MoveCtrlHome (BOOL bSelect);
    void MoveCtrlEnd (BOOL bSelect);

    void SelectAll ();
    void Copy ();

    BOOL IsSelection ();
    BOOL IsInsideSelection (const CPoint & ptTextPos);
    void GetSelection (CPoint & ptStart, CPoint & ptEnd);
    void GetFullySelectedLines(int & firstLine, int & lastLine);
    virtual void SetSelection (const CPoint & ptStart, const CPoint & ptEnd);

    int m_nTopLine, m_nOffsetChar;
    //BEGIN SW
    /**
    The index of the subline that is the first visible line on the screen.
    */
    int m_nTopSubLine;
    //END SW
    BOOL m_bSmoothScroll;

    int GetLineHeight ();
	//BEGIN SW
	/**
	Returns the number of sublines the given line contains of.
	Allway "1", if word wrapping is disabled.

	@param nLineIndex Index of the line to get the subline count of.

	@return Number of sublines the given line contains of
	*/
	int GetSubLines( int nLineIndex );

	/**
	Converts the given character position for the given line into a point.

	After the call the x-member of the returned point contains the
	character position relative to the beginning of the subline. The y-member
	contains the zero based index of the subline relative to the line, the
	character position was given for.

	@param nLineIndex Zero based index of the line, nCharPos refers to.
	@param nCharPos The character position, the point shoult be calculated for.
	@param charPoint Reference to a point, which should receive the result.

	@return The character position of the beginning of the subline charPoint.y.
	*/
	int CharPosToPoint( int nLineIndex, int nCharPos, CPoint &charPoint );

	/**
	Converts the given cursor point for the given line to the character position
	for the given line.

	The y-member of the cursor position specifies the subline inside the given
	line, the cursor is placed on and the x-member specifies the cursor position
	(in character widths) relative to the beginning of that subline.

	@param nLineIndex Zero based index of the line the cursor position refers to.
	@param curPoint Position of the cursor relative to the line in sub lines and
		char widths.

	@return The character position the best matches the cursor position.
	*/
	int CursorPointToCharPos( int nLineIndex, const CPoint &curPoint );

	/**
	Converts the given cursor position to a text position.

	The x-member of the subLinePos parameter describes the cursor position in
	char widths relative to the beginning of the subline described by the
	y-member. The subline is the zero based number of the subline relative to
	the beginning of the text buffer.

	<p>
	The returned point contains a valid text position, where the y-member is
	the zero based index of the textline and the x-member is the character
	position inside this line.

	@param subLinePos The sublinebased cursor position
		(see text above for detailed description).
	@param textPos The calculated line and character position that best matches
		the cursor position (see text above for detailed descritpion).
	*/
	void SubLineCursorPosToTextPos( const CPoint &subLinePos, CPoint &textPos );

	/**
	Returns the character position relative to the given line, that matches
	the end of the given sub line.

	@param nLineIndex Zero based index of the line to work on.
	@param nSubLineOffset Zero based index of the subline inside the given line.

	@return Character position that matches the end of the given subline, relative
		to the given line.
	*/
	int SubLineEndToCharPos( int nLineIndex, int nSubLineOffset );

	/**
	Returns the character position relative to the given line, that matches
	the start of the given sub line.

	@param nLineIndex Zero based index of the line to work on.
	@param nSubLineOffset Zero based index of the subline inside the given line.

	@return Character position that matches the start of the given subline, relative
		to the given line.
	*/
	int SubLineHomeToCharPos( int nLineIndex, int nSubLineOffset );
	//END SW
    int GetCharWidth ();
    int GetMaxLineLength ();
    int GetScreenLines ();
    int GetScreenChars ();
    CFont *GetFont (BOOL bItalic = FALSE, BOOL bBold = FALSE);

    void RecalcVertScrollBar (BOOL bPositionOnly = FALSE);
    virtual void RecalcHorzScrollBar (BOOL bPositionOnly = FALSE);

    //  Scrolling helpers
    void ScrollToChar (int nNewOffsetChar, BOOL bNoSmoothScroll = FALSE, BOOL bTrackScrollBar = TRUE);
    void ScrollToLine (int nNewTopLine, BOOL bNoSmoothScroll = FALSE, BOOL bTrackScrollBar = TRUE);

	//BEGIN SW
	/**
	Scrolls to the given sub line, so that the sub line is the first visible
	line on the screen.

	@param nNewTopSubLine Index of the sub line to scroll to.
	@param bNoSmoothScroll TRUE to disable smooth scrolling, else FALSE.
	@param bTrackScrollBar TRUE to recalculate the scroll bar after scrolling,
		else FALSE.
	*/
	virtual void ScrollToSubLine( int nNewTopSubLine, BOOL bNoSmoothScroll = FALSE, BOOL bTrackScrollBar = TRUE );
	//END SW

    //  Splitter support
    virtual void UpdateSiblingScrollPos (BOOL bHorz);
    virtual void OnUpdateSibling (CCrystalTextView * pUpdateSource, BOOL bHorz);
    CCrystalTextView *GetSiblingView (int nRow, int nCol);

	//BEGIN SW
	/**
	Returns the number of sublines in the whole text buffer.

	The number of sublines is the sum of all sublines of all lines.

	@return Number of sublines in the whole text buffer.
	*/
	virtual int GetSubLineCount();

	/**
	Returns the zero-based subline index of the given line.

	@param nLineIndex Index of the line to calculate the subline index of

	@return The zero-based subline index of the given line.
	*/
	virtual int GetSubLineIndex( int nLineIndex );

	/**
	Splits the given subline index into line and sub line of this line.

	@param nSubLineIndex The zero based index of the subline to get info about
	@param nLine Gets the line number the give subline is included in
	@param nSubLine Get the subline of the given subline relative to nLine
	*/
	virtual void GetLineBySubLine( int nSubLineIndex, int &nLine, int &nSubLine );
	//END SW

public:
    virtual int GetLineLength (int nLineIndex) const;
    virtual int GetFullLineLength (int nLineIndex) const;
    virtual int GetLineActualLength (int nLineIndex);
    virtual LPCTSTR GetLineChars (int nLineIndex) const;
protected:
    virtual DWORD GetLineFlags (int nLineIndex) const;
    virtual void GetText (const CPoint & ptStart, const CPoint & ptEnd, CString & text);

    //  Clipboard overridable
    virtual BOOL TextInClipboard ();
    virtual BOOL PutToClipboard (LPCTSTR pszText);
    virtual BOOL GetFromClipboard (CString & text);

    //  Drag-n-drop overrideable
    virtual HGLOBAL PrepareDragData ();
    virtual DROPEFFECT GetDropEffect ();
    virtual void OnDropSource (DROPEFFECT de);
    BOOL IsDraggingText () const;

    virtual COLORREF GetColor (int nColorIndex);
    virtual void GetLineColors (int nLineIndex, COLORREF & crBkgnd,
                                COLORREF & crText, BOOL & bDrawWhitespace);
    virtual BOOL GetItalic (int nColorIndex);
    virtual BOOL GetBold (int nColorIndex);

    void DrawLineHelper (CDC * pdc, CPoint & ptOrigin, const CRect & rcClip, int nColorIndex,
                         LPCTSTR pszChars, int nOffset, int nCount, CPoint ptTextPos);
    virtual void DrawSingleLine (CDC * pdc, const CRect & rect, int nLineIndex);
    virtual void DrawMargin (CDC * pdc, const CRect & rect, int nLineIndex);

#ifdef _UNICODE
    int GetCharWidthUnicodeChar(wchar_t ch);
#endif

	//BEGIN SW
	// word wrapping

	/**
	Called to wrap the line with the given index into sublines.

	The standard implementation wraps the line at the first non-whitespace after
	an whitespace that exceeds the visible line width (nMaxLineWidth). Override
	this function to provide your own word wrapping.

	<b>Attention:</b> Never call this function directly,
	call WrapLineCached() instead, which calls this method.

	@param nLineIndex The index of the line to wrap

	@param nMaxLineWidth The number of characters a subline of this line should
	not exceed (except whitespaces)

	@param anBreaks An array of integers. Put the positions where to wrap the line
	in that array (its allready allocated). If this pointer is NULL, the function
	has only to compute the number of breaks (the parameter nBreaks).

	@param nBreaks The number of breaks this line has (number of sublines - 1). When
	the function is called, this variable is 0. If the line is not wrapped, this value
	should be 0 after the call.

	@see WrapLineCached()
	*/
	virtual void WrapLine( int nLineIndex, int nMaxLineWidth, int *anBreaks, int &nBreaks );

	/**
	Called to wrap the line with the given index into sublines.

	Call this method instead of WrapLine() (which is called internal by this
	method). This function uses an internal cache which contains the number
	of sublines for each line, so it has only to call WrapLine(), if the
	cache for the given line is invalid or if the caller wants to get the
	wrap postions (anBreaks != NULL).

	This functions also tests m_bWordWrap -- you can call it even if
	word wrapping is disabled and you will retrieve a valid value.

	@param nLineIndex The index of the line to wrap

	@param nMaxLineWidth The number of characters a subline of this line should
	not exceed (except whitespaces)

	@param anBreaks An array of integers. Put the positions where to wrap the line
	in that array (its allready allocated). If this pointer is NULL, the function
	has only to compute the number of breaks (the parameter nBreaks).

	@param nBreaks The number of breaks this line has (number of sublines - 1). When
	the function is called, this variable is 0. If the line is not wrapped, this value
	should be 0 after the call.

	@see WrapLine()
	@see m_anSubLines
	*/
	void WrapLineCached( int nLineIndex, int nMaxLineWidth, int *anBreaks, int &nBreaks );

	/**
	Invalidates the cached data for the given lines.

	<b>Remarks:</b> Override this method, if your derived class caches other
	view specific line info, which becomes invalid, when this line changes.
	Call this standard implementation in your overriding.

	@param nLineIndex1 The index of the first line to invalidate.

	@param nLineIndex2 The index of the last line to invalidate. If this value is
	-1 (default) all lines from nLineIndex1 to the end are invalidated.
	*/
	virtual void InvalidateLineCache( int nLineIndex1, int nLineIndex2 );
	//END SW

    //  Syntax coloring overrides
    struct TEXTBLOCK
      {
        int m_nCharPos;
        int m_nColorIndex;
      };

    virtual HINSTANCE GetResourceHandle ();

	//BEGIN SW
	// function to draw a single screen line
	// (a wrapped line can consist of many screen lines
	virtual void DrawScreenLine( CDC *pdc, CPoint &ptOrigin, const CRect &rcClip,
		TEXTBLOCK *pBuf, int nBlocks, int &nActualItem,
		COLORREF crText, COLORREF crBkgnd, BOOL bDrawWhitespace,
		LPCTSTR pszChars,
		int nOffset, int nCount, CPoint ptTextPos );
	//END SW


	//BEGIN SW
	// helpers for incremental search

	/**
	Called each time the position-information in the status bar
	is updated. Use this to change the text of the message field
	in the status bar.

	@param pStatusBar
		Pointer to the status bar
	*/
	void OnUpdateStatusMessage( CStatusBar *pStatusBar );

	/**
	Called by OnFindIncrementalForward() and OnFindIncrementalBackward().

	@param bFindNextOccurence
		TRUE, if the method should look for the next occurence of the
		search string in search direction.

	@see #OnFindIncrementalForward
	@see #OnFindIncrementalBackward
	*/
	void OnEditFindIncremental( BOOL bFindNextOccurence = FALSE );

	/** TRUE if incremental forward search is active, FALSE otherwise */
	BOOL m_bIncrementalSearchForward;

	/** TRUE if incremental backward search is active, FALSE otherwise */
	BOOL m_bIncrementalSearchBackward;

private:
	/** TRUE if we found the string to search for */
	BOOL m_bIncrementalFound;

	/** String we are looking for.*/
	CString *m_pstrIncrementalSearchString;

	/** String we looked for last time.*/
	CString *m_pstrIncrementalSearchStringOld;

	/** Start of selection at the time the incremental search started */
	CPoint m_selStartBeforeIncrementalSearch;

	/** Start of selection at the time the incremental search started */
	CPoint m_selEndBeforeIncrementalSearch;

	/** Cursor position at the time the incremental search started */
	CPoint m_cursorPosBeforeIncrementalSearch;

	/** position to start the incremental search at */
	CPoint m_incrementalSearchStartPos;

	//END SW

public :
    void GoToLine (int nLine, bool bRelative);
    DWORD ParseLine (DWORD dwCookie, int nLineIndex, TEXTBLOCK * pBuf, int &nActualItems);
    DWORD ParseLinePlain (DWORD dwCookie, int nLineIndex, TEXTBLOCK * pBuf, int &nActualItems);
    DWORD ParseLineAsp (DWORD dwCookie, int nLineIndex, TEXTBLOCK * pBuf, int &nActualItems);
    DWORD ParseLineBasic (DWORD dwCookie, int nLineIndex, TEXTBLOCK * pBuf, int &nActualItems);
    DWORD ParseLineBatch (DWORD dwCookie, int nLineIndex, TEXTBLOCK * pBuf, int &nActualItems);
    DWORD ParseLineC (DWORD dwCookie, int nLineIndex, TEXTBLOCK * pBuf, int &nActualItems);
    DWORD ParseLineCSharp (DWORD dwCookie, int nLineIndex, TEXTBLOCK * pBuf, int &nActualItems);
    DWORD ParseLineDcl (DWORD dwCookie, int nLineIndex, TEXTBLOCK * pBuf, int &nActualItems);
    DWORD ParseLineFortran (DWORD dwCookie, int nLineIndex, TEXTBLOCK * pBuf, int &nActualItems);
    DWORD ParseLineHtml (DWORD dwCookie, int nLineIndex, TEXTBLOCK * pBuf, int &nActualItems);
    DWORD ParseLineIni (DWORD dwCookie, int nLineIndex, TEXTBLOCK * pBuf, int &nActualItems);
    DWORD ParseLineIS (DWORD dwCookie, int nLineIndex, TEXTBLOCK * pBuf, int &nActualItems);
    DWORD ParseLineJava (DWORD dwCookie, int nLineIndex, TEXTBLOCK * pBuf, int &nActualItems);
    DWORD ParseLineLisp (DWORD dwCookie, int nLineIndex, TEXTBLOCK * pBuf, int &nActualItems);
    DWORD ParseLinePascal (DWORD dwCookie, int nLineIndex, TEXTBLOCK * pBuf, int &nActualItems);
    DWORD ParseLinePerl (DWORD dwCookie, int nLineIndex, TEXTBLOCK * pBuf, int &nActualItems);
    DWORD ParseLinePhp (DWORD dwCookie, int nLineIndex, TEXTBLOCK * pBuf, int &nActualItems);
    DWORD ParseLinePython (DWORD dwCookie, int nLineIndex, TEXTBLOCK * pBuf, int &nActualItems);
    DWORD ParseLineRexx (DWORD dwCookie, int nLineIndex, TEXTBLOCK * pBuf, int &nActualItems);
    DWORD ParseLineRsrc (DWORD dwCookie, int nLineIndex, TEXTBLOCK * pBuf, int &nActualItems);
    DWORD ParseLineSgml (DWORD dwCookie, int nLineIndex, TEXTBLOCK * pBuf, int &nActualItems);
    DWORD ParseLineSh (DWORD dwCookie, int nLineIndex, TEXTBLOCK * pBuf, int &nActualItems);
    DWORD ParseLineSiod (DWORD dwCookie, int nLineIndex, TEXTBLOCK * pBuf, int &nActualItems);
    DWORD ParseLineSql (DWORD dwCookie, int nLineIndex, TEXTBLOCK * pBuf, int &nActualItems);
    DWORD ParseLineTcl (DWORD dwCookie, int nLineIndex, TEXTBLOCK * pBuf, int &nActualItems);
    DWORD ParseLineTex (DWORD dwCookie, int nLineIndex, TEXTBLOCK * pBuf, int &nActualItems);
    DWORD ParseLineXml (DWORD dwCookie, int nLineIndex, TEXTBLOCK * pBuf, int &nActualItems);

    // Attributes
public :
    int GetCRLFMode ();
    void SetCRLFMode (int nCRLFMode);
    BOOL GetViewTabs ();
    void SetViewTabs (BOOL bViewTabs);
    void SetViewEols (BOOL bViewEols, BOOL bDistinguishEols);
    int GetTabSize ();
    void SetTabSize (int nTabSize);
    BOOL GetSelectionMargin ();
    void SetSelectionMargin (BOOL bSelMargin);
    void GetFont (LOGFONT & lf);
    void SetFont (const LOGFONT & lf);
    DWORD GetFlags ();
    void SetFlags (DWORD dwFlags);
    BOOL GetSmoothScroll () const;
    void SetSmoothScroll (BOOL bSmoothScroll);
    //  [JRT]:
    BOOL GetDisableDragAndDrop () const;
    void SetDisableDragAndDrop (BOOL bDDAD);

	//BEGIN SW
	BOOL GetWordWrapping() const;
	void SetWordWrapping( BOOL bWordWrap );

	/**
	Sets the Parser to use to parse the file.

	@param pParser Pointer to parser to use. Set to NULL to use no parser.

	@return Pointer to parser used before or NULL, if no parser has been used before.
	*/
	CCrystalParser *SetParser( CCrystalParser *pParser );
	//END SW

    //  Default handle to resources
    static HINSTANCE s_hResourceInst;

    int m_nLastFindWhatLen;
    RxNode *m_rxnode;
    RxMatchRes m_rxmatch;
    LPTSTR m_pszMatched;
    static LOGFONT m_LogFont;

    typedef enum
    {
      SRC_PLAIN,
      SRC_ASP,
      SRC_BASIC,
      SRC_BATCH,
      SRC_C,
	  SRC_CSHARP,
      SRC_DCL,
      SRC_FORTRAN,
      SRC_HTML,
      SRC_INI,
      SRC_INSTALLSHIELD,
      SRC_JAVA,
      SRC_LISP,
      SRC_PASCAL,
      SRC_PERL,
      SRC_PHP,
      SRC_PYTHON,
      SRC_REXX,
      SRC_RSRC,
      SRC_SGML,
      SRC_SH,
      SRC_SIOD,
      SRC_SQL,
      SRC_TCL,
      SRC_TEX,
      SRC_XML
    }
    TextType;

    struct TextDefinition
      {
        TextType type;
        TCHAR name[256];
        TCHAR exts[256];
        DWORD (CCrystalTextView:: * ParseLineX) (DWORD dwCookie, int nLineIndex, TEXTBLOCK * pBuf, int &nActualItems);
        DWORD flags;
//        DWORD tabsize;
        TCHAR opencomment[8];
        TCHAR closecomment[8];
        TCHAR commentline[8];
        DWORD encoding;
      };

#define SRCOPT_INSERTTABS 1
#define SRCOPT_SHOWTABS 2
#define SRCOPT_BSATBOL 4
#define SRCOPT_SELMARGIN 8
#define SRCOPT_AUTOINDENT 16
#define SRCOPT_BRACEANSI 32
#define SRCOPT_BRACEGNU 64
#define SRCOPT_EOLNDOS 128
#define SRCOPT_EOLNUNIX 256
#define SRCOPT_EOLNMAC 512
#define SRCOPT_FNBRACE 1024
#define SRCOPT_WORDWRAP 2048

    //  Source type
    TextDefinition *m_CurSourceDef;
    static TextDefinition m_SourceDefs[];
    bool m_bRememberLastPos;
    virtual bool DoSetTextType (TextDefinition *def);
    static TextDefinition *GetTextType (LPCTSTR pszExt);
    virtual bool SetTextType (LPCTSTR pszExt);
    virtual bool SetTextType (CCrystalTextView::TextType enuType);
    virtual bool SetTextType (CCrystalTextView::TextDefinition *def);
    static void LoadSettings ();
    static void SaveSettings ();

    enum
    {
      //  Base colors
      COLORINDEX_WHITESPACE,
      COLORINDEX_BKGND,
      COLORINDEX_NORMALTEXT,
      COLORINDEX_SELMARGIN,
      COLORINDEX_SELBKGND,
      COLORINDEX_SELTEXT,
      //  Syntax colors
      COLORINDEX_KEYWORD,
      COLORINDEX_FUNCNAME,
      COLORINDEX_COMMENT,
      COLORINDEX_NUMBER,
      COLORINDEX_OPERATOR,      // [JRT]:
      COLORINDEX_STRING,
      COLORINDEX_PREPROCESSOR,
      COLORINDEX_USER1,
      COLORINDEX_USER2,
      //  Compiler/debugger colors
      COLORINDEX_ERRORBKGND,
      COLORINDEX_ERRORTEXT,
      COLORINDEX_EXECUTIONBKGND,
      COLORINDEX_EXECUTIONTEXT,
      COLORINDEX_BREAKPOINTBKGND,
      COLORINDEX_BREAKPOINTTEXT
      //  ...
      //  Expandable: custom elements are allowed.
    };

    // Operations
public :
    virtual void ReAttachToBuffer (CCrystalTextBuffer * pBuf = NULL);
    virtual void AttachToBuffer (CCrystalTextBuffer * pBuf = NULL);
    virtual void DetachFromBuffer ();

    //  Buffer-view interaction, multiple views
    virtual CCrystalTextBuffer *LocateTextBuffer ();
    virtual void UpdateView (CCrystalTextView * pSource, CUpdateContext * pContext, DWORD dwFlags, int nLineIndex = -1);

    //  Attributes
    CPoint GetCursorPos () const;
    virtual void SetCursorPos (const CPoint & ptCursorPos);
    void ShowCursor ();
    void HideCursor ();

    //  Operations
    virtual void EnsureVisible (CPoint pt);

    //  Text search helpers
    BOOL FindText (LPCTSTR pszText, const CPoint & ptStartPos, DWORD dwFlags, BOOL bWrapSearch, CPoint * pptFoundPos);
    BOOL FindTextInBlock (LPCTSTR pszText, const CPoint & ptStartPos, const CPoint & ptBlockBegin, const CPoint & ptBlockEnd,
                          DWORD dwFlags, BOOL bWrapSearch, CPoint * pptFoundPos);
    BOOL HighlightText (const CPoint & ptStartPos, int nLength, BOOL bReverse = FALSE);

    //  Overridable: an opportunity for Auto-Indent, Smart-Indent etc.
    virtual void OnEditOperation (int nAction, LPCTSTR pszText);

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CCrystalTextView)
public :
    virtual void OnDraw (CDC * pDC);  // overridden to draw this view

    virtual BOOL PreCreateWindow (CREATESTRUCT & cs);
    virtual BOOL PreTranslateMessage (MSG * pMsg);
    virtual void OnPrepareDC (CDC * pDC, CPrintInfo * pInfo = NULL);
    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO *pHandlerInfo);
protected :
    virtual void OnInitialUpdate ();  // called first time after construct

    virtual BOOL OnPreparePrinting (CPrintInfo * pInfo);
    virtual void OnBeginPrinting (CDC * pDC, CPrintInfo * pInfo);
    virtual void OnEndPrinting (CDC * pDC, CPrintInfo * pInfo);
    virtual void OnPrint (CDC * pDC, CPrintInfo * pInfo);
    //}}AFX_VIRTUAL

    // Implementation
public :
    CCrystalTextView ();
    ~CCrystalTextView ();

protected :

    // Generated message map functions
protected :
#ifdef _DEBUG
    void AssertValidTextPos (const CPoint & pt);
#endif

    //{{AFX_MSG(CCrystalTextView)
    afx_msg void OnDestroy ();
    afx_msg BOOL OnEraseBkgnd (CDC * pDC);
    afx_msg void OnSize (UINT nType, int cx, int cy);
    afx_msg void OnVScroll (UINT nSBCode, UINT nPos, CScrollBar * pScrollBar);
    afx_msg BOOL OnSetCursor (CWnd * pWnd, UINT nHitTest, UINT message);
    afx_msg void OnLButtonDown (UINT nFlags, CPoint point);
    afx_msg void OnSetFocus (CWnd * pOldWnd);
    afx_msg void OnHScroll (UINT nSBCode, UINT nPos, CScrollBar * pScrollBar);
    afx_msg void OnLButtonUp (UINT nFlags, CPoint point);
    afx_msg void OnMouseMove (UINT nFlags, CPoint point);
    afx_msg void OnTimer (UINT nIDEvent);
    afx_msg void OnKillFocus (CWnd * pNewWnd);
    afx_msg void OnLButtonDblClk (UINT nFlags, CPoint point);
    afx_msg void OnEditCopy ();
    afx_msg void OnUpdateEditCopy (CCmdUI * pCmdUI);
    afx_msg void OnEditSelectAll ();
    afx_msg void OnUpdateEditSelectAll (CCmdUI * pCmdUI);
    afx_msg void OnRButtonDown (UINT nFlags, CPoint point);
    afx_msg void OnSysColorChange ();
    afx_msg int OnCreate (LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnEditFind ();
    afx_msg void OnEditRepeat ();
    afx_msg void OnUpdateEditRepeat (CCmdUI * pCmdUI);
    afx_msg void OnEditDeleteBack();
    afx_msg void OnChar( UINT nChar, UINT nRepCnt, UINT nFlags );

    afx_msg BOOL OnMouseWheel (UINT nFlags, short zDelta, CPoint pt);
    //}}AFX_MSG
    afx_msg void OnFilePageSetup ();

    afx_msg void OnCharLeft ();
    afx_msg void OnExtCharLeft ();
    afx_msg void OnCharRight ();
    afx_msg void OnExtCharRight ();
    afx_msg void OnWordLeft ();
    afx_msg void OnExtWordLeft ();
    afx_msg void OnWordRight ();
    afx_msg void OnExtWordRight ();
    afx_msg void OnLineUp ();
    afx_msg void OnExtLineUp ();
    afx_msg void OnLineDown ();
    afx_msg void OnExtLineDown ();
    afx_msg void OnPageUp ();
    afx_msg void OnExtPageUp ();
    afx_msg void OnPageDown ();
    afx_msg void OnExtPageDown ();
    afx_msg void OnLineEnd ();
    afx_msg void OnExtLineEnd ();
    afx_msg void OnHome ();
    afx_msg void OnExtHome ();
    afx_msg void OnTextBegin ();
    afx_msg void OnExtTextBegin ();
    afx_msg void OnTextEnd ();
    afx_msg void OnExtTextEnd ();
    afx_msg void OnUpdateIndicatorCRLF (CCmdUI * pCmdUI);
    afx_msg void OnUpdateIndicatorPosition (CCmdUI * pCmdUI);
    afx_msg void OnToggleBookmark (UINT nCmdID);
    afx_msg void OnGoBookmark (UINT nCmdID);
    afx_msg void OnClearBookmarks ();

    afx_msg void OnToggleBookmark ();     // More bookmarks

    afx_msg void OnClearAllBookmarks ();
    afx_msg void OnNextBookmark ();
    afx_msg void OnPrevBookmark ();
    afx_msg void OnUpdateClearAllBookmarks (CCmdUI * pCmdUI);
    afx_msg void OnUpdateNextBookmark (CCmdUI * pCmdUI);
    afx_msg void OnUpdatePrevBookmark (CCmdUI * pCmdUI);

    afx_msg void ScrollUp ();
    afx_msg void ScrollDown ();
    afx_msg void ScrollLeft ();
    afx_msg void ScrollRight ();

    afx_msg void OnSourceType (UINT nId);
    afx_msg void OnUpdateSourceType (CCmdUI * pCmdUI);
    afx_msg void OnMatchBrace ();
    afx_msg void OnUpdateMatchBrace (CCmdUI * pCmdUI);
    afx_msg void OnEditGoTo ();
    afx_msg void OnUpdateToggleSourceHeader (CCmdUI * pCmdUI);
    afx_msg void OnToggleSourceHeader ();
    afx_msg void OnUpdateSelMargin (CCmdUI * pCmdUI);
    afx_msg void OnSelMargin ();
    afx_msg void OnUpdateWordWrap (CCmdUI * pCmdUI);
    afx_msg void OnWordWrap ();
    afx_msg void OnForceRedraw ();

	//BEGIN SW
	// incremental search
	afx_msg void OnEditFindIncrementalForward();
	afx_msg void OnEditFindIncrementalBackward();
	afx_msg void OnUpdateEditFindIncrementalForward(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditFindIncrementalBackward(CCmdUI* pCmdUI);
	//END SW

    DECLARE_MESSAGE_MAP ()
  };

#ifdef _DEBUG
#define ASSERT_VALIDTEXTPOS(pt)     AssertValidTextPos(pt);
#else
#define ASSERT_VALIDTEXTPOS(pt)
#endif

#if ! (defined(CE_FROM_DLL) || defined(CE_DLL_BUILD))
#include "ccrystaltextview.inl"
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CCRYSTALTEXTVIEW_H__AD7F2F41_6CB3_11D2_8C32_0080ADB86836__INCLUDED_)
