/** 
 * @file  UndoRecord.cpp
 *
 * @brief Implementation of UndoRecord struct.
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"
#include "UndoRecord.h"

void SUndoRecord::
SetText (LPCTSTR pszText, int nLength)
{
  FreeText();
  if (nLength)
    {
      if (nLength > 1)
        {
          m_pszText = (TextBuffer *)malloc(sizeof(TextBuffer) + nLength * sizeof(TCHAR));
          m_pszText->size = nLength;
          memcpy(m_pszText->data, pszText, nLength * sizeof(TCHAR));
          m_pszText->data[nLength] = _T('?'); // debug sentinel
        }
      else
        {
          m_szText[0] = pszText[0];
        }
    }
}

void SUndoRecord::
FreeText ()
{
  // See the m_szText/m_pszText definition
  // Check if m_pszText is a pointer by removing bits having
  // possible char value
  if (((INT_PTR)m_pszText >> 16) != 0)
    free(m_pszText);
  m_pszText = NULL;
}
