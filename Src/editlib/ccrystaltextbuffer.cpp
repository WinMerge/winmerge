////////////////////////////////////////////////////////////////////////////
//  File:       ccrystaltextbuffer.cpp
//  Version:    1.0.0.0
//  Created:    29-Dec-1998
//
//  Author:     Stcherbatchenko Andrei
//  E-mail:     windfall@gmx.de
//
//  Implementation of the CCrystalTextBuffer class, a part of Crystal Edit -
//  syntax coloring text editor.
//
//  You are free to use or modify this code to the following restrictions:
//  - Acknowledge me somewhere in your about box, simple "Parts of code by.."
//  will be enough. If you can't (or don't want to), contact me personally.
//  - LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//  17-Feb-99
//  +   FIX: unnecessary 'HANDLE' in CCrystalTextBuffer::SaveToFile
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//  21-Feb-99
//      Paul Selormey, James R. Twine:
//  +   FEATURE: description for Undo/Redo actions
//  +   FEATURE: multiple MSVC-like bookmarks
//  +   FEATURE: 'Disable backspace at beginning of line' option
//  +   FEATURE: 'Disable drag-n-drop editing' option
//
//  +   FEATURE: changed layout of SUndoRecord. Now takes less memory
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//  19-Jul-99
//      Ferdinand Prantl:
//  +   FEATURE: some other things I've forgotten ...
//
//  ... it's being edited very rapidly so sorry for non-commented
//        and maybe "ugly" code ...
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//	??-Aug-99
//		Sven Wiegand (search for "//BEGIN SW" to find my changes):
//	+ FEATURE: Remembering the text-position of the latest change.
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//	24-Oct-99
//		Sven Wiegand
//	+ FIX: Setting m_ptLastChange to the beginning of the selection in
//		     InternalDeleteText(), so that position is valid in any case.
//		     Editor won't crash any more i.e. by selecting whole buffer and
//		     deleting it and then executing ID_EDIT_GOTO_LAST_CHANGE-command.
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <malloc.h>
#include "editcmd.h"
#include "ccrystaltextbuffer.h"
#include "ccrystaltextview.h"
#include "filesup.h"
#include "cs2cs.h"
#include <mbctype.h>

#ifndef __AFXPRIV_H__
#pragma message("Include <afxpriv.h> in your stdafx.h to avoid this message")
#include <afxpriv.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//  Line allocation granularity
#define     CHAR_ALIGN                  16
#define     ALIGN_BUF_SIZE(size)        ((size) / CHAR_ALIGN) * CHAR_ALIGN + CHAR_ALIGN;

#define     UNDO_BUF_SIZE               1024

const TCHAR crlf[] = _T ("\r\n");

#ifdef _DEBUG
#define _ADVANCED_BUGCHECK  1
#endif

int CCrystalTextBuffer::m_nDefaultEncoding = -1;

/////////////////////////////////////////////////////////////////////////////
// CCrystalTextBuffer::SUndoRecord

void CCrystalTextBuffer::SUndoRecord::
SetText (LPCTSTR pszText)
{
  m_pszText = NULL;
  if (pszText != NULL && pszText[0] != _T ('\0'))
    {
      int nLength = _tcslen (pszText);
      if (nLength > 1)
        {
          m_pszText = new TCHAR[(nLength + 1) * sizeof (TCHAR)];
          _tcscpy (m_pszText, pszText);
        }
      else
        {
          m_szText[0] = pszText[0];
        }
    }
}

void CCrystalTextBuffer::SUndoRecord::
FreeText ()
{
  if (HIWORD ((DWORD) m_pszText) != 0)
    delete[] m_pszText;
}


/////////////////////////////////////////////////////////////////////////////
// CCrystalTextBuffer::CUpdateContext

void CCrystalTextBuffer::CInsertContext::
RecalcPoint (CPoint & ptPoint)
{
  ASSERT (m_ptEnd.y > m_ptStart.y ||
          m_ptEnd.y == m_ptStart.y && m_ptEnd.x >= m_ptStart.x);
  if (ptPoint.y < m_ptStart.y)
    return;
  if (ptPoint.y > m_ptStart.y)
    {
      ptPoint.y += (m_ptEnd.y - m_ptStart.y);
      return;
    }
  if (ptPoint.x <= m_ptStart.x)
    return;
  ptPoint.y += (m_ptEnd.y - m_ptStart.y);
  ptPoint.x = m_ptEnd.x + (ptPoint.x - m_ptStart.x);
}

void CCrystalTextBuffer::CDeleteContext::
RecalcPoint (CPoint & ptPoint)
{
  ASSERT (m_ptEnd.y > m_ptStart.y ||
          m_ptEnd.y == m_ptStart.y && m_ptEnd.x >= m_ptStart.x);
  if (ptPoint.y < m_ptStart.y)
    return;
  if (ptPoint.y > m_ptEnd.y)
    {
      ptPoint.y -= (m_ptEnd.y - m_ptStart.y);
      return;
    }
  if (ptPoint.y == m_ptEnd.y && ptPoint.x >= m_ptEnd.x)
    {
      ptPoint.y = m_ptStart.y;
      ptPoint.x = m_ptStart.x + (ptPoint.x - m_ptEnd.x);
      return;
    }
  if (ptPoint.y == m_ptStart.y)
    {
      if (ptPoint.x > m_ptStart.x)
        ptPoint.x = m_ptStart.x;
      return;
    }
  ptPoint = m_ptStart;
}


/////////////////////////////////////////////////////////////////////////////
// CCrystalTextBuffer

IMPLEMENT_DYNCREATE (CCrystalTextBuffer, CCmdTarget)

CCrystalTextBuffer::CCrystalTextBuffer ()
{
  m_bInit = FALSE;
  m_bReadOnly = FALSE;
  m_bModified = FALSE;
  m_nCRLFMode = 0;
  m_bCreateBackupFile = FALSE;
  m_nUndoPosition = 0;
  m_bInsertTabs = TRUE;
  m_nTabSize = 4;
  //BEGIN SW
  m_ptLastChange.x = m_ptLastChange.y = -1;
  //END SW
  m_nSourceEncoding = m_nDefaultEncoding;
}

CCrystalTextBuffer:: ~ CCrystalTextBuffer ()
{
  ASSERT (!m_bInit);            //  You must call FreeAll() before deleting the object
}


BEGIN_MESSAGE_MAP (CCrystalTextBuffer, CCmdTarget)
//{{AFX_MSG_MAP(CCrystalTextBuffer)
//}}AFX_MSG_MAP
END_MESSAGE_MAP ()


/////////////////////////////////////////////////////////////////////////////
// CCrystalTextBuffer message handlers

static bool iseol(TCHAR ch)
{
  return ch=='\r' || ch=='\n';
}

static bool isdoseol(LPCTSTR sz)
{
  return sz[0]=='\r' && sz[1]=='\n';
}

void CCrystalTextBuffer::InsertLine (LPCTSTR pszLine, int nLength /*= -1*/ , int nPosition /*= -1*/ )
{
  if (nLength == -1)
    {
      if (pszLine == NULL)
        nLength = 0;
      else
        nLength = _tcslen (pszLine);
    }

  SLineInfo li;
  li.m_nLength = nLength;
  li.m_nMax = ALIGN_BUF_SIZE (li.m_nLength + 1);
  ASSERT (li.m_nMax >= li.m_nLength + 1);
  li.m_pcLine = new TCHAR[li.m_nMax];
  if (li.m_nLength > 0)
    {
      DWORD dwLen = sizeof (TCHAR) * li.m_nLength;
      CopyMemory (li.m_pcLine, pszLine, dwLen);
    }
  li.m_pcLine[li.m_nLength] = '\0';

  int nEols = 0;
  if (nLength>1 && isdoseol(&pszLine[nLength-2]))
    nEols = 2;
  else if (nLength && iseol(pszLine[nLength-1]))
    nEols = 1;
  li.m_nLength -= nEols;
  li.m_nEolChars = nEols;


  if (nPosition == -1)
    m_aLines.Add (li);
  else
    m_aLines.InsertAt (nPosition, li);

#ifdef _DEBUG
  int nLines = m_aLines.GetSize ();
  if (nLines % 5000 == 0)
    TRACE1 ("%d lines loaded!\n", nLines);
#endif
}

