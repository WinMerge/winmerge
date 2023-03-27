///////////////////////////////////////////////////////////////////////////
//  File:    php.cpp
//  Version: 1.1.0.4
//  Updated: 19-Jul-1998
//
//  Copyright:  Ferdinand Prantl, portions by Stcherbatchenko Andrei
//  E-mail:     prantl@ff.cuni.cz
//
//  PHP syntax highlighing definition
//
//  You are free to use or modify this code to the following restrictions:
//  - Acknowledge me somewhere in your about box, simple "Parts of code by.."
//  will be enough. If you can't (or don't want to), contact me personally.
//  - LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "crystallineparser.h"
#include "../SyntaxColors.h"
#include "../utils/string_util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// PHP Keywords
// (See https://www.php.net/manual/en/reserved.keywords.php)
static const tchar_t * s_apszPhpKeywordList[] =
  {
    _T ("__halt_compiler"),
    _T ("abstract"),
    _T ("and"),
    _T ("array"),
    _T ("as"),
    _T ("break"),
    _T ("callable"),        // as of PHP 5.4
    _T ("case"),
    _T ("catch"),
    _T ("cfunction"),       // Only PHP4
    _T ("class"),
    _T ("clone"),
    _T ("const"),
    _T ("continue"),
    _T ("declare"),
    _T ("default"),
    _T ("die"),
    _T ("do"),
    _T ("echo"),
    _T ("else"),
    _T ("elseif"),
    _T ("empty"),
    _T ("enddeclare"),
    _T ("endfor"),
    _T ("endforeach"),
    _T ("endif"),
    _T ("endswitch"),
    _T ("endwhile"),
    _T ("eval"),
    _T ("exit"),
    _T ("extends"),
    _T ("final"),
    _T ("finally"),         // as of PHP 5.5
    _T ("fn"),              // as of PHP 7.4
    _T ("for"),
    _T ("foreach"),
    _T ("from"),            // as of PHP 7.0. This is part of "yield from".
    _T ("function"),
    _T ("global"),
    _T ("goto"),            // as of PHP 5.3
    _T ("if"),
    _T ("implements"),
    _T ("include"),
    _T ("include_once"),
    _T ("instanceof"),
    _T ("insteadof"),       // as of PHP 5.4
    _T ("interface"),
    _T ("isset"),
    _T ("list"),
    _T ("match"),           // as of PHP 8.0
    _T ("namespace"),       // as of PHP 5.3
    _T ("new"),
    _T ("old_function"),    // Only PHP4
    _T ("or"),
    _T ("print"),
    _T ("private"),
    _T ("protected"),
    _T ("public"),
    _T ("require"),
    _T ("require_once"),
    _T ("return"),
    _T ("static"),
    _T ("switch"),
    _T ("throw"),
    _T ("trait"),           // as of PHP 5.4
    _T ("try"),
    _T ("unset"),
    _T ("use"),
    _T ("var"),
    _T ("while"),
    _T ("xor"),
    _T ("yield"),           // as of PHP 5.5
  };

// Compile-time constants
// (See https://www.php.net/manual/en/reserved.keywords.php)
static const tchar_t * s_apszCompileTimeConstantList[] =
  {
    _T ("__CLASS__"),
    _T ("__DIR__"),     // as of PHP 5.3
    _T ("__FILE__"),
    _T ("__FUNCTION__"),
    _T ("__LINE__"),
    _T ("__METHOD__"),
    _T ("__NAMESPACE__"), // as of PHP 5.3
    _T ("__TRAIT__"),     // as of PHP 5.4
  };

// Predefined Classes
// (See https://www.php.net/manual/en/reserved.classes.php)
static const tchar_t * s_apszPredefinedClassList[] =
  {
    _T ("__PHP_Incomplete_Class"),
    _T ("ArithmeticError"),         // as of PHP 7.0.0
    _T ("AssertionError"),          // as of PHP 7.0.0
    _T ("Closure"),                 // as of PHP 5.3.0
    _T ("Directory"),
    _T ("DivisionByZeroError"),     // as of PHP 7.0.0
    _T ("Error"),                   // as of PHP 7.0.0
    _T ("ErrorException"),          // as of PHP 5.1.0
    _T ("Exception"),               // as of PHP 5.0.0
    _T ("Generator"),               // as of PHP 5.5.0
    _T ("parent"),
    _T ("ParseError"),              // as of PHP 7.0.0
    _T ("php_user_filter"),         // as of PHP 5.0.0
    _T ("self"),
//  _T ("static"),                  // This is also defined as a PHP keyword, so comment it out.
    _T ("stdClass"),
    _T ("Throwable"),               // as of PHP 7.0.0
    _T ("TypeError"),               // as of PHP 7.0.0

  };

