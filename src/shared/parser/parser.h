#include <RGBLed/RGBLed.h>
#include <generator/Generator.h>
#include <miosix.h>
#include <parser/parser/lexer.h>
#include <parser/parser/parser.h>

#include <atomic>
#include <functional>
#include <thread>

class Parser {
public:
    Parser(std::function<void(const Command &)> onCommand,
           std::function<void()> onError);

    void start();

private:
    void run();

    bool checkChannel(float value);

    std::function<void(const Command &)> onCommand;
    std::function<void()> onError;

    std::thread runningThread;
};
