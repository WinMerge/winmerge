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
//  ??-Aug-99
//      Sven Wiegand (search for "//BEGIN SW" to find my changes):
//  + FEATURE: Remembering the text-position of the latest change.
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//  24-Oct-99
//      Sven Wiegand
//  + FIX: Setting m_ptLastChange to the beginning of the selection in
//           InternalDeleteText(), so that position is valid in any case.
//           Editor won't crash any more i.e. by selecting whole buffer and
//           deleting it and then executing ID_EDIT_GOTO_LAST_CHANGE-command.
////////////////////////////////////////////////////////////////////////////
/** 
 * @file ccrystaltextbuffer.cpp
 *
 * @brief Code for CCrystalTextBuffer class
 */

#include "StdAfx.h"
#include "ccrystaltextbuffer.h"
#include "ccrystaltextview.h"
#include "editcmd.h"
#include "LineInfo.h"
#include "UndoRecord.h"
#include "utils/filesup.h"
#include "utils/cs2cs.h"
#include <vector>
#include <malloc.h>

#ifndef __AFXPRIV_H__
#pragma message("Include <afxpriv.h> in your stdafx.h to avoid this message")
#include <afxpriv.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using std::vector;

int CCrystalTextBuffer::m_nDefaultEncoding = -1;


/////////////////////////////////////////////////////////////////////////////
// CCrystalTextBuffer::CUpdateContext

void CCrystalTextBuffer::CInsertContext::
RecalcPoint (CEPoint & ptPoint)
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
RecalcPoint (CEPoint & ptPoint)
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

CCrystalTextBuffer::CCrystalTextBuffer ()
{
  m_bInit = false;
  m_bReadOnly = false;
  m_bModified = false;
  m_nCRLFMode = CRLFSTYLE::DOS;
  m_IgnoreEol = false;
  m_bCreateBackupFile = false;
  m_nSyncPosition = m_nUndoPosition = 0;
  m_bInsertTabs = true;
  m_nTabSize = 4;
  //BEGIN SW
  m_ptLastChange.x = m_ptLastChange.y = -1;
  //END SW
  m_nSourceEncoding = m_nDefaultEncoding;
  m_dwCurrentRevisionNumber = 0;
  m_dwRevisionNumberOnSave = 0;
  m_bUndoGroup = m_bUndoBeginGroup = false;

  // Table Editing
  m_bAllowNewlinesInQuotes = true;
  m_cFieldDelimiter = '\t';
  m_cFieldEnclosure = '"';
  m_bTableEditing = false;
  m_pSharedTableProps.reset(new SharedTableProperties());
  m_pSharedTableProps->m_textBufferList.push_back(this);
}

CCrystalTextBuffer:: ~ CCrystalTextBuffer ()
{
  ASSERT (!m_bInit);            //  You must call FreeAll() before deleting the object
}

/////////////////////////////////////////////////////////////////////////////
// CCrystalTextBuffer message handlers

/** 
 * @brief Insert the same line once or several times
 *
 * @param nPosition : not defined (or -1) = add lines at the end of array
 */
void CCrystalTextBuffer::InsertLine (const tchar_t* pszLine, size_t nLength,
    int nPosition /*= -1*/, int nCount /*= 1*/ )
{
  ASSERT(nLength != -1);

  LineInfo line{ pszLine, nLength };

  // nPosition not defined ? Insert at end of array
  if (nPosition == -1)
    nPosition = (int) m_aLines.size();

  // insert all lines in one pass
  std::vector<LineInfo>::iterator iter = m_aLines.begin() + nPosition;
  m_aLines.insert(iter, nCount, line);

  // create text data for lines after the first one
  for (int ic = 1; ic < nCount; ic++)
    {
      m_aLines[nPosition + ic] = std::move(LineInfo{ pszLine, nLength });
    }

#ifdef _DEBUG
  // Warning : this function is also used during rescan
  // and this trace will appear even after the initial load
  int nLines = (int) m_aLines.size();
  if (nLines / 5000 != (nLines-nCount) / 5000)
    TRACE1 ("%d lines loaded!\n", nLines);
#endif
}

// Add characters to end of specified line
// Specified line must not have any EOL characters
void CCrystalTextBuffer::
AppendLine (int nLineIndex, const tchar_t* pszChars, size_t nLength, bool bDetectEol)
{
  ASSERT(nLength != -1);

  if (nLength == 0)
    return;

  LineInfo & li = m_aLines[nLineIndex];
  li.Append(pszChars, nLength, bDetectEol);
}

/**
 * @brief Copy line range [line1;line2] to range starting at newline1
 *
 * NB: Lines are assigned, not inserted
 *
 * Example#1:
 *   MoveLine(5,7,100)
 *    line1=5
 *    line2=10
 *    newline1=100
 *    ldiff=95
 *     l=10  lines[105] = lines[10]
 *     l=9   lines[104] = lines[9]
 *     l=8   lines[103] = lines[8]
 *
 * Example#2:
 *   MoveLine(40,42,10)
 *    line1=40
 *    line2=42
 *    newline1=10
 *    ldiff=-30
 *     l=40  lines[10] = lines[40]
 *     l=41  lines[11] = lines[41]
 *     l=42  lines[12] = lines[42]
 */
void CCrystalTextBuffer::MoveLine(int line1, int line2, int newline1)
{
  int ldiff = newline1 - line1;
  if (ldiff > 0)
    {
      for (int l = line2; l >= line1; l--)
        m_aLines[l+ldiff] = std::move(m_aLines[l]);
    }
  else if (ldiff < 0)
    {
      for (int l = line1; l <= line2; l++)
        m_aLines[l+ldiff] = std::move(m_aLines[l]);
    }
}

void CCrystalTextBuffer::SetEmptyLine (int nPosition, int nCount /*= 1*/ )
{
  for (int i = 0; i < nCount; i++) 
    {
      m_aLines[nPosition + i] = std::move(LineInfo{ nullptr, 0 });
    }
}

void CCrystalTextBuffer::
FreeAll ()
{
  //  Free text
  std::vector<LineInfo>::iterator iter = m_aLines.begin();
  std::vector<LineInfo>::iterator end = m_aLines.end();
  while (iter != end)
    {
      (*iter).Clear();
      ++iter;
    }
  m_aLines.clear();

  // Undo buffer will be cleared by its destructor

  m_bInit = false;
  //BEGIN SW
  m_ptLastChange.x = m_ptLastChange.y = -1;
  //END SW
}

bool CCrystalTextBuffer::
InitNew (CRLFSTYLE nCrlfStyle /*= CRLFSTYLE::DOS*/ )
{
  ASSERT (!m_bInit);
  ASSERT (m_aLines.size() == 0);
  ASSERT (nCrlfStyle != CRLFSTYLE::AUTOMATIC && nCrlfStyle != CRLFSTYLE::MIXED);
  InsertLine (_T (""), 0);
  m_bInit = true;
  m_bReadOnly = false;
  m_nCRLFMode = nCrlfStyle;
  m_bModified = false;
  m_bInsertTabs	= true;
  m_nTabSize = 4;
  m_nSyncPosition = m_nUndoPosition = 0;
  m_bUndoGroup = m_bUndoBeginGroup = false;
  ASSERT (m_aUndoBuf.size () == 0);
  UpdateViews (nullptr, nullptr, UPDATE_RESET);
  //BEGIN SW
  m_ptLastChange.x = m_ptLastChange.y = -1;
  //END SW
  return true;
}

bool CCrystalTextBuffer::
GetReadOnly ()
const
{
  ASSERT (m_bInit);        //  Text buffer not yet initialized.
  //  You must call InitNew() or LoadFromFile() first!

  return m_bReadOnly;
}

void CCrystalTextBuffer::SetReadOnly (bool bReadOnly /*= true*/ )
{
  ASSERT (m_bInit);             //  Text buffer not yet initialized.
  //  You must call InitNew() or LoadFromFile() first!

  m_bReadOnly = bReadOnly;
}


// WinMerge has own routine for loading
#ifdef CRYSTALEDIT_ENABLELOADER

static const tchar_t* crlfs[] =
{
  _T ("\x0d\x0a"), //  DOS/Windows style
  _T ("\x0a"),     //  UNIX style
  _T ("\x0a")      //  Macintosh style
};

