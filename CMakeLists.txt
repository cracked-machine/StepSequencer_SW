cmake_minimum_required(VERSION 3.13)

# recommended compiler: gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2
# https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads

set(TARGET          STM32G0B1KETxN)
set(ARCH			armv6-m)
set(CORE			cortex-m0plus)
set(ARM_ASM            mthumb)
set(LINKER_SCRIPT   ${CMAKE_SOURCE_DIR}/STM32G0B1KETXN_FLASH.ld)
set(TARGET_ELF     build.elf)
set(TARGET_HEX     build.hex)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(MSG_LEN 0)
set(DEBUG   "-g3")
set(OPTIMIZATION "-O0")



set(COMMON_FLAGS "-march=${ARCH} -mcpu=${CORE} -${ARM_ASM} ${OPTIMIZATION} ${DEBUG} ${ADDITIONAL_FLAGS} -mfloat-abi=soft -pedantic -Wall -Wextra -Wfloat-equal -Wshadow -Wall -Wl,--gc-sections -fmessage-length=${MSG_LEN} -ffunction-sections -fdata-sections -ffreestanding -fno-builtin")
set(CPP_FLAGS "-fno-rtti -fno-exceptions -fno-use-cxa-atexit -fno-threadsafe-statics -ftemplate-backtrace-limit=0")
set(C_FLAGS "-ffunction-sections -fdata-sections -g3 -fstack-usage -Wno-unused-parameter -Wno-pedantic --specs=nano.specs")
set(CMAKE_EXE_LINKER_FLAGS  "-mthumb -mcpu=cortex-m0 -specs=nosys.specs -static -Wl,-Map=build.map -Wl,--gc-sections -Wl,--defsym=malloc_getpagesize_P=0x80 -Wl,--start-group -lc -lm -Wl,--end-group -T${LINKER_SCRIPT}" CACHE INTERNAL "exe link flags")


set(CMAKE_ASM_FLAGS	        "${COMMON_FLAGS} -MP -MD -x assembler-with-cpp" CACHE INTERNAL "asm compiler flags")
set(CMAKE_ASM_FLAGS	        "${COMMON_FLAGS}" CACHE INTERNAL "asm compiler flags")
set(CMAKE_C_FLAGS           "${COMMON_FLAGS} ${C_FLAGS}" CACHE INTERNAL "c compiler flags")
set(CMAKE_CXX_FLAGS	        "${COMMON_FLAGS} ${CPP_FLAGS}" CACHE INTERNAL "cpp compiler flags")


project(STEP_SEQUENCER VERSION 0.1 DESCRIPTION "STEP_SEQUENCER")
enable_language(C)
enable_language(CXX)
enable_language(ASM)
set(TYPE EXECUTABLE)
set(CMAKE_SYSTEM_NAME		Generic)
set(CMAKE_SYSTEM_VERSION	1)
set(CMAKE_SYSTEM_PROCESSOR	arm-eabi)
set(CMAKE_C_COMPILER		arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER		arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER		arm-none-eabi-gcc)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
endif()

add_compile_definitions(${TARGET} USE_HAL_DRIVER)
add_compile_definitions(${TARGET} STM32G0B1xx)
add_compile_definitions(${TARGET} SSD1306_USE_SPI)
add_compile_definitions(${TARGET} DEBUG)

include_directories(
    "source/cube_gen_src/Inc"
    "source/hal_drivers/STM32G0xx_HAL_Driver/Inc"
    "source/hal_drivers/STM32G0xx_HAL_Driver/Inc/Legacy"
    "source/hal_drivers/CMSIS/Device/ST/STM32G0xx/Include"
    "source/hal_drivers/CMSIS/Include"
    "source/main_app"
    "source/cpp_tlc5955/"
    "source/cpp_ssd1306/"
    "source/segger_rtt"
)

file(GLOB_RECURSE 
	 MAIN_SOURCE
     "source/cube_gen_src/Src/*.c"
     "source/hal_drivers/STM32G0xx_HAL_Driver/Src/*.c"
     "source/main_app/*.cpp"
     "source/cpp_ssd1306/*.cpp"
     "source/cpp_tlc5955/*.cpp"
     "source/cube_gen_src/Startup/startup_stm32g0b1ketxn.s"
     "source/segger_rtt/*.c"
)

# build the elf binary
add_executable(${TARGET_ELF} ${MAIN_SOURCE})

# display size info
add_custom_target(size ALL arm-none-eabi-size ${TARGET_ELF} DEPENDS ${TARGET_ELF})

# objcopy the elf file as a hex file
add_custom_target(build.bin ALL DEPENDS ${TARGET_ELF} COMMAND arm-none-eabi-objcopy -O ihex ${TARGET_ELF} ${TARGET_HEX})
