/* Copyright (c) 2023 Skyward Experimental Rocketry
 * Authors: Alberto Nidasio
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include <drivers/DMA/DMA.h>

#include "SPIBus.h"

/**
 * @brief Driver for STM32 low level SPI peripheral with DMA transactions.
 */
class SPIDMABus : public SPIBus {
public:
    SPIDMABus(SPI_TypeDef* spi, DMAStreamId txStreamId,
              DMATransaction::Channel txStreamChannel, DMAStreamId rxStreamId,
              DMATransaction::Channel rxStreamChannel);

    ///< Delete copy/move contructors/operators.
    SPIDMABus(const SPIDMABus&) = delete;
    SPIDMABus& operator=(const SPIDMABus&) = delete;
    SPIDMABus(SPIDMABus&&) = delete;
    SPIDMABus& operator=(SPIDMABus&&) = delete;

    // void configure(SPIBusConfig newConfig) override;

    void write(const uint8_t* data, size_t nBytes) override;

private:
    DMAStream& txStream;
    DMATransaction::Channel txStreamChannel;
    DMAStream& rxStream;
    DMATransaction::Channel rxStreamChannel;
};

inline SPIDMABus::SPIDMABus(SPI_TypeDef* spi, DMAStreamId txStreamId,
                            DMATransaction::Channel txStreamChannel,
                            DMAStreamId rxStreamId,
                            DMATransaction::Channel rxStreamChannel)
    : SPIBus(spi),
      txStream(DMADriver::instance().acquireStream(txStreamId)),
      txStreamChannel(txStreamChannel),
      rxStream(DMADriver::instance().acquireStream(rxStreamId)),
      rxStreamChannel(rxStreamChannel) {}

// inline void SPIDMABus::configure(SPIBusConfig newConfig) {}

void SPIDMABus::write(const uint8_t* data, size_t nBytes) {
    DMATransaction txTrn{
        .channel = txStreamChannel,
        .direction = DMATransaction::Direction::MEM_TO_PER,
        .priority = DMATransaction::Priority::HIGH,
        .srcSize = DMATransaction::DataSize::BITS_8,
        .dstSize = DMATransaction::DataSize::BITS_8,
        .srcAddress = const_cast<uint8_t*>(data),
        .dstAddress = &(spi->DR),
        .numberOfDataItems = 20,
        .sourceIncrement = true,
    };
    txStream.setup(txTrn);

    enableTxDMARequest();
    txStream.enable();

    miosix::delayMs(250);

    disableTxDMARequest();
    txStream.disable();
    txStream.getInterruptsStatus().print();
    txStream.clearAllInterrupts();
}