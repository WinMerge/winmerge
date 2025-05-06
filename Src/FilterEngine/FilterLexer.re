/*!re2c
re2c:define:YYCTYPE = wchar_t;
re2c:yyfill:enable = 0;
re2c:flags:case-insensitive = 1;
re2c:eof = 0;
*/

#include "pch.h"
#include "FilterParser.h"
#include "FilterEngineInternal.h"
#include <string>
#include <iostream>
#include <vector>

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
	[a-zA-Z_][a-zA-Z0-9_]* {
						  std::wstring tmp = std::wstring((const wchar_t*)yycursor, YYCURSOR - yycursor);
						  yylval.string = dupString(tmp.c_str());
						  return IDENTIFIER;
						}
	[0-9]+            { yylval.integer = std::stoi(std::wstring((const wchar_t*)yycursor, YYCURSOR - yycursor)); return INTEGER_LITERAL; }
	"\"" {
		const wchar_t* start = YYCURSOR;
		std::wstring str;
		while (*YYCURSOR != '\0') {
			if (*YYCURSOR == '"') {
				if (*(YYCURSOR + 1) == '"') {
					str += '"';
					YYCURSOR += 2;
				} else {
					++YYCURSOR;
					break;
				}
			} else {
				str += *YYCURSOR++;
			}
		}
		yylval.string = dupString(str.c_str());
		return STRING_LITERAL;
	}
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
	$                 { return 0; }
	.                 { lexerError(L"?????"); return 0; }
	*/
}
