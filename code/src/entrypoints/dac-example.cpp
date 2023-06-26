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
