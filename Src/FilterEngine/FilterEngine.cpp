#include "pch.h"
#include "FilterEngine.h"
#include "FilterExpression.h"
#include "FilterEngineInternal.h"
#include "DiffContext.h"
#include "DiffItem.h"
#include <Poco/LocalDateTime.h>

YYSTYPE resultFilterExpression;

FilterContext::FilterContext(const CDiffContext* ctxt)
	: ctxt(ctxt)
	, rootNode(nullptr)
	, now(nullptr)
	, today(nullptr)
{
	UpdateTimestamp();
}

FilterContext::~FilterContext()
{
	delete now;
	delete today;
	delete rootNode;
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

void FilterEngine::Parse(const std::wstring& expression, FilterContext& ctxt)
{
	delete ctxt.rootNode;
	ctxt.UpdateTimestamp();

	FilterLexer lexer(expression);
	void* prs = ParseAlloc(malloc);
	int token;

	while ((token = lexer.yylex()) != 0)
	{
		::Parse(prs, token, lexer.yylval, &ctxt);
		lexer.yycursor = lexer.YYCURSOR;
	}
	::Parse(prs, 0, lexer.yylval, &ctxt);

	::ParseFree(prs, free);
}

bool FilterEngine::Evaluate(FilterContext& ctxt, const DIFFITEM& di)
{
	auto result = ctxt.rootNode->evaluate(di);
	if (auto boolVal = std::get_if<bool>(&result))
		return *boolVal;
	return false;
}
