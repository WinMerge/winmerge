#pragma once

#include <string>
#include <variant>
#include <vector>

struct ExprNode;
class CDiffContext;

typedef struct {
	const wchar_t* string;
	int integer;
	bool boolean;
	const CDiffContext* ctxt;
	ExprNode* node;
} YYSTYPE;

struct FilterLexer
{
	FilterLexer(const std::wstring& input)
		: yycursor((wchar_t*)input.c_str())
		, YYCURSOR((wchar_t*)input.c_str())
		, YYLIMIT((wchar_t*)input.c_str() + input.length())
	{
	}

	~FilterLexer()
	{
		freeStrings();
	}

	int yylex();

	const wchar_t* dupString(const wchar_t* str)
	{
		wchar_t* newStr = _wcsdup(str);
		strings.push_back(newStr);
		return newStr;
	}

	void freeStrings()
	{
		for (auto str : strings)
			free(str);
		strings.clear();
	}

	void lexerError(const wchar_t* msg)
	{
		std::cerr << "Lexer Error: " << msg << std::endl;
	}

	YYSTYPE yylval{};
	wchar_t* yycursor = nullptr;
	wchar_t* YYMARKER = nullptr;
	wchar_t* YYCURSOR = nullptr;
	wchar_t* YYLIMIT = nullptr;
	std::vector<wchar_t*> strings;
};

extern YYSTYPE resultFilterExpression;
void Parse(void* yyp, int yymajor, YYSTYPE yyminor);
void* ParseAlloc(void* (*mallocProc)(size_t));
void ParseFree(void* yyp, void (*freeProc)(void*));
