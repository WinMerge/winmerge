///////////////////////////////////////////////////////////////////////////
//  File:       fsharp.cpp
//  Version:    1.1.0
//  Created:    27-Apr-2024
//
//  Copyright:  Stcherbatchenko Andrei
//  E-mail:     windfall@gmx.de
//
//  Implementation of the CCrystalEditView class, a part of the Crystal Edit -
//  syntax coloring text editor. Based on C# version.
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

// The keyword lists are searched with a binary search: keep them sorted by
// character code (upper case < '_' < lower case).

//  F# keywords and reserved words
// https://learn.microsoft.com/en-us/dotnet/fsharp/language-reference/keyword-reference
static const tchar_t * s_apszFsKeywordList[] =
  {
    _T ("abstract"),
    _T ("and"),
    _T ("as"),
    _T ("asr"),
    _T ("assert"),
    _T ("async"),
    _T ("base"),
    _T ("begin"),
    _T ("break"),
    _T ("checked"),
    _T ("class"),
    _T ("component"),
    _T ("const"),
    _T ("constraint"),
    _T ("continue"),
    _T ("default"),
    _T ("delegate"),
    _T ("do"),
    _T ("done"),
    _T ("downcast"),
    _T ("downto"),
    _T ("elif"),
    _T ("else"),
    _T ("end"),
    _T ("event"),
    _T ("exception"),
    _T ("extern"),
    _T ("external"),
    _T ("false"),
    _T ("finally"),
    _T ("fixed"),
    _T ("for"),
    _T ("fun"),
    _T ("function"),
    _T ("global"),
    _T ("if"),
    _T ("in"),
    _T ("include"),
    _T ("inherit"),
    _T ("inline"),
    _T ("interface"),
    _T ("internal"),
    _T ("land"),
    _T ("lazy"),
    _T ("let"),
    _T ("lor"),
    _T ("lsl"),
    _T ("lsr"),
    _T ("lxor"),
    _T ("match"),
    _T ("member"),
    _T ("mixin"),
    _T ("mod"),
    _T ("module"),
    _T ("mutable"),
    _T ("namespace"),
    _T ("new"),
    _T ("not"),
    _T ("null"),
    _T ("of"),
    _T ("open"),
    _T ("or"),
    _T ("override"),
    _T ("parallel"),
    _T ("private"),
    _T ("process"),
    _T ("protected"),
    _T ("public"),
    _T ("pure"),
    _T ("query"),
    _T ("rec"),
    _T ("return"),
    _T ("sealed"),
    _T ("select"),
    _T ("seq"),
    _T ("sig"),
    _T ("static"),
    _T ("struct"),
    _T ("tailcall"),
    _T ("task"),
    _T ("then"),
    _T ("to"),
    _T ("trait"),
    _T ("true"),
    _T ("try"),
    _T ("type"),
    _T ("upcast"),
    _T ("use"),
    _T ("val"),
    _T ("virtual"),
    _T ("void"),
    _T ("when"),
    _T ("where"),
    _T ("while"),
    _T ("with"),
    _T ("yield"),
  };

//  Keywords that also have a "!" form in computation expressions (let!, match!, ...)
static const tchar_t * s_apszFsBangKeywordList[] =
  {
    _T ("and"),
    _T ("do"),
    _T ("let"),
    _T ("match"),
    _T ("return"),
    _T ("use"),
    _T ("while"),
    _T ("yield"),
  };

