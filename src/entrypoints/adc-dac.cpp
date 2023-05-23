#include <drivers/ADC/ADC.h>
#include <drivers/DAC/DAC.h>
#include <math.h>
#include <miosix.h>

#include <thread>

using namespace std;
using namespace miosix;

typedef Gpio<GPIOA_BASE, 1> ch1Adc;
typedef Gpio<GPIOA_BASE, 2> ch2Adc;
typedef Gpio<GPIOA_BASE, 4> ch1Dac;
typedef Gpio<GPIOA_BASE, 5> ch2Dac;

int main() {
    ch1Adc::mode(Mode::INPUT_ANALOG);
    ch2Adc::mode(Mode::INPUT_ANALOG);
    ADCDriver adc(ADC1);
    adc.enableChannel(ADCDriver::Channel::CH1);
    adc.enableChannel(ADCDriver::Channel::CH2);
    adc.enableTemperature();
    adc.enable();

    DACDriver dac;
    dac.enableChannel(DACDriver::Channel::CH1);
    dac.enableChannel(DACDriver::Channel::CH2);

    thread signalGen([&]() {
        while (true) {
            float sinVal =
                1.65 + 1.65 * sin(2 * 3.14 * (getTime() / 1e9) * 0.01);
            float cosVal =
                1.65 + 1.65 * cos(2 * 3.14 * (getTime() / 1e9) * 0.01);

            dac.setChannel(DACDriver::Channel::CH1, sinVal);
            dac.setChannel(DACDriver::Channel::CH2, cosVal);

            printf("Val: %5.3f %5.3f\n", sinVal, cosVal);

            Thread::sleep(100);
        }
    });
    signalGen.detach();

    while (true) {
        adc.sample();
        float voltageCh1 = adc.getVoltage(ADCDriver::Channel::CH1);
        float voltageCh2 = adc.getVoltage(ADCDriver::Channel::CH2);
        float temperature = adc.getTemperature();

        printf("%5.3f %5.3f %4.1f\n", voltageCh1, voltageCh2, temperature);
        Thread::sleep(100);
    }
}
