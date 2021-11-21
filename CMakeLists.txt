cmake_minimum_required(VERSION 3.13)

# recommended compiler: gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2
# https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads


# variables
set(CMAKE_C_STANDARD    11)
set(CMAKE_CXX_STANDARD  17)
set(DEBUG_LVL           "-g3")
set(OPTIM_LVL           "-O0")

include(stm32.cmake)

# The project name
project(STEP_SEQUENCER VERSION 0.1 DESCRIPTION "STEP_SEQUENCER")

find_package(Catch2 3 REQUIRED)

# set the executable target
add_executable(${BUILD_NAME} "")

# add source files and include dependencies
add_subdirectory(stm32cube_workspace)
add_subdirectory(source/main_app)
add_subdirectory(source/segger_rtt)
add_subdirectory(source/cpp_tlc5955)
add_subdirectory(source/cpp_ssd1306)


# display size info
add_custom_target(size ALL arm-none-eabi-size ${BUILD_NAME} DEPENDS ${BUILD_NAME})

# objcopy the elf file as a hex file
add_custom_target(build.bin ALL DEPENDS ${BUILD_NAME} COMMAND arm-none-eabi-objcopy -O ihex ${BUILD_NAME} ${HEX_NAME})

message(STATUS "HOST=${CMAKE_HOST_SYSTEM_NAME}")
message(STATUS "SYSTEM=${CMAKE_SYSTEM_NAME}")
message(STATUS "CPU=${CMAKE_SYSTEM_PROCESSOR}")