#ifndef __GHOSTTEXTVIEW_H__
#define __GHOSTTEXTVIEW_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

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
		int x;
		// real line number of itself (is the line is real)/the first real line after this line (else)
		int y;
		// 0 (is the line is real)/ distance to the first real line after this one 
		int nToFirstReal;
	};
	
	/** 
	 * @brief Save a position to be able to restore it after Rescan.
	 * The saved position is based only on real lines
	 *
	 * @note We can only push/pop valid positions
	 * For positions which are sometimes invalid, use a flag
	 */
	void pushPosition(SCursorPushed & Sdest, CPoint pt);
	void popPosition(SCursorPushed Ssrc, CPoint & pt);

	/// basic cursor
	SCursorPushed m_ptCursorPosPushed;
	/// selection extremities
	SCursorPushed m_ptSelStartPushed, m_ptSelEndPushed;
	/// anchor point for selection (during shift is pressed)
	SCursorPushed m_ptAnchorPushed;
	/// during dragging, extremities of dragged text : if (m_bDraggingText == TRUE)
	SCursorPushed m_ptDraggedTextBeginPushed, m_ptDraggedTextEndPushed;
	/// during drag/drop, droping position : if (m_bDropPosVisible == TRUE)
	SCursorPushed m_ptSavedCaretPosPushed;
	/// memorize selected text during OnEditReplace : if (m_bSelectionPushed == TRUE)
	SCursorPushed m_ptSavedSelStartPushed, m_ptSavedSelEndPushed;
	/** last change position, in the buffer ; used in insertText
	 * initialized with (-1,-1), so don't assert for this invalid value
	 */
	SCursorPushed m_ptLastChangePushed;
	// Other CPoint used but not preserved :
	//   m_ptDrawSelStart, m_ptDrawSelEnd : built from m_ptSelStart and m_ptSelEnd
	//   m_ptDropPos : only used inside one function which does not change the buffer

public:
	virtual void AttachToBuffer (CCrystalTextBuffer * pBuf = NULL);
	virtual void DetachFromBuffer ();

	/** real cursor function to preserve cursor during Rescan */
	void PopCursors ();
	/** real cursor function to preserve cursor during Rescan */
	void PushCursors ();

	virtual void GetTextWithoutEmptys (int nStartLine, int nStartChar, int nEndLine, int nEndChar, CString &text, int nCrlfStyle =CRLF_STYLE_AUTOMATIC );
	/** 
	 * @brief Override this drag-n-drop function to call GetTextWithoutEmptys
	 */
	virtual HGLOBAL PrepareDragData ();

	int ComputeApparentLine (int nRealLine) const;
	int ComputeRealLine (int nApparentLine) const;

};


/////////////////////////////////////////////////////////////////////////////


#endif //__GHOSTTEXTVIEW_H__
