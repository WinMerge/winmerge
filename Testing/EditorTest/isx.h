#ifndef countof
#define countof(array)  (sizeof(array)/sizeof((array)[0]))
#endif /* countof */

#define ISXKEYWORD(keywordlist, key, keylen) IsXKeyword(key, keylen, keywordlist, countof(keywordlist), _tcsncmp)
#define ISXKEYWORDI(keywordlist, key, keylen) IsXKeyword(key, keylen, keywordlist, countof(keywordlist), _tcsnicmp)

int xisspecial (int c);
int xisalpha (int c);
int xisalnum (int c);
int xisspace (int c);

int GetClipTcharTextFormat();

BOOL IsXKeyword(LPCTSTR pszKey, size_t nKeyLen, LPCTSTR pszKeywordList[], size_t nKeywordListCount, int (*compare)(LPCTSTR, LPCTSTR, size_t));

