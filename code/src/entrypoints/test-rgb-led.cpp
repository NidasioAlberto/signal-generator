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