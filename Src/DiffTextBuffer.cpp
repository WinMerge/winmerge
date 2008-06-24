/** 
 * @file  DiffTextBuffer.cpp
 *
 * @brief Implementation file for CDiffTextBuffer
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"
#include "UniFile.h"
#include "files.h"
#include "cs2cs.h"
#include "locality.h"
#include "coretools.h"
#include "Merge.h"
#include "OptionsDef.h"
#include "Environment.h"
#include "MergeLineFlags.h"
#include "MergeDoc.h"
#include "FileTransform.h"
#include "FileTextEncoding.h"
#include "DiffTextBuffer.h"

static bool IsTextFileStylePure(const UniMemFile::txtstats & stats);
static CString GetLineByteTimeReport(UINT lines, __int64 bytes,
	const COleDateTime & start);
static void EscapeControlChars(CString &s);
static LPCTSTR GetEol(const CString &str);
static CRLFSTYLE GetTextFileStyle(const UniMemFile::txtstats & stats);

/**
 * @brief Check if file has only one EOL type.
 * @param [in] stats File's text stats.
 * @return true if only one EOL type is found, false otherwise.
 */
static bool IsTextFileStylePure(const UniMemFile::txtstats & stats)
{
	int nType = 0;
	nType += (stats.ncrlfs > 0);
	nType += (stats.ncrs > 0);
	nType += (stats.nlfs > 0);
	return (nType <= 1);
}

/**
 * @brief Return a string giving #lines and #bytes and how much time elapsed.
 * @param [in] lines Count of lines.
 * @param [in] bytes Count of bytes.
 * @param [in] start Time used.
 * @return Formatted string.
 */
static CString GetLineByteTimeReport(UINT lines, __int64 bytes,
	const COleDateTime & start)
{
	String sLines = locality::NumToLocaleStr((int)lines);
	String sBytes = locality::NumToLocaleStr(bytes);
	COleDateTimeSpan duration = COleDateTime::GetCurrentTime() - start;
	String sMinutes = locality::NumToLocaleStr((int)duration.GetTotalMinutes());
	CString str;
	str.Format(_T("%s lines (%s byte) saved in %sm%02ds")
		, sLines.c_str(), sBytes.c_str(), sMinutes.c_str()
		, duration.GetSeconds()
		);
	return str;
}

/**
 * @brief Escape control characters.
 * @param [in,out] s Line of text excluding eol chars.
 *
 * @note Escape sequences follow the pattern
 * (leadin character, high nibble, low nibble, leadout character).
 * The leadin character is '\x0F'. The leadout character is a backslash.
 */
static void EscapeControlChars(CString &s)
{
	// Compute buffer length required for escaping
	int n = s.GetLength();
	LPCTSTR q = s;
	int i = n;
	while (i)
	{
		TCHAR c = q[--i];
		// Is it a control character in the range 0..31 except TAB?
		if (!(c & ~_T('\x1F')) && c != _T('\t'))
		{
			n += 3; // Need 3 extra characters to escape
		}
	}
	// Reallocate accordingly
	i = s.GetLength();
	LPTSTR p = s.GetBufferSetLength(n);
	// Copy/translate characters starting at end of string
	while (i)
	{
		TCHAR c = p[--i];
		// Is it a control character in the range 0..31 except TAB?
		if (!(c & ~_T('\x1F')) && c != _T('\t'))
		{
			// Bitwise OR with 0x100 so _itot() will output 3 hex digits
			_itot(0x100 | c, p + n - 4, 16);
			// Replace terminating zero with leadout character
			p[n - 1] = _T('\\');
			// Prepare to replace 1st hex digit with leadin character
			c = _T('\x0F');
			n -= 3;
		}
		p[--n] = c;
	}
}

/**
 * @brief Get EOL of the string.
 * This function returns a pointer to the EOL chars in the given string.
 * Behavior is similar to CCrystalTextBuffer::GetLineEol().
 * @param [in] str String whose EOL chars are returned.
 * @return Pointer to string's EOL chars, or empty string if no EOL found.
 */
static LPCTSTR GetEol(const CString &str)
{
	if (str.GetLength()>1 && str[str.GetLength()-2]=='\r' && str[str.GetLength()-1]=='\n')
		return (LPCTSTR)str + str.GetLength()-2;
	if (str.GetLength()>0 && (str[str.GetLength()-1]=='\r' || str[str.GetLength()-1]=='\n'))
		return (LPCTSTR)str + str.GetLength()-1;
	return _T("");
}

