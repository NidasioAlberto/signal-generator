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

#include "DMA.h"

#include <kernel/logging.h>

#include <map>

using namespace miosix;

void __attribute__((naked)) DMA1_Stream0_IRQHandler() {
    saveContext();
    asm volatile("bl _Z20DMA1_Stream0_IRQImplv");
    restoreContext();
}

void __attribute__((used)) DMA1_Stream0_IRQImpl() {
    DMADriver::instance().IRQhandleInterrupt(DMAStreamId::DMA1_Str0);
}

void __attribute__((naked)) DMA1_Stream1_IRQHandler() {
    saveContext();
    asm volatile("bl _Z20DMA1_Stream1_IRQImplv");
    restoreContext();
}

void __attribute__((used)) DMA1_Stream1_IRQImpl() {
    DMADriver::instance().IRQhandleInterrupt(DMAStreamId::DMA1_Str1);
}

void __attribute__((naked)) DMA1_Stream2_IRQHandler() {
    saveContext();
    asm volatile("bl _Z20DMA1_Stream2_IRQImplv");
    restoreContext();
}

void __attribute__((used)) DMA1_Stream2_IRQImpl() {
    DMADriver::instance().IRQhandleInterrupt(DMAStreamId::DMA1_Str2);
}

void __attribute__((naked)) DMA1_Stream3_IRQHandler() {
    saveContext();
    asm volatile("bl _Z20DMA1_Stream3_IRQImplv");
    restoreContext();
}

void __attribute__((used)) DMA1_Stream3_IRQImpl() {
    DMADriver::instance().IRQhandleInterrupt(DMAStreamId::DMA1_Str3);
}

void __attribute__((naked)) DMA1_Stream4_IRQHandler() {
    saveContext();
    asm volatile("bl _Z20DMA1_Stream4_IRQImplv");
    restoreContext();
}

void __attribute__((used)) DMA1_Stream4_IRQImpl() {
    DMADriver::instance().IRQhandleInterrupt(DMAStreamId::DMA1_Str4);
}

void __attribute__((naked)) DMA1_Stream5_IRQHandler() {
    saveContext();
    asm volatile("bl _Z20DMA1_Stream5_IRQImplv");
    restoreContext();
}

void __attribute__((used)) DMA1_Stream5_IRQImpl() {
    DMADriver::instance().IRQhandleInterrupt(DMAStreamId::DMA1_Str5);
}

void __attribute__((naked)) DMA1_Stream6_IRQHandler() {
    saveContext();
    asm volatile("bl _Z20DMA1_Stream6_IRQImplv");
    restoreContext();
}

void __attribute__((used)) DMA1_Stream6_IRQImpl() {
    DMADriver::instance().IRQhandleInterrupt(DMAStreamId::DMA1_Str6);
}

void __attribute__((naked)) DMA1_Stream7_IRQHandler() {
    saveContext();
    asm volatile("bl _Z20DMA1_Stream7_IRQImplv");
    restoreContext();
}

void __attribute__((used)) DMA1_Stream7_IRQImpl() {
    DMADriver::instance().IRQhandleInterrupt(DMAStreamId::DMA1_Str7);
}

void __attribute__((naked)) DMA2_Stream0_IRQHandler() {
    saveContext();
    asm volatile("bl _Z20DMA2_Stream0_IRQImplv");
    restoreContext();
}

void __attribute__((used)) DMA2_Stream0_IRQImpl() {
    DMADriver::instance().IRQhandleInterrupt(DMAStreamId::DMA2_Str0);
}

void __attribute__((naked)) DMA2_Stream1_IRQHandler() {
    saveContext();
    asm volatile("bl _Z20DMA2_Stream1_IRQImplv");
    restoreContext();
}

void __attribute__((used)) DMA2_Stream1_IRQImpl() {
    DMADriver::instance().IRQhandleInterrupt(DMAStreamId::DMA2_Str1);
}

void __attribute__((naked)) DMA2_Stream2_IRQHandler() {
    saveContext();
    asm volatile("bl _Z20DMA2_Stream2_IRQImplv");
    restoreContext();
}

void __attribute__((used)) DMA2_Stream2_IRQImpl() {
    DMADriver::instance().IRQhandleInterrupt(DMAStreamId::DMA2_Str2);
}

void __attribute__((naked)) DMA2_Stream3_IRQHandler() {
    saveContext();
    asm volatile("bl _Z20DMA2_Stream3_IRQImplv");
    restoreContext();
}

void __attribute__((used)) DMA2_Stream3_IRQImpl() {
    DMADriver::instance().IRQhandleInterrupt(DMAStreamId::DMA2_Str3);
}

