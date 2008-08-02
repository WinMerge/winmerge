/**
 * @file UndoRecord.h
 *
 * @brief Declaration for SUndoRecord structure.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _EDITOR_UNDO_RECORD_H_
#define _EDITOR_UNDO_RECORD_H_

struct SUndoRecord
{
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
  SUndoRecord () // default constructor
  {
    memset (this, 0, sizeof (SUndoRecord));
  }

  SUndoRecord (const SUndoRecord & src) // copy constructor
  {
    memset (this, 0, sizeof (SUndoRecord));
    (*this)=src;
  }

  SUndoRecord & operator=(const SUndoRecord & src) // copy assignment
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
    return *this;
  }

  ~SUndoRecord () // destructor
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
