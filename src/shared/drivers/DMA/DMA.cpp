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

#include "DMA.h"

using namespace miosix;

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
    // Reset the configuration
    registers->CR = 0;

    registers->CR |= static_cast<uint32_t>(transaction.channel);
    registers->CR |= static_cast<uint32_t>(transaction.direction);
    registers->CR |= static_cast<uint32_t>(transaction.priority);
    if (transaction.circularMode)
        registers->CR |= DMA_SxCR_CIRC;
    registers->NDTR = transaction.numberOfDataItems;

    if (transaction.direction == DMATransaction::Direction::MEM_TO_PER) {
        // In memory to peripheral mode, the source address is the memory
        // address

        registers->CR |= static_cast<uint32_t>(transaction.srcSize) << 13;
        registers->CR |= static_cast<uint32_t>(transaction.dstSize) << 11;

        if (transaction.sourceIncrement)
            registers->CR |= DMA_SxCR_MINC;
        if (transaction.destinationIncrement)
            registers->CR |= DMA_SxCR_PINC;

        registers->M0AR = reinterpret_cast<uint32_t>(transaction.srcAddress);
        registers->PAR = reinterpret_cast<uint32_t>(transaction.dstAddress);

    } else {
        // In peripheral to memory or memory to memory mode, the source address
        // goes into the peripheral address register

        registers->CR |= static_cast<uint32_t>(transaction.srcSize) << 11;
        registers->CR |= static_cast<uint32_t>(transaction.dstSize) << 13;

        if (transaction.sourceIncrement)
            registers->CR |= DMA_SxCR_PINC;
        if (transaction.destinationIncrement)
            registers->CR |= DMA_SxCR_MINC;

        registers->PAR = reinterpret_cast<uint32_t>(transaction.srcAddress);
        registers->M0AR = reinterpret_cast<uint32_t>(transaction.dstAddress);
    }

    if (transaction.doubleBufferMode) {
        registers->CR |= DMA_SxCR_DBM;
        registers->M1AR =
            reinterpret_cast<uint32_t>(transaction.secondMemoryAddress);
    }

    if (transaction.enableHalfTransferInterrupt) {
        DMA1->HIFCR |= DMA_HIFCR_CHTIF5;
        registers->CR |= DMA_SxCR_HTIE;
    }
    if (transaction.enableTransferCompleteInterrupt) {
        DMA1->HIFCR |= DMA_HIFCR_CTCIF5;
        registers->CR |= DMA_SxCR_TCIE;
    }
    if (transaction.enableTransferErrorInterrupt) {
        DMA1->HIFCR |= DMA_HIFCR_CTEIF5;
        registers->CR |= DMA_SxCR_TEIE;
    }
    if (transaction.enableFifoErrorInterrupt) {
        DMA1->HIFCR |= DMA_HIFCR_CFEIF5;
        registers->CR |= DMA_SxFCR_FEIE;
    }
    if (transaction.enableDirectModeErrorInterrupt) {
        DMA1->HIFCR |= DMA_HIFCR_CDMEIF5;
        registers->CR |= DMA_SxCR_DMEIE;
    }
}

void DMAStream::enable() { registers->CR |= DMA_SxCR_EN; }

void DMAStream::disable() { registers->CR &= ~DMA_SxCR_EN; }

DMAStream::DMAStream(DMAStreamId id) {
    // Get the channel registers base address
    if (id < DMAStreamId::DMA2_Str0) {
        registers = reinterpret_cast<DMA_Stream_TypeDef*>(
            DMA1_BASE + 0x10 + 0x18 * static_cast<int>(id));
    } else {
        registers = reinterpret_cast<DMA_Stream_TypeDef*>(
            DMA2_BASE + 0x10 + 0x18 * (static_cast<int>(id) - 8));
    }
}
