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
	const char* string;
	int64_t integer;
	bool boolean;
	ExprNode* node;
	std::vector<ExprNode*> *nodeList;
};

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

	std::string UnescapeQuotes(char*& str);
	const char* DupString(const char* str);
	void FreeStrings();

	YYSTYPE yylval{};
	char* yycursor = nullptr;
	char* YYMARKER = nullptr;
	char* YYCURSOR = nullptr;
	char* YYLIMIT = nullptr;
	std::vector<char*> strings;
};
