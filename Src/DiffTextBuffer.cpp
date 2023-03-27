/** 
 * @file  DiffTextBuffer.cpp
 *
 * @brief Implementation file for CDiffTextBuffer
 *
 */

#include "StdAfx.h"
#include "DiffTextBuffer.h"
#include "ccrystaltextview.h"
#include "UniFile.h"
#include "FileLoadResult.h"
#include "locality.h"
#include "paths.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "Environment.h"
#include "MergeLineFlags.h"
#include "MergeDoc.h"
#include "FileTransform.h"
#include "FileTextEncoding.h"
#include "codepage_detect.h"
#include "TFile.h"
#include <Poco/Exception.h>

using Poco::Exception;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static bool IsTextFileStylePure(const UniMemFile::txtstats & stats);
static CRLFSTYLE GetTextFileStyle(const UniMemFile::txtstats & stats);

/**
 * @brief Check if file has only one EOL type.
 * @param [in] stats File's text stats.
 * @return true if only one EOL type is found, false otherwise.
 */
static bool IsTextFileStylePure(const UniMemFile::txtstats & stats)
{
	int nType = 0;
	if (stats.ncrlfs > 0)
		nType++;
	if ( stats.ncrs > 0)
		nType++;
	if (stats.nlfs > 0)
		nType++;
	return (nType <= 1);
}

/**
 * @brief Get file's EOL type.
 * @param [in] stats File's text stats.
 * @return EOL type.
 */
static CRLFSTYLE GetTextFileStyle(const UniMemFile::txtstats & stats)
{
	// Check if file has more than one EOL type.
	if (!IsTextFileStylePure(stats))
			return CRLFSTYLE::MIXED;
	else if (stats.ncrlfs >= stats.nlfs)
	{
		if (stats.ncrlfs >= stats.ncrs)
			return CRLFSTYLE::DOS;
		else
			return CRLFSTYLE::MAC;
	}
	else
	{
		if (stats.nlfs >= stats.ncrs)
			return CRLFSTYLE::UNIX;
		else
			return CRLFSTYLE::MAC;
	}
}

/**
 * @brief Constructor.
 * @param [in] pDoc Owning CMergeDoc.
 * @param [in] pane Pane number this buffer is associated with.
 */
CDiffTextBuffer::CDiffTextBuffer(CMergeDoc * pDoc, int pane)
: m_pOwnerDoc(pDoc)
, m_nThisPane(pane)
, m_bMixedEOL(false)
{
}

/**
 * @brief Get a line from the buffer.
 * @param [in] nLineIndex Index of the line to get.
 * @param [out] strLine Returns line text in the index.
 */
bool CDiffTextBuffer::GetLine(int nLineIndex, String &strLine) const
{
	int nLineLength = CCrystalTextBuffer::GetLineLength(nLineIndex);
	if (nLineLength < 0)
		return false;
	else if (nLineLength == 0)
		strLine.clear();
	else
		strLine.assign(CCrystalTextBuffer::GetLineChars(nLineIndex), nLineLength);
	return true;
}

/**
 * @brief Set the buffer modified status.
 * @param [in] bModified New modified status, true if buffer has been
 *   modified since last saving.
 */
void CDiffTextBuffer::			/* virtual override */
SetModified(bool bModified /*= true*/)	
{
	CCrystalTextBuffer::SetModified (bModified);
	m_pOwnerDoc->SetModifiedFlag (bModified);
}

void CDiffTextBuffer::			/* virtual override */
AddUndoRecord(bool bInsert, const CEPoint & ptStartPos,
		const CEPoint & ptEndPos, const tchar_t* pszText, size_t cchText,
		int nActionType /*= CE_ACTION_UNKNOWN*/,
		std::vector<uint32_t> *paSavedRevisionNumbers /*= nullptr*/)
{
	CGhostTextBuffer::AddUndoRecord(bInsert, ptStartPos, ptEndPos, pszText,
		cchText, nActionType, paSavedRevisionNumbers);
	if (m_aUndoBuf[m_nUndoPosition - 1].m_dwFlags & UNDO_BEGINGROUP)
	{
		m_pOwnerDoc->undoTgt.erase(m_pOwnerDoc->curUndo, m_pOwnerDoc->undoTgt.end());
		m_pOwnerDoc->undoTgt.push_back(m_nThisPane);
		m_pOwnerDoc->curUndo = m_pOwnerDoc->undoTgt.end();
	}
}

