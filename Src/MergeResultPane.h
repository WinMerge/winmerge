/**
 * @file  MergeResultPane.h
 *
 * @brief Declarations for the kdiff3-style merge result pane support:
 *        result segment model and the specialized result text buffer.
 */
#pragma once

#include "DiffTextBuffer.h"
#include <vector>

class CMergeDoc;

/**
 * @brief Resolution state of one result segment.
 */
enum class ResultSegmentState
{
	Common,   /**< Text outside any difference (taken from middle/base pane) */
	Auto,     /**< Difference resolved automatically (non-conflicting change) */
	Chosen,   /**< Difference resolved by an explicit Choose Left/Middle/Right */
	Conflict, /**< Unresolved 3-way conflict (placeholder line in result) */
	Edited,   /**< Segment has been edited by hand in the result pane */
};

/**
 * @brief One contiguous run of lines in the merge result buffer.
 *
 * The result buffer is fully covered by segments, ordered by start line.
 * Segments with diffIdx == -1 are common text between differences; other
 * segments correspond 1:1 to entries in the document's diff list.
 */
struct MergeResultSegment
{
	int diffIdx;      /**< Index in diff list, or -1 for common text */
	ResultSegmentState state;
	/**
	 * Source panes (0/1/2) of the content for Auto/Chosen, in order.
	 * Like KDiff3, several sources may be selected for one difference
	 * (their blocks are concatenated); empty for unresolved conflicts.
	 */
	std::vector<int> srcPanes;
	int nStartLine;   /**< First line of segment in result buffer */
	int nLines;       /**< Number of lines in segment (can be 0) */
};

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

private:
	CMergeDoc* m_pResultOwnerDoc; /**< Owning document (base class member is private) */
	int m_nInternalOp; /**< >0 while the document itself modifies the buffer */
};
