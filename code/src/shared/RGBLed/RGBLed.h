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

#include <drivers/spi/SPIDriver.h>
#include <interfaces/gpio.h>
#include <miosix.h>

#pragma once

class RGBLed {
public:
    struct Color {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
        uint8_t white;
    };

    RGBLed()
        : spi(SPI2, DMAStreamId::DMA1_Str4, DMATransaction::Channel::CHANNEL0,
              DMAStreamId::DMA1_Str3, DMATransaction::Channel::CHANNEL0),
          mosi(GPIOB_BASE, 15) {}

    void init() {
        mosi.mode(miosix::Mode::ALTERNATE);
        mosi.alternateFunction(5);

        config.clockDivider = SPI::ClockDivider::DIV_8;
        spi.configure(config);
    }

    void setColor(Color color) {
        uint32_t data[] = {
            computeIntensity(color.green),
            computeIntensity(color.red),
            computeIntensity(color.blue),
            computeIntensity(color.white),
            0x0,
        };

        for (int i = 0; i < 4; i++) {
            data[i] =
                (data[i] & 0xff000000) >> 24 | (data[i] & 0x00ff0000) >> 8 |
                (data[i] & 0x0000ff00) << 8 | (data[i] & 0x000000ff) << 24;
        }

        // for (int i = 0; i < 4; i++) {
        //     printf("%08lx\n", data[i]);
        // }

        spi.write((uint8_t *)data, sizeof(data));
    }

private:
    uint32_t computeIntensity(uint8_t intensity) {
        uint32_t value = 0;

        for (uint8_t i = 0; i < 8; i++) {
            if (intensity & (0x1 << i))
                value |= 0b1110 << (i * 4);
            else
                value |= 0b1000 << (i * 4);
        }

        return value;
    }

    SPIDMABus spi;
    SPIBusConfig config;
    miosix::GpioPin mosi;
};
