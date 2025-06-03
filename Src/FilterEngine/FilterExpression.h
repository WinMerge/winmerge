/**
 * @file  FilterExpression.h
 *
 * @brief Header file for the FilterExpression class, which provides functionality to parse and evaluate filter expressions.
 */
#pragma once

#include <string>
#include <memory>

class CDiffContext;
class DIFFITEM;
struct ExprNode;
struct YYSTYPE;
namespace Poco { class Timestamp; }

struct FilterExpression
{
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
	FilterExpression();
	FilterExpression(const FilterExpression& other);
	FilterExpression(const std::string& expression);
	~FilterExpression();
	bool Parse(const std::string& expression);
	bool Parse();
	void SetDiffContext(const CDiffContext* pCtxt) { ctxt = pCtxt; }
	bool Evaluate(const DIFFITEM& di);
	void UpdateTimestamp();
	void Clear();
	bool optimize = true;
	const CDiffContext* ctxt = nullptr;
	std::unique_ptr<Poco::Timestamp> now;
	std::unique_ptr<Poco::Timestamp> today;
	std::unique_ptr<ExprNode> rootNode;
	std::string expression;
	int errorCode = 0;
	int errorPosition = -1;
};
