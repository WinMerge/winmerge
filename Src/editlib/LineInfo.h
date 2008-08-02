/** 
 * @file LineInfo.h
 *
 * @brief Declaration for SLineInfo structure.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _EDITOR_LINEINFO_H_
#define _EDITOR_LINEINFO_H_

/**
 * @brief Line information.
 * This structure presents one line in the editor.
 */
struct SLineInfo
  {
    TCHAR *m_pcLine; /**< Line data. */
    int m_nLength; /**< Line length (without EOL bytes). */
    int m_nMax; /**< Allocated space for line data. */
    int m_nEolChars; /**< # of EOL bytes. */
    DWORD m_dwFlags; /**< Line flags. */
    DWORD m_dwRevisionNumber; /**< Edit revision (for edit tracking). */

    int FullLength() const { return m_nLength+m_nEolChars; }
    int Length() const { return m_nLength; }

    SLineInfo ()
    {
      memset (this, 0, sizeof (SLineInfo));
    };
  };

#endif // _EDITOR_LINEINFO_H_
