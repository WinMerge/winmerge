/** 
 * @file LineInfo.h
 *
 * @brief Declaration for LineInfo class.
 *
 */

#pragma once

#include "utils/ctchar.h"

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
    void Create(const tchar_t* pszLine, size_t nLength);
    void CreateEmpty();
    void Append(const tchar_t* pszChars, size_t nLength, bool bDetectEol = true);
    void Delete(size_t nStartChar, size_t nEndChar);
    void DeleteEnd(size_t nStartChar);
    void CopyFrom(const LineInfo &li);
    bool HasEol() const;
    const tchar_t* GetEol() const;
    bool ChangeEol(const tchar_t* lpEOL);
    void RemoveEol();
    const tchar_t* GetLine(size_t index = 0) const;

    /** @brief Return full line length (including EOL bytes). */
    size_t FullLength() const { return m_nLength + m_nEolChars; }
    /** @brief Return line length. */
    size_t Length() const { return m_nLength; }

    /** @brief Is the char an EOL char? */
    static bool IsEol(tchar_t ch)
    {
      return ch=='\r' || ch=='\n';
    };

    /** @brief Are the characters DOS EOL bytes? */
    static bool IsDosEol(const tchar_t* sz)
    {
      return sz[0]=='\r' && sz[1]=='\n';
    };

private:
    tchar_t *m_pcLine; /**< Line data. */
    size_t m_nMax; /**< Allocated space for line data. */
    size_t m_nLength; /**< Line length (without EOL bytes). */
    int m_nEolChars; /**< # of EOL bytes. */
  };
