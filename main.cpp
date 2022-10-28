#include <drivers/dac/dac.h>
#include <miosix.h>

using namespace std;
using namespace miosix;

int main() {
    DACDriver adc;

    adc.enableChannel(1);

    float voltage = 0;

    while (true) {
        adc.setChannel(voltage, 1);

        voltage += 0.5;
        if (voltage > 3)
            voltage = 0;

        delayMs(1);
    }
}
