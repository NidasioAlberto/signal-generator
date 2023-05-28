/* Bison file */

%{
    #include "parser.h"
    #include <stdio.h>

    extern void yyerror(char *message);
    extern int yylex(void);
%}

%union {
    float numeric_value;
}

%token SIN
%token TRI
%token STEP

%token LPAR RPAR

%token <numeric_value> NUMBER

%%

sin_stm : SIN LPAR NUMBER RPAR {
    printf("Argument of sin function: %f\n", $3);
}
