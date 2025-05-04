#include "parser.h"
#include "parser_internal.h"
#include "node.h"
#include <iostream>
#include <string>
#include <map>

ExprNode* ParseFilterCondition(const std::string& str);

int main()
{
	std::string str;
	std::cout << "Filter condition: ";
	std::getline(std::cin, str);

	ExprNode* rootNode = ParseFilterCondition(str);

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
			std::cout << "result: unknown type" << std::endl;
		delete rootNode;
	}
	else
	{
		std::cerr << "error" << std::endl;
	}

	return 0;
}