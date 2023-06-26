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
#include <drivers/spi/SPIDriver.h>
#include <interfaces/endianness.h>
#include <miosix.h>
#include <util/util.h>

#include <thread>

using namespace miosix;

int main() {
    std::thread profThread(CPUProfiler::thread, 1e9);

    RGBLed led;
    led.init();

    while (true) {
        for (int i = 0; i < 256; i++) {
            led.setColor({i, 0, 0, 0});
            Thread::sleep(5);
        }

        for (int i = 0; i < 256; i++) {
            led.setColor({0x00, i, 0x00, 0x00});
            Thread::sleep(5);
        }

        for (int i = 0; i < 256; i++) {
            led.setColor({0x00, 0x00, i, 0x00});
            Thread::sleep(5);
        }

        for (int i = 0; i < 256; i++) {
            led.setColor({0x00, 0x00, 0x00, i});
            Thread::sleep(5);
        }
    }
}