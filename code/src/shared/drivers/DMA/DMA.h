/***************************************************************************
 *   Copyright (C) 2023 by Alberto Nidasio                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   As a special exception, if other files instantiate templates or use   *
 *   macros or inline functions from this file, or you compile this file   *
 *   and link it with other works to produce a work based on this file,    *
 *   this file does not by itself cause the resulting work to be covered   *
 *   by the GNU General Public License. However the source code for this   *
 *   file must still be made available in accordance with the GNU General  *
 *   Public License. This exception does not invalidate any other reasons  *
 *   why a work based on this file might be covered by the GNU General     *
 *   Public License.                                                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>   *
 ***************************************************************************/

#pragma once

#include <interfaces/arch_registers.h>
#include <kernel/scheduler/scheduler.h>
#include <kernel/sync.h>
#include <utils/OtherUtils.h>

#include <functional>
#include <map>

enum class DMAStreamId {
    DMA1_Str0 = 0,
    DMA1_Str1,
    DMA1_Str2,
    DMA1_Str3,
    DMA1_Str4,
    DMA1_Str5,
    DMA1_Str6,
    DMA1_Str7,
    DMA2_Str0,
    DMA2_Str1,
    DMA2_Str2,
    DMA2_Str3,
    DMA2_Str4,
    DMA2_Str5,
    DMA2_Str6,
    DMA2_Str7,
};

struct DMATransaction {
    enum class Channel : uint32_t {
        CHANNEL0 = 0,
        CHANNEL1 = DMA_SxCR_CHSEL_0,
        CHANNEL2 = DMA_SxCR_CHSEL_1,
        CHANNEL3 = DMA_SxCR_CHSEL_1 | DMA_SxCR_CHSEL_0,
        CHANNEL4 = DMA_SxCR_CHSEL_2,
        CHANNEL5 = DMA_SxCR_CHSEL_2 | DMA_SxCR_CHSEL_0,
        CHANNEL6 = DMA_SxCR_CHSEL_2 | DMA_SxCR_CHSEL_1,
        CHANNEL7 = DMA_SxCR_CHSEL,
    };

    enum class Direction : uint16_t {
        MEM_TO_MEM = DMA_SxCR_DIR_1,
        MEM_TO_PER = DMA_SxCR_DIR_0,
        PER_TO_MEM = 0,
    };

    enum class Priority : uint32_t {
        VERY_HIGH = DMA_SxCR_PL,
        HIGH = DMA_SxCR_PL_1,
        MEDIUM = DMA_SxCR_PL_0,
        LOW = 0,
    };

    enum class DataSize : uint8_t {
        BITS_8,
        BITS_16,
        BITS_32,
    };

    Channel channel = Channel::CHANNEL0;
    Direction direction = Direction::MEM_TO_MEM;
    Priority priority = Priority::LOW;
    DataSize srcSize = DataSize::BITS_32;
    DataSize dstSize = DataSize::BITS_32;
    volatile void* srcAddress = nullptr;
    volatile void* dstAddress = nullptr;
    volatile void* secondMemoryAddress = nullptr;
    uint16_t numberOfDataItems = 0;
    bool srcIncrement = false;
    bool dstIncrement = false;
    bool circularMode = false;
    bool doubleBufferMode = false;
    bool enableHalfTransferInterrupt = false;
    bool enableTransferCompleteInterrupt = false;
    bool enableTransferErrorInterrupt = false;
    bool enableFifoErrorInterrupt = false;
    bool enableDirectModeErrorInterrupt = false;
};

// Forward declaration
class DMAStream;

class DMADriver {
public:
    void IRQhandleInterrupt(DMAStreamId id);

    static DMADriver& instance();

    bool tryChannel(DMAStreamId id);

    DMAStream& acquireStream(DMAStreamId id);

    void releaseStream(DMAStreamId id);

private:
    DMADriver();

    void IRQwakeupThread(DMAStream* stream);

    miosix::FastMutex mutex;
    miosix::ConditionVariable cv;
    std::map<DMAStreamId, DMAStream*> streams;

public:
    DMADriver(const DMADriver&) = delete;
    DMADriver& operator=(const DMADriver&) = delete;
};

class DMAStream {
    friend DMADriver;

public:
    void setup(DMATransaction transaction);

    void enable();

    void disable();

    void waitForHalfTransfer();

    void waitForTransferComplete();

    bool timedWaitForHalfTransfer(uint64_t timeout_ns);

    bool timedWaitForTransferComplete(uint64_t timeout_ns);

    void setHalfTransferCallback(std::function<void()> callback);

    void resetHalfTransferCallback();

    void setTransferCompleteCallback(std::function<void()> callback);

    void resetTransferCompleteCallback();

    void setErrorCallback(std::function<void()> callback);

    void resetErrorCallback();

    /**
     * @brief Reads the current flags status.
     *
     * The values can be read with the get***FlagStatus functions.
     */
    void readFlags();

    /**
     * @brief Returns the last read status of the half transfer flag.
     *
     * TODO: Explain what this flag intails and what to do.
     */
    inline bool getHalfTransferFlagStatus() { return halfTransferFlag; }

