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
 * @brief Create a line.
 * @param [in] pszLine Line data.
 * @param [in] nLength Line length.
 */
void LineInfo::Create(LPCTSTR pszLine, int nLength)
{
  m_nLength = nLength;
  m_nMax = ALIGN_BUF_SIZE (m_nLength + 1);
  ASSERT (m_nMax >= m_nLength + 1);
  m_pcLine = new TCHAR[m_nMax];
  if (m_nLength > 0)
    {
      DWORD dwLen = sizeof (TCHAR) * m_nLength;
      CopyMemory (m_pcLine, pszLine, dwLen);
    }
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
  m_pcLine = new TCHAR[m_nMax];
  m_pcLine[0] = '\0';
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
