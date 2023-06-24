#include <RGBLed/RGBLed.h>
#include <generator/Generator.h>
#include <miosix.h>
#include <parser/Parser.h>
#include <util/util.h>

using namespace miosix;

RGBLed::Color errorColor = {64, 0, 0, 0};
RGBLed::Color runningColor = {0, 64, 0, 0};
RGBLed::Color idleColor = {0, 64, 64, 0};

int main() {
    // Start the CPU profiler and print every second the CPU usage
    // CPUProfiler::thread(1e9);

    RGBLed led;
    led.init();

    Generator generator;
    generator.init();

    std::function<void(const Command &)> onCommand =
        [&](const Command &command) {
            // TODO: Check also if the expression set before start
            switch (command.type) {
                case CommandType::START:
                    printf("Starting channel %d...\n",
                           static_cast<int>(command.channel));
                    generator.start(
                        static_cast<DACDriver::Channel>(command.channel));
                    led.setColor(runningColor);
                    break;
                case CommandType::STOP:
                    printf("Stopping channel %d...\n",
                           static_cast<int>(command.channel));
                    generator.stop(
                        static_cast<DACDriver::Channel>(command.channel));
                    led.setColor(idleColor);
                    break;
                case CommandType::EXPRESSION:
                    printf("Loading expression for channel %d\n",
                           static_cast<int>(command.channel));
                    generator.setExpression(
                        static_cast<DACDriver::Channel>(command.channel),
                        command.exp);
                    led.setColor(idleColor);
                    break;
            }
        };

    std::function<void()> onError = [&]() { led.setColor(errorColor); };

    Parser parser(onCommand, onError);
    parser.start();

    while (true)
        Thread::sleep(1000);
}