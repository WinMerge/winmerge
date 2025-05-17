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
	void UpdateTimestamp();
	const CDiffContext* ctxt;
	Poco::Timestamp* now;
	Poco::Timestamp* today;
	ExprNode* rootNode;
};

class FilterEngine
{
public:
	static void Parse(const std::wstring& expression, FilterContext& ctxt);
	static bool Evaluate(FilterContext& ctxt, const DIFFITEM& di);
};
