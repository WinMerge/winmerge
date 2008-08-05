/** 
 * @file LineInfo.h
 *
 * @brief Declaration for LineInfo class.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _EDITOR_LINEINFO_H_
#define _EDITOR_LINEINFO_H_

//  Line allocation granularity
#define     CHAR_ALIGN                  16
#define     ALIGN_BUF_SIZE(size)        ((size) / CHAR_ALIGN) * CHAR_ALIGN + CHAR_ALIGN;

/**
 * @brief Line information.
 * This class presents one line in the editor.
 */
class LineInfo
  {
public:
    DWORD m_dwFlags; /**< Line flags. */
    DWORD m_dwRevisionNumber; /**< Edit revision (for edit tracking). */

    LineInfo();
    void Clear();
    void FreeBuffer();
    void Create(LPCTSTR pszLine, int nLength);
    void CreateEmpty();
    void Append(LPCTSTR pszChars, int nLength);
    void Delete(int nStartChar, int nEndChar);
    void DeleteEnd(int nStartChar);
    void CopyFrom(const LineInfo &li);
    BOOL HasEol() const;
    LPCTSTR GetEol() const;
    BOOL ChangeEol(LPCTSTR lpEOL);
    void RemoveEol();
    LPCTSTR GetLine(int index = 0) const;

    /** @brief Return full line length (including EOL bytes). */
    int FullLength() const { return m_nLength + m_nEolChars; }
    /** @brief Return line length. */
    int Length() const { return m_nLength; }

    /** @brief Is the char an EOL char? */
    static bool IsEol(TCHAR ch)
    {
      return ch=='\r' || ch=='\n';
    };

    /** @brief Are the characters DOS EOL bytes? */
    static bool IsDosEol(LPCTSTR sz)
    {
      return sz[0]=='\r' && sz[1]=='\n';
    };

private:
    TCHAR *m_pcLine; /**< Line data. */
    int m_nMax; /**< Allocated space for line data. */
    int m_nLength; /**< Line length (without EOL bytes). */
    int m_nEolChars; /**< # of EOL bytes. */
  };

#endif // _EDITOR_LINEINFO_H_
