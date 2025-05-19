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
	"AND"             { return AND; }
	"OR"              { return OR; }
	"NOT"             { return NOT; }
	"TRUE"            { yylval.boolean = true; return TRUE_LITERAL; }
	"FALSE"           { yylval.boolean = false; return FALSE_LITERAL; }
	"CONTAINS"        { return CONTAINS; }
	"MATCHES"         { return MATCHES; }
	[a-zA-Z_][a-zA-Z0-9_]* {
		const char* p = yycursor;
		while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')
			p++;
		std::string tmp = std::string(p, YYCURSOR - p);
		yylval.string = dupString(tmp.c_str());
		return IDENTIFIER;
	}
	[0-9]+            { yylval.integer = std::stoi(std::string((const char*)yycursor, YYCURSOR - yycursor)); return INTEGER_LITERAL; }
	"\"" {
		std::string str = unescapeQuotes(YYCURSOR);
		if (*(YYCURSOR - 1) != '"')
			return LEXER_ERR_UNTERMINATED_STRING;
		yylval.string = dupString(str.c_str());
		return STRING_LITERAL;
	}
	"=="              { return EQ; }
	"="               { return EQ; }
	"!="              { return NE; }
	"<"               { return LT; }
	"<="              { return LE; }
	">"               { return GT; }
	">="              { return GE; }
	"("               { return LPAREN; }
	")"               { return RPAREN; }
	"+"               { return PLUS; }
	"-"               { return MINUS; }
	"*"               { return STAR; }
	"/"               { return SLASH; }
	"%"               { return MOD; }
	$                 { return 0; }
	.                 { return LEXER_ERR_UNKNOWN_CHAR; }
	*/
}

std::string FilterLexer::unescapeQuotes(char*& str)
{
	std::string result;
	const char* start = str;
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

const char* FilterLexer::dupString(const char* str)
{
	char* newStr = _strdup(str);
	strings.push_back(newStr);
	return newStr;
}

void FilterLexer::freeStrings()
{
	for (auto str : strings)
		free(str);
	strings.clear();
}