void __attribute__((naked)) DMA2_Stream4_IRQHandler() {
    saveContext();
    asm volatile("bl _Z20DMA2_Stream4_IRQImplv");
    restoreContext();
}

void __attribute__((used)) DMA2_Stream4_IRQImpl() {
    DMADriver::instance().IRQhandleInterrupt(DMAStreamId::DMA2_Str4);
}

// void __attribute__((naked)) DMA2_Stream5_IRQHandler() {
//     saveContext();
//     asm volatile("bl _Z20DMA2_Stream5_IRQImplv");
//     restoreContext();
// }

// void __attribute__((used)) DMA2_Stream5_IRQImpl() {
//     DMADriver::instance().IRQhandleInterrupt(DMAStreamId::DMA2_Str5);
// }

void __attribute__((naked)) DMA2_Stream6_IRQHandler() {
    saveContext();
    asm volatile("bl _Z20DMA2_Stream6_IRQImplv");
    restoreContext();
}

void __attribute__((used)) DMA2_Stream6_IRQImpl() {
    DMADriver::instance().IRQhandleInterrupt(DMAStreamId::DMA2_Str6);
}

// void __attribute__((naked)) DMA2_Stream7_IRQHandler() {
//     saveContext();
//     asm volatile("bl _Z20DMA2_Stream7_IRQImplv");
//     restoreContext();
// }

// void __attribute__((used)) DMA2_Stream7_IRQImpl() {
//     DMADriver::instance().IRQhandleInterrupt(DMAStreamId::DMA2_Str7);
// }

void DMADriver::IRQhandleInterrupt(DMAStreamId id) {
    auto stream = streams[id];

    // Check if the interrupt was triggered by an half transfer event
    if (stream->getHalfTransferInterruptStatus()) {
        stream->halfTransferTriggered = true;

        // Run the callback if defined
        if (stream->halfTransferCallback) {
            stream->halfTransferCallback();
        }

        // Wakeup the waiting thread if it is waiting
        if (stream->waitingThread && stream->waitingForHalfTransfer) {
            IRQwakeupThread(stream);
        }

        stream->clearHalfTransferInterrupt();
    }

    // Check if the interrupt was triggered by a transfer complete event
    if (stream->getTransferCompleteInterruptStatus()) {
        stream->transferCompleteTriggered = true;

        // Run the callback if defined
        if (stream->transferCompleteCallback) {
            stream->transferCompleteCallback();
        }

        // Wakeup the waiting thread if it is waiting
        if (stream->waitingThread && stream->waitingForTransferComplete) {
            IRQwakeupThread(stream);
        }

        stream->clearTransferCompleteInterrupt();
    }
}

void DMADriver::IRQwakeupThread(DMAStream* stream) {
    // Wakeup the waiting thread
    stream->waitingThread->wakeup();

    // If the waiting thread has a higher priority than the current
    // thread then reschedule
    if (stream->waitingThread->IRQgetPriority() >
        miosix::Thread::IRQgetCurrentThread()->IRQgetPriority()) {
        miosix::Scheduler::IRQfindNextThread();
    }

    // Clear the thread pointer, this way the thread will be sure it is
    // not a spurious wakeup
    stream->waitingThread = nullptr;
}

DMADriver& DMADriver::instance() {
    static DMADriver instance;
    return instance;
}

bool DMADriver::tryChannel(DMAStreamId id) {
    Lock<FastMutex> l(mutex);

    // Return true, meaning that the channel is free, only if it is not yet
    // allocated
    return streams.count(id) == 0;
}

DMAStream& DMADriver::acquireStream(DMAStreamId id) {
    Lock<FastMutex> l(mutex);

    // Wait until the channel is free
    while (streams.count(id) != 0)
        cv.wait(l);

    // Enable the clock if not already done
    // TODO: Enable DMA1 or DMA2
    // if (streams.size() == 0)
    //     RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

    return *(streams[id] = new DMAStream(id));
}

void DMADriver::releaseStream(DMAStreamId id) {
    Lock<FastMutex> l(mutex);

    if (streams.count(id) != 0) {
        delete streams[id];
        streams.erase(id);
        cv.broadcast();
    }

    // Disable the clock if there are no more channels
    // TODO: Disable DMA1 or DMA2
    // if (streams.size() == 0)
    //     RCC->AHB1ENR &= ~RCC_AHB1ENR_DMA1EN;
}

DMADriver::DMADriver() {
    // For now enable the clock always
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;

    // Reset interrupts flags
    DMA1->HIFCR = 0x0f7d0f7d;
    DMA1->LIFCR = 0x0f7d0f7d;
    DMA2->HIFCR = 0x0f7d0f7d;
    DMA2->LIFCR = 0x0f7d0f7d;
}

