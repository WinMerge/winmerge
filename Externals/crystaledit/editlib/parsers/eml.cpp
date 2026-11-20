#include "pch.h"
#include "crystallineparser.h"

namespace
{
constexpr unsigned headerContinuation = 1;
constexpr unsigned messageBody = 2;

bool IsFieldNameCharacter(tchar_t ch)
{
  // RFC 5322 field names use printable ASCII other than space and colon.
  return ch >= '!' && ch <= '~' && ch != ':';
}
}

unsigned CrystalLineParser::ParseLineEml(unsigned dwCookie, const tchar_t* pszChars,
                                        int nLength, std::vector<TEXTBLOCK>* pBuf)
{
  // An empty line ends the message headers. Body lines may themselves contain colons.
  if (dwCookie == messageBody || nLength == 0)
    return messageBody;

  if (dwCookie == headerContinuation && (pszChars[0] == ' ' || pszChars[0] == '\t'))
    {
      DEFINE_BLOCK(0, COLORINDEX_STRING);
      return headerContinuation;
    }

  int colon = 0;
  while (colon < nLength && IsFieldNameCharacter(pszChars[colon]))
    ++colon;
  if (colon == 0 || colon == nLength || pszChars[colon] != ':')
    return messageBody;

  DEFINE_BLOCK(0, COLORINDEX_KEYWORD);
  DEFINE_BLOCK(colon, COLORINDEX_OPERATOR);
  if (colon + 1 < nLength)
    DEFINE_BLOCK(colon + 1, COLORINDEX_STRING);
  return headerContinuation;
}
