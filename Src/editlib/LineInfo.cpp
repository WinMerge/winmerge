/** 
 * @file  LineInfo.cpp
 *
 * @brief Implementation of LineInfo class.
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"
#include "LineInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/**
 @brief Constructor.
 */
LineInfo::LineInfo()
: m_pcLine(NULL)
, m_nLength(0)
, m_nMax(0)
, m_nEolChars(0)
, m_dwFlags(0)
, m_dwRevisionNumber(0)
{
};

/**
 * @brief Clear item.
 * Frees buffer, sets members to initial values.
 */
void LineInfo::Clear()
{
  if (m_pcLine != NULL)
    {
      delete[] m_pcLine;
      m_pcLine = NULL;
      m_nLength = 0;
      m_nMax = 0;
      m_nEolChars = 0;
      m_dwFlags = 0;
      m_dwRevisionNumber = 0;
    }
}

/**
 * @brief Free reserved memory.
 * Frees reserved memory, but does not clear flags.
 */
void LineInfo::FreeBuffer()
{
  if (m_pcLine != NULL)
    {
      delete[] m_pcLine;
      m_pcLine = NULL;
      m_nLength = 0;
      m_nMax = 0;
      m_nEolChars = 0;
    }
}

/**
 * @brief Create a line.
 * @param [in] pszLine Line data.
 * @param [in] nLength Line length.
 */
void LineInfo::Create(LPCTSTR pszLine, int nLength)
{
  if (nLength == 0)
    {
      CreateEmpty();
      return;
    }

  m_nLength = nLength;
  m_nMax = ALIGN_BUF_SIZE (m_nLength + 1);
  ASSERT (m_nMax >= m_nLength + 1);
  if (m_pcLine != NULL)
    delete[] m_pcLine;
  m_pcLine = new TCHAR[m_nMax];
  ZeroMemory(m_pcLine, m_nMax * sizeof(TCHAR));
  const DWORD dwLen = sizeof (TCHAR) * m_nLength;
  CopyMemory (m_pcLine, pszLine, dwLen);
  m_pcLine[m_nLength] = '\0';

  int nEols = 0;
  if (nLength > 1 && IsDosEol(&pszLine[nLength - 2]))
    nEols = 2;
  else if (nLength && IsEol(pszLine[nLength - 1]))
    nEols = 1;
  m_nLength -= nEols;
  m_nEolChars = nEols;
}

/**
 * @brief Create an empty line.
 */
void LineInfo::CreateEmpty()
{
  m_nLength = 0;
  m_nEolChars = 0;
  m_nMax = ALIGN_BUF_SIZE (m_nLength + 1);
  if (m_pcLine != NULL)
    delete [] m_pcLine;
  m_pcLine = new TCHAR[m_nMax];
  ZeroMemory(m_pcLine, m_nMax * sizeof(TCHAR));
}

/**
 * @brief Append a text to the line.
 * @param [in] pszChars String to append to the line.
 * @param [in] nLength Length of the string to append.
 */
void LineInfo::Append(LPCTSTR pszChars, int nLength)
{
  int nBufNeeded = m_nLength + nLength + 1;
  if (nBufNeeded > m_nMax)
    {
      m_nMax = ALIGN_BUF_SIZE (nBufNeeded);
      ASSERT (m_nMax >= m_nLength + nLength);
      TCHAR *pcNewBuf = new TCHAR[m_nMax];
      if (FullLength() > 0)
        memcpy (pcNewBuf, m_pcLine, sizeof (TCHAR) * (FullLength() + 1));
      delete[] m_pcLine;
      m_pcLine = pcNewBuf;
    }

  memcpy (m_pcLine + m_nLength, pszChars, sizeof (TCHAR) * nLength);
  m_nLength += nLength;
  m_pcLine[m_nLength] = '\0';

  // Did line gain eol ? (We asserted above that it had none at start)
   if (nLength > 1 && IsDosEol(&m_pcLine[m_nLength - 2]))
     {
       m_nEolChars = 2;
     }
   else if (LineInfo::IsEol(m_pcLine[m_nLength - 1]))
      {
       m_nEolChars = 1;
      }
   m_nLength -= m_nEolChars;
  ASSERT (m_nLength + m_nEolChars <= m_nMax);
}

