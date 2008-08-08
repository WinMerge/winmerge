/**
 * @file UndoRecord.h
 *
 * @brief Declaration for UndoRecord structure.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _EDITOR_UNDO_RECORD_H_
#define _EDITOR_UNDO_RECORD_H_

class UndoRecord
{
public:
  DWORD m_dwFlags;
  CPoint m_ptStartPos, m_ptEndPos;  //  Block of text participating
  int m_nAction;            //  For information only: action type
  CDWordArray *m_paSavedRevisonNumbers;

private:
  //  TCHAR   *m_pcText;
  //  Since in most cases we have 1 character here,
  //  we should invent a better way. Note: 2 * sizeof(WORD) <= sizeof(TCHAR*)
  //
  //  Here we will use the following trick: on Win32 platforms high-order word
  //  of any pointer will be != 0. So we can store 1 character strings without
  //  allocating memory.
  //
  struct TextBuffer
  {
    int size;
    TCHAR data[1];
  };

  union
  {
    TextBuffer *m_pszText;     //  For cases when we have > 1 character strings
    TCHAR m_szText[2];    //  For single-character strings
  };

  public:
  UndoRecord () // default constructor
    : m_dwFlags(0)
    , m_nAction(0)
    , m_paSavedRevisonNumbers(NULL)
    , m_pszText(NULL)
  {
  }

  UndoRecord (const UndoRecord & src) // copy constructor
    : m_dwFlags(0)
    , m_nAction(0)
    , m_paSavedRevisonNumbers(NULL)
    , m_pszText(NULL)
  {
    Clone(src);
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
    if (m_paSavedRevisonNumbers)
    delete m_paSavedRevisonNumbers;
  }

  void SetText (LPCTSTR pszText, int cchText);
  void FreeText ();

  LPCTSTR GetText () const
  {
    // See the m_szText/m_pszText definition
    // Check if m_pszText is a pointer by removing bits having
    // possible char value
    if (((INT_PTR)m_pszText >> 16) != 0)
      return m_pszText->data;
    return m_szText;
  }

  int GetTextLength () const
  {
    if (((INT_PTR)m_pszText >> 16) != 0)
      return m_pszText->size;
    return 1;
  }
};

#endif // _EDITOR_UNDO_RECORD_H_