// Add characters to end of specified line
// Specified line must not have any EOL characters
void CCrystalTextBuffer::
AppendLine (int nLineIndex, LPCTSTR pszChars, int nLength /*= -1*/ )
{
  if (nLength == -1)
    {
      if (pszChars == NULL)
        return;
      nLength = _tcslen (pszChars);
    }

  if (nLength == 0)
    return;

  register SLineInfo & li = m_aLines[nLineIndex];
  ASSERT(!li.m_nEolChars);
  int nBufNeeded = li.m_nLength + nLength+1;
  if (nBufNeeded > li.m_nMax)
    {
      li.m_nMax = ALIGN_BUF_SIZE (nBufNeeded);
      ASSERT (li.m_nMax >= li.m_nLength + nLength);
      TCHAR *pcNewBuf = new TCHAR[li.m_nMax];
      if (li.FullLength() > 0)
        memcpy (pcNewBuf, li.m_pcLine, sizeof (TCHAR) * (li.FullLength()+1));
      delete[] li.m_pcLine;
      li.m_pcLine = pcNewBuf;
    }
  memcpy (li.m_pcLine + li.m_nLength, pszChars, sizeof (TCHAR) * (nLength+1));
  li.m_nLength += nLength;
  li.m_pcLine[li.m_nLength] = '\0';
  // Did line gain eol ? (We asserted above that it had none at start)
   if (nLength>1 && isdoseol(&li.m_pcLine[li.m_nLength-2]))
     {
       li.m_nEolChars = 2;
     }
   else if (iseol(li.m_pcLine[li.m_nLength-1]))
      {
       li.m_nEolChars = 1;
      }
   li.m_nLength -= li.m_nEolChars;
  ASSERT (li.m_nLength + li.m_nEolChars <= li.m_nMax);
}

void CCrystalTextBuffer::
FreeAll ()
{
  //  Free text
  int nCount = m_aLines.GetSize ();
  for (int I = 0; I < nCount; I++)
    {
      if (m_aLines[I].m_nMax > 0)
        delete[] m_aLines[I].m_pcLine;
    }
  m_aLines.RemoveAll ();

  // Undo buffer will be cleared by its destructor

  m_bInit = FALSE;
	//BEGIN SW
	m_ptLastChange.x = m_ptLastChange.y = -1;
	//END SW
}

BOOL CCrystalTextBuffer::
InitNew (int nCrlfStyle /*= CRLF_STYLE_DOS*/ )
{
  ASSERT (!m_bInit);
  ASSERT (m_aLines.GetSize () == 0);
  ASSERT (nCrlfStyle >= 0 && nCrlfStyle <= 2);
  InsertLine (_T (""));
  m_bInit = TRUE;
  m_bReadOnly = FALSE;
  m_nCRLFMode = nCrlfStyle;
  m_bModified = FALSE;
  m_bInsertTabs	= TRUE;
  m_nTabSize = 4;
  m_nSyncPosition = m_nUndoPosition = 0;
  m_bUndoGroup = m_bUndoBeginGroup = FALSE;
  m_nUndoBufSize = UNDO_BUF_SIZE;
  ASSERT (m_aUndoBuf.GetSize () == 0);
  UpdateViews (NULL, NULL, UPDATE_RESET);
	//BEGIN SW
	m_ptLastChange.x = m_ptLastChange.y = -1;
	//END SW
  return TRUE;
}

BOOL CCrystalTextBuffer::
GetReadOnly ()
const
{
  ASSERT (m_bInit);        //  Text buffer not yet initialized.
  //  You must call InitNew() or LoadFromFile() first!

  return m_bReadOnly;
}

void CCrystalTextBuffer::SetReadOnly (BOOL bReadOnly /*= TRUE*/ )
{
  ASSERT (m_bInit);             //  Text buffer not yet initialized.
  //  You must call InitNew() or LoadFromFile() first!

  m_bReadOnly = bReadOnly;
}


// WinMerge has own routine for loading
#if 0

static LPCTSTR crlfs[] =
  {
    _T ("\x0d\x0a"), //  DOS/Windows style
    _T ("\x0a"),     //  UNIX style
    _T ("\x0a")      //  Macintosh style
  };

