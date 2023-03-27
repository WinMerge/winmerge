/** 
 * @file  UndoRecord.cpp
 *
 * @brief Implementation of UndoRecord struct.
 */

#include "pch.h"
#include "UndoRecord.h"
#include <cassert>

void UndoRecord::
Clone(const UndoRecord &src)
  {
    m_dwFlags = src.m_dwFlags;
    m_ptStartPos = src.m_ptStartPos;
    m_ptEndPos = src.m_ptEndPos;
    m_nAction = src.m_nAction;
    SetText(src.GetText(), src.GetTextLength());
    size_t size = src.m_paSavedRevisionNumbers ? src.m_paSavedRevisionNumbers->size() : 0;
    if (m_paSavedRevisionNumbers == nullptr)
      m_paSavedRevisionNumbers = new std::vector<uint32_t>();
    m_paSavedRevisionNumbers->resize(size);
    for (size_t i = 0; i < size; i++)
      (*m_paSavedRevisionNumbers)[i] = (*src.m_paSavedRevisionNumbers)[i];
  }

void UndoRecord::
SetText (const tchar_t* pszText, size_t nLength)
{
  FreeText();
  if (nLength != 1)
    {
      assert (nLength < INT_MAX);
      m_pszText = reinterpret_cast<TextBuffer *>(malloc(sizeof(TextBuffer) + (nLength+2) * sizeof(tchar_t)));
      if (m_pszText != nullptr)
        {
          m_pszText->size = nLength;
          memcpy(m_pszText->data, pszText, nLength * sizeof(tchar_t));
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
  if (((intptr_t)m_pszText >> 16) != 0)
    free(m_pszText);
  m_pszText = nullptr;
}
