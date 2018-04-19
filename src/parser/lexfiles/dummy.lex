%{
#include <stdio.h>
%}

%%
abc     { printf("foo__"); }
123     { printf("bar__"); }
%%

void main(int ac, char **av)
{
    if(ac > 1 && (yyin = fopen(av[1], "r")) == NULL) {
        perror(av[1]);
        exit(1);
    }
    do {
        printf("every loop...\n");
        yylex();
    } while(!feof(yyin));
} /* main */
