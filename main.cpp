#include <miosix.h>

#include <cstdio>

using namespace std;
using namespace miosix;

#define XRAM_0 ((uint32_t *)0xc0000000 + 0x100000 - 1)

int main() {
    printf("Testing 32bit accesses\n");
    uint32_t *ptr = ((uint32_t *)0xc0000000);
    for (uint32_t i = 0; i < 4 * 1024 * 1024; i += 0x100) {
        printf("%8lX\n", (uint32_t) & (ptr[i]));
        ptr[i] = 42;
    }

    // printf("Testing 8bit accesses\n");
    // uint8_t *ptr2 = ((uint8_t *)0xc0000000);
    // for (uint32_t i = 0; i < 16 * 1024 * 1024; i += 0x100) {
    //     printf("%8lX\n", (uint32_t) & (ptr2[i]));
    //     ptr2[i] = 42;
    // }

    while (true) {
        userLed2::low();

        Thread::sleep(500);

        userLed2::high();

        Thread::sleep(500);
    }
}
