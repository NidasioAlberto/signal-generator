cmake_minimum_required(VERSION 3.21)
include(miosix-kernel/miosix/cmake/mbs.cmake)

if(CMAKE_HOST_SYSTEM_NAME MATCHES "Darwin")
    set(BISON_EXECUTABLE "/usr/local/opt/bison/bin/bison" CACHE PATH "Bison executable")
endif()

find_package(BISON 3.0.0 REQUIRED)
find_package(FLEX)

project(SignalGenerator)

function(mbs_custom_target TARGET OPT_BOARD)
    # Use options.cmake to get board specific variables
    include(${KPATH}/config/options.cmake)

    target_include_directories(${TARGET} PUBLIC src/shared)
    target_sources(${TARGET} PUBLIC
        # DAC driver only for f2, f4 and f7 micros
        $<$<STREQUAL:${ARCH},cortexM3_stm32f2>:src/shared/drivers/DAC/DAC.cpp>
        $<$<STREQUAL:${ARCH},cortexM4_stm32f4>:src/shared/drivers/DAC/DAC.cpp>
        $<$<STREQUAL:${ARCH},cortexM4_stm32f7>:src/shared/drivers/DAC/DAC.cpp>
        
        # ADC driver only for f2, f4 and f7 micros
        $<$<STREQUAL:${ARCH},cortexM3_stm32f2>:src/shared/drivers/ADC/ADC.cpp>
        $<$<STREQUAL:${ARCH},cortexM4_stm32f4>:src/shared/drivers/ADC/ADC.cpp>
        $<$<STREQUAL:${ARCH},cortexM4_stm32f7>:src/shared/drivers/ADC/ADC.cpp>

        # DMA driver only for f2, f4 and f7 micros
        $<$<STREQUAL:${ARCH},cortexM3_stm32f2>:src/shared/drivers/DMA/DMA.cpp>
        $<$<STREQUAL:${ARCH},cortexM4_stm32f4>:src/shared/drivers/DMA/DMA.cpp>
        $<$<STREQUAL:${ARCH},cortexM4_stm32f7>:src/shared/drivers/DMA/DMA.cpp>

        # SPI driver only for f2, f4 and f7 micros
        $<$<STREQUAL:${ARCH},cortexM3_stm32f2>:src/shared/drivers/spi/SPITransaction.cpp>
        $<$<STREQUAL:${ARCH},cortexM4_stm32f4>:src/shared/drivers/spi/SPITransaction.cpp>
        $<$<STREQUAL:${ARCH},cortexM4_stm32f7>:src/shared/drivers/spi/SPITransaction.cpp>
            
    )
    mbs_target(${TARGET} ${OPT_BOARD})
endfunction()

#-----------------------------------------------------------------------------#
#                                   Tests                                     #
#-----------------------------------------------------------------------------#

add_executable(adc-dac src/entrypoints/adc-dac.cpp)
mbs_custom_target(adc-dac stm32f205rc_signal_generator)

add_executable(dac-dma-timer src/entrypoints/dac-dma-timer.cpp)
mbs_custom_target(dac-dma-timer stm32f205rc_signal_generator)

add_executable(dac-example src/entrypoints/dac-example.cpp)
mbs_custom_target(dac-example stm32f205rc_signal_generator)

add_executable(hello-world src/entrypoints/hello-world.cpp)
mbs_custom_target(hello-world stm32f205rc_signal_generator)

add_executable(test-rgb-led src/entrypoints/test-rgb-led.cpp)
mbs_custom_target(test-rgb-led stm32f205rc_signal_generator)

set(PARSER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/shared/parser)
flex_target(LEXER src/shared/parser/lexer.l ${PARSER_DIR}/lexer.c DEFINES_FILE ${PARSER_DIR}/lexer.h)
bison_target(PARSER src/shared/parser/parser.y ${PARSER_DIR}/parser.c DEFINES_FILE ${PARSER_DIR}/parser.h)
set_source_files_properties(${PARSER_DIR}/lexer.c PROPERTIES LANGUAGE CXX)
set_source_files_properties(${PARSER_DIR}/parser.c PROPERTIES LANGUAGE CXX)
add_executable(parser
    src/entrypoints/parser.cpp
    ${PARSER_DIR}/lexer.c
    ${PARSER_DIR}/parser.c
)
mbs_custom_target(parser stm32f205rc_signal_generator)