bool CCrystalTextBuffer::
LoadFromFile (const tchar_t* pszFileName, CRLFSTYLE nCrlfStyle /*= CRLFSTYLE::AUTOMATIC*/ )
{
  ASSERT (!m_bInit);
  ASSERT (m_aLines.size() == 0);

  HANDLE hFile = nullptr;
  int nCurrentMax = 256;
  char *pcLineBuf = new char[nCurrentMax];

  bool bSuccess = false;

  int nExt = GetExtPosition (pszFileName);
  if (pszFileName[nExt] == _T ('.'))
    nExt++;
  CrystalLineParser::TextDefinition *def = CrystalLineParser::GetTextType (pszFileName + nExt);
  if (def && def->encoding != -1)
    m_nSourceEncoding = def->encoding;

  __try
    {
      DWORD dwFileAttributes =::GetFileAttributes (pszFileName);
      if (dwFileAttributes == (DWORD) - 1)
        __leave;

      hFile =::CreateFile (pszFileName, GENERIC_READ, FILE_SHARE_READ + FILE_SHARE_WRITE, nullptr,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
      if (hFile == INVALID_HANDLE_VALUE)
        __leave;

      int nCurrentLength = 0;

      const DWORD dwBufSize = 32768;
      LPSTR pcBuf = (LPSTR) _alloca (dwBufSize);
      DWORD dwCurSize;
      if (!::ReadFile (hFile, pcBuf, dwBufSize, &dwCurSize, nullptr))
        __leave;

      if (nCrlfStyle == CRLFSTYLE::AUTOMATIC)
        {
          //  Try to determine current CRLF mode based on first line
          DWORD I;
          for (I = 0; I < dwCurSize; I++)
            {
              if ((pcBuf[I] == _T('\x0d')) || (pcBuf[I] == _T('\x0a')))
                break;
            }
          if (I == dwCurSize)
            {
              //  By default (or in the case of empty file), set DOS style
              nCrlfStyle = CRLFSTYLE::DOS;
            }
          else
            {
              //  Otherwise, analyse the first occurance of line-feed character
              if (pcBuf[I] == _T('\x0a'))
                {
                  nCrlfStyle = CRLFSTYLE::UNIX;
                }
              else
                {
                  if (I < dwCurSize - 1 && pcBuf[I + 1] == _T ('\x0a'))
                    nCrlfStyle = CRLFSTYLE::DOS;
                  else
                    nCrlfStyle = CRLFSTYLE::MAC;
                }
            }
        }

      ASSERT (nCrlfStyle != CRLFSTYLE::AUTOMATIC && nCrlfStyle != CRLFSTYLE::MIXED);
      m_nCRLFMode = nCrlfStyle;

      m_aLines.reserve(4096);

      DWORD dwBufPtr = 0;
      while (dwBufPtr < dwCurSize)
        {
          char c = pcBuf[dwBufPtr];
          dwBufPtr++;
          if (dwBufPtr == dwCurSize && dwCurSize == dwBufSize)
            {
              if (!::ReadFile (hFile, pcBuf, dwBufSize, &dwCurSize, nullptr))
                __leave;
              dwBufPtr = 0;
            }

          pcLineBuf[nCurrentLength] = c;
          nCurrentLength++;
          if (nCurrentLength == nCurrentMax)
            {
              //  Reallocate line buffer
              nCurrentMax += 256;
              char *pcNewLineBuf = new char[nCurrentMax];
              memcpy(pcNewLineBuf, pcLineBuf, sizeof(char) * (nCurrentMax - 256));
              delete [] pcLineBuf;
              pcLineBuf = pcNewLineBuf;
            }

          // detect both types of EOL for each line
          // handles mixed mode files.
          // Perry (2002-11-26): What about MAC files ? They don't have 0x0A at all. I think this doesn't handle them.
          if( c==0x0A )
            {
              pcLineBuf[nCurrentLength] = '\0';
#ifdef _UNICODE
              wchar_t *buf = new wchar_t[nCurrentLength];
              int len = MultiByteToWideChar(CP_UTF8, 0, pcLineBuf, nCurrentLength, buf, nCurrentLength);
              if (m_nSourceEncoding >= 0)
                iconvert(buf, m_nSourceEncoding, 1, m_nSourceEncoding == 15);
              InsertLine(buf, len);
              delete[] buf;
#else
              if (m_nSourceEncoding >= 0)
                iconvert (pcLineBuf, m_nSourceEncoding, 1, m_nSourceEncoding == 15);
              InsertLine (pcLineBuf, nCurrentLength);
#endif
              nCurrentLength = 0;
            }
        }

      pcLineBuf[nCurrentLength] = 0;
#ifdef _UNICODE
      wchar_t *buf = new wchar_t[nCurrentLength];
      int len = MultiByteToWideChar(CP_UTF8, 0, pcLineBuf, nCurrentLength, buf, nCurrentLength);
      InsertLine(buf, len);
      delete[] buf;
#else
      InsertLine (&pcLineBuf[0], nCurrentLength);
#endif

      ASSERT (m_aLines.size() > 0);   //  At least one empty line must present

      m_bInit = true;
      m_bReadOnly = (dwFileAttributes & FILE_ATTRIBUTE_READONLY) != 0;
      m_bModified = false;
      m_bUndoGroup = m_bUndoBeginGroup = false;
      m_nSyncPosition = m_nUndoPosition = 0;
      ASSERT (m_aUndoBuf.size () == 0);
      bSuccess = true;

      RetypeViews (pszFileName);
      UpdateViews (nullptr, nullptr, UPDATE_RESET);
    }
  __finally
    {
    }
  if (hFile != nullptr && hFile != INVALID_HANDLE_VALUE)
    ::CloseHandle (hFile);
  delete [] pcLineBuf;
  //BEGIN SW
  m_ptLastChange.x = m_ptLastChange.y = -1;
  //END SW
  return bSuccess;
}
#endif // #if 0 loadfromfile

// WinMerge has own routine for saving
#ifdef CRYSTALEDIT_ENABLESAVER
bool CCrystalTextBuffer::SaveToFile(const tchar_t* pszFileName,
                  CRLFSTYLE nCrlfStyle /*= CRLFSTYLE::AUTOMATIC*/,
                  bool bClearModifiedFlag /*= true*/)
{
  ASSERT (nCrlfStyle == CRLFSTYLE::AUTOMATIC || nCrlfStyle == CRLFSTYLE::DOS ||
          nCrlfStyle == CRLFSTYLE::UNIX || nCrlfStyle == CRLFSTYLE::MAC);
  ASSERT (m_bInit);
  HANDLE hTempFile = INVALID_HANDLE_VALUE;
  HANDLE hSearch = INVALID_HANDLE_VALUE;
  tchar_t szTempFileDir[_MAX_PATH + 1];
  tchar_t szTempFileName[_MAX_PATH + 1];
  tchar_t szBackupFileName[_MAX_PATH + 1];
  bool bSuccess = false;
  {
    tchar_t drive[_MAX_PATH], dir[_MAX_PATH], name[_MAX_PATH], ext[_MAX_PATH];
#ifdef _UNICODE
    _wsplitpath_s (pszFileName, drive, dir, name, ext);
#else
    _splitpath_s (pszFileName, drive, dir, name, ext);
#endif
    tc::tcslcpy (szTempFileDir, drive);
    tc::tcslcat (szTempFileDir, dir);
    tc::tcslcpy (szBackupFileName, pszFileName);
    tc::tcslcat (szBackupFileName, _T (".bak"));

    if (::GetTempFileName(szTempFileDir, _T("CRE"), 0, szTempFileName) == 0)
      goto EXIT;

    hTempFile =::CreateFile (szTempFileName, GENERIC_WRITE, 0, nullptr,
      CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hTempFile == INVALID_HANDLE_VALUE)
      goto EXIT;

    if (nCrlfStyle == CRLFSTYLE::AUTOMATIC)
      nCrlfStyle = m_nCRLFMode;

    ASSERT (nCrlfStyle != CRLFSTYLE::AUTOMATIC && nCrlfStyle != CRLFSTYLE::MIXED);
    const tchar_t* pszCRLF = crlfs[static_cast<int>(nCrlfStyle)];
    int nCRLFLength = static_cast<int>(tc::tcslen (pszCRLF));

    int nLineCount = static_cast<int>(m_aLines.size());
    for (int nLine = 0; nLine < nLineCount; nLine++)
      {
        int nLength = static_cast<int>(m_aLines[nLine].Length());
        DWORD dwWrittenBytes;
        if (nLength > 0)
          {
            const tchar_t* pszLine = m_aLines[nLine].GetLine(0);
            if (m_nSourceEncoding >= 0)
              {
                tchar_t* pszBuf;
                iconvert_new (m_aLines[nLine].GetLine(0), &pszBuf, 1, m_nSourceEncoding, m_nSourceEncoding == 15);
#ifdef _UNICODE
                std::unique_ptr<char> abuf{ new char[nLength * 4] };
                nLength = WideCharToMultiByte (CP_UTF8, 0, pszBuf, nLength, abuf.get(), nLength * 4, nullptr, nullptr);
                if (!::WriteFile (hTempFile, abuf.get(), nLength, &dwWrittenBytes, nullptr))
#else
                if (!::WriteFile (hTempFile, pszBuf, nLength, &dwWrittenBytes, nullptr))
#endif
                  {
                    free (pszBuf);
                    goto EXIT;
                  }
                free (pszBuf);
              }
            else
              {
#ifdef _UNICODE
                std::unique_ptr<char> abuf{ new char[nLength * 4] };
                nLength = WideCharToMultiByte (CP_UTF8, 0, pszLine, nLength, abuf.get(), nLength * 4, nullptr, nullptr);
                if (!::WriteFile (hTempFile, abuf.get(), nLength, &dwWrittenBytes, nullptr))
#else
                if (!::WriteFile (hTempFile, pszLine, nLength, &dwWrittenBytes, nullptr))
#endif
                  goto EXIT;
              }
            if (nLength != (int)dwWrittenBytes)
              goto EXIT;
          }
        if (nLine < nLineCount - 1)     //  Last line must not end with CRLF

          {
#ifdef _UNICODE
            std::unique_ptr<char> abuf{ new char[nCRLFLength * 4] };
            nCRLFLength = WideCharToMultiByte (CP_UTF8, 0, pszCRLF, nCRLFLength, abuf.get(), nCRLFLength * 4, nullptr, nullptr);
            if (!::WriteFile (hTempFile, abuf.get(), nCRLFLength, &dwWrittenBytes, nullptr))
#else
            if (!::WriteFile (hTempFile, pszCRLF, nCRLFLength, &dwWrittenBytes, nullptr))
#endif
              goto EXIT;
            if (nCRLFLength != (int) dwWrittenBytes)
              goto EXIT;
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
              goto EXIT;
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
      goto EXIT;

    if (bClearModifiedFlag)
      {
        SetModified (false);
        m_nSyncPosition = m_nUndoPosition;
      }
    bSuccess = true;

    // remember revision number on save
    m_dwRevisionNumberOnSave = m_dwCurrentRevisionNumber;

    // redraw line revision marks
    UpdateViews (nullptr, nullptr, UPDATE_FLAGSONLY);
  }
EXIT:
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

// Default EOL to use if editor has to manufacture one
// (this occurs with ghost lines)
void CCrystalTextBuffer::
SetCRLFMode (CRLFSTYLE nCRLFMode)
{
  if (nCRLFMode==CRLFSTYLE::AUTOMATIC)
    nCRLFMode = CRLFSTYLE::DOS;
  m_nCRLFMode = nCRLFMode;

  ASSERT(m_nCRLFMode == CRLFSTYLE::DOS || m_nCRLFMode == CRLFSTYLE::UNIX ||
      m_nCRLFMode == CRLFSTYLE::MAC || m_nCRLFMode == CRLFSTYLE::MIXED);
}

bool CCrystalTextBuffer::
applyEOLMode()
{
  const tchar_t* lpEOLtoApply = GetDefaultEol();
  bool bChanged = false;
  for (size_t i = 0 ; i < m_aLines.size(); i++)
    {
      // the last real line has no EOL
      if (!m_aLines[i].HasEol())
        continue;
      bChanged |= ChangeLineEol(static_cast<int>(i), lpEOLtoApply);
    }

  if (bChanged)
    SetModified(true);

  return bChanged;
}

int CCrystalTextBuffer::
GetLineCount () const
{
  ASSERT (m_bInit);             //  Text buffer not yet initialized.
  ASSERT (m_aLines.size() < INT_MAX);
  //  You must call InitNew() or LoadFromFile() first!

  return (int) m_aLines.size ();
}

// number of characters in line (excluding any trailing eol characters)
int CCrystalTextBuffer::
GetLineLength (int nLine) const
{
  ASSERT (m_bInit);             //  Text buffer not yet initialized.
  ASSERT (m_aLines[nLine].Length() < INT_MAX);
  //  You must call InitNew() or LoadFromFile() first!

  return static_cast<int>(m_aLines[nLine].Length());
}

// number of characters in line (including any trailing eol characters)
int CCrystalTextBuffer::
GetFullLineLength (int nLine) const
{
  ASSERT (m_bInit);             //  Text buffer not yet initialized.
  if (nLine >= static_cast<int>(m_aLines.size()))
    {
      ASSERT(false);
      return 0;
    }
  ASSERT (m_aLines[nLine].FullLength() < INT_MAX);
  //  You must call InitNew() or LoadFromFile() first!

  return static_cast<int>(m_aLines[nLine].FullLength());
}

// get pointer to any trailing eol characters (pointer to empty string if none)
const tchar_t* CCrystalTextBuffer::
GetLineEol (int nLine) const
{
  ASSERT (m_bInit);             //  Text buffer not yet initialized.
  if (m_aLines[nLine].HasEol())
    return m_aLines[nLine].GetEol();
  else
    return _T("");
}

bool CCrystalTextBuffer::
ChangeLineEol (int nLine, const tchar_t* lpEOL) 
{
  LineInfo & li = m_aLines[nLine];
  return li.ChangeEol(lpEOL);
}

const tchar_t* CCrystalTextBuffer::
GetLineChars (int nLine) const
{
  ASSERT (m_bInit);             //  Text buffer not yet initialized.
  //  You must call InitNew() or LoadFromFile() first!

  return m_aLines[nLine].GetLine();
}

lineflags_t CCrystalTextBuffer::
GetLineFlags (int nLine) const
{
  ASSERT (m_bInit);             //  Text buffer not yet initialized.
  //  You must call InitNew() or LoadFromFile() first!

  if (nLine < 0 || nLine >= static_cast<int>(m_aLines.size()))
    {
      ASSERT(false);      //  nLine is out of range.
      return 0;
    }

  return m_aLines[nLine].m_dwFlags;
}

/** 
 * @brief Get line revision number.
 *
 * @param nLine Index of the line to get the revision number.
 */
uint32_t CCrystalTextBuffer::
GetLineRevisionNumber (int nLine) const
{
  ASSERT (m_bInit);             //  Text buffer not yet initialized.
  //  You must call InitNew() or LoadFromFile() first!

  return m_aLines[nLine].m_dwRevisionNumber;
}

static int
FlagToIndex (lineflags_t dwFlag)
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
FindLineWithFlag (lineflags_t dwFlag) const
{
  const size_t nSize = m_aLines.size();
  for (size_t L = 0; L < nSize; L++)
    {
      if ((m_aLines[L].m_dwFlags & dwFlag) != 0)
        return (int) L;
    }
  return -1;
}

int CCrystalTextBuffer::
GetLineWithFlag (lineflags_t dwFlag) const
{
  int nFlagIndex =::FlagToIndex (dwFlag);
  if (nFlagIndex < 0)
    {
      ASSERT (false);           //  Invalid flag passed in

      return -1;
    }
  return FindLineWithFlag (dwFlag);
}

void CCrystalTextBuffer::
SetLineFlag (int nLine, lineflags_t dwFlag, bool bSet, bool bRemoveFromPreviousLine /*= true*/, bool bUpdate /*= true*/)
{
  ASSERT (m_bInit);             //  Text buffer not yet initialized.
  //  You must call InitNew() or LoadFromFile() first!

  int nFlagIndex =::FlagToIndex (dwFlag);
  if (nFlagIndex < 0 && (nLine == -1 || bRemoveFromPreviousLine))
    {
      ASSERT (false);           //  Invalid flag passed in

      return;
    }

  if (nLine == -1)
    {
      ASSERT (!bSet);
      nLine = FindLineWithFlag (dwFlag);
      if (nLine == -1)
        return;
      bRemoveFromPreviousLine = false;
    }

  if (nLine < 0 || nLine >= static_cast<int>(m_aLines.size()))
    {
      ASSERT(false);    //  nLine is out of range.
      return;
    }

  lineflags_t dwNewFlags = m_aLines[nLine].m_dwFlags;
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
                    UpdateViews (nullptr, nullptr, UPDATE_SINGLELINE | UPDATE_FLAGSONLY, nPrevLine);
                }
            }
          else
            {
              ASSERT (nPrevLine == nLine);
            }
        }

      m_aLines[nLine].m_dwFlags = dwNewFlags;
      if (bUpdate)
        UpdateViews (nullptr, nullptr, UPDATE_SINGLELINE | UPDATE_FLAGSONLY, nLine);
    }
}


