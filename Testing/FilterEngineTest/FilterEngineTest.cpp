#include "parser.h"
#include "parser_internal.h"
#include <iostream>
#include <string>
#include <map>

YYSTYPE yylval;
char* yytext;

extern unsigned char* yycursor;
extern int yylex();

int main()
{
	std::string whereClause;
	std::cout << "WHERE??????????: ";
	std::getline(std::cin, whereClause);

	unsigned char* input = (unsigned char*)whereClause.c_str();
	yycursor = input;

	void* parser = ParseAlloc(malloc);
	int token;

	while ((token = yylex()) != 0) {
		Parse(parser, token, yylval);
		yytext = (char*)yycursor;
	}
	Parse(parser, 0, yylval);

	ExprNode* rootNode = (ExprNode*)parser;

	if (rootNode)
	{
		std::map<std::string, std::variant<int, std::string, bool>> data = {
			{"age", 30},
			{"name", "Alice"},
			{"city", "Tokyo"},
			{"is_active", true}
		};

		bool result = rootNode->evaluate(data);
		std::cout << "????: " << (result ? "true" : "false") << std::endl;

		delete rootNode;
	}
	else
	{
		std::cerr << "???????????" << std::endl;
	}

	ParseFree(parser, free);

	return 0;
}