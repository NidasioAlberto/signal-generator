/* Bison file */

%code requires {
#include "parser/parser_types.h"
}

%{
#include "parser.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

extern void yyerror(Command *command, char *message);
extern int yylex(void);
%}

%parse-param {Command *ret}

%union {
    float numeric_value;
    Expression *exp;
}

%token SIN
%token TRI
%token STEP

%token LPAR RPAR

%token ADD_OP SUB_OP MUL_OP DIV_OP

%token ASSIGN

%token PI

%token START STOP HELP CPU

%token <numeric_value> NUMBER
%type <exp> exp

%%

start : NUMBER ASSIGN exp {
            (*ret).type = CommandType::EXPRESSION;
            (*ret).exp = $3;
            (*ret).channel = $1;
        } |
        START NUMBER {
            (*ret).type = CommandType::START;
            (*ret).channel = $2;
        } |
        STOP NUMBER {
            (*ret).type = CommandType::STOP;
            (*ret).channel = $2;
        } |
        HELP {
            (*ret).type = CommandType::HELP;
        } |
        CPU {
            (*ret).type = CommandType::CPU;
        }

exp : NUMBER {
            $$ = new Expression($1);
        } |
        SIN LPAR exp RPAR {
            $$ = new Expression($3, ExpressionType::SINUSOIDS);
        } |
        TRI LPAR exp RPAR {
            $$ = new Expression($3, ExpressionType::TRIANGULAR);
        } |
        STEP LPAR exp RPAR {
            $$ = new Expression($3, ExpressionType::STEP);
        } |
        exp ADD_OP exp {
            $$ = new Expression($1, $3, ExpressionType::ADD_OP);
        } |
        exp SUB_OP exp {
            $$ = new Expression($1, $3, ExpressionType::SUB_OP);
        } |
        exp MUL_OP exp {
            $$ = new Expression($1, $3, ExpressionType::MUL_OP);
        } |
        exp DIV_OP exp {
            $$ = new Expression($1, $3, ExpressionType::DIV_OP);
        } |
        PI {
            $$ = new Expression(M_PI);
        }
