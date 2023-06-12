#include <drivers/DAC/DAC.h>
#include <drivers/DMA/DMA.h>
#include <drivers/timer/GeneralPurposeTimer.h>
#include <miosix.h>

#include <cmath>

#define PI 3.14159265358979f

using namespace miosix;

DACDriver dac;
DMAStream& stream5 =
    DMADriver::instance().acquireStream(DMAStreamId::DMA1_Str5);
GP32bitTimer tim2(TIM2);

static constexpr size_t SIGNAL_ARRAY_SIZE = 100;
static uint32_t buffer1[SIGNAL_ARRAY_SIZE] = {0};
static uint32_t buffer2[SIGNAL_ARRAY_SIZE] = {0};

static int counter1 = 0;
static int counter2 = 0;
static int counterHalfTransferInt = 0;
static int counterTransferCompleteInt = 0;
static int counterErrorInterruptInt = 0;
static int counterFifoErrorInt = 0;
static int counterDirectModeErrorInt = 0;

static uint64_t benchmark;

void loadBuffer1() {
    uint64_t start = IRQgetTime();

    auto last = buffer2[SIGNAL_ARRAY_SIZE - 1] + 1;
    for (size_t i = 0; i < SIGNAL_ARRAY_SIZE; i++)
        buffer1[i] = (last + i) % 4096;

    counter1++;
    benchmark = IRQgetTime() - start;
}

void loadBuffer2() {
    auto start = buffer1[SIGNAL_ARRAY_SIZE - 1] + 1;
    for (size_t i = 0; i < SIGNAL_ARRAY_SIZE; i++)
        buffer2[i] = (start + i) % 4096;

    counter2++;
}

void __attribute__((naked)) DMA1_Stream5_IRQHandler() {
    saveContext();
    asm volatile("bl _Z20DMA1_Stream5_IRQImplv");
    restoreContext();
}

void __attribute__((used)) DMA1_Stream5_IRQImpl() {
    if (DMA1->HISR & DMA_HISR_HTIF5) {
        counterHalfTransferInt++;

        // Clear the interrupt flag
        DMA1->HIFCR |= DMA_HIFCR_CHTIF5;
    }

    if (DMA1->HISR & DMA_HISR_TCIF5) {
        counterTransferCompleteInt++;

        if (!(DMA1_Stream5->CR & DMA_SxCR_CT)) {
            // Current target is buffer 1 so we refill buffer 2
            loadBuffer2();
        } else {
            // Current target is buffer 2 so we refill buffer 1
            loadBuffer1();
        }

        DMA1->HIFCR |= DMA_HIFCR_CTCIF5;
    }

    if (DMA1->HISR & DMA_HISR_TEIF5) {
        counterErrorInterruptInt++;
        DMA1->HIFCR |= DMA_HIFCR_CTEIF5;
    }

    if (DMA1->HISR & DMA_HISR_FEIF5) {
        counterFifoErrorInt++;
        DMA1->HIFCR |= DMA_HIFCR_CFEIF5;
    }

    if (DMA1->HISR & DMA_HISR_DMEIF5) {
        counterDirectModeErrorInt++;
        DMA1->HIFCR |= DMA_HIFCR_CDMEIF5;
    }
}

int main() {
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
            .destinationIncrement = false,
            .circularMode = true,
            .doubleBufferMode = true,
            .enableHalfTransferInterrupt = false,
            .enableTransferCompleteInterrupt = true,
            .enableTransferErrorInterrupt = false,
            .enableFifoErrorInterrupt = false,
            .enableDirectModeErrorInterrupt = false,
        };
        stream5.setup(trn);

        NVIC_SetPriority(DMA1_Stream5_IRQn, 2);  // High priority for DMA
        NVIC_EnableIRQ(DMA1_Stream5_IRQn);

        stream5.enable();
    }

    // Setup TIM2
    {
        tim2.setMasterMode(TimerUtils::MasterMode::UPDATE);
        tim2.setFrequency(2 * 10000);
        tim2.setAutoReloadRegister(1);
        tim2.enable();
    }

    while (true) {
        printf("%lld\t", benchmark);
        printf("%d %d %d ", counter1, counter2,
               (int)(DMA1_Stream5->CR & DMA_SxCR_CT) >> DMA_SxCR_CT_Pos);
        printf("ints: %d %d %d %d %d\n", counterHalfTransferInt,
               counterTransferCompleteInt, counterErrorInterruptInt,
               counterFifoErrorInt, counterDirectModeErrorInt);
        Thread::sleep(100);
    };
}