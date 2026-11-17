// SPDX-License-Identifier: BSL-1.0
/**
 * @file  cobol.cpp
 *
 * @brief COBOL syntax highlighting definition.
 *
 * Both reference (fixed) format and free format source are supported.
 *
 * Fixed format:
 *   - columns 1-6   : sequence number area
 *   - column  7     : indicator area ('*' or '/' = comment line,
 *                     '-' = continuation line, 'D' = debugging line)
 *   - columns 8-72  : program text (area A and area B)
 *   - columns 73-   : program identification area
 *
 * Free format:
 *   - "*>" starts a comment that runs to the end of the line.
 *
 * COBOL has no construct that spans multiple lines (a continued literal is
 * re-opened with a quote on the continuation line), so each line is parsed
 * independently and no state is carried over in the cookie.
 */

#include "pch.h"
#include "crystallineparser.h"
#include "../SyntaxColors.h"
#include "../utils/string_util.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//  COBOL reserved words (sorted case-insensitively: required by ISXKEYWORDI)
static const tchar_t* s_apszCobolKeywordList[] =
{
	_T("ACCEPT"),
	_T("ACCESS"),
	_T("ACTIVE-CLASS"),
	_T("ADD"),
	_T("ADDRESS"),
	_T("ADVANCING"),
	_T("AFTER"),
	_T("ALIGNED"),
	_T("ALL"),
	_T("ALLOCATE"),
	_T("ALPHABET"),
	_T("ALPHABETIC"),
	_T("ALPHABETIC-LOWER"),
	_T("ALPHABETIC-UPPER"),
	_T("ALPHANUMERIC"),
	_T("ALPHANUMERIC-EDITED"),
	_T("ALSO"),
	_T("ALTER"),
	_T("ALTERNATE"),
	_T("AND"),
	_T("ANY"),
	_T("ANYCASE"),
	_T("APPLY"),
	_T("ARE"),
	_T("AREA"),
	_T("AREAS"),
	_T("AS"),
	_T("ASCENDING"),
	_T("ASSIGN"),
	_T("AT"),
	_T("AUTHOR"),
	_T("B-AND"),
	_T("B-NOT"),
	_T("B-OR"),
	_T("B-XOR"),
	_T("BASED"),
	_T("BASIS"),
	_T("BEFORE"),
	_T("BEGINNING"),
	_T("BINARY"),
	_T("BINARY-CHAR"),
	_T("BINARY-DOUBLE"),
	_T("BINARY-LONG"),
	_T("BINARY-SHORT"),
	_T("BIT"),
	_T("BLANK"),
	_T("BLOCK"),
	_T("BOOLEAN"),
	_T("BOTTOM"),
	_T("BY"),
	_T("CALL"),
	_T("CANCEL"),
	_T("CBL"),
	_T("CD"),
	_T("CF"),
	_T("CH"),
	_T("CHARACTER"),
	_T("CHARACTERS"),
	_T("CLASS"),
	_T("CLASS-ID"),
	_T("CLOCK-UNITS"),
	_T("CLOSE"),
	_T("COBOL"),
	_T("CODE"),
	_T("CODE-SET"),
	_T("COL"),
	_T("COLLATING"),
	_T("COLS"),
	_T("COLUMN"),
	_T("COLUMNS"),
	_T("COMMA"),
	_T("COMMIT"),
	_T("COMMON"),
	_T("COMMUNICATION"),
	_T("COMP"),
	_T("COMP-1"),
	_T("COMP-2"),
	_T("COMP-3"),
	_T("COMP-4"),
	_T("COMP-5"),
	_T("COMPUTATIONAL"),
	_T("COMPUTATIONAL-1"),
	_T("COMPUTATIONAL-2"),
	_T("COMPUTATIONAL-3"),
	_T("COMPUTATIONAL-4"),
	_T("COMPUTATIONAL-5"),
	_T("COMPUTE"),
	_T("CONDITION"),
	_T("CONFIGURATION"),
	_T("CONSTANT"),
	_T("CONTAINS"),
	_T("CONTENT"),
	_T("CONTINUE"),
	_T("CONTROL"),
	_T("CONTROLS"),
	_T("CONVERTING"),
	_T("COPY"),
	_T("CORR"),
	_T("CORRESPONDING"),
	_T("COUNT"),
	_T("CRT"),
	_T("CURRENCY"),
	_T("CURSOR"),
	_T("DATA"),
	_T("DATA-POINTER"),
	_T("DATE"),
	_T("DATE-COMPILED"),
	_T("DATE-WRITTEN"),
	_T("DAY"),
	_T("DAY-OF-WEEK"),
	_T("DBCS"),
	_T("DE"),
	_T("DEBUG-CONTENTS"),
	_T("DEBUG-ITEM"),
	_T("DEBUG-LINE"),
	_T("DEBUG-NAME"),
	_T("DEBUG-SUB-1"),
	_T("DEBUG-SUB-2"),
	_T("DEBUG-SUB-3"),
	_T("DEBUGGING"),
	_T("DECIMAL-POINT"),
	_T("DECLARATIVES"),
	_T("DEFAULT"),
	_T("DELETE"),
	_T("DELIMITED"),
	_T("DELIMITER"),
	_T("DEPENDING"),
	_T("DESCENDING"),
	_T("DESTINATION"),
	_T("DETAIL"),
	_T("DISABLE"),
	_T("DISPLAY"),
	_T("DISPLAY-1"),
	_T("DIVIDE"),
	_T("DIVISION"),
	_T("DOWN"),
	_T("DUPLICATES"),
	_T("DYNAMIC"),
	_T("EC"),
	_T("EGCS"),
	_T("EGI"),
	_T("ELSE"),
	_T("EMI"),
	_T("ENABLE"),
	_T("END"),
	_T("END-ACCEPT"),
	_T("END-ADD"),
	_T("END-CALL"),
	_T("END-COMPUTE"),
	_T("END-DELETE"),
	_T("END-DISPLAY"),
	_T("END-DIVIDE"),
	_T("END-EVALUATE"),
	_T("END-EXEC"),
	_T("END-IF"),
	_T("END-INVOKE"),
	_T("END-JSON"),
	_T("END-MULTIPLY"),
	_T("END-OF-PAGE"),
	_T("END-PERFORM"),
	_T("END-READ"),
	_T("END-RECEIVE"),
	_T("END-RETURN"),
	_T("END-REWRITE"),
	_T("END-SEARCH"),
	_T("END-START"),
	_T("END-STRING"),
	_T("END-SUBTRACT"),
	_T("END-UNSTRING"),
	_T("END-WRITE"),
	_T("END-XML"),
	_T("ENDING"),
	_T("ENTER"),
	_T("ENTRY"),
	_T("ENVIRONMENT"),
	_T("EO"),
	_T("EOP"),
	_T("EQUAL"),
	_T("ERROR"),
	_T("ESI"),
	_T("EVALUATE"),
	_T("EVERY"),
	_T("EXCEPTION"),
	_T("EXCEPTION-OBJECT"),
	_T("EXEC"),
	_T("EXIT"),
	_T("EXTEND"),
	_T("EXTERNAL"),
	_T("FACTORY"),
	_T("FALSE"),
	_T("FD"),
	_T("FILE"),
	_T("FILE-CONTROL"),
	_T("FILLER"),
	_T("FINAL"),
	_T("FIRST"),
	_T("FLOAT-EXTENDED"),
	_T("FLOAT-LONG"),
	_T("FLOAT-SHORT"),
	_T("FOOTING"),
	_T("FOR"),
	_T("FORMAT"),
	_T("FREE"),
	_T("FROM"),
	_T("FUNCTION"),
	_T("FUNCTION-ID"),
	_T("FUNCTION-POINTER"),
	_T("GENERATE"),
	_T("GET"),
	_T("GIVING"),
	_T("GLOBAL"),
	_T("GO"),
	_T("GOBACK"),
	_T("GREATER"),
	_T("GROUP"),
	_T("GROUP-USAGE"),
	_T("HEADING"),
	_T("HIGH-VALUE"),
	_T("HIGH-VALUES"),
	_T("I-O"),
	_T("I-O-CONTROL"),
	_T("ID"),
	_T("IDENTIFICATION"),
	_T("IF"),
	_T("IN"),
	_T("INDEX"),
	_T("INDEXED"),
	_T("INDICATE"),
	_T("INHERITS"),
	_T("INITIAL"),
	_T("INITIALIZE"),
	_T("INITIATE"),
	_T("INPUT"),
	_T("INPUT-OUTPUT"),
	_T("INSPECT"),
	_T("INSTALLATION"),
	_T("INTERFACE"),
	_T("INTERFACE-ID"),
	_T("INTO"),
	_T("INVALID"),
	_T("INVOKE"),
	_T("IS"),
	_T("JSON"),
	_T("JUST"),
	_T("JUSTIFIED"),
	_T("KANJI"),
	_T("KEY"),
	_T("LABEL"),
	_T("LAST"),
	_T("LEADING"),
	_T("LEFT"),
	_T("LENGTH"),
	_T("LESS"),
	_T("LIMIT"),
	_T("LIMITS"),
	_T("LINAGE"),
	_T("LINAGE-COUNTER"),
	_T("LINE"),
	_T("LINE-COUNTER"),
	_T("LINES"),
	_T("LINKAGE"),
	_T("LOCAL-STORAGE"),
	_T("LOCALE"),
	_T("LOCK"),
	_T("LOW-VALUE"),
	_T("LOW-VALUES"),
	_T("MEMORY"),
	_T("MERGE"),
	_T("MESSAGE"),
	_T("METHOD"),
	_T("METHOD-ID"),
	_T("MINUS"),
	_T("MODE"),
	_T("MODULES"),
	_T("MORE-LABELS"),
	_T("MOVE"),
	_T("MULTIPLE"),
	_T("MULTIPLY"),
	_T("NATIONAL"),
	_T("NATIONAL-EDITED"),
	_T("NATIVE"),
	_T("NEGATIVE"),
	_T("NESTED"),
	_T("NEXT"),
	_T("NO"),
	_T("NOT"),
	_T("NULL"),
	_T("NULLS"),
	_T("NUMBER"),
	_T("NUMERIC"),
	_T("NUMERIC-EDITED"),
	_T("OBJECT"),
	_T("OBJECT-COMPUTER"),
	_T("OBJECT-REFERENCE"),
	_T("OCCURS"),
	_T("OF"),
	_T("OFF"),
	_T("OMITTED"),
	_T("ON"),
	_T("OPEN"),
	_T("OPTIONAL"),
	_T("OPTIONS"),
	_T("OR"),
	_T("ORDER"),
	_T("ORGANIZATION"),
	_T("OTHER"),
	_T("OUTPUT"),
	_T("OVERFLOW"),
	_T("OVERRIDE"),
	_T("PACKED-DECIMAL"),
	_T("PADDING"),
	_T("PAGE"),
	_T("PAGE-COUNTER"),
	_T("PASSWORD"),
	_T("PERFORM"),
	_T("PF"),
	_T("PH"),
	_T("PIC"),
	_T("PICTURE"),
	_T("PLUS"),
	_T("POINTER"),
	_T("POSITION"),
	_T("POSITIVE"),
	_T("PRESENT"),
	_T("PREVIOUS"),
	_T("PRINTING"),
	_T("PROCEDURE"),
	_T("PROCEDURE-POINTER"),
	_T("PROCEDURES"),
	_T("PROCEED"),
	_T("PROCESSING"),
	_T("PROGRAM"),
	_T("PROGRAM-ID"),
	_T("PROGRAM-POINTER"),
	_T("PROPERTY"),
	_T("PROTOTYPE"),
	_T("PURGE"),
	_T("QUEUE"),
	_T("QUOTE"),
	_T("QUOTES"),
	_T("RAISE"),
	_T("RAISING"),
	_T("RANDOM"),
	_T("RD"),
	_T("READ"),
	_T("READY"),
	_T("RECEIVE"),
	_T("RECORD"),
	_T("RECORDING"),
	_T("RECORDS"),
	_T("RECURSIVE"),
	_T("REDEFINES"),
	_T("REEL"),
	_T("REFERENCE"),
	_T("REFERENCES"),
	_T("RELATIVE"),
	_T("RELEASE"),
	_T("RELOAD"),
	_T("REMAINDER"),
	_T("REMOVAL"),
	_T("RENAMES"),
	_T("REPLACE"),
	_T("REPLACING"),
	_T("REPORT"),
	_T("REPORTING"),
	_T("REPORTS"),
	_T("REPOSITORY"),
	_T("RERUN"),
	_T("RESERVE"),
	_T("RESET"),
	_T("RESUME"),
	_T("RETRY"),
	_T("RETURN"),
	_T("RETURN-CODE"),
	_T("RETURNING"),
	_T("REVERSED"),
	_T("REWIND"),
	_T("REWRITE"),
	_T("RF"),
	_T("RH"),
	_T("RIGHT"),
	_T("ROLLBACK"),
	_T("ROUNDED"),
	_T("RUN"),
	_T("SAME"),
	_T("SCREEN"),
	_T("SD"),
	_T("SEARCH"),
	_T("SECTION"),
	_T("SECURITY"),
	_T("SEGMENT"),
	_T("SEGMENT-LIMIT"),
	_T("SELECT"),
	_T("SELF"),
	_T("SEND"),
	_T("SENTENCE"),
	_T("SEPARATE"),
	_T("SEQUENCE"),
	_T("SEQUENTIAL"),
	_T("SERVICE"),
	_T("SET"),
	_T("SHARING"),
	_T("SHIFT-IN"),
	_T("SHIFT-OUT"),
	_T("SIGN"),
	_T("SIZE"),
	_T("SKIP1"),
	_T("SKIP2"),
	_T("SKIP3"),
	_T("SORT"),
	_T("SORT-CONTROL"),
	_T("SORT-CORE-SIZE"),
	_T("SORT-FILE-SIZE"),
	_T("SORT-MERGE"),
	_T("SORT-MESSAGE"),
	_T("SORT-MODE-SIZE"),
	_T("SORT-RETURN"),
	_T("SOURCE"),
	_T("SOURCE-COMPUTER"),
	_T("SOURCES"),
	_T("SPACE"),
	_T("SPACES"),
	_T("SPECIAL-NAMES"),
	_T("SQL"),
	_T("STANDARD"),
	_T("STANDARD-1"),
	_T("STANDARD-2"),
	_T("START"),
	_T("STATUS"),
	_T("STOP"),
	_T("STRING"),
	_T("SUB-QUEUE-1"),
	_T("SUB-QUEUE-2"),
	_T("SUB-QUEUE-3"),
	_T("SUBTRACT"),
	_T("SUM"),
	_T("SUPER"),
	_T("SUPPRESS"),
	_T("SYMBOLIC"),
	_T("SYNC"),
	_T("SYNCHRONIZED"),
	_T("SYSTEM-DEFAULT"),
	_T("TABLE"),
	_T("TALLY"),
	_T("TALLYING"),
	_T("TAPE"),
	_T("TERMINAL"),
	_T("TERMINATE"),
	_T("TEST"),
	_T("TEXT"),
	_T("THAN"),
	_T("THEN"),
	_T("THROUGH"),
	_T("THRU"),
	_T("TIME"),
	_T("TIMES"),
	_T("TITLE"),
	_T("TO"),
	_T("TOP"),
	_T("TRACE"),
	_T("TRAILING"),
	_T("TRUE"),
	_T("TYPE"),
	_T("TYPEDEF"),
	_T("UNIT"),
	_T("UNIVERSAL"),
	_T("UNLOCK"),
	_T("UNSTRING"),
	_T("UNTIL"),
	_T("UP"),
	_T("UPON"),
	_T("USAGE"),
	_T("USE"),
	_T("USER-DEFAULT"),
	_T("USING"),
	_T("VALIDATE"),
	_T("VALUE"),
	_T("VALUES"),
	_T("VARYING"),
	_T("WHEN"),
	_T("WHEN-COMPILED"),
	_T("WITH"),
	_T("WORDS"),
	_T("WORKING-STORAGE"),
	_T("WRITE"),
	_T("WRITE-ONLY"),
	_T("XML"),
	_T("ZERO"),
	_T("ZEROES"),
	_T("ZEROS"),
};

