%{
#include <stdio.h>
#include <stdlib.h>
#include "ast_defs.h"
#include "calculator.tab.h"
#include "lex.yy.h"
%}

%token <d> NUMBER
%token EOL

%type <a> exp factor term
%%

calclist:    
        |   calclist exp EOL
            {
                printf("=%4.4g\n",eval($2));
                freetree($2);
                printf("> ");
            }
        | calclist EOL { printf("> "); }
        ;

exp: factor
   | exp '+' factor { $$ = newast('+', $1, $3); }
   | exp '-' factor { $$ = newast('-', $1, $3); }
   ;

factor: term
      | factor '*' term { $$ = newast('*', $1, $3); }
      | factor '/' term { $$ = newast('/', $1, $3); } 
      ;

term: NUMBER { $$ = newnum($1); }
    | '|' term { $$ = newast('|', $2, NULL); }
    | '(' exp ')' { $$ = $2; }
    | '-' term { $$ = newast('M', $2, NULL); }
    ;

%% 

int main(int argc, char **argv)
{
    yyparse();
    return 0;
}

void yyerror(char *msg)
{
    fprintf(stderr, "error on line %d of file %s in functions __FUNCTION__\nerror message: %s\n", __LINE__, __FILE__, __FUNCTION__, msg);
}

