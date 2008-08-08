/** 
 * @file  UndoRecord.cpp
 *
 * @brief Implementation of UndoRecord struct.
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"
#include "UndoRecord.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void UndoRecord::
Clone(const UndoRecord &src)
  {
    m_dwFlags = src.m_dwFlags;
    m_ptStartPos = src.m_ptStartPos;
    m_ptEndPos = src.m_ptEndPos;
    m_nAction = src.m_nAction;
    SetText(src.GetText(), src.GetTextLength());
    INT_PTR size = src.m_paSavedRevisonNumbers->GetSize();
    if (!m_paSavedRevisonNumbers)
      m_paSavedRevisonNumbers = new CDWordArray();
    m_paSavedRevisonNumbers->SetSize(size);
    INT_PTR i;
    for (i = 0; i < size; i++)
      (*m_paSavedRevisonNumbers)[i] = (*src.m_paSavedRevisonNumbers)[i];
  }

void UndoRecord::
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

void UndoRecord::
FreeText ()
{
  // See the m_szText/m_pszText definition
  // Check if m_pszText is a pointer by removing bits having
  // possible char value
  if (((INT_PTR)m_pszText >> 16) != 0)
    free(m_pszText);
  m_pszText = NULL;
}
