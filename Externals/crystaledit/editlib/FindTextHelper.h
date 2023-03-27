#pragma once

#include "utils/ctchar.h"
#include "utils/cregexp.h"

//  CCrystalTextView::FindText() flags
typedef unsigned findtext_flags_t;

enum : findtext_flags_t
{
	FIND_MATCH_CASE = 0x0001U,
	FIND_WHOLE_WORD = 0x0002U,
	FIND_REGEXP = 0x0004U,
	FIND_DIRECTION_UP = 0x0010U,
	REPLACE_SELECTION = 0x0100U,
	FIND_NO_WRAP = 0x200U,
	FIND_NO_CLOSE = 0x400U
};

/**
 * @brief Infos about the last search settings (saved in registry)
 *
 * @note Is also used in the replace dialog
 */
struct LastSearchInfos
{
	int m_nDirection;       // only for search
	bool m_bNoWrap;
	bool m_bMatchCase;
	CString m_sText;
	bool m_bWholeWord;
	bool m_bRegExp;
	bool m_bNoClose;
};

ptrdiff_t FindStringHelper(const tchar_t* pszLineBegin, size_t nLineLength, const tchar_t* pszFindWhere, const tchar_t* pszFindWhat, findtext_flags_t dwFlags, int& nLen, RxNode*& rxnode, RxMatchRes* rxmatch);
void ConvertSearchFlagsToLastSearchInfos(LastSearchInfos* lastSearch, findtext_flags_t dwFlags);
findtext_flags_t ConvertSearchInfosToSearchFlags(const LastSearchInfos* lastSearch);

