#include <miosix.h>

using namespace miosix;

int main() {
    while (true) {
        printf("Hello World!\n");
        Thread::sleep(1000);
    }
}