BOOL CCrystalTextBuffer::
LoadFromFile (LPCTSTR pszFileName, int nCrlfStyle /*= CRLF_STYLE_AUTOMATIC*/ )
{
	ASSERT (!m_bInit);
	ASSERT (m_aLines.GetSize () == 0);

	HANDLE hFile = NULL;
	int nCurrentMax = 256;
	LPTSTR pcLineBuf = new TCHAR[nCurrentMax];

	BOOL bSuccess = FALSE;

	int nExt = GetExtPosition (pszFileName);
	if (pszFileName[nExt] == _T ('.'))
		nExt++;
	CCrystalTextView::TextDefinition *def = CCrystalTextView::GetTextType (pszFileName + nExt);
	if (def && def->encoding != -1)
		m_nSourceEncoding = def->encoding;

	__try
	{
		DWORD dwFileAttributes =::GetFileAttributes (pszFileName);
		if (dwFileAttributes == (DWORD) - 1)
			__leave;

		hFile =::CreateFile (pszFileName, GENERIC_READ, FILE_SHARE_READ + FILE_SHARE_WRITE, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			__leave;

        int nCurrentLength = 0;

        const DWORD dwBufSize = 32768;
        LPTSTR pcBuf = (LPTSTR) _alloca (dwBufSize);
        DWORD dwCurSize;
        if (!::ReadFile (hFile, pcBuf, dwBufSize, &dwCurSize, NULL))
			__leave;

		if (nCrlfStyle == CRLF_STYLE_AUTOMATIC)
		{
			//  Try to determine current CRLF mode based on first line
			for (DWORD I = 0; I < dwCurSize; I++)
			{
				if ((pcBuf[I] == _T('\x0d')) || (pcBuf[I] == _T('\x0a')))
                    break;
			}
			if (I == dwCurSize)
			{
				//  By default (or in the case of empty file), set DOS style
				nCrlfStyle = CRLF_STYLE_DOS;
			}
			else
			{
				//  Otherwise, analyse the first occurance of line-feed character
				if (pcBuf[I] == _T('\x0a'))
				{
					nCrlfStyle = CRLF_STYLE_UNIX;
				}
				else
				{
					if (I < dwCurSize - 1 && pcBuf[I + 1] == _T ('\x0a'))
                        nCrlfStyle = CRLF_STYLE_DOS;
					else
                        nCrlfStyle = CRLF_STYLE_MAC;
				}
			}
		}

		ASSERT (nCrlfStyle >= 0 && nCrlfStyle <= 2);
		m_nCRLFMode = nCrlfStyle;

		m_aLines.SetSize (0, 4096);

		DWORD dwBufPtr = 0;
		while (dwBufPtr < dwCurSize)
		{
			TCHAR c = pcBuf[dwBufPtr];
			dwBufPtr++;
			if (dwBufPtr == dwCurSize && dwCurSize == dwBufSize)
			{
				if (!::ReadFile (hFile, pcBuf, dwBufSize, &dwCurSize, NULL))
					__leave;
				dwBufPtr = 0;
			}

			pcLineBuf[nCurrentLength] = c;
			nCurrentLength++;
			if (nCurrentLength == nCurrentMax)
			{
				//  Reallocate line buffer
				nCurrentMax += 256;
				LPTSTR pcNewBuf = new TCHAR[nCurrentMax];
				memcpy (pcNewBuf, pcLineBuf, nCurrentLength);
				delete[] pcLineBuf;
				pcLineBuf = pcNewBuf;
			}

			// detect both types of EOL for each line
			// handles mixed mode files.
			// Perry (2002-11-26): What about MAC files ? They don't have 0x0A at all. I think this doesn't handle them.
			if( c==0x0A )
			{
				// 2002-11-26  Perry fixed bug & added optional sensitivity to CR
				// remove EOL characters
				if (!m_EolSensitive && nCurrentLength>1)
				{
					TCHAR prevChar = pcLineBuf[nCurrentLength-2];
					pcLineBuf[nCurrentLength - (prevChar==0x0D?2:1) ] = '\0';
				}
				else
				{
					pcLineBuf[nCurrentLength - 1] = '\0';
				}
				nCurrentLength = 0;
				if (m_nSourceEncoding >= 0)
					iconvert (pcLineBuf, m_nSourceEncoding, 1, m_nSourceEncoding == 15);
				InsertLine (pcLineBuf);
			}
		}

		pcLineBuf[nCurrentLength] = 0;
		InsertLine (pcLineBuf);

		ASSERT (m_aLines.GetSize () > 0);   //  At least one empty line must present

		m_bInit = TRUE;
		m_bReadOnly = (dwFileAttributes & FILE_ATTRIBUTE_READONLY) != 0;
		m_bModified = FALSE;
		m_bUndoGroup = m_bUndoBeginGroup = FALSE;
		m_nUndoBufSize = UNDO_BUF_SIZE;
		m_nSyncPosition = m_nUndoPosition = 0;
		ASSERT (m_aUndoBuf.GetSize () == 0);
		bSuccess = TRUE;

		RetypeViews (pszFileName);
		UpdateViews (NULL, NULL, UPDATE_RESET);
  }
  __finally
  {
	  if (pcLineBuf != NULL)
		  delete[] pcLineBuf;
  }
  if (hFile != NULL && hFile != INVALID_HANDLE_VALUE)
	  ::CloseHandle (hFile);
  //BEGIN SW
  m_ptLastChange.x = m_ptLastChange.y = -1;
  //END SW
  return bSuccess;
}
#endif // #if 0 loadfromfile

// WinMerge has own routine for saving
#if 0
BOOL CCrystalTextBuffer::SaveToFile(LPCTSTR pszFileName,
									int nCrlfStyle /*= CRLF_STYLE_AUTOMATIC*/,
									BOOL bClearModifiedFlag /*= TRUE*/)
{
  ASSERT (nCrlfStyle == CRLF_STYLE_AUTOMATIC || nCrlfStyle == CRLF_STYLE_DOS ||
          nCrlfStyle == CRLF_STYLE_UNIX || nCrlfStyle == CRLF_STYLE_MAC);
  ASSERT (m_bInit);
  HANDLE hTempFile = INVALID_HANDLE_VALUE;
  HANDLE hSearch = INVALID_HANDLE_VALUE;
  TCHAR szTempFileDir[_MAX_PATH + 1];
  TCHAR szTempFileName[_MAX_PATH + 1];
  TCHAR szBackupFileName[_MAX_PATH + 1];
  BOOL bSuccess = FALSE;
  __try
  {
    TCHAR drive[_MAX_PATH], dir[_MAX_PATH], name[_MAX_PATH], ext[_MAX_PATH];
#ifdef _UNICODE
    _wsplitpath (pszFileName, drive, dir, name, ext);
#else
    _splitpath (pszFileName, drive, dir, name, ext);
#endif
    _tcscpy (szTempFileDir, drive);
    _tcscat (szTempFileDir, dir);
    _tcscpy (szBackupFileName, pszFileName);
    _tcscat (szBackupFileName, _T (".bak"));

    if (::GetTempFileName (szTempFileDir, _T ("CRE"), 0, szTempFileName) == 0)
      __leave;

      hTempFile =::CreateFile (szTempFileName, GENERIC_WRITE, 0, NULL,
                               CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
      if (hTempFile == INVALID_HANDLE_VALUE)
        __leave;

        if (nCrlfStyle == CRLF_STYLE_AUTOMATIC)
          nCrlfStyle = m_nCRLFMode;

          ASSERT (nCrlfStyle >= 0 && nCrlfStyle <= 2);
          LPCTSTR pszCRLF = crlfs[nCrlfStyle];
          int nCRLFLength = _tcslen (pszCRLF);

          int nLineCount = m_aLines.GetSize ();
          for (int nLine = 0; nLine < nLineCount; nLine++)
            {
              int nLength = m_aLines[nLine].m_nLength;
              DWORD dwWrittenBytes;
              if (nLength > 0)
                {
                  LPCTSTR pszLine = m_aLines[nLine].m_pcLine;
                  if (m_nSourceEncoding >= 0)
                    {
                      LPTSTR pszBuf;
                      iconvert_new (m_aLines[nLine].m_pcLine, &pszBuf, 1, m_nSourceEncoding, m_nSourceEncoding == 15);
                      if (!::WriteFile (hTempFile, pszBuf, nLength, &dwWrittenBytes, NULL))
                        {
                          free (pszBuf);
                          __leave;
                        }
                      free (pszBuf);
                    }
                  else
                    if (!::WriteFile (hTempFile, pszLine, nLength, &dwWrittenBytes, NULL))
                      __leave;
                  if (nLength != (int) dwWrittenBytes)
                    __leave;
                }
              if (nLine < nLineCount - 1)     //  Last line must not end with CRLF

                {
                  if (!::WriteFile (hTempFile, pszCRLF, nCRLFLength, &dwWrittenBytes, NULL))
                    __leave;
                  if (nCRLFLength != (int) dwWrittenBytes)
                    __leave;
                }
            }
    ::CloseHandle (hTempFile);
    hTempFile = INVALID_HANDLE_VALUE;

    if (m_bCreateBackupFile)
      {
        WIN32_FIND_DATA wfd;
        hSearch =::FindFirstFile (pszFileName, &wfd);
        if (hSearch != INVALID_HANDLE_VALUE)
          {
            //  File exist - create backup file
            ::DeleteFile (szBackupFileName);
            if (!::MoveFile (pszFileName, szBackupFileName))
              __leave;
            ::FindClose (hSearch);
            hSearch = INVALID_HANDLE_VALUE;
          }
      }
    else
      {
        ::DeleteFile (pszFileName);
      }

    //  Move temporary file to target name
    if (!::MoveFile (szTempFileName, pszFileName))
      __leave;

      if (bClearModifiedFlag)
        {
          SetModified (FALSE);
          m_nSyncPosition = m_nUndoPosition;
        }
    bSuccess = TRUE;
  }
  __finally
  {
    if (hSearch != INVALID_HANDLE_VALUE)
      ::FindClose (hSearch);
      if (hTempFile != INVALID_HANDLE_VALUE)
        ::CloseHandle (hTempFile);
        ::DeleteFile (szTempFileName);
      }
      return bSuccess;
}
#endif // #if 0 savetofile

int CCrystalTextBuffer::
GetCRLFMode ()
{
  return m_nCRLFMode;
}

// Default EOL to use if editor has to manufacture one
// (this occurs with ghost lines)
void CCrystalTextBuffer::
SetCRLFMode (int nCRLFMode)
{
  if (nCRLFMode==CRLF_STYLE_AUTOMATIC)
    nCRLFMode = CRLF_STYLE_DOS;
  m_nCRLFMode = nCRLFMode;

  ASSERT(m_nCRLFMode==CRLF_STYLE_DOS || m_nCRLFMode==CRLF_STYLE_UNIX || m_nCRLFMode==CRLF_STYLE_MAC);
}

int CCrystalTextBuffer::
GetLineCount ()
{
  ASSERT (m_bInit);             //  Text buffer not yet initialized.
  //  You must call InitNew() or LoadFromFile() first!

  return m_aLines.GetSize ();
}

// number of characters in line (excluding any trailing eol characters)
int CCrystalTextBuffer::
GetLineLength (int nLine)
{
  ASSERT (m_bInit);             //  Text buffer not yet initialized.
  //  You must call InitNew() or LoadFromFile() first!

  return m_aLines[nLine].m_nLength;
}

// number of characters in line (including any trailing eol characters)
int CCrystalTextBuffer::
GetFullLineLength (int nLine)
{
  ASSERT (m_bInit);             //  Text buffer not yet initialized.
  //  You must call InitNew() or LoadFromFile() first!

  return m_aLines[nLine].m_nLength + m_aLines[nLine].m_nEolChars;
}

// get pointer to any trailing eol characters (pointer to empty string if none)
LPCTSTR CCrystalTextBuffer::
GetLineEol (int nLine)
{
  ASSERT (m_bInit);             //  Text buffer not yet initialized.
  if (m_aLines[nLine].m_nEolChars)
    return &m_aLines[nLine].m_pcLine[m_aLines[nLine].Length()];
  else
    return _T("");
}

LPTSTR CCrystalTextBuffer::
GetLineChars (int nLine)
{
  ASSERT (m_bInit);             //  Text buffer not yet initialized.
  //  You must call InitNew() or LoadFromFile() first!

  return m_aLines[nLine].m_pcLine;
}

DWORD CCrystalTextBuffer::
GetLineFlags (int nLine)
{
  ASSERT (m_bInit);             //  Text buffer not yet initialized.
  //  You must call InitNew() or LoadFromFile() first!

  return m_aLines[nLine].m_dwFlags;
}

static int
FlagToIndex (DWORD dwFlag)
{
  int nIndex = 0;
  while ((dwFlag & 1) == 0)
    {
      dwFlag = dwFlag >> 1;
      nIndex++;
      if (nIndex == 32)
        return -1;
    }
  dwFlag = dwFlag & 0xFFFFFFFE;
  if (dwFlag != 0)
    return -1;
  return nIndex;

}

int CCrystalTextBuffer::
FindLineWithFlag (DWORD dwFlag)
{
  int nSize = m_aLines.GetSize ();
  for (int L = 0; L < nSize; L++)
    {
      if ((m_aLines[L].m_dwFlags & dwFlag) != 0)
        return L;
    }
  return -1;
}

int CCrystalTextBuffer::
GetLineWithFlag (DWORD dwFlag)
{
  int nFlagIndex =::FlagToIndex (dwFlag);
  if (nFlagIndex < 0)
    {
      ASSERT (FALSE);           //  Invalid flag passed in

      return -1;
    }
  return FindLineWithFlag (dwFlag);
}

void CCrystalTextBuffer::
SetLineFlag (int nLine, DWORD dwFlag, BOOL bSet, BOOL bRemoveFromPreviousLine /*= TRUE*/ , BOOL bUpdate /*=TRUE*/)
{
  ASSERT (m_bInit);             //  Text buffer not yet initialized.
  //  You must call InitNew() or LoadFromFile() first!

  int nFlagIndex =::FlagToIndex (dwFlag);
  if (nFlagIndex < 0 && (nLine == -1 || bRemoveFromPreviousLine))
    {
      ASSERT (FALSE);           //  Invalid flag passed in

      return;
    }

  if (nLine == -1)
    {
      ASSERT (!bSet);
      nLine = FindLineWithFlag (dwFlag);
      if (nLine == -1)
        return;
      bRemoveFromPreviousLine = FALSE;
    }

  DWORD dwNewFlags = m_aLines[nLine].m_dwFlags;
  if (bSet)
  {
	  if (dwFlag==0)
		  dwNewFlags=0;
	  else
		dwNewFlags = dwNewFlags | dwFlag;
  }
  else
    dwNewFlags = dwNewFlags & ~dwFlag;

  if (m_aLines[nLine].m_dwFlags != dwNewFlags)
    {
      if (bRemoveFromPreviousLine)
        {
          int nPrevLine = FindLineWithFlag (dwFlag);
          if (bSet)
            {
              if (nPrevLine >= 0)
                {
                  ASSERT ((m_aLines[nPrevLine].m_dwFlags & dwFlag) != 0);
                  m_aLines[nPrevLine].m_dwFlags &= ~dwFlag;
				  if (bUpdate)
					UpdateViews (NULL, NULL, UPDATE_SINGLELINE | UPDATE_FLAGSONLY, nPrevLine);
                }
            }
          else
            {
              ASSERT (nPrevLine == nLine);
            }
        }

      m_aLines[nLine].m_dwFlags = dwNewFlags;
      if (bUpdate)
		  UpdateViews (NULL, NULL, UPDATE_SINGLELINE | UPDATE_FLAGSONLY, nLine);
    }
}

void CCrystalTextBuffer::
GetText (int nStartLine, int nStartChar, int nEndLine, int nEndChar, CString & text, LPCTSTR pszCRLF /*= NULL*/ )
{
  ASSERT (m_bInit);             //  Text buffer not yet initialized.
  //  You must call InitNew() or LoadFromFile() first!

  ASSERT (nStartLine >= 0 && nStartLine < m_aLines.GetSize ());
  ASSERT (nStartChar >= 0 && nStartChar <= m_aLines[nStartLine].m_nLength);
  ASSERT (nEndLine >= 0 && nEndLine < m_aLines.GetSize ());
  ASSERT (nEndChar >= 0 && nEndChar <= m_aLines[nEndLine].m_nLength);
  ASSERT (nStartLine < nEndLine || nStartLine == nEndLine && nStartChar < nEndChar);

  if (pszCRLF == NULL)
    pszCRLF = crlf;
  int nCRLFLength = _tcslen (pszCRLF);
  ASSERT (nCRLFLength > 0);

  int nBufSize = 0;
  for (int L = nStartLine; L <= nEndLine; L++)
    {
      nBufSize += m_aLines[L].m_nLength;
      nBufSize += nCRLFLength;
    }

  LPTSTR pszBuf = text.GetBuffer (nBufSize);

  if (nStartLine < nEndLine)
    {
      int nCount = m_aLines[nStartLine].m_nLength - nStartChar;
      if (nCount > 0)
        {
          memcpy (pszBuf, m_aLines[nStartLine].m_pcLine + nStartChar, sizeof (TCHAR) * nCount);
          pszBuf += nCount;
        }
      memcpy (pszBuf, pszCRLF, sizeof (TCHAR) * nCRLFLength);
      pszBuf += nCRLFLength;
      for (int I = nStartLine + 1; I < nEndLine; I++)
        {
          nCount = m_aLines[I].m_nLength;
          if (nCount > 0)
            {
              memcpy (pszBuf, m_aLines[I].m_pcLine, sizeof (TCHAR) * nCount);
              pszBuf += nCount;
            }
          memcpy (pszBuf, pszCRLF, sizeof (TCHAR) * nCRLFLength);
          pszBuf += nCRLFLength;
        }
      if (nEndChar > 0)
        {
          memcpy (pszBuf, m_aLines[nEndLine].m_pcLine, sizeof (TCHAR) * nEndChar);
          pszBuf += nEndChar;
        }
    }
  else
    {
      int nCount = nEndChar - nStartChar;
      memcpy (pszBuf, m_aLines[nStartLine].m_pcLine + nStartChar, sizeof (TCHAR) * nCount);
      pszBuf += nCount;
    }
  pszBuf[0] = 0;
  text.ReleaseBuffer ();
  text.FreeExtra ();
}

void CCrystalTextBuffer::
AddView (CCrystalTextView * pView)
{
  m_lpViews.AddTail (pView);
}

void CCrystalTextBuffer::
RemoveView (CCrystalTextView * pView)
{
  POSITION pos = m_lpViews.GetHeadPosition ();
  while (pos != NULL)
    {
      POSITION thispos = pos;
      CCrystalTextView *pvw = m_lpViews.GetNext (pos);
      if (pvw == pView)
        {
          m_lpViews.RemoveAt (thispos);
          return;
        }
    }
  ASSERT (FALSE);
}

CCrystalTextView::TextDefinition *CCrystalTextBuffer::
RetypeViews (LPCTSTR lpszFileName)
{
  POSITION pos = m_lpViews.GetHeadPosition ();
  CString sNew = GetExt (lpszFileName);
  CCrystalTextView::TextDefinition *def = CCrystalTextView::GetTextType (sNew);
  while (pos != NULL)
    {
      CCrystalTextView *pView = m_lpViews.GetNext (pos);
      pView->SetTextType (def);
    }
  return def;
}

void CCrystalTextBuffer::
UpdateViews (CCrystalTextView * pSource, CUpdateContext * pContext, DWORD dwUpdateFlags, int nLineIndex /*= -1*/ )
{
  POSITION pos = m_lpViews.GetHeadPosition ();
  while (pos != NULL)
    {
      CCrystalTextView *pView = m_lpViews.GetNext (pos);
      pView->UpdateView (pSource, pContext, dwUpdateFlags, nLineIndex);
    }
}

BOOL CCrystalTextBuffer::
InternalDeleteText (CCrystalTextView * pSource, int nStartLine, int nStartChar, int nEndLine, int nEndChar, int * pRealStart, int * pRealEnd)
{
  ASSERT (m_bInit);             //  Text buffer not yet initialized.
  //  You must call InitNew() or LoadFromFile() first!

  ASSERT (nStartLine >= 0 && nStartLine < m_aLines.GetSize ());
  ASSERT (nStartChar >= 0 && nStartChar <= m_aLines[nStartLine].m_nLength);
  ASSERT (nEndLine >= 0 && nEndLine < m_aLines.GetSize ());
  ASSERT (nEndChar >= 0 && nEndChar <= m_aLines[nEndLine].m_nLength);
  ASSERT (nStartLine < nEndLine || nStartLine == nEndLine && nStartChar < nEndChar);
  if (m_bReadOnly)
    return FALSE;

  int nRealStart = ComputeRealLine(nStartLine);
  if (pRealStart) *pRealStart = nRealStart;
  int nRealEnd = ComputeRealLine(nEndLine);
  if (pRealEnd) *pRealEnd = nRealEnd;

  CDeleteContext context;
  context.m_ptStart.y = nStartLine;
  context.m_ptStart.x = nStartChar;
  context.m_ptEnd.y = nEndLine;
  context.m_ptEnd.x = nEndChar;
  if (nStartLine == nEndLine)
    {
      // delete part of one line
      SLineInfo & li = m_aLines[nStartLine];
      if (nEndChar < li.Length() || li.m_nEolChars)
        {
          // preserve characters after deleted range by shifting up
          memcpy (li.m_pcLine + nStartChar, li.m_pcLine + nEndChar,
                  sizeof (TCHAR) * (li.FullLength() - nEndChar));
        }
      li.m_nLength -= (nEndChar - nStartChar);
      li.m_pcLine[li.FullLength()] = '\0';

      if (pSource!=NULL)
        UpdateViews (pSource, &context, UPDATE_SINGLELINE | UPDATE_HORZRANGE, nStartLine);
    }
  else
    {
      // delete multiple lines
      int nRestCount = m_aLines[nEndLine].FullLength() - nEndChar;
      CString sTail;
      if (nRestCount > 0)
        {
          sTail = m_aLines[nEndLine].m_pcLine + nEndChar;
	   ASSERT(sTail.GetLength() == nRestCount);
        }

      int nDelCount = nEndLine - nStartLine;
      for (int L = nStartLine + 1; L <= nEndLine; L++)
        delete[] m_aLines[L].m_pcLine;
      m_aLines.RemoveAt (nStartLine + 1, nDelCount);

      //  nEndLine is no more valid
      m_aLines[nStartLine].m_nLength = nStartChar;
      m_aLines[nStartLine].m_nEolChars = 0;
      if (nRestCount > 0)
        {
          AppendLine (nStartLine, sTail, nRestCount);
        }

	  if (pSource!=NULL)
		UpdateViews (pSource, &context, UPDATE_HORZRANGE | UPDATE_VERTRANGE, nStartLine);
    }

  if (!m_bModified)
    SetModified (TRUE);
	//BEGIN SW
	// remember current cursor position as last editing position
	m_ptLastChange = context.m_ptStart;
	//END SW

  if (nRealStart != nRealEnd)
  {
    // TODO: Be smarter, and don't recompute if it is easy to see what changed
    RecomputeRealityMapping();
  }

  return TRUE;
}

// Remove the last [bytes] characters from specified line, and return them
// (EOL characters are included)
CString CCrystalTextBuffer::
StripTail (int i, int bytes)
{
  SLineInfo & li = m_aLines[i];
  // Must at least take off the EOL characters
  ASSERT(bytes >= li.FullLength() - li.Length());

  int offset = li.FullLength()-bytes;
  // Must not take off more than exist
  ASSERT(offset >= 0);

  CString str = li.m_pcLine + offset;
  li.m_nLength = offset;
  li.m_nEolChars = 0;
  return str;
}

BOOL CCrystalTextBuffer::
InternalInsertText (CCrystalTextView * pSource, int nLine, int nPos, LPCTSTR pszText, int &nEndLine, int &nEndChar)
{
  ASSERT (m_bInit);             //  Text buffer not yet initialized.
  //  You must call InitNew() or LoadFromFile() first!

  ASSERT (nLine >= 0 && nLine < m_aLines.GetSize ());
  ASSERT (nPos >= 0 && nPos <= m_aLines[nLine].m_nLength);
  if (m_bReadOnly)
    return FALSE;

  CInsertContext context;
  context.m_ptStart.x = nPos;
  context.m_ptStart.y = nLine;

  int nRestCount = GetFullLineLength(nLine) - nPos;
  CString sTail;
  if (nRestCount > 0)
    {
      // remove end of line (we'll put it back on afterwards)
      sTail = StripTail(nLine, nRestCount);
    }


  int nCurrentLine = nLine;
  BOOL bNewLines = FALSE;
  int nTextPos;
  for (;;)
    {
      int haseol = 0;
      nTextPos = 0;
      // advance to end of line
      while (pszText[nTextPos] && !iseol(pszText[nTextPos]))
        nTextPos++;
      // advance after EOL of line
      if (isdoseol(&pszText[nTextPos]))
        {
          haseol = 1;
          nTextPos += 2;
        }
      else if (iseol(pszText[nTextPos]))
        {
          haseol = 1;
          nTextPos++;
        }

      // The first line of the new text is appended to the start line
      // All succeeding lines are inserted
      if (nCurrentLine == nLine)
        {
          AppendLine (nLine, pszText, nTextPos);
          if (haseol)
            bNewLines = TRUE;
        }
      else
        {
          InsertLine (pszText, nTextPos, nCurrentLine);
          bNewLines = TRUE;
        }


      if (pszText[nTextPos] == 0)
        {
          // we just finished our insert
          // now we have to reattach the tail
          if (haseol)
            {
              nEndLine = nCurrentLine+1;
              nEndChar = 0;
            }
          else
            {
              nEndLine = nCurrentLine;
              nEndChar = GetLineLength(nEndLine);
            }
	   if (!sTail.IsEmpty())
            {
              if (haseol)
                InsertLine(sTail, -1, nEndLine);
              else
                AppendLine (nCurrentLine, sTail, nRestCount);
            }
          if (nEndLine == GetLineCount())
            {
                 // We left cursor after last screen line
                 // which is an illegal cursor position
                 // so manufacture a new trailing ghost line
		   InsertLine(_T(""));
            }
          break;
        }

      ++nCurrentLine;
      pszText += nTextPos;
    }

  context.m_ptEnd.x = nEndChar;
  context.m_ptEnd.y = nEndLine;

  if (nLine != nEndLine)
  {
    // TODO: Be smarter, and don't recompute if it is easy to see what changed
    RecomputeRealityMapping();
  }

  if (pSource!=NULL)
  {
	  if (bNewLines)
		UpdateViews (pSource, &context, UPDATE_HORZRANGE | UPDATE_VERTRANGE, nLine);
	  else
		UpdateViews (pSource, &context, UPDATE_SINGLELINE | UPDATE_HORZRANGE, nLine);
  }

  if (!m_bModified)
    SetModified (TRUE);
	//BEGIN SW
	// remember current cursor position as last editing position
	m_ptLastChange = context.m_ptEnd;
	//END SW
  return TRUE;
}

BOOL CCrystalTextBuffer::
CanUndo ()
{
  ASSERT (m_nUndoPosition >= 0 && m_nUndoPosition <= m_aUndoBuf.GetSize ());
  return m_nUndoPosition > 0;
}

BOOL CCrystalTextBuffer::
CanRedo ()
{
  ASSERT (m_nUndoPosition >= 0 && m_nUndoPosition <= m_aUndoBuf.GetSize ());
  return m_nUndoPosition < m_aUndoBuf.GetSize ();
}

POSITION CCrystalTextBuffer::
GetUndoDescription (CString & desc, POSITION pos /*= NULL*/ )
{
  ASSERT (CanUndo ());          //  Please call CanUndo() first

  ASSERT ((m_aUndoBuf[0].m_dwFlags & UNDO_BEGINGROUP) != 0);

  int nPosition;
  if (pos == NULL)
    {
      //  Start from beginning
      nPosition = m_nUndoPosition;
    }
  else
    {
      nPosition = (int) pos;
      ASSERT (nPosition > 0 && nPosition < m_nUndoPosition);
      ASSERT ((m_aUndoBuf[nPosition].m_dwFlags & UNDO_BEGINGROUP) != 0);
    }

  //  Advance to next undo group
  nPosition--;
  while ((m_aUndoBuf[nPosition].m_dwFlags & UNDO_BEGINGROUP) == 0)
    nPosition--;

  //  Read description
  if (!GetActionDescription (m_aUndoBuf[nPosition].m_nAction, desc))
    desc.Empty ();              //  Use empty string as description

  //  Now, if we stop at zero position, this will be the last action,
  //  since we return (POSITION) nPosition
  return (POSITION) nPosition;
}

POSITION CCrystalTextBuffer::
GetRedoDescription (CString & desc, POSITION pos /*= NULL*/ )
{
  ASSERT (CanRedo ());          //  Please call CanRedo() before!

  ASSERT ((m_aUndoBuf[0].m_dwFlags & UNDO_BEGINGROUP) != 0);
  ASSERT ((m_aUndoBuf[m_nUndoPosition].m_dwFlags & UNDO_BEGINGROUP) != 0);

  int nPosition;
  if (pos == NULL)
    {
      //  Start from beginning
      nPosition = m_nUndoPosition;
    }
  else
    {
      nPosition = (int) pos;
      ASSERT (nPosition > m_nUndoPosition);
      ASSERT ((m_aUndoBuf[nPosition].m_dwFlags & UNDO_BEGINGROUP) != 0);
    }

  //  Read description
  if (!GetActionDescription (m_aUndoBuf[nPosition].m_nAction, desc))
    desc.Empty ();              //  Use empty string as description

  //  Advance to next undo group
  nPosition++;
  while (nPosition < m_aUndoBuf.GetSize () &&
        (m_aUndoBuf[nPosition].m_dwFlags & UNDO_BEGINGROUP) == 0)
    nPosition--;

  if (nPosition >= m_aUndoBuf.GetSize ())
    return NULL;                //  No more redo actions!

  return (POSITION) nPosition;
}

BOOL CCrystalTextBuffer::
Undo (CPoint & ptCursorPos)
{
  ASSERT (CanUndo ());
  ASSERT ((m_aUndoBuf[0].m_dwFlags & UNDO_BEGINGROUP) != 0);
  BOOL failed = FALSE;
  int tmpPos = m_nUndoPosition;

  while (!failed)
    {
      tmpPos--;;
      SUndoRecord ur = m_aUndoBuf[tmpPos];
      // Undo records are stored in file line numbers
      // and must be converted to apparent (screen) line numbers for use
      int nEndRealLine = ur.m_ptEndPos.y;
      ur.m_ptStartPos.y = ComputeApparentLine(ur.m_ptStartPos.y);
      ur.m_ptEndPos.y = ComputeApparentLine(ur.m_ptEndPos.y);
      if (ur.m_dwFlags & UNDO_INSERT)
        {
#if 0 //original code
#ifdef _ADVANCED_BUGCHECK
          //  Try to ensure that we undoing correctly...
          //  Just compare the text as it was before Undo operation
          CString text;
          GetText (ur.m_ptStartPos.y, ur.m_ptStartPos.x, ur.m_ptEndPos.y, ur.m_ptEndPos.x, text);
          ASSERT (lstrcmp (text, ur.GetText ()) == 0);
#endif
#endif // original code

          // WINMERGE -- Check that text in undo buffer matches text in
          // file buffer.  If not, then rescan() has moved lines and undo
          // is skipped.
          CString text;
          if ((ur.m_ptStartPos.y < m_aLines.GetSize ()) &&
              (ur.m_ptStartPos.x <= m_aLines[ur.m_ptStartPos.y].m_nLength) &&
              (ur.m_ptEndPos.y < m_aLines.GetSize ()) &&
              (ur.m_ptEndPos.x <= m_aLines[ur.m_ptEndPos.y].m_nLength))
            {
              GetText (ur.m_ptStartPos.y, ur.m_ptStartPos.x, ur.m_ptEndPos.y, ur.m_ptEndPos.x, text);
              if (_tcscmp(text, ur.GetText()) == 0)
                {
                   VERIFY (InternalDeleteText (NULL, ur.m_ptStartPos.y, ur.m_ptStartPos.x, ur.m_ptEndPos.y, ur.m_ptEndPos.x));
                      ptCursorPos = ur.m_ptStartPos;
                }
              else
                   failed = TRUE;
            }
          else
              failed = TRUE;
        }
      else
        {
          int nEndLine, nEndChar;
          VERIFY (InternalInsertText (NULL, ur.m_ptStartPos.y, ur.m_ptStartPos.x, ur.GetText (), nEndLine, nEndChar));
#ifdef _ADVANCED_BUGCHECK
          ASSERT (nEndRealLine == ComputeRealLine(nEndLine));
          ASSERT (ur.m_ptEndPos.x == nEndChar);
#endif
          ptCursorPos = ur.m_ptEndPos;
        }
      if (ur.m_dwFlags & UNDO_BEGINGROUP)
        break;
    }
  if (m_bModified && m_nSyncPosition == tmpPos)
    SetModified (FALSE);
  if (!m_bModified && m_nSyncPosition != tmpPos)
    SetModified (TRUE);
  if (failed)
    {
      // If the Undo failed, clear the entire Undo/Redo stack
      // Not only can we not Redo the failed Undo, but the Undo
      // may have partially completed (if in a group)
      m_nUndoPosition = 0;
      m_aUndoBuf.SetSize (m_nUndoPosition);
    }
  else
    {
      m_nUndoPosition = tmpPos;
    }
  return !failed;
}

BOOL CCrystalTextBuffer::
Redo (CPoint & ptCursorPos)
{
  ASSERT (CanRedo ());
  ASSERT ((m_aUndoBuf[0].m_dwFlags & UNDO_BEGINGROUP) != 0);
  ASSERT ((m_aUndoBuf[m_nUndoPosition].m_dwFlags & UNDO_BEGINGROUP) != 0);
  for (;;)
    {
      SUndoRecord ur = m_aUndoBuf[m_nUndoPosition];
      ur.m_ptStartPos.y = ComputeApparentLine(ur.m_ptStartPos.y);
      ur.m_ptEndPos.y = ComputeApparentLine(ur.m_ptEndPos.y);
      if (ur.m_dwFlags & UNDO_INSERT)
        {
          int nEndLine, nEndChar;
          VERIFY (InternalInsertText (NULL, ur.m_ptStartPos.y, ur.m_ptStartPos.x, ur.GetText (), nEndLine, nEndChar));
#ifdef _ADVANCED_BUGCHECK
          ASSERT (ur.m_ptEndPos.y == nEndLine);
          ASSERT (ur.m_ptEndPos.x == nEndChar);
#endif
          ptCursorPos = ur.m_ptEndPos;
        }
      else
        {
#ifdef _ADVANCED_BUGCHECK
          CString text;
          GetText (ur.m_ptStartPos.y, ur.m_ptStartPos.x, ur.m_ptEndPos.y, ur.m_ptEndPos.x, text);
          ASSERT (lstrcmp (text, ur.GetText ()) == 0);
#endif
          VERIFY (InternalDeleteText (NULL, ur.m_ptStartPos.y, ur.m_ptStartPos.x, ur.m_ptEndPos.y, ur.m_ptEndPos.x));
          ptCursorPos = ur.m_ptStartPos;
        }
      m_nUndoPosition++;
      if (m_nUndoPosition == m_aUndoBuf.GetSize ())
        break;
      if ((m_aUndoBuf[m_nUndoPosition].m_dwFlags & UNDO_BEGINGROUP) != 0)
        break;
    }
  if (m_bModified && m_nSyncPosition == m_nUndoPosition)
    SetModified (FALSE);
  if (!m_bModified && m_nSyncPosition != m_nUndoPosition)
    SetModified (TRUE);
  return TRUE;
}

//  [JRT] Support For Descriptions On Undo/Redo Actions
void CCrystalTextBuffer::
AddUndoRecord (BOOL bInsert, const CPoint & ptStartPos, const CPoint & ptEndPos, LPCTSTR pszText, int nActionType)
{
  //  Forgot to call BeginUndoGroup()?
  ASSERT (m_bUndoGroup);
  ASSERT (m_aUndoBuf.GetSize () == 0 || (m_aUndoBuf[0].m_dwFlags & UNDO_BEGINGROUP) != 0);

  //  Strip unnecessary undo records (edit after undo wipes all potential redo records)
  int nBufSize = m_aUndoBuf.GetSize ();
  if (m_nUndoPosition < nBufSize)
    {
      m_aUndoBuf.SetSize (m_nUndoPosition);
    }

  //  If undo buffer size is close to critical, remove the oldest records
  ASSERT (m_aUndoBuf.GetSize () <= m_nUndoBufSize);
  nBufSize = m_aUndoBuf.GetSize ();
  if (nBufSize >= m_nUndoBufSize)
    {
      int nIndex = 0;
      for (;;)
        {
          nIndex++;
          if (nIndex == nBufSize || (m_aUndoBuf[nIndex].m_dwFlags & UNDO_BEGINGROUP) != 0)
            break;
        }
      m_aUndoBuf.RemoveAt (0, nIndex);
    }
  ASSERT (m_aUndoBuf.GetSize () < m_nUndoBufSize);

  //  Add new record
  SUndoRecord ur;
  ur.m_dwFlags = bInsert ? UNDO_INSERT : 0;
  ur.m_nAction = nActionType;
  if (m_bUndoBeginGroup)
    {
      ur.m_dwFlags |= UNDO_BEGINGROUP;
      m_bUndoBeginGroup = FALSE;
    }
  ur.m_ptStartPos = ptStartPos;
  ur.m_ptEndPos = ptEndPos;
  ur.SetText (pszText);

  m_aUndoBuf.Add (ur);
  m_nUndoPosition = m_aUndoBuf.GetSize ();

  ASSERT (m_aUndoBuf.GetSize () <= m_nUndoBufSize);
}

static BOOL HasEol(LPCTSTR szText)
{
  int len = _tcslen(szText);
  return (len && iseol(szText[len-1]));
}

LPCTSTR CCrystalTextBuffer::GetDefaultEol() const
{
  switch(m_nCRLFMode)
  {
  case CRLF_STYLE_DOS: return _T("\r\n");
  case CRLF_STYLE_UNIX: return _T("\n");
  case CRLF_STYLE_MAC: return _T("\r");
  default: return _T("\r\n");
  }
}

BOOL CCrystalTextBuffer::
InsertText (CCrystalTextView * pSource, int nLine, int nPos, LPCTSTR pszText,
            int &nEndLine, int &nEndChar, int nAction, BOOL bUpdate /*=TRUE*/)
{
  // Are we inserting into a ghost line without EOL ?
  if (!GetFullLineLength(nLine) && !HasEol(pszText))
    {
      // Is there any text after this line ?
      if (m_RealityBlocks.GetSize() && m_RealityBlocks[m_RealityBlocks.GetUpperBound()].nStartApparent > nLine)
        {
          // terminate line with default EOL
          CString str = pszText;
          str += GetDefaultEol();
          return InsertText(pSource, nLine, nPos, str, nEndLine, nEndChar, nAction, bUpdate);
        }
    }

  int nRealStart = ComputeRealLine(nLine);
  if (!InternalInsertText (bUpdate? pSource:NULL, nLine, nPos, pszText, nEndLine, nEndChar))
    return FALSE;
  int nRealEnd = ComputeRealLine(nEndLine);

  BOOL bGroupFlag = FALSE;
  if (!m_bUndoGroup)
    {
      BeginUndoGroup ();
      bGroupFlag = TRUE;
    }
  AddUndoRecord (TRUE, CPoint (nPos, nRealStart), CPoint (nEndChar, nRealEnd), pszText, nAction);
  if (bGroupFlag)
    FlushUndoGroup (pSource);
  return TRUE;
}

BOOL CCrystalTextBuffer::
DeleteText (CCrystalTextView * pSource, int nStartLine, int nStartChar,
            int nEndLine, int nEndChar, int nAction, BOOL bUpdate /*=TRUE*/)
{
  CString sTextToDelete;
  GetText (nStartLine, nStartChar, nEndLine, nEndChar, sTextToDelete);

  int nRealStart=0, nRealEnd=0;
  if (!InternalDeleteText (bUpdate? pSource:NULL, nStartLine, nStartChar, nEndLine, nEndChar, &nRealStart, &nRealEnd))
    return FALSE;

  BOOL bGroupFlag = FALSE;
  if (!m_bUndoGroup)
    {
      BeginUndoGroup ();
      bGroupFlag = TRUE;
    }
  AddUndoRecord (FALSE, CPoint (nStartChar, nRealStart), CPoint (nEndChar, nRealEnd), sTextToDelete, nAction);
  if (bGroupFlag)
    FlushUndoGroup (pSource);
  return TRUE;
}

BOOL CCrystalTextBuffer::
GetActionDescription (int nAction, CString & desc)
{
  HINSTANCE hOldResHandle = AfxGetResourceHandle ();
#ifdef CRYSEDIT_RES_HANDLE
  AfxSetResourceHandle (CRYSEDIT_RES_HANDLE);
#else
  if (CCrystalTextView::s_hResourceInst != NULL)
    AfxSetResourceHandle (CCrystalTextView::s_hResourceInst);
#endif
  BOOL bSuccess = FALSE;
  switch (nAction)
    {
    case CE_ACTION_PASTE:
      bSuccess = desc.LoadString (IDS_EDITOP_PASTE);
      break;
    case CE_ACTION_DELSEL:
      bSuccess = desc.LoadString (IDS_EDITOP_DELSELECTION);
      break;
    case CE_ACTION_CUT:
      bSuccess = desc.LoadString (IDS_EDITOP_CUT);
      break;
    case CE_ACTION_TYPING:
      bSuccess = desc.LoadString (IDS_EDITOP_TYPING);
      break;
    case CE_ACTION_BACKSPACE:
      bSuccess = desc.LoadString (IDS_EDITOP_BACKSPACE);
      break;
    case CE_ACTION_INDENT:
      bSuccess = desc.LoadString (IDS_EDITOP_INDENT);
      break;
    case CE_ACTION_DRAGDROP:
      bSuccess = desc.LoadString (IDS_EDITOP_DRAGDROP);
      break;
    case CE_ACTION_REPLACE:
      bSuccess = desc.LoadString (IDS_EDITOP_REPLACE);
      break;
    case CE_ACTION_DELETE:
      bSuccess = desc.LoadString (IDS_EDITOP_DELETE);
      break;
    case CE_ACTION_AUTOINDENT:
      bSuccess = desc.LoadString (IDS_EDITOP_AUTOINDENT);
      break;
    case CE_ACTION_AUTOCOMPLETE:
      bSuccess = desc.LoadString (IDS_EDITOP_AUTOCOMPLETE);
      break;
    case CE_ACTION_AUTOEXPAND:
      bSuccess = desc.LoadString (IDS_EDITOP_AUTOEXPAND);
      break;
    case CE_ACTION_LOWERCASE:
      bSuccess = desc.LoadString (IDS_EDITOP_LOWERCASE);
      break;
    case CE_ACTION_UPPERCASE:
      bSuccess = desc.LoadString (IDS_EDITOP_UPPERCASE);
      break;
    case CE_ACTION_SWAPCASE:
      bSuccess = desc.LoadString (IDS_EDITOP_SWAPCASE);
      break;
    case CE_ACTION_CAPITALIZE:
      bSuccess = desc.LoadString (IDS_EDITOP_CAPITALIZE);
      break;
    case CE_ACTION_SENTENCIZE:
      bSuccess = desc.LoadString (IDS_EDITOP_SENTENCIZE);
      break;
    case CE_ACTION_RECODE:
      bSuccess = desc.LoadString (IDS_EDITOP_RECODE);
      break;
    case CE_ACTION_SPELL:
      bSuccess = desc.LoadString (IDS_EDITOP_SPELL);
      break;
    default: /* case CE_ACTION_UNKNOWN: */
      bSuccess = desc.LoadString (IDS_EDITOP_UNKNOWN);
    }
  AfxSetResourceHandle (hOldResHandle);
  return bSuccess;
}

void CCrystalTextBuffer::
SetModified (BOOL bModified /*= TRUE*/ )
{
  m_bModified = bModified;
}

void CCrystalTextBuffer::
BeginUndoGroup (BOOL bMergeWithPrevious /*= FALSE*/ )
{
  ASSERT (!m_bUndoGroup);
  m_bUndoGroup = TRUE;
  m_bUndoBeginGroup = m_nUndoPosition == 0 || !bMergeWithPrevious;
}

void CCrystalTextBuffer::
FlushUndoGroup (CCrystalTextView * pSource)
{
  ASSERT (m_bUndoGroup);
  if (pSource != NULL)
    {
      ASSERT (m_nUndoPosition == m_aUndoBuf.GetSize ());
      if (m_nUndoPosition > 0)
        {
          pSource->OnEditOperation (m_aUndoBuf[m_nUndoPosition - 1].m_nAction, m_aUndoBuf[m_nUndoPosition - 1].GetText ());
        }
    }
  m_bUndoGroup = FALSE;
}

int CCrystalTextBuffer::
FindNextBookmarkLine (int nCurrentLine)
{
  BOOL bWrapIt = TRUE;
  DWORD dwFlags = GetLineFlags (nCurrentLine);
  if ((dwFlags & LF_BOOKMARKS) != 0)
    nCurrentLine++;

  int nSize = m_aLines.GetSize ();
  for (;;)
    {
      while (nCurrentLine < nSize)
        {
          if ((m_aLines[nCurrentLine].m_dwFlags & LF_BOOKMARKS) != 0)
            return nCurrentLine;
          // Keep going
          nCurrentLine++;
        }
      // End of text reached
      if (!bWrapIt)
        return -1;

      // Start from the beginning of text
      bWrapIt = FALSE;
      nCurrentLine = 0;
    }
  return -1;
}

int CCrystalTextBuffer::
FindPrevBookmarkLine (int nCurrentLine)
{
  BOOL bWrapIt = TRUE;
  DWORD dwFlags = GetLineFlags (nCurrentLine);
  if ((dwFlags & LF_BOOKMARKS) != 0)
    nCurrentLine--;

  int nSize = m_aLines.GetSize ();
  for (;;)
    {
      while (nCurrentLine >= 0)
        {
          if ((m_aLines[nCurrentLine].m_dwFlags & LF_BOOKMARKS) != 0)
            return nCurrentLine;
          // Keep moving up
          nCurrentLine--;
        }
      // Beginning of text reached
      if (!bWrapIt)
        return -1;

      // Start from the end of text
      bWrapIt = FALSE;
      nCurrentLine = nSize - 1;
    }
  return -1;
}

BOOL CCrystalTextBuffer::
IsMBSLead (int nLine, int nCol)
{
  ASSERT (m_bInit);             //  Text buffer not yet initialized.
  //  You must call InitNew() or LoadFromFile() first!

#ifdef _UNICODE
  LPCTSTR string = (LPCTSTR) GetLineChars (nLine);
  LPCTSTR current = string + nCol;
  return FALSE;
#else // _UNICODE
  const unsigned char *string = (const unsigned char *) GetLineChars (nLine);
  const unsigned char *current = string + nCol;
  if (_ismbslead (string, current) < 0)
    return TRUE;
  return FALSE;
#endif // _UNICODE
}

BOOL CCrystalTextBuffer::
IsMBSTrail (int nLine, int nCol)
{
  ASSERT (m_bInit);             //  Text buffer not yet initialized.
  //  You must call InitNew() or LoadFromFile() first!

#ifdef _UNICODE
  LPCTSTR string = (LPCTSTR) GetLineChars (nLine);
  LPCTSTR current = string + nCol;
  return FALSE;
#else // _UNICODE
  const unsigned char *string = (const unsigned char *) GetLineChars (nLine);
  const unsigned char *current = string + nCol;
  if (_ismbstrail (string, current) < 0)
    return TRUE;
  return FALSE;
#endif // _UNICODE
}

//BEGIN SW
CPoint CCrystalTextBuffer::GetLastChangePos() const
{
	return m_ptLastChange;
}
//END SW

void CCrystalTextBuffer::DeleteLine(int line)
{
  delete[] m_aLines[line].m_pcLine;
  m_aLines.RemoveAt(line);
}

int CCrystalTextBuffer::GetTabSize()
{
  ASSERT( m_nTabSize >= 0 && m_nTabSize <= 64 );
  return m_nTabSize;
}

void CCrystalTextBuffer::SetTabSize(int nTabSize)
{
  ASSERT( nTabSize >= 0 && nTabSize <= 64 );
  m_nTabSize = nTabSize;
}

// Return highest valid real (file) line
// Returns -1 if no lines
int CCrystalTextBuffer::LastRealLine() const
{
  int bmax = m_RealityBlocks.GetUpperBound();
  if (bmax<0) return -1;
  RealityBlock & block = m_RealityBlocks[bmax];
  return block.nStartReal + block.nCount;
}

// return underlying real line
// for ghost lines, return next higher real line
// ie, lines 0->0, 1->2, 2->4,
// for argument of 3, return 2
int CCrystalTextBuffer::ComputeRealLine(int nApparentLine) const
{
  int bmax = m_RealityBlocks.GetUpperBound();
  // first get the degenerate cases out of the way
  // empty file ?
  if (bmax<0)
    return 0;
  // after last block ?
  RealityBlock & maxblock = m_RealityBlocks[bmax];
  if (nApparentLine >= maxblock.nStartApparent + maxblock.nCount)
    return maxblock.nStartReal + maxblock.nCount;

  // binary search to find correct (or nearest block)
  int blo=0, bhi=bmax;
  int i;
  while (blo<=bhi)
    {
      i = (blo+bhi)/2;
      RealityBlock & block = m_RealityBlocks[i];
      if (nApparentLine < block.nStartApparent)
        bhi = i-1;
      else if (nApparentLine >= block.nStartApparent + block.nCount)
        blo = i+1;
      else // found it inside this block
        return (nApparentLine - block.nStartApparent) + block.nStartReal;
    }
  // it is a ghost line just before block blo
  return m_RealityBlocks[blo].nStartReal;
}

// return apparent line for this underlying real line
// if real line is out of bounds, returns -1
int CCrystalTextBuffer::ComputeApparentLine(int nRealLine) const
{
  int bmax = m_RealityBlocks.GetUpperBound();
  // first get the degenerate cases out of the way
  // empty file ?
  if (bmax<0)
    return 0;
  // after last block ?
  // (This can happen with the endline of a paste undo record)
  RealityBlock & maxblock = m_RealityBlocks[bmax];
  if (nRealLine >= maxblock.nStartReal + maxblock.nCount)
    return nRealLine - maxblock.nStartReal + maxblock.nStartApparent;

  // binary search to find correct (or nearest block)
  int blo=0, bhi=bmax;
  int i;
  while (blo<=bhi)
    {
      i = (blo+bhi)/2;
      RealityBlock & block = m_RealityBlocks[i];
      if (nRealLine < block.nStartReal)
        bhi = i-1;
      else if (nRealLine >= block.nStartReal + block.nCount)
        blo = i+1;
      else
        return (nRealLine - block.nStartReal) + block.nStartApparent;
    }
  // Should have found it; all real lines should be in a block
  ASSERT(0);
  return -1;
}

// Do what we need to do just after we've been reloaded
void CCrystalTextBuffer::FinishLoading()
{
  if (!m_bInit) return;
  RecomputeRealityMapping();
}

// Recompute the reality mapping (this is fairly naive)
void CCrystalTextBuffer::RecomputeRealityMapping()
{
  m_RealityBlocks.RemoveAll();
  int reality=-1; // last encountered real line
  int i=0; // current line
  RealityBlock block; // current block being traversed (in state 2)

  // This is a state machine with 2 states

  // state 1, i-1 not real line
passingGhosts:
  if (i==GetLineCount())
    return;
  if (!GetFullLineLength(i))
    {
      ++i;
      goto passingGhosts;
    }
  // this is the first line of a reality block
  block.nStartApparent = i;
  block.nStartReal = reality+1;
  ++reality;
  ++i;
  // fall through to other state

  // state 2, i-1 is real line
inReality:
  if (i==GetLineCount() || !GetFullLineLength(i))
    {
      // i-1 is the last line of a reality block
      ASSERT(reality >= 0);
      block.nCount = i - block.nStartApparent;
      ASSERT(block.nCount > 0);
      ASSERT(reality+1-block.nStartReal == block.nCount);
      m_RealityBlocks.Add(block);
      if (i==GetLineCount())
        return;
      ++i;
      goto passingGhosts;
    }
  ++reality;
  ++i;
  goto inReality;
}