/**
 * @brief Get text of specified line range (excluding ghost lines)
 */
void CCrystalTextBuffer::			/* virtual base */
GetTextWithoutEmptys(int nStartLine, int nStartChar, 
                 int nEndLine, int nEndChar, 
                 std::basic_string<tchar_t>& text, CRLFSTYLE nCrlfStyle /*= CRLFSTYLE::AUTOMATIC */,
                 bool bExcludeInvisibleLines/*= true*/) const
{
  GetText(nStartLine, nStartChar, nEndLine, nEndChar, text, (nCrlfStyle == CRLFSTYLE::AUTOMATIC) ? nullptr : GetStringEol (nCrlfStyle), bExcludeInvisibleLines);
}


void CCrystalTextBuffer::
GetText (int nStartLine, int nStartChar, int nEndLine, int nEndChar,
         std::basic_string<tchar_t>& text, const tchar_t* pszCRLF /*= nullptr*/, bool bExcludeInvisibleLines/*= true*/) const
{
  ASSERT (m_bInit);             //  Text buffer not yet initialized.
  //  You must call InitNew() or LoadFromFile() first!
  
  ASSERT (m_aLines.size() < INT_MAX);
  ASSERT (m_aLines[nStartLine].Length() < INT_MAX);
  ASSERT (nStartLine >= 0 && nStartLine < (int)m_aLines.size ());
  ASSERT (nStartChar >= 0 && nStartChar <= (int)m_aLines[nStartLine].Length());
  ASSERT (nEndLine >= 0 && nEndLine < (int)m_aLines.size ());
  ASSERT (nEndChar >= 0 && nEndChar <= (int)m_aLines[nEndLine].Length());
  ASSERT (nStartLine < nEndLine || nStartLine == nEndLine && nStartChar <= nEndChar);
  // some edit functions (copy...) should do nothing when there is no selection.
  // assert to be sure to catch these 'do nothing' cases.
  ASSERT (nStartLine != nEndLine || nStartChar != nEndChar);

  int nCRLFLength;
  const tchar_t* pszCurCRLF;

  size_t nBufSize = 0;
  for (int L = nStartLine; L <= nEndLine; L++)
    {
      nBufSize += m_aLines[L].Length();
      pszCurCRLF = pszCRLF ? pszCRLF : m_aLines[L].GetEol();
      pszCurCRLF = pszCurCRLF ? pszCurCRLF : _T("");
      nCRLFLength = static_cast<int>(tc::tcslen(pszCurCRLF));
      nBufSize += nCRLFLength;
    }

  text.resize (nBufSize);
  tchar_t* pszBuf = text.data ();

  const LineInfo &startLine = m_aLines[nStartLine];
  if (nStartLine < nEndLine)
    {
      ptrdiff_t nCount = startLine.Length() - nStartChar;
      if (nCount > 0)
        {
          memcpy (pszBuf, startLine.GetLine(nStartChar), sizeof (tchar_t) * nCount);
          pszBuf += nCount;
        }
      pszCurCRLF = pszCRLF ? pszCRLF : startLine.GetEol();
      pszCurCRLF = pszCurCRLF ? pszCurCRLF : _T("");
      nCRLFLength = static_cast<int>(tc::tcslen(pszCurCRLF));
      memcpy (pszBuf, pszCurCRLF, sizeof (tchar_t) * nCRLFLength);
      pszBuf += nCRLFLength;
      for (int I = nStartLine + 1; I < nEndLine; I++)
        {
          if (bExcludeInvisibleLines && (GetLineFlags (I) & LF_INVISIBLE))
            continue;
          const LineInfo &li = m_aLines[I];
          nCount = li.Length();
          if (nCount > 0)
            {
              memcpy (pszBuf, li.GetLine(), sizeof (tchar_t) * nCount);
              pszBuf += nCount;
            }
          pszCurCRLF = pszCRLF ? pszCRLF : li.GetEol();
          pszCurCRLF = pszCurCRLF ? pszCurCRLF : _T("");
          nCRLFLength = static_cast<int>(tc::tcslen(pszCurCRLF));
          memcpy (pszBuf, pszCurCRLF, sizeof (tchar_t) * nCRLFLength);
          pszBuf += nCRLFLength;
        }
      if (nEndChar > 0)
        {
          memcpy (pszBuf, m_aLines[nEndLine].GetLine(), sizeof (tchar_t) * nEndChar);
          pszBuf += nEndChar;
        }
    }
  else
    {
      int nCount = nEndChar - nStartChar;
      memcpy (pszBuf, startLine.GetLine(nStartChar), sizeof (tchar_t) * nCount);
      pszBuf += nCount;
    }
  text.resize (pszBuf - text.data ());
}

