/**
 * @file  MergeResultTextBuffer.h
 *
 * @brief Declaration of CMergeResultTextBuffer class (kdiff3-style merge result buffer)
 */

#pragma once

#include "DiffTextBuffer.h"
#include <vector>

class CMergeDoc;
class CCrystalTextView;

/**
 * @brief Text buffer for the merge result pane.
 *
 * A CDiffTextBuffer that is not one of the compared buffers: it does not
 * take part in diffing/rescan and keeps the document's result segment
 * table up to date when it is edited.
 */
class CMergeResultTextBuffer : public CDiffTextBuffer
{
public:
	explicit CMergeResultTextBuffer(CMergeDoc* pDoc);

	/** RAII guard for internal (programmatic) buffer operations */
	struct InternalOpGuard
	{
		explicit InternalOpGuard(CMergeResultTextBuffer& buf) : m_buf(buf) { ++m_buf.m_nInternalOp; }
		~InternalOpGuard() { --m_buf.m_nInternalOp; }
		CMergeResultTextBuffer& m_buf;
	};
	bool IsInternalOp() const { return m_nInternalOp > 0; }

	virtual void SetModified(bool bModified = true) override;
	virtual void AddUndoRecord(bool bInsert, const CEPoint & ptStartPos,
		const CEPoint & ptEndPos, const tchar_t* pszText, size_t cchText,
		int nActionType = CE_ACTION_UNKNOWN,
		std::vector<uint32_t> *paSavedRevisionNumbers = nullptr) override;
	virtual void OnNotifyLineHasBeenEdited(int nLine) override;
	virtual bool InsertText(CCrystalTextView * pSource, int nLine, int nPos,
		const tchar_t* pszText, size_t cchText, int &nEndLine, int &nEndChar,
		int nAction = CE_ACTION_UNKNOWN, bool bHistory = true) override;
	virtual bool DeleteText2(CCrystalTextView * pSource, int nStartLine,
		int nStartPos, int nEndLine, int nEndPos,
		int nAction = CE_ACTION_UNKNOWN, bool bHistory = true) override;
	virtual bool Undo(CCrystalTextView * pSource, CEPoint & ptCursorPos) override;
	virtual bool Redo(CCrystalTextView * pSource, CEPoint & ptCursorPos) override;

	/**
	 * @brief Treat the current content as the unchanged baseline, so the
	 * margin change markers only show lines the user modifies afterwards.
	 */
	void AdoptCurrentRevision() { m_dwRevisionNumberOnSave = m_dwCurrentRevisionNumber; }
	/** Buffer-wide revision counter (lines edited later get higher numbers) */
	uint32_t GetCurrentRevisionNumber() const { return m_dwCurrentRevisionNumber; }
	/**
	 * @brief Drop the undo history (both directions). Needed after a
	 * programmatic, history-less buffer change (conflict rendering toggle,
	 * link severing): the recorded positions no longer match the text.
	 */
	void ClearUndoBuffer()
	{
		m_aUndoBuf.clear();
		m_nUndoPosition = 0;
		m_nSyncPosition = 0;
	}

private:
	CMergeDoc* m_pResultOwnerDoc; /**< Owning document (base class member is private) */
	int m_nInternalOp; /**< >0 while the document itself modifies the buffer */
};
