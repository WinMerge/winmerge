/**
 * @file  GhostTextBuffer.h
 *
 * @brief Declaration of CGhostTextBuffer (subclasses CCrystalTextBuffer to handle ghost lines)
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef __GHOSTTEXTBUFFER_H__
#define __GHOSTTEXTBUFFER_H__

#include <vector>
#include "ccrystaltextbuffer.h"
#include "GhostUndoRecord.h"


/////////////////////////////////////////////////////////////////////////////

/**
 * We use the current ccrystalEditor flags 
 *
 * This flag must be cleared and set in GhostTextBuffer.cpp 
 * and MergeDoc.cpp (Rescan) only.
 *
 * GetLineColors (in MergeEditView) reads it to choose the line color.
 */
enum GHOST_LINEFLAGS
{
	LF_GHOST = 0x00400000L, /**< Ghost line. */
};

/////////////////////////////////////////////////////////////////////////////
// CCrystalTextBuffer command target

/**
 * @brief A class handling ghost lines.
 * Features offered with this class : 
 * <ul>
 *  <li> apparent/real line conversion 
 *  <li> insertText/deleteText working with ghost lines 
 *  <li> AddUndoRecord/Undo/Redo working with ghost lines 
 *  <li> insertGhostLine function 
 * </ul>
 */
class EDITPADC_CLASS CGhostTextBuffer : public CCrystalTextBuffer
{
public:
	DECLARE_DYNCREATE (CGhostTextBuffer)

protected :
#pragma pack(push, 1)
	//  Nested class declarations
	enum
	{
		UNDO_INSERT = 0x0001,
		UNDO_BEGINGROUP = 0x0100
	};



#pragma pack(pop)

protected:
	/** 
	We need another array with our richer structure.

	We share the positions with the CCrystalTextBuffer object. 
	We share m_bUndoGroup, its utility is to check we opened the UndoBeginGroup.
	We share m_nUndoBufSize which is the max buffer size.
	*/
	std::vector<GhostUndoRecord> m_aUndoBuf;
	/** 
	This one must be duplicated because the flag UNDO_BEGINGROUP needs to be set in both 
	CGhostTextBuffer::m_aUndoBuf and CCrystalTextBuffer::m_aUndoBuf CArrays 
	*/
	BOOL m_bUndoBeginGroup;

	// [JRT] Support For Descriptions On Undo/Redo Actions
	virtual void AddUndoRecord (BOOL bInsert, const CPoint & ptStartPos, const CPoint & ptEndPos,
                              LPCTSTR pszText, int cchText, int nRealLinesChanged, int nActionType = CE_ACTION_UNKNOWN, CDWordArray *paSavedRevisonNumbers = NULL);

private:
	/**
	 * @brief A struct mapping real lines and apparent (screen) lines.
	 * This struct maps lines between real lines and apparent (screen) lines.
	 * The mapping records for each text block an apparent line and matching
	 * real line.
	 */
	struct RealityBlock
	{
		int nStartReal; /**< Start line of real block. */
		int nStartApparent; /**< Start line of apparent block. */
		int nCount; /**< Lines in the block. */
	};
	std::vector<RealityBlock> m_RealityBlocks; /**< Mapping of real and apparent lines. */

	// Operations
private:
	BOOL InternalInsertGhostLine (CCrystalTextView * pSource, int nLine);
	BOOL InternalDeleteGhostLine (CCrystalTextView * pSource, int nLine, int nCount);
public :
	// Construction/destruction code
	CGhostTextBuffer ();
	virtual BOOL InitNew (CRLFSTYLE nCrlfStyle = CRLF_STYLE_DOS);

	/** 
	This should work in base code as ghost lines are real empty lines
	but maybe it doesn't (if there is an assert to check there is an EOL,
	or if it adds the default EOL)
	*/
	virtual void GetTextWithoutEmptys (int nStartLine, int nStartChar,
			int nEndLine, int nEndChar, CString &text,
			CRLFSTYLE nCrlfStyle =CRLF_STYLE_AUTOMATIC);


	// Text modification functions
	virtual BOOL InsertText (CCrystalTextView * pSource, int nLine, int nPos,
		LPCTSTR pszText, int cchText, int &nEndLine, int &nEndChar,
		int nAction = CE_ACTION_UNKNOWN, BOOL bHistory = TRUE);
	virtual BOOL DeleteText (CCrystalTextView * pSource, int nStartLine,
		int nStartPos, int nEndLine, int nEndPos,
		int nAction = CE_ACTION_UNKNOWN, BOOL bHistory = TRUE);
	BOOL InsertGhostLine (CCrystalTextView * pSource, int nLine);

	// Undo/Redo
	virtual BOOL Undo (CCrystalTextView * pSource, CPoint & ptCursorPos);
	virtual BOOL Redo (CCrystalTextView * pSource, CPoint & ptCursorPos);

	// Undo grouping
	virtual void BeginUndoGroup (BOOL bMergeWithPrevious = FALSE);
	virtual void FlushUndoGroup (CCrystalTextView * pSource);

public:
	//@{
	/**
	 * @name Real/apparent line number conversion functions.
	 * These functions convert line numbers between file line numbers
	 * (real line numbers) and screen line numbers (apparent line numbers).
	 *
	 * This mapping is needed to handle ghost lines (ones with no text or
	 * EOL chars) which WinMerge uses for left-only or right-only lines.
	*/
	int ApparentLastRealLine() const;
	int ComputeRealLine(int nApparentLine) const;
	int ComputeApparentLine(int nRealLine) const;
	/** richer position information   yApparent = apparent(yReal) - yGhost */
	int ComputeRealLineAndGhostAdjustment(int nApparentLine, int& decToReal) const;
	/** richer position information   yApparent = apparent(yReal) - yGhost */
	int ComputeApparentLine(int nRealLine, int decToReal) const;
	//@}

	/** for loading file */
	void FinishLoading();
	/** for saving file */ 
	void RemoveAllGhostLines();


private:
	void RecomputeRealityMapping();
	/** 
	Code to set EOL, if the status ghost/real of the line changes 

	We should call a CCrystalTextBuffer function to add the correct EOL
	(if CCrystalTextBuffer keeps the default EOL for the file)
	*/
	void RecomputeEOL(CCrystalTextView * pSource, int nStartLine, int nEndLine);
	/** For debugging purpose */
	void checkFlagsFromReality(BOOL bFlag) const;

protected:
	virtual void OnNotifyLineHasBeenEdited(int nLine);


protected:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCrystalTextBuffer)
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CCrystalTextBuffer)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP ()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.


#endif //__GHOSTTEXTBUFFER_H__
