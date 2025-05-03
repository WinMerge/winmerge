#include "parser.h"
#include "parser_internal.h"
#include "node.h"
#include <iostream>
#include <string>
#include <map>

extern YYSTYPE yylval;
const char* yytext;
YYSTYPE result;

extern char* yycursor;
extern char* YYMARKER;
extern char* YYCURSOR;
extern char* YYLIMIT;
extern int yylex();

int main()
{
	std::string whereClause;
	std::cout << "WHERE??????????: ";
	std::getline(std::cin, whereClause);

	char* input = (char*)whereClause.c_str();
	yycursor = input;
	YYCURSOR = input;
	YYLIMIT = YYCURSOR + whereClause.length();

	void* parser = ParseAlloc(malloc);
	int token;
	std::string tmp;

	while ((token = yylex()) != 0) {
		Parse(parser, token, yylval);
		tmp = std::string(yycursor, YYCURSOR - yycursor);
		yycursor = YYCURSOR;
		yytext = tmp.c_str();
	}
	Parse(parser, 0, yylval);

	ExprNode* rootNode = (ExprNode*)result.node;

	if (rootNode)
	{
		std::map<std::string, std::variant<int, std::string, bool>> data = {
			{"age", 30},
			{"name", "Alice"},
			{"city", "Tokyo"},
			{"is_active", true}
		};

		auto result = rootNode->evaluate(data);
		if (auto boolVal = std::get_if<bool>(&result))
			std::cout << "result: " << (*boolVal ? "true" : "false") << std::endl;
		else if (auto intVal = std::get_if<int>(&result))
			std::cout << "result: " << *intVal << std::endl;
		else if (auto strVal = std::get_if<std::string>(&result))
			std::cout << "result: " << *strVal << std::endl;
		else

		delete rootNode;
	}
	else
	{
		std::cerr << "???????????" << std::endl;
	}

	ParseFree(parser, free);

	return 0;
}