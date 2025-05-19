#pragma once

#include <string>
#include <variant>
#include <vector>

struct ExprNode;
struct FilterContext;

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
		freeStrings();
	}

	int yylex();

	std::string unescapeQuotes(char*& str)
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

	const char* dupString(const char* str)
	{
		char* newStr = _strdup(str);
		strings.push_back(newStr);
		return newStr;
	}

	void freeStrings()
	{
		for (auto str : strings)
			free(str);
		strings.clear();
	}

	YYSTYPE yylval{};
	char* yycursor = nullptr;
	char* YYMARKER = nullptr;
	char* YYCURSOR = nullptr;
	char* YYLIMIT = nullptr;
	std::vector<char*> strings;
};

void Parse(void* yyp, int yymajor, YYSTYPE yyminor, FilterContext* pCtx);
void* ParseAlloc(void* (*mallocProc)(size_t));
void ParseFree(void* yyp, void (*freeProc)(void*));
