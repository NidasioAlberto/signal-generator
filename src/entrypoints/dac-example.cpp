#include <drivers/dac/dac.h>
#include <miosix.h>

using namespace std;
using namespace miosix;

constexpr long long DEMO_DURATION = 5 * 1000;  // [ms]

// Serial pins: PA9 PA10

int main() {
    DACDriver dac;
    long long start;

    dac.enableChannel(1);

    while (true) {
        // Noise generator
        dac.enableNoiseGeneration(1);
        dac.setWaveGeneratorMask(1, 10);
        dac.centerWaveOutput(1, 1);
        start = getTime() / 1e6;
        while (getTime() / 1e6 - start < DEMO_DURATION) {
            dac.dispatchSoftwareTrigger(1);
            Thread::sleep(10);
        }
        dac.disableWaveGenerator(1);

        // Triangular wave generator
        dac.enableTriangularWaveGeneration(1);
        dac.setWaveGeneratorMask(1, 10);
        dac.centerWaveOutput(1, 2);
        start = getTime() / 1e6;
        while (getTime() / 1e6 - start < DEMO_DURATION) {
            dac.dispatchSoftwareTrigger(1);
            delayUs(100);
        }
        dac.disableWaveGenerator(1);
    }
}
