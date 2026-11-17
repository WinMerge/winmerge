///////////////////////////////////////////////////////////////////////////
//  File:    cobol.cpp
//  Version: 1.1.0.0
//  Updated: 17-Nov-2026
//
//  COBOL (free format) syntax highlighing definition
//
//  Color usage:
//  - COLORINDEX_KEYWORD:      verbs, clauses and other reserved words
//  - COLORINDEX_USER1:        data description and types: PIC and its picture string,
//                             USAGE, COMP-3, OCCURS, REDEFINES..., and the type after AS
//  - COLORINDEX_USER2:        any other word, that is data names (variables)
//  - COLORINDEX_FUNCNAME:     paragraph and section names, both declared and referenced
//                             by PERFORM / GO TO / THRU; function names (FUNCTION LENGTH,
//                             $LENGTH) and method names after ":>" or "::"
//  - COLORINDEX_NUMBER:       numeric literals, hexadecimal literals (X"0D0A"),
//                             figurative constants and special registers
//  - COLORINDEX_OPERATOR:     symbols and AND, OR, NOT, EQUAL, GREATER, LESS
//  - COLORINDEX_STRING:       "..." and '...' (doubled quote as escape), Z"..", N"..", G".."
//  - COLORINDEX_COMMENT:      "*>" anywhere, and "*" or "/" in column 1
//  - COLORINDEX_PREPROCESSOR: ">>" directives and "$" directives ($SET, $IF...)
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "crystallineparser.h"
#include "../SyntaxColors.h"
#include "../utils/string_util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//  The lists below are searched with a binary search: keep them sorted (case-insensitive).