/**
 * @brief Get file's EOL type.
 * @param [in] stats File's text stats.
 * @return EOL type.
 */
static CRLFSTYLE GetTextFileStyle(const UniMemFile::txtstats & stats)
{
	if (stats.ncrlfs >= stats.nlfs)
	{
		if (stats.ncrlfs >= stats.ncrs)
		{
			return CRLF_STYLE_DOS;
		}
		else
		{
			return CRLF_STYLE_MAC;
		}
	}
	else
	{
		if (stats.nlfs >= stats.ncrs)
		{
			return CRLF_STYLE_UNIX;
		}
		else
		{
			return CRLF_STYLE_MAC;
		}
	}
}

CDiffTextBuffer::CDiffTextBuffer(CMergeDoc * pDoc, int pane)
: m_pOwnerDoc(pDoc)
, m_nThisPane(pane)
, unpackerSubcode(0)
{
}

BOOL CDiffTextBuffer::GetLine(int nLineIndex, CString &strLine)
{ 
	int nLineLength = CCrystalTextBuffer::GetLineLength 
		( nLineIndex ); 
	
	if( nLineLength < 0 ) 
		return FALSE; 
	else if( nLineLength == 0 ) 
		strLine.Empty(); 
	else 
	{ 
		_tcsncpy ( strLine.GetBuffer( nLineLength + 1 ), 
			CCrystalTextBuffer::GetLineChars( nLineIndex ), 
			nLineLength ); 
		strLine.ReleaseBuffer( nLineLength ); 
	} 
	return TRUE; 
}

void CDiffTextBuffer::SetModified(BOOL bModified /*= TRUE*/)
{
	CCrystalTextBuffer::SetModified (bModified);
	m_pOwnerDoc->SetModifiedFlag (bModified);
}

BOOL CDiffTextBuffer::GetFullLine(int nLineIndex, CString &strLine)
{
	int cchText = GetFullLineLength(nLineIndex);
	if (cchText == 0)
		return FALSE;
	LPTSTR pchText = strLine.GetBufferSetLength(cchText);
	memcpy(pchText, GetLineChars(nLineIndex), cchText * sizeof(TCHAR));
	return TRUE;
}

void CDiffTextBuffer::AddUndoRecord(BOOL bInsert, const CPoint & ptStartPos, const CPoint & ptEndPos, LPCTSTR pszText, int cchText, int nLinesToValidate, int nActionType /*= CE_ACTION_UNKNOWN*/, CDWordArray *paSavedRevisonNumbers)
{
	CGhostTextBuffer::AddUndoRecord(bInsert, ptStartPos, ptEndPos, pszText, cchText, nLinesToValidate, nActionType, paSavedRevisonNumbers);
	if (m_aUndoBuf[m_nUndoPosition - 1].m_dwFlags & UNDO_BEGINGROUP)
	{
		m_pOwnerDoc->undoTgt.erase(m_pOwnerDoc->curUndo, m_pOwnerDoc->undoTgt.end());
		m_pOwnerDoc->undoTgt.push_back(m_pOwnerDoc->GetView(m_nThisPane));
		m_pOwnerDoc->curUndo = m_pOwnerDoc->undoTgt.end();
	}
}
/**
 * @brief Checks if a flag is set for line.
 * @param [in] line Index (0-based) for line.
 * @param [in] flag Flag to check.
 * @return TRUE if flag is set, FALSE otherwise.
 */
BOOL CDiffTextBuffer::FlagIsSet(UINT line, DWORD flag)
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
	for(int ct=GetLineCount()-1; ct>=0; --ct)
	{
		SetLineFlag(ct, LF_DIFF, FALSE, FALSE, FALSE);
		SetLineFlag(ct, LF_TRIVIAL, FALSE, FALSE, FALSE);
		SetLineFlag(ct, LF_MOVED, FALSE, FALSE, FALSE);
	}
}


void CDiffTextBuffer::OnNotifyLineHasBeenEdited(int nLine)
{
	SetLineFlag(nLine, LF_DIFF, FALSE, FALSE, FALSE);
	SetLineFlag(nLine, LF_TRIVIAL, FALSE, FALSE, FALSE);
	SetLineFlag(nLine, LF_MOVED, FALSE, FALSE, FALSE);
	CGhostTextBuffer::OnNotifyLineHasBeenEdited(nLine);
}

