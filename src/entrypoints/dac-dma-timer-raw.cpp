#include <drivers/dac/dac.h>
#include <math.h>
#include <miosix.h>

#define PI 3.14159265358979f

using namespace std;
using namespace miosix;

#define RCC_APB1ENR_DACEN \
    ((uint32_t)0x20000000) /*!< DAC interface clock enable */

// Serial pins: PA9 PA10
typedef Gpio<GPIOA_BASE, 4> ch1;

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

    printf("Started\n");

    // DAC
    {
        ch1::mode(Mode::INPUT_ANALOG);

        // Enable the DAC clock
        RCC->APB1ENR |= RCC_APB1ENR_DACEN;

        // Enable DAC ch1
        DAC->CR |= DAC_CR_EN1;

        // Enable DMA on DAC ch1
        DAC->CR |= DAC_CR_DMAEN1;

        // Select TIM2 as trigger by writing 000 in TSEL1
        DAC->CR |= DAC_CR_TSEL1_2;

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
        DMA1_Channel3->CCR |= DMA_CCR3_MINC;

        // Memory size 32bit
        DMA1_Channel3->CCR |= DMA_CCR3_MSIZE_1;

        // Peripheral size 32 bit
        DMA1_Channel3->CCR |= DMA_CCR3_PSIZE_1;

        // Set transfer form memory to peripheral
        DMA1_Channel3->CCR |= DMA_CCR3_DIR;

        // Enable circular mode
        DMA1_Channel3->CCR |= DMA_CCR3_CIRC;

        // Enable the DMA channel 3
        DMA1_Channel3->CCR |= DMA_CCR3_EN;
    }

    // TIM2
    {
        // Enable the TIM2 clock
        RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

        // Set as TRGO source the UEV
        TIM2->CR2 |= TIM_CR2_MMS_1;

        // Set the prescaler
        TIM2->PSC = 4;

        // Set the auto reload register
        TIM2->ARR = 1;

        // Enable the timer
        TIM2->CR1 |= TIM_CR1_CEN;
    }

    while (true)
        ;
}