//  Built-in types, FSharp.Core types and cases, source constants
// https://learn.microsoft.com/en-us/dotnet/fsharp/language-reference/basic-types
static const tchar_t * s_apszUser1KeywordList[] =
  {
    _T ("Array"),
    _T ("Async"),
    _T ("Char"),
    _T ("DateTime"),
    _T ("Decimal"),
    _T ("Error"),
    _T ("Guid"),
    _T ("Int16"),
    _T ("Int32"),
    _T ("Int64"),
    _T ("IntPtr"),
    _T ("List"),
    _T ("Map"),
    _T ("None"),
    _T ("Ok"),
    _T ("Option"),
    _T ("Result"),
    _T ("Seq"),
    _T ("Set"),
    _T ("Some"),
    _T ("String"),
    _T ("Task"),
    _T ("Type"),
    _T ("UInt16"),
    _T ("UInt32"),
    _T ("UInt64"),
    _T ("UIntPtr"),
    _T ("Unit"),
    _T ("ValueNone"),
    _T ("ValueOption"),
    _T ("ValueSome"),
    _T ("__LINE__"),
    _T ("__SOURCE_DIRECTORY__"),
    _T ("__SOURCE_FILE__"),
    _T ("bigint"),
    _T ("bool"),
    _T ("byref"),
    _T ("byte"),
    _T ("char"),
    _T ("decimal"),
    _T ("double"),
    _T ("exn"),
    _T ("float"),
    _T ("float32"),
    _T ("int"),
    _T ("int16"),
    _T ("int32"),
    _T ("int64"),
    _T ("int8"),
    _T ("list"),
    _T ("nativeint"),
    _T ("nativeptr"),
    _T ("obj"),
    _T ("option"),
    _T ("sbyte"),
    _T ("single"),
    _T ("string"),
    _T ("uint"),
    _T ("uint16"),
    _T ("uint32"),
    _T ("uint64"),
    _T ("uint8"),
    _T ("unativeint"),
    _T ("unit"),
    _T ("voption"),
  };

static bool
IsFsKeyword (const tchar_t *pszChars, int nLength)
{
  return ISXKEYWORD (s_apszFsKeywordList, pszChars, nLength);
}

static bool
IsFsBangKeyword (const tchar_t *pszChars, int nLength)
{
  return ISXKEYWORD (s_apszFsBangKeywordList, pszChars, nLength);
}

static bool
IsUserKeyword (const tchar_t *pszChars, int nLength)
{
  return ISXKEYWORD (s_apszUser1KeywordList, pszChars, nLength);
}

//  Verbatim string @"...": "" is the only escape
#define COOKIE_VERBATIM COOKIE_EXT_USER1
//  Nesting depth of (* *) comments is kept in COOKIE_SET_EXT_COMMENT_DEPTH bits
static constexpr unsigned COOKIE_FS_DEPTH_MASK = 0x0F000000;
static constexpr unsigned COOKIE_FS_MAX_DEPTH = 15;
static constexpr unsigned COOKIE_FS_MULTILINE = COOKIE_EXT_COMMENT | COOKIE_STRING | COOKIE_VERBATIM | COOKIE_RAWSTRING | COOKIE_FS_DEPTH_MASK;

namespace
{
  // What the next identifier on the line is, from the tokens before it
  enum class FsContext
  {
    None,           // expression: a name followed by "(" is a call
    LetName,        // after let/use: a name followed by parameters is a function
    AndName,        // after and: an upper-case name is a type, otherwise as LetName
    MemberName,     // after member/override/abstract/default: [self.]Name
    TypeName,       // after type/exception
    TypePath,       // after open/module/namespace/inherit/new/interface: A.B.C
    TypeAnnotation, // after : :> :? :?> of, until = , ; ) ] } | or a keyword
    TypeArguments,  // inside <...> after a type name
    Attribute,      // after [< or ; in an attribute: the attribute name
    AttributeArgs,  // the rest of an attribute, until ; or >]
    ActivePattern,  // (|A|B|_|) after let/and/member
  };

  inline bool IsIdentStart (tchar_t c)
  {
    return xisalpha (c) != 0;
  }

  inline bool IsIdentChar (tchar_t c)
  {
    return xisalnum (c) != 0 || c == '\'';
  }

  inline bool IsDigit (tchar_t c)
  {
    return c >= '0' && c <= '9';
  }

