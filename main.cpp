#include <miosix.h>

#include <cstdio>

using namespace std;
using namespace miosix;

int main() {
    while (true) {
        printf("Hello world!\n");
        Thread::sleep(1000);
    }
}
