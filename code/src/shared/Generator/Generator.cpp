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

#include "Generator.h"

Generator::Generator(const uint16_t totalBuffersSize, const float waveFrequency)
    : channelCtrlData{
          // DAC1 on TIM2 and DMA1 Stream 5
          {TIM2, DMADriver::instance().acquireStream(DMAStreamId::DMA1_Str5),
           DACDriver::TriggerSource::TIM2_TRGO,
           static_cast<size_t>(totalBuffersSize / 2), waveFrequency},
          // DAC2 on TIM4 and DMA1 Stream 6
          {TIM4, DMADriver::instance().acquireStream(DMAStreamId::DMA1_Str6),
           DACDriver::TriggerSource::TIM4_TRGO,
           static_cast<size_t>(totalBuffersSize / 2), waveFrequency},
      } {}

void Generator::init() {
    dac.enableChannel(DACDriver::Channel::CH1);
    dac.enableChannel(DACDriver::Channel::CH2);

    // Disable the buffer otherwise the front end won't work
    dac.disableBuffer(DACDriver::Channel::CH1);
    dac.disableBuffer(DACDriver::Channel::CH2);

    // Bring the output to 0V
    dac.setChannel(DACDriver::Channel::CH1, V_DDA_VOLTAGE);
    dac.setChannel(DACDriver::Channel::CH2, V_DDA_VOLTAGE);
}

void Generator::setExpression(DACDriver::Channel channel,
                              const Expression *exp) {
    if (exp == nullptr) {
        stop(channel);
    } else {
        channelCtrlData[static_cast<int>(channel)].func = buildFunction(exp);
    }
}

bool Generator::start(DACDriver::Channel channel) {
    ChannelCtrlData &ctrlData = channelCtrlData[static_cast<int>(channel)];
    volatile void *dacDstAddress =
        channel == DACDriver::Channel::CH1 ? &(DAC->DHR12R1) : &(DAC->DHR12R2);

    // Check if the channel was already started
    if (ctrlData.thread != nullptr) {
        return false;
    }

    // Check if the function has been set
    if (!ctrlData.func) {
        return false;
    }

    // Make the wave start at time 0
    ctrlData.nextStartTime = 0;

    // Prefill both buffers
    generateWave(ctrlData, ctrlData.buffer1);
    ctrlData.nextStartTime += ctrlData.bufferSizeItems * ctrlData.wavePeriod;
    generateWave(ctrlData, ctrlData.buffer2);
    ctrlData.nextStartTime += ctrlData.bufferSizeItems * ctrlData.wavePeriod;

    // Setup DAC
    dac.enableDMA(channel);
    dac.enableTrigger(channel, ctrlData.triggerSource);

    // Setup DMA
    DMATransaction trn{
        .channel = DMATransaction::Channel::CHANNEL7,
        .direction = DMATransaction::Direction::MEM_TO_PER,
        .srcSize = DMATransaction::DataSize::BITS_16,
        .dstSize = DMATransaction::DataSize::BITS_16,
        .srcAddress = ctrlData.buffer1,
        .dstAddress = dacDstAddress,
        .secondMemoryAddress = ctrlData.buffer2,
        .numberOfDataItems = static_cast<uint16_t>(ctrlData.bufferSizeItems),
        .srcIncrement = true,
        .circularMode = true,
        .doubleBufferMode = true,
        .enableTransferCompleteInterrupt = true,
    };
    ctrlData.stream.setup(trn);
    ctrlData.stream.enable();

    // Prepare the thread
    ctrlData.thread = new std::thread([this, &ctrlData]() {
        while (!ctrlData.shouldStop) {
            // Wait for maximum the double duration of a buffer
            if (!ctrlData.stream.timedWaitForTransferComplete(
                    1e9 * 2 * ctrlData.bufferSizeItems /
                    ctrlData.waveFrequency)) {
                continue;
            }

            // Generate the next buffer
            switch (ctrlData.stream.getCurrentBufferNumber()) {
                case 1: {
                    // Current target is buffer 1 so we refill buffer 2
                    generateWave(ctrlData, ctrlData.buffer2);
                    ctrlData.nextStartTime +=
                        ctrlData.bufferSizeItems * ctrlData.wavePeriod;
                    break;
                }
                case 2: {
                    // Current target is buffer 2 so we refill buffer 1
                    generateWave(ctrlData, ctrlData.buffer1);
                    ctrlData.nextStartTime +=
                        ctrlData.bufferSizeItems * ctrlData.wavePeriod;
                    break;
                }
            }
        }
    });

    ctrlData.timer.setMasterMode(TimerUtils::MasterMode::UPDATE);
    ctrlData.timer.setFrequency(2 * ctrlData.waveFrequency);
    ctrlData.timer.setAutoReloadRegister(1);
    ctrlData.timer.enable();

    return true;
}