void CCrystalTextBuffer::
AddView (CCrystalTextView * pView)
{
  m_lpViews.push_back (pView);
}

void CCrystalTextBuffer::
RemoveView (CCrystalTextView * pView)
{
  auto it = std::find (m_lpViews.begin (), m_lpViews.end (), pView);
  if (it != m_lpViews.end ())
      m_lpViews.erase (it);
}

CrystalLineParser::TextDefinition *CCrystalTextBuffer::
RetypeViews (const tchar_t* lpszFileName)
{
  std::basic_string<tchar_t> sNew = GetExt (lpszFileName);
  CrystalLineParser::TextDefinition *def = CrystalLineParser::GetTextType (sNew.c_str ());
  for (auto* pView : m_lpViews)
    pView->SetTextType (def);
  return def;
}

void CCrystalTextBuffer::
UpdateViews (CCrystalTextView * pSource, CUpdateContext * pContext, DWORD dwUpdateFlags, int nLineIndex /*= -1*/ )
{
  for (auto* pView : m_lpViews)
    pView->UpdateView (pSource, pContext, dwUpdateFlags, nLineIndex);
}

/**
 * @brief Delete text from the buffer.
 * @param [in] pSource A view from which the text is deleted.
 * @param [in] nStartLine Starting line for the deletion.
 * @param [in] nStartChar Starting char position for the deletion.
 * @param [in] nEndLine Ending line for the deletion.
 * @param [in] nEndChar Ending char position for the deletion.
 * @return true if the deletion succeeded, false otherwise.
 * @note Line numbers are apparent (screen) line numbers, not real
 * line numbers in the file.
 */
bool CCrystalTextBuffer::
InternalDeleteText (CCrystalTextView * pSource, int nStartLine, int nStartChar,
    int nEndLine, int nEndChar)
{
  ASSERT (m_bInit);             //  Text buffer not yet initialized.
  //  You must call InitNew() or LoadFromFile() first!

  ASSERT (m_aLines.size() < INT_MAX);
  ASSERT (m_aLines[nStartLine].Length() < INT_MAX);
  ASSERT (nStartLine >= 0 && nStartLine < (int)m_aLines.size ());
  ASSERT (nStartChar >= 0 && nStartChar <= (int)m_aLines[nStartLine].Length());
  ASSERT (nEndLine >= 0 && nEndLine < (int)m_aLines.size ());
  ASSERT (nEndChar >= 0 && nEndChar <= (int)m_aLines[nEndLine].FullLength());
  ASSERT (nStartLine < nEndLine || nStartLine == nEndLine && nStartChar <= nEndChar);
  // some edit functions (delete...) should do nothing when there is no selection.
  // assert to be sure to catch these 'do nothing' cases.
//  ASSERT (nStartLine != nEndLine || nStartChar != nEndChar);
  if (m_bReadOnly)
    return false;

  CDeleteContext context;
  context.m_ptStart.y = nStartLine;
  context.m_ptStart.x = nStartChar;
  context.m_ptEnd.y = nEndLine;
  context.m_ptEnd.x = nEndChar;
  if (nStartLine == nEndLine)
    {
      // delete part of one line
      m_aLines[nStartLine].Delete(nStartChar, nEndChar);

      if (pSource!=nullptr)
        UpdateViews (pSource, &context, UPDATE_SINGLELINE | UPDATE_HORZRANGE, nStartLine);
    }
  else
    {
      // delete multiple lines
      const ptrdiff_t nRestCount = m_aLines[nEndLine].FullLength() - nEndChar;
      std::basic_string<tchar_t> sTail(m_aLines[nEndLine].GetLine(nEndChar), static_cast<int>(nRestCount));
      lineflags_t dwFlags = GetLineFlags (nEndLine);

      const int nDelCount = nEndLine - nStartLine;
      for (int L = nStartLine + 1; L <= nEndLine; L++)
        m_aLines[L].Clear();
      std::vector<LineInfo>::iterator iterBegin = m_aLines.begin() + nStartLine + 1;
      std::vector<LineInfo>::iterator iterEnd = iterBegin + nDelCount;
      m_aLines.erase(iterBegin, iterEnd);

      //  nEndLine is no more valid
      m_aLines[nStartLine].DeleteEnd(nStartChar);
      if (nRestCount > 0)
        {
          AppendLine (nStartLine, sTail.c_str (), sTail.length ());
        }
      if (nStartChar == 0)
        m_aLines[nStartLine].m_dwFlags = dwFlags;

      if (pSource!=nullptr)
        UpdateViews (pSource, &context, UPDATE_HORZRANGE | UPDATE_VERTRANGE, nStartLine);
    }

  if (!m_bModified)
    SetModified (true);
  //BEGIN SW
  // remember current cursor position as last editing position
  m_ptLastChange = context.m_ptStart;
  //END SW

  return true;
}

// Remove the last [bytes] characters from specified line, and return them
// (EOL characters are included)
std::basic_string<tchar_t> CCrystalTextBuffer::
StripTail (int i, size_t bytes)
{
  LineInfo & li = m_aLines[i];
  // Must at least take off the EOL characters
  ASSERT(bytes >= li.FullLength() - li.Length());

  const ptrdiff_t offset = li.FullLength() - bytes;
  // Must not take off more than exist
  ASSERT(offset >= 0);

  std::basic_string<tchar_t> ret(li.GetLine(offset), static_cast<int>(bytes));
  li.DeleteEnd(offset);
  return ret;
}