    /**
     * @brief Returns the last read status of the transfer complete flag.
     *
     * TODO: Explain what this flag intails and what to do.
     */
    inline bool getTransferCompleteFlagStatus() { return transferCompleteFlag; }

    /**
     * @brief Returns the last read status of the transfer error flag.
     *
     * TODO: Explain what this flag intails and what to do.
     */
    inline bool getTransferErrorFlagStatus() { return transferErrorFlag; }

    /**
     * @brief Returns the last read status of the fifo error flag.
     *
     * TODO: Explain what this flag intails and what to do.
     */
    inline bool getFifoErrorFlagStatus() { return fifoErrorFlag; }

    /**
     * @brief Returns the last read status of the direct mode error flag.
     *
     * TODO: Explain what this flag intails and what to do.
     */
    inline bool getDirectModeErrorFlagStatus() { return directModeErrorFlag; }

    /**
     * @brief Returns the number of the buffer currently in use.
     *
     * @return 1 or 2 depending on the buffer currently in use.
     */
    int getCurrentBufferNumber();

    inline void clearHalfTransferFlag() {
        *IFCR |= DMA_LIFCR_CHTIF0 << IFindex;
    }

    inline void clearTransferCompleteFlag() {
        *IFCR |= DMA_LIFCR_CTCIF0 << IFindex;
    }

    inline void clearTransferErrorFlag() {
        *IFCR |= DMA_LIFCR_CTEIF0 << IFindex;
    }

    inline void clearFifoErrorFlag() { *IFCR |= DMA_LIFCR_CFEIF0 << IFindex; }

    inline void clearDirectModeErrorFlag() {
        *IFCR |= DMA_LIFCR_CDMEIF0 << IFindex;
    }

    inline void clearAllFlags() {
        *IFCR |= (DMA_LIFCR_CHTIF0 | DMA_LIFCR_CTCIF0 | DMA_LIFCR_CTEIF0 |
                  DMA_LIFCR_CFEIF0 | DMA_LIFCR_CDMEIF0)
                 << IFindex;
    }

private:
    DMAStream(DMAStreamId id);

    DMATransaction currentSetup;
    miosix::Thread* waitingThread = nullptr;

    // These flags are set by the interrupt routine and tells the user
    // which event were triggered
    bool halfTransferFlag = false;
    bool transferCompleteFlag = false;
    bool transferErrorFlag = false;
    bool fifoErrorFlag = false;
    bool directModeErrorFlag = false;

    std::function<void()> halfTransferCallback;
    std::function<void()> transferCompleteCallback;
    std::function<void()> errorCallback;

    DMAStreamId id;
    IRQn_Type irqNumber;
    DMA_Stream_TypeDef* registers;

    volatile uint32_t* ISR;   ///< Interrupt status register
    volatile uint32_t* IFCR;  ///< Interrupt flags clear register
    int IFindex;              ///< Interrupt flags index

    inline bool waitForInterruptEventImpl(
        bool isInterruptEnabled, std::function<bool()> getEventStatus,
        std::function<void()> clearEventStatus, bool& eventTriggered,
        long long timeout_ns) {
        // Return value: true if the event was triggered, false if the timeout
        // expired
        bool result = false;

        // If the interrupt is enabled we can just pause and wait for it.
        // Otherwise we need to pool the flag.
        if (isInterruptEnabled) {
            // Here we have 2 cases:
            // - This function has been called after the interrupt fired. In
            // this case the interrupt saves the flags status and we check them
            // - The interrupt has not yet fired. We pause the thread and the
            // interrupt will wake us up

            if (eventTriggered) {
                result = true;
            } else {
                // Save the current thread pointer
                waitingThread = miosix::Thread::getCurrentThread();

                // Wait until the thread is woken up and the pointer is cleared
                miosix::FastInterruptDisableLock dLock;
                if (timeout_ns >= 0) {
                    do {
                        if (miosix::Thread::IRQenableIrqAndTimedWait(
                                dLock, timeout_ns + miosix::getTime()) ==
                            miosix::TimedWaitResult::Timeout) {
                            result = false;

                            // If the timeout expired we clear the thread
                            // pointer so that the interrupt, if it will occur,
                            // will not wake up the thread (and we can exit the
                            // while loop)
                            waitingThread = nullptr;
                        } else {
                            result = true;
                        }
                    } while (waitingThread);
                } else {
                    do {
                        miosix::Thread::IRQenableIrqAndWait(dLock);
                    } while (waitingThread);
                    result = true;
                }
            }

            // Before returning we need to clear the flags otherwise we could
            // get misfires
            eventTriggered = false;
        } else {
            // Pool the flag if the user did not enable the interrupt
            if (timeout_ns >= 0) {
                result =
                    OtherUtils::timedPollingFlag(getEventStatus, timeout_ns);
            } else {
                while (!getEventStatus())
                    ;
                result = true;
            }

            if (result) {
                // Clear the flag
                clearEventStatus();
            }
        }

        return result;
    }

public:
    DMAStream(const DMAStream&) = delete;
    DMAStream& operator=(const DMAStream&) = delete;
};
