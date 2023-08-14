/* Copyright (c) 2023 Skyward Experimental Rocketry
 * Author: Alberto Nidasio
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include <miosix.h>

#if not defined(STM32F407xx) && not defined(STM32F429xx) && \
    not defined(STM32F767xx) && not defined(STM32F769xx)
#define INTERNAL_ADC_WITHOUT_CALIBRATION
#endif

/**
 * @brief Driver for the ADC peripheral in STM32 microcontrollers.
 *
 * Allows conversions on multiple channels with per-channel sample time.
 *
 * The driver uses basic ADC features, that is the single conversion mode.
 * A previous version of the driver featured injected and regular channels with
 * also DMA. Since ADC conversion time is very low, the driver has been
 * simplified to provide better usability and simpler implementation.
 *
 * @warning This driver has been tested on f205, f407, f429, f767 and f769
 */
class ADCDriver {
public:
    /**
     * @brief ADC channels enumeration.
     */
    enum class Channel : uint8_t {
        CH0 = 0,
        CH1,
        CH2,
        CH3,
        CH4,
        CH5,
        CH6,
        CH7,
        CH8,
        CH9,
        CH10,
        CH11,
        CH12,
        CH13,
        CH14,
        CH15,
        CH16,
        CH17,
        CH18,
        CH_NUM
    };

    /**
     * @brief Conversion sample time. See reference manual.
     *
     * CYCLES_3 is not exposed because in 12-bit mode the minimum is 15
     */
    enum class SampleTime : uint8_t {
        // CYCLES_3   = 0x0,
        CYCLES_15 = 0x1,
        CYCLES_28 = 0x2,
        CYCLES_56 = 0x3,
        CYCLES_84 = 0x4,
        CYCLES_112 = 0x5,
        CYCLES_144 = 0x6,
        CYCLES_480 = 0x7
    };

    enum class TriggerPolarity : uint32_t {
        RISING_EDGE = 0x1 << ADC_CR2_EXTEN_Pos,
        FALLING_EDGE = 0x2 << ADC_CR2_EXTEN_Pos,
        BOTH_EDGES = 0x3 << ADC_CR2_EXTEN_Pos
    };

    /**
     * @brief Trigger sources for regular channels.
     */
    enum class RegularTriggerSource : uint32_t {
        TIM1_CH1 = 0x0 << ADC_CR2_EXTSEL_Pos,
        TIM1_CH2 = 0x1 << ADC_CR2_EXTSEL_Pos,
        TIM1_CH3 = 0x2 << ADC_CR2_EXTSEL_Pos,
        TIM2_CH2 = 0x3 << ADC_CR2_EXTSEL_Pos,
        TIM2_CH3 = 0x4 << ADC_CR2_EXTSEL_Pos,
        TIM2_CH4 = 0x5 << ADC_CR2_EXTSEL_Pos,
        TIM2_TRGO = 0x6 << ADC_CR2_EXTSEL_Pos,
        TIM3_CH1 = 0x7 << ADC_CR2_EXTSEL_Pos,
        TIM3_TRGO = 0x8 << ADC_CR2_EXTSEL_Pos,
        TIM4_CH4 = 0x9 << ADC_CR2_EXTSEL_Pos,
        TIM5_CH1 = 0xa << ADC_CR2_EXTSEL_Pos,
        TIM5_CH2 = 0xb << ADC_CR2_EXTSEL_Pos,
        TIM5_CH3 = 0xc << ADC_CR2_EXTSEL_Pos,
        TIM8_CH1 = 0xd << ADC_CR2_EXTSEL_Pos,
        TIM8_TRGO = 0xe << ADC_CR2_EXTSEL_Pos,
        EXTI_line_11 = 0xf << ADC_CR2_EXTSEL_Pos,
    };

    /**
     * @brief Resets the ADC configuration and automatically enables the
     * peripheral clock.
     */
    explicit ADCDriver(ADC_TypeDef *adc);

    ~ADCDriver();

    void enable();

    void disable();

    void sample();

    void enableChannel(Channel channel,
                       SampleTime sampleTime = SampleTime::CYCLES_480);

    void disableChannel(Channel channel);

    void enableRegularSequenceTrigger(
        RegularTriggerSource triggerSource,
        TriggerPolarity triggerPolarity = TriggerPolarity::RISING_EDGE);

    void disableRegularSequenceTrigger();

    void enableTemperature(SampleTime sampleTime = SampleTime::CYCLES_480);

    void disableTemperature();

    void enableVbat(SampleTime sampleTime = SampleTime::CYCLES_480);

    void disableVbat();

    float getVoltage(Channel channel);

    float getTemperature();

    float getVbatVoltage();

    /**
     * @brief Loads all enabled channels in the regular sequence.
     *
     * This is useful when using the DMA and you want to sample multiple
     * channels. The channels will be ordered by their number.
     */
    void loadEnabledChannelsInRegularSequence();

    void startRegularSequence();

    /**
     * @brief Enables DMA request generation.
     *
     * When the DMA is enabled, a new DMA request is generated after each
     * conversion of a regular channel. This allows the transfer of the
     * converted data from the ADC_DR register to the destination location
     * selected by the software.
     *
     * If data are lost (overrun), the OVR bit in the ADC_SR register is set and
     * an interrupt is generated (if enabled). DMA transfers are then disabled.
     *
     * If an overrun event occurred, follow this steps:
     * 1. Reconfigure the DMA stream (the destination address and data count)
     * 2. Clear the overrun flag
     * 3. Trigger again the ADC to start a new conversion
     *
     * At the end of a DMA transfer (when the number of items configured in the
     * DMA has been sent), the ADC can continue to generate new requests if the
     * dds option is set.
     *
     * @param dds If true, DMA requests continue to be generated.
     */
    void enableDMA(bool dds = false);

    /**
     * @brief Disables DMA request generation.
     *
     * See enableDMA() for more detail on how the DMA request is generated.
     */
    void disableDMA();

    bool getOverrunFlag();

    void clearOverrunFlag();

private:
    void resetRegisters();

    void setChannelSampleTime(Channel channel, SampleTime sampleTime);

    uint16_t readChannel(Channel channel);

    /**
     * @brief Adds the channel the the regular sequence at the given position.
     */
    bool addRegularChannel(Channel channel, uint8_t position);

    ADC_TypeDef *adc;

    bool channelsEnabled[19] = {0};
    bool tempEnabled = false;
    bool vbatEnabled = false;

    float voltages[16] = {0};
    float temperature = 0;
    float vBat = 0;

#ifndef INTERNAL_ADC_WITHOUT_CALIBRATION
    void loadCalibrationValues();

    // Factory calibration values
    // Read "Temperature sensor characteristics" chapter in the datasheet
    float calPt1Voltage;
    float calPt2Voltage;
    float calSlope;
#endif
};
