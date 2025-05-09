%token AND OR NOT TRUE_LITERAL FALSE_LITERAL IDENTIFIER INTEGER_LITERAL STRING_LITERAL EQ NE LT LE GT GE LPAREN RPAREN PLUS MINUS STAR SLASH MOD COMMA.

%left OR.
%left AND.
%left EQ NE LT LE GT GE.
%right NOT.
%left PLUS MINUS.
%left STAR SLASH MOD.

%token_type {YYSTYPE}
%extra_argument { FilterParseContext* pCtx }

%include {
#include "FilterEngineInternal.h"
#include "FilterExpression.h"
}

filter_expr ::= or_expr(A). { pCtx->rootNode = A.node; }

or_expr(A) ::= or_expr(B) OR and_expr(C).  { A.node = new OrNode(B.node, C.node); }
or_expr(A) ::= and_expr(A).

and_expr(A) ::= and_expr(B) AND not_expr(C). { A.node = new AndNode(B.node, C.node); }
and_expr(A) ::= not_expr(A).

not_expr(A) ::= NOT not_expr(B). { A.node = new NotNode(B.node); }
not_expr(A) ::= cmp_expr(A).

cmp_expr(A) ::= arithmetic(B) EQ arithmetic(C). { A.node = new ComparisonNode(B.node, L"==", C.node); }
cmp_expr(A) ::= arithmetic(B) NE arithmetic(C). { A.node = new ComparisonNode(B.node, L"!=", C.node); }
cmp_expr(A) ::= arithmetic(B) LT arithmetic(C). { A.node = new ComparisonNode(B.node, L"<",  C.node); }
cmp_expr(A) ::= arithmetic(B) LE arithmetic(C). { A.node = new ComparisonNode(B.node, L"<=", C.node); }
cmp_expr(A) ::= arithmetic(B) GT arithmetic(C). { A.node = new ComparisonNode(B.node, L">",  C.node); }
cmp_expr(A) ::= arithmetic(B) GE arithmetic(C). { A.node = new ComparisonNode(B.node, L">=", C.node); }
cmp_expr(A) ::= arithmetic(A).
cmp_expr(A) ::= LPAREN expr(B) RPAREN. { A = B; }

arithmetic(A) ::= arithmetic(B) PLUS arithmetic(C).  { A.node = new ArithmeticNode(B.node, L"+", C.node); }
arithmetic(A) ::= arithmetic(B) MINUS arithmetic(C). { A.node = new ArithmeticNode(B.node, L"-", C.node); }
arithmetic(A) ::= arithmetic(B) STAR arithmetic(C).  { A.node = new ArithmeticNode(B.node, L"*", C.node); }
arithmetic(A) ::= arithmetic(B) SLASH arithmetic(C). { A.node = new ArithmeticNode(B.node, L"/", C.node); }
arithmetic(A) ::= arithmetic(B) MOD arithmetic(C).   { A.node = new ArithmeticNode(B.node, L"%", C.node); }
arithmetic(A) ::= unary(A).

expr(A) ::= or_expr(A).

unary(A) ::= MINUS unary(B). { A.node = new NegateNode(B.node); }
unary(A) ::= term(A).

term(A) ::= TRUE_LITERAL.       { A.node = new BoolLiteral(true); }
term(A) ::= FALSE_LITERAL.      { A.node = new BoolLiteral(false); }
term(A) ::= INTEGER_LITERAL(B). { A.node = new IntLiteral(B.integer); }
term(A) ::= STRING_LITERAL(B).  { A.node = new StringLiteral(B.string); }
term(A) ::= IDENTIFIER(B) LPAREN RPAREN. { A.node = new FunctionNode(pCtx->ctxt, B.string, {}); }
term(A) ::= IDENTIFIER(B) LPAREN expr_list(C) RPAREN. { A.node = new FunctionNode(pCtx->ctxt, B.string, C.nodeList); }
term(A) ::= IDENTIFIER(B).      { A.node = new FieldNode(pCtx->ctxt, B.string); }
term(A) ::= LPAREN expr(B) RPAREN. { A = B; }

expr_list(A) ::= expr(B). { A.nodeList = new std::vector<ExprNode*>{ B.node }; }
expr_list(A) ::= expr_list(B) COMMA expr(C). { B.nodeList->push_back(C.node); A.nodeList = B.nodeList; }