/**
 * @brief Checks if a flag is set for line.
 * @param [in] line Index (0-based) for line.
 * @param [in] flag Flag to check.
 * @return true if flag is set, false otherwise.
 */
bool CDiffTextBuffer::FlagIsSet(int line, lineflags_t flag) const
{
	return ((m_aLines[line].m_dwFlags & flag) == flag);
}

/**
Remove blank lines and clear winmerge flags
(2003-06-21, Perry: I don't understand why this is necessary, but if this isn't 
done, more and more gray lines appear in the file)
(2003-07-31, Laoran I don't understand either why it is necessary, but it works
fine, so let's go on with it)
*/
void CDiffTextBuffer::prepareForRescan()
{
	RemoveAllGhostLines();
	for (int ct = GetLineCount() - 1; ct >= 0; --ct)
	{
		SetLineFlag(ct, 
			LF_INVISIBLE | LF_DIFF | LF_TRIVIAL | LF_MOVED | LF_SNP,
			false, false, false);
	}
}

/** 
 * @brief Called when line has been edited.
 * After editing a line, we don't know if there is a diff or not.
 * So we clear the LF_DIFF flag (and it is more easy to read during editing).
 * Rescan will set the proper color.
 * @param [in] nLine Line that has been edited.
 */

void CDiffTextBuffer::			/* virtual override */
OnNotifyLineHasBeenEdited(int nLine)
{
	SetLineFlag(nLine, LF_DIFF, false, false, false);
	SetLineFlag(nLine, LF_TRIVIAL, false, false, false);
	SetLineFlag(nLine, LF_MOVED, false, false, false);
	SetLineFlag(nLine, LF_SNP, false, false, false);
	CGhostTextBuffer::OnNotifyLineHasBeenEdited(nLine);
}

/**
 * @brief Load file from disk into buffer
 *
 * @param [in] pszFileNameInit File to load
 * @param [in] infoUnpacker Unpacker plugin
 * @param [in] sToFindUnpacker String for finding unpacker plugin
 * @param [out] readOnly Loading was lossy so file should be read-only
 * @param [in] nCrlfStyle EOL style used
 * @param [in] encoding Encoding used
 * @param [out] sError Error message returned
 * @return FRESULT_OK when loading succeed or (list in files.h):
 * - FRESULT_OK_IMPURE : load OK, but the EOL are of different types
 * - FRESULT_ERROR_UNPACK : plugin failed to unpack
 * - FRESULT_ERROR : loading failed, sError contains error message
 * - FRESULT_BINARY : file is binary file
 * @note If this method fails, it calls InitNew so the CDiffTextBuffer is in a valid state
 */
