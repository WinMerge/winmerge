%token AND OR NOT TRUE_LITERAL FALSE_LITERAL IDENTIFIER INTEGER_LITERAL STRING_LITERAL EQ NE LT LE GT GE LPAREN RPAREN PLUS MINUS STAR SLASH.

%left OR.
%left AND.
%left EQ NE LT LE GT GE.
%right NOT.
%left STAR SLASH.
%left PLUS MINUS.

%token_type {YYSTYPE}
%extra_argument { FilterParseContext* pCtx }

%include {
#include "FilterEngineInternal.h"
#include "FilterExpression.h"
}

filter_expr ::= expr(A). { pCtx->rootNode = A.node; }

expr(A) ::= expr(B) OR expr(C).   { A.node = new OrNode(B.node, C.node); }
expr(A) ::= expr(B) AND expr(C).  { A.node = new AndNode(B.node, C.node); }
expr(A) ::= NOT expr(B).          { A.node = new NotNode(B.node); }
expr(A) ::= comparison(A).
expr(A) ::= LPAREN expr(B) RPAREN. { A = B; }

comparison(A) ::= arithmetic(B) EQ arithmetic(C). { A.node = new ComparisonNode(B.node, L"==", C.node); }
comparison(A) ::= arithmetic(B) NE arithmetic(C). { A.node = new ComparisonNode(B.node, L"!=", C.node); }
comparison(A) ::= arithmetic(B) LT arithmetic(C). { A.node = new ComparisonNode(B.node, L"<",  C.node); }
comparison(A) ::= arithmetic(B) LE arithmetic(C). { A.node = new ComparisonNode(B.node, L"<=", C.node); }
comparison(A) ::= arithmetic(B) GT arithmetic(C). { A.node = new ComparisonNode(B.node, L">",  C.node); }
comparison(A) ::= arithmetic(B) GE arithmetic(C). { A.node = new ComparisonNode(B.node, L">=", C.node); }

arithmetic(A) ::= arithmetic(B) PLUS arithmetic(C).  { A.node = new ArithmeticNode(B.node, L"+", C.node); }
arithmetic(A) ::= arithmetic(B) MINUS arithmetic(C). { A.node = new ArithmeticNode(B.node, L"-", C.node); }
arithmetic(A) ::= arithmetic(B) STAR arithmetic(C).  { A.node = new ArithmeticNode(B.node, L"*", C.node); }
arithmetic(A) ::= arithmetic(B) SLASH arithmetic(C). { A.node = new ArithmeticNode(B.node, L"/", C.node); }
arithmetic(A) ::= term(A).

term(A) ::= TRUE_LITERAL.       { A.node = new BoolLiteral(true); }
term(A) ::= FALSE_LITERAL.      { A.node = new BoolLiteral(false); }
term(A) ::= INTEGER_LITERAL(B). { A.node = new IntLiteral(B.integer); }
term(A) ::= STRING_LITERAL(B).  { A.node = new StringLiteral(B.string); }
term(A) ::= IDENTIFIER(B).      { A.node = new FieldNode(pCtx->ctxt, B.string); }
term(A) ::= LPAREN arithmetic(B) RPAREN. { A = B; }