/**
 * @brief Insert text to the buffer.
 * @param [in] pSource A view to which the text is added.
 * @param [in] nLine Line to add the text.
 * @param [in] nPos Position in the line to insert the text.
 * @param [in] pszText The text to insert.
 * @param [in] cchText The length of the text.
 * @param [out] nEndLine Line number of last added line in the buffer.
 * @param [out] nEndChar Character position of the end of the added text
 *   in the buffer.
 * @return true if the insertion succeeded, false otherwise.
 * @note Line numbers are apparent (screen) line numbers, not real
 * line numbers in the file.
 */
bool CCrystalTextBuffer::
InternalInsertText (CCrystalTextView * pSource, int nLine, int nPos,
    const tchar_t* pszText, size_t cchText, int &nEndLine, int &nEndChar)
{
  ASSERT (m_bInit);             //  Text buffer not yet initialized.
  //  You must call InitNew() or LoadFromFile() first!
  
  ASSERT (m_aLines.size() < INT_MAX);
  ASSERT (m_aLines[nLine].Length() < INT_MAX);
  ASSERT (nLine >= 0 && nLine < (int)m_aLines.size ());
  ASSERT (nPos >= 0 && nPos <= (int)m_aLines[nLine].Length());
  if (m_bReadOnly)
    return false;

  CInsertContext context;
  context.m_ptStart.x = nPos;
  context.m_ptStart.y = nLine;
  nEndLine = 0;
  nEndChar = 0;

  int nRestCount = GetFullLineLength(nLine) - nPos;
  std::basic_string<tchar_t> sTail;
  if (nRestCount > 0)
    {
      // remove end of line (we'll put it back on afterwards)
      sTail = StripTail(nLine, nRestCount);
    }

  bool bInQuote = false;
  if (m_bTableEditing && m_bAllowNewlinesInQuotes)
    {
      const tchar_t* pszLine = m_aLines[nLine].GetLine ();
      for (int j = 0; j < nPos; ++j)
        {
          if (pszLine[j] == m_cFieldEnclosure)
            bInQuote = !bInQuote;
        }
    }

  int nInsertedLines = 0;
  int nCurrentLine = nLine;
  for (;;)
    {
      int haseol = 0;
      size_t nTextPos = 0;
      // advance to end of line
      if (m_bTableEditing && m_bAllowNewlinesInQuotes)
        {
          while (nTextPos < cchText && (bInQuote || !LineInfo::IsEol(pszText[nTextPos])))
            {
              if (pszText[nTextPos] == m_cFieldEnclosure)
                bInQuote = !bInQuote;
              nTextPos++;
            }
        }
      else
        {
          while (nTextPos < cchText && !LineInfo::IsEol(pszText[nTextPos]))
            nTextPos++;
        }
      // advance after EOL of line
      if (nTextPos < cchText)
        {
          haseol = 1;
          const tchar_t* eol = &pszText[nTextPos];
          nTextPos++;
          if (nTextPos < cchText && LineInfo::IsDosEol(eol))
            nTextPos++;
        }

      // The first line of the new text is appended to the start line
      // All succeeding lines are inserted
      if (nCurrentLine == nLine)
        {
          AppendLine (nLine, pszText, nTextPos, !bInQuote);
        }
      else
        {
          InsertLine (pszText, nTextPos, nCurrentLine);
          nInsertedLines ++;
        }


      if (nTextPos == cchText)
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
              nEndChar = GetFullLineLength(nEndLine);
            }
          if (!sTail.empty())
            {
              if (haseol)
                {
                  InsertLine(sTail.c_str (), sTail.length (), nEndLine);
                  nInsertedLines ++;
                }
              else
                AppendLine (nEndLine, sTail.c_str (), nRestCount);
            }
          if (nEndLine == GetLineCount())
            {
              // We left cursor after last screen line
              // which is an illegal cursor position
              // so manufacture a new trailing line
              InsertLine(_T(""), 0);
              nInsertedLines ++;
            }
          break;
        }

      ++nCurrentLine;
      pszText += nTextPos;
      cchText -= nTextPos;
    }

  // Compute the context : all positions after context.m_ptStart are
  // shifted accordingly to (context.m_ptEnd - context.m_ptStart)
  // The begin point is the insertion point.
  // The end point is more tedious : if we insert in a ghost line, we reuse it, 
  // so we insert fewer lines than the number of lines in the text buffer
  if (nEndLine - nLine != nInsertedLines)
    {
      context.m_ptEnd.y = nLine + nInsertedLines;
      context.m_ptEnd.x = GetFullLineLength(context.m_ptEnd.y);
    }
  else
    {
      context.m_ptEnd.x = nEndChar;
      context.m_ptEnd.y = nEndLine;
    }


  if (pSource!=nullptr)
    {
      if (nInsertedLines > 0)
        UpdateViews (pSource, &context, UPDATE_HORZRANGE | UPDATE_VERTRANGE, nLine);
      else
        UpdateViews (pSource, &context, UPDATE_SINGLELINE | UPDATE_HORZRANGE, nLine);
    }

  if (!m_bModified)
    SetModified (true);

  // remember current cursor position as last editing position
  m_ptLastChange.x = nEndChar;
  m_ptLastChange.y = nEndLine;
  return true;
}

bool CCrystalTextBuffer::
CanUndo () const
{
  ASSERT (m_aUndoBuf.size () < INT_MAX);
  ASSERT (m_nUndoPosition >= 0 && m_nUndoPosition <= (int)m_aUndoBuf.size ());
  return m_nUndoPosition > 0;
}

bool CCrystalTextBuffer::
CanRedo () const
{
  ASSERT (m_aUndoBuf.size () < INT_MAX);
  ASSERT (m_nUndoPosition >= 0 && m_nUndoPosition <= (int)m_aUndoBuf.size ());
  return m_nUndoPosition < static_cast<int>(m_aUndoBuf.size ());
}

size_t CCrystalTextBuffer::
GetUndoActionCode (int & nAction, size_t pos /*= 0*/ ) const
{
  ASSERT (CanUndo ());          //  Please call CanUndo() first

  ASSERT ((m_aUndoBuf[0].m_dwFlags & UNDO_BEGINGROUP) != 0);

  intptr_t nPosition;
  if (pos == 0)
    {
      //  Start from beginning
      nPosition = m_nUndoPosition;
    }
  else
    {
      nPosition = static_cast<intptr_t>(pos);
      ASSERT (nPosition > 0 && nPosition < m_nUndoPosition);
      ASSERT ((m_aUndoBuf[nPosition].m_dwFlags & UNDO_BEGINGROUP) != 0);
    }

  //  Advance to next undo group
  nPosition--;
  std::vector<UndoRecord>::const_iterator iter = m_aUndoBuf.begin () + nPosition;
  while (((*iter).m_dwFlags & UNDO_BEGINGROUP) == 0)
    {
      --iter;
      --nPosition;
    }

  //  Get description
  nAction = (*iter).m_nAction;

  //  Now, if we stop at zero position, this will be the last action,
  //  since we return (size_t) nPosition
  return (size_t) nPosition;
}

size_t CCrystalTextBuffer::
GetRedoActionCode (int & nAction, size_t pos /*= 0*/ ) const
{
  ASSERT (CanRedo ());          //  Please call CanRedo() before!

  ASSERT ((m_aUndoBuf[0].m_dwFlags & UNDO_BEGINGROUP) != 0);
  ASSERT ((m_aUndoBuf[m_nUndoPosition].m_dwFlags & UNDO_BEGINGROUP) != 0);

  intptr_t nPosition;
  if (pos == 0)
    {
      //  Start from beginning
      nPosition = m_nUndoPosition;
    }
  else
    {
      nPosition = static_cast<intptr_t>(pos);
      ASSERT (nPosition > m_nUndoPosition);
      ASSERT ((m_aUndoBuf[nPosition].m_dwFlags & UNDO_BEGINGROUP) != 0);
    }

  //  Get description
  nAction = m_aUndoBuf[nPosition].m_nAction;

  //  Advance to next undo group
  nPosition++;
  if (nPosition < static_cast<intptr_t>(m_aUndoBuf.size ()))
    {
      std::vector<UndoRecord>::const_iterator iter = m_aUndoBuf.begin () + nPosition;
      while (iter != m_aUndoBuf.end () && ((*iter).m_dwFlags & UNDO_BEGINGROUP) == 0)
        {
          ++iter;
          ++nPosition;
        }
    }
  if (nPosition >= static_cast<intptr_t>(m_aUndoBuf.size ()))
    return 0;                //  No more redo actions!

  return (size_t) nPosition;
}

size_t CCrystalTextBuffer::
GetUndoDescription (std::basic_string<tchar_t>& desc, size_t pos /*= 0*/ ) const
{
  int nAction;
  size_t retValue = GetUndoActionCode(nAction, pos);

  //  Read description
  if (!GetActionDescription (nAction, desc))
    desc.clear ();              //  Use empty string as description

  return retValue;
}