int CDiffTextBuffer::LoadFromFile(const tchar_t* pszFileNameInit,
		PackingInfo& infoUnpacker, const tchar_t* sToFindUnpacker, bool & readOnly,
		CRLFSTYLE nCrlfStyle, const FileTextEncoding & encoding, String &sError)
{
	ASSERT(!m_bInit);
	ASSERT(m_aLines.size() == 0);

	// Unpacking the file here, save the result in a temporary file
	m_strTempFileName = pszFileNameInit;
	if (!infoUnpacker.Unpacking(&m_unpackerSubcodes, m_strTempFileName, sToFindUnpacker, { m_strTempFileName }))
	{
		InitNew(); // leave crystal editor in valid, empty state
		return FileLoadResult::FRESULT_ERROR_UNPACK;
	}

	// we will load the transformed file
	const tchar_t* pszFileName = m_strTempFileName.c_str();

	String sExt;
	FileLoadResult::flags_t nRetVal = FileLoadResult::FRESULT_OK;

	// Set encoding based on extension, if we know one
	paths::SplitFilename(pszFileName, nullptr, nullptr, &sExt);
	CrystalLineParser::TextDefinition *def = 
		CrystalLineParser::GetTextType(sExt.c_str());
	if (def && def->encoding != -1)
		m_nSourceEncoding = def->encoding;
	
	UniFile *pufile = new UniMemFile;

	// Now we only use the UniFile interface
	// which is something we could implement for HTTP and/or FTP files

	if (!pufile->OpenReadOnly(pszFileName))
	{
		nRetVal = FileLoadResult::FRESULT_ERROR;
		UniFile::UniError uniErr = pufile->GetLastUniError();
		if (uniErr.HasError())
		{
			sError = uniErr.GetError();
		}
		InitNew(); // leave crystal editor in valid, empty state
	}
	else
	{
		if (!m_unpackerSubcodes.empty())
		{
			// re-detect codepage
			int iGuessEncodingType = GetOptionsMgr()->GetInt(OPT_CP_DETECT);
			FileTextEncoding encoding2 = codepage_detect::Guess(pszFileName, iGuessEncodingType);
			pufile->SetUnicoding(encoding2.m_unicoding);
			pufile->SetCodepage(encoding2.m_codepage);
			pufile->SetBom(encoding2.m_bom);
			if (encoding2.m_bom)
				pufile->ReadBom();
		}
		else
		{
			// If the file is not unicode file, use the codepage we were given to
			// interpret the 8-bit characters. If the file is unicode file,
			// determine its type (IsUnicode() does that).
			if (encoding.m_unicoding == ucr::NONE  || !pufile->IsUnicode())
				pufile->SetCodepage(encoding.m_codepage);
		}
		unsigned lineno = 0;
		String eol, preveol;
		String sline;
		bool done = false;

		// Manually grow line array exponentially
		size_t arraysize = 500;
		m_aLines.resize(arraysize);
		
		// preveol must be initialized for empty files
		preveol = _T("\n");
		
		do {
			bool lossy = false;
			done = !pufile->ReadString(sline, eol, &lossy);

			// if last line had no eol, we can quit
			if (done && preveol.empty())
				break;
			// but if last line had eol, we add an extra (empty) line to buffer

			// Grow line array
			if (lineno == arraysize)
			{
				// For smaller sizes use exponential growth, but for larger
				// sizes grow by constant ratio. Unlimited exponential growth
				// easily runs out of memory.
				if (arraysize < 100 * 1024)
					arraysize *= 2;
				else
					arraysize += 100 * 1024;
				m_aLines.resize(arraysize);
			}

			sline += eol; // TODO: opportunity for optimization, as CString append is terrible
			if (lossy)
			{
				// TODO: Should record lossy status of line
			}
			AppendLine(lineno, sline.c_str(), static_cast<int>(sline.length()));
			++lineno;
			preveol = eol;

		} while (!done);

		// fix array size (due to our manual exponential growth
		m_aLines.resize(lineno);
	
		
		//Try to determine current CRLF mode (most frequent)
		if (nCrlfStyle == CRLFSTYLE::AUTOMATIC)
		{
			nCrlfStyle = GetTextFileStyle(pufile->GetTxtStats());
		}
		ASSERT (nCrlfStyle != CRLFSTYLE::AUTOMATIC);
		SetCRLFMode(nCrlfStyle);
		
		//  At least one empty line must present
		// (view does not work for empty buffers)
		ASSERT(m_aLines.size() > 0);
		
		m_bInit = true;
		m_bModified = false;
		m_bUndoGroup = m_bUndoBeginGroup = false;
		m_nSyncPosition = m_nUndoPosition = 0;
		ASSERT(m_aUndoBuf.size() == 0);
		m_ptLastChange.x = m_ptLastChange.y = -1;
		
		FinishLoading();
		// flags don't need initialization because 0 is the default value

		// Set the return value : OK + info if the file is impure
		// A pure file is a file where EOL are consistent (all DOS, or all UNIX, or all MAC)
		// An impure file is a file with several EOL types
		// WinMerge may display impure files, but the default option is to unify the EOL
		// We return this info to the caller, so it may display a confirmation box
		if (IsTextFileStylePure(pufile->GetTxtStats()))
			nRetVal = FileLoadResult::FRESULT_OK;
		else
			nRetVal = FileLoadResult::FRESULT_OK_IMPURE;

		// stash original encoding away
		m_encoding.m_unicoding = pufile->GetUnicoding();
		m_encoding.m_bom = pufile->HasBom();
		m_encoding.m_codepage = pufile->GetCodepage();

		if (pufile->GetTxtStats().nlosses)
		{
			FileLoadResult::AddModifier(nRetVal, FileLoadResult::FRESULT_LOSSY);
			readOnly = true;
		}
	}
	
	// close the file now to free the handle
	pufile->Close();
	delete pufile;

	// delete the file that unpacking may have created
	if (tc::tcscmp(pszFileNameInit, pszFileName) != 0)
	{
		try
		{
			TFile(pszFileName).remove();
		}
		catch (Exception& e)
		{
			LogErrorStringUTF8(e.displayText());
		}
	}
	return nRetVal;
}

