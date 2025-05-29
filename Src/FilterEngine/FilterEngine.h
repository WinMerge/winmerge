#pragma once

#include <string>

class CDiffContext;
class DIFFITEM;
struct ExprNode;
namespace Poco { class Timestamp; }

struct FilterContext
{
	FilterContext(const CDiffContext* ctxt);
	~FilterContext();
	void Clear();
	void UpdateTimestamp();
	ExprNode* RegisterNode(ExprNode* node);
	std::vector<ExprNode*> allocatedNodes;
	const CDiffContext* ctxt;
	Poco::Timestamp* now;
	Poco::Timestamp* today;
	ExprNode* rootNode;
	int errorCode;
};

class FilterEngine
{
public:
	enum ErrorCode
	{
		ERROR_NO_ERROR = 0,
		ERROR_UNKNOWN_CHAR = 1,
		ERROR_UNTERMINATED_STRING = 2,
		ERROR_SYNTAX_ERROR = 3,
		ERROR_PARSE_FAILURE = 4
	};
	static bool Parse(const std::wstring& expression, FilterContext& ctxt);
	static bool Evaluate(FilterContext& ctxt, const DIFFITEM& di);
};