static const tchar_t* s_apszCobolLiteralPrefixList[] =
{
	_T("B"),
	_T("BX"),
	_T("G"),
	_T("N"),
	_T("NX"),
	_T("U"),
	_T("X"),
	_T("Z"),
};

static constexpr int FIXED_INDICATOR_COLUMN = 6;
static constexpr int FIXED_AREA_A_COLUMN = 7;
static constexpr int FIXED_AREA_B_COLUMN = 11;
static constexpr int FIXED_IDENTIFICATION_COLUMN = 72;

static bool
IsCobolKeyword(const tchar_t* pszChars, int nLength)
{
	return ISXKEYWORDI(s_apszCobolKeywordList, pszChars, nLength);
}

static bool
IsCobolLiteralPrefix(const tchar_t* pszChars, int nLength)
{
	return ISXKEYWORDI(s_apszCobolLiteralPrefixList, pszChars, nLength);
}

static bool
IsWord(const tchar_t* pszChars, int nLength, const tchar_t* pszWord)
{
	return static_cast<int>(tc::tcslen(pszWord)) == nLength && tc::tcsnicmp(pszChars, pszWord, nLength) == 0;
}

static inline bool
IsCobolWordChar(tchar_t c)
{
	return xisalnum(c) || c == '-';
}

static inline bool
IsQuote(tchar_t c)
{
	return c == '"' || c == '\'';
}