//  COBOL reserved words
static const tchar_t * s_apszCobolKeywordList[] =
  {
    _T ("ACCEPT"),
    _T ("ACCESS"),
    _T ("ADD"),
    _T ("ADDRESS"),
    _T ("ADVANCING"),
    _T ("AFTER"),
    _T ("ALL"),
    _T ("ALPHABET"),
    _T ("ALPHABETIC"),
    _T ("ALPHABETIC-LOWER"),
    _T ("ALPHABETIC-UPPER"),
    _T ("ALPHANUMERIC"),
    _T ("ALPHANUMERIC-EDITED"),
    _T ("ALSO"),
    _T ("ALTER"),
    _T ("ALTERNATE"),
    _T ("ANY"),
    _T ("ARE"),
    _T ("AREA"),
    _T ("AREAS"),
    _T ("AS"),
    _T ("ASCENDING"),
    _T ("ASSIGN"),
    _T ("AT"),
    _T ("AUTHOR"),
    _T ("AUTO"),
    _T ("BACKGROUND-COLOR"),
    _T ("BASED"),
    _T ("BEEP"),
    _T ("BEFORE"),
    _T ("BELL"),
    _T ("BLANK"),
    _T ("BLINK"),
    _T ("BLOCK"),
    _T ("BOOLEAN"),
    _T ("BOTTOM"),
    _T ("BY"),
    _T ("CALL"),
    _T ("CANCEL"),
    _T ("CD"),
    _T ("CF"),
    _T ("CH"),
    _T ("CHARACTER"),
    _T ("CHARACTERS"),
    _T ("CLASS"),
    _T ("CLASS-ID"),
    _T ("CLOCK-UNITS"),
    _T ("CLOSE"),
    _T ("COBOL"),
    _T ("CODE"),
    _T ("CODE-SET"),
    _T ("COLLATING"),
    _T ("COLUMN"),
    _T ("COMMA"),
    _T ("COMMIT"),
    _T ("COMMON"),
    _T ("COMMUNICATION"),
    _T ("COMPUTE"),
    _T ("CONFIGURATION"),
    _T ("CONSTANT"),
    _T ("CONTAINS"),
    _T ("CONTENT"),
    _T ("CONTINUE"),
    _T ("CONTROL"),
    _T ("CONTROLS"),
    _T ("CONVERTING"),
    _T ("COPY"),
    _T ("CORR"),
    _T ("CORRESPONDING"),
    _T ("COUNT"),
    _T ("CRT"),
    _T ("CURRENCY"),
    _T ("CURSOR"),
    _T ("CYCLE"),
    _T ("DATA"),
    _T ("DATE"),
    _T ("DATE-COMPILED"),
    _T ("DATE-WRITTEN"),
    _T ("DAY"),
    _T ("DAY-OF-WEEK"),
    _T ("DE"),
    _T ("DEBUGGING"),
    _T ("DECIMAL-POINT"),
    _T ("DECLARATIVES"),
    _T ("DECLARE"),
    _T ("DEFAULT"),
    _T ("DELETE"),
    _T ("DELIMITED"),
    _T ("DELIMITER"),
    _T ("DEPENDING"),
    _T ("DESCENDING"),
    _T ("DESTINATION"),
    _T ("DETAIL"),
    _T ("DISABLE"),
    _T ("DISPLAY"),
    _T ("DIVIDE"),
    _T ("DIVISION"),
    _T ("DOWN"),
    _T ("DUPLICATES"),
    _T ("DYNAMIC"),
    _T ("EGI"),
    _T ("ELSE"),
    _T ("EMI"),
    _T ("ENABLE"),
    _T ("END"),
    _T ("END-ACCEPT"),
    _T ("END-ADD"),
    _T ("END-CALL"),
    _T ("END-COMPUTE"),
    _T ("END-DELETE"),
    _T ("END-DISPLAY"),
    _T ("END-DIVIDE"),
    _T ("END-EVALUATE"),
    _T ("END-EXEC"),
    _T ("END-IF"),
    _T ("END-MULTIPLY"),
    _T ("END-OF-PAGE"),
    _T ("END-PERFORM"),
    _T ("END-READ"),
    _T ("END-RECEIVE"),
    _T ("END-RETURN"),
    _T ("END-REWRITE"),
    _T ("END-SEARCH"),
    _T ("END-START"),
    _T ("END-STRING"),
    _T ("END-SUBTRACT"),
    _T ("END-UNSTRING"),
    _T ("END-WRITE"),
    _T ("ENTRY"),
    _T ("ENVIRONMENT"),
    _T ("EOP"),
    _T ("ERASE"),
    _T ("ERROR"),
    _T ("ESI"),
    _T ("EVALUATE"),
    _T ("EVERY"),
    _T ("EXCEPTION"),
    _T ("EXCLUSIVE"),
    _T ("EXEC"),
    _T ("EXIT"),
    _T ("EXTEND"),
    _T ("EXTERNAL"),
    _T ("FACTORY"),
    _T ("FD"),
    _T ("FILE"),
    _T ("FILE-CONTROL"),
    _T ("FILLER"),
    _T ("FINAL"),
    _T ("FIRST"),
    _T ("FOOTING"),
    _T ("FOR"),
    _T ("FOREGROUND-COLOR"),
    _T ("FORMAT"),
    _T ("FREE"),
    _T ("FROM"),
    _T ("FULL"),
    _T ("FUNCTION"),
    _T ("FUNCTION-ID"),
    _T ("GENERATE"),
    _T ("GIVING"),
    _T ("GLOBAL"),
    _T ("GO"),
    _T ("GOBACK"),
    _T ("GROUP"),
    _T ("HEADING"),
    _T ("HIGHLIGHT"),
    _T ("I-O"),
    _T ("I-O-CONTROL"),
    _T ("IDENTIFICATION"),
    _T ("IF"),
    _T ("IN"),
    _T ("INDEXED"),
    _T ("INDICATE"),
    _T ("INHERITS"),
    _T ("INITIAL"),
    _T ("INITIALIZE"),
    _T ("INITIATE"),
    _T ("INPUT"),
    _T ("INPUT-OUTPUT"),
    _T ("INSPECT"),
    _T ("INSTALLATION"),
    _T ("INTERFACE"),
    _T ("INTERFACE-ID"),
    _T ("INTO"),
    _T ("INVALID"),
    _T ("INVOKE"),
    _T ("IS"),
    _T ("KEY"),
    _T ("LABEL"),
    _T ("LAST"),
    _T ("LEADING"),
    _T ("LEFT"),
    _T ("LENGTH"),
    _T ("LIMIT"),
    _T ("LIMITS"),
    _T ("LINAGE"),
    _T ("LINE"),
    _T ("LINES"),
    _T ("LINKAGE"),
    _T ("LOCAL-STORAGE"),
    _T ("LOCK"),
    _T ("LOWLIGHT"),
    _T ("MEMORY"),
    _T ("MERGE"),
    _T ("MESSAGE"),
    _T ("METHOD"),
    _T ("METHOD-ID"),
    _T ("MODE"),
    _T ("MODULES"),
    _T ("MOVE"),
    _T ("MULTIPLE"),
    _T ("MULTIPLY"),
    _T ("NATIONAL"),
    _T ("NATIONAL-EDITED"),
    _T ("NATIVE"),
    _T ("NEGATIVE"),
    _T ("NEXT"),
    _T ("NO"),
    _T ("NUMBER"),
    _T ("NUMERIC"),
    _T ("NUMERIC-EDITED"),
    _T ("OBJECT"),
    _T ("OBJECT-COMPUTER"),
    _T ("OF"),
    _T ("OFF"),
    _T ("OMITTED"),
    _T ("ON"),
    _T ("OPEN"),
    _T ("OPTIONAL"),
    _T ("OPTIONS"),
    _T ("ORDER"),
    _T ("ORGANIZATION"),
    _T ("OTHER"),
    _T ("OUTPUT"),
    _T ("OVERFLOW"),
    _T ("OVERRIDE"),
    _T ("PADDING"),
    _T ("PAGE"),
    _T ("PARAGRAPH"),
    _T ("PERFORM"),
    _T ("PF"),
    _T ("PH"),
    _T ("PLUS"),
    _T ("POSITION"),
    _T ("POSITIVE"),
    _T ("PRINTING"),
    _T ("PROCEDURE"),
    _T ("PROCEDURES"),
    _T ("PROCEED"),
    _T ("PROGRAM"),
    _T ("PROGRAM-ID"),
    _T ("PROMPT"),
    _T ("PROPERTY"),
    _T ("PROTOTYPE"),
    _T ("PURGE"),
    _T ("QUEUE"),
    _T ("RAISE"),
    _T ("RAISING"),
    _T ("RANDOM"),
    _T ("RD"),
    _T ("READ"),
    _T ("RECEIVE"),
    _T ("RECORD"),
    _T ("RECORDING"),
    _T ("RECORDS"),
    _T ("REEL"),
    _T ("REFERENCE"),
    _T ("REFERENCES"),
    _T ("RELATIVE"),
    _T ("RELEASE"),
    _T ("REMAINDER"),
    _T ("REMOVAL"),
    _T ("REPLACE"),
    _T ("REPLACING"),
    _T ("REPORT"),
    _T ("REPORTING"),
    _T ("REPORTS"),
    _T ("REPOSITORY"),
    _T ("REQUIRED"),
    _T ("RERUN"),
    _T ("RESERVE"),
    _T ("RESET"),
    _T ("RESUME"),
    _T ("RETRY"),
    _T ("RETURN"),
    _T ("RETURNING"),
    _T ("REVERSE-VIDEO"),
    _T ("REWIND"),
    _T ("REWRITE"),
    _T ("RF"),
    _T ("RH"),
    _T ("RIGHT"),
    _T ("ROLLBACK"),
    _T ("ROUNDED"),
    _T ("RUN"),
    _T ("SAME"),
    _T ("SCREEN"),
    _T ("SD"),
    _T ("SEARCH"),
    _T ("SECTION"),
    _T ("SECURE"),
    _T ("SECURITY"),
    _T ("SEGMENT"),
    _T ("SEGMENT-LIMIT"),
    _T ("SELECT"),
    _T ("SELF"),
    _T ("SEND"),
    _T ("SENTENCE"),
    _T ("SEQUENCE"),
    _T ("SEQUENTIAL"),
    _T ("SET"),
    _T ("SHARING"),
    _T ("SIZE"),
    _T ("SORT"),
    _T ("SORT-MERGE"),
    _T ("SOURCE"),
    _T ("SOURCE-COMPUTER"),
    _T ("SPECIAL-NAMES"),
    _T ("STANDARD"),
    _T ("STANDARD-1"),
    _T ("STANDARD-2"),
    _T ("START"),
    _T ("STATUS"),
    _T ("STOP"),
    _T ("STRING"),
    _T ("SUB-QUEUE-1"),
    _T ("SUB-QUEUE-2"),
    _T ("SUB-QUEUE-3"),
    _T ("SUBTRACT"),
    _T ("SUM"),
    _T ("SUPER"),
    _T ("SUPPRESS"),
    _T ("SYMBOLIC"),
    _T ("TALLYING"),
    _T ("TAPE"),
    _T ("TERMINAL"),
    _T ("TERMINATE"),
    _T ("TEST"),
    _T ("TEXT"),
    _T ("THAN"),
    _T ("THEN"),
    _T ("THROUGH"),
    _T ("THRU"),
    _T ("TIME"),
    _T ("TIMES"),
    _T ("TO"),
    _T ("TOP"),
    _T ("TRAILING"),
    _T ("TYPE"),
    _T ("UNDERLINE"),
    _T ("UNIT"),
    _T ("UNLOCK"),
    _T ("UNSTRING"),
    _T ("UNTIL"),
    _T ("UP"),
    _T ("UPON"),
    _T ("USE"),
    _T ("USING"),
    _T ("VALIDATE"),
    _T ("VALUE"),
    _T ("VALUES"),
    _T ("VARYING"),
    _T ("WAIT"),
    _T ("WHEN"),
    _T ("WITH"),
    _T ("WORDS"),
    _T ("WORKING-STORAGE"),
    _T ("WRITE"),
  };

