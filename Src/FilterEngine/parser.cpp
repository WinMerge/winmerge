#include "parser.c"
#include "parser_internal.h"
#include <vector>

extern char* yycursor;
extern char* YYMARKER;
extern char* YYCURSOR;
extern char* YYLIMIT;
extern int yylex();
const char* yytext;
YYSTYPE result;
std::vector<char*> strings;
YYSTYPE yylval;

void lexerError(const char* msg) {
    std::cerr << "Lexer Error: " << msg << std::endl;
}

const char* dupString(const char* str)
{
    char* newStr = _strdup(str);
    strings.push_back(newStr);
    return newStr;
}

ExprNode* ParseFilterCondition(const std::string& str)
{
	char* input = (char*)str.c_str();
	yycursor = input;
	YYCURSOR = input;
	YYLIMIT = YYCURSOR + str.length();

	void* parser = ParseAlloc(malloc);
	int token;
	std::string tmp;

	while ((token = yylex()) != 0) {
		Parse(parser, token, yylval);
		if (token == IDENTIFIER || token == STRING_LITERAL)
			yytext = yylval.string;
		else
		{
			tmp = std::string(yycursor, YYCURSOR - yycursor);
			yycursor = YYCURSOR;
			yytext = tmp.c_str();
		}
	}
	Parse(parser, 0, yylval);

	ExprNode* rootNode = (ExprNode*)result.node;

	ParseFree(parser, free);

	return rootNode;
}

