/**
 * @file  FilterEngine.cpp
 *
 * @brief Filter engine implementation.
 */
#include "pch.h"
#include "FilterEngine.h"
#include "FilterExpression.h"
#include "FilterLexer.h"
#include "DiffContext.h"
#include "DiffItem.h"
#include "unicoder.h"
#include <Poco/LocalDateTime.h>

extern void Parse(void* yyp, int yymajor, YYSTYPE yyminor, FilterContext* pCtx);
extern void* ParseAlloc(void* (*mallocProc)(size_t));
extern void ParseFree(void* yyp, void (*freeProc)(void*));

FilterContext::FilterContext(const CDiffContext* ctxt)
	: ctxt(ctxt)
{
	UpdateTimestamp();
}

FilterContext::~FilterContext()
{
	Clear();
}

void FilterContext::YYSTYPEDestructor(YYSTYPE& yystype)
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

void FilterContext::Clear()
{
	now.reset();
	today.reset();
	rootNode.reset();
	errorCode = 0;
}

void FilterContext::UpdateTimestamp()
{
	now.reset(new Poco::Timestamp());
	Poco::LocalDateTime ldt(*now);
	Poco::LocalDateTime midnight(ldt.year(), ldt.month(), ldt.day(), 0, 0, 0);
	today.reset(new Poco::Timestamp(midnight.timestamp()));
}

bool FilterEngine::Parse(const std::wstring& expression, FilterContext& ctxt)
{
	ctxt.Clear();
	ctxt.UpdateTimestamp();
	std::string expressionStr = ucr::toUTF8(expression);
	FilterLexer lexer(expressionStr);
	void* prs = ParseAlloc(malloc);
	int token;
	int lastError = 0;
	while ((token = lexer.yylex()) != 0)
	{
		if (token < 0)
		{
			lastError = -token;
			break;
		}
		::Parse(prs, token, lexer.yylval, &ctxt);
		lexer.yycursor = lexer.YYCURSOR;
	}
	::Parse(prs, 0, lexer.yylval, &ctxt);
	::ParseFree(prs, free);
	if (lastError != 0)
		ctxt.errorCode = lastError;
	return (ctxt.errorCode == 0);
}

bool FilterEngine::Evaluate(FilterContext& ctxt, const DIFFITEM& di)
{
	const auto result = ctxt.rootNode->Evaluate(di);

	if (const auto boolVal = std::get_if<bool>(&result))
		return *boolVal;

	if (const auto arrayVal = std::get_if<std::unique_ptr<std::vector<ValueType2>>>(&result))
	{
		const auto& vec = *arrayVal->get();
		return std::any_of(vec.begin(), vec.end(), [](const ValueType2& item) {
			const auto boolVal = std::get_if<bool>(&item.value);
			return boolVal && *boolVal;
			});
	}

	return false;
}
