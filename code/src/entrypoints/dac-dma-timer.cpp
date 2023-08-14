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
static uint32_t buffer1[SIGNAL_ARRAY_SIZE] = {0};
static uint32_t buffer2[SIGNAL_ARRAY_SIZE] = {0};

static int counter1 = 0;
static int counter2 = 0;

void loadBuffer1() {
    auto last = buffer2[SIGNAL_ARRAY_SIZE - 1] + 1;
    for (size_t i = 0; i < SIGNAL_ARRAY_SIZE; i++)
        buffer1[i] = (last + i) % 4096;

    counter1++;
}

void loadBuffer2() {
    auto start = buffer1[SIGNAL_ARRAY_SIZE - 1] + 1;
    for (size_t i = 0; i < SIGNAL_ARRAY_SIZE; i++)
        buffer2[i] = (start + i) % 4096;

    counter2++;
}

int main() {
    CPUProfiler p;

    loadBuffer1();
    loadBuffer2();

    // Setup DAC
    {
        dac.enableChannel(DACDriver::Channel::CH1);
        dac.enableChannel(DACDriver::Channel::CH2);

        dac.disableBuffer(DACDriver::Channel::CH1);
        dac.disableBuffer(DACDriver::Channel::CH2);

        dac.enableDMA(DACDriver::Channel::CH1);
        dac.enableDMA(DACDriver::Channel::CH2);

        dac.enableTrigger(DACDriver::Channel::CH1,
                          DACDriver::TriggerSource::TIM2_TRGO);
        dac.enableTrigger(DACDriver::Channel::CH2,
                          DACDriver::TriggerSource::TIM2_TRGO);
    }

    // Setup DMA
    {
        DMATransaction trn{
            .channel = DMATransaction::Channel::CHANNEL7,
            .direction = DMATransaction::Direction::MEM_TO_PER,
            .priority = DMATransaction::Priority::HIGH,
            .srcSize = DMATransaction::DataSize::BITS_32,
            .dstSize = DMATransaction::DataSize::BITS_32,
            .srcAddress = buffer1,
            .dstAddress = &(DAC->DHR12R1),
            .secondMemoryAddress = buffer2,
            .numberOfDataItems = SIGNAL_ARRAY_SIZE - 1,
            .srcIncrement = true,
            .circularMode = true,
            .doubleBufferMode = true,
            .enableTransferCompleteInterrupt = true,
        };
        stream5.setup(trn);

        stream5.setTransferCompleteCallback([]() {
            if (!(DMA1_Stream5->CR & DMA_SxCR_CT)) {
                // Current target is buffer 1 so we refill buffer 2
                loadBuffer2();
            } else {
                // Current target is buffer 2 so we refill buffer 1
                loadBuffer1();
            }
        });

        stream5.enable();
    }

    // Setup TIM2
    {
        tim2.setMasterMode(TimerUtils::MasterMode::UPDATE);
        tim2.setFrequency(2 * 1e6);
        tim2.setAutoReloadRegister(1);
        tim2.enable();
    }

    while (true) {
        p.update();
        p.print();
        printf("Counts: %d %d\n", counter1, counter2);
        printf("Free heap: %d\n", MemoryProfiling::getCurrentFreeHeap());
        printf("Free stack: %d\n", MemoryProfiling::getCurrentFreeStack());
        Thread::sleep(1000);
    };
}