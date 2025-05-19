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
	, rootNode(nullptr)
	, now(nullptr)
	, today(nullptr)
	, errorCode(0)
{
	UpdateTimestamp();
}


FilterContext::~FilterContext()
{
	Clear();
}

void FilterContext::Clear()
{
	delete now;
	delete today;
	delete rootNode;
	now = nullptr;
	today = nullptr;
	rootNode = nullptr;
	errorCode = 0;
}

void FilterContext::UpdateTimestamp()
{
	if (now)
	{
		delete now;
		now = nullptr;
	}
	if (today)
	{
		delete today;
		today = nullptr;
	}
	now = new Poco::Timestamp();
	Poco::LocalDateTime ldt(*now);
	Poco::LocalDateTime midnight(ldt.year(), ldt.month(), ldt.day(), 0, 0, 0);
	today = new Poco::Timestamp(midnight.timestamp());
}

bool FilterEngine::Parse(const std::wstring& expression, FilterContext& ctxt)
{
	ctxt.Clear();
	ctxt.UpdateTimestamp();
	std::string expressionStr = ucr::toUTF8(expression);
	FilterLexer lexer(expressionStr);
	void* prs = ParseAlloc(malloc);
	int token;
	while ((token = lexer.yylex()) != 0)
	{
		if (token < 0)
		{
			ctxt.errorCode = -token;
			break;
		}
		::Parse(prs, token, lexer.yylval, &ctxt);
		lexer.yycursor = lexer.YYCURSOR;
	}
	::Parse(prs, 0, lexer.yylval, &ctxt);
	::ParseFree(prs, free);
	return (ctxt.errorCode == 0);
}

bool FilterEngine::Evaluate(FilterContext& ctxt, const DIFFITEM& di)
{
	auto result = ctxt.rootNode->evaluate(di);
	if (auto boolVal = std::get_if<bool>(&result))
		return *boolVal;
	return false;
}
