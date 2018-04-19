%{
#include "stdio.h"
%}
%%
[/n]                  ;
[0-9]+                printf("Int     : %s/n",yytext);
[0-9]*/.[0-9]+        printf("Float   : %s/n",yytext);
[a-zA-Z][a-zA-Z0-9]*  printf("Var     : %s/n",yytext);
[/+/-/*///%]          printf("Op      : %s/n",yytext);
.                     printf("Unknown : %c/n",yytext[0]);
%%

int main(int argc, char **argv) 
{
    yyin = fopen(argv[1], "r");
    if (!yyin) {
        return 0;
    }
    yylex();
    printf("parse finish......");
    fclose(yyin);
    return 1;
}

