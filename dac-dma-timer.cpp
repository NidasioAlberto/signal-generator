#include <drivers/dac/dac.h>
#include <math.h>
#include <miosix.h>

#define PI 3.14159265358979f

using namespace std;
using namespace miosix;

// Serial pins: PA9 PA10

constexpr size_t UPDATE_FREQ = 1e6;

constexpr size_t SIGNAL_ARRAY_SIZE = 100;
uint32_t signal_data[SIGNAL_ARRAY_SIZE] = {0};

void loadWave() {
    float step = 2 * PI / SIGNAL_ARRAY_SIZE;

    for (size_t i = 0; i < SIGNAL_ARRAY_SIZE; i++)
        signal_data[i] =
            static_cast<uint32_t>(sinf(step * i) * 2047.5 + 2047.5);
}

int main() {
    loadWave();

    // DAC
    {
        // Enable the DAC clock
        RCC->APB1ENR |= RCC_APB1ENR_DACEN;

        // Enable DAC ch1
        DAC->CR |= DAC_CR_EN1;

        // Enable DMA on DAC ch1
        DAC->CR |= DAC_CR_DMAEN1;

        // Select TIM6 as trigger by writing 000 in TSEL1
        DAC->CR &= ~DAC_CR_TSEL1;

        // Enable DAC ch1 trigger
        DAC->CR |= DAC_CR_TEN1;
    }

    // DMA
    {
        // Enable the DMA clock
        RCC->AHBENR |= RCC_AHBENR_DMA1EN;

        // Configure the peripheral data register address
        DMA1_Channel3->CPAR |= reinterpret_cast<uint32_t>(&(DAC->DHR12R1));

        // Configure the memory address
        DMA1_Channel3->CMAR |= reinterpret_cast<uint32_t>(signal_data);

        // Configure the number of DMA transfer to perform
        DMA1_Channel3->CNDTR = SIGNAL_ARRAY_SIZE;

        // Enable memory increment
        DMA1_Channel3->CCR |= DMA_CCR_MINC;

        // Memory size 32bit
        DMA1_Channel3->CCR |= DMA_CCR_MSIZE_1;

        // Peripheral size 32 bit
        DMA1_Channel3->CCR |= DMA_CCR_PSIZE_1;

        // Set transfer form memory to peripheral
        DMA1_Channel3->CCR |= DMA_CCR_DIR;

        // Enable circular mode
        DMA1_Channel3->CCR |= DMA_CCR_CIRC;

        // Enable the DMA channel 3
        DMA1_Channel3->CCR |= DMA_CCR_EN;
    }

    // TIM6
    {
        // Enable the TIM6 clock
        RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;

        // Set as TRGO source the UEV
        TIM6->CR2 |= TIM_CR2_MMS_1;

        // Set the prescaler
        TIM6->PSC = 4;

        // Set the auto reload register
        TIM6->ARR = 1;

        // Enable the timer
        TIM6->CR1 |= TIM_CR1_CEN;
    }

    while (true)
        ;
}
