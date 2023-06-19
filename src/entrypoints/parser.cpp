#include <drivers/DAC/DAC.h>
#include <generator/Generator.h>
#include <math.h>
#include <miosix.h>
#include <parser/lexer.h>
#include <parser/parser.h>

using namespace miosix;

void yyerror(Command *command, char *message) {
    printf("Error: %s\n", message);
}

int main() {
    Generator generator;
    generator.init();

    while (true) {
        fflush(stdin);
        char expr[256];

        fgets(expr, sizeof(expr), stdin);
        auto size = strcspn(expr, "\n");

        if (size > 0) {
            YY_BUFFER_STATE state;

            state = yy_scan_bytes(expr, size);

            Command command;
            int result = yyparse(&command);

            yy_delete_buffer(state);

            if (result == 0) {
                switch (command.type) {
                    case CommandType::START:
                        printf("Starting...\n");
                        generator.start();
                        break;
                    case CommandType::STOP:
                        printf("Stopping...\n");
                        generator.stop();
                        break;
                    case CommandType::EXPRESSION:
                        printf("Expression recognized\n");
                        generator.setExpression(command.exp);
                        break;
                }
            } else {
                printf("Command not recognized\n");
            }
        }
    }
}