static bool
IsAllDigits(const tchar_t* pszChars, int nLength)
{
	if (nLength <= 0)
		return false;
	for (int i = 0; i < nLength; ++i)
	{
		if (!tc::istdigit(pszChars[i]))
			return false;
	}
	return true;
}

/**
 * @brief Return the position just after the literal starting with the quote at nStart.
 * A doubled quote inside the literal stands for one quote character.
 */
static int
ScanLiteral(const tchar_t* pszChars, int nStart, int nEnd)
{
	const tchar_t cQuote = pszChars[nStart];
	int i = nStart + 1;
	while (i < nEnd)
	{
		if (pszChars[i] == cQuote)
		{
			if (i + 1 < nEnd && pszChars[i + 1] == cQuote)
			{
				i += 2;
				continue;
			}
			return i + 1;
		}
		++i;
	}
	return nEnd;
}

/**
 * @brief Return true if the line looks like reference (fixed) format source.
 */
static bool
IsFixedFormatLine(const tchar_t* pszChars, int nLength)
{
	if (nLength <= FIXED_INDICATOR_COLUMN)
		return false;

	bool bDigitsOrSpaces = true;
	bool bHasSpace = false;
	for (int i = 0; i < FIXED_INDICATOR_COLUMN; ++i)
	{
		const tchar_t c = pszChars[i];
		if (c == ' ')
			bHasSpace = true;
		else if (!tc::istdigit(c))
			bDigitsOrSpaces = false;
	}

	const tchar_t cIndicator = pszChars[FIXED_INDICATOR_COLUMN];
	switch (cIndicator)
	{
	case '*':
	case '/':
		// Sequence areas sometimes contain letters (e.g. "PGM010").
		return bDigitsOrSpaces || !bHasSpace;
	case ' ':
	case '-':
		return bDigitsOrSpaces;
	case 'D':
	case 'd':
		return bDigitsOrSpaces && (nLength == FIXED_INDICATOR_COLUMN + 1 || pszChars[FIXED_INDICATOR_COLUMN + 1] == ' ');
	default:
		return false;
	}
}