//  Data description and type words
static const tchar_t * s_apszCobolStorageKeywordList[] =
  {
    _T ("BINARY"),
    _T ("BINARY-CHAR"),
    _T ("BINARY-DOUBLE"),
    _T ("BINARY-LONG"),
    _T ("BINARY-SHORT"),
    _T ("BIT"),
    _T ("COMP"),
    _T ("COMP-1"),
    _T ("COMP-2"),
    _T ("COMP-3"),
    _T ("COMP-4"),
    _T ("COMP-5"),
    _T ("COMP-6"),
    _T ("COMP-N"),
    _T ("COMP-X"),
    _T ("COMPUTATIONAL"),
    _T ("COMPUTATIONAL-1"),
    _T ("COMPUTATIONAL-2"),
    _T ("COMPUTATIONAL-3"),
    _T ("COMPUTATIONAL-4"),
    _T ("COMPUTATIONAL-5"),
    _T ("COMPUTATIONAL-6"),
    _T ("COMPUTATIONAL-N"),
    _T ("COMPUTATIONAL-X"),
    _T ("FLOAT-EXTENDED"),
    _T ("FLOAT-LONG"),
    _T ("FLOAT-SHORT"),
    _T ("INDEX"),
    _T ("JUST"),
    _T ("JUSTIFIED"),
    _T ("OBJECT-REFERENCE"),
    _T ("OCCURS"),
    _T ("PACKED-DECIMAL"),
    _T ("PIC"),
    _T ("PICTURE"),
    _T ("POINTER"),
    _T ("PROCEDURE-POINTER"),
    _T ("PROGRAM-POINTER"),
    _T ("REDEFINES"),
    _T ("RENAMES"),
    _T ("SEPARATE"),
    _T ("SIGN"),
    _T ("SIGNED"),
    _T ("SYNC"),
    _T ("SYNCHRONIZED"),
    _T ("TYPEDEF"),
    _T ("UNSIGNED"),
    _T ("USAGE"),
  };

