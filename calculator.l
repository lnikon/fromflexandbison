%{
    #include "calculator.tab.h"

    int yylval;
%}

%%
"+"         { return ADD; }
"-"         { return SUB; }
"*"         { return MUL; }
"/"         { return DIV; }
"|"         { return ABS; }
"//".*      /* Ignore comments */
[0-9]+      { yylval = atoi(yytext); return NUMBER; }
\n          { return EOL; }
[ \t]       {  }
.           { printf("Mystery character %c\n", *yytext);  }
%%

int yywrap()
{
    return 1;
}
