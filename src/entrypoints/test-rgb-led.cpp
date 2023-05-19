#include <drivers/spi/SPIDriver.h>
#include <interfaces/endianness.h>
#include <miosix.h>

using namespace miosix;

GpioPin mosi(GPIOB_BASE, 15);

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

void setColor(SPIBus &spi, uint8_t red, uint8_t green, uint8_t blue,
              uint8_t white) {
    uint32_t data[] = {
        computeIntensity(green),
        computeIntensity(red),
        computeIntensity(blue),
        computeIntensity(white),
        0x0,
    };

    for (int i = 0; i < 4; i++) {
        data[i] = data[i] >> 16 | data[i] << 16;
    }

    spi.write16((uint16_t *)data, sizeof(data));
    // spi.write(0x00);
}

int main() {
    printf("Starting...\n");

    mosi.mode(Mode::OUTPUT);
    mosi.low();
    Thread::sleep(100);
    mosi.high();
    Thread::sleep(100);

    mosi.mode(Mode::ALTERNATE);
    mosi.alternateFunction(5);

    SPIBus spi(SPI2);
    SPIBusConfig config;
    config.clockDivider = SPI::ClockDivider::DIV_8;
    spi.configure(config);

    while (true) {
        printf("loop\n");

        for (int i = 0; i < 256; i++) {
            setColor(spi, i, 0x00, 0x00, 0x00);
            Thread::sleep(5);
        }

        for (int i = 0; i < 256; i++) {
            setColor(spi, 0x00, i, 0x00, 0x00);
            Thread::sleep(5);
        }

        for (int i = 0; i < 256; i++) {
            setColor(spi, 0x00, 0x00, i, 0x00);
            Thread::sleep(5);
        }

        for (int i = 0; i < 256; i++) {
            setColor(spi, 0x00, 0x00, 0x00, i);
            Thread::sleep(5);
        }
    }
}