//  Figurative constants and special registers
static const tchar_t * s_apszCobolConstantList[] =
  {
    _T ("FALSE"),
    _T ("HIGH-VALUE"),
    _T ("HIGH-VALUES"),
    _T ("LINAGE-COUNTER"),
    _T ("LINE-COUNTER"),
    _T ("LOW-VALUE"),
    _T ("LOW-VALUES"),
    _T ("NULL"),
    _T ("NULLS"),
    _T ("PAGE-COUNTER"),
    _T ("QUOTE"),
    _T ("QUOTES"),
    _T ("RETURN-CODE"),
    _T ("SORT-RETURN"),
    _T ("SPACE"),
    _T ("SPACES"),
    _T ("TALLY"),
    _T ("TRUE"),
    _T ("WHEN-COMPILED"),
    _T ("ZERO"),
    _T ("ZEROES"),
    _T ("ZEROS"),
  };

//  Logical and relational operators written as words
static const tchar_t * s_apszCobolOperatorList[] =
  {
    _T ("AND"),
    _T ("EQUAL"),
    _T ("EQUALS"),
    _T ("GREATER"),
    _T ("LESS"),
    _T ("NOT"),
    _T ("OR"),
  };

//  "$" compiler directives
static const tchar_t * s_apszCobolDirectiveList[] =
  {
    _T ("DEFINE"),
    _T ("DISPLAY"),
    _T ("ELIF"),
    _T ("ELSE"),
    _T ("END"),
    _T ("END-IF"),
    _T ("IF"),
    _T ("SET"),
    _T ("XFD"),
  };

