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
            .srcSize = DMATransaction::DataSize::BIT_32,
            .dstSize = DMATransaction::DataSize::BIT_32,
            .srcAddress = buffer1,
            .dstAddress = &(DAC->DHR12R1),
            .secondMemoryAddress = buffer2,
            .numberOfDataItems = SIGNAL_ARRAY_SIZE - 1,
            .sourceIncrement = true,
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