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
    dac.disableBuffer(DACDriver::Channel::CH1);
    dac.disableBuffer(DACDriver::Channel::CH2);

    thread signalGen([&]() {
        while (true) {
            float sinVal =
                1.65 + 1.65 * sin(2 * 3.14 * (getTime() / 1e9) * 0.05);
            float cosVal =
                1.65 + 1.65 * cos(2 * 3.14 * (getTime() / 1e9) * 0.05);

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
