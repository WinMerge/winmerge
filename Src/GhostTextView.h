/**
 * @file  GhostTextView.h
 *
 * @brief Declaration of CGhostTextView (subclasses CCrystalEditViewEx to handle ghost lines)
 */
#pragma once

#include "crystalEditViewex.h"

////////////////////////////////////////////////////////////////////////////
// Forward class declarations

class CGhostTextBuffer;


////////////////////////////////////////////////////////////////////////////
// CCrystalTextView class declaration

/** 
This class hooks CCrystalEditViewEx to work with ghost lines. 

Use a CGhostTextBuffer buffer and virtualize some functions 
to handle apparent/real lines differences. 

We don't need to hook the drawing. Drawing ghost lines with the function used for
real lines is correct, as they are empty and have no EOL. 
WinMerge also paints the ghost lines in a custom color through virtualizing 
GetLineColors (in MergeEditView).
*/
class EDITPADC_CLASS CGhostTextView : public CCrystalEditViewEx
{
	DECLARE_DYNCREATE (CGhostTextView)
protected:
	CGhostTextView();           // protected constructor used by dynamic creation

private:
	CGhostTextBuffer * m_pGhostTextBuffer;

	/** 
	 * @brief Real point structure to preserve a position during Rescan 
	 * 
	 * @note The preserved positions are also updated in CCrystalTextView::UpdateView
	 * or in CCrystalEditView::UpdateView
	 * except for m_ptLastChange, which is set just after the call to UpdateView
	 */
	struct SCursorPushed
	{
		/// char pos
		int x;
		/// real line number of itself (is the line is real)/the first real line after this line (else)
		int y;
		/// 0 (is the line is real)/ distance to the first real line after this one 
		int nToFirstReal;

		SCursorPushed() : x(0), y(0), nToFirstReal(0) { }
	};
	
	/** 
	 * @brief Save a position to be able to restore it after Rescan.
	 * The saved position is based only on real lines
	 *
	 * @note We can only push/pop valid positions
	 * For positions which are sometimes invalid, use a flag
	 */
	void pushPosition(SCursorPushed & Sdest, CPoint pt);
	/** 
	 * @brief Restore cursors after Rescan.
	 *
	 * @note : also scroll to the old top line
	 */
	void popPosition(SCursorPushed Ssrc, CPoint & pt);

	/// basic cursor
	SCursorPushed m_ptCursorPosPushed;
	/// selection extremities
	SCursorPushed m_ptSelStartPushed, m_ptSelEndPushed;
	/// anchor point for selection (during shift is pressed)
	SCursorPushed m_ptAnchorPushed;
	/// during dragging, extremities of dragged text : if (m_bDraggingText == true)
	SCursorPushed m_ptDraggedTextBeginPushed, m_ptDraggedTextEndPushed;
	/// during drag/drop, droping position : if (m_bDropPosVisible == true)
	SCursorPushed m_ptSavedCaretPosPushed;
	/// memorize selected text during OnEditReplace : if (m_bSelectionPushed == true)
	SCursorPushed m_ptSavedSelStartPushed, m_ptSavedSelEndPushed;
	/// memorize top line positions
	int m_nTopSubLinePushed;
	/// memorize X offset positions
	int m_nOffsetCharPushed;
	/** last change position, in the buffer ; used in insertText
	 * initialized with (-1,-1), so don't assert for this invalid value
	 */
	SCursorPushed m_ptLastChangePushed;
	// Other CPoint used but not preserved :
	//   m_ptDrawSelStart, m_ptDrawSelEnd : built from m_ptSelStart and m_ptSelEnd
	//   m_ptDropPos : only used inside one function which does not change the buffer

public:
	virtual void ReAttachToBuffer (CCrystalTextBuffer * pBuf = nullptr) override;
	virtual void AttachToBuffer (CCrystalTextBuffer * pBuf = nullptr) override;
	virtual void DetachFromBuffer () override;

	/** real cursor function to preserve cursor during Rescan */
	void PopCursors ();
	/** real cursor function to preserve cursor during Rescan */
	void PushCursors ();

	void GetTextWithoutEmptys (int nStartLine, int nStartChar,
			int nEndLine, int nEndChar, CString &text,
			CRLFSTYLE nCrlfStyle = CRLFSTYLE::AUTOMATIC,
			bool bExcludeInvisibleLines = true);
	void GetTextWithoutEmptysInColumnSelection (CString & text,
			bool bExcludeInvisibleLines = true);
	/** 
	 * @brief Override this drag-n-drop function to call GetTextWithoutEmptys
	 */
	virtual HGLOBAL PrepareDragData () override;

	int ComputeApparentLine (int nRealLine) const;
	int ComputeRealLine (int nApparentLine) const;
	virtual void DrawMargin (const CRect & rect, int nLineIndex, int nLineNumber) override;

};