// Predefined Constants
// (See https://www.php.net/manual/en/reserved.constants.php)
static const tchar_t * s_apszPredefinedConstantList[] =
  {
    _T ("__COMPILER_HALT_OFFSET__"),        // as of PHP 5.1.0
    _T ("DEFAULT_INCLUDE_PATH"),
    _T ("E_ALL"),
    _T ("E_COMPILE_ERROR"),
    _T ("E_COMPILE_WARNING"),
    _T ("E_CORE_ERROR"),
    _T ("E_CORE_WARNING"),
    _T ("E_DEPRECATED"),                    // as of PHP 5.3.0
    _T ("E_ERROR"),
    _T ("E_NOTICE"),
    _T ("E_PARSE"),
    _T ("E_RECOVERABLE_ERROR"),             // as of PHP 5.2.0
    _T ("E_STRICT"),
    _T ("E_USER_DEPRECATED"),               // as of PHP 5.3.0
    _T ("E_USER_ERROR"),
    _T ("E_USER_NOTICE"),
    _T ("E_USER_WARNING"),
    _T ("E_WARNING"),
    _T ("false"),
    _T ("null"),
    _T ("PEAR_EXTENSION_DIR"),
    _T ("PEAR_INSTALL_DIR"),
    _T ("PHP_BINARY"),
    _T ("PHP_BINDIR"),
    _T ("PHP_CONFIG_FILE_PATH"),
    _T ("PHP_CONFIG_FILE_SCAN_DIR"),
    _T ("PHP_DATADIR"),
    _T ("PHP_DEBUG"),                       // as of PHP 5.2.7
    _T ("PHP_EOL"),                         // as of PHP 5.0.2
    _T ("PHP_EXTENSION_DIR"),
    _T ("PHP_EXTRA_VERSION"),               // as of PHP 5.2.7
    _T ("PHP_FD_SETSIZE"),                  // as of PHP 7.1.0
    _T ("PHP_FLOAT_DIG"),                   // as of PHP 7.2.0
    _T ("PHP_FLOAT_EPSILON"),               // as of PHP 7.2.0
    _T ("PHP_FLOAT_MAX"),                   // as of PHP 7.2.0
    _T ("PHP_FLOAT_MIN"),                   // as of PHP 7.2.0
    _T ("PHP_INT_MAX"),                     // as of PHP 5.0.5
    _T ("PHP_INT_MIN"),                     // as of PHP 7.0.0
    _T ("PHP_INT_SIZE"),                    // as of PHP 5.0.5
    _T ("PHP_LIBDIR"),
    _T ("PHP_LOCALSTATEDIR"),
    _T ("PHP_MAJOR_VERSION"),               // as of PHP 5.2.7
    _T ("PHP_MANDIR"),
    _T ("PHP_MAXPATHLEN"),                  // as of PHP 5.3.0
    _T ("PHP_MINOR_VERSION"),               // as of PHP 5.2.7
    _T ("PHP_OS"),
    _T ("PHP_OS_FAMILY"),                   // as of PHP 7.2.0
    _T ("PHP_PREFIX"),
    _T ("PHP_RELEASE_VERSION"),             // as of PHP 5.2.7
    _T ("PHP_SAPI"),
    _T ("PHP_SHLIB_SUFFIX"),
    _T ("PHP_SYSCONFDIR"),
    _T ("PHP_VERSION"),
    _T ("PHP_VERSION_ID"),                  // as of PHP 5.2.7
    _T ("PHP_WINDOWS_EVENT_CTRL_BREAK"),    // as of PHP 7.4.0
    _T ("PHP_WINDOWS_EVENT_CTRL_C"),        // as of PHP 7.4.0
    _T ("PHP_ZTS"),                         // as of PHP 5.2.7
    _T ("true"),
  };

// Reserved words
// (See https://www.php.net/manual/en/reserved.other-reserved-words.php)
static const tchar_t * s_apszReservedWordList[] =
  {
    _T ("bool"),        // as of PHP 7
//  _T ("false"),       // as of PHP 7.  This is also defined as a predefined constant, so comment it out.
    _T ("float"),       // as of PHP 7
    _T ("int"),         // as of PHP 7
    _T ("iterable"),    // as of PHP 7.1
    _T ("mixed"),       // as of PHP 7
//  _T ("null"),        // as of PHP 7.  This is also defined as a predefined constant, so comment it out.
    _T ("numeric"),     // as of PHP 7
    _T ("object"),      // as of PHP 7.2
    _T ("resource"),    // as of PHP 7
    _T ("string"),      // as of PHP 7
//  _T ("true"),        // as of PHP 7.  This is also defined as a predefined constant, so comment it out.
    _T ("void"),        // as of PHP 7.1
  };

