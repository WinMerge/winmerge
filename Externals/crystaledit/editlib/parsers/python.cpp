///////////////////////////////////////////////////////////////////////////
//  File:    python.cpp
//  Version: 1.1.0.4
//  Updated: 19-Jul-1998
//
//  Copyright:  Ferdinand Prantl, portions by Stcherbatchenko Andrei
//  E-mail:     prantl@ff.cuni.cz
//
//  Python syntax highlighing definition
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

//  Python 3.11.4 Keywords
//  (See https://docs.python.org/3/reference/index.html and https://docs.python.org/3/library/index.html)
static const tchar_t * s_apszPythonKeywordList[] =
  {
    _T ("and"),
    _T ("as"),
    _T ("assert"),
    _T ("async"),
    _T ("await"),
    _T ("break"),
    _T ("class"),
    _T ("continue"),
    _T ("def"),
    _T ("del"),
    _T ("elif"),
    _T ("else"),
    _T ("except"),
    _T ("finally"),
    _T ("for"),
    _T ("from"),
    _T ("global"),
    _T ("if"),
    _T ("import"),
    _T ("in"),
    _T ("is"),
    _T ("lambda"),
    _T ("nonlocal"),
    _T ("not"),
    _T ("or"),
    _T ("pass"),
    _T ("raise"),
    _T ("return"),
    _T ("try"),
    _T ("while"),
    _T ("with"),
    _T ("yield"),
  };

static const tchar_t * s_apszUser1KeywordList[] =
  {
    _T ("ArithmeticError"),
    _T ("AssertionError"),
    _T ("AttributeError"),
    _T ("BaseException"),
    _T ("BaseExceptionGroup"),
    _T ("BlockingIOError"),
    _T ("BrokenPipeError"),
    _T ("BufferError"),
    _T ("BytesWarning"),
    _T ("ChildProcessError"),
    _T ("ConnectionAbortedError"),
    _T ("ConnectionError"),
    _T ("ConnectionRefusedError"),
    _T ("ConnectionResetError"),
    _T ("DeprecationWarning"),
    _T ("EOFError"),
    _T ("Ellipsis"),
    _T ("EncodingWarning"),
    _T ("EnvironmentError"),
    _T ("Exception"),
    _T ("ExceptionGroup"),
    _T ("False"),
    _T ("FileExistsError"),
    _T ("FileNotFoundError"),
    _T ("FloatingPointError"),
    _T ("FutureWarning"),
    _T ("GeneratorExit"),
    _T ("IOError"),
    _T ("ImportError"),
    _T ("ImportWarning"),
    _T ("IndentationError"),
    _T ("IndexError"),
    _T ("InterruptedError"),
    _T ("IsADirectoryError"),
    _T ("KeyError"),
    _T ("KeyboardInterrupt"),
    _T ("LookupError"),
    _T ("MemoryError"),
    _T ("ModuleNotFoundError"),
    _T ("NameError"),
    _T ("None"),
    _T ("NotADirectoryError"),
    _T ("NotImplemented"),
    _T ("NotImplementedError"),
    _T ("OSError"),
    _T ("OverflowError"),
    _T ("PendingDeprecationWarning"),
    _T ("PermissionError"),
    _T ("ProcessLookupError"),
    _T ("RecursionError"),
    _T ("ReferenceError"),
    _T ("ResourceWarning"),
    _T ("RuntimeError"),
    _T ("RuntimeWarning"),
    _T ("StopAsyncIteration"),
    _T ("StopIteration"),
    _T ("SyntaxError"),
    _T ("SyntaxWarning"),
    _T ("SystemError"),
    _T ("SystemExit"),
    _T ("TabError"),
    _T ("TimeoutError"),
    _T ("True"),
    _T ("TypeError"),
    _T ("UnboundLocalError"),
    _T ("UnicodeDecodeError"),
    _T ("UnicodeEncodeError"),
    _T ("UnicodeError"),
    _T ("UnicodeTranslateError"),
    _T ("UnicodeWarning"),
    _T ("UserWarning"),
    _T ("ValueError"),
    _T ("Warning"),
    _T ("WindowsError"),
    _T ("ZeroDivisionError"),
    _T ("__debug__"),
    _T ("argv"),
    _T ("builtin_module_names"),
    _T ("copyright"),
    _T ("credits"),
    _T ("exc_traceback"),
    _T ("exc_type"),
    _T ("exc_value"),
    _T ("exit"),
    _T ("exitfunc"),
    _T ("last_traceback"),
    _T ("last_type"),
    _T ("last_value"),
    _T ("license"),
    _T ("modules"),
    _T ("path"),
    _T ("ps1"),
    _T ("ps2"),
    _T ("quit"),
    _T ("setprofile"),
    _T ("settrace"),
    _T ("stderr"),
    _T ("stdin"),
    _T ("stdout"),
    _T ("tracebacklimit"),
  };

