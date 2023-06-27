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

#include "DAC.h"

#include <interfaces/gpio.h>
#include <kernel/kernel.h>
#include <math.h>

#ifndef V_DDA_VOLTAGE
#error "V_DDA_VOLTAGE not defined. Pease specify the analog supply voltage."
#endif

using namespace miosix;

typedef Gpio<GPIOA_BASE, 4> ch1;
typedef Gpio<GPIOA_BASE, 5> ch2;

void DACDriver::enableChannel(Channel channel) {
    if (channel == Channel::CH1)
        ch1::mode(Mode::INPUT_ANALOG);
    else
        ch2::mode(Mode::INPUT_ANALOG);

    // DAC clock must be enabled
    RCC->APB1ENR |= RCC_APB1ENR_DACEN;

    // Enable the channel
    DAC->CR |= channel == Channel::CH1 ? DAC_CR_EN1 : DAC_CR_EN2;
}

void DACDriver::disableChannel(Channel channel) {
    // Leave the gpio pins in analog configuration to consume less power.
    // Check AN4899 chapter 7

    // Disable the DAC clock if both channels are disabled
    if (!(DAC->CR & (channel == Channel::CH1 ? DAC_CR_EN2 : DAC_CR_EN1)))
        RCC->APB1ENR &= ~RCC_APB1ENR_DACEN;
}

void DACDriver::setChannel(Channel channel, float voltage) {
    if (voltage > V_DDA_VOLTAGE)
        voltage = V_DDA_VOLTAGE;

    if (channel == Channel::CH1)
        DAC->DHR12R1 = static_cast<uint16_t>(0xfff / V_DDA_VOLTAGE * voltage);
    else
        DAC->DHR12R2 = static_cast<uint16_t>(0xfff / V_DDA_VOLTAGE * voltage);
}

void DACDriver::enableBuffer(Channel channel) {
    if (channel == Channel::CH1)
        DAC->CR &= ~DAC_CR_BOFF1;
    else
        DAC->CR &= ~DAC_CR_BOFF2;
}

void DACDriver::disableBuffer(Channel channel) {
    if (channel == Channel::CH1)
        DAC->CR |= DAC_CR_BOFF1;
    else
        DAC->CR |= DAC_CR_BOFF2;
}

void DACDriver::enableTrigger(Channel channel, TriggerSource source) {
    // We need to:
    // - First disable the trigger before changing the TSEL bits
    // - Change the trigger source (TSEL bits)
    // - Enable the trigger source
    if (channel == Channel::CH1) {
        DAC->CR &= ~DAC_CR_TEN1;
        DAC->CR &= ~DAC_CR_TSEL1;
        DAC->CR |= static_cast<uint32_t>(source);
        DAC->CR |= DAC_CR_TEN1;
    } else {
        DAC->CR &= ~DAC_CR_TEN2;
        DAC->CR &= ~DAC_CR_TSEL2;
        DAC->CR |= static_cast<uint32_t>(source) << 16;
        DAC->CR |= DAC_CR_TEN2;
    }
}

void DACDriver::disableTrigger(Channel channel) {
    if (channel == Channel::CH1)
        DAC->CR &= ~DAC_CR_TEN1;
    else
        DAC->CR &= ~DAC_CR_TEN2;
}

void DACDriver::dispatchSoftwareTrigger(Channel channel) {
    // The SWTRIGRx bit will be reset by hardware once the current DHRx value is
    // shifted into DORx
    if (channel == Channel::CH1)
        DAC->SWTRIGR |= DAC_SWTRIGR_SWTRIG1;
    else
        DAC->SWTRIGR |= DAC_SWTRIGR_SWTRIG2;
}

void DACDriver::enableNoiseGeneration(Channel channel, TriggerSource source) {
    // The noise generator requires a trigger source, so we'll enable it
    enableTrigger(channel, source);

    // Enable the noise generator and set the mask to unmask all bits by default
    if (channel == Channel::CH1) {
        DAC->CR &= ~DAC_CR_WAVE1;
        DAC->CR |= DAC_CR_WAVE1_0 | DAC_CR_MAMP1;
    } else {
        DAC->CR &= ~DAC_CR_WAVE2;
        DAC->CR |= DAC_CR_WAVE2_0 | DAC_CR_MAMP2;
    }
}

void DACDriver::enableTriangularWaveGeneration(Channel channel,
                                               TriggerSource source) {
    // The noise generator requires a trigger source, so we'll enable it
    enableTrigger(channel, source);

    // Enable the noise generator and set the mask to full amplitude by default
    if (channel == Channel::CH1) {
        DAC->CR &= ~DAC_CR_WAVE1;
        DAC->CR |= DAC_CR_WAVE1_1 | DAC_CR_MAMP1;
    } else {
        DAC->CR &= ~DAC_CR_WAVE2;
        DAC->CR |= DAC_CR_WAVE2_1 | DAC_CR_MAMP2;
    }
}

void DACDriver::disableWaveGenerator(Channel channel) {
    // Disable the trigger that was enabled
    disableTrigger(channel);

    // Disable the wave generator
    if (channel == Channel::CH1)
        DAC->CR &= ~DAC_CR_WAVE1;
    else
        DAC->CR &= ~DAC_CR_WAVE2;
}

bool DACDriver::setWaveGeneratorMask(Channel channel, uint8_t bits) {
    if (bits < 1 || bits > 12)
        return false;

    if (channel == Channel::CH1) {
        DAC->CR &= ~DAC_CR_MAMP1;    // Reset the value
        DAC->CR |= (bits - 1) << 8;  // Set the new value
    } else {
        DAC->CR &= ~DAC_CR_MAMP2;     // Reset the value
        DAC->CR |= (bits - 1) << 24;  // Set the new value
    }

    return true;
}

void DACDriver::centerWaveOutput(Channel channel, float voltage) {
    uint8_t bits;

    // Fetch the unmasked bits
    if (channel == Channel::CH1)
        bits = ((DAC->CR & DAC_CR_MAMP1) >> 8) + 1;
    else
        bits = ((DAC->CR & DAC_CR_MAMP2) >> 24) + 1;

    // Compute the wave amplitude
    float amplitude = V_DDA_VOLTAGE / 4095.0 * (powf(2, bits) - 1);

    // Set the appropriate voltage
    setChannel(channel, voltage - amplitude / 2);
}

void DACDriver::enableDMA(Channel channel) {
    if (channel == Channel::CH1)
        DAC->CR |= DAC_CR_DMAEN1;
    else
        DAC->CR |= DAC_CR_DMAEN2;
}

void DACDriver::disableDMA(Channel channel) {
    if (channel == Channel::CH1)
        DAC->CR &= ~DAC_CR_DMAEN1;
    else
        DAC->CR &= ~DAC_CR_DMAEN2;
}