size_t CCrystalTextBuffer::
GetRedoDescription (std::basic_string<tchar_t>& desc, size_t pos /*= 0*/ ) const
{
  int nAction;
  size_t retValue = GetRedoActionCode(nAction, pos);

  //  Read description
  if (!GetActionDescription (nAction, desc))
    desc.clear ();              //  Use empty string as description

  return retValue;
}


bool CCrystalTextBuffer::		/* virtual base */		
UndoInsert(CCrystalTextView * pSource, CEPoint & ptCursorPos, const CEPoint apparent_ptStartPos, CEPoint const apparent_ptEndPos, const UndoRecord & ur)
{
  if (DeleteText (pSource, apparent_ptStartPos.y, apparent_ptStartPos.x, apparent_ptEndPos.y, apparent_ptEndPos.x, 0, false, false))
    {
      ptCursorPos = apparent_ptStartPos;
      return true;
    }
  ASSERT(false);
  return false;
}

bool CCrystalTextBuffer::		/* virtual base */
Undo (CCrystalTextView * pSource, CEPoint & ptCursorPos)
{
  ASSERT (CanUndo ());
  ASSERT ((m_aUndoBuf[0].m_dwFlags & UNDO_BEGINGROUP) != 0);
  bool failed = false;
  int tmpPos = m_nUndoPosition;

  // Hide caret to quickly undo changes made by Replace All operation
  bool bCursorHiddenSaved = false;
  if (pSource)
    {
      bCursorHiddenSaved = pSource->m_bCursorHidden;
      pSource->m_bCursorHidden = true;
    }

  while (!failed)
    {
      --tmpPos;
      const UndoRecord ur = GetUndoRecord(tmpPos);
      // Undo records are stored in file line numbers
      // and must be converted to apparent (screen) line numbers for use
      CEPoint apparent_ptStartPos = ur.m_ptStartPos;
      CEPoint apparent_ptEndPos = ur.m_ptEndPos;

      if (ur.m_dwFlags & UNDO_INSERT)
        {
          if (!UndoInsert(pSource, ptCursorPos, apparent_ptStartPos, apparent_ptEndPos, ur))
            {
              failed = true;
              break;
            }
          // ptCursorPos = apparent_ptStartPos;
        }
      else
        {
          int nEndLine, nEndChar;
          if (!InsertText(pSource, apparent_ptStartPos.y, apparent_ptStartPos.x, ur.GetText(), ur.GetTextLength(), nEndLine, nEndChar, 0, false))
            {
              ASSERT(false);
              failed = true;
              break;
            }
          ptCursorPos = m_ptLastChange;
        }

      // restore line revision numbers
      RestoreRevisionNumbers(ur.m_ptStartPos.y, ur.m_paSavedRevisionNumbers);

      if (ur.m_dwFlags & UNDO_BEGINGROUP)
        break;
    }
  if (m_bModified && m_nSyncPosition == tmpPos)
    SetModified (false);
  if (!m_bModified && m_nSyncPosition != tmpPos)
    SetModified (true);
  if (failed)
    {
      // If the Undo failed, clear the entire Undo/Redo stack
      // Not only can we not Redo the failed Undo, but the Undo
      // may have partially completed (if in a group)
      m_nUndoPosition = 0;
      m_aUndoBuf.clear ();
    }
  else
    {
      m_nUndoPosition = tmpPos;
    }

  if (pSource)
    {
      pSource->m_bCursorHidden = bCursorHiddenSaved;
      pSource->UpdateCaret();
    }

  return !failed;
}

bool CCrystalTextBuffer::		/* virtual base */
Redo (CCrystalTextView * pSource, CEPoint & ptCursorPos)
{
  ASSERT (CanRedo ());
  ASSERT ((m_aUndoBuf[0].m_dwFlags & UNDO_BEGINGROUP) != 0);
  ASSERT ((m_aUndoBuf[m_nUndoPosition].m_dwFlags & UNDO_BEGINGROUP) != 0);

  // Hide caret to quickly redo changes made by Replace All operation
  bool bCursorHiddenSaved = false;
  if (pSource)
    {
      bCursorHiddenSaved = pSource->m_bCursorHidden;
      pSource->m_bCursorHidden = true;
    }

  for (;;)
    {
      const UndoRecord ur = GetUndoRecord(m_nUndoPosition);
      CEPoint apparent_ptStartPos = ur.m_ptStartPos;
      CEPoint apparent_ptEndPos = ur.m_ptEndPos;

      // now we can use normal insertTxt or deleteText
      if (ur.m_dwFlags & UNDO_INSERT)
        {
          int nEndLine, nEndChar;
          VERIFY(InsertText (pSource, apparent_ptStartPos.y, apparent_ptStartPos.x,
            ur.GetText(), ur.GetTextLength(), nEndLine, nEndChar, 0, false));
          ptCursorPos = m_ptLastChange;
        }
      else
        {
          if (apparent_ptStartPos != apparent_ptEndPos)
            {
#ifdef _DEBUG
              std::basic_string<tchar_t> text;
              GetTextWithoutEmptys (apparent_ptStartPos.y, apparent_ptStartPos.x, apparent_ptEndPos.y, apparent_ptEndPos.x, text, CRLFSTYLE::AUTOMATIC, false);
              ASSERT (text.length () == ur.GetTextLength() && memcmp(text.c_str (), ur.GetText(), text.length () * sizeof(tchar_t)) == 0);
#endif
              VERIFY(DeleteText(pSource, apparent_ptStartPos.y, apparent_ptStartPos.x, 
                apparent_ptEndPos.y, apparent_ptEndPos.x, 0, false, false));
            }
          ptCursorPos = apparent_ptStartPos;
        }
      m_nUndoPosition++;
      if (static_cast<size_t>(m_nUndoPosition) == m_aUndoBuf.size())
        break;
      if ((m_aUndoBuf[m_nUndoPosition].m_dwFlags & UNDO_BEGINGROUP) != 0)
        break;
    }

  if (m_bModified && m_nSyncPosition == m_nUndoPosition)
    SetModified (false);
  if (!m_bModified && m_nSyncPosition != m_nUndoPosition)
    SetModified (true);

  if (pSource)
    {
      pSource->m_bCursorHidden = bCursorHiddenSaved;
      pSource->UpdateCaret();
    }

  return true;
}

// the CEPoint parameters are apparent (on screen) line numbers

void CCrystalTextBuffer::			/* virtual base */
AddUndoRecord (bool bInsert, const CEPoint & ptStartPos,
    const CEPoint & ptEndPos, const tchar_t* pszText, size_t cchText, int nActionType /*= CE_ACTION_UNKNOWN*/,
    std::vector<uint32_t> *paSavedRevisionNumbers /*= nullptr*/)
{
  //  Forgot to call BeginUndoGroup()?
  ASSERT (m_bUndoGroup);
  ASSERT (m_aUndoBuf.size () == 0 || (m_aUndoBuf[0].m_dwFlags & UNDO_BEGINGROUP) != 0);

  //  Strip unnecessary undo records (edit after undo wipes all potential redo records)
  int nBufSize = (int) m_aUndoBuf.size ();
  if (m_nUndoPosition < nBufSize)
    {
      m_aUndoBuf.resize (m_nUndoPosition);
    }

  //  Add new record
  UndoRecord ur;
  ur.m_dwFlags = bInsert ? UNDO_INSERT : 0;
  ur.m_nAction = nActionType;
  if (m_bUndoBeginGroup)
    {
      ur.m_dwFlags |= UNDO_BEGINGROUP;
      m_bUndoBeginGroup = false;
    }
  ur.m_ptStartPos = ptStartPos;
  ur.m_ptEndPos = ptEndPos;
  ur.SetText (pszText, cchText);
  ur.m_paSavedRevisionNumbers = paSavedRevisionNumbers;

  // Optimize memory allocation
  if (m_aUndoBuf.capacity() == m_aUndoBuf.size())
    {
      if (m_aUndoBuf.size() == 0)
        m_aUndoBuf.reserve(16);
      else if (m_aUndoBuf.size() < 1025)
        m_aUndoBuf.reserve(m_aUndoBuf.size() * 2);
      else
        m_aUndoBuf.reserve(m_aUndoBuf.size() + 1024);
    }
  m_aUndoBuf.push_back (ur);
  m_nUndoPosition = (int) m_aUndoBuf.size ();
}

/**
/**
 * @brief Get EOL style string.
 * @param [in] nCRLFMode.
 * @return string of CRLF style.
 */
const tchar_t* CCrystalTextBuffer::GetStringEol(CRLFSTYLE nCRLFMode)
{
  switch(nCRLFMode)
  {
  case CRLFSTYLE::DOS: return _T("\r\n");
  case CRLFSTYLE::UNIX: return _T("\n");
  case CRLFSTYLE::MAC: return _T("\r");
      // If mixed or not defined
  default: return _T("\r\n");
  }
}