static bool
IsPhpKeyword (const tchar_t *pszChars, int nLength)
{
  return ISXKEYWORDI (s_apszPhpKeywordList, pszChars, nLength) ||
         ISXKEYWORDI (s_apszReservedWordList, pszChars, nLength);
}

static bool
IsPhp1Keyword (const tchar_t *pszChars, int nLength)
{
  return false;
}

static bool
IsPhp2Keyword (const tchar_t *pszChars, int nLength)
{
  return ISXKEYWORDI (s_apszCompileTimeConstantList, pszChars, nLength) ||
         ISXKEYWORDI (s_apszPredefinedClassList, pszChars, nLength) ||
         ISXKEYWORDI (s_apszPredefinedConstantList, pszChars, nLength);
}

unsigned
CrystalLineParser::ParseLinePhp(unsigned dwCookie, const tchar_t* pszChars, int nLength, TEXTBLOCK* pBuf, int& nActualItems)
{
  return ParseLineHtmlEx(dwCookie, pszChars, nLength, pBuf, nActualItems, SRC_PHP);
}

unsigned
CrystalLineParser::ParseLinePhpLanguage (unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems)
{
  if (nLength == 0)
    return dwCookie & (COOKIE_EXT_COMMENT | COOKIE_STRING | COOKIE_CHAR);

  const tchar_t *pszCommentBegin = nullptr;
  const tchar_t *pszCommentEnd = nullptr;
  bool bRedefineBlock = true;
  bool bDecIndex = false;
  int nIdentBegin = -1;
  int nPrevI = -1;
  int I=0;
  for (I = 0;; nPrevI = I, I = static_cast<int>(tc::tcharnext(pszChars+I) - pszChars))
    {
      if (I == nPrevI)
        {
          // CharNext did not advance, so we're at the end of the string
          // and we already handled this character, so stop
          break;
        }

      if (bRedefineBlock)
        {
          int nPos = I;
          if (bDecIndex)
            nPos = nPrevI;
          if (dwCookie & (COOKIE_COMMENT | COOKIE_EXT_COMMENT))
            {
              DEFINE_BLOCK (nPos, COLORINDEX_COMMENT);
            }
          else if (dwCookie & (COOKIE_CHAR | COOKIE_STRING))
            {
              DEFINE_BLOCK (nPos, COLORINDEX_STRING);
            }
          else if (dwCookie & COOKIE_PREPROCESSOR)
            {
              DEFINE_BLOCK (nPos, COLORINDEX_PREPROCESSOR);
            }
          else
            {
              if (xisalnum (pszChars[nPos]) || pszChars[nPos] == '.')
                {
                  DEFINE_BLOCK (nPos, COLORINDEX_NORMALTEXT);
                }
              else
                {
                  DEFINE_BLOCK (nPos, COLORINDEX_OPERATOR);
                  bRedefineBlock = true;
                  bDecIndex = true;
                  goto out;
                }
            }
          bRedefineBlock = false;
          bDecIndex = false;
        }
out:

      // Can be bigger than length if there is binary data
      // See bug #1474782 Crash when comparing SQL with with binary data
      if (I >= nLength || pszChars[I] == 0)
        break;

      if (dwCookie & COOKIE_COMMENT)
        {
          DEFINE_BLOCK (I, COLORINDEX_COMMENT);
          dwCookie |= COOKIE_COMMENT;
          break;
        }

      //  String constant "...."
      if (dwCookie & COOKIE_STRING)
        {
          if (pszChars[I] == '"' && (I == 0 || I == 1 && pszChars[nPrevI] != '\\' || I >= 2 && (pszChars[nPrevI] != '\\' || *tc::tcharprev(pszChars, pszChars + nPrevI) == '\\')))
            {
              dwCookie &= ~COOKIE_STRING;
              bRedefineBlock = true;
            }
          continue;
        }

      //  Char constant '..'
      if (dwCookie & COOKIE_CHAR)
        {
          if (pszChars[I] == '\'' && (I == 0 || I == 1 && pszChars[nPrevI] != '\\' || I >= 2 && (pszChars[nPrevI] != '\\' || *tc::tcharprev(pszChars, pszChars + nPrevI) == '\\')))
            {
              dwCookie &= ~COOKIE_CHAR;
              bRedefineBlock = true;
            }
          continue;
        }

      //  Extended comment <!--....-->
      if (dwCookie & COOKIE_EXT_COMMENT)
        {
          if ((pszCommentBegin < pszChars + I) && (I > 0 && pszChars[I] == '/' && pszChars[nPrevI] == '*'))
            {
              dwCookie &= ~COOKIE_EXT_COMMENT;
              bRedefineBlock = true;
              pszCommentEnd = pszChars + I + 1;
            }
          continue;
        }

      if ((pszCommentEnd < pszChars + I) && (I > 0 && pszChars[I] == '/' && pszChars[nPrevI] == '/'))
        {
          DEFINE_BLOCK (nPrevI, COLORINDEX_COMMENT);
          dwCookie |= COOKIE_COMMENT;
          break;
        }

      if (pszChars[I] == '#')
        {
          DEFINE_BLOCK (I, COLORINDEX_COMMENT);
          dwCookie |= COOKIE_COMMENT;
          break;
        }

      //  Normal text
      if (pszChars[I] == '"')
        {
          DEFINE_BLOCK (I, COLORINDEX_STRING);
          dwCookie |= COOKIE_STRING;
          continue;
        }

      if (pszChars[I] == '\'')
        {
          // if (I + 1 < nLength && pszChars[I + 1] == '\'' || I + 2 < nLength && pszChars[I + 1] != '\\' && pszChars[I + 2] == '\'' || I + 3 < nLength && pszChars[I + 1] == '\\' && pszChars[I + 3] == '\'')
          if (!I || !xisalnum (pszChars[nPrevI]))
            {
              DEFINE_BLOCK (I, COLORINDEX_STRING);
              dwCookie |= COOKIE_CHAR;
              continue;
            }
        }

      if ((pszCommentEnd < pszChars + I) && (I > 0 && pszChars[I] == '*' && pszChars[nPrevI] == '/'))
        {
          DEFINE_BLOCK (nPrevI, COLORINDEX_COMMENT);
          dwCookie |= COOKIE_EXT_COMMENT;
          pszCommentBegin = pszChars + I + 1;
          continue;
        }

      if (pBuf == nullptr)
        continue;               //  We don't need to extract keywords,
      //  for faster parsing skip the rest of loop

      if (xisalnum (pszChars[I]) || pszChars[I] == '.')
        {
          if (nIdentBegin == -1)
            nIdentBegin = I;
        }
      else
        {
          if (nIdentBegin >= 0)
            {
              if (dwCookie & COOKIE_USER2)
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER1);
                }
              if (IsPhpKeyword (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
                }
              else if (IsPhp1Keyword (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_OPERATOR);
                }
              else if (IsPhp2Keyword (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER2);
                }
              else if (IsXNumber (pszChars + nIdentBegin, I - nIdentBegin))
                {
                  DEFINE_BLOCK (nIdentBegin, COLORINDEX_NUMBER);
                }
              else
                {
                  bool bFunction = false;

                  for (int j = I; j < nLength; j++)
                    {
                      if (!xisspace (pszChars[j]))
                        {
                          if (pszChars[j] == '(')
                            {
                              bFunction = true;
                            }
                          break;
                        }
                    }
                  if (bFunction)
                    {
                      DEFINE_BLOCK (nIdentBegin, COLORINDEX_FUNCNAME);
                    }
                }
              bRedefineBlock = true;
              bDecIndex = true;
              nIdentBegin = -1;
            }

          //  Preprocessor start: $
          if (pszChars[I] == '$')
            {
              dwCookie |= COOKIE_USER2;
              nIdentBegin = -1;
              continue;
            }

          //  Preprocessor end: ...
          if (dwCookie & COOKIE_USER2)
            {
              if (!xisalnum (pszChars[I]))
                {
                  dwCookie &= ~COOKIE_USER2;
                  nIdentBegin = -1;
                  continue;
                }
            }
        }
    }

  if (nIdentBegin >= 0)
    {
      if (dwCookie & COOKIE_USER2)
        {
          DEFINE_BLOCK(nIdentBegin, COLORINDEX_USER1);
        }
      if (IsPhpKeyword (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
        }
      else if (IsPhp1Keyword (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_OPERATOR);
        }
      else if (IsPhp2Keyword (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER2);
        }
      else if (IsXNumber (pszChars + nIdentBegin, I - nIdentBegin))
        {
          DEFINE_BLOCK (nIdentBegin, COLORINDEX_NUMBER);
        }
      else
        {
          bool bFunction = false;

          for (int j = I; j < nLength; j++)
            {
              if (!xisspace (pszChars[j]))
                {
                  if (pszChars[j] == '(')
                    {
                      bFunction = true;
                    }
                  break;
                }
            }
          if (bFunction)
            {
              DEFINE_BLOCK (nIdentBegin, COLORINDEX_FUNCNAME);
            }
        }
    }

  dwCookie &= (COOKIE_EXT_COMMENT | COOKIE_STRING | COOKIE_CHAR);
  return dwCookie;
}