/**
 * @brief Saves file from buffer to disk
 *
 * @param bTempFile : false if we are saving user files and
 * true if we are saving working-temp-files for diff-engine
 *
 * @return SAVE_DONE or an error code (list in MergeDoc.h)
 */
int CDiffTextBuffer::SaveToFile (const String& pszFileName,
		bool bTempFile, String & sError, PackingInfo& infoUnpacker /*= nullptr*/,
		CRLFSTYLE nCrlfStyle /*= CRLFSTYLE::AUTOMATIC*/,
		bool bClearModifiedFlag /*= true*/,
		int nStartLine /*= 0*/, int nLines /*= -1*/)
{
	ASSERT (nCrlfStyle == CRLFSTYLE::AUTOMATIC || nCrlfStyle == CRLFSTYLE::DOS ||
		nCrlfStyle == CRLFSTYLE::UNIX || nCrlfStyle == CRLFSTYLE::MAC);
	ASSERT (m_bInit);

	if (nLines == -1)
		nLines = static_cast<int>(m_aLines.size() - nStartLine);

	if (pszFileName.empty())
		return SAVE_FAILED;	// No filename, cannot save...

	if (nCrlfStyle == CRLFSTYLE::AUTOMATIC &&
		!GetOptionsMgr()->GetBool(OPT_ALLOW_MIXED_EOL))
	{
			// get the default nCrlfStyle of the CDiffTextBuffer
		nCrlfStyle = GetCRLFMode();
		ASSERT(nCrlfStyle != CRLFSTYLE::AUTOMATIC);
	}

	bool bOpenSuccess = true;
	bool bSaveSuccess = false;

	UniStdioFile file;

	String sIntermediateFilename; // used when !bTempFile

	if (bTempFile)
	{
		file.SetUnicoding(ucr::UTF8);
		file.SetBom(GetOptionsMgr()->GetInt(OPT_CMP_DIFF_ALGORITHM) == 0);
		bOpenSuccess = !!file.OpenCreate(pszFileName);
	}
	else
	{
		file.SetUnicoding(m_encoding.m_unicoding);
		file.SetBom(m_encoding.m_bom);
		file.SetCodepage(m_encoding.m_codepage);
		sIntermediateFilename = env::GetTemporaryFileName(m_strTempPath,
			_T("MRG_"), nullptr);
		if (sIntermediateFilename.empty())
			return SAVE_FAILED;  //Nothing to do if even tempfile name fails
		bOpenSuccess = !!file.OpenCreate(sIntermediateFilename);
	}

	if (!bOpenSuccess)
	{	
		UniFile::UniError uniErr = file.GetLastUniError();
		if (uniErr.HasError())
		{
			sError = uniErr.GetError();
			if (bTempFile)
				LogErrorString(strutils::format(_T("Opening file %s failed: %s"),
					pszFileName, sError));
			else
				LogErrorString(strutils::format(_T("Opening file %s failed: %s"),
					sIntermediateFilename, sError));
		}
		return SAVE_FAILED;
	}

	const size_t StdioBufSize = (std::min)(512 * 1024, BUFSIZ + nLines * 32);
	file.SetVBuf(_IOFBF, StdioBufSize);
	file.WriteBom();

	// line loop : get each real line and write it in the file
	String sLine;
	String sEol = GetStringEol(nCrlfStyle);
	int lastRealLine = ApparentLastRealLine();
	for (int line = nStartLine; line < nStartLine + nLines; ++line)
	{
		if (GetLineFlags(line) & LF_GHOST)
			continue;

		// get the characters of the line (excluding EOL)
		if (GetLineLength(line) > 0)
		{
			int nLineLength = GetLineLength(line);
			sLine.resize(0);
			sLine.reserve(nLineLength + 4);
			sLine.append(GetLineChars(line), nLineLength);
		}
		else
			sLine.clear();

		if (bTempFile && m_bTableEditing && m_bAllowNewlinesInQuotes)
		{
			strutils::replace(sLine, _T("\x1b"), _T("\x1b\x1b"));
			strutils::replace(sLine, _T("\r"), _T("\x1br"));
			strutils::replace(sLine, _T("\n"), _T("\x1bn"));
		}

		// last real line ?
		if (line == lastRealLine || lastRealLine == -1 )
		{
			// If original last line had no EOL, then we are done
			if( !m_aLines[line].HasEol() )
			{
				file.WriteString(sLine);
				break;
			}
			// Otherwise, add the appropriate EOL to the last line ...
		}

		// normal line : append an EOL
		if (nCrlfStyle == CRLFSTYLE::AUTOMATIC || nCrlfStyle == CRLFSTYLE::MIXED)
		{
			// either the EOL of the line (when preserve original EOL chars is on)
			sLine += GetLineEol(line);
		}
		else
		{
			// or the default EOL for this file
			sLine += sEol;
		}

		// write this line to the file (codeset or unicode conversions are done there
		file.WriteString(sLine);

		if (line == lastRealLine || lastRealLine == -1)
		{
			// Last line, so now done
			break;
		}
	}
	file.Close();

	if (!bTempFile)
	{
		// If we are saving user files
		// we need an unpacker/packer, at least a "do nothing" one
		// repack the file here, overwrite the temporary file we did save in
		bSaveSuccess = infoUnpacker.Packing(sIntermediateFilename, pszFileName, m_unpackerSubcodes, { pszFileName });
		if (!bSaveSuccess)
			sError = GetSysError();
		try
		{
			TFile(sIntermediateFilename).remove();
		}
		catch (Exception& e)
		{
			LogErrorStringUTF8(e.displayText());
		}
		if (!bSaveSuccess)
		{
			// returns now, don't overwrite the original file
			return m_unpackerSubcodes.empty() ? SAVE_FAILED : SAVE_PACK_FAILED;
		}

		if (bClearModifiedFlag)
		{
			SetModified(false);
			m_nSyncPosition = m_nUndoPosition;
		}

		// remember revision number on save
		m_dwRevisionNumberOnSave = m_dwCurrentRevisionNumber;

		// redraw line revision marks
		UpdateViews (nullptr, nullptr, UPDATE_FLAGSONLY);	
	}
	else
	{
		if (bClearModifiedFlag)
		{
			SetModified(false);
			m_nSyncPosition = m_nUndoPosition;
		}
		bSaveSuccess = true;
	}

	if (bSaveSuccess)
		return SAVE_DONE;
	else
		return SAVE_FAILED;
}