static const tchar_t * s_apszUser2KeywordList[] =
  {
    _T ("__abs__"),
    _T ("__add__"),
    _T ("__aenter__"),
    _T ("__aexit__"),
    _T ("__aiter__"),
    _T ("__and__"),
    _T ("__anext__"),
    _T ("__annotations__"),
    _T ("__await__"),
    _T ("__bases__"),
    _T ("__bool__"),
    _T ("__bytes__"),
    _T ("__cached__"),
    _T ("__call__"),
    _T ("__ceil__"),
    _T ("__class__"),
    _T ("__class_getitem__"),
    _T ("__closure__"),
    _T ("__cmp__"),
    _T ("__code__"),
    _T ("__coerce__"),
    _T ("__complex__"),
    _T ("__contains__"),
    _T ("__defaults__"),
    _T ("__del__"),
    _T ("__delattr__"),
    _T ("__delete__"),
    _T ("__delitem__"),
    _T ("__dict__"),
    _T ("__dir__"),
    _T ("__div__"),
    _T ("__divmod__"),
    _T ("__doc__"),
    _T ("__enter__"),
    _T ("__eq__"),
    _T ("__exit__"),
    _T ("__file__"),
    _T ("__float__"),
    _T ("__floor__"),
    _T ("__floordiv__"),
    _T ("__format__"),
    _T ("__ge__"),
    _T ("__get__"),
    _T ("__getattr__"),
    _T ("__getattribute__"),
    _T ("__getitem__"),
    _T ("__globals__"),
    _T ("__gt__"),
    _T ("__hash__"),
    _T ("__hex__"),
    _T ("__iadd__"),
    _T ("__iand__"),
    _T ("__ifloordiv__"),
    _T ("__ilshift__"),
    _T ("__imatmul__"),
    _T ("__imod__"),
    _T ("__import__"),
    _T ("__imul__"),
    _T ("__index__"),
    _T ("__init__"),
    _T ("__init_subclass__"),
    _T ("__instancecheck__"),
    _T ("__int__"),
    _T ("__invert__"),
    _T ("__ior__"),
    _T ("__ipow__"),
    _T ("__irshift__"),
    _T ("__isub__"),
    _T ("__iter__"),
    _T ("__itruediv__"),
    _T ("__ixor__"),
    _T ("__kwdefaults__"),
    _T ("__le__"),
    _T ("__len__"),
    _T ("__length_hint__"),
    _T ("__loader__"),
    _T ("__long__"),
    _T ("__lshift__"),
    _T ("__lt__"),
    _T ("__match_args__"),
    _T ("__matmul__"),
    _T ("__members__"),
    _T ("__methods__"),
    _T ("__missing__"),
    _T ("__mod__"),
    _T ("__module__"),
    _T ("__mro_entries__"),
    _T ("__mul__"),
    _T ("__name__"),
    _T ("__ne__"),
    _T ("__neg__"),
    _T ("__new__"),
    _T ("__next__"),
    _T ("__nonzero__"),
    _T ("__oct__"),
    _T ("__or__"),
    _T ("__package__"),
    _T ("__path__"),
    _T ("__pos__"),
    _T ("__pow__"),
    _T ("__qualname__"),
    _T ("__radd__"),
    _T ("__rand__"),
    _T ("__rdivmod__"),
    _T ("__repr__"),
    _T ("__reversed__"),
    _T ("__rfloordiv__"),
    _T ("__rlshift__"),
    _T ("__rmatmul__"),
    _T ("__rmod__"),
    _T ("__rmul__"),
    _T ("__ror__"),
    _T ("__round__"),
    _T ("__rpow__"),
    _T ("__rrshift__"),
    _T ("__rshift__"),
    _T ("__rsub__"),
    _T ("__rtruediv__"),
    _T ("__rxor__"),
    _T ("__set__"),
    _T ("__set_name__"),
    _T ("__setattr__"),
    _T ("__setitem__"),
    _T ("__slots__"),
    _T ("__spec__"),
    _T ("__str__"),
    _T ("__sub__"),
    _T ("__subclasscheck__"),
    _T ("__truediv__"),
    _T ("__trunc__"),
    _T ("__xor__"),
    _T ("abs"),
    _T ("aiter"),
    _T ("all"),
    _T ("anext"),
    _T ("any"),
    _T ("ascii"),
    _T ("bin"),
    _T ("bool"),
    _T ("breakpoint"),
    _T ("bytearray"),
    _T ("bytes"),
    _T ("callable"),
    _T ("chr"),
    _T ("classmethod"),
    _T ("coerce"),
    _T ("compile"),
    _T ("complex"),
    _T ("delattr"),
    _T ("dict"),
    _T ("dir"),
    _T ("divmod"),
    _T ("enumerate"),
    _T ("eval"),
    _T ("exec"),
    _T ("filter"),
    _T ("float"),
    _T ("format"),
    _T ("frozenset"),
    _T ("getattr"),
    _T ("globals"),
    _T ("hasattr"),
    _T ("hash"),
    _T ("help"),
    _T ("hex"),
    _T ("id"),
    _T ("input"),
    _T ("int"),
    _T ("isinstance"),
    _T ("issubclass"),
    _T ("iter"),
    _T ("len"),
    _T ("list"),
    _T ("locals"),
    _T ("long"),
    _T ("map"),
    _T ("max"),
    _T ("memoryview"),
    _T ("min"),
    _T ("next"),
    _T ("nonzero"),
    _T ("object"),
    _T ("oct"),
    _T ("open"),
    _T ("ord"),
    _T ("pow"),
    _T ("print"),
    _T ("property"),
    _T ("range"),
    _T ("repr"),
    _T ("reversed"),
    _T ("round"),
    _T ("set"),
    _T ("setattr"),
    _T ("slice"),
    _T ("sorted"),
    _T ("staticmethod"),
    _T ("str"),
    _T ("sum"),
    _T ("super"),
    _T ("tuple"),
    _T ("type"),
    _T ("vars"),
    _T ("xrange"),
    _T ("zip"),
  };

