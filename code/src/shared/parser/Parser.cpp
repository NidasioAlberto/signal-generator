#include "Parser.h"

using namespace miosix;
using namespace std;

void yyerror(Command *command, char *message) { printf("%s\n", message); }

Parser::Parser(std::function<void(const Command &)> onCommand,
               std::function<void()> onError)
    : onCommand(onCommand), onError(onError) {}

void Parser::start() {
    runningThread = thread([](Parser *parser) { parser->run(); }, this);
    runningThread.detach();
}

void Parser::run() {
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

            /**
             * Values of result:
             * 0: Command recognized
             * 1: Abort
             * 2: Memory exhausted
             */
            if (result == 0) {
                // Check if the command is valid
                if (command.type == CommandType::EXPRESSION &&
                    command.exp == nullptr) {
                    printf("ERROR: Expression not valid\n");
                    onError();
                } else if (!checkChannel(command.channel)) {
                    printf(
                        "ERROR: Channel not recognized, only 0 or 1 are valid "
                        "channel numbers\n");
                    onError();
                } else {
                    onCommand(command);
                }
            } else {
                printf("ERROR: Command not recognized\n");
                onError();
            }
        }
    }
}

bool Parser::checkChannel(float value) {
    int channel = static_cast<int>(value);

    return channel == static_cast<int>(DACDriver::Channel::CH1) ||
           channel == static_cast<int>(DACDriver::Channel::CH2);
}