void DMAStream::setup(DMATransaction transaction) {
    currentSetup = transaction;

    // Reset the configuration
    registers->CR = 0;

    // Wait for the stream to actually be disabled
    while (registers->CR & DMA_SxCR_EN)
        ;

    registers->CR |= static_cast<uint32_t>(transaction.channel);
    registers->CR |= static_cast<uint32_t>(transaction.direction);
    registers->CR |= static_cast<uint32_t>(transaction.priority);
    if (transaction.circularMode)
        registers->CR |= DMA_SxCR_CIRC;
    registers->NDTR = transaction.numberOfDataItems;

    if (transaction.direction == DMATransaction::Direction::MEM_TO_PER) {
        // In memory to peripheral mode, the source address is the memory
        // address

        registers->CR |= static_cast<uint32_t>(transaction.srcSize)
                         << DMA_SxCR_MSIZE_Pos;
        registers->CR |= static_cast<uint32_t>(transaction.dstSize)
                         << DMA_SxCR_PSIZE_Pos;

        if (transaction.srcIncrement)
            registers->CR |= DMA_SxCR_MINC;
        if (transaction.dstIncrement)
            registers->CR |= DMA_SxCR_PINC;

        registers->M0AR = reinterpret_cast<uint32_t>(transaction.srcAddress);
        registers->PAR = reinterpret_cast<uint32_t>(transaction.dstAddress);

    } else {
        // In peripheral to memory or memory to memory mode, the source address
        // goes into the peripheral address register

        registers->CR |= static_cast<uint32_t>(transaction.srcSize)
                         << DMA_SxCR_PSIZE_Pos;
        registers->CR |= static_cast<uint32_t>(transaction.dstSize)
                         << DMA_SxCR_MSIZE_Pos;

        if (transaction.srcIncrement)
            registers->CR |= DMA_SxCR_PINC;
        if (transaction.dstIncrement)
            registers->CR |= DMA_SxCR_MINC;

        registers->PAR = reinterpret_cast<uint32_t>(transaction.srcAddress);
        registers->M0AR = reinterpret_cast<uint32_t>(transaction.dstAddress);
    }

    if (transaction.doubleBufferMode) {
        registers->CR |= DMA_SxCR_DBM;
        registers->M1AR =
            reinterpret_cast<uint32_t>(transaction.secondMemoryAddress);
    }

    bool enableInterrupt = false;
    if (transaction.enableHalfTransferInterrupt) {
        clearHalfTransferInterrupt();
        registers->CR |= DMA_SxCR_HTIE;
        enableInterrupt = true;
    }
    if (transaction.enableTransferCompleteInterrupt) {
        clearTransferCompleteInterrupt();
        registers->CR |= DMA_SxCR_TCIE;
        enableInterrupt = true;
    }
    if (transaction.enableTransferErrorInterrupt) {
        clearTransferErrorInterrupt();
        registers->CR |= DMA_SxCR_TEIE;
        enableInterrupt = true;
    }
    if (transaction.enableFifoErrorInterrupt) {
        clearFifoErrorInterrupt();
        registers->CR |= DMA_SxFCR_FEIE;
        enableInterrupt = true;
    }
    if (transaction.enableDirectModeErrorInterrupt) {
        clearDirectModeErrorInterrupt();
        registers->CR |= DMA_SxCR_DMEIE;
        enableInterrupt = true;
    }

    if (enableInterrupt) {
        NVIC_SetPriority(irqNumber, 8);
        NVIC_ClearPendingIRQ(irqNumber);
        NVIC_EnableIRQ(irqNumber);
    } else {
        NVIC_DisableIRQ(irqNumber);
    }
}

void DMAStream::enable() {
    // Reset the flags
    transferCompleteTriggered = false;

    // Enable the peripheral
    registers->CR |= DMA_SxCR_EN;
}

void DMAStream::disable() { registers->CR &= ~DMA_SxCR_EN; }

void DMAStream::waitForHalfTransfer() {
    waitForInterruptEventImpl(
        currentSetup.enableHalfTransferInterrupt,
        std::bind(&DMAStream::getHalfTransferInterruptStatus, this),
        std::bind(&DMAStream::clearHalfTransferInterrupt, this),
        halfTransferTriggered, waitingForHalfTransfer);
}

void DMAStream::waitForTransferComplete() {
    waitForInterruptEventImpl(
        currentSetup.enableTransferCompleteInterrupt,
        std::bind(&DMAStream::getTransferCompleteInterruptStatus, this),
        std::bind(&DMAStream::clearTransferCompleteInterrupt, this),
        transferCompleteTriggered, waitingForTransferComplete);
}

