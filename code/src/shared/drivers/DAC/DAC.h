/***************************************************************************
 *   Copyright (C) 2022 by Alberto Nidasio                                 *
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

#pragma once

#include <interfaces/arch_registers.h>

/**
 * Driver for the DAC peripheral in STM32F4 microcontrollers.
 */
class DACDriver {
public:
    enum class Channel {
        CH1,
        CH2,
    };

    // TODO: Different stm32 families have different trigger sources
    enum class TriggerSource : uint32_t {
        TIM6_TRGO = 0x0 << DAC_CR_TSEL1_Pos,
        TIM8_TRGO = 0x1 << DAC_CR_TSEL1_Pos,
        TIM7_TRGO = 0x2 << DAC_CR_TSEL1_Pos,
        TIM5_TRGO = 0x3 << DAC_CR_TSEL1_Pos,
        TIM2_TRGO = 0x4 << DAC_CR_TSEL1_Pos,
        TIM4_TRGO = 0x5 << DAC_CR_TSEL1_Pos,
        EXTI_line_9 = 0x6 << DAC_CR_TSEL1_Pos,
        SWTRIG = 0x7 << DAC_CR_TSEL1_Pos
    };

    /**
     * @brief Enables one channel of the DAC.
     *
     * @param channel Channel number, must be either 1 or 2.
     * @return True if the channel was enabled.
     */
    void enableChannel(Channel channel);

    /**
     * @brief Disables one channel of the DAC.
     *
     * @param channel Channel number, must be either 1 or 2.
     * @return True if the channel was disabled.
     */
    void disableChannel(Channel channel);

    /**
     * @brief Set the channel output voltage
     *
     * @param voltage On most boards the output is between 0V and 3V
     * @param channel Channel number, must be either 1 or 2.
     * @return True if the output voltage was changed.
     */
    void setChannel(Channel channel, float voltage);

    /**
     * @brief Enables the internal output buffer.
     *
     * @param channel Channel number, must be either 1 or 2.
     * @return True if the output buffer was enabled.
     */
    void enableBuffer(Channel channel);

    /**
     * @brief Disables the internal output buffer.
     *
     * @param channel Channel number, must be either 1 or 2.
     * @return True if the output buffer was disabled.
     */
    void disableBuffer(Channel channel);

    /**
     * @brief Enables the external trigger.
     *
     * This function allows to set and change the trigger source. It first
     * disables the trigger, changes the source and then re-enables it.
     *
     * @param channel Channel number, must be either 1 or 2.
     * @param source Trigger source to be set.
     * @return True if the output buffer was disabled.
     */
    void enableTrigger(Channel channel,
                       TriggerSource source = TriggerSource::SWTRIG);

    /**
     * @brief Disables the external trigger.
     *
     * @param channel Channel number, must be either 1 or 2.
     * @return True if the output buffer was disabled.
     */
    void disableTrigger(Channel channel);

    /**
     * @brief Triggers the software trigger.
     *
     * The new DAC conversion starts one APB clock cycle after the trigger.
     *
     * @param channel Channel number, must be either 1 or 2.
     * @return True if the output buffer was disabled.
     */
    void dispatchSoftwareTrigger(Channel channel);

    /**
     * @brief Enables the noise generator.
     *
     * The noise generator allow to create a variable amplitude pseudo noise.
     * The noise generator requires a trigger source.
     *
     * At each trigger event the wave generator output value is updated and then
     * added to the DHRx register (the normal DAC output). This allows to offset
     * the wave by a specific amount.
     *
     * The wave generator output can also be masked to control the wave
     * amplitude. This allows to control the amplitude in step of 2^-n.
     *
     * @warning If you see the wave with an offset then use setChannel(x, 0) to
     * zero the offset.
     *
     * By default this function enables the software trigger source.
     * @see DACDriver::enableTrigger to change the trigger source.
     * @see DACDriver::dispatchSoftwareTrigger to trigger an event.
     *
     * @param channel Channel number, must be either 1 or 2.
     * @param source Trigger source to be set. Software trigger by default.
     * @return True if the output buffer was disabled.
     */
    void enableNoiseGeneration(Channel channel,
                               TriggerSource source = TriggerSource::SWTRIG);

    /**
     * @brief Enables the triangular wave generator.
     *
     * The triangular wave generator allow to create a variable amplitude wave.
     * The generator requires a trigger source.
     *
     * At each trigger event the wave generator output value is updated and then
     * added to the DHRx register (the normal DAC output). This allows to offset
     * the wave by a specific amount.
     *
     * The wave generator output can also be masked to control the wave
     * amplitude. This allows to control the amplitude in step of 2^-n. @see
     * DACDriver::setWaveGeneratorMask for more informations.
     *
     * @warning If you see the wave with an offset then use setChannel(x, 0) to
     * zero the offset.
     *
     * By default this function enables the software trigger source.
     * @see DACDriver::enableTrigger to change the trigger source.
     * @see DACDriver::dispatchSoftwareTrigger to trigger an event.
     *
     * @param channel Channel number, must be either 1 or 2.
     * @param source Trigger source to be set. Software trigger by default.
     * @return True if the output buffer was disabled.
     */
    void enableTriangularWaveGeneration(
        Channel channel, TriggerSource source = TriggerSource::SWTRIG);

    /**
     * @brief Disables the wave generator.
     *
     * @warning Also disables the trigger source.
     *
     * @param channel Channel number, must be either 1 or 2.
     * @return True if the output buffer was disabled.
     */
    void disableWaveGenerator(Channel channel);

    /**
     * @brief Allows to configure the wave amplitude.
     *
     * The wave generator can be configured to mask specific bits of the
     * generated output. This allows to control the amplitude in step of 2^-n:
     * - bits = 12: All bits are masked -> Amplitude = V_DDA
     * - bits = 11: 11 bits masked -> Amplitude = V_DDA / 2
     * - ...
     * - bits =  1: Only the first bit is masked -> Amplitude = V_DDA / 4096
     *
     * @param channel Channel number, must be either 1 or 2.
     * @param bits Bits to mask, must be between 1 and 12.
     * @return True if the output buffer was disabled.
     */
    bool setWaveGeneratorMask(Channel channel, uint8_t bits);

    /**
     * @brief Utility function that sets the channel voltage to center the wave.
     *
     * Since the wave generator output is added to the channel configured value,
     * this function uses the currently configured wave amplitude to calculate
     * the proper channel output such that the wave is centered on a specific
     * voltage value.
     *
     * @warning If you measure the wave non correctly center, measure your V_DDA
     * actual value.
     *
     * @param channel Channel number, must be either 1 or 2.
     * @param voltage Voltage value to center the wave output.
     * @return True if the output buffer was disabled.
     */
    void centerWaveOutput(Channel channel, float voltage);

    /**
     * @brief Enables DMA on the specified channel.
     *
     * When the DMA is enabled, a new DMA request is generated when an external
     * trigger (but not a software trigger) occurs.
     *
     * @param channel Channel number, must be either 1 or 2.
     * @return True if the output buffer was enabled.
     */
    void enableDMA(Channel channel);

    /**
     * @brief Disables DMA on the specified channel.
     *
     * @param channel Channel number, must be either 1 or 2.
     * @return True if the output buffer was enabled.
     */
    void disableDMA(Channel channel);
};
