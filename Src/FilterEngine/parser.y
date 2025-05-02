%token AND OR NOT TRUE_LITERAL FALSE_LITERAL IDENTIFIER INTEGER_LITERAL STRING_LITERAL EQ NE LT LE GT GE LPAREN RPAREN.

%left OR.
%left AND.
%left EQ NE LT LE GT GE.
%left NOT.

%syntax_error {
  std::cerr << "Syntax Error near \"" << yytext << "\"" << std::endl;
}

where_clause ::= expr(A). { result = A; }

expr(A) ::= expr(B) OR expr(C).   { A = new OrNode(B, C); }
expr(A) ::= expr(B) AND expr(C).  { A = new AndNode(B, C); }
expr(A) ::= NOT expr(B).          { A = new NotNode(B); }
expr(A) ::= comparison(A).
expr(A) ::= LPAREN expr(B) RPAREN. { A = B; }

comparison(A) ::= IDENTIFIER EQ literal(B). { A = new ComparisonNode(yytext, "==", B); }
comparison(A) ::= IDENTIFIER NE literal(B). { A = new ComparisonNode(yytext, "!=", B); }
comparison(A) ::= IDENTIFIER LT literal(B). { A = new ComparisonNode(yytext, "<", B); }
comparison(A) ::= IDENTIFIER LE literal(B). { A = new ComparisonNode(yytext, "<=", B); }
comparison(A) ::= IDENTIFIER GT literal(B). { A = new ComparisonNode(yytext, ">", B); }
comparison(A) ::= IDENTIFIER GE literal(B). { A = new ComparisonNode(yytext, ">=", B); }

literal(A) ::= TRUE_LITERAL.        { A = new BoolLiteral(true); }
literal(A) ::= FALSE_LITERAL.       { A = new BoolLiteral(false); }
literal(A) ::= INTEGER_LITERAL.     { A = new IntLiteral(std::stoi(yytext)); }
literal(A) ::= STRING_LITERAL.      { A = new StringLiteral(yytext); }


%include {
#include "node.h"
int yylex();
extern char *yytext;
void yy_delete(void *p);

%}