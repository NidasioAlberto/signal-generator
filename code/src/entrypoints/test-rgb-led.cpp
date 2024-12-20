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
#include <miosix.h>

using namespace miosix;

int main() {
    RGBLed led;
    led.init();

    while (true) {
        printf("Red swipe\n");
        for (uint8_t i = 0; i < 255; i++) {
            led.setColor(RGB(i, 0, 0));
            Thread::sleep(20);
        }

        printf("Green swipe\n");
        for (uint8_t i = 0; i < 255; i++) {
            led.setColor(RGB(0, i, 0));
            Thread::sleep(20);
        }

        printf("Blue swipe\n");
        for (uint8_t i = 0; i < 255; i++) {
            led.setColor(RGB(0, 0, i));
            Thread::sleep(20);
        }

        printf("White swipe\n");
        for (uint8_t i = 0; i < 255; i++) {
            led.setColor(RGB(0, 0, 0, i));
            Thread::sleep(20);
        }

        printf("Hue swipe\n");
        for (uint16_t i = 0; i < 360; i++) {
            led.setColor(HSV(i, 1, 0.1));
            Thread::sleep(10);
        }
    }
}