  inline bool IsOperatorChar (tchar_t c)
  {
    switch (c)
      {
      case '!': case '%': case '&': case '*': case '+': case '-': case '.': case '/':
      case '<': case '=': case '>': case '?': case '@': case '^': case '|': case '~':
        return true;
      default:
        return false;
      }
  }

  inline bool IsWord (const tchar_t *pszChars, int nLength, const tchar_t *pszWord)
  {
    return tc::tcsncmp (pszChars, pszWord, nLength) == 0 && pszWord[nLength] == 0;
  }

  inline int SkipSpaces (const tchar_t *pszChars, int nLength, int I)
  {
    while (I < nLength && xisspace (pszChars[I]))
      I++;
    return I;
  }

  // "(*" that starts a comment: "(*)" is the multiplication operator
  inline bool IsCommentStart (const tchar_t *pszChars, int nLength, int I)
  {
    return I + 1 < nLength && pszChars[I] == '(' && pszChars[I + 1] == '*'
      && !(I + 2 < nLength && pszChars[I + 2] == ')');
  }

  // The context after a keyword
  FsContext ContextAfterKeyword (FsContext ctx, const tchar_t *pszChars, int nLength)
  {
    if (IsWord (pszChars, nLength, _T ("let")) || IsWord (pszChars, nLength, _T ("use")))
      return FsContext::LetName;
    if (IsWord (pszChars, nLength, _T ("and")))
      return FsContext::AndName;
    if (IsWord (pszChars, nLength, _T ("member")) || IsWord (pszChars, nLength, _T ("override"))
        || IsWord (pszChars, nLength, _T ("abstract")) || IsWord (pszChars, nLength, _T ("default")))
      return FsContext::MemberName;
    if (IsWord (pszChars, nLength, _T ("type")))
      return ctx == FsContext::TypePath ? FsContext::TypePath : FsContext::TypeName;   // open type X
    if (IsWord (pszChars, nLength, _T ("exception")))
      return FsContext::TypeName;
    if (IsWord (pszChars, nLength, _T ("open")) || IsWord (pszChars, nLength, _T ("module"))
        || IsWord (pszChars, nLength, _T ("namespace")) || IsWord (pszChars, nLength, _T ("inherit"))
        || IsWord (pszChars, nLength, _T ("new")) || IsWord (pszChars, nLength, _T ("interface")))
      return FsContext::TypePath;
    if (IsWord (pszChars, nLength, _T ("of")))
      return FsContext::TypeAnnotation;
    // modifiers between the keyword and the name
    if (ctx == FsContext::LetName || ctx == FsContext::AndName || ctx == FsContext::MemberName
        || ctx == FsContext::TypeName || ctx == FsContext::TypePath)
      {
        if (IsWord (pszChars, nLength, _T ("rec")) || IsWord (pszChars, nLength, _T ("inline"))
            || IsWord (pszChars, nLength, _T ("mutable")) || IsWord (pszChars, nLength, _T ("private"))
            || IsWord (pszChars, nLength, _T ("internal")) || IsWord (pszChars, nLength, _T ("public")))
          return ctx;
        if (ctx == FsContext::MemberName && IsWord (pszChars, nLength, _T ("val")))
          return ctx;   // member val X = ...
      }
    // keywords that are types or appear inside types
    if (ctx == FsContext::TypeArguments)
      return ctx;
    if (ctx == FsContext::TypeAnnotation
        && (IsWord (pszChars, nLength, _T ("seq")) || IsWord (pszChars, nLength, _T ("async"))
            || IsWord (pszChars, nLength, _T ("task")) || IsWord (pszChars, nLength, _T ("null"))))
      return ctx;
    return FsContext::None;
  }
}