const tchar_t* CCrystalTextBuffer::GetDefaultEol() const
{
  return GetStringEol(m_nCRLFMode);
}

/**
 * @brief Insert text to the buffer.
 * @param [in] pSource A view to which the text is added.
 * @param [in] nLine Line to add the text.
 * @param [in] nPos Position in the line to insert the text.
 * @param [in] pszText The text to insert.
 * @param [in] cchText The length of the text.
 * @param [out] nEndLine Line number of last added line in the buffer.
 * @param [out] nEndChar Character position of the end of the added text
 *   in the buffer.
 * @param [in] nAction Edit action.
 * @param [in] bHistory Save insertion for undo/redo?
 * @return true if the insertion succeeded, false otherwise.
 * @note Line numbers are apparent (screen) line numbers, not real
 * line numbers in the file.
 */
bool CCrystalTextBuffer::			/* virtual base */
InsertText (CCrystalTextView * pSource, int nLine, int nPos, const tchar_t* pszText,
    size_t cchText, int &nEndLine, int &nEndChar, int nAction,
    bool bHistory /*= true*/)
{
  // save line revision numbers for undo
  std::vector<uint32_t> *paSavedRevisionNumbers = new std::vector<uint32_t>;
  paSavedRevisionNumbers->resize(1);
  (*paSavedRevisionNumbers)[0] = m_aLines[nLine].m_dwRevisionNumber;

  if (!InternalInsertText (pSource, nLine, nPos, pszText, cchText, nEndLine, nEndChar))
    {
      delete paSavedRevisionNumbers;
      return false;
    }

  // update line revision numbers of modified lines
  m_dwCurrentRevisionNumber++;
  for (int i = nLine ; i < nEndLine; i++)
    m_aLines[i].m_dwRevisionNumber = m_dwCurrentRevisionNumber;
  if (nPos != 0 || nEndChar != 0)
    m_aLines[nEndLine].m_dwRevisionNumber = m_dwCurrentRevisionNumber;

  if (!bHistory)
    {
      delete paSavedRevisionNumbers;
      return true;
    }

  bool bGroupFlag = false;
  if (!m_bUndoGroup)
    {
      BeginUndoGroup ();
      bGroupFlag = true;
    }

  AddUndoRecord (true, CEPoint (nPos, nLine), CEPoint (nEndChar, nEndLine),
                 pszText, cchText, nAction, paSavedRevisionNumbers);

  if (bGroupFlag)
    FlushUndoGroup (pSource);

  return true;
}

/**
 * @brief Delete text from the buffer.
 * @param [in] pSource A view from which the text is deleted.
 * @param [in] nStartLine Starting line for the deletion.
 * @param [in] nStartChar Starting char position for the deletion.
 * @param [in] nEndLine Ending line for the deletion.
 * @param [in] nEndChar Ending char position for the deletion.
 * @param [in] nAction Edit action.
 * @param [in] bHistory Save deletion for undo/redo?
 * @param [in] bExcludeInvisibleLines Don't delete LF_INVISIBLE lines 
 * @return true if the deletion succeeded, false otherwise.
 * @note Line numbers are apparent (screen) line numbers, not real
 * line numbers in the file.
 */
bool CCrystalTextBuffer::			/* virtual base */
DeleteText (CCrystalTextView * pSource, int nStartLine, int nStartChar,
            int nEndLine, int nEndChar, int nAction, bool bHistory /*= true*/, bool bExcludeInvisibleLines /*= true*/)
{
  bool bGroupFlag = false;
  if (bHistory)
    {
      if (!m_bUndoGroup)
        {
          BeginUndoGroup ();
          bGroupFlag = true;
        }
    }
  if (bExcludeInvisibleLines && pSource != nullptr && pSource->GetEnableHideLines ())
    {
      for (int nLineIndex = nEndLine; nLineIndex >= nStartLine; nLineIndex--)
        {
          if (!(GetLineFlags (nLineIndex) & LF_INVISIBLE))
            {
              int nEndLine2 = nLineIndex;
              int nStartLine2;
              for (nStartLine2 = nLineIndex - 1; nStartLine2 >= nStartLine; nStartLine2--)
                {
                  if (GetLineFlags (nStartLine2) & LF_INVISIBLE)
                    break;
                }  
              nStartLine2++;
              nLineIndex = nStartLine2;
              int nStartChar2 = (nStartLine == nStartLine2) ? nStartChar : 0;
              int nEndChar2;
              if (nEndLine == nEndLine2)
                nEndChar2 = nEndChar;
              else
                {
                  nEndChar2 = 0;
                  nEndLine2++;
                }
              if (!DeleteText2 (pSource, nStartLine2, nStartChar2, nEndLine2, nEndChar2, nAction, bHistory))
                return false;
            }
        }
    }
  else
    {
      if (!DeleteText2 (pSource, nStartLine, nStartChar, nEndLine, nEndChar, nAction, bHistory))
        return false;
    }

  if (bGroupFlag)
    FlushUndoGroup (pSource);

  return true;
}

std::vector<uint32_t> *CCrystalTextBuffer::
CopyRevisionNumbers(int nStartLine, int nEndLine) const
{
  // save line revision numbers for undo
  std::vector<uint32_t> *paSavedRevisionNumbers = new std::vector<uint32_t>;
  paSavedRevisionNumbers->resize(nEndLine - nStartLine + 1);
  for (int i = 0; i < nEndLine - nStartLine + 1; i++)
    (*paSavedRevisionNumbers)[i] = m_aLines[nStartLine + i].m_dwRevisionNumber;
  return paSavedRevisionNumbers;
}

void CCrystalTextBuffer::
RestoreRevisionNumbers(int nStartLine, std::vector<uint32_t> *paSavedRevisionNumbers)
{
  for (size_t i = 0; i < paSavedRevisionNumbers->size(); i++)
    m_aLines[nStartLine + i].m_dwRevisionNumber = (*paSavedRevisionNumbers)[i];
}

bool CCrystalTextBuffer::			/* virtual base */
DeleteText2 (CCrystalTextView * pSource, int nStartLine, int nStartChar,
            int nEndLine, int nEndChar, int nAction /* = CE_ACTION_UNKNOWN*/, bool bHistory /*= true*/)
{
  std::basic_string<tchar_t> sTextToDelete;
  GetTextWithoutEmptys (nStartLine, nStartChar, nEndLine, nEndChar, sTextToDelete);

  // save line revision numbers for undo
  std::vector<uint32_t> *paSavedRevisionNumbers = CopyRevisionNumbers(nStartLine, nEndLine);

  if (!InternalDeleteText (pSource, nStartLine, nStartChar, nEndLine, nEndChar))
    {
      delete paSavedRevisionNumbers;
      return false;
    }

  // update line revision numbers of modified lines
  m_dwCurrentRevisionNumber++;
  m_aLines[nStartLine].m_dwRevisionNumber = m_dwCurrentRevisionNumber;

  if (!bHistory)
    {
      delete paSavedRevisionNumbers;
      return true;
    }

  AddUndoRecord (false, CEPoint (nStartChar, nStartLine), CEPoint (nEndChar, nEndLine),
                 sTextToDelete.c_str (), sTextToDelete.length (), nAction, paSavedRevisionNumbers);

  return true;
}

bool CCrystalTextBuffer::
GetActionDescription (int nAction, std::basic_string<tchar_t>& desc) const
{
  switch (nAction)
    {
    case CE_ACTION_PASTE:
      desc = _T ("Paste");
      break;
    case CE_ACTION_DELSEL:
      desc = _T ("Delete Selection");
      break;
    case CE_ACTION_CUT:
      desc = _T ("Cut");
      break;
    case CE_ACTION_TYPING:
      desc = _T ("Typing");
      break;
    case CE_ACTION_BACKSPACE:
      desc = _T ("Backspace");
      break;
    case CE_ACTION_INDENT:
      desc = _T ("Indent");
      break;
    case CE_ACTION_DRAGDROP:
      desc = _T ("Drag And Drop");
      break;
    case CE_ACTION_REPLACE:
      desc = _T ("Replace");
      break;
    case CE_ACTION_DELETE:
      desc = _T ("Delete");
      break;
    case CE_ACTION_AUTOINDENT:
      desc = _T ("Auto Indent");
      break;
    case CE_ACTION_AUTOCOMPLETE:
      desc = _T ("Auto Complete");
      break;
    case CE_ACTION_AUTOEXPAND:
      desc = _T ("Auto Expand");
      break;
    case CE_ACTION_LOWERCASE:
      desc = _T ("Lower Case");
      break;
    case CE_ACTION_UPPERCASE:
      desc = _T ("Upper Case");
      break;
    case CE_ACTION_SWAPCASE:
      desc = _T ("Swap Case");
      break;
    case CE_ACTION_CAPITALIZE:
      desc = _T ("Capitalize");
      break;
    case CE_ACTION_SENTENCIZE:
      desc = _T ("Sentencize");
      break;
    case CE_ACTION_RECODE:
      desc = _T ("Recode");
      break;
    case CE_ACTION_SPELL:
      desc = _T ("Spell");
      break;
    default: /* case CE_ACTION_UNKNOWN: */
      desc = _T ("Unknown");
    }
  return true;
}

