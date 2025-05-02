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

YYSTYPE yylval;

void lexerError(const char* msg) {
    std::cerr << "Lexer Error: " << msg << std::endl;
}

char* yycursor;

int yylex()
{
begin:
    char* YYMARKER = nullptr;
    char* YYCURSOR = yycursor;
    char* YYLIMIT = yycursor + strlen(yycursor);
    /*!re2c
    [ \t\r\n]+        { goto begin; }
    "AND"             { return AND; }
    "OR"              { return OR; }
    "NOT"             { return NOT; }
    "TRUE"            { yylval.boolean = true; return TRUE_LITERAL; }
    "FALSE"           { yylval.boolean = false; return FALSE_LITERAL; }
    [a-zA-Z_][a-zA-Z0-9_]* {
                          yylval.string = std::string((const char*)yycursor, YYCURSOR - yycursor);
                          return IDENTIFIER;
                        }
    [0-9]+            { yylval.integer = std::stoi(std::string((const char*)yycursor, YYCURSOR - yycursor)); return INTEGER_LITERAL; }
    '"' [^"]* '"'     {
                          std::string str((const char*)yycursor + 1, YYCURSOR - yycursor - 2);
                          yylval.string = str;
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
    $                 { return 0; }
   .                  { lexerError("?????"); return 0; }
    */
}