/**
 * @brief Helper to determine the most used CRLF mode in the file
 * Normal call : determine the CRLF mode of the current line
 * Final call  : parameter lpLineBegin = NULL, return the style of the file
 *
 * @note  The lowest CRL_STYLE has the priority in case of equality
 */
int CDiffTextBuffer::NoteCRLFStyleFromBuffer(TCHAR *lpLineBegin, DWORD dwLineLen /* =0 */)
{
	static int count[3] = {0};
	int iStyle = -1;

	// give back the result when we ask for it
	if (lpLineBegin == NULL)
	{
		iStyle = 0;
		if (count[1] > count[iStyle])
			iStyle = 1;
		if (count[2] > count[iStyle])
			iStyle = 2;

		// reset the counter for the next file
		count[0] = count[1] = count[2] = 0;

		return iStyle;
	}

	if (dwLineLen >= 1)
	{
		if (lpLineBegin[dwLineLen-1] == _T('\r'))
			iStyle = CRLF_STYLE_MAC;
		if (lpLineBegin[dwLineLen-1] == _T('\n'))
			iStyle = CRLF_STYLE_UNIX;
	}
	if (dwLineLen >= 2)
	{
		if (lpLineBegin[dwLineLen-2] == _T('\r') && lpLineBegin[dwLineLen-1] == _T('\n'))
			iStyle = CRLF_STYLE_DOS;
	}
	ASSERT (iStyle != -1);
	count[iStyle] ++;
	return iStyle;
}

/// Reads one line from filebuffer and inserts to textbuffer
void CDiffTextBuffer::ReadLineFromBuffer(TCHAR *lpLineBegin, DWORD dwLineNum, DWORD dwLineLen /* =0 */)
{
	if (m_nSourceEncoding >= 0)
		iconvert (lpLineBegin, m_nSourceEncoding, 1, m_nSourceEncoding == 15);
	AppendLine(dwLineNum, lpLineBegin, dwLineLen);
}

/// Sets path for temporary files
void CDiffTextBuffer::SetTempPath(String path)
{
	m_strTempPath = path;
}