bool DMAStream::timedWaitForHalfTransfer(uint64_t timeout_ns) {
    return timedWaitForInterruptEventImpl(
        currentSetup.enableHalfTransferInterrupt,
        std::bind(&DMAStream::getHalfTransferInterruptStatus, this),
        std::bind(&DMAStream::clearHalfTransferInterrupt, this),
        halfTransferTriggered, waitingForHalfTransfer, timeout_ns);
}

bool DMAStream::timedWaitForTransferComplete(uint64_t timeout_ns) {
    return timedWaitForInterruptEventImpl(
        currentSetup.enableTransferCompleteInterrupt,
        std::bind(&DMAStream::getTransferCompleteInterruptStatus, this),
        std::bind(&DMAStream::clearTransferCompleteInterrupt, this),
        transferCompleteTriggered, waitingForTransferComplete, timeout_ns);
}

void DMAStream::setHalfTransferCallback(std::function<void()> callback) {
    halfTransferCallback = callback;
}

void DMAStream::resetHalfTransferCallback() { halfTransferCallback = nullptr; }

void DMAStream::setTransferCompleteCallback(std::function<void()> callback) {
    transferCompleteCallback = callback;
}

void DMAStream::resetTransferCompleteCallback() {
    transferCompleteCallback = nullptr;
}

void DMAStream::clearHalfTransferInterrupt() {
    *IFCR |= DMA_LIFCR_CHTIF0 << IFindex;
}

void DMAStream::clearTransferCompleteInterrupt() {
    *IFCR |= DMA_LIFCR_CTCIF0 << IFindex;
}

void DMAStream::clearTransferErrorInterrupt() {
    *IFCR |= DMA_LIFCR_CTEIF0 << IFindex;
}

void DMAStream::clearFifoErrorInterrupt() {
    *IFCR |= DMA_LIFCR_CFEIF0 << IFindex;
}

void DMAStream::clearDirectModeErrorInterrupt() {
    *IFCR |= DMA_LIFCR_CDMEIF0 << IFindex;
}

void DMAStream::clearAllInterrupts() {
    *IFCR |= (DMA_LIFCR_CHTIF0 | DMA_LIFCR_CTCIF0 | DMA_LIFCR_CTEIF0 |
              DMA_LIFCR_CFEIF0 | DMA_LIFCR_CDMEIF0)
             << IFindex;
}

bool DMAStream::getHalfTransferInterruptStatus() {
    return (*ISR & (DMA_LISR_HTIF0 << IFindex)) != 0;
}

bool DMAStream::getTransferCompleteInterruptStatus() {
    return (*ISR & (DMA_LISR_TCIF0 << IFindex)) != 0;
}

bool DMAStream::getTransferErrorInterruptStatus() {
    return (*ISR & (DMA_LISR_TEIF0 << IFindex)) != 0;
}

bool DMAStream::getFifoErrorInterruptStatus() {
    return (*ISR & (DMA_LISR_FEIF0 << IFindex)) != 0;
}

bool DMAStream::getDirectModeErrorInterruptStatus() {
    return (*ISR & (DMA_LISR_DMEIF0 << IFindex)) != 0;
}

int DMAStream::getCurrentBufferNumber() {
    return (registers->CR & DMA_SxCR_CT) != 0 ? 2 : 1;
}

DMAStream::DMAStream(DMAStreamId id) : id(id) {
    // Get the channel registers base address and the interrupt flags clear
    // register address
    if (id < DMAStreamId::DMA2_Str0) {
        registers = reinterpret_cast<DMA_Stream_TypeDef*>(
            DMA1_BASE + 0x10 + 0x18 * static_cast<int>(id));

        if (id < DMAStreamId::DMA1_Str4) {
            IFCR = &DMA1->LIFCR;
            ISR = &DMA1->LISR;
        } else {
            IFCR = &DMA1->HIFCR;
            ISR = &DMA1->HISR;
        }
    } else {
        registers = reinterpret_cast<DMA_Stream_TypeDef*>(
            DMA2_BASE + 0x10 + 0x18 * (static_cast<int>(id) - 8));

        if (id < DMAStreamId::DMA2_Str4) {
            IFCR = &DMA2->LIFCR;
            ISR = &DMA2->LISR;
        } else {
            IFCR = &DMA2->HIFCR;
            ISR = &DMA2->HISR;
        }
    }

    // Compute the index for the interrupt flags clear register
    // Refer to reference manual for the register bits structure
    int offset = static_cast<int>(id) % 4;
    IFindex = (offset % 2) * 6 + (offset / 2) * 16;

    // Select the interrupt
    irqNumber = static_cast<IRQn_Type>(
        static_cast<int>(IRQn_Type::DMA1_Stream0_IRQn) + static_cast<int>(id));
}
