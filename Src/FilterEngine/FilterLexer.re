/**
 * @file  FilterLexer.re
 *
 * @brief Lexer for the filter parser.
 */
/*!re2c
re2c:define:YYCTYPE = char;
re2c:yyfill:enable = 0;
re2c:flags:case-insensitive = 1;
re2c:eof = 0;
*/

#include "pch.h"
#include "FilterParser.h"
#include "FilterLexer.h"
#include <string>

int FilterLexer::yylex()
{
begin:
	/*!re2c
	[ \t\r\n]+        { goto begin; }
	"AND"             { return TK_AND; }
	"OR"              { return TK_OR; }
	"NOT"             { return TK_NOT; }
	"TRUE"            { yylval.boolean = true; return TK_TRUE_LITERAL; }
	"FALSE"           { yylval.boolean = false; return TK_FALSE_LITERAL; }
	"CONTAINS"        { return TK_CONTAINS; }
	"RECONTAINS"      { return TK_RECONTAINS; }
	"LIKE"            { return TK_LIKE; }
	"MATCHES"         { return TK_MATCHES; }
	([0-9]+([.][0-9]+)?)("KB"|"MB"|"GB"|"TB") {
		const char* p = yycursor;
		while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')
			p++;
		std::string lit(p, YYCURSOR - p);
		yylval.string = DupString(lit.c_str());
		return TK_SIZE_LITERAL;
	}
	([0-9]+([.][0-9]+)?)("weeks"|"week"|"w"|"days"|"day"|"d"|"hours"|"hour"|"hr"|"h"|"minutes"|"minute"|"min"|"m"|"seconds"|"second"|"sec"|"s"|"milliseconds"|"millisecond"|"msec"|"ms") {
		const char* p = yycursor;
		while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')
			p++;
		std::string lit(p, YYCURSOR - p);
		yylval.string = DupString(lit.c_str());
		return TK_DURATION_LITERAL;
	}
	"d\"" {
		std::string str = UnescapeQuotes(YYCURSOR);
		if (*(YYCURSOR - 1) != '"')
			return LEXER_ERR_UNTERMINATED_STRING;
		yylval.string = DupString(str.c_str());
		return TK_DATETIME_LITERAL;
	}
	"v\"" {
		std::string str = UnescapeQuotes(YYCURSOR);
		if (*(YYCURSOR - 1) != '"')
			return LEXER_ERR_UNTERMINATED_STRING;
		yylval.string = DupString(str.c_str());
		return TK_VERSION_LITERAL;
	}
	[0-9]+ {
		yylval.integer = std::stoi(std::string((const char*)yycursor, YYCURSOR - yycursor));
		return TK_INTEGER_LITERAL;
	}
	[a-zA-Z_][a-zA-Z0-9_]* {
		const char* p = yycursor;
		while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')
			p++;
		std::string tmp = std::string(p, YYCURSOR - p);
		yylval.string = DupString(tmp.c_str());
		return TK_IDENTIFIER;
	}
	"\"" {
		std::string str = UnescapeQuotes(YYCURSOR);
		if (*(YYCURSOR - 1) != '"')
			return LEXER_ERR_UNTERMINATED_STRING;
		yylval.string = DupString(str.c_str());
		return TK_STRING_LITERAL;
	}
	"=="              { return TK_EQ; }
	"="               { return TK_EQ; }
	"!="              { return TK_NE; }
	"<"               { return TK_LT; }
	"<="              { return TK_LE; }
	">"               { return TK_GT; }
	">="              { return TK_GE; }
	"("               { return TK_LPAREN; }
	")"               { return TK_RPAREN; }
	"+"               { return TK_PLUS; }
	"-"               { return TK_MINUS; }
	"*"               { return TK_STAR; }
	"/"               { return TK_SLASH; }
	"%"               { return TK_MOD; }
	$                 { return 0; }
	","               { return TK_COMMA; }
	.                 { return LEXER_ERR_UNKNOWN_CHAR; }
	*/
}

std::string FilterLexer::UnescapeQuotes(char*& str)
{
	std::string result;
	while (*str != '\0')
	{
		if (*str == '"')
		{
			if (*(str + 1) == '"')
			{
				result += '"';
				str += 2;
			}
			else
			{
				str++;
				break;
			}
		}
		else
		{
			result += *str++;
		}
	}
	return result;
}

const char* FilterLexer::DupString(const char* str)
{
	char* newStr = _strdup(str);
	strings.push_back(newStr);
	return newStr;
}

void FilterLexer::FreeStrings()
{
	for (auto str : strings)
		free(str);
	strings.clear();
}

