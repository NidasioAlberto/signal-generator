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

#include <RGBLed/RGBLed.h>
#include <generator/Generator.h>
#include <miosix.h>
#include <parser/Parser.h>
#include <util/util.h>

using namespace miosix;

RGBLed::Color errorColor = {64, 0, 0, 0};
RGBLed::Color runningColor = {0, 64, 0, 0};
RGBLed::Color idleColor = {0, 64, 64, 0};

int main() {
    // Start the CPU profiler and print every second the CPU usage
    // CPUProfiler::thread(1e9);

    RGBLed led;
    led.init();

    Generator generator;
    generator.init();

    std::function<void(const Command &)> onCommand =
        [&](const Command &command) {
            // TODO: Check also if the expression set before start
            switch (command.type) {
                case CommandType::START:
                    printf("Starting channel %d...\n",
                           static_cast<int>(command.channel));
                    generator.start(
                        static_cast<DACDriver::Channel>(command.channel));
                    led.setColor(runningColor);
                    break;
                case CommandType::STOP:
                    printf("Stopping channel %d...\n",
                           static_cast<int>(command.channel));
                    generator.stop(
                        static_cast<DACDriver::Channel>(command.channel));
                    led.setColor(idleColor);
                    break;
                case CommandType::EXPRESSION:
                    printf("Loading expression for channel %d\n",
                           static_cast<int>(command.channel));
                    generator.setExpression(
                        static_cast<DACDriver::Channel>(command.channel),
                        command.exp);
                    led.setColor(idleColor);
                    break;
            }
        };

    std::function<void()> onError = [&]() { led.setColor(errorColor); };

    Parser parser(onCommand, onError);
    parser.start();

    while (true)
        Thread::sleep(1000);
}