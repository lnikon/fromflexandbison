%{
#include <stdio.h>
%}

%token NUMBER
%token ADD SUB MUL DIV ABS
%token EOL
%%

calclist:
        | calclist exp EOL { printf("=%d\n", $2); }
        | calclist EOL { printf("> "); }
        ;

exp: factor
   | exp ADD factor { $$ = $1 + $3; }
   | exp SUB factor { $$ = $1 - $3; }
   ;

factor: term
      | factor MUL term { $$ = $1 * $3; }
      | factor DIV term { $$ = $1 / $3; } 
      ;

term: NUMBER
    | ABS term { $$ = $2 >= 0 ? $2 : -$2; }
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


