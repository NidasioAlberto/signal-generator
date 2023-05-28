#include <assert.h>

#include <memory>

#include "parser.h"

#pragma once

enum class ExpressionType {
    NONE,
    NUMBER,
    SINUSOIDS,
    TRIANGULAR,
    STEP,
    ADD_OP,
    SUB_OP,
    MUL_OP,
    DIV_OP
};

struct Expression {
    ExpressionType type;
    union {
        float number;
        Expression *args[2];
    } argument;

    Expression() = default;

    Expression(float number) : type(ExpressionType::NUMBER) {
        argument.number = number;
    }

    Expression(Expression *exp1, ExpressionType type) : type(type) {
        argument.args[0] = exp1;
    }

    Expression(Expression *exp1, Expression *exp2, ExpressionType type)
        : type(type) {
        assert(type != ExpressionType::NONE);
        assert(type != ExpressionType::NUMBER);

        argument.args[0] = exp1;
        argument.args[1] = exp2;
    }

    ~Expression() {
        // Delete recursively all subexpressions
        if (type == ExpressionType::SINUSOIDS ||
            type == ExpressionType::TRIANGULAR ||
            type == ExpressionType::STEP) {
            delete argument.args[0];
        } else if (type == ExpressionType::ADD_OP ||
                   type == ExpressionType::SUB_OP ||
                   type == ExpressionType::MUL_OP ||
                   type == ExpressionType::DIV_OP) {
            delete argument.args[0];
            delete argument.args[1];
        }
    }
};

enum class CommandType {
    START,
    STOP,
    MATH_FUNCTION,
};

struct Command {
    CommandType type;
    union {
        Expression *exp;
    } argument;

    ~Command() {
        if (type == CommandType::MATH_FUNCTION) {
            delete argument.exp;
        }
    }
};
