#include <miosix.h>

using namespace std;
using namespace miosix;

GpioPin ch1(GPIOA_BASE, 4);
GpioPin ch2(GPIOA_BASE, 5);

// #define RCC_APB1ENR_DACEN \
//     ((uint32_t)0x20000000) /*!< DAC interface clock enable */

#define V_DDA_VOLTAGE 3.0f

int main() {
    ch2.mode(Mode::OUTPUT);

    // See page 255 of rm0008
    ch1.mode(Mode::INPUT_ANALOG);

    // Enable peripheral
    RCC->APB1ENR |= RCC_APB1ENR_DACEN;

    // Enable channel 1
    DAC->CR = DAC_CR_EN1;

    // Set output
    DAC->DHR12R1 = static_cast<uint16_t>(0xfff / V_DDA_VOLTAGE * 1.42);

    while (true) {
        ch2.high();
        Thread::sleep(5000);
        ch2.low();
        Thread::sleep(5000);
    }
}