/**
 * @brief Has the line EOL?
 * @return TRUE if the line has EOL bytes.
 */
BOOL LineInfo::HasEol() const
{
  if (m_nEolChars)
    return TRUE;
  else
    return FALSE;
}

/**
 * @brief Get line's EOL bytes.
 * @return EOL bytes, or NULL if no EOL bytes.
 */
LPCTSTR LineInfo::GetEol() const
{
  if (HasEol())
    return &m_pcLine[Length()];
  else
    return NULL;
}

/**
 * @brief Change line's EOL.
 * @param [in] lpEOL New EOL bytes.
 * @return TRUE if succeeded, FALSE if failed (nothing to change).
 */
BOOL LineInfo::ChangeEol(LPCTSTR lpEOL)
{
  const int nNewEolChars = (int) _tcslen(lpEOL);

  // Check if we really are changing EOL.
  if (nNewEolChars == m_nEolChars)
    if (_tcscmp(m_pcLine + Length(), lpEOL) == 0)
      return FALSE;

  int nBufNeeded = m_nLength + nNewEolChars+1;
  if (nBufNeeded > m_nMax)
    {
      m_nMax = ALIGN_BUF_SIZE (nBufNeeded);
      ASSERT (m_nMax >= nBufNeeded);
      TCHAR *pcNewBuf = new TCHAR[m_nMax];
      if (FullLength() > 0)
        memcpy (pcNewBuf, m_pcLine, sizeof (TCHAR) * (FullLength() + 1));
      delete[] m_pcLine;
      m_pcLine = pcNewBuf;
    }
  
  // copy also the 0 to zero-terminate the line
  memcpy (m_pcLine + m_nLength, lpEOL, sizeof (TCHAR) * (nNewEolChars + 1));
  m_nEolChars = nNewEolChars;
  return TRUE;
}

/**
 * @brief Delete part of the line.
 * @param [in] nStartChar Start position for removal.
 * @param [in] nEndChar End position for removal.
 */
void LineInfo::Delete(int nStartChar, int nEndChar)
{
  if (nEndChar < Length() || m_nEolChars)
    {
      // preserve characters after deleted range by shifting up
      memcpy (m_pcLine + nStartChar, m_pcLine + nEndChar,
              sizeof (TCHAR) * (FullLength() - nEndChar));
    }
  m_nLength -= (nEndChar - nStartChar);
  m_pcLine[FullLength()] = '\0';
}

/**
 * @brief Delete line contents from given index to the end.
 * @param [in] Index of first character to remove.
 */
void LineInfo::DeleteEnd(int nStartChar)
{
  m_nLength = nStartChar;
  m_pcLine[nStartChar] = 0;
  m_nEolChars = 0;
}

/**
 * @brief Copy contents from another LineInfo item.
 * @param [in] li Item to copy.
 */
void LineInfo::CopyFrom(const LineInfo &li)
{
  if (m_pcLine != NULL)
    delete [] m_pcLine;
  m_pcLine = new TCHAR[li.m_nMax];
  memcpy(m_pcLine, li.m_pcLine, li.m_nMax * sizeof(TCHAR));
}

/**
 * @brief Remove EOL from line.
 */
void LineInfo::RemoveEol()
{
  if (HasEol())
  {
    m_pcLine[m_nLength] = '\0';
    m_nEolChars = 0;
  }
}

/**
 * @brief Get line contents.
 * @param [in] index Index of first character to get.
 * @note Make a copy from returned string, as it can get reallocated.
 */
LPCTSTR LineInfo::GetLine(int index) const
{
  return &m_pcLine[index];
}
