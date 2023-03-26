/**
 * @file UndoRecord.h
 *
 * @brief Declaration for UndoRecord structure.
 *
 */

#pragma once

#include "utils/ctchar.h"
#include "cepoint.h"
#include <vector>

typedef uint32_t undoflags_t;
enum : undoflags_t
{
    UNDO_INSERT = 0x0001U,
    UNDO_BEGINGROUP = 0x0100U
};

class UndoRecord
{
public:
  undoflags_t m_dwFlags;
  CEPoint m_ptStartPos, m_ptEndPos;  //  Block of text participating
  int m_nAction;            //  For information only: action type
  std::vector<uint32_t> *m_paSavedRevisionNumbers;

private:
  //  tchar_t   *m_pcText;
  //  Since in most cases we have 1 character here,
  //  we should invent a better way. Note: 2 * sizeof(WORD) <= sizeof(tchar_t*)
  //
  //  Here we will use the following trick: on Win32 platforms high-order word
  //  of any pointer will be != 0. So we can store 1 character strings without
  //  allocating memory.
  //
  struct TextBuffer
  {
    size_t size;
    tchar_t data[1];
  };

  union
  {
    TextBuffer *m_pszText;     //  For cases when we have > 1 character strings
    tchar_t m_szText[2];    //  For single-character strings
  };

  public:
  UndoRecord () // default constructor
    : m_dwFlags(0)
    , m_nAction(0)
    , m_paSavedRevisionNumbers(nullptr)
    , m_pszText(nullptr)
  {
  }

  UndoRecord (const UndoRecord & src) // copy constructor
    : m_dwFlags(0)
    , m_nAction(0)
    , m_paSavedRevisionNumbers(nullptr)
    , m_pszText(nullptr)
  {
    UndoRecord::Clone(src);
  }

  virtual void Clone(const UndoRecord &src);

  virtual UndoRecord & operator=(const UndoRecord & src) // copy assignment
  {
    Clone(src);
    return *this;
  }

  virtual ~UndoRecord () // destructor
  {
    FreeText();
    delete m_paSavedRevisionNumbers;
  }

  void SetText (const tchar_t* pszText, size_t cchText);
  void FreeText ();

  const tchar_t* GetText () const
  {
    // See the m_szText/m_pszText definition
    // Check if m_pszText is a pointer by removing bits having
    // possible char value
    if (((intptr_t)m_pszText >> 16) != 0)
      return m_pszText->data;
    return m_szText;
  }

  size_t GetTextLength () const
  {
    if (((intptr_t)m_pszText >> 16) != 0)
      return m_pszText->size;
    return 1;
  }
};

