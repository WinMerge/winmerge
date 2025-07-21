/**
 * @file  FilterExpression.h
 *
 * @brief Header file for the FilterExpression class, which provides functionality to parse and evaluate filter expressions.
 */
#pragma once

#include <string>
#include <memory>
#include "FilterError.h"

class CDiffContext;
class DIFFITEM;
struct ExprNode;
struct YYSTYPE;
namespace Poco { class Timestamp; }

struct FilterExpression
{
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
	static void SetLogger(std::function<void(const std::string&)> func) { logger = func; };
	bool optimize = true;
	const CDiffContext* ctxt = nullptr;
	std::unique_ptr<Poco::Timestamp> now;
	std::unique_ptr<Poco::Timestamp> today;
	std::unique_ptr<ExprNode> rootNode;
	std::string expression;
	FilterErrorCode errorCode = FILTER_ERROR_NO_ERROR;
	int errorPosition = -1;
	std::string errorMessage;
	inline static std::function<void(const std::string&)> logger;
};
