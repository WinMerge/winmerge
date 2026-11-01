// RTF syntax highlighting.  This lexer colors the source RTF, not the rendered text.

#include "pch.h"
#include "crystallineparser.h"
#include <limits>

namespace
{

bool IsRtfLetter(tchar_t ch)
{
  return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

bool IsRtfDigit(tchar_t ch)
{
  return ch >= '0' && ch <= '9';
}

bool IsRtfHexDigit(tchar_t ch)
{
  return IsRtfDigit(ch) || (ch >= 'a' && ch <= 'f') ||
         (ch >= 'A' && ch <= 'F');
}

void ColorToken(std::vector<CrystalLineParser::TEXTBLOCK>* pBuf,
                int begin, int end, int length, int color)
{
  if (begin >= end)
    return;
  CrystalLineParser::defineBlock(pBuf, begin, color);
  if (end < length)
    CrystalLineParser::defineBlock(pBuf, end, COLORINDEX_NORMALTEXT);
}

unsigned SkipRtfLineEnding(unsigned dwCookie, int nEolLength)
{
  // \binN counts the physical CR/LF bytes, although line text excludes them.
  if (nEolLength <= 0)
    return dwCookie;
  const unsigned eolLength = static_cast<unsigned>(nEolLength);
  return dwCookie > eolLength ? dwCookie - eolLength : 0;
}

} // namespace

unsigned CrystalLineParser::ParseLineRtf(unsigned dwCookie, const tchar_t* pszChars,
                                        int nLength, std::vector<TEXTBLOCK>* pBuf)
{
  const int nEolLength = pszChars ? static_cast<int>(tc::tcslen(pszChars + nLength)) : 0;
  // The cookie is the number of unconsumed raw characters following \binN.
  // Reserve UINT_MAX for the syntax parser's "not cached" sentinel.
  constexpr unsigned maxBinaryLength = (std::numeric_limits<unsigned>::max)() - 1;
  if (nLength <= 0)
    return SkipRtfLineEnding(dwCookie, nEolLength);

  DEFINE_BLOCK(0, COLORINDEX_NORMALTEXT);

  for (int i = 0; i < nLength;)
    {
      if (dwCookie != 0)
        {
          const unsigned available = static_cast<unsigned>(nLength - i);
          const unsigned consumed = dwCookie < available ? dwCookie : available;
          ColorToken(pBuf, i, i + static_cast<int>(consumed), nLength,
                     COLORINDEX_STRING);
          i += static_cast<int>(consumed);
          dwCookie -= consumed;
          continue;
        }

      if (pszChars[i] == '{' || pszChars[i] == '}')
        {
          ColorToken(pBuf, i, i + 1, nLength, COLORINDEX_OPERATOR);
          ++i;
          continue;
        }

      if (pszChars[i] != '\\')
        {
          ++i;
          continue;
        }

      const int start = i++;
      if (i == nLength)
        {
          ColorToken(pBuf, start, i, nLength, COLORINDEX_PREPROCESSOR);
          break;
        }

      if (!IsRtfLetter(pszChars[i]))
        {
          // Control symbols include escaped braces, backslashes and \'hh.
          if (pszChars[i] == '\'' && i + 2 < nLength &&
              IsRtfHexDigit(pszChars[i + 1]) && IsRtfHexDigit(pszChars[i + 2]))
            {
              i += 3;
              ColorToken(pBuf, start, i, nLength, COLORINDEX_STRING);
            }
          else
            {
              const int color = pszChars[i] == '{' || pszChars[i] == '}' ||
                                pszChars[i] == '\\' ? COLORINDEX_STRING :
                                COLORINDEX_PREPROCESSOR;
              ++i;
              ColorToken(pBuf, start, i, nLength, color);
            }
          continue;
        }

      const int wordStart = i;
      while (i < nLength && IsRtfLetter(pszChars[i]))
        ++i;
      const int wordEnd = i;
      ColorToken(pBuf, start, wordEnd, nLength, COLORINDEX_KEYWORD);

      const bool isBinary = wordEnd - wordStart == 3 &&
                            pszChars[wordStart] == 'b' &&
                            pszChars[wordStart + 1] == 'i' &&
                            pszChars[wordStart + 2] == 'n';
      bool negative = false;
      if (i < nLength && pszChars[i] == '-' &&
          i + 1 < nLength && IsRtfDigit(pszChars[i + 1]))
        {
          negative = true;
          ++i;
        }

      const int numberStart = i;
      unsigned number = 0;
      while (i < nLength && IsRtfDigit(pszChars[i]))
        {
          const unsigned digit = static_cast<unsigned>(pszChars[i] - '0');
          number = number > (maxBinaryLength - digit) / 10 ?
                   maxBinaryLength : number * 10 + digit;
          ++i;
        }
      if (numberStart != i)
        {
          ColorToken(pBuf, negative ? numberStart - 1 : numberStart,
                     i, nLength, COLORINDEX_NUMBER);
          if (isBinary && !negative)
            dwCookie = number;
        }

      // A single space after a control word is a delimiter, not document text.
      if (i < nLength && pszChars[i] == ' ')
        ++i;
    }

  return SkipRtfLineEnding(dwCookie, nEolLength);
}
