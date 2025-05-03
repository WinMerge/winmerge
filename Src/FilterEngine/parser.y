%token AND OR NOT TRUE_LITERAL FALSE_LITERAL IDENTIFIER INTEGER_LITERAL STRING_LITERAL EQ NE LT LE GT GE LPAREN RPAREN PLUS MINUS.

%left OR.
%left AND.
%left EQ NE LT LE GT GE.
%left NOT.
%left PLUS MINUS.

%token_type {YYSTYPE}

%include {
#include "parser_internal.h"
#include "node.h"
int yylex();
extern YYSTYPE result;
extern const char *yytext;
void yy_delete(void *p);

}

where_clause ::= expr(A). { result = A; }

expr(A) ::= expr(B) OR expr(C).   { A.node = new OrNode(B.node, C.node); }
expr(A) ::= expr(B) AND expr(C).  { A.node = new AndNode(B.node, C.node); }
expr(A) ::= NOT expr(B).          { A.node = new NotNode(B.node); }
expr(A) ::= comparison(A).
expr(A) ::= LPAREN expr(B) RPAREN. { A = B; }

comparison(A) ::= arithmetic(B) EQ arithmetic(C). { A.node = new ComparisonNode(B.node, "==", C.node); }
comparison(A) ::= arithmetic(B) NE arithmetic(C). { A.node = new ComparisonNode(B.node, "!=", C.node); }
comparison(A) ::= arithmetic(B) LT arithmetic(C). { A.node = new ComparisonNode(B.node, "<",  C.node); }
comparison(A) ::= arithmetic(B) LE arithmetic(C). { A.node = new ComparisonNode(B.node, "<=", C.node); }
comparison(A) ::= arithmetic(B) GT arithmetic(C). { A.node = new ComparisonNode(B.node, ">",  C.node); }
comparison(A) ::= arithmetic(B) GE arithmetic(C). { A.node = new ComparisonNode(B.node, ">=", C.node); }

arithmetic(A) ::= arithmetic(B) PLUS arithmetic(C).  { A.node = new ArithmeticNode(B.node, "+", C.node); }
arithmetic(A) ::= arithmetic(B) MINUS arithmetic(C). { A.node = new ArithmeticNode(B.node, "-", C.node); }
arithmetic(A) ::= term(A).

term(A) ::= TRUE_LITERAL.       { A.node = new BoolLiteral(true); }
term(A) ::= FALSE_LITERAL.      { A.node = new BoolLiteral(false); }
term(A) ::= IDENTIFIER.         { A.node = new FieldNode(yytext); }
term(A) ::= INTEGER_LITERAL.    { A.node = new IntLiteral(std::stoi(yytext)); }
term(A) ::= STRING_LITERAL.     { A.node = new StringLiteral(yytext); }
term(A) ::= LPAREN arithmetic(B) RPAREN. { A = B; }