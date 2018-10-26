/** 
 * @file  UndoRecord.cpp
 *
 * @brief Implementation of UndoRecord struct.
 */
// ID line follows -- this is updated by SVN
// $Id: UndoRecord.cpp 5762 2008-08-08 05:32:15Z kimmov $

#include "stdafx.h"
#include "UndoRecord.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void UndoRecord::
Clone(const UndoRecord &src)
  {
    m_dwFlags = src.m_dwFlags;
    m_ptStartPos = src.m_ptStartPos;
    m_ptEndPos = src.m_ptEndPos;
    m_nAction = src.m_nAction;
    SetText(src.GetText(), src.GetTextLength());
    INT_PTR size = src.m_paSavedRevisionNumbers->GetSize();
    if (m_paSavedRevisionNumbers == nullptr)
      m_paSavedRevisionNumbers = new CDWordArray();
    m_paSavedRevisionNumbers->SetSize(size);
    INT_PTR i;
    for (i = 0; i < size; i++)
      (*m_paSavedRevisionNumbers)[i] = (*src.m_paSavedRevisionNumbers)[i];
  }

void UndoRecord::
SetText (LPCTSTR pszText, size_t nLength)
{
  FreeText();
  if (nLength != 1)
    {
	  ASSERT (nLength < INT_MAX);
      m_pszText = (TextBuffer *)malloc(sizeof(TextBuffer) + (nLength+2) * sizeof(TCHAR));
      if (m_pszText != nullptr)
        {
          m_pszText->size = nLength;
          memcpy(m_pszText->data, pszText, nLength * sizeof(TCHAR));
          m_pszText->data[nLength] = _T('?'); // debug sentinel
          m_pszText->data[nLength+1] = _T('\0'); // terminator
        }
    }
  else
    {
      m_szText[0] = pszText[0];
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
  m_pszText = nullptr;
}
