#include <miosix.h>

using namespace miosix;

// Serial pins: PA9 PA10

int main() {
    while (true) {
        printf("Hey\n");
        delayMs(1000);
    }
}
