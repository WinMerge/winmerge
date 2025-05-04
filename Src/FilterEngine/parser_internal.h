#pragma once

#include <string>
#include <variant>
#include <vector>

struct ExprNode;

typedef struct {
    const char* string;
    int integer;
    bool boolean;
    ExprNode* node;
} YYSTYPE;

extern YYSTYPE yylval;
extern const char* yytext;

void lexerError(const char* msg);
const char* dupString(const char* str);

int yylex();
void Parse(void* yyp, int yymajor, YYSTYPE yyminor);
void* ParseAlloc(void* (*mallocProc)(size_t));
void ParseFree(void* yyp, void (*freeProc)(void*));
