#include "pch.h"
#include "FilterEngine.h"
#include "FilterExpression.h"
#include "FilterEngineInternal.h"
#include "DiffContext.h"
#include "DiffItem.h"

YYSTYPE resultFilterExpression;

static ExprNode* ParseFilterExpression(const std::wstring& str, const CDiffContext& ctxt)
{
	FilterLexer lexer(str);
	void* parser = ParseAlloc(malloc);
	int token;

	lexer.yylval.ctxt = &ctxt;
	while ((token = lexer.yylex()) != 0)
	{
		Parse(parser, token, lexer.yylval);
		lexer.yycursor = lexer.YYCURSOR;
	}
	Parse(parser, 0, lexer.yylval);

	ExprNode* rootNode = (ExprNode*)resultFilterExpression.node;

	ParseFree(parser, free);

	return rootNode;
}


FilterEngine::ParseResult FilterEngine::Parse(const std::wstring& expression, const CDiffContext& ctxt)
{
	ParseResult result;
	result.root.reset(ParseFilterExpression(expression, ctxt));
	return result;
}

bool FilterEngine::Evaluate(const std::shared_ptr<ExprNode>& expr, const DIFFITEM& di)
{
	auto result = expr->evaluate(di);
	if (auto boolVal = std::get_if<bool>(&result))
		return *boolVal;
	return false;
}
