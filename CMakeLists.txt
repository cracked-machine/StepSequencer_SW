cmake_minimum_required(VERSION 3.13)

# recommended compiler: gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2
# https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads

# common variables
set(CMAKE_C_STANDARD    11)
set(CMAKE_CXX_STANDARD  17)
set(DEBUG_LVL           "-g3")
set(OPTIM_LVL           "-O0")

set(ARM_TRIPLET     "arm-none-eabi")
set(X86_TRIPLET     "x86_64-linux-gnu")

# determine which kit was selected by user in VSCode CMake Tools extension
# and load the appropriate toolchain settings (stm32.cmake or linux.cmake)
if(${CMAKE_C_COMPILER} MATCHES "(${ARM_TRIPLET})+")
    include(cmake/stm32.cmake)
elseif(${CMAKE_C_COMPILER} MATCHES "(${X86_TRIPLET})+")   
    include(cmake/linux.cmake)
else()
    message(FATAL_ERROR "No suitable kit found. Aborting.")
endif()

# Declare the project 
project(STEP_SEQUENCER VERSION 0.1 DESCRIPTION "STEP_SEQUENCER")

# set the executable target
add_executable(${BUILD_NAME} "")

# add common sources
add_subdirectory(source/segger_rtt)
add_subdirectory(source/cpp_tlc5955)
add_subdirectory(source/cpp_ssd1306)

# add the stm32-specific sources for the embedded build
if(${CMAKE_C_COMPILER} MATCHES "(${ARM_TRIPLET})+") 

    add_subdirectory(stm32cube_workspace)
    add_subdirectory(source/main_app)

    # add the x86-specific sources for host build
elseif(${CMAKE_C_COMPILER} MATCHES "(${X86_TRIPLET})+")   

    # setup catch2 testing framework
    find_package(Catch2 3 REQUIRED)
    # add the test sources from these dirs
    add_subdirectory(source/tests)
    add_subdirectory(source/cpp_tlc5955/tests)
    add_subdirectory(source/cpp_ssd1306/tests)
    # link catch2 into the x86 build
    target_link_libraries(${BUILD_NAME} PRIVATE Catch2::Catch2WithMain)

endif()

# display size info
add_custom_target(size ALL ${CMAKE_SIZE} ${BUILD_NAME} DEPENDS ${BUILD_NAME})

# objcopy the elf file as a hex file, if using STM32
if(DEFINED ${HEX_NAME})
    add_custom_target(build.bin ALL DEPENDS ${BUILD_NAME} COMMAND ${CMAKE_OBJCOPY} -O ihex ${BUILD_NAME} ${HEX_NAME})
endif()