bool CDiffTextBuffer::IsInitialized() const
{
	return !!m_bInit;
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
int CDiffTextBuffer::LoadFromFile(LPCTSTR pszFileNameInit,
		PackingInfo * infoUnpacker, LPCTSTR sToFindUnpacker, BOOL & readOnly,
		CRLFSTYLE nCrlfStyle, const FileTextEncoding & encoding, CString &sError)
{
	ASSERT(!m_bInit);
	ASSERT(m_aLines.GetSize() == 0);

	// Unpacking the file here, save the result in a temporary file
	String sFileName = pszFileNameInit;
	if (infoUnpacker->bToBeScanned)
	{
		if (!FileTransform_Unpacking(sFileName, sToFindUnpacker, infoUnpacker, &unpackerSubcode))
		{
			InitNew(); // leave crystal editor in valid, empty state
			return FileLoadResult::FRESULT_ERROR_UNPACK;
		}
	}
	else
	{
		if (!FileTransform_Unpacking(sFileName, infoUnpacker, &unpackerSubcode))
		{
			InitNew(); // leave crystal editor in valid, empty state
			return FileLoadResult::FRESULT_ERROR_UNPACK;
		}
	}
	// we use the same unpacker for both files, so it must be defined after first file
	ASSERT(infoUnpacker->bToBeScanned != PLUGIN_AUTO);
	// we will load the transformed file
	LPCTSTR pszFileName = sFileName.c_str();

	String sExt;
	DWORD nRetVal = FileLoadResult::FRESULT_OK;

	// Set encoding based on extension, if we know one
	SplitFilename(pszFileName, NULL, NULL, &sExt);
	CCrystalTextView::TextDefinition *def = 
		CCrystalTextView::GetTextType(sExt.c_str());
	if (def && def->encoding != -1)
		m_nSourceEncoding = def->encoding;
	
	UniFile *pufile = infoUnpacker->pufile;
	if (pufile == 0)
		pufile = new UniMemFile;

	// Now we only use the UniFile interface
	// which is something we could implement for HTTP and/or FTP files

	if (!pufile->OpenReadOnly(pszFileName))
	{
		nRetVal = FileLoadResult::FRESULT_ERROR;
		UniFile::UniError uniErr = pufile->GetLastUniError();
		if (uniErr.hasError())
		{
			if (uniErr.apiname.IsEmpty())
				sError = uniErr.desc;
			else
				sError = GetSysError(uniErr.syserrnum);
		}
		InitNew(); // leave crystal editor in valid, empty state
		goto LoadFromFileExit;
	}
	else
	{
		// If the file is not unicode file, use the codepage we were given to
		// interpret the 8-bit characters. If the file is unicode file,
		// determine its type (IsUnicode() does that).
		if (encoding.m_unicoding == ucr::NONE || !pufile->IsUnicode())
			pufile->SetCodepage(encoding.m_codepage);
		UINT lineno = 0;
		CString eol, preveol;
		CString sline;
		bool done = false;
		UINT next_line_report = 100; // for trace messages
		UINT next_line_multiple = 5; // for trace messages
		COleDateTime start = COleDateTime::GetCurrentTime(); // for trace messages

		// Manually grow line array exponentially
		UINT arraysize = 500;
		m_aLines.SetSize(arraysize);
		
		// preveol must be initialized for empty files
		preveol = "\n";
		
		do {
			bool lossy=false;
			done = !pufile->ReadString(sline, eol, &lossy);

			// if last line had no eol, we can quit
			if (done && preveol.IsEmpty())
				break;
			// but if last line had eol, we add an extra (empty) line to buffer

			// Manually grow line array exponentially
			if (lineno == arraysize)
			{
				arraysize *= 2;
				m_aLines.SetSize(arraysize);
			}

			sline += eol; // TODO: opportunity for optimization, as CString append is terrible
			if (lossy)
			{
				// TODO: Should record lossy status of line
			}
			AppendLine(lineno, sline, sline.GetLength());
			++lineno;
			preveol = eol;

#ifdef _DEBUG
			// send occasional line counts to trace
			// (at 100, 500, 1000, 5000, etc)
			if (lineno == next_line_report)
			{
				__int64 dwBytesRead = pufile->GetPosition();
				COleDateTimeSpan duration = COleDateTime::GetCurrentTime() - start;
				if (duration.GetTotalMinutes() > 0)
				{
					CString strace = GetLineByteTimeReport(lineno, dwBytesRead, start);
					TRACE(_T("%s\n"), (LPCTSTR)strace);
				}
				next_line_report = next_line_multiple * next_line_report;
				next_line_multiple = (next_line_multiple == 5) ? 2 : 5;
			}
#endif // _DEBUG
		} while (!done);

#ifdef _DEBUG
		// Send report of duration to trace (if it took a while)
		COleDateTime end = COleDateTime::GetCurrentTime();
		COleDateTimeSpan duration = end - start;
		if (duration.GetTotalMinutes() > 0)
		{
			__int64 dwBytesRead = pufile->GetPosition();
			CString strace = GetLineByteTimeReport(lineno, dwBytesRead, start);
			TRACE(_T("%s\n"), (LPCTSTR)strace);
		}
#endif // _DEBUG

		// fix array size (due to our manual exponential growth
		m_aLines.SetSize(lineno);
	
		
		//Try to determine current CRLF mode (most frequent)
		if (nCrlfStyle == CRLF_STYLE_AUTOMATIC)
		{
			nCrlfStyle = GetTextFileStyle(pufile->GetTxtStats());
		}
		ASSERT(nCrlfStyle >= 0 && nCrlfStyle <= 2);
		SetCRLFMode(nCrlfStyle);
		
		//  At least one empty line must present
		// (view does not work for empty buffers)
		ASSERT(m_aLines.GetSize() > 0);
		
		m_bInit = TRUE;
		m_bModified = FALSE;
		m_bUndoGroup = m_bUndoBeginGroup = FALSE;
		m_nUndoBufSize = 1024; // crystaltextbuffer.cpp - UNDO_BUF_SIZE;
		m_nSyncPosition = m_nUndoPosition = 0;
		ASSERT(m_aUndoBuf.GetSize() == 0);
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
			readOnly = TRUE;
		}
	}
	
LoadFromFileExit:
	// close the file now to free the handle
	pufile->Close();
	delete pufile;

	// delete the file that unpacking may have created
	if (_tcscmp(pszFileNameInit, pszFileName) != 0)
		if (!::DeleteFile(pszFileName))
		{
			LogErrorString(Fmt(_T("DeleteFile(%s) failed: %s"),
				pszFileName, GetSysError(GetLastError())));
		}

	return nRetVal;
}