static bool
IsCobolKeyword (const tchar_t *pszChars, int nLength)
{
  return ISXKEYWORDI (s_apszCobolKeywordList, pszChars, nLength);
}

static bool
IsCobolStorageKeyword (const tchar_t *pszChars, int nLength)
{
  return ISXKEYWORDI (s_apszCobolStorageKeywordList, pszChars, nLength);
}

static bool
IsCobolConstant (const tchar_t *pszChars, int nLength)
{
  return ISXKEYWORDI (s_apszCobolConstantList, pszChars, nLength);
}

static bool
IsCobolOperator (const tchar_t *pszChars, int nLength)
{
  return ISXKEYWORDI (s_apszCobolOperatorList, pszChars, nLength);
}

static bool
IsCobolDirective (const tchar_t *pszChars, int nLength)
{
  return ISXKEYWORDI (s_apszCobolDirectiveList, pszChars, nLength);
}

static inline bool
IsCobolWordChar (tchar_t c)
{
  return xisalnum (c) || c == '_';
}

static inline bool
IsCobolQuote (tchar_t c)
{
  return c == '"' || c == '\'';
}

static inline bool
IsWord (const tchar_t *pszChars, int nLength, const tchar_t *pszWord)
{
  return static_cast<int>(tc::tcslen (pszWord)) == nLength && tc::tcsnicmp (pszChars, pszWord, nLength) == 0;
}

/**
 * @brief Prefixes of hexadecimal and boolean literals: X"..", H"..", B"..", NX"..", BX".."
 */
static bool
IsCobolNumericLiteralPrefix (const tchar_t *pszChars, int nLength)
{
  return IsWord (pszChars, nLength, _T ("X")) || IsWord (pszChars, nLength, _T ("H")) ||
         IsWord (pszChars, nLength, _T ("B")) || IsWord (pszChars, nLength, _T ("NX")) ||
         IsWord (pszChars, nLength, _T ("BX"));
}

/**
 * @brief Prefixes of alphanumeric literals: Z"..", N"..", G".."
 */
