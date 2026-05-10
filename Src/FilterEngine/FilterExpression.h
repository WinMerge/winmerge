/**
 * @file  FilterExpression.h
 *
 * @brief Header file for the FilterExpression class, which provides functionality to parse and evaluate filter expressions.
 */
#pragma once

#include <string>
#include <memory>
#include <map>
#include "FilterError.h"
#include "UnicodeString.h"
#include <Poco/Timestamp.h>

class CDiffContext;
class DIFFITEM;
struct ExprNode;
struct YYSTYPE;
struct ILineDataProvider;
struct FilterExpression;
struct FilterSharedContext;

struct FilterEvalContext
{
	const FilterExpression* expr = nullptr;
	const DIFFITEM* di = nullptr;
	const ILineDataProvider* provider = nullptr;
	const FilterSharedContext* sharedContext = nullptr;
	int lineIndex = -1;
};

using Range = std::pair<int, int>;
using InsideKey = std::pair<ExprNode*, ExprNode*>;

struct StatisticsResult
{
	double average = 0.0;
	double maxNumber = 0.0;
	double minNumber = 0.0;
	std::string maxString;
	std::string minString;
	Poco::Timestamp maxTimestamp;
	Poco::Timestamp minTimestamp;
	int64_t count = 0;
	int valueType = 0; // 0=none, 1=number, 2=string, 3=timestamp
};

struct FilterSharedContext
{
	FilterSharedContext() = default;
	FilterSharedContext(const FilterSharedContext&) = delete;
	FilterSharedContext& operator=(const FilterSharedContext&) = delete;

	mutable std::map<ExprNode*, std::vector<std::pair<int, int>>> matchRanges;
	mutable std::map<InsideKey, std::vector<Range>> insideRanges;
	mutable std::map<ExprNode*, StatisticsResult> statistics;
};

struct FilterExpression
{
	FilterExpression();
	FilterExpression(const FilterExpression& other);
	FilterExpression(const std::string& expression);
	~FilterExpression();
	bool Parse(const std::string& expression);
	bool Parse();
	void SetDiffContext(const CDiffContext* pCtxt) { ctxt = pCtxt; }
	bool Evaluate(const DIFFITEM& di) { FilterEvalContext ectxt{ this, &di }; return Evaluate(ectxt); }
	bool Evaluate(const FilterEvalContext& ectxt);
	std::vector<String> EvaluateKeys(const DIFFITEM& di);
	void UpdateTimestamp();
	void Clear();
	std::vector<std::string> GetPropertyNames() const;
	static void SetLogger(std::function<void(int level, const std::string&)> func) { logger = func; };

	struct DirectivesAndExpr
	{
		String directives;
		String expr;
	};

	static bool HasCaseSensitiveDirective(const String& expression);
	static String AddCaseSensitiveDirective(const String& expression);
	static String RemoveCaseSensitiveDirective(const String& expression);
	static String ExtractDirectivesPrefix(const String& expression);
	static String RemoveAllDirectives(const String& expression);
	static DirectivesAndExpr SplitDirectivesAndExpr(const String& expression);
	static String MergeDirectives(const String& directives1, const String& directives2);
	bool optimize = true;
	bool caseSensitive = false;
	bool diritem = true;
	std::string name;
	const CDiffContext* ctxt = nullptr;
	std::unique_ptr<Poco::Timestamp> now;
	std::unique_ptr<Poco::Timestamp> today;
	std::unique_ptr<ExprNode> rootNode;
	std::string expression;
	FilterErrorCode errorCode = FILTER_ERROR_NO_ERROR;
	int errorPosition = -1;
	std::string errorMessage;
	inline static std::function<void(int, const std::string&)> logger;
private:
	bool ParseDirective(const std::string& directive);
	bool ParseAllDirectives(const std::string& expressionStr, std::string& actualExpression);
};