void CCrystalTextBuffer::			/* virtual base */
SetModified (bool bModified /*= true*/ )
{
  m_bModified = bModified;
}

void CCrystalTextBuffer::
BeginUndoGroup (bool bMergeWithPrevious /*= false*/ )
{
  ASSERT (!m_bUndoGroup);
  m_bUndoGroup = true;
  m_bUndoBeginGroup = m_nUndoPosition == 0 || !bMergeWithPrevious;
}

void CCrystalTextBuffer::
FlushUndoGroup (CCrystalTextView * pSource)
{
  ASSERT (m_bUndoGroup);
  if (pSource != nullptr)
    {
      ASSERT (static_cast<size_t>(m_nUndoPosition) <= m_aUndoBuf.size());
      if (m_nUndoPosition > 0)
        {
          pSource->OnEditOperation (m_aUndoBuf[m_nUndoPosition - 1].m_nAction, m_aUndoBuf[m_nUndoPosition - 1].GetText (), m_aUndoBuf[m_nUndoPosition - 1].GetTextLength ());
        }
    }
  m_bUndoGroup = false;
}

int CCrystalTextBuffer::
FindNextBookmarkLine (int nCurrentLine) const
{
  bool bWrapIt = true;
  lineflags_t dwFlags = GetLineFlags (nCurrentLine);
  if ((dwFlags & LF_BOOKMARKS) != 0)
    nCurrentLine++;

  const size_t nSize = m_aLines.size ();
  for (;;)
    {
      while (nCurrentLine < static_cast<int>(nSize))
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
      bWrapIt = false;
      nCurrentLine = 0;
    }
//~  return -1;
}

int CCrystalTextBuffer::
FindPrevBookmarkLine (int nCurrentLine) const
{
  bool bWrapIt = true;
  lineflags_t dwFlags = GetLineFlags (nCurrentLine);
  if ((dwFlags & LF_BOOKMARKS) != 0)
    nCurrentLine--;

  const size_t nSize = m_aLines.size ();
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
      bWrapIt = false;
      nCurrentLine = (int) (nSize - 1);
    }
//~  return -1;
}

//BEGIN SW
CEPoint CCrystalTextBuffer::GetLastChangePos() const
{
  return m_ptLastChange;
}
//END SW
void CCrystalTextBuffer::RestoreLastChangePos(CEPoint pt)
{
  m_ptLastChange = pt;
}


/**
 * @brief Delete one or several lines
 */
void CCrystalTextBuffer::DeleteLine(int line, int nCount /*=1*/)
{
  for (int ic = 0; ic < nCount; ic++)
    m_aLines[line + ic].Clear();
  std::vector<LineInfo>::iterator iterBegin = m_aLines.begin() + line;
  std::vector<LineInfo>::iterator iterEnd = iterBegin + nCount;
  m_aLines.erase(iterBegin, iterEnd);
}

int CCrystalTextBuffer::GetTabSize() const
{
  ASSERT( m_nTabSize >= 0 && m_nTabSize <= 64 );
  return m_nTabSize;
}

void CCrystalTextBuffer::SetTabSize(int nTabSize)
{
  ASSERT( nTabSize >= 0 && nTabSize <= 64 );
  m_nTabSize = nTabSize;
}

int CCrystalTextBuffer::GetColumnWidth (int nColumnIndex) const
{
  ASSERT( nColumnIndex >= 0 );
  if (nColumnIndex < static_cast<int>(m_pSharedTableProps->m_aColumnWidths.size ()))
    return m_pSharedTableProps->m_aColumnWidths[nColumnIndex];
  else
    return m_nTabSize;
}

void CCrystalTextBuffer::SetColumnWidth (int nColumnIndex, int nColumnWidth)
{
  ASSERT( nColumnIndex >= 0 );
  ASSERT( nColumnWidth >= 0 );
  if (nColumnIndex >= static_cast<int>(m_pSharedTableProps->m_aColumnWidths.size ()))
    m_pSharedTableProps->m_aColumnWidths.resize (nColumnIndex + 1, m_nTabSize);
  m_pSharedTableProps->m_aColumnWidths[nColumnIndex] = nColumnWidth;
}

void CCrystalTextBuffer::SetColumnWidths (const std::vector<int>& columnWidths)
{
  ASSERT( m_pSharedTableProps != nullptr );
  m_pSharedTableProps->m_aColumnWidths = columnWidths;
}

int CCrystalTextBuffer::GetColumnCount (int nLineIndex) const
{
  ASSERT( nLineIndex >= 0 );
  int nColumnCount = 0;
  const tchar_t* pszLine = GetLineChars (nLineIndex);
  int nLength = GetLineLength (nLineIndex);
  bool bInQuote = false;
  for (int j = 0; j < nLength; ++j)
    {
      if (pszLine[j] == m_cFieldEnclosure)
        bInQuote = !bInQuote;
      else if (!bInQuote && pszLine[j] == m_cFieldDelimiter)
        ++nColumnCount;
    }
  return nColumnCount;
}

std::basic_string<tchar_t> CCrystalTextBuffer::GetCellText (int nLineIndex, int nColumnIndex) const
{
  ASSERT( nLineIndex >= 0 && nColumnIndex >= 0 && GetTableEditing() );
  std::basic_string<tchar_t> sText;
  int nColumnCount = 0;
  const tchar_t* pszLine = GetLineChars (nLineIndex);
  int nLength = GetLineLength (nLineIndex);
  bool bInQuote = false;
  for (int j = 0; j < nLength && nColumnCount <= nColumnIndex; ++j)
    {
      if (nColumnIndex == nColumnCount && (bInQuote || pszLine[j] != m_cFieldDelimiter))
        sText += pszLine[j];
      if (pszLine[j] == m_cFieldEnclosure)
        bInQuote = !bInQuote;
      else if (!bInQuote && pszLine[j] == m_cFieldDelimiter)
        ++nColumnCount;
    }
  return sText;
}

void CCrystalTextBuffer::JoinLinesForTableEditingMode ()
{
  if (!m_bAllowNewlinesInQuotes)
      return;
  size_t nLineCount = m_aLines.size ();
  size_t j = 0;
  bool bInQuote = false;
  for (size_t i = 0; i < nLineCount;)
    {
      const tchar_t* pszChars = m_aLines[i].GetLine ();
      const size_t nLineLength = m_aLines[i].FullLength ();
      for (; j < nLineLength; ++j)
        {
          if (pszChars[j] == m_cFieldEnclosure)
            bInQuote = !bInQuote;
        }
      m_aLines[i].m_dwRevisionNumber = 0;
      if (bInQuote && i < nLineCount - 1)
        {
          std::basic_string<tchar_t> line(m_aLines[i].GetLine (), m_aLines[i].FullLength ());
          line.append (m_aLines[i + 1].GetLine (), m_aLines[i + 1].FullLength ());
          m_aLines[i].FreeBuffer ();
          m_aLines[i].Create (line.c_str (), line.size ());
          m_aLines[i + 1].FreeBuffer ();
          m_aLines.erase (m_aLines.begin () + i + 1);
          --nLineCount;
          continue;
        }
      ++i;
      j = 0;
      bInQuote = false;
    }
  m_aUndoBuf.clear();
  m_nUndoPosition = 0;
  m_bModified = false;
}

void CCrystalTextBuffer::SplitLinesForTableEditingMode ()
{
  size_t nLineCount = m_aLines.size ();
  for (size_t i = 0; i < nLineCount; ++i)
    {
      const tchar_t* pszChars = m_aLines[i].GetLine ();
      const size_t nLineLength = m_aLines[i].FullLength ();
      for (size_t j = 0; j < nLineLength; ++j)
        {
          int eols = 0;
          if (pszChars[j] == '\r')
            eols = (j < nLineLength - 1 && pszChars[j + 1] == '\n') ? 2 : 1;
          else if (pszChars[j] == '\n')
            eols = 1;
          if (eols > 0)
            {
              m_aLines.emplace (m_aLines.begin () + i + 1, pszChars + j + eols, nLineLength - (j + eols));
              m_aLines[i].DeleteEnd (j + eols);
              m_aLines[i].m_dwRevisionNumber = 0;
            }
        }
    }
  m_aUndoBuf.clear ();
  m_nUndoPosition = 0;
  m_bModified = false;
}

void CCrystalTextBuffer::
InvalidateColumns ()
{
  for (auto& buf : m_pSharedTableProps->m_textBufferList)
    {
      for (auto* pView : buf->m_lpViews)
        {
          pView->InvalidateScreenRect ();
          pView->UpdateView (nullptr, nullptr, UPDATE_HORZRANGE | UPDATE_VERTRANGE, -1);
        }
    }
}

