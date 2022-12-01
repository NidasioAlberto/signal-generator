
#include <cstdio>

#include "miosix.h"

using namespace std;
using namespace miosix;

int main() {
    while (true) {
        printf("Hello world, write your application here\n");
        Thread::sleep(1000);
    }
}
