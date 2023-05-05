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

constexpr size_t SIGNAL_ARRAY_SIZE = 100;
uint32_t signal_data[SIGNAL_ARRAY_SIZE];

void loadWave() {
    float step = 2 * PI / SIGNAL_ARRAY_SIZE;

    for (size_t i = 0; i < SIGNAL_ARRAY_SIZE; i++)
        signal_data[i] =
            static_cast<uint32_t>(sinf(step * i) * 2047.5 + 2047.5);
}

int main() {
    loadWave();

    // Setup DAC
    {
        dac.enableChannel(DACDriver::Channel::CH1);
        dac.enableChannel(DACDriver::Channel::CH2);

        dac.enableDMA(DACDriver::Channel::CH1);
        dac.enableDMA(DACDriver::Channel::CH2);

        dac.enableTrigger(DACDriver::Channel::CH1,
                          DACDriver::TriggerSource::TIM2_TRGO);
        dac.enableTrigger(DACDriver::Channel::CH2,
                          DACDriver::TriggerSource::TIM2_TRGO);
    }

    // Setup DMA
    {
        DMATransaction ch1Trn{
            .channel = DMATransaction::Channel::CHANNEL7,
            .direction = DMATransaction::Direction::MEM_TO_PER,
            .srcAddress = signal_data,
            .dstAddress = &(DAC->DHR12R1),
            .numberOfDataItems = SIGNAL_ARRAY_SIZE,
            .sourceIncrement = true,
            .circularMode = true,
        };
        stream5.setup(ch1Trn);
        stream5.enable();
    }

    // Setup TIM2
    {
        tim2.setMasterMode(TimerUtils::MasterMode::UPDATE);
        tim2.setFrequency(2 * 1e6);
        tim2.setAutoReloadRegister(1);
        tim2.enable();
    }

    while (true)
        ;
}