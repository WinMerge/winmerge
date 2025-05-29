%token AND OR NOT TRUE_LITERAL FALSE_LITERAL INTEGER_LITERAL STRING_LITERAL SIZE_LITERAL DATETIME_LITERAL DURATION_LITERAL VERSION_LITERAL IDENTIFIER EQ NE LT LE GT GE CONTAINS MATCHES LPAREN RPAREN PLUS MINUS STAR SLASH MOD COMMA.

%left OR.
%left AND.
%left EQ NE LT LE GT GE CONTAINS MATCHES.
%right NOT.
%left PLUS MINUS.
%left STAR SLASH MOD.

%token_type {YYSTYPE}
%extra_argument { FilterContext* pCtx }

%include {
#include "FilterLexer.h"
#include "FilterExpression.h"
#include "FilterEngine.h"
}

%syntax_error {
    pCtx->errorCode = FilterEngine::ERROR_SYNTAX_ERROR;
}

%parse_failure {
    pCtx->errorCode = FilterEngine::ERROR_PARSE_FAILURE;
}

filter_expr ::= or_expr(A). { pCtx->rootNode = A.node; }

or_expr(A) ::= or_expr(B) OR and_expr(C).  { A.node = pCtx->RegisterNode(new OrNode(B.node, C.node)); }
or_expr(A) ::= and_expr(A).

and_expr(A) ::= and_expr(B) AND not_expr(C). { A.node = pCtx->RegisterNode(new AndNode(B.node, C.node)); }
and_expr(A) ::= not_expr(A).

not_expr(A) ::= NOT not_expr(B). { A.node = pCtx->RegisterNode(new NotNode(B.node)); }
not_expr(A) ::= cmp_expr(A).

cmp_expr(A) ::= arithmetic(B) EQ arithmetic(C). { A.node = pCtx->RegisterNode(new BinaryOpNode(B.node, "==", C.node)); }
cmp_expr(A) ::= arithmetic(B) NE arithmetic(C). { A.node = pCtx->RegisterNode(new BinaryOpNode(B.node, "!=", C.node)); }
cmp_expr(A) ::= arithmetic(B) LT arithmetic(C). { A.node = pCtx->RegisterNode(new BinaryOpNode(B.node, "<",  C.node)); }
cmp_expr(A) ::= arithmetic(B) LE arithmetic(C). { A.node = pCtx->RegisterNode(new BinaryOpNode(B.node, "<=", C.node)); }
cmp_expr(A) ::= arithmetic(B) GT arithmetic(C). { A.node = pCtx->RegisterNode(new BinaryOpNode(B.node, ">",  C.node)); }
cmp_expr(A) ::= arithmetic(B) GE arithmetic(C). { A.node = pCtx->RegisterNode(new BinaryOpNode(B.node, ">=", C.node)); }
cmp_expr(A) ::= arithmetic(B) CONTAINS arithmetic(C). { A.node = pCtx->RegisterNode(new BinaryOpNode(B.node, "CONTAINS", C.node)); }
cmp_expr(A) ::= arithmetic(B) MATCHES  arithmetic(C). { A.node = pCtx->RegisterNode(new BinaryOpNode(B.node, "MATCHES", C.node)); }
cmp_expr(A) ::= arithmetic(A).

arithmetic(A) ::= arithmetic(B) PLUS arithmetic(C).  { A.node = pCtx->RegisterNode(new BinaryOpNode(B.node, "+", C.node)); }
arithmetic(A) ::= arithmetic(B) MINUS arithmetic(C). { A.node = pCtx->RegisterNode(new BinaryOpNode(B.node, "-", C.node)); }
arithmetic(A) ::= arithmetic(B) STAR arithmetic(C).  { A.node = pCtx->RegisterNode(new BinaryOpNode(B.node, "*", C.node)); }
arithmetic(A) ::= arithmetic(B) SLASH arithmetic(C). { A.node = pCtx->RegisterNode(new BinaryOpNode(B.node, "/", C.node)); }
arithmetic(A) ::= arithmetic(B) MOD arithmetic(C).   { A.node = pCtx->RegisterNode(new BinaryOpNode(B.node, "%", C.node)); }
arithmetic(A) ::= unary(A).

expr(A) ::= or_expr(A).

unary(A) ::= MINUS unary(B). { A.node = pCtx->RegisterNode(new NegateNode(B.node)); }
unary(A) ::= term(A).

term(A) ::= TRUE_LITERAL.        { A.node = pCtx->RegisterNode(new BoolLiteral(true)); }
term(A) ::= FALSE_LITERAL.       { A.node = pCtx->RegisterNode(new BoolLiteral(false)); }
term(A) ::= INTEGER_LITERAL(B).  { A.node = pCtx->RegisterNode(new IntLiteral(B.integer)); }
term(A) ::= STRING_LITERAL(B).   { A.node = pCtx->RegisterNode(new StringLiteral(B.string)); }
term(A) ::= SIZE_LITERAL(B).     { A.node = pCtx->RegisterNode(new SizeLiteral(B.string)); }
term(A) ::= DATETIME_LITERAL(B). { A.node = pCtx->RegisterNode(new DateTimeLiteral(B.string)); }
term(A) ::= DURATION_LITERAL(B). { A.node = pCtx->RegisterNode(new DurationLiteral(B.string)); }
term(A) ::= VERSION_LITERAL(B).  { A.node = pCtx->RegisterNode(new VersionLiteral(B.string)); }
term(A) ::= IDENTIFIER(B) LPAREN RPAREN. { A.node = pCtx->RegisterNode(new FunctionNode(pCtx, B.string, {})); }
term(A) ::= IDENTIFIER(B) LPAREN expr_list(C) RPAREN. { A.node = pCtx->RegisterNode(new FunctionNode(pCtx, B.string, C.nodeList)); }
term(A) ::= IDENTIFIER(B).      { A.node = pCtx->RegisterNode(new FieldNode(pCtx, B.string)); }
term(A) ::= LPAREN expr(B) RPAREN. { A = B; }

expr_list(A) ::= expr(B). { A.nodeList = new std::vector<ExprNode*>{ B.node }; }
expr_list(A) ::= expr_list(B) COMMA expr(C). { B.nodeList->push_back(C.node); A.nodeList = B.nodeList; }

