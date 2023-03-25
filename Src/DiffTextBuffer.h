/** 
 * @file  DiffTextBuffer.h
 *
 * @brief Declaration of CDiffTextBuffer class
 */
#pragma once

#include "GhostTextBuffer.h"
#include "FileTextEncoding.h"

class CMergeDoc;
class PackingInfo;

/**
 * @brief Specialized buffer to save file data
 */
class CDiffTextBuffer : public CGhostTextBuffer
{
	friend class CMergeDoc;

private :
	CMergeDoc * m_pOwnerDoc; /**< Merge document owning this buffer. */
	int m_nThisPane; /**< Left/Right side */
	String m_strTempPath; /**< Temporary files folder. */
	String m_strTempFileName; /**< Temporary file name. */
	std::vector<int> m_unpackerSubcodes; /**< Plugin information. */
	bool m_bMixedEOL; /**< EOL style of this buffer is mixed? */

	/** 
	 * @brief Unicode encoding from ucr::UNICODESET.
	 *
	 * @note m_unicoding and m_codepage are indications of how the buffer is
	 * supposed to be saved on disk. In memory, it is invariant, depending on
	 * build:
	 * - ANSI: in memory it is CP_ACP/CP_THREAD_ACP 8-bit characters
	 * - Unicode: in memory it is wchars
	 */
	FileTextEncoding m_encoding;

	bool FlagIsSet(int line, lineflags_t flag) const;

public :
	CDiffTextBuffer(CMergeDoc * pDoc, int pane);

	void SetTempPath(const String &path);
	String GetTempFileName() const { return m_strTempFileName; }
	virtual void AddUndoRecord (bool bInsert, const CEPoint & ptStartPos,
		const CEPoint & ptEndPos, const tchar_t* pszText, size_t cchText,
		int nActionType = CE_ACTION_UNKNOWN,
		std::vector<uint32_t> *paSavedRevisionNumbers = nullptr) override;
	bool curUndoGroup();
	void ReplaceFullLines(CDiffTextBuffer& dbuf, CDiffTextBuffer& sbuf, CCrystalTextView * pSource, int nLineBegin, int nLineEnd, int nAction =CE_ACTION_UNKNOWN);

	int LoadFromFile(const tchar_t* pszFileName, PackingInfo& infoUnpacker,
		const tchar_t* filteredFilenames, bool & readOnly, CRLFSTYLE nCrlfStyle,
		const FileTextEncoding & encoding, String &sError);
	int SaveToFile (const String& pszFileName, bool bTempFile, String & sError,
		PackingInfo& infoUnpacker, CRLFSTYLE nCrlfStyle = CRLFSTYLE::AUTOMATIC,
		bool bClearModifiedFlag = true, int nStartLine = 0, int nLines = -1);
	ucr::UNICODESET getUnicoding() const { return m_encoding.m_unicoding; }
	void setUnicoding(ucr::UNICODESET value) { m_encoding.m_unicoding = value; }
	int getCodepage() const { return m_encoding.m_codepage; }
	void setCodepage(int value) { m_encoding.SetCodepage(value); }
	bool getHasBom() const { return m_encoding.m_bom; }
	void setHasBom(bool value) { m_encoding.m_bom = value; }
	const FileTextEncoding & getEncoding() const { return m_encoding; }
	void setEncoding(const FileTextEncoding &encoding) { m_encoding = encoding; }
	bool IsMixedEOL() const { return m_bMixedEOL; }
	void SetMixedEOL(bool bMixed) { m_bMixedEOL = bMixed; }

	// If line has text (excluding eol), set strLine to text (excluding eol)
	bool GetLine(int nLineIndex, String &strLine) const;

	virtual void SetModified (bool bModified = true) override;
	void prepareForRescan();
	virtual void OnNotifyLineHasBeenEdited(int nLine) override;
	bool IsInitialized() const;
	virtual bool DeleteText2 (CCrystalTextView * pSource, int nStartLine,
		int nStartPos, int nEndLine, int nEndPos,
		int nAction = CE_ACTION_UNKNOWN, bool bHistory = true) override;
};

/**
 * @brief Set the folder for temp files.
 * @param [in] path Temp files folder.
 */
inline void CDiffTextBuffer::SetTempPath(const String &path)
{
	m_strTempPath = path;
}

/**
 * @brief Is the buffer initialized?
 * @return true if the buffer is initialized, false otherwise.
 */
inline bool CDiffTextBuffer::IsInitialized() const
{
	return !!m_bInit;
}

