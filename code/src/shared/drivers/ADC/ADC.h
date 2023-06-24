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

#pragma once

#include <miosix.h>

#if not defined(STM32F407xx) && not defined(STM32F429xx) && \
    not defined(STM32F767xx) && not defined(STM32F769xx)
#define INTERNAL_ADC_WITHOUT_CALIBRATION
#endif

/**
 * @brief Driver for stm32 internal ADC
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
    enum SampleTime : uint8_t {
        // CYCLES_3   = 0x0,
        CYCLES_15 = 0x1,
        CYCLES_28 = 0x2,
        CYCLES_56 = 0x3,
        CYCLES_84 = 0x4,
        CYCLES_112 = 0x5,
        CYCLES_144 = 0x6,
        CYCLES_480 = 0x7
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

    void enableChannel(Channel channel, SampleTime sampleTime = CYCLES_480);

    void disableChannel(Channel channel);

    void enableTemperature(SampleTime sampleTime = CYCLES_480);

    void disableTemperature();

    void enableVbat(SampleTime sampleTime = CYCLES_480);

    void disableVbat();

    float getVoltage(Channel channel);

    float getTemperature();

    float getVbatVoltage();

private:
    void resetRegisters();

    void setChannelSampleTime(Channel channel, SampleTime sampleTime);

    uint16_t readChannel(Channel channel);

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