static bool
IsCobolStringLiteralPrefix (const tchar_t *pszChars, int nLength)
{
  return IsWord (pszChars, nLength, _T ("Z")) || IsWord (pszChars, nLength, _T ("N")) ||
         IsWord (pszChars, nLength, _T ("G"));
}

/**
 * @brief Return the position just after the end of a COBOL word starting at I.
 * Hyphens are part of the word only when followed by another word character (WS-NAME, END-IF).
 */
static int
SkipCobolWord (const tchar_t *pszChars, int nLength, int I)
{
  while (I < nLength)
    {
      if (IsCobolWordChar (pszChars[I]))
        I++;
      else if (pszChars[I] == '-')
        {
          int J = I;
          while (J < nLength && pszChars[J] == '-')
            J++;
          if (J < nLength && IsCobolWordChar (pszChars[J]))
            I = J;
          else
            break;
        }
      else
        break;
    }
  return I;
}

/**
 * @brief Return the position just after the closing quote of the literal starting at I.
 * A doubled quote ("" or '') inside the literal is an escaped quote.
 * An unterminated literal extends to the end of the line.
 */
static int
SkipCobolString (const tchar_t *pszChars, int nLength, int I)
{
  const tchar_t quote = pszChars[I];
  for (I++; I < nLength; I++)
    {
      if (pszChars[I] == quote)
        {
          if (I + 1 < nLength && pszChars[I + 1] == quote)
            I++;
          else
            return I + 1;
        }
    }
  return nLength;
}

static int
SkipBlanks (const tchar_t *pszChars, int nLength, int I)
{
  while (I < nLength && xisspace (pszChars[I]))
    I++;
  return I;
}

static bool
IsCobolNumber (const tchar_t *pszChars, int nLength)
{
  for (int i = 0; i < nLength; i++)
    {
      if (!tc::istdigit (pszChars[i]))
        return false;
    }
  return nLength > 0;
}

/**
 * @brief Return true if the word starting at I (after blanks) is pszWord.
 */
static bool
IsNextWord (const tchar_t *pszChars, int nLength, int I, const tchar_t *pszWord)
{
  I = SkipBlanks (pszChars, nLength, I);
  return IsWord (pszChars + I, SkipCobolWord (pszChars, nLength, I) - I, pszWord);
}

/**
 * @brief Return true if only blanks or a "*>" comment remain from position I.
 */
static bool
IsRestOfLineEmpty (const tchar_t *pszChars, int nLength, int I)
{
  I = SkipBlanks (pszChars, nLength, I);
  return I >= nLength || pszChars[I] == 0 || (pszChars[I] == '*' && I + 1 < nLength && pszChars[I + 1] == '>');
}

