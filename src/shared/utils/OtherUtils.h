#pragma once

#include <kernel/kernel.h>

#include <functional>

namespace OtherUtils {

/**
 * @brief Pools a flag until it is set or the timeout is reached.
 *
 * @return true if the flag was set, false if the timeout was reached.
 */
bool timedWaitFlag(std::function<bool()> readFlag, uint64_t timeout_ns) {
    uint64_t start = miosix::getTime();

    while (miosix::getTime() - start < timeout_ns) {
        if (readFlag()) {
            return true;
        }
    }

    return false;
}

}  // namespace OtherUtils