bool Generator::stop(DACDriver::Channel channel) {
    ChannelCtrlData &ctrlData = channelCtrlData[static_cast<int>(channel)];

    // If the thread is null the channel has already been stopped
    if (ctrlData.thread != nullptr) {
        // Stop the thread
        ctrlData.shouldStop = true;
        ctrlData.thread->join();
        delete ctrlData.thread;
        ctrlData.thread = nullptr;

        // Stop peripherals
        ctrlData.timer.disable();
        ctrlData.stream.disable();
        dac.disableDMA(channel);
        dac.disableTrigger(channel);

        // Bring the output to 0V
        dac.setChannel(channel, V_DDA_VOLTAGE);

        return true;
    } else {
        return false;
    }
}

bool Generator::isRunning(DACDriver::Channel channel) {
    return channelCtrlData[static_cast<int>(channel)].thread != nullptr;
}

std::function<float(float)> Generator::buildFunction(const Expression *exp) {
    assert(exp != nullptr);

    switch (exp->type) {
        case ExpressionType::NUMBER: {
            float number = exp->argument.number;
            return [=](float) { return number; };
            break;
        }
        case ExpressionType::SINUSOIDS: {
            auto exp1 = buildFunction(exp->argument.args[0]);
            return [=](float t) { return sin(exp1(t) * t); };
            break;
        }
        case ExpressionType::TRIANGULAR: {
            auto exp1 = buildFunction(exp->argument.args[0]);
            return [=](float t) {
                float arg = exp1(t);
                return 4 * abs(arg * t - floorf(arg * t + 0.5)) - 1;
            };
            break;
        }
        case ExpressionType::STEP: {
            auto exp1 = buildFunction(exp->argument.args[0]);
            return [=](float t) { return t >= exp1(t) ? 1 : 0; };
        }
        case ExpressionType::ADD_OP: {
            auto exp1 = buildFunction(exp->argument.args[0]);
            auto exp2 = buildFunction(exp->argument.args[1]);
            return [=](float t) { return exp1(t) + exp2(t); };
        }
        case ExpressionType::SUB_OP: {
            auto exp1 = buildFunction(exp->argument.args[0]);
            auto exp2 = buildFunction(exp->argument.args[1]);
            return [=](float t) { return exp1(t) - exp2(t); };
        }
        case ExpressionType::MUL_OP: {
            auto exp1 = buildFunction(exp->argument.args[0]);
            auto exp2 = buildFunction(exp->argument.args[1]);
            return [=](float t) { return exp1(t) * exp2(t); };
        }
        case ExpressionType::DIV_OP: {
            auto exp1 = buildFunction(exp->argument.args[0]);
            auto exp2 = buildFunction(exp->argument.args[1]);
            return [=](float t) { return exp1(t) / exp2(t); };
        }
        default:  // NONE
            return [](float) { return 0.0; };
    }
}

uint16_t Generator::computeDacValue(const std::function<float(float)> &func,
                                    float t) {
    // Compute the target value
    float val = func(t);

    // Convert the target value into the dac register value
    uint16_t rawVal = (1 - val / 12) * 4095;

    return rawVal < 4096 ? rawVal : 4095;
}

void Generator::generateWave(ChannelCtrlData &ctrlData, uint16_t *buff) {
    for (uint16_t i = 0; i < ctrlData.bufferSizeItems; i++) {
        buff[i] = computeDacValue(
            ctrlData.func, ctrlData.nextStartTime + i * ctrlData.wavePeriod);
    }
}
