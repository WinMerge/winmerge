/**
 * @file  FilterExpression.cpp
 *
 * @brief Filter engine implementation.
 */
#include "pch.h"
#include "FilterExpression.h"
#include "FilterExpressionNodes.h"
#include "FilterLexer.h"
#include "DiffContext.h"
#include "DiffItem.h"
#include <Poco/LocalDateTime.h>

extern void Parse(void* yyp, int yymajor, YYSTYPE yyminor, FilterExpression* pCtx);
extern void* ParseAlloc(void* (*mallocProc)(size_t));
extern void ParseFree(void* yyp, void (*freeProc)(void*));

void YYSTYPEDestructor(YYSTYPE& yystype)
{
	if (yystype.nodeList)
	{
		for (auto& node : *yystype.nodeList)
			delete node;
		delete yystype.nodeList;
	}
	delete yystype.node;
	yystype.node = nullptr;
	yystype.nodeList = nullptr;
}

FilterExpression::FilterExpression()
{
}

FilterExpression::FilterExpression(const FilterExpression& other)
	: optimize(other.optimize)
	, ctxt(other.ctxt)
	, now(other.now ? new Poco::Timestamp(*other.now) : nullptr)
	, today(other.today ? new Poco::Timestamp(*other.today) : nullptr)
	, expression(other.expression)
{
	Parse(expression);
}

FilterExpression::FilterExpression(const std::string& expression)
{
	Parse(expression);
}

FilterExpression::~FilterExpression()
{
	Clear();
}

void FilterExpression::Clear()
{
	now.reset();
	today.reset();
	rootNode.reset();
	errorCode = FILTER_ERROR_NO_ERROR;
	errorPosition = -1;
}

void FilterExpression::UpdateTimestamp()
{
	now.reset(new Poco::Timestamp());
	Poco::LocalDateTime ldt(*now);
	Poco::LocalDateTime midnight(ldt.year(), ldt.month(), ldt.day(), 0, 0, 0, 0, 0);
	today.reset(new Poco::Timestamp(midnight.utc().timestamp()));
}

static int getErrorPosition(const FilterExpression* pCtx, const FilterLexer& lexer)
{
	if (pCtx->errorCode == FILTER_ERROR_UNDEFINED_IDENTIFIER)
	{
		return static_cast<int>(lexer.yycursor - strlen(lexer.yylval.string) - pCtx->expression.c_str());
	}
	return static_cast<int>(lexer.yycursor - pCtx->expression.c_str());
}

bool FilterExpression::Parse()
{
	Clear();
	UpdateTimestamp();
	if (expression.empty())
	{
		errorCode = FILTER_ERROR_EMPTY_EXPRESSION;
		errorPosition = 0;
		return false;
	}
	FilterLexer lexer(expression);
	void* prs = ParseAlloc(malloc);
	int token;
	FilterErrorCode firstError = FILTER_ERROR_NO_ERROR;
	while ((token = lexer.yylex()) != 0)
	{
		if (token < 0)
		{
			firstError = static_cast<FilterErrorCode>(-token);
			errorPosition = getErrorPosition(this, lexer);
			break;
		}
		::Parse(prs, token, lexer.yylval, this);
		if (errorCode != 0)
		{
			firstError = errorCode;
			errorPosition = getErrorPosition(this, lexer);
			break;
		}
		lexer.yycursor = lexer.YYCURSOR;
	}
	::Parse(prs, 0, lexer.yylval, this);
	if (firstError == 0 && errorCode != 0)
	{
		firstError = errorCode;
		errorPosition = getErrorPosition(this, lexer);
	}
	::ParseFree(prs, free);
	if (firstError != 0)
		errorCode = firstError;
	return (errorCode == 0 && rootNode != nullptr);
}

bool FilterExpression::Parse(const std::string& expressionStr)
{
	expression = expressionStr;
	return Parse();
}

bool FilterExpression::Evaluate(const DIFFITEM& di)
{
	try
	{
		const auto result = rootNode->Evaluate(di);
		if (const auto boolVal = std::get_if<bool>(&result))
			return *boolVal;
		if (const auto arrayVal = std::get_if<std::shared_ptr<std::vector<ValueType2>>>(&result))
		{
			const auto& vec = *arrayVal->get();
			return std::any_of(vec.begin(), vec.end(), [](const ValueType2& item) {
				const auto boolVal = std::get_if<bool>(&item.value);
				return boolVal && *boolVal;
				});
		}
		return false;
	}
	catch (const Poco::RegularExpressionException& e)
	{
		errorCode = FILTER_ERROR_INVALID_REGULAR_EXPRESSION;
		errorPosition = -1;
		errorMessage = e.message();
		if (logger)
			logger("FilterExpression evaluation error: " + errorMessage);
		return false;
	}
	catch (const std::exception& e)
	{
		errorCode = FILTER_ERROR_EVALUATION_FAILED;
		errorPosition = -1;
		errorMessage = e.what();
		if (logger)
			logger("FilterExpression evaluation error: " + errorMessage);
		return false;
	}
}
