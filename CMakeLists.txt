cmake_minimum_required(VERSION 3.21)
include(miosix-kernel/miosix/cmake/mbs.cmake)

project(SignalGenerator)

#-----------------------------------------------------------------------------#
#                                   Tests                                     #
#-----------------------------------------------------------------------------#

add_executable(main src/entrypoints/main.cpp)
mbs_target(main stm32f429zi_stm32f4discovery)
