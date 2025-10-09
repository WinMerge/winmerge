/**
 * @file  FilterLexer.h
 *
 * @brief Header file for the FilterLexer class, which is responsible for tokenizing filter expressions.
 */
#pragma once

#include <string>
#include <variant>
#include <vector>

struct ExprNode;

struct YYSTYPE
{
	ExprNode* node;
	std::vector<ExprNode*> *nodeList;
	const char* string;
	int64_t integer;
	double real;
	bool boolean;
};

extern void YYSTYPEDestructor(YYSTYPE& yystype);

struct FilterLexer
{
	enum ErrorCode {
		LEXER_ERR_UNKNOWN_CHAR = -1,
		LEXER_ERR_UNTERMINATED_STRING = -2,
	};

	FilterLexer(const std::string& input)
		: yycursor((char*)input.c_str())
		, YYCURSOR((char*)input.c_str())
		, YYLIMIT((char*)input.c_str() + input.length())
	{
	}

	~FilterLexer()
	{
		FreeStrings();
	}

	int yylex();

	std::string UnescapeQuotes(char*& str, int& errorCode);
	const char* DupString(const char* str);
	void FreeStrings();

	YYSTYPE yylval{};
	char* yycursor = nullptr;
	char* YYMARKER = nullptr;
	char* YYCURSOR = nullptr;
	char* YYLIMIT = nullptr;
	std::vector<char*> strings;
};
