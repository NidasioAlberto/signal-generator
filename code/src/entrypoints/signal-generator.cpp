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

#include <Generator/Generator.h>
#include <Parser/Parser.h>
#include <RGBLed/RGBLed.h>
#include <miosix.h>
#include <util/util.h>

using namespace miosix;

RGB errorColor(64, 0, 0);
RGB runningColor(0, 64, 0);
RGB idleColor(0, 64, 64);

CPUProfiler profiler;

void printWelcomeMessage();
void printHelpMessage();

int main() {
    RGBLed led;
    led.init();
    led.setColor(idleColor);

    Generator generator(8e3, 10e3);
    generator.init();

    printWelcomeMessage();

    std::function<void(const Command &)> onCommand =
        [&](const Command &command) {
            DACDriver::Channel channel =
                static_cast<DACDriver::Channel>(command.channel);

            switch (command.type) {
                case CommandType::START:
                    if (generator.start(channel)) {
                        printf("Started channel %d\n", command.channel);
                        led.setColor(runningColor);
                    } else {
                        printf("Could not start channel %d\n", command.channel);
                    }
                    break;
                case CommandType::STOP:
                    if (generator.stop(channel)) {
                        printf("Stopped channel %d\n", command.channel);
                        led.setColor(idleColor);
                    } else {
                        printf("Could not stop channel %d\n", command.channel);
                    }
                    break;
                case CommandType::EXPRESSION:
                    if (generator.isRunning(channel)) {
                        printf(
                            "The channel is on, I will first stop and then "
                            "restart it\n");
                        generator.stop(channel);
                        generator.setExpression(channel, command.exp);
                        generator.start(channel);
                    } else {
                        generator.setExpression(channel, command.exp);
                    }
                    printf("Channel %d updated with new expression\n",
                           command.channel);
                    break;
                case CommandType::HELP:
                    printHelpMessage();
                    break;
                case CommandType::CPU:
                    profiler.print();
                    break;
            }
        };

    std::function<void()> onError = [&]() { led.setColor(errorColor); };

    Parser parser(onCommand, onError);
    parser.start();

    while (true) {
        profiler.update();
        Thread::sleep(1000);
    }
}

void printWelcomeMessage() {
    printf(
        "  ____  _                   _    ____                           _     "
        "        \n");
    printf(
        " / ___|(_) __ _ _ __   __ _| |  / ___| ___ _ __   ___ _ __ __ _| |_ "
        "___  _ __ \n");
    printf(
        " \\___ \\| |/ _` | '_ \\ / _` | | | |  _ / _ \\ '_ \\ / _ \\ '__/ _` "
        "| __/ _ \\| '__|\n");
    printf(
        "  ___) | | (_| | | | | (_| | | | |_| |  __/ | | |  __/ | | (_| | || "
        "(_) | |   \n");
    printf(
        " |____/|_|\\__, |_| |_|\\__,_|_|  \\____|\\___|_| |_|\\___|_|  "
        "\\__,_|\\__\\___/|_|   \n");
    printf(
        "          |___/                                                       "
        "        \n");
}

void printHelpMessage() {
    printf(
        "Type \"<channel number> = <expression>\" to set an expression for a "
        "channel\n");
    printf(
        "\tAn expression can be composed by operations (+, -, *, /), between "
        "numbers or functions (sin, tri, step)\n");
    printf("\tsin is a classic sinusoid\n");
    printf("\ttri is a triangle wave\n");
    printf("\tstep is a step function that starts at the given time\n");
    printf(
        "Type \"<start|stop> <channel number>\" to start or stop a channel\n");
    printf("Type \"help\" to show this message\n");
}
