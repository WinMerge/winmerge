/**
 * @file  FilterParser.y
 *
 * @brief Parser for filter expressions.
 */
%token AND OR NOT TRUE_LITERAL FALSE_LITERAL INTEGER_LITERAL STRING_LITERAL SIZE_LITERAL DATETIME_LITERAL DURATION_LITERAL VERSION_LITERAL IDENTIFIER EQ NE LT LE GT GE CONTAINS RECONTAINS LIKE MATCHES LPAREN RPAREN PLUS MINUS STAR SLASH MOD COMMA.

%left OR.
%left AND.
%left EQ NE LT LE GT GE CONTAINS RECONTAINS LIKE MATCHES.
%right NOT.
%left PLUS MINUS.
%left STAR SLASH MOD.

%token_type {YYSTYPE}
%token_prefix TK_
%extra_argument { FilterExpression* pCtx }

%include {
#include "FilterLexer.h"
#include "FilterExpressionNodes.h"
#include "FilterExpression.h"
#include <Poco/Exception.h>
}

%syntax_error {
	pCtx->errorCode = FILTER_ERROR_SYNTAX_ERROR;
}
%parse_failure {
	pCtx->errorCode = FILTER_ERROR_PARSE_FAILURE;
}
%default_destructor {
	YYSTYPEDestructor($$);
}

filter_expr ::= or_expr(A). {
	if (pCtx->errorCode == 0 && pCtx->optimize)
	{
		try
		{
			pCtx->rootNode.reset(A.node->Optimize());
		}
		catch (Poco::RegularExpressionException& e)
		{
			pCtx->errorCode = FILTER_ERROR_INVALID_REGULAR_EXPRESSION;
			pCtx->rootNode.reset(A.node);
			pCtx->errorMessage = e.message();
		}
		catch (const std::invalid_argument& e)
		{
			pCtx->errorCode = FILTER_ERROR_DIVIDE_BY_ZERO;
			pCtx->rootNode.reset(A.node);
			pCtx->errorMessage = e.what();
		}
		catch (const std::exception&)
		{
			pCtx->rootNode.reset(A.node);
		}
	}
	else
	{
		pCtx->rootNode.reset(A.node);
	}
}

or_expr(A) ::= or_expr(B) OR and_expr(C).             { A = { new OrNode(B.node, C.node) }; }
or_expr(A) ::= and_expr(A).

and_expr(A) ::= and_expr(B) AND not_expr(C).          { A = { new AndNode(B.node, C.node) }; }
and_expr(A) ::= not_expr(A).

not_expr(A) ::= NOT not_expr(B).                      { A = { new NotNode(B.node) }; }
not_expr(A) ::= cmp_expr(A).

cmp_expr(A) ::= arithmetic(B) EQ arithmetic(C).       { A = { new BinaryOpNode(B.node, TK_EQ, C.node) }; }
cmp_expr(A) ::= arithmetic(B) NE arithmetic(C).       { A = { new BinaryOpNode(B.node, TK_NE, C.node) }; }
cmp_expr(A) ::= arithmetic(B) LT arithmetic(C).       { A = { new BinaryOpNode(B.node, TK_LT,  C.node) }; }
cmp_expr(A) ::= arithmetic(B) LE arithmetic(C).       { A = { new BinaryOpNode(B.node, TK_LE, C.node) }; }
cmp_expr(A) ::= arithmetic(B) GT arithmetic(C).       { A = { new BinaryOpNode(B.node, TK_GT,  C.node) }; }
cmp_expr(A) ::= arithmetic(B) GE arithmetic(C).       { A = { new BinaryOpNode(B.node, TK_GE, C.node) }; }
cmp_expr(A) ::= arithmetic(B) CONTAINS arithmetic(C). { A = { new BinaryOpNode(B.node, TK_CONTAINS, C.node) }; }
cmp_expr(A) ::= arithmetic(B) NOT CONTAINS arithmetic(C). { A = { new NotNode(new BinaryOpNode(B.node, TK_CONTAINS, C.node)) }; }
cmp_expr(A) ::= arithmetic(B) RECONTAINS arithmetic(C). { A = { new BinaryOpNode(B.node, TK_RECONTAINS, C.node) }; }
cmp_expr(A) ::= arithmetic(B) NOT RECONTAINS arithmetic(C). { A = { new NotNode(new BinaryOpNode(B.node, TK_RECONTAINS, C.node)) }; }
cmp_expr(A) ::= arithmetic(B) LIKE arithmetic(C).     { A = { new BinaryOpNode(B.node, TK_LIKE, C.node) }; }
cmp_expr(A) ::= arithmetic(B) NOT LIKE arithmetic(C).     { A = { new NotNode(new BinaryOpNode(B.node, TK_LIKE, C.node)) }; }
cmp_expr(A) ::= arithmetic(B) MATCHES  arithmetic(C). { A = { new BinaryOpNode(B.node, TK_MATCHES, C.node) }; }
cmp_expr(A) ::= arithmetic(B) NOT MATCHES  arithmetic(C). { A = { new NotNode(new BinaryOpNode(B.node, TK_MATCHES, C.node)) }; }
cmp_expr(A) ::= arithmetic(A).

