%option noyywrap nodefault yylineno
%{
    //#include "ast_defs.h"
    #include "calculator.tab.h"
%}

EXP ([Ee][-+]?[0-9]+)

%%

"+" |
"-" |
"*" |
"/" |
"|" |
"(" |
")" { return yytext[0]; }

[0-9]+"."[0-9]*{EXP} |
"."?[0-9]+{EXP}? { yylval.d = atof(yytext); return NUMBER; }

\n { return EOL; }
"//".*
[ \t] { }
.   {  yyerror("unkown character", *yytext); }

%%
