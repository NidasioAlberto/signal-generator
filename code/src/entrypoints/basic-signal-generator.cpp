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
#include <drivers/DMA/DMA.h>
#include <drivers/timer/GeneralPurposeTimer.h>
#include <miosix.h>
#include <util/util.h>

#include <cmath>
#include <thread>

#define PI 3.14159265358979f

using namespace miosix;

DACDriver dac;
DMAStream& stream5 =
    DMADriver::instance().acquireStream(DMAStreamId::DMA1_Str5);
GP32bitTimer tim2(TIM2);

static constexpr size_t SIGNAL_ARRAY_SIZE = 10e3;
static uint16_t buffer[SIGNAL_ARRAY_SIZE] = {0};

static constexpr int TIMER_FREQUENCY = 5e6;

static int counter1 = 0;
static int counter2 = 0;

void loadBuffer() {
    for (size_t i = 0; i < SIGNAL_ARRAY_SIZE; i++) {
        float time = (1.0 / TIMER_FREQUENCY) * i;
        float val = 5 * sin(100e3 * 2 * PI * time) + 5.5;

        // Convert to DAC values
        uint16_t rawVal = (1 - val / 12) * 4095;
        buffer[i] = rawVal < 4096 ? rawVal : 4095;

        // Debug
        // printf("[%.9f] %f -> %d\n", time, val, rawVal);
    }
}

int main() {
    CPUProfiler p;

    // Prepare the buffers
    loadBuffer();

    // Setup DAC
    {
        dac.enableChannel(DACDriver::Channel::CH1);
        dac.disableBuffer(DACDriver::Channel::CH1);
        dac.enableDMA(DACDriver::Channel::CH1);
        dac.enableTrigger(DACDriver::Channel::CH1,
                          DACDriver::TriggerSource::TIM2_TRGO);
    }

    // Setup DMA
    {
        DMATransaction trn{
            .channel = DMATransaction::Channel::CHANNEL7,
            .direction = DMATransaction::Direction::MEM_TO_PER,
            .priority = DMATransaction::Priority::HIGH,
            .srcSize = DMATransaction::DataSize::BITS_16,
            .dstSize = DMATransaction::DataSize::BITS_16,
            .srcAddress = buffer,
            .dstAddress = &(DAC->DHR12R1),
            .numberOfDataItems = SIGNAL_ARRAY_SIZE,
            .srcIncrement = true,
            .circularMode = true,
        };
        stream5.setup(trn);
        stream5.enable();
    }

    // Setup TIM2
    {
        tim2.setMasterMode(TimerUtils::MasterMode::UPDATE);
        tim2.setFrequency(2 * TIMER_FREQUENCY);
        tim2.setAutoReloadRegister(1);
        tim2.enable();
    }

    printf("Started\n");

    while (true) {
        Thread::sleep(1000);
    };
}