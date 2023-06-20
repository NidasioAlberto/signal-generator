/***************************************************************************
 *   Copyright (C) 2022 by Alberto Nidasio                                 *
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
#include <kernel/sync.h>

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
        BIT_32,
    };

    Channel channel = Channel::CHANNEL0;
    Direction direction = Direction::MEM_TO_MEM;
    Priority priority = Priority::LOW;
    DataSize srcSize = DataSize::BIT_32;
    DataSize dstSize = DataSize::BIT_32;
    volatile void* srcAddress = nullptr;
    volatile void* dstAddress = nullptr;
    volatile void* secondMemoryAddress = nullptr;
    uint16_t numberOfDataItems = 0;
    bool sourceIncrement = false;
    bool destinationIncrement = false;
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
    static DMADriver& instance();

    bool tryChannel(DMAStreamId id);

    DMAStream& acquireStream(DMAStreamId id);

    void releaseStream(DMAStreamId id);

private:
    DMADriver();

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
    struct InterruptStatus {
        bool halfTransfer;
        bool transferComplete;
        bool transferError;
        bool fifoError;
        bool directModeError;

        void print();
    };

    void setup(DMATransaction transaction);

    void enable();

    void disable();

    void clearHalfTransferInterrupt();

    void clearTransferCompleteInterrupt();

    void clearTransferErrorInterrupt();

    void clearFifoErrorInterrupt();

    void clearDirectModeErrorInterrupt();

    void clearAllInterrupts();

    InterruptStatus getInterruptsStatus();

private:
    DMAStream(DMAStreamId id);

    DMAStreamId id;
    DMA_Stream_TypeDef* registers;

    volatile uint32_t* ISR;   ///< Interrupt status register
    volatile uint32_t* IFCR;  ///< Interrupt flags clear register
    int IFindex;              ///< Interrupt flags index

public:
    DMAStream(const DMAStream&) = delete;
    DMAStream& operator=(const DMAStream&) = delete;
};
