#include <drivers/spi/SPIDriver.h>
#include <interfaces/gpio.h>
#include <miosix.h>

#pragma once

class RGBLed {
public:
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

    void setColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t white) {
        uint32_t data[] = {
            computeIntensity(green),
            computeIntensity(red),
            computeIntensity(blue),
            computeIntensity(white),
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
