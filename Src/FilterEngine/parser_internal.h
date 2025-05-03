#pragma once

#include <string>
#include <variant>

struct ExprNode;

typedef struct {
    const char* string;
    int integer;
    bool boolean;
    ExprNode* node;
} YYSTYPE;

extern YYSTYPE yylval;
extern const char* yytext;
int yylex();
void Parse(void* yyp, int yymajor, YYSTYPE yyminor);
void* ParseAlloc(void* (*mallocProc)(size_t));
void ParseFree(void* yyp, void (*freeProc)(void*));