unsigned
CrystalLineParser::ParseLineCobol (unsigned dwCookie, const tchar_t *pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf)
{
  // Free format COBOL has no construct spanning several lines, so no state is kept between lines.
  (void) dwCookie;
  if (nLength == 0 || pBuf == nullptr)
    return 0;

  DEFINE_BLOCK (0, COLORINDEX_NORMALTEXT);

  //  "*" or "/" in column 1: comment line
  if (pszChars[0] == '*' || pszChars[0] == '/')
    {
      DEFINE_BLOCK (0, COLORINDEX_COMMENT);
      return 0;
    }

  int I = SkipBlanks (pszChars, nLength, 0);

  //  Compiler directive: >>SOURCE FORMAT FREE, $SET ..., $IF ...
  //  A "$" followed by any other word is a function call such as $LENGTH(X).
  bool bDirective = false;
  if (I + 1 < nLength && pszChars[I] == '>' && pszChars[I + 1] == '>')
    bDirective = true;
  else if (I < nLength && pszChars[I] == '$')
    {
      const int nWord = SkipBlanks (pszChars, nLength, I + 1);
      bDirective = IsCobolDirective (pszChars + nWord, SkipCobolWord (pszChars, nLength, nWord) - nWord);
    }
  if (bDirective)
    {
      DEFINE_BLOCK (I, COLORINDEX_PREPROCESSOR);
      for (; I + 1 < nLength; I++)
        {
          if (pszChars[I] == '*' && pszChars[I + 1] == '>')
            {
              DEFINE_BLOCK (I, COLORINDEX_COMMENT);
              break;
            }
        }
      return 0;
    }

  bool bFirstWord = true;   // next word is the first one of the line
  bool bPicture = false;    // next token is a picture string (after PIC / PICTURE)
  bool bFunction = false;   // next word is a function or method name (after FUNCTION, ":>", "::")
  bool bParagraph = false;  // next word is a paragraph name (after PERFORM, GO [TO], THRU)
  bool bType = false;       // next word is a type name (after AS)
  bool bAfterGo = false;    // previous word was GO

  while (I < nLength)
    {
      const tchar_t c = pszChars[I];

      // Can be bigger than length if there is binary data
      if (c == 0)
        break;

      //  Blanks
      if (xisspace (c))
        {
          DEFINE_BLOCK (I, COLORINDEX_NORMALTEXT);
          I = SkipBlanks (pszChars, nLength, I);
          continue;
        }

      //  Inline comment "*>"
      if (c == '*' && I + 1 < nLength && pszChars[I + 1] == '>')
        {
          DEFINE_BLOCK (I, COLORINDEX_COMMENT);
          break;
        }

      //  Picture string: PIC X(30), PIC S9(7)V99, PIC ZZZ.ZZ9,99
      if (bPicture)
        {
          int J = I;
          while (J < nLength && pszChars[J] != 0 && !xisspace (pszChars[J]))
            J++;
          //  "PIC IS X(10)": IS is not part of the picture string
          if (IsWord (pszChars + I, J - I, _T ("IS")))
            {
              DEFINE_BLOCK (I, COLORINDEX_KEYWORD);
              I = J;
              continue;
            }
          //  A trailing period ends the sentence
          if (J - I > 1 && pszChars[J - 1] == '.')
            J--;
          DEFINE_BLOCK (I, COLORINDEX_USER1);
          bPicture = false;
          bFirstWord = false;
          I = J;
          continue;
        }

      //  String literal "..." or '...'
      if (IsCobolQuote (c))
        {
          DEFINE_BLOCK (I, COLORINDEX_STRING);
          I = SkipCobolString (pszChars, nLength, I);
          bFirstWord = bFunction = bParagraph = bType = bAfterGo = false;
          continue;
        }

      //  Function call written as $LENGTH(X)
      if (c == '$' && I + 1 < nLength && IsCobolWordChar (pszChars[I + 1]))
        {
          DEFINE_BLOCK (I, COLORINDEX_FUNCNAME);
          I = SkipCobolWord (pszChars, nLength, I + 1);
          bFirstWord = bFunction = bParagraph = bType = bAfterGo = false;
          continue;
        }

      //  Words, numbers and prefixed literals
      if (IsCobolWordChar (c))
        {
          int J = SkipCobolWord (pszChars, nLength, I);
          const tchar_t *pszWord = pszChars + I;
          const int nWordLength = J - I;
          const bool bWasParagraph = bParagraph;
          const bool bWasAfterGo = bAfterGo;
          bParagraph = bAfterGo = false;

          //  Number: 3, 3.14 or 3,14 (DECIMAL-POINT IS COMMA)
          if (IsCobolNumber (pszWord, nWordLength))
            {
              while (J + 1 < nLength && (pszChars[J] == '.' || pszChars[J] == ',') && tc::istdigit (pszChars[J + 1]))
                {
                  J++;
                  while (J < nLength && tc::istdigit (pszChars[J]))
                    J++;
                }
              DEFINE_BLOCK (I, COLORINDEX_NUMBER);
              bFunction = bType = false;
            }
          //  Prefixed literal: X"0D0A", NX"...", B"1" / N"...", Z"..."
          else if (J < nLength && IsCobolQuote (pszChars[J]) && IsCobolNumericLiteralPrefix (pszWord, nWordLength))
            {
              DEFINE_BLOCK (I, COLORINDEX_NUMBER);
              J = SkipCobolString (pszChars, nLength, J);
            }
          else if (J < nLength && IsCobolQuote (pszChars[J]) && IsCobolStringLiteralPrefix (pszWord, nWordLength))
            {
              DEFINE_BLOCK (I, COLORINDEX_STRING);
              J = SkipCobolString (pszChars, nLength, J);
            }
          //  FUNCTION LENGTH(X), obj:>method, Class::method
          else if (bFunction)
            {
              DEFINE_BLOCK (I, COLORINDEX_FUNCNAME);
              bFunction = false;
            }
          //  DECLARE NAME AS TYPE, method parameters "NAME AS TYPE"
          else if (bType)
            {
              DEFINE_BLOCK (I, COLORINDEX_USER1);
              bType = false;
            }
          else if (IsCobolOperator (pszWord, nWordLength))
            {
              DEFINE_BLOCK (I, COLORINDEX_OPERATOR);
            }
          else if (IsCobolConstant (pszWord, nWordLength))
            {
              DEFINE_BLOCK (I, COLORINDEX_NUMBER);
            }
          else if (IsCobolStorageKeyword (pszWord, nWordLength))
            {
              DEFINE_BLOCK (I, COLORINDEX_USER1);
              if (IsWord (pszWord, nWordLength, _T ("PIC")) || IsWord (pszWord, nWordLength, _T ("PICTURE")))
                bPicture = true;
            }
          else if (IsCobolKeyword (pszWord, nWordLength))
            {
              DEFINE_BLOCK (I, COLORINDEX_KEYWORD);
              if (IsWord (pszWord, nWordLength, _T ("FUNCTION")))
                bFunction = true;
              else if (IsWord (pszWord, nWordLength, _T ("AS")))
                bType = true;
              else if (IsWord (pszWord, nWordLength, _T ("PERFORM")) || IsWord (pszWord, nWordLength, _T ("THRU")) ||
                       IsWord (pszWord, nWordLength, _T ("THROUGH")))
                bParagraph = true;
              else if (IsWord (pszWord, nWordLength, _T ("GO")))
                bParagraph = bAfterGo = true;
              else if (bWasAfterGo && IsWord (pszWord, nWordLength, _T ("TO")))
                bParagraph = true;
            }
          else
            {
              //  Paragraph name: "PERFORM NAME", "GO TO NAME", "THRU NAME", but not "PERFORM WS-COUNT TIMES"
              bool bLabel = bWasParagraph && !IsNextWord (pszChars, nLength, J, _T ("TIMES"));
              //  Paragraph declaration ("NAME." alone on the line) or section declaration ("NAME SECTION.")
              if (bFirstWord)
                {
                  if (J < nLength && pszChars[J] == '.' && IsRestOfLineEmpty (pszChars, nLength, J + 1))
                    bLabel = true;
                  else if (IsNextWord (pszChars, nLength, J, _T ("SECTION")))
                    bLabel = true;
                }
              DEFINE_BLOCK (I, bLabel ? COLORINDEX_FUNCNAME : COLORINDEX_USER2);
            }

          bFirstWord = false;
          I = J;
          continue;
        }

      //  Method invocation: obj:>method, Class::method
      if (c == ':' && I + 1 < nLength && (pszChars[I + 1] == '>' || pszChars[I + 1] == ':'))
        {
          DEFINE_BLOCK (I, COLORINDEX_OPERATOR);
          I += 2;
          bFunction = true;
          bFirstWord = bParagraph = bType = bAfterGo = false;
          continue;
        }

      //  Operators and separators: . , ; ( ) + - * / ** = < > : &
      DEFINE_BLOCK (I, COLORINDEX_OPERATOR);
      I++;
      bFirstWord = bFunction = bParagraph = bType = bAfterGo = false;
    }

  return 0;
}
