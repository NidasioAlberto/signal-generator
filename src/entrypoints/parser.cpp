#include <drivers/DAC/DAC.h>
#include <math.h>
#include <miosix.h>
#include <parser/lexer.h>
#include <parser/parser.h>

using namespace miosix;

void yyerror(Command *command, char *message) {
    printf("Error: %s\n", message);
}

void printExpression(const Expression *exp) {
    assert(exp != nullptr);

    switch (exp->type) {
        case ExpressionType::NUMBER:
            printf("%f", exp->argument.number);
            break;
        case ExpressionType::SINUSOIDS:
            printf("sin(%f)", exp->argument.number);
            break;
        case ExpressionType::TRIANGULAR:
            printf("tri(%f)", exp->argument.number);
            break;
        case ExpressionType::STEP:
            printf("step(%f)", exp->argument.number);
            break;
        case ExpressionType::ADD_OP:
            printExpression(exp->argument.args[0]);
            printf(" + ");
            printExpression(exp->argument.args[1]);
            break;
        case ExpressionType::SUB_OP:
            printExpression(exp->argument.args[0]);
            printf(" - ");
            printExpression(exp->argument.args[1]);
            break;
        case ExpressionType::MUL_OP:
            printExpression(exp->argument.args[0]);
            printf(" * ");
            printExpression(exp->argument.args[1]);
            break;
        case ExpressionType::DIV_OP:
            printExpression(exp->argument.args[0]);
            printf(" / ");
            printExpression(exp->argument.args[1]);
            break;
        default:  // NONE
            break;
    }
}

std::function<float(float)> buildExpression(const Expression *exp) {
    assert(exp != nullptr);

    switch (exp->type) {
        case ExpressionType::NUMBER:
            return [=](float) { return exp->argument.number; };
            break;
        case ExpressionType::SINUSOIDS:
            return [=](float t) {
                return sin(buildExpression(exp->argument.args[0])(t) * t);
            };
            break;
        case ExpressionType::TRIANGULAR:
            return [=](float t) {
                float arg = buildExpression(exp->argument.args[0])(t);
                return 2 * abs(arg * t - floorf(arg * t + 0.5));
            };
            break;
        case ExpressionType::STEP:
            return [=](float t) {
                return t >= buildExpression(exp->argument.args[0])(t) ? 1 : 0;
            };
        case ExpressionType::ADD_OP:
            return [=](float t) {
                return buildExpression(exp->argument.args[0])(t) +
                       buildExpression(exp->argument.args[1])(t);
            };
        case ExpressionType::SUB_OP:
            return [=](float t) {
                return buildExpression(exp->argument.args[0])(t) +
                       buildExpression(exp->argument.args[1])(t);
            };
        case ExpressionType::MUL_OP:
            return [=](float t) {
                return buildExpression(exp->argument.args[0])(t) +
                       buildExpression(exp->argument.args[1])(t);
            };
        case ExpressionType::DIV_OP:
            return [=](float t) {
                return buildExpression(exp->argument.args[0])(t) +
                       buildExpression(exp->argument.args[1])(t);
            };
        default:  // NONE
            return [](float) { return 0.0; };
    }
}

int main() {
    DACDriver dac;
    dac.enableChannel(DACDriver::Channel::CH1);
    dac.disableBuffer(DACDriver::Channel::CH1);
    dac.setChannel(DACDriver::Channel::CH1, V_DDA_VOLTAGE);

    while (true) {
        fflush(stdin);
        char expr[256];

        if (!fgets(expr, sizeof(expr), stdin)) {
            continue;
        }

        YY_BUFFER_STATE state;

        if (!(state = yy_scan_bytes(expr, strcspn(expr, "\n")))) {
            continue;
        }

        Command command;
        auto result = yyparse(&command);

        yy_delete_buffer(state);

        if (result == 0) {
            switch (command.type) {
                case CommandType::START:
                    printf("Start command\n");
                    break;
                case CommandType::STOP:
                    printf("Stop command\n");
                    break;
                case CommandType::MATH_FUNCTION:
                    printf("Math function: ");
                    printExpression(command.argument.exp);
                    printf("\n");

                    auto function = buildExpression(command.argument.exp);

                    // auto start = getTime();
                    // while (getTime() - start < 1 * 1e9) {
                    //     printf("[%.2f] %f\n", (getTime() - start) / 1e9,
                    //            function((getTime() - start) / 1e9));
                    //     Thread::sleep(10);
                    // }

                    auto start = getTime();
                    while (true) {
                        float val = (12 - function((getTime() - start) / 1e9)) *
                                    V_DDA_VOLTAGE / 12;
                        dac.setChannel(DACDriver::Channel::CH1, val);
                        Thread::sleep(10);
                    }

                    break;
            }
        }
    }
}