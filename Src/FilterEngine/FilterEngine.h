/**
 * @file  FilterEngine.h
 *
 * @brief Header file for the FilterEngine class, which provides functionality to parse and evaluate filter expressions.
 */
#pragma once

#include <string>
#include <memory>

class CDiffContext;
class DIFFITEM;
struct ExprNode;
struct YYSTYPE;
namespace Poco { class Timestamp; }

struct FilterContext
{
	FilterContext(const CDiffContext* ctxt);
	~FilterContext();
	void Clear();
	void UpdateTimestamp();
	void YYSTYPEDestructor(YYSTYPE& yystype);
	bool optimize = true;
	const CDiffContext* ctxt = nullptr;
	std::unique_ptr<Poco::Timestamp> now;
	std::unique_ptr<Poco::Timestamp> today;
	std::unique_ptr<ExprNode> rootNode;
	int errorCode = 0;
	int errorPosition = -1;
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
		ERROR_PARSE_FAILURE = 4,
		ERROR_INVALID_LITERAL = 5,
		ERROR_INVALID_REGULAR_EXPRESSION = 6,
		ERROR_UNDEFINED_IDENTIFIER = 7,
		ERROR_INVALID_ARGUMENT_COUNT = 8,
	};
	static bool Parse(const std::wstring& expression, FilterContext& ctxt);
	static bool Evaluate(FilterContext& ctxt, const DIFFITEM& di);
};