/// Replace line (removing any eol, and only including one if in strText)
void CDiffTextBuffer::ReplaceFullLines(CDiffTextBuffer& dbuf, CDiffTextBuffer& sbuf, CCrystalTextView * pSource, int nLineBegin, int nLineEnd, int nAction /*=CE_ACTION_UNKNOWN*/)
{
	String strText;
	if (nLineBegin != nLineEnd || sbuf.GetLineLength(nLineEnd) > 0)
		sbuf.GetTextWithoutEmptys(nLineBegin, 0, nLineEnd, sbuf.GetLineLength(nLineEnd), strText);
	strText += sbuf.GetLineEol(nLineEnd);

	if (nLineBegin != nLineEnd || dbuf.GetFullLineLength(nLineEnd) > 0)
	{
		int nLineEndSource = nLineEnd < dbuf.GetLineCount() ? nLineEnd : dbuf.GetLineCount();
		if (nLineEnd+1 < GetLineCount())
			dbuf.DeleteText(pSource, nLineBegin, 0, nLineEndSource + 1, 0, nAction);
		else
			dbuf.DeleteText(pSource, nLineBegin, 0, nLineEndSource, dbuf.GetLineLength(nLineEndSource), nAction); 
	}

	if (size_t cchText = strText.length())
	{
		int endl,endc;
		dbuf.InsertText(pSource, nLineBegin, 0, strText.c_str(), cchText, endl, endc, nAction);
	}
}

bool CDiffTextBuffer::curUndoGroup()
{
	return (m_aUndoBuf.size() != 0 && m_aUndoBuf[0].m_dwFlags&UNDO_BEGINGROUP);
}

bool CDiffTextBuffer::			/* virtual override */
DeleteText2(CCrystalTextView * pSource, int nStartLine, int nStartChar,
	int nEndLine, int nEndChar, int nAction /*= CE_ACTION_UNKNOWN*/, bool bHistory /*= true*/)
{
	for (auto syncpnt : m_pOwnerDoc->GetSyncPointList())
	{
		const int nLineSyncPoint = syncpnt[m_nThisPane];
		if (((nStartChar == 0 && nStartLine == nLineSyncPoint) || nStartLine < nLineSyncPoint) &&
			nLineSyncPoint < nEndLine)
			m_pOwnerDoc->DeleteSyncPoint(m_nThisPane, nLineSyncPoint, false);
	}
	return CGhostTextBuffer::DeleteText2(pSource, nStartLine, nStartChar, nEndLine, nEndChar, nAction, bHistory);
}