/**
 * @brief Saves file from buffer to disk
 *
 * @param bTempFile : FALSE if we are saving user files and
 * TRUE if we are saving workin-temp-files for diff-engine
 *
 * @return SAVE_DONE or an error code (list in MergeDoc.h)
 */
int CDiffTextBuffer::SaveToFile (LPCTSTR pszFileName,
		BOOL bTempFile, CString & sError, PackingInfo * infoUnpacker /*= NULL*/,
		CRLFSTYLE nCrlfStyle /*= CRLF_STYLE_AUTOMATIC*/,
		BOOL bClearModifiedFlag /*= TRUE*/ )
{
	ASSERT (m_bInit);

	if (!pszFileName || _tcslen(pszFileName) == 0)
		return SAVE_FAILED;	// No filename, cannot save...

	if (nCrlfStyle == CRLF_STYLE_AUTOMATIC &&
		!GetOptionsMgr()->GetBool(OPT_ALLOW_MIXED_EOL) ||
		infoUnpacker && infoUnpacker->disallowMixedEOL)
	{
			// get the default nCrlfStyle of the CDiffTextBuffer
		nCrlfStyle = GetCRLFMode();
		ASSERT(nCrlfStyle >= 0 && nCrlfStyle <= 2);
	}

	BOOL bOpenSuccess = TRUE;
	BOOL bSaveSuccess = FALSE;

	UniStdioFile file;
	file.SetUnicoding(m_encoding.m_unicoding);
	file.SetBom(m_encoding.m_bom);
	file.SetCodepage(m_encoding.m_codepage);

	String sIntermediateFilename; // used when !bTempFile

	if (bTempFile)
	{
		bOpenSuccess = !!file.OpenCreate(pszFileName);
	}
	else
	{
		sIntermediateFilename = env_GetTempFileName(m_strTempPath.c_str(),
			_T("MRG_"), NULL);
		if (sIntermediateFilename.empty())
			return SAVE_FAILED;  //Nothing to do if even tempfile name fails
		bOpenSuccess = !!file.OpenCreate(sIntermediateFilename.c_str());
	}

	if (!bOpenSuccess)
	{	
		UniFile::UniError uniErr = file.GetLastUniError();
		if (uniErr.hasError())
		{
			if (uniErr.apiname.IsEmpty())
				sError = uniErr.desc;
			else
				sError = GetSysError(uniErr.syserrnum);
			if (bTempFile)
				LogErrorString(Fmt(_T("Opening file %s failed: %s"),
					pszFileName, sError));
			else
				LogErrorString(Fmt(_T("Opening file %s failed: %s"),
					sIntermediateFilename, sError));
		}
		return SAVE_FAILED;
	}

	file.WriteBom();

	// line loop : get each real line and write it in the file
	CString sLine;
	CString sEol = GetStringEol(nCrlfStyle);
	int nLineCount = m_aLines.GetSize();
	for (int line=0; line<nLineCount; ++line)
	{
		if (GetLineFlags(line) & LF_GHOST)
			continue;

		// get the characters of the line (excluding EOL)
		if (GetLineLength(line) > 0)
			GetText(line, 0, line, GetLineLength(line), sLine, 0);
		else
			sLine = _T("");

		if (bTempFile)
			EscapeControlChars(sLine);
		// last real line ?
		if (line == ApparentLastRealLine())
		{
			// last real line is never EOL terminated
			ASSERT (_tcslen(GetLineEol(line)) == 0);
			// write the line and exit loop
			file.WriteString(sLine);
			break;
		}

		// normal real line : append an EOL
		if (nCrlfStyle == CRLF_STYLE_AUTOMATIC)
		{
			// either the EOL of the line (when preserve original EOL chars is on)
			sLine += GetLineEol(line);
		}
		else
		{
			// or the default EOL for this file
			sLine += sEol;
		}

		// write this line to the file (codeset or unicode conversions are done there)
		file.WriteString(sLine);
	}
	file.Close();


	if (!bTempFile)
	{
		// If we are saving user files
		// we need an unpacker/packer, at least a "do nothing" one
		ASSERT(infoUnpacker != NULL);
		// repack the file here, overwrite the temporary file we did save in
		String csTempFileName = sIntermediateFilename;
		infoUnpacker->subcode = unpackerSubcode;
		if (!FileTransform_Packing(csTempFileName, *infoUnpacker))
		{
			if (!::DeleteFile(sIntermediateFilename.c_str()))
			{
				LogErrorString(Fmt(_T("DeleteFile(%s) failed: %s"),
					sIntermediateFilename.c_str(), GetSysError(GetLastError())));
			}
			// returns now, don't overwrite the original file
			return SAVE_PACK_FAILED;
		}
		// the temp filename may have changed during packing
		if (csTempFileName != sIntermediateFilename)
		{
			if (!::DeleteFile(sIntermediateFilename.c_str()))
			{
				LogErrorString(Fmt(_T("DeleteFile(%s) failed: %s"),
					sIntermediateFilename.c_str(), GetSysError(GetLastError())));
			}
			sIntermediateFilename = csTempFileName;
		}

		// Write tempfile over original file
		if (::CopyFile(sIntermediateFilename.c_str(), pszFileName, FALSE))
		{
			if (!::DeleteFile(sIntermediateFilename.c_str()))
			{
				LogErrorString(Fmt(_T("DeleteFile(%s) failed: %s"),
					sIntermediateFilename.c_str(), GetSysError(GetLastError())));
			}
			if (bClearModifiedFlag)
			{
				SetModified(FALSE);
				m_nSyncPosition = m_nUndoPosition;
			}
			bSaveSuccess = TRUE;

			// remember revision number on save
			m_dwRevisionNumberOnSave = m_dwCurrentRevisionNumber;

			// redraw line revision marks
			UpdateViews (NULL, NULL, UPDATE_FLAGSONLY);	
		}
		else
		{
			sError = GetSysError(GetLastError());
			LogErrorString(Fmt(_T("CopyFile(%s, %s) failed: %s"),
				sIntermediateFilename.c_str(), pszFileName, sError));
		}
	}
	else
	{
		if (bClearModifiedFlag)
		{
			SetModified(FALSE);
			m_nSyncPosition = m_nUndoPosition;
		}
		bSaveSuccess = TRUE;
	}

	if (bSaveSuccess)
		return SAVE_DONE;
	else
		return SAVE_FAILED;
}

