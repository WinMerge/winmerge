#include "StdAfx.h"
#include "FindTextHelper.h"
#include "utils/cregexp.h"
#include "utils/string_util.h"

static const tchar_t *memstr(const tchar_t *str1, size_t str1len, const tchar_t *str2, size_t str2len)
{
  ASSERT(str1 && str2 && str2len > 0);
  for (const tchar_t *p = str1; p < str1 + str1len; ++p)
    {
      if (*p == *str2)
        {
          if (memcmp(p, str2, str2len * sizeof(tchar_t)) == 0)
            return p;
        }
    }
  return nullptr;
}

inline tchar_t mytoupper(tchar_t ch)
{
    return static_cast<tchar_t>(reinterpret_cast<uintptr_t>(CharUpper(reinterpret_cast<LPTSTR>(ch))));
}

static const tchar_t *memistr(const tchar_t *str1, size_t str1len, const tchar_t *str2, size_t str2len)
{
  ASSERT(str1 && str2 && str2len > 0);
  for (const tchar_t *p = str1; p < str1 + str1len; ++p)
    {
      if (mytoupper(*p) == mytoupper(*str2))
        {
          size_t i;
          for (i = 0; i < str2len; ++i)
            {
              if (mytoupper(p[i]) != mytoupper(str2[i]))
                break;
            }
          if (i == str2len)
            return p;
        }
    }
  return nullptr;
}

ptrdiff_t
FindStringHelper (const tchar_t* pszLineBegin, size_t nLineLength, const tchar_t* pszFindWhere, const tchar_t* pszFindWhat, findtext_flags_t dwFlags, int &nLen, RxNode *&rxnode, RxMatchRes *rxmatch)
{
  if (dwFlags & FIND_REGEXP)
    {
      ptrdiff_t pos = -1;

      if (rxnode)
        RxFree (rxnode);
      rxnode = nullptr;
      if (pszFindWhat[0] == '^' && pszLineBegin != pszFindWhere)
        return pos;
      rxnode = RxCompile (pszFindWhat, (dwFlags & FIND_MATCH_CASE) != 0 ? RX_CASE : 0);
      if (rxnode && RxExec (rxnode, pszLineBegin, nLineLength, pszFindWhere, rxmatch))
        {
          pos = rxmatch->Open[0];
          ASSERT((rxmatch->Close[0] - rxmatch->Open[0]) < INT_MAX);
          nLen = static_cast<int>(rxmatch->Close[0] - rxmatch->Open[0]);
        }
      return pos;
    }
  else
    {
      ASSERT (pszFindWhere != nullptr);
      ASSERT (pszFindWhat != nullptr);
      int nCur = static_cast<int>(pszFindWhere - pszLineBegin);
      int nLength = (int) tc::tcslen (pszFindWhat);
      const tchar_t* pszFindWhereOrig = pszFindWhere;
      nLen = nLength;
      for (;;)
        {
          const tchar_t* pszPos;
          if (dwFlags & FIND_MATCH_CASE)
            pszPos = memstr(pszFindWhere,  nLineLength - (pszFindWhere - pszLineBegin), pszFindWhat, nLength);
          else
            pszPos = memistr(pszFindWhere, nLineLength - (pszFindWhere - pszLineBegin), pszFindWhat, nLength);
          if (pszPos == nullptr)
            return -1;
          if ((dwFlags & FIND_WHOLE_WORD) == 0)
            return nCur + (int) (pszPos - pszFindWhere);
          if (pszPos > pszFindWhereOrig && xisalnum (pszPos[-1]))
            {
              nCur += (int) (pszPos - pszFindWhere + 1);
              pszFindWhere = pszPos + 1;
              continue;
            }
          if (xisalnum (pszPos[nLength]))
            {
              nCur += (int) (pszPos - pszFindWhere + 1);
              pszFindWhere = pszPos + 1;
              continue;
            }
          return nCur + (int) (pszPos - pszFindWhere);
        }
    }
//~  ASSERT (false);               // Unreachable
}

findtext_flags_t ConvertSearchInfosToSearchFlags(const LastSearchInfos* lastSearch)
{
    findtext_flags_t dwSearchFlags = 0;
    if (lastSearch->m_bMatchCase)
        dwSearchFlags |= FIND_MATCH_CASE;
    if (lastSearch->m_bWholeWord)
        dwSearchFlags |= FIND_WHOLE_WORD;
    if (lastSearch->m_bRegExp)
        dwSearchFlags |= FIND_REGEXP;
    if (lastSearch->m_nDirection == 0)
        dwSearchFlags |= FIND_DIRECTION_UP;
    if (lastSearch->m_bNoWrap)
        dwSearchFlags |= FIND_NO_WRAP;
    if (lastSearch->m_bNoClose)
        dwSearchFlags |= FIND_NO_CLOSE;
    return dwSearchFlags;
}

void ConvertSearchFlagsToLastSearchInfos(LastSearchInfos* lastSearch, findtext_flags_t dwFlags)
{
    lastSearch->m_bMatchCase = (dwFlags & FIND_MATCH_CASE) != 0;
    lastSearch->m_bWholeWord = (dwFlags & FIND_WHOLE_WORD) != 0;
    lastSearch->m_bRegExp = (dwFlags & FIND_REGEXP) != 0;
    lastSearch->m_nDirection = (dwFlags & FIND_DIRECTION_UP) == 0;
    lastSearch->m_bNoWrap = (dwFlags & FIND_NO_WRAP) != 0;
    lastSearch->m_bNoClose = (dwFlags & FIND_NO_CLOSE) != 0;
}

