#include <drivers/DAC/DAC.h>
#include <miosix.h>

using namespace std;
using namespace miosix;

constexpr long long DEMO_DURATION = 5 * 1000;  // [ms]

typedef Gpio<GPIOA_BASE, 4> ch1;

int main() {
    DACDriver dac;
    long long start;

    dac.enableChannel(DACDriver::Channel::CH1);

    while (true) {
        // Noise generator
        dac.enableNoiseGeneration(DACDriver::Channel::CH1);
        dac.setWaveGeneratorMask(DACDriver::Channel::CH1, 10);
        dac.centerWaveOutput(DACDriver::Channel::CH1, 1);
        start = getTime() / 1e6;
        while (getTime() / 1e6 - start < DEMO_DURATION) {
            dac.dispatchSoftwareTrigger(DACDriver::Channel::CH1);
            Thread::sleep(100);
        }
        dac.disableWaveGenerator(DACDriver::Channel::CH1);

        // Triangular wave generator
        dac.enableTriangularWaveGeneration(DACDriver::Channel::CH1);
        dac.setWaveGeneratorMask(DACDriver::Channel::CH1, 10);
        dac.centerWaveOutput(DACDriver::Channel::CH1, 2);
        start = getTime() / 1e6;
        while (getTime() / 1e6 - start < DEMO_DURATION) {
            dac.dispatchSoftwareTrigger(DACDriver::Channel::CH1);
            delayUs(100);
        }
        dac.disableWaveGenerator(DACDriver::Channel::CH1);
    }
}
