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
public: // All public as this used to be a struct.
    TCHAR *m_pcLine; /**< Line data. */
    int m_nLength; /**< Line length (without EOL bytes). */
    int m_nMax; /**< Allocated space for line data. */
    int m_nEolChars; /**< # of EOL bytes. */
    DWORD m_dwFlags; /**< Line flags. */
    DWORD m_dwRevisionNumber; /**< Edit revision (for edit tracking). */

    int FullLength() const { return m_nLength+m_nEolChars; }
    int Length() const { return m_nLength; }
    void Create(LPCTSTR pszLine, int nLength);
    void CreateEmpty();
    void Append(LPCTSTR pszChars, int nLength);

    LineInfo ()
    {
      memset (this, 0, sizeof (LineInfo));
    };

    static bool IsEol(TCHAR ch)
    {
      return ch=='\r' || ch=='\n';
    };

    static bool IsDosEol(LPCTSTR sz)
    {
      return sz[0]=='\r' && sz[1]=='\n';
    };
  };

#endif // _EDITOR_LINEINFO_H_
