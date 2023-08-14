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
#include <drivers/DMA/DMA.h>
#include <drivers/timer/GeneralPurposeTimer.h>
#include <math.h>
#include <miosix.h>

using namespace miosix;

int main() {
    constexpr uint16_t ARRAY_SIZE = 10;
    uint16_t adcBuffer[ARRAY_SIZE] = {0};

    DMAStream &adcStream =
        DMADriver::instance().acquireStream(DMAStreamId::DMA2_Str0);

    DMATransaction trn{
        .channel = DMATransaction::Channel::CHANNEL0,
        .direction = DMATransaction::Direction::PER_TO_MEM,
        .srcSize = DMATransaction::DataSize::BITS_16,
        .dstSize = DMATransaction::DataSize::BITS_16,
        .srcAddress = &(ADC1->DR),
        .dstAddress = adcBuffer,
        .numberOfDataItems = ARRAY_SIZE,
        .dstIncrement = true,
    };
    adcStream.setup(trn);
    adcStream.enable();

    ADCDriver adc(ADC1);
    adc.enableChannel(ADCDriver::Channel::CH1);
    adc.enableChannel(ADCDriver::Channel::CH2);
    adc.loadEnabledChannelsInRegularSequence();
    adc.enableRegularSequenceTrigger(
        ADCDriver::RegularTriggerSource::TIM2_TRGO);
    adc.enableDMA(true);
    adc.enable();

    GP32bitTimer tim(TIM2);
    tim.setMasterMode(TimerUtils::MasterMode::UPDATE);
    tim.setFrequency(2 * 1e3);
    tim.setAutoReloadRegister(5);
    tim.enable();

    adc.enable();

    printf("TIM2 frequency: %d\n", tim.getFrequency());

    for (int i = 0; i < 10; i++)
        printf("DR: %ld, buff[0]: %d, buff[1]: %d, OVR: %d\n", ADC1->DR,
               adcBuffer[0], adcBuffer[1], adc.getOverrunFlag());

    adcStream.waitForTransferComplete();

    printf("Transfer completed, here's the content:\n");
    for (int i = 0; i < ARRAY_SIZE; i++) {
        printf("%d\n", adcBuffer[i]);
    }
}