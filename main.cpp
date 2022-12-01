#include <miosix.h>

#include <cstdio>

using namespace std;
using namespace miosix;

#define XRAM_0 ((uint32_t *)0xc0000000 + 0x100000 - 1)

int main() {
    bool success = true;

    uint32_t *ptr = ((uint32_t *)0xc0000000);

    for (uint32_t i = 0; i < 4 * 1024 * 1024; i += 0x100)
        ptr[i] = 42;

    for (uint32_t i = 0; i < 4 * 1024 * 1024; i += 0x100) {
        if (ptr[i] != 42) {
            printf("XRAM test failed\n");
            success = false;
            break;
        }
    }

    if (success)
        printf("XRAM test success\n");

    uint8_t *start = ((uint8_t *)0xc0000480);
    uint8_t *stop = ((uint8_t *)0xc0000598);

    for (size_t i = 0; i < stop - start; i++)
        start[i] = 0;
    printf("Zeroing completed\n");

    while (true) {
        userLed2::low();
        Thread::sleep(500);
        userLed2::high();
        Thread::sleep(500);
    }
}