/**
 * @brief Return true if the word ending at nWordEnd is followed by a period
 * or by the word SECTION, i.e. it is a paragraph name or a section name.
 */
static bool
IsFollowedByLabelTerminator(const tchar_t* pszChars, int nWordEnd, int nEnd)
{
	if (nWordEnd < nEnd && pszChars[nWordEnd] == '.')
		return true;
	int i = nWordEnd;
	while (i < nEnd && xisspace(pszChars[i]))
		++i;
	int j = i;
	while (j < nEnd && IsCobolWordChar(pszChars[j]))
		++j;
	return j > i && IsWord(pszChars + i, j - i, _T("SECTION"));
}

unsigned
CrystalLineParser::ParseLineCobol(unsigned /*dwCookie*/, const tchar_t* pszChars, int nLength, std::vector<TEXTBLOCK>* pBuf)
{
	if (nLength == 0 || pBuf == nullptr)
		return 0;

	int nBegin = 0;
	int nEnd = nLength;
	const bool bFixed = IsFixedFormatLine(pszChars, nLength);
	if (bFixed)
	{
		const tchar_t cIndicator = pszChars[FIXED_INDICATOR_COLUMN];
		// Sequence number area (and the whole line for comment lines)
		DEFINE_BLOCK(0, COLORINDEX_COMMENT);
		if (cIndicator == '*' || cIndicator == '/')
			return 0;
		// Indicator area
		if (cIndicator != ' ')
			DEFINE_BLOCK(FIXED_INDICATOR_COLUMN, COLORINDEX_PREPROCESSOR);
		nBegin = FIXED_AREA_A_COLUMN;
		nEnd = (std::min)(nLength, FIXED_IDENTIFICATION_COLUMN);
	}

	auto defineNormalText = [&](int nPos)
	{
		if (nPos < nEnd)
			DEFINE_BLOCK(nPos, COLORINDEX_NORMALTEXT);
	};

	defineNormalText(nBegin);

	enum class Expect { None, FunctionName, Picture };
	Expect expect = Expect::None;
	bool bFirstToken = true;
	int i = nBegin;
	while (i < nEnd)
	{
		const tchar_t c = pszChars[i];
		if (xisspace(c))
		{
			++i;
			continue;
		}

		const bool bFirst = bFirstToken;
		bFirstToken = false;

		// Floating comment indicator "*>"
		if (c == '*' && i + 1 < nEnd && pszChars[i + 1] == '>')
		{
			DEFINE_BLOCK(i, COLORINDEX_COMMENT);
			break;
		}

		// Compiler directives: >>SOURCE, >>IF, >>DEFINE, $SET, ...
		if (bFirst && ((c == '>' && i + 1 < nEnd && pszChars[i + 1] == '>') || c == '$'))
		{
			DEFINE_BLOCK(i, COLORINDEX_PREPROCESSOR);
			break;
		}

		// Character-string of a PICTURE clause: 9(5)V99, X(10), ZZ,ZZ9.99-, ...
		if (expect == Expect::Picture)
		{
			int j = i;
			while (j < nEnd && !xisspace(pszChars[j]))
				++j;
			// A trailing separator period, comma or semicolon is not part of the picture string
			if (j - 1 > i && (pszChars[j - 1] == '.' || pszChars[j - 1] == ',' || pszChars[j - 1] == ';'))
				--j;
			if (!IsWord(pszChars + i, j - i, _T("IS")))
			{
				DEFINE_BLOCK(i, COLORINDEX_USER1);
				defineNormalText(j);
				expect = Expect::None;
				i = j;
				continue;
			}
		}

		// Alphanumeric or national literal
		if (IsQuote(c))
		{
			DEFINE_BLOCK(i, COLORINDEX_STRING);
			i = ScanLiteral(pszChars, i, nEnd);
			defineNormalText(i);
			expect = Expect::None;
			continue;
		}

		// User-defined word, reserved word or numeric literal
		if (xisalnum(c))
		{
			int j = i;
			while (j < nEnd && IsCobolWordChar(pszChars[j]))
				++j;
			// A COBOL word cannot end with a hyphen
			while (j - 1 > i && pszChars[j - 1] == '-')
				--j;
			const int nWordLen = j - i;
			Expect nextExpect = Expect::None;

			if (j < nEnd && IsQuote(pszChars[j]) && IsCobolLiteralPrefix(pszChars + i, nWordLen))
			{
				// Hexadecimal, national or boolean literal: X"FF", N"...", B"1010", ...
				DEFINE_BLOCK(i, COLORINDEX_STRING);
				j = ScanLiteral(pszChars, j, nEnd);
			}
			else if (IsAllDigits(pszChars + i, nWordLen))
			{
				// Decimal fraction: 3.14 (or 3,14 with DECIMAL-POINT IS COMMA)
				if (j + 1 < nEnd && (pszChars[j] == '.' || pszChars[j] == ',') && tc::istdigit(pszChars[j + 1]))
				{
					j += 2;
					while (j < nEnd && tc::istdigit(pszChars[j]))
						++j;
				}
				DEFINE_BLOCK(i, COLORINDEX_NUMBER);
			}
			else if (expect == Expect::FunctionName && !IsWord(pszChars + i, nWordLen, _T("ALL")))
			{
				// Intrinsic function name: FUNCTION UPPER-CASE(...)
				DEFINE_BLOCK(i, COLORINDEX_FUNCNAME);
			}
			else if (IsCobolKeyword(pszChars + i, nWordLen))
			{
				DEFINE_BLOCK(i, COLORINDEX_KEYWORD);
				if (IsWord(pszChars + i, nWordLen, _T("PIC")) || IsWord(pszChars + i, nWordLen, _T("PICTURE")))
					nextExpect = Expect::Picture;
				else if (IsWord(pszChars + i, nWordLen, _T("FUNCTION")))
					nextExpect = Expect::FunctionName;
				else if (expect == Expect::Picture && IsWord(pszChars + i, nWordLen, _T("IS")))
					nextExpect = Expect::Picture;
			}
			else if (bFirst && (!bFixed || i < FIXED_AREA_B_COLUMN) && IsFollowedByLabelTerminator(pszChars, j, nEnd))
			{
				// Paragraph name or section name
				DEFINE_BLOCK(i, COLORINDEX_FUNCNAME);
			}
			else
			{
				DEFINE_BLOCK(i, COLORINDEX_NORMALTEXT);
			}
			defineNormalText(j);
			expect = nextExpect;
			i = j;
			continue;
		}

		// Separators and operators: . , ; ( ) + - * / ** = < > <= >= <> : &
		DEFINE_BLOCK(i, COLORINDEX_OPERATOR);
		++i;
		defineNormalText(i);
		expect = Expect::None;
	}

	// Program identification area
	if (bFixed && nLength > FIXED_IDENTIFICATION_COLUMN)
		DEFINE_BLOCK(FIXED_IDENTIFICATION_COLUMN, COLORINDEX_COMMENT);

	return 0;
}