static bool
IsPythonKeyword (const tchar_t *pszChars, int nLength)
{
  return ISXKEYWORD (s_apszPythonKeywordList, pszChars, nLength);
}

static bool
IsUser1Keyword (const tchar_t *pszChars, int nLength)
{
  return ISXKEYWORD (s_apszUser1KeywordList, pszChars, nLength);
}

static bool
IsUser2Keyword (const tchar_t *pszChars, int nLength)
{
  return ISXKEYWORD (s_apszUser2KeywordList, pszChars, nLength);
}

static inline void
DefineIdentiferBlock(const tchar_t *pszChars, int nLength, CrystalLineParser::TEXTBLOCK * pBuf, int &nActualItems, int nIdentBegin, int I)
{
  if (IsPythonKeyword (pszChars + nIdentBegin, I - nIdentBegin))
    {
      DEFINE_BLOCK (nIdentBegin, COLORINDEX_KEYWORD);
    }
  else if (IsUser1Keyword (pszChars + nIdentBegin, I - nIdentBegin))
    {
      DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER1);
    }
  else if (IsUser2Keyword (pszChars + nIdentBegin, I - nIdentBegin))
    {
      DEFINE_BLOCK (nIdentBegin, COLORINDEX_USER2);
    }
  else if (CrystalLineParser::IsXNumber (pszChars + nIdentBegin, I - nIdentBegin))
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

unsigned
CrystalLineParser::ParseLinePython (unsigned dwCookie, const tchar_t *pszChars, int nLength, TEXTBLOCK * pBuf, int &nActualItems)
{
  if (nLength == 0)
    return dwCookie & (COOKIE_EXT_COMMENT | COOKIE_RAWSTRING);

  bool bRedefineBlock = true;
  bool bDecIndex = false;
  int nTripleQuotesBegin = -3;
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
          else if (dwCookie & (COOKIE_CHAR | COOKIE_STRING | COOKIE_RAWSTRING))
            {
              DEFINE_BLOCK (nPos, COLORINDEX_STRING);
            }
          else
            {
              if (xisalnum (pszChars[nPos]) || pszChars[nPos] == '.' && nPos > 0 && (!xisalpha (*tc::tcharprev(pszChars, pszChars + nPos)) && !xisalpha (*tc::tcharnext(pszChars + nPos))))
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

      //  Triple quotes """....""""
      if (dwCookie & COOKIE_RAWSTRING)
        {
          if (I >= 2 && I >= nTripleQuotesBegin + 5 && pszChars[I] == '"' && pszChars[nPrevI] == '"' && *tc::tcharprev(pszChars, pszChars + nPrevI) == '"')
            {
              dwCookie &= ~COOKIE_RAWSTRING;
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

      if (pszChars[I] == '#')
        {
          DEFINE_BLOCK (I, COLORINDEX_COMMENT);
          dwCookie |= COOKIE_COMMENT;
          break;
        }

      //  Normal text or Triple quotes
      if (pszChars[I] == '"')
        {
          //  Triple quotes
          if (I + 2 < nLength && pszChars[I + 1] == '"' && pszChars[I + 2] == '"')
            {
              nTripleQuotesBegin = I;
              DEFINE_BLOCK (I, COLORINDEX_STRING);
              dwCookie |= COOKIE_RAWSTRING;
              continue;
            }

          //  Normal text
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

      if (pBuf == nullptr)
        continue;               //  We don't need to extract keywords,
      //  for faster parsing skip the rest of loop

      if (xisalnum (pszChars[I]) || pszChars[I] == '.' && I > 0 && (!xisalpha (pszChars[nPrevI]) && !xisalpha (pszChars[I + 1])))
        {
          if (nIdentBegin == -1)
            nIdentBegin = I;
        }
      else
        {
          if (nIdentBegin >= 0)
            {
              DefineIdentiferBlock(pszChars, nLength, pBuf, nActualItems, nIdentBegin, I);
              bRedefineBlock = true;
              bDecIndex = true;
              nIdentBegin = -1;
            }
        }
    }

  if (nIdentBegin >= 0)
    {
      DefineIdentiferBlock(pszChars, nLength, pBuf, nActualItems, nIdentBegin, I);
    }

  if (pszChars[nLength - 1] != '\\' || IsMBSTrail(pszChars, nLength - 1))
    dwCookie &= (COOKIE_EXT_COMMENT | COOKIE_RAWSTRING);
  return dwCookie;
}
