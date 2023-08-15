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
#include <math.h>
#include <miosix.h>
#include <parser/parser_types.h>

#include <functional>

#pragma once

class Generator {
public:
    /**
     * @brief Allocates the buffers on the heap.
     *
     * @param totalBuffersSize Total size occupied by the buffers, in bytes.
     */
    Generator(uint16_t totalBuffersSize = 8e3);

    void init();

    /**
     * @brief Set the expression to be generated on the given channel.
     *
     * If the expression is null, the channel will be disabled.
     */
    void setExpression(DACDriver::Channel channel, const Expression *exp);

    void start(DACDriver::Channel channel);

    void stop(DACDriver::Channel channel);

private:
    /**
     * @brief Generate a function for the given expression.
     *
     * @warning The expression must not be null.
     */
    std::function<float(float)> buildFunction(const Expression *exp);

    void generateWave(uint16_t *buff, const std::function<float(float)> &func,
                      float startTime, float interval);

    uint16_t computeDacValue(const std::function<float(float)> &func, float t);

    struct ChannelCtrlData {
        uint16_t *buffer1;
        uint16_t *buffer2;
        std::function<float(float)> func;
        float nextStartTime = 0;
    };

    float benchmarkComputation(ChannelCtrlData &ctrlData);

    uint16_t buffersSize;  // Size of each buffers in number of elements
    ChannelCtrlData channelCtrlData[2];

    static constexpr float waveFrequency = 1000;

    DACDriver dac;
    GP32bitTimer timer2;
    GP32bitTimer timer4;

    // DAC1 on DMA1 Stream 5
    DMAStream &stream5 =
        DMADriver::instance().acquireStream(DMAStreamId::DMA1_Str5);

    // DAC2 on DMA1 Stream 6
    DMAStream &stream6 =
        DMADriver::instance().acquireStream(DMAStreamId::DMA1_Str6);
};
