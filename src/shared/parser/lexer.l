/* Flex file */
%option noyywrap
%option caseless

%top{
    #include "parser.h"
}

%%

"sin"   { return SIN; }
"tri"   { return TRI; }
"step"  { return STEP; }

"("     { return LPAR; }
")"     { return RPAR; }

"+"     { return ADD_OP; }
"-"     { return SUB_OP; }
"*"     { return MUL_OP; }
"/"     { return DIV_OP; }

"="     { return ASSIGN; }

"pi"    { return PI; }

"start" { return START; }
"stop"  { return STOP; }

[0-9]+("."[0-9]+)? {
    yylval.numeric_value = atof(yytext);
    return(NUMBER);
}
