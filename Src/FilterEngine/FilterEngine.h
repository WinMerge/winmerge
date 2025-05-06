#pragma once

#include <string>
#include <map>
#include <variant>
#include <memory>
#include <optional>

class CDiffContext;
class DIFFITEM;
struct ExprNode;

class FilterEngine
{
public:
	struct ParseResult
	{
		std::shared_ptr<ExprNode> root;
		std::optional<std::string> error;
	};

	static ParseResult Parse(const std::wstring& expression, const CDiffContext& ctxt);

	static bool Evaluate(const std::shared_ptr<ExprNode>& expr, const DIFFITEM& di);
};
