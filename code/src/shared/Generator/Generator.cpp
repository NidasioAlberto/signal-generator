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

Generator::Generator(uint16_t totalBuffersSize)
    : buffersSize(totalBuffersSize / 4 / 2), timer2(TIM2), timer4(TIM4) {
    // Allocate the buffers
    channelCtrlData[0].buffer1 =
        static_cast<uint16_t *>(malloc(totalBuffersSize / 4));
    channelCtrlData[0].buffer2 =
        static_cast<uint16_t *>(malloc(totalBuffersSize / 4));
    channelCtrlData[1].buffer1 =
        static_cast<uint16_t *>(malloc(totalBuffersSize / 4));
    channelCtrlData[1].buffer2 =
        static_cast<uint16_t *>(malloc(totalBuffersSize / 4));
}

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

void Generator::start(DACDriver::Channel channel) {
    ChannelCtrlData &ctrlData = channelCtrlData[static_cast<int>(channel)];
    DMAStream &stream = channel == DACDriver::Channel::CH1 ? stream5 : stream6;
    GP32bitTimer &timer = channel == DACDriver::Channel::CH1 ? timer2 : timer4;
    volatile void *dacDstAddress =
        channel == DACDriver::Channel::CH1 ? &(DAC->DHR12R1) : &(DAC->DHR12R2);

    // Benchmark the function to check maximum freq for wave generation
    auto maxFreq = benchmarkComputation(ctrlData);
    printf("Maximum frequency: %f\n", static_cast<float>(maxFreq));

    // Make the wave start at time 0
    ctrlData.nextStartTime = fixed{0};

    // Prefill both buffers
    generateWave(ctrlData.buffer1, ctrlData.func, fixed{0}, 1 / waveFrequency);
    generateWave(ctrlData.buffer2, ctrlData.func, buffersSize / waveFrequency,
                 1 / waveFrequency);
    ctrlData.nextStartTime = 2 * buffersSize / waveFrequency;

    // Setup DAC
    dac.enableDMA(channel);
    dac.enableTrigger(channel, channel == DACDriver::Channel::CH1
                                   ? DACDriver::TriggerSource::TIM2_TRGO
                                   : DACDriver::TriggerSource::TIM4_TRGO);

    // Setup DMA
    DMATransaction trn{
        .channel = DMATransaction::Channel::CHANNEL7,
        .direction = DMATransaction::Direction::MEM_TO_PER,
        .srcSize = DMATransaction::DataSize::BITS_16,
        .dstSize = DMATransaction::DataSize::BITS_16,
        .srcAddress = ctrlData.buffer1,
        .dstAddress = dacDstAddress,
        .secondMemoryAddress = ctrlData.buffer2,
        .numberOfDataItems = buffersSize,
        .srcIncrement = true,
        .circularMode = true,
        .doubleBufferMode = true,
        .enableTransferCompleteInterrupt = true,
    };
    stream.setup(trn);
    stream.setTransferCompleteCallback([&ctrlData, this]() {
        if (!(DMA1_Stream5->CR & DMA_SxCR_CT)) {
            // Current target is buffer 1 so we refill buffer 2
            generateWave(ctrlData.buffer2, ctrlData.func,
                         ctrlData.nextStartTime, 1 / waveFrequency);
            ctrlData.nextStartTime += buffersSize / waveFrequency;
        } else {
            // Current target is buffer 2 so we refill buffer 1
            generateWave(ctrlData.buffer1, ctrlData.func,
                         ctrlData.nextStartTime, 1 / waveFrequency);
            ctrlData.nextStartTime += buffersSize / waveFrequency;
        }
    });
    stream.enable();

    timer.setMasterMode(TimerUtils::MasterMode::UPDATE);
    timer.setFrequency(static_cast<int>(2 * waveFrequency));
    timer.setAutoReloadRegister(1);
    timer.enable();
}

void Generator::stop(DACDriver::Channel channel) {
    DMAStream &stream = channel == DACDriver::Channel::CH1 ? stream5 : stream6;
    GP32bitTimer &timer = channel == DACDriver::Channel::CH1 ? timer2 : timer4;

    // Stop peripherals
    timer.disable();
    stream.disable();
    dac.disableDMA(channel);
    dac.disableTrigger(channel);

    // Bring the output to 0V
    dac.setChannel(channel, V_DDA_VOLTAGE);
}

std::function<fixed(fixed)> Generator::buildFunction(const Expression *exp) {
    assert(exp != nullptr);

    switch (exp->type) {
        case ExpressionType::NUMBER: {
            fixed number{exp->argument.number};
            return [=](fixed) { return number; };
            break;
        }
        case ExpressionType::SINUSOIDS: {
            auto exp1 = buildFunction(exp->argument.args[0]);
            return [=](fixed t) { return sin(exp1(t) * t); };
            break;
        }
        case ExpressionType::TRIANGULAR: {
            auto exp1 = buildFunction(exp->argument.args[0]);
            return [=](fixed t) {
                fixed arg = exp1(t);
                return 4 * abs(arg * t - fpm::floor(arg * t + fixed{0.5})) - 1;
            };
            break;
        }
        case ExpressionType::STEP: {
            auto exp1 = buildFunction(exp->argument.args[0]);
            return [=](fixed t) { return t >= exp1(t) ? fixed{1} : fixed{0}; };
        }
        case ExpressionType::ADD_OP: {
            auto exp1 = buildFunction(exp->argument.args[0]);
            auto exp2 = buildFunction(exp->argument.args[1]);
            return [=](fixed t) { return exp1(t) + exp2(t); };
        }
        case ExpressionType::SUB_OP: {
            auto exp1 = buildFunction(exp->argument.args[0]);
            auto exp2 = buildFunction(exp->argument.args[1]);
            return [=](fixed t) { return exp1(t) - exp2(t); };
        }
        case ExpressionType::MUL_OP: {
            auto exp1 = buildFunction(exp->argument.args[0]);
            auto exp2 = buildFunction(exp->argument.args[1]);
            return [=](fixed t) { return exp1(t) * exp2(t); };
        }
        case ExpressionType::DIV_OP: {
            auto exp1 = buildFunction(exp->argument.args[0]);
            auto exp2 = buildFunction(exp->argument.args[1]);
            return [=](fixed t) { return exp1(t) / exp2(t); };
        }
        default:  // NONE
            return [](fixed) { return fixed{0.0}; };
    }
}

void Generator::generateWave(uint16_t *buff,
                             const std::function<fixed(fixed)> &func,
                             fixed startTime, fixed interval) {
    for (uint16_t i = 0; i < buffersSize; i++) {
        buff[i] = computeDacValue(func, startTime + i * interval);
    }
}

uint16_t Generator::computeDacValue(const std::function<fixed(fixed)> &func,
                                    fixed t) {
    // Compute the target value
    auto val = func(t);

    // Convert the target value into the dac register value
    uint16_t rawVal = static_cast<uint16_t>((1 - val / 12) * 4095);

    return rawVal < 4096 ? rawVal : 4095;
}

fixed Generator::benchmarkComputation(ChannelCtrlData &ctrlData) {
    // Measure the execution time of the function
    auto start = miosix::getTime();
    generateWave(ctrlData.buffer1, ctrlData.func, fixed{0}, 1 / waveFrequency);
    auto stop = miosix::getTime();

    // Compute the maximum possible frequency
    return fixed{1e9f} / ((stop - start) / buffersSize);
}