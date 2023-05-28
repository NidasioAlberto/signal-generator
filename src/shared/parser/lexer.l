/* Flex file */
%option noyywrap

%top{
    #include "parser.h"
}

DIGIT    [0-9]
ID       [a-zA-Z_][a-zA-Z0-9_]*

%%

"sin"   { return SIN; }
"tri"   { return TRI; }
"step"  { return STEP; }

"("     { return LPAR; }
")"     { return RPAR; }

{DIGIT}+("."{DIGIT}+)?    {
    yylval.numeric_value = atof(yytext);
    return(NUMBER);
}