/// Replace text of line (no change to eol)
void CDiffTextBuffer::ReplaceLine(CCrystalTextView * pSource, int nLine, LPCTSTR pchText, int cchText, int nAction /*=CE_ACTION_UNKNOWN*/)
{
	if (GetLineLength(nLine)>0)
		DeleteText(pSource, nLine, 0, nLine, GetLineLength(nLine), nAction);
	int endl, endc;
	if (cchText)
		InsertText(pSource, nLine, 0, pchText, cchText, endl, endc, nAction);
}

/// Replace line (removing any eol, and only including one if in strText)
void CDiffTextBuffer::ReplaceFullLine(CCrystalTextView * pSource, int nLine,
		const CString &strText, int nAction /*=CE_ACTION_UNKNOWN*/)
{
	LPCTSTR eol = GetEol(strText);
	if (_tcscmp(GetLineEol(nLine), eol) == 0)
	{
		// (optimization) eols are the same, so just replace text inside line
		// we must clean strText from its eol...
		int eolLength = _tcslen(eol);
		ReplaceLine(pSource, nLine, strText, strText.GetLength() - eolLength, nAction);
		return;
	}

	// we may need a last line as the DeleteText end is (x=0,y=line+1)
	if (nLine+1 == GetLineCount())
		InsertGhostLine (pSource, GetLineCount());

	if (GetFullLineLength(nLine))
		DeleteText(pSource, nLine, 0, nLine+1, 0, nAction); 
	int endl,endc;
	if (int cchText = strText.GetLength())
		InsertText(pSource, nLine, 0, strText, cchText, endl,endc, nAction);
}

bool CDiffTextBuffer::curUndoGroup()
{
	return (m_aUndoBuf.GetSize()!=0 && m_aUndoBuf[0].m_dwFlags&UNDO_BEGINGROUP);
}
