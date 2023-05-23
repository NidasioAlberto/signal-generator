
#include <miosix.h>

using namespace miosix;

int main() {
    while (true) {
        printf("Hi mom!\n");
        Thread::sleep(1000);
    }
}
