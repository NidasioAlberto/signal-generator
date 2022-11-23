#include <drivers/dac/dac.h>
#include <drivers/dma/dma.h>
#include <drivers/timers/basic_timer.h>
#include <math.h>
#include <miosix.h>

#define PI 3.14159265358979f

using namespace std;
using namespace miosix;

// Serial pins: PA9 PA10

constexpr size_t UPDATE_FREQ = 1e6;

constexpr size_t SIGNAL_ARRAY_SIZE = 100;
uint16_t signal_data[SIGNAL_ARRAY_SIZE] = {0};

void loadWave() {
    float step = 2 * PI / SIGNAL_ARRAY_SIZE;

    for (size_t i = 0; i < SIGNAL_ARRAY_SIZE; i++)
        signal_data[i] = static_cast<uint32_t>(sinf(step * i) * 1800 + 2047.5);
}

int main() {
    loadWave();

    // DAC
    DACDriver dac;
    dac.enableChannel(1);
    dac.enableDMA(1);
    dac.enableTrigger(1, DACDriver::TriggerSource::TIM6_TRGO);

    // DMA
    DMATransaction tr;
    tr.direction = DMATransaction::Direction::MEM_TO_PER;
    tr.sourceSize = DMATransaction::DataSize::BITS_16;
    tr.sourceAddress = signal_data;
    tr.destinationAddress = (void*)&(DAC->DHR12R1);
    tr.numberOfDataItems = SIGNAL_ARRAY_SIZE;
    tr.sourceIncrement = true;
    tr.circularMode = true;

    auto channel = DMADriver::instance().acquireChannel(DMAChannelId::DMA1_CH3);
    channel->setup(tr);
    channel->enable();
    printf("DMA ready: %4X\n", DMA1_Channel3->CCR);

    // TIM6
    BasicTimerDriver tim6(TIM6);
    tim6.clockOn();
    tim6.setMasterMode(TimerUtils::MasterMode::UPDATE);
    tim6.setClockFrequency(2e6);    // 2MHz
    tim6.setAutoReloadRegister(1);  // This will give an UEV freq. of 1MHz
    tim6.enable();

    while (true)
        ;
}
