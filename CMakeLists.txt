cmake_minimum_required(VERSION 3.13)

# recommended compiler: gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2
# https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads

# common variables
set(CMAKE_C_STANDARD    11)
set(CMAKE_CXX_STANDARD  17)
set(DEBUG_LVL           "-g3")
set(OPTIM_LVL           "-O0")

# determine which kit was selected by user in VSCode CMake Tools extension
# and load the appropriate toolchain settings (stm32.cmake or linux.cmake)
if(${CMAKE_C_COMPILER} MATCHES "(gcc-arm-none-eabi)+")
    include(stm32.cmake)
elseif(${CMAKE_C_COMPILER} MATCHES "(x86_64-linux-gnu)+")   
    include(linux.cmake)
else()
    message(FATAL_ERROR "No suitable kit found. Aborting.")
endif()

message("Selected Kit: ${CMAKE_C_COMPILER}")

# Declare the project 
project(STEP_SEQUENCER VERSION 0.1 DESCRIPTION "STEP_SEQUENCER")

# set the executable target
add_executable(${BUILD_NAME} "")

# add common sources
add_subdirectory(source/segger_rtt)
add_subdirectory(source/cpp_tlc5955)
add_subdirectory(source/cpp_ssd1306)

if(${CMAKE_C_COMPILER} MATCHES "(gcc-arm-none-eabi)+") 
    # add the stm32-specific sources
    add_subdirectory(stm32cube_workspace)
    add_subdirectory(source/main_app)
elseif(${CMAKE_C_COMPILER} MATCHES "(x86_64-linux-gnu)+")   
    # setup CTest and add tests
    include(CTest)
    add_test(NAME tests COMMAND ${BUILD_NAME})
    add_subdirectory(source/tests)
endif()

# display size info
add_custom_target(size ALL ${CMAKE_SIZE} ${BUILD_NAME} DEPENDS ${BUILD_NAME})

# objcopy the elf file as a hex file, if using STM32
if(DEFINED ${HEX_NAME})
    add_custom_target(build.bin ALL DEPENDS ${BUILD_NAME} COMMAND ${CMAKE_OBJCOPY} -O ihex ${BUILD_NAME} ${HEX_NAME})
endif()