unsigned
CrystalLineParser::ParseLineFSharp (unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf)
{
  dwCookie &= COOKIE_FS_MULTILINE;
  if (nLength == 0)
    return dwCookie;

  FsContext ctx = FsContext::None;
  FsContext ctxAfterAttribute = FsContext::None;
  FsContext ctxAfterTypeArgs = FsContext::None;
  int nTypeArgsDepth = 0;
  bool bLineStart = true;

  // Enter <...> when a type name is directly followed by "<"
  auto enterTypeArguments = [&] (int nNext)
    {
      if (nNext < nLength && pszChars[nNext] == '<'
          && (ctx == FsContext::TypeAnnotation || ctx == FsContext::TypePath))
        {
          ctxAfterTypeArgs = ctx == FsContext::TypeAnnotation ? FsContext::TypeAnnotation : FsContext::None;
          ctx = FsContext::TypeArguments;
          nTypeArgsDepth = 0;
          return true;
        }
      return false;
    };

  // Color an identifier that is neither a keyword nor a built-in name (or is
  // in a name position), and update the context
  auto defineName = [&] (int nBegin, int nEnd)
    {
      const int nNext = SkipSpaces (pszChars, nLength, nEnd);
      switch (ctx)
        {
        case FsContext::AndName:
          if (pszChars[nBegin] >= 'A' && pszChars[nBegin] <= 'Z')
            {
              DEFINE_BLOCK (nBegin, COLORINDEX_USER1);
              ctx = FsContext::None;
              break;
            }
          [[fallthrough]];
        case FsContext::LetName:
          {
            bool bFunction = false;
            if (nNext < nLength)
              {
                const tchar_t c = pszChars[nNext];
                bFunction = IsIdentStart (c) || c == '<' || c == '`'
                  || (c == '(' && !IsCommentStart (pszChars, nLength, nNext));
              }
            DEFINE_BLOCK (nBegin, bFunction ? COLORINDEX_FUNCNAME : COLORINDEX_NORMALTEXT);
            ctx = FsContext::None;
          }
          break;
        case FsContext::MemberName:
          if (nEnd < nLength && pszChars[nEnd] == '.')
            DEFINE_BLOCK (nBegin, COLORINDEX_NORMALTEXT);   // self identifier: this.Name
          else
            {
              DEFINE_BLOCK (nBegin, COLORINDEX_FUNCNAME);
              ctx = FsContext::None;
            }
          break;
        case FsContext::TypeName:
          DEFINE_BLOCK (nBegin, COLORINDEX_USER1);
          ctx = FsContext::None;
          break;
        case FsContext::TypePath:
          DEFINE_BLOCK (nBegin, COLORINDEX_USER1);
          if (!enterTypeArguments (nEnd) && !(nEnd < nLength && pszChars[nEnd] == '.'))
            ctx = FsContext::None;
          break;
        case FsContext::TypeAnnotation:
          DEFINE_BLOCK (nBegin, COLORINDEX_USER1);
          enterTypeArguments (nEnd);
          break;
        case FsContext::TypeArguments:
          DEFINE_BLOCK (nBegin, COLORINDEX_USER1);
          break;
        case FsContext::Attribute:
          DEFINE_BLOCK (nBegin, COLORINDEX_PREPROCESSOR);
          if (!(nEnd < nLength && pszChars[nEnd] == '.'))
            ctx = FsContext::AttributeArgs;
          break;
        case FsContext::AttributeArgs:
          DEFINE_BLOCK (nBegin, COLORINDEX_NORMALTEXT);
          break;
        case FsContext::ActivePattern:
          DEFINE_BLOCK (nBegin, COLORINDEX_FUNCNAME);
          break;
        default:
          // a call: name (...) or name(...)
          DEFINE_BLOCK (nBegin, nNext < nLength && pszChars[nNext] == '(' && !IsCommentStart (pszChars, nLength, nNext)
            ? COLORINDEX_FUNCNAME : COLORINDEX_NORMALTEXT);
          break;
        }
    };

  // the view draws from the first block: it must start at column 0
  DEFINE_BLOCK (0, COLORINDEX_NORMALTEXT);

  int I = 0;
  while (I < nLength && pszChars[I] != 0)
    {
      //  Block comment (* ... *), may be nested
      if (dwCookie & COOKIE_EXT_COMMENT)
        {
          DEFINE_BLOCK (I, COLORINDEX_COMMENT);
          unsigned depth = COOKIE_GET_EXT_COMMENT_DEPTH (dwCookie);
          while (I < nLength)
            {
              const tchar_t c = pszChars[I];
              if (c == '(' && I + 1 < nLength && pszChars[I + 1] == '*')
                {
                  if (IsCommentStart (pszChars, nLength, I))
                    {
                      if (depth < COOKIE_FS_MAX_DEPTH)
                        depth++;
                      I += 2;
                    }
                  else
                    I += 3;   // (*)
                  continue;
                }
              if (c == '*' && I + 1 < nLength && pszChars[I + 1] == ')')
                {
                  I += 2;
                  if (depth == 0)
                    {
                      dwCookie &= ~COOKIE_EXT_COMMENT;
                      break;
                    }
                  depth--;
                  continue;
                }
              if (c == '"')
                {
                  // a string inside a comment hides "*)"
                  int J = I + 1;
                  while (J < nLength && pszChars[J] != '"')
                    J += pszChars[J] == '\\' ? 2 : 1;
                  I = J < nLength ? J + 1 : I + 1;
                  continue;
                }
              I++;
            }
          COOKIE_SET_EXT_COMMENT_DEPTH (dwCookie, depth);
          continue;
        }

      //  Strings: "...", @"...", """..."""; $ prefixes do not change where they end
      if (dwCookie & (COOKIE_STRING | COOKIE_RAWSTRING))
        {
          DEFINE_BLOCK (I, COLORINDEX_STRING);
          bool bClosed = false;
          if (dwCookie & COOKIE_RAWSTRING)
            {
              for (; I < nLength; I++)
                if (pszChars[I] == '"' && I + 2 < nLength && pszChars[I + 1] == '"' && pszChars[I + 2] == '"')
                  {
                    I += 3;
                    bClosed = true;
                    break;
                  }
            }
          else
            {
              const bool bVerbatim = (dwCookie & COOKIE_VERBATIM) != 0;
              while (I < nLength)
                {
                  if (!bVerbatim && pszChars[I] == '\\')
                    I += 2;
                  else if (pszChars[I] == '"')
                    {
                      if (bVerbatim && I + 1 < nLength && pszChars[I + 1] == '"')
                        I += 2;
                      else
                        {
                          I++;
                          bClosed = true;
                          break;
                        }
                    }
                  else
                    I++;
                }
            }
          if (!bClosed)
            break;
          dwCookie &= ~(COOKIE_STRING | COOKIE_VERBATIM | COOKIE_RAWSTRING);
          // byte array "abc"B
          if (I < nLength && pszChars[I] == 'B' && !(I + 1 < nLength && IsIdentChar (pszChars[I + 1])))
            I++;
          if (ctx != FsContext::AttributeArgs)
            ctx = FsContext::None;
          continue;
        }

      const tchar_t c = pszChars[I];
      if (xisspace (c))
        {
          I++;
          continue;
        }
      const bool bFirstOnLine = bLineStart;
      bLineStart = false;

      //  Line comment
      if (c == '/' && I + 1 < nLength && pszChars[I + 1] == '/')
        {
          DEFINE_BLOCK (I, COLORINDEX_COMMENT);
          break;
        }

      //  Block comment
      if (IsCommentStart (pszChars, nLength, I))
        {
          DEFINE_BLOCK (I, COLORINDEX_COMMENT);
          dwCookie |= COOKIE_EXT_COMMENT;
          COOKIE_SET_EXT_COMMENT_DEPTH (dwCookie, 0);
          I += 2;
          continue;
        }

      //  Directives: #if/#else/#endif color the whole line, other directives
      //  (#r, #load, #nowarn, ...) only the name
      if (c == '#' && bFirstOnLine)
        {
          if (I + 1 < nLength && pszChars[I + 1] == '!')
            {
              DEFINE_BLOCK (I, COLORINDEX_PREPROCESSOR);   // #!/usr/bin/env dotnet fsi
              break;
            }
          int J = I + 1;
          while (J < nLength && xisalpha (pszChars[J]))
            J++;
          if (J > I + 1)
            {
              DEFINE_BLOCK (I, COLORINDEX_PREPROCESSOR);
              const tchar_t *pszName = pszChars + I + 1;
              const int nName = J - I - 1;
              if (IsWord (pszName, nName, _T ("if")) || IsWord (pszName, nName, _T ("elif"))
                  || IsWord (pszName, nName, _T ("else")) || IsWord (pszName, nName, _T ("endif")))
                {
                  while (J < nLength && !(pszChars[J] == '/' && J + 1 < nLength && pszChars[J + 1] == '/'))
                    J++;
                  if (J < nLength)
                    DEFINE_BLOCK (J, COLORINDEX_COMMENT);
                  break;
                }
              I = J;
              continue;
            }
        }

      //  String start
      if (c == '"' || c == '$' || c == '@')
        {
          int J = I;
          int nAt = 0;
          while (J < nLength && (pszChars[J] == '$' || pszChars[J] == '@'))
            {
              if (pszChars[J] == '@')
                nAt++;
              J++;
            }
          if (J < nLength && pszChars[J] == '"' && nAt <= 1)
            {
              DEFINE_BLOCK (I, COLORINDEX_STRING);
              if (J + 2 < nLength && pszChars[J + 1] == '"' && pszChars[J + 2] == '"')
                {
                  dwCookie |= COOKIE_RAWSTRING;
                  I = J + 3;
                }
              else
                {
                  dwCookie |= COOKIE_STRING | (nAt ? COOKIE_VERBATIM : 0);
                  I = J + 1;
                }
              continue;
            }
        }

      //  Character literal 'a', '\n', 'A', or type variable 'T
      if (c == '\'')
        {
          int nEnd = -1;
          if (I + 1 < nLength && pszChars[I + 1] == '\\')
            {
              for (int J = I + 3; J < nLength && J <= I + 11; J++)
                if (pszChars[J] == '\'')
                  {
                    nEnd = J;
                    break;
                  }
            }
          else if (I + 2 < nLength && pszChars[I + 2] == '\'' && pszChars[I + 1] != '\'')
            nEnd = I + 2;
          if (nEnd >= 0)
            {
              DEFINE_BLOCK (I, COLORINDEX_STRING);
              I = nEnd + 1;
              if (I < nLength && pszChars[I] == 'B' && !(I + 1 < nLength && IsIdentChar (pszChars[I + 1])))
                I++;
              continue;
            }
          if (I + 1 < nLength && IsIdentStart (pszChars[I + 1]))
            {
              int J = I + 1;
              while (J < nLength && IsIdentChar (pszChars[J]))
                J++;
              DEFINE_BLOCK (I, COLORINDEX_USER1);
              I = J;
              continue;
            }
          DEFINE_BLOCK (I, COLORINDEX_OPERATOR);
          I++;
          continue;
        }

      //  Number: 42, 0x2A, 0b101, 1_000L, 1.5e-3f, 3.14M, 2uy
      if (IsDigit (c))
        {
          const bool bHex = c == '0' && I + 1 < nLength && (pszChars[I + 1] == 'x' || pszChars[I + 1] == 'X');
          int J = I + 1;
          while (J < nLength)
            {
              const tchar_t d = pszChars[J];
              if (xisalnum (d))
                {
                  if (!bHex && (d == 'e' || d == 'E') && J + 1 < nLength && (pszChars[J + 1] == '+' || pszChars[J + 1] == '-'))
                    J += 2;
                  else
                    J++;
                }
              else if (d == '.' && J + 1 < nLength && IsDigit (pszChars[J + 1]))
                J++;
              else
                break;
            }
          DEFINE_BLOCK (I, COLORINDEX_NUMBER);
          I = J;
          continue;
        }

      //  ``name with spaces``
      if (c == '`' && I + 1 < nLength && pszChars[I + 1] == '`')
        {
          int J = I + 2;
          while (J + 1 < nLength && !(pszChars[J] == '`' && pszChars[J + 1] == '`'))
            J++;
          const int nEnd = J + 1 < nLength ? J + 2 : nLength;
          defineName (I, nEnd);
          I = nEnd;
          continue;
        }

      //  Identifier
      if (IsIdentStart (c))
        {
          int J = I + 1;
          while (J < nLength && IsIdentChar (pszChars[J]))
            J++;
          const tchar_t *pszWord = pszChars + I;
          const int nWord = J - I;
          const bool bNamePosition = ctx == FsContext::LetName || ctx == FsContext::AndName
            || ctx == FsContext::MemberName || ctx == FsContext::TypeName
            || ctx == FsContext::Attribute || ctx == FsContext::ActivePattern;
          if (IsFsKeyword (pszWord, nWord))
            {
              DEFINE_BLOCK (I, COLORINDEX_KEYWORD);
              if (J < nLength && pszChars[J] == '!' && IsFsBangKeyword (pszWord, nWord))
                J++;   // let!, match!, ...
              ctx = ContextAfterKeyword (ctx, pszWord, nWord);
              if (ctx == FsContext::TypeAnnotation)
                enterTypeArguments (J);   // seq<Foo>
            }
          else if (!bNamePosition && IsUserKeyword (pszWord, nWord))
            {
              DEFINE_BLOCK (I, COLORINDEX_USER1);
              if (ctx == FsContext::TypePath)
                {
                  if (!enterTypeArguments (J) && !(J < nLength && pszChars[J] == '.'))
                    ctx = FsContext::None;
                }
              else if (ctx == FsContext::TypeAnnotation)
                enterTypeArguments (J);
              else if (ctx != FsContext::TypeArguments && ctx != FsContext::AttributeArgs)
                ctx = FsContext::None;
            }
          else
            defineName (I, J);
          I = J;
          continue;
        }

      //  Operators and punctuation
      DEFINE_BLOCK (I, COLORINDEX_OPERATOR);
      const tchar_t next = I + 1 < nLength ? pszChars[I + 1] : 0;
      // attribute arguments run until ; or >] (":" starts a target: [<assembly: Name>])
      if (ctx == FsContext::AttributeArgs && c != ';' && c != ':' && !(c == '>' && next == ']'))
        {
          I++;
          continue;
        }
      int nStep = 1;
      switch (c)
        {
        case '.':
          if (ctx != FsContext::MemberName && ctx != FsContext::TypePath && ctx != FsContext::TypeAnnotation
              && ctx != FsContext::TypeArguments && ctx != FsContext::Attribute)
            ctx = FsContext::None;
          break;
        case ':':
          if (next == ':' || next == '=')
            {
              nStep = 2;   // :: and :=
              ctx = FsContext::None;
            }
          else if (next == '>')
            {
              nStep = 2;   // :>
              ctx = FsContext::TypeAnnotation;
            }
          else if (next == '?')
            {
              nStep = I + 2 < nLength && pszChars[I + 2] == '>' ? 3 : 2;   // :? and :?>
              ctx = FsContext::TypeAnnotation;
            }
          else if (ctx == FsContext::Attribute || ctx == FsContext::AttributeArgs)
            ctx = FsContext::Attribute;   // [<assembly: Name>]
          else if (ctx != FsContext::TypeArguments)
            ctx = FsContext::TypeAnnotation;
          break;
        case '[':
          if (next == '<')
            {
              nStep = 2;
              ctxAfterAttribute = (ctx == FsContext::LetName || ctx == FsContext::AndName
                || ctx == FsContext::MemberName || ctx == FsContext::TypeName) ? ctx : FsContext::None;
              ctx = FsContext::Attribute;
            }
          else if (ctx != FsContext::TypeAnnotation && ctx != FsContext::TypeArguments && ctx != FsContext::AttributeArgs)
            ctx = FsContext::None;   // int[] stays a type
          break;
        case ']':
          if (ctx != FsContext::TypeAnnotation && ctx != FsContext::TypeArguments && ctx != FsContext::AttributeArgs)
            ctx = FsContext::None;
          break;
        case '>':
          if ((ctx == FsContext::Attribute || ctx == FsContext::AttributeArgs) && next == ']')
            {
              nStep = 2;
              ctx = ctxAfterAttribute;
            }
          else if (ctx == FsContext::TypeArguments)
            {
              if (--nTypeArgsDepth <= 0)
                ctx = ctxAfterTypeArgs;
            }
          else if (ctx != FsContext::AttributeArgs)
            ctx = FsContext::None;
          break;
        case '<':
          if (ctx == FsContext::TypeArguments)
            nTypeArgsDepth++;
          else if (ctx != FsContext::AttributeArgs)
            ctx = FsContext::None;
          break;
        case '-':
          if (next == '>')
            {
              nStep = 2;   // -> inside a type
              if (ctx != FsContext::TypeAnnotation && ctx != FsContext::TypeArguments)
                ctx = FsContext::None;
            }
          else if (ctx != FsContext::AttributeArgs)
            ctx = FsContext::None;
          break;
        case '*': case '#': case '^':
          // tuple types, flexible types #seq, statically resolved ^T
          if (ctx != FsContext::TypeAnnotation && ctx != FsContext::TypeArguments && ctx != FsContext::AttributeArgs)
            ctx = FsContext::None;
          break;
        case ',':
          if (ctx != FsContext::TypeArguments && ctx != FsContext::AttributeArgs)
            ctx = FsContext::None;
          break;
        case ';':
          if (ctx == FsContext::Attribute || ctx == FsContext::AttributeArgs)
            ctx = FsContext::Attribute;
          else
            ctx = FsContext::None;
          break;
        case '(':
          if (ctx == FsContext::LetName || ctx == FsContext::AndName || ctx == FsContext::MemberName)
            {
              if (next == '|' && I + 2 < nLength && IsIdentStart (pszChars[I + 2]))
                {
                  nStep = 2;   // (|Even|Odd|), but not (|>)
                  ctx = FsContext::ActivePattern;
                  break;
                }
              // operator definition: let (>>=) a b
              const int nOpBegin = SkipSpaces (pszChars, nLength, I + 1);
              int nOpEnd = nOpBegin;
              while (nOpEnd < nLength && IsOperatorChar (pszChars[nOpEnd]))
                nOpEnd++;
              const int nClose = SkipSpaces (pszChars, nLength, nOpEnd);
              if (nOpEnd > nOpBegin && nClose < nLength && pszChars[nClose] == ')')
                {
                  DEFINE_BLOCK (nOpBegin, COLORINDEX_FUNCNAME);
                  DEFINE_BLOCK (nOpEnd, COLORINDEX_OPERATOR);
                  nStep = nClose + 1 - I;
                }
              ctx = FsContext::None;
            }
          else if (ctx != FsContext::TypeAnnotation && ctx != FsContext::TypeArguments && ctx != FsContext::AttributeArgs)
            ctx = FsContext::None;
          break;
        case ')':
          if (ctx != FsContext::TypeArguments && ctx != FsContext::AttributeArgs)
            ctx = FsContext::None;
          break;
        case '|':
          if (ctx == FsContext::ActivePattern)
            {
              if (next == ')')
                {
                  nStep = 2;
                  ctx = FsContext::None;
                }
            }
          else
            ctx = FsContext::None;
          break;
        default:
          if (ctx != FsContext::AttributeArgs)
            ctx = FsContext::None;
          break;
        }
      I += nStep;
    }

  return dwCookie & COOKIE_FS_MULTILINE;
}
