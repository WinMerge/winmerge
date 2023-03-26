/**
 * @file  GhostTextBuffer.h
 *
 * @brief Declaration of CGhostTextBuffer (subclasses CCrystalTextBuffer to handle ghost lines)
 */
#pragma once

#include <vector>
#include "ccrystaltextbuffer.h"


/////////////////////////////////////////////////////////////////////////////

/**
 * We use the current ccrystalEditor flags 
 *
 * This flag must be cleared and set in GhostTextBuffer.cpp 
 * and MergeDoc.cpp (Rescan) only.
 *
 * GetLineColors (in MergeEditView) reads it to choose the line color.
 */
enum GHOST_LINEFLAGS : lineflags_t
{
	LF_GHOST = 0x00400000UL, /**< Ghost line. */
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
#if 0
	bool InternalInsertGhostLine (CCrystalTextView * pSource, int nLine);
#endif
	bool InternalDeleteGhostLine (CCrystalTextView * pSource, int nLine, int nCount);
public :
	// Construction/destruction code
	CGhostTextBuffer ();

	/** 
	This should work in base code as ghost lines are real empty lines
	but maybe it doesn't (if there is an assert to check there is an EOL,
	or if it adds the default EOL)
	*/
	virtual void GetTextWithoutEmptys (int nStartLine, int nStartChar,
			int nEndLine, int nEndChar, String &text,
			CRLFSTYLE nCrlfStyle =CRLFSTYLE::AUTOMATIC,
			bool bExcludeInvisibleLines = true) const override;

	virtual bool IsIndentableLine(int nLine) const override { return (GetLineFlags(nLine) & LF_GHOST) == 0; }

	// Text modification functions
	virtual bool InsertText (CCrystalTextView * pSource, int nLine, int nPos,
		const tchar_t* pszText, size_t cchText, int &nEndLine, int &nEndChar,
		int nAction = CE_ACTION_UNKNOWN, bool bHistory =true) override;
	virtual bool DeleteText2 (CCrystalTextView * pSource, int nStartLine,
		int nStartPos, int nEndLine, int nEndPos,
		int nAction = CE_ACTION_UNKNOWN, bool bHistory =true) override;
#if 0
	bool InsertGhostLine (CCrystalTextView * pSource, int nLine);
#endif

	virtual void AddUndoRecord (bool bInsert, const CEPoint & ptStartPos, const CEPoint & ptEndPos,
	                            const tchar_t* pszText, size_t cchText, int nActionType = CE_ACTION_UNKNOWN, std::vector<uint32_t> *paSavedRevisionNumbers = nullptr) override;
	virtual UndoRecord GetUndoRecord(int nUndoPos) const override;
	virtual bool UndoInsert(CCrystalTextView * pSource, CEPoint & ptCursorPos,
							const CEPoint apparent_ptStartPos, CEPoint const apparent_ptEndPos, const UndoRecord & ur) override;

	virtual std::vector<uint32_t> *CopyRevisionNumbers(int nStartLine, int nEndLine) const override;
	virtual void RestoreRevisionNumbers(int nStartLine, std::vector<uint32_t> *paSavedRevisionNumbers) override;

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
	void CountEolAndLastLineLength(const CEPoint& ptStartPos, const tchar_t* pszText, size_t cchText, int& nLastLineLength, int& nEol);
	/** For debugging purpose */
	void checkFlagsFromReality() const;

protected:
	virtual void OnNotifyLineHasBeenEdited(int nLine);


protected:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCrystalTextBuffer)
	//}}AFX_VIRTUAL
};
