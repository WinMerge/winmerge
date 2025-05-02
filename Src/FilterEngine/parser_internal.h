#pragma once

#include <string>
#include <variant>

typedef std::variant<int, std::string, bool> ValueType;
typedef struct {
    ValueType value;
    std::string string;
    int integer;
    bool boolean;
} YYSTYPE;

extern YYSTYPE yylval;
extern char* yytext;
int yylex(const char* str);
int Parse(void* yyp, int yymajor, YYSTYPE yyminor);
void* ParseAlloc(void* (*mallocProc)(size_t));
void ParseFree(void* yyp, void (*freeProc)(void*));
