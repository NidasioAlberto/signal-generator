#include <drivers/dac/dac.h>
#include <miosix.h>

using namespace std;
using namespace miosix;

constexpr long long DEMO_DURATION = 5 * 1000;  // [ms]

// Serial pins: PA9 PA10

int main() {
    DACDriver adc;
    long long start;

    adc.enableChannel(1);

    while (true) {
        // Noise generator
        adc.enableNoiseGeneration(1);
        adc.setWaveGeneratorMask(1, 10);
        adc.centerWaveOutput(1, 1);
        start = getTick();
        while (getTick() - start < DEMO_DURATION) {
            adc.dispatchSoftwareTrigger(1);
            Thread::sleep(10);
        }
        adc.disableWaveGenerator(1);

        // Triangular wave generator
        adc.enableTriangularWaveGeneration(1);
        adc.setWaveGeneratorMask(1, 10);
        adc.centerWaveOutput(1, 2);
        start = getTick();
        while (getTick() - start < DEMO_DURATION) {
            adc.dispatchSoftwareTrigger(1);
            delayUs(100);
        }
        adc.disableWaveGenerator(1);
    }
}
