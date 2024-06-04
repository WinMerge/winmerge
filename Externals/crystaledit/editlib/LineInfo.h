/** 
 * @file LineInfo.h
 *
 * @brief Declaration for LineInfo class.
 *
 */

#pragma once

#include "utils/ctchar.h"
#include <cstdint>

//  Line allocation granularity
constexpr size_t CHAR_ALIGN = 16;
constexpr size_t ALIGN_BUF_SIZE(size_t size) { return ((size) / CHAR_ALIGN) * CHAR_ALIGN + CHAR_ALIGN; }

typedef unsigned long lineflags_t;
enum LINEFLAGS: lineflags_t
{
  LF_BOOKMARK_FIRST = 0x00000001UL,
  LF_EXECUTION = 0x00010000UL,
  LF_BREAKPOINT = 0x00020000UL,
  LF_COMPILATION_ERROR = 0x00040000UL,
  LF_BOOKMARKS = 0x00080000UL,
  LF_INVALID_BREAKPOINT = 0x00100000UL,
  LF_INVISIBLE = 0x80000000UL
};

#define LF_BOOKMARK(id)     (LF_BOOKMARK_FIRST << id)

/**
 * @brief Line information.
 * This class presents one line in the editor.
 */
class LineInfo
  {
public:
    lineflags_t m_dwFlags; /**< Line flags. */
    uint32_t m_dwRevisionNumber; /**< Edit revision (for edit tracking). */

    LineInfo();
    LineInfo(const tchar_t* pszLine, size_t nLength);
    LineInfo(const LineInfo& li);
    LineInfo(LineInfo&& li) noexcept;
    LineInfo& operator=(const LineInfo& li);
    LineInfo& operator=(LineInfo&& li) noexcept;
    ~LineInfo();
    void Clear();
    void FreeBuffer();
    void Create(const tchar_t* pszLine, size_t nLength);
    void CreateEmpty();
    void Append(const tchar_t* pszChars, size_t nLength, bool bDetectEol = true);
    void Delete(size_t nStartChar, size_t nEndChar);
    void DeleteEnd(size_t nStartChar);
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

/**
 * @brief Has the line EOL?
 * @return true if the line has EOL bytes.
 */
inline bool LineInfo::HasEol() const { return (m_nEolChars != 0); }


/**
 * @brief Get line contents.
 * @param [in] index Index of first character to get.
 * @note Make a copy from returned string, as it can get reallocated.
 */
inline const tchar_t* LineInfo::GetLine(size_t index) const { return &m_pcLine[index]; }
