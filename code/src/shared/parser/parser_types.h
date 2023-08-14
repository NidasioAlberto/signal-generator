/***************************************************************************
 *   Copyright (C) 2023 by Alberto Nidasio                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   As a special exception, if other files instantiate templates or use   *
 *   macros or inline functions from this file, or you compile this file   *
 *   and link it with other works to produce a work based on this file,    *
 *   this file does not by itself cause the resulting work to be covered   *
 *   by the GNU General Public License. However the source code for this   *
 *   file must still be made available in accordance with the GNU General  *
 *   Public License. This exception does not invalidate any other reasons  *
 *   why a work based on this file might be covered by the GNU General     *
 *   Public License.                                                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>   *
 ***************************************************************************/

#include <assert.h>

#include <memory>

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
    EXPRESSION,
    HELP,
};

struct Command {
    CommandType type;
    Expression *exp = nullptr;
    int channel = -1;

    ~Command() {
        if (type == CommandType::EXPRESSION) {
            delete exp;
        }
    }
};
