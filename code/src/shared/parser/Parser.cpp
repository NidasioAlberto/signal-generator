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