/*!re2c
re2c:define:YYCTYPE = char;
re2c:yyfill:enable = 0;
re2c:flags:case-insensitive = 1;
re2c:eof = 0;
*/

#include "parser.h"
#include "parser_internal.h"
#include <string>
#include <iostream>
#include <vector>

YYSTYPE yylval;
std::vector<char*> strings;

void lexerError(const char* msg) {
    std::cerr << "Lexer Error: " << msg << std::endl;
}

const char* dupString(const char* str)
{
    char* newStr = _strdup(str);
    strings.push_back(newStr);
    return newStr;
}

char* yycursor = nullptr;
char* YYMARKER = nullptr;
char* YYCURSOR = nullptr;
char* YYLIMIT = nullptr;

int yylex()
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
                          std::string tmp = std::string((const char*)yycursor, YYCURSOR - yycursor);
                          yylval.string = dupString(tmp.c_str());
                          return IDENTIFIER;
                        }
    [0-9]+            { yylval.integer = std::stoi(std::string((const char*)yycursor, YYCURSOR - yycursor)); return INTEGER_LITERAL; }
    '"' [^"]* '"'     {
                          std::string tmp = std::string((const char*)yycursor, YYCURSOR - yycursor);
                          yylval.string = dupString(tmp.c_str());
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
    $                 { return 0; }
   .                  { lexerError("?????"); return 0; }
    */
}