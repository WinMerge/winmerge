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

	/** real cursor position to preserve cursor during Rescan */
	CPoint m_ptCursorLast;
	/** real cursor position to preserve cursor during Rescan */
	int    m_ptCursorLast_nGhost;

public:
	virtual void AttachToBuffer (CCrystalTextBuffer * pBuf = NULL);
	virtual void DetachFromBuffer ();

	/** real cursor function to preserve cursor during Rescan */
	void PopCursor ();
	/** real cursor function to preserve cursor during Rescan */
	void PushCursor ();

	int ComputeApparentLine (int nRealLine) const;
	int ComputeRealLine (int nApparentLine) const;

};


/////////////////////////////////////////////////////////////////////////////


#endif //__GHOSTTEXTVIEW_H__
