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
    Generator(uint16_t totalBuffersSize = 8e3)
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

    void init() {
        dac.enableChannel(DACDriver::Channel::CH1);
        dac.enableChannel(DACDriver::Channel::CH2);

        dac.disableBuffer(DACDriver::Channel::CH1);
        dac.disableBuffer(DACDriver::Channel::CH2);

        dac.setChannel(DACDriver::Channel::CH1, V_DDA_VOLTAGE);
        dac.setChannel(DACDriver::Channel::CH2, V_DDA_VOLTAGE);
    }

    /**
     * @brief Set the expression to be generated on the given channel.
     *
     * If the expression is null, the channel will be disabled.
     */
    void setExpression(DACDriver::Channel channel, const Expression *exp) {
        if (exp == nullptr) {
            stop(channel);
        } else {
            channelCtrlData[static_cast<int>(channel)].func =
                buildFunction(exp);
        }
    }

    void start(DACDriver::Channel channel) {
        ChannelCtrlData &ctrlData = channelCtrlData[static_cast<int>(channel)];
        DMAStream &stream =
            channel == DACDriver::Channel::CH1 ? stream5 : stream6;
        GP32bitTimer &timer =
            channel == DACDriver::Channel::CH1 ? timer2 : timer4;
        volatile void *dacDstAddress = channel == DACDriver::Channel::CH1
                                           ? &(DAC->DHR12R1)
                                           : &(DAC->DHR12R2);

        // TODO: Benchmark the computation of the expression to check
        // maximum freq for wave generation
        // float maxFreq = benchmarkComputation();
        // printf("Maximum frequency: %f\n", maxFreq);

        // Make the wave start at time 0
        ctrlData.nextStartTime = 0;

        // Fill both buffers
        generateWave(ctrlData.buffer1, ctrlData.func, 0, 1 / waveFrequency);
        generateWave(ctrlData.buffer2, ctrlData.func,
                     buffersSize / waveFrequency, 1 / waveFrequency);
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
            .sourceIncrement = true,
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
        timer.setFrequency(2 * waveFrequency);
        timer.setAutoReloadRegister(1);
        timer.enable();
    }

    void stop(DACDriver::Channel channel) {
        DMAStream &stream =
            channel == DACDriver::Channel::CH1 ? stream5 : stream6;
        GP32bitTimer &timer =
            channel == DACDriver::Channel::CH1 ? timer2 : timer4;

        // Stop peripherals
        timer.disable();
        stream.disable();
        dac.disableDMA(channel);
        dac.disableTrigger(channel);
        dac.setChannel(channel, V_DDA_VOLTAGE);
    }

private:
    /**
     * @brief Generate a function for the given expression.
     *
     * @warning The expression must not be null.
     */
    std::function<float(float)> buildFunction(const Expression *exp) {
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

    void generateWave(uint16_t *buff, std::function<float(float)> func,
                      float startTime, float interval) {
        for (uint16_t i = 0; i < buffersSize; i++) {
            buff[i] = computeDacValue(func, startTime + i * interval);
        }
    }

    uint16_t computeDacValue(std::function<float(float)> func, float t) {
        // Compute the target value
        float val = func(t);

        // Convert the target value into the dac register value
        uint16_t rawVal = (1 - val / 12) * 4095;

        return rawVal < 4096 ? rawVal : 4095;
    }

    // float benchmarkComputation() {
    //     // Measure the execution time of the function
    //     auto start = miosix::getTime();
    //     (void)computeDacValue(42.123);
    //     auto stop = miosix::getTime();

    //     // Compute the maximum possible frequency
    //     return 1e9f / (stop - start);
    // }

    struct ChannelCtrlData {
        uint16_t *buffer1;
        uint16_t *buffer2;
        std::function<float(float)> func;
        float nextStartTime = 0;
    };

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