arithmetic(A) ::= arithmetic(B) PLUS arithmetic(C).   { A = { new BinaryOpNode(B.node, TK_PLUS, C.node) }; }
arithmetic(A) ::= arithmetic(B) MINUS arithmetic(C).  { A = { new BinaryOpNode(B.node, TK_MINUS, C.node) }; }
arithmetic(A) ::= arithmetic(B) STAR arithmetic(C).   { A = { new BinaryOpNode(B.node, TK_STAR, C.node) }; }
arithmetic(A) ::= arithmetic(B) SLASH arithmetic(C).  { A = { new BinaryOpNode(B.node, TK_SLASH, C.node) }; }
arithmetic(A) ::= arithmetic(B) MOD arithmetic(C).    { A = { new BinaryOpNode(B.node, TK_MOD, C.node) }; }
arithmetic(A) ::= unary(A).

expr(A) ::= or_expr(A).

unary(A) ::= MINUS unary(B).       { A = { new NegateNode(B.node) }; }
unary(A) ::= term(A).

term(A) ::= TRUE_LITERAL.          { A = { new BoolLiteral(true) }; }
term(A) ::= FALSE_LITERAL.         { A = { new BoolLiteral(false) }; }
term(A) ::= INTEGER_LITERAL(B).    { A = { new IntLiteral(B.integer) }; }
term(A) ::= STRING_LITERAL(B).     { A = { new StringLiteral(B.string) }; }
term(A) ::= SIZE_LITERAL(B).       { A = { new SizeLiteral(B.string) }; }
term(A) ::= DATETIME_LITERAL(B).{
  try
  {
    A = {};
    A.node = new DateTimeLiteral(B.string);
  }
  catch (const std::exception&)
  {
    pCtx->errorCode = FILTER_ERROR_INVALID_LITERAL;
  }
}
term(A) ::= DURATION_LITERAL(B).   { A = { new DurationLiteral(B.string) }; }
term(A) ::= VERSION_LITERAL(B).    { A = { new VersionLiteral(B.string) }; }
term(A) ::= IDENTIFIER(B) LPAREN RPAREN. {
  try
  {
    A = {};
    A.node = new FunctionNode(pCtx, B.string, {});
  }
  catch (const std::invalid_argument& e)
  {
    pCtx->errorCode = FILTER_ERROR_INVALID_ARGUMENT_COUNT;
	pCtx->errorMessage = e.what();
  }
  catch (const std::runtime_error& e)
  {
    pCtx->errorCode = FILTER_ERROR_UNDEFINED_IDENTIFIER;
	pCtx->errorMessage = e.what();
  }
}
term(A) ::= IDENTIFIER(B) LPAREN expr_list(C) RPAREN. {
  try
  {
    A = {};
    A.node = new FunctionNode(pCtx, B.string, C.nodeList);
  }
  catch (const std::invalid_argument& e)
  {
    pCtx->errorCode = FILTER_ERROR_INVALID_ARGUMENT_COUNT;
	pCtx->errorMessage = e.what();
    YYSTYPEDestructor(C);
  }
  catch (const std::runtime_error& e)
  {
    pCtx->errorCode = FILTER_ERROR_UNDEFINED_IDENTIFIER;
	pCtx->errorMessage = e.what();
    YYSTYPEDestructor(C);
  }
}
term(A) ::= IDENTIFIER(B). {
  try
  {
    A = {};
    A.node = new FieldNode(pCtx, B.string);
  }
  catch (const std::exception&)
  {
    pCtx->errorCode = FILTER_ERROR_UNDEFINED_IDENTIFIER;
  }
}
term(A) ::= LPAREN expr(B) RPAREN. { A = B; }

expr_list(A) ::= expr(B). {
  A = {};
  A.nodeList = new std::vector<ExprNode*>{ B.node };
}
expr_list(A) ::= expr_list(B) COMMA expr(C). {
  A = {};
  B.nodeList->push_back(C.node);
  A.nodeList = B.nodeList;
}

