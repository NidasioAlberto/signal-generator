cmake_minimum_required(VERSION 3.21)
include(miosix-kernel/miosix/cmake/mbs.cmake)

project(SignalGenerator)

function(mbs_custom_target TARGET OPT_BOARD)
    # Use options.cmake to get board specific variables
    include(${KPATH}/config/options.cmake)

    target_include_directories(${TARGET} PUBLIC src/shared)
    target_sources(${TARGET} PUBLIC
        # DAC driver only for f2, f4 and f7 micros
        $<$<STREQUAL:${ARCH},cortexM4_stm32f2>:src/shared/drivers/DAC/DAC.cpp>
        $<$<STREQUAL:${ARCH},cortexM4_stm32f4>:src/shared/drivers/DAC/DAC.cpp>
        $<$<STREQUAL:${ARCH},cortexM4_stm32f7>:src/shared/drivers/DAC/DAC.cpp>

        # DMA driver only for f2, f4 and f7 micros
        $<$<STREQUAL:${ARCH},cortexM4_stm32f2>:src/shared/drivers/DMA/DMA.cpp>
        $<$<STREQUAL:${ARCH},cortexM4_stm32f4>:src/shared/drivers/DMA/DMA.cpp>
        $<$<STREQUAL:${ARCH},cortexM4_stm32f7>:src/shared/drivers/DMA/DMA.cpp>

        # SPI driver only for f2, f4 and f7 micros
        $<$<STREQUAL:${ARCH},cortexM4_stm32f2>:src/shared/drivers/spi/SPITransaction.cpp>
        $<$<STREQUAL:${ARCH},cortexM4_stm32f4>:src/shared/drivers/spi/SPITransaction.cpp>
        $<$<STREQUAL:${ARCH},cortexM4_stm32f7>:src/shared/drivers/spi/SPITransaction.cpp>
            
    )
    mbs_target(${TARGET} ${OPT_BOARD})
endfunction()

#-----------------------------------------------------------------------------#
#                                   Tests                                     #
#-----------------------------------------------------------------------------#

add_executable(test-rgb-led src/entrypoints/test-rgb-led.cpp)
mbs_custom_target(test-rgb-led stm32f103cx_generic)

add_executable(dac-example src/entrypoints/dac-example.cpp)
mbs_custom_target(dac-example stm32f429zi_stm32f4discovery)

add_executable(dac-dma-timer src/entrypoints/dac-dma-timer.cpp)
mbs_custom_target(dac-dma-timer stm32f429zi_stm